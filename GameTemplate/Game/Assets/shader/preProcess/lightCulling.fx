#include "../PBRLighting_struct.h"

//カメラ情報の定数バッファ。
cbuffer cbCameraParam : register(b0)
{
    float4x4 mtxProj        : packoffset(c0);   //投影行列。
    float4x4 mtxProjInv     : packoffset(c4);   //投影行列の逆行列。
    float4x4 mtxViewRot     : packoffset(c8);   //回転行列。
    float4   screenParam    : packoffset(c12);  //スクリーンパラメータ。
}

//深度テクスチャ。
Texture2D depthTexture : register(t0);
//出力用バッファー。
RWStructuredBuffer<uint> rwPointLightIndices : register(u0);
RWStructuredBuffer<uint> rwSpotLightIndices : register(u1);

//共有メモリ。
groupshared uint sMinZ;                                     //タイルの最小深度。
groupshared uint sMaxZ;                                     //タイルの最大深度。
groupshared uint sTilePointLightIndices[MAX_POINT_LIGHT];   //タイルに接触しているポイントライトのインデックス。
groupshared uint sTilePointLightNum;                        //タイルに接触しているポイントライトの数。
groupshared uint sTileSpotLightIndices[MAX_SPOT_LIGHT];     //タイルに接触しているスポットライトのインデックス。
groupshared uint sTileSpotLightNum;                         //タイルに接触しているスポットライトの数。
groupshared uint ligNum = 0;


/// <summary>
/// タイルごとの視錐台平面を求める。
/// </summary>
void GetTileFrustumPlane(out float4 frustumPlanes[6], uint3 groupId)
{
    //タイルの最大、最小深度を浮動小数点に変換。
    float minTileZ = asfloat(sMinZ);
    float maxTileZ = asfloat(sMaxZ);

    float2 tileScale = screenParam.zw * rcp(float(2 * TILE_WIDTH));
    float2 tileBias = tileScale - float2(groupId.xy);

    float4 c1 = float4(mtxProj._11 * tileScale.x, 0.0f, tileBias.x, 0.0f);
    float4 c2 = float4(0.0f, -mtxProj._22 * tileScale.y, tileBias.y, 0.0f);
    float4 c4 = float4(0.0f, 0.0f, 1.0f, 0.0f);

    frustumPlanes[0] = c4 - c1;
    frustumPlanes[1] = c4 + c1;
    frustumPlanes[2] = c4 - c2;
    frustumPlanes[3] = c4 + c2;
    frustumPlanes[4] = float4(0.0f, 0.0f, 1.0f, -minTileZ);
    frustumPlanes[5] = float4(0.0f, 0.0f, -1.0f, maxTileZ);

    //法線が正規化されていない4面を正規化する。
    [unroll]
    for(uint i = 0; i < 4; ++i)
    {
        frustumPlanes[i] *= rcp(length(frustumPlanes[i].xyz));
    }
}

/// <summary>
/// カメラ空間での座標を計算する。
/// </summary>
float3 ComputePositionInCamera(uint2 globalCoords)
{
    float2 st = ((float2)globalCoords + 0.5f) * rcp(screenParam.zw);
    st = st * float2(2.0f, -2.0f) - float2(1.0f, -1.0f);
    float3 screenPos;
    screenPos.xy = st.xy;
    screenPos.z = depthTexture.Load(uint3(globalCoords, 0.0f));
    float4 cameraPos = mul(mtxProjInv, float4(screenPos, 1.0f));

    return cameraPos.xyz / cameraPos.w;
}

/// <summary>
/// タイルに含まれているポイントライトのインデックスの配列を作成。
/// </summary>
/// <param name="threadNoInTile">タイル内のインデックス</param>
/// <param name="frustumPlanes">視錐台平面</param>
void CreatePointLightIndexArrayInTile(
    uint threadNoInTile,
    float4 frustumPlanes[6]
){
    for(
        uint lightIndex = threadNoInTile;
        lightIndex < MAX_POINT_LIGHT;
        lightIndex += TILE_SIZE
    ) {
        PointLig light = ptLig[lightIndex];

        bool inFrustum = true;
        for(uint i = 0; i < 6; ++i){
            //ライトの座標と平面の法線とで内積を求め、ライトと平面との距離を計算する。
            float4 lp = float4(light.posInView, 1.0f);
            float d = dot(frustumPlanes[i], lp);

            //ライトと平面の距離を使って衝突判定を行う。
            inFrustum = inFrustum && (d >= -light.ptRange);
        }

        if(inFrustum){
            //衝突したポイントライトの番号を影響リストに積んでいく。
            uint listIndex;
            InterlockedAdd(sTilePointLightNum, 1, listIndex);
            sTilePointLightIndices[listIndex] = lightIndex;
        }
    }
}

/// <summary>
/// タイルに含まれているスポットライトのインデックスの配列を作成。
/// </summary>
/// <param name="threadNoInTile">タイル内のインデックス</param>
/// <param name="frustumPlanes">視錐台平面</param>
void CreateSpotLightIndexArrayInTile(
    uint threadNoInTile,
    float4 frustumPlanes[6]
){
    for(
        uint lightIndex = threadNoInTile;
        lightIndex < MAX_SPOT_LIGHT;
        lightIndex += TILE_SIZE
    ) {
        SpotLig light = spLig[lightIndex];

        bool inFrustum = true;
        for(uint i = 0; i < 6; ++i){
            //ライトの座標と平面の法線とで内積を求め、ライトと平面との距離を計算。
            float4 lp = float4(light.posInView, 1.0f);
            float d = dot(frustumPlanes[i], lp);

            if(d < 0.0f){
                //スポットライトが視錐台の外にあるので、視錐台の方向を向いているか調べる。
                float t = dot(frustumPlanes[i].xyz, light.directionInView);
                if(t < 0.0f){
                    //タイルに当たらない。
                    inFrustum = false;
                }
            }
            //ライトと平面の距離を使い衝突判定を行う。
            inFrustum = inFrustum && (d >= -light.spRange);
        }   

        if(inFrustum){
            //衝突したスポットライトの番号を影響リストに積んでいく。
            uint listIndex;
            InterlockedAdd(sTileSpotLightNum, 1, listIndex);
            sTileSpotLightIndices[listIndex] = lightIndex;
        }
    }
}

/// <summary>
/// タイルに含まれているライトの番号を出力用のリストに書き込み。
/// </summary>
/// <param name="threadNoInTile">タイル内のインデックス</param>
/// <param name="frameUV">このスレッドが担当するピクセルのカメラ空間での座標</param>
void WriteLightIndexInTileNoList(uint threadNoInTile, uint2 frameUV)
{
    uint numCellX = (screenParam.z + TILE_WIDTH - 1) / TILE_WIDTH;
    uint tileIndex = floor(frameUV.x / TILE_WIDTH) + floor(frameUV.y / TILE_WIDTH) * numCellX;

    {
         //ポイントライトの番号を出力。
        uint lightStart = ptNum * tileIndex;
        for(
            uint lightIndex = threadNoInTile;
            lightIndex < sTilePointLightNum;
            lightIndex += TILE_SIZE
        ) {
            rwPointLightIndices[lightStart + lightIndex] = sTilePointLightIndices[lightIndex];
        }

        //最後に番兵を設定する。
        if(threadNoInTile == 0 && 
            sTilePointLightNum < ptNum
        ) {
            rwPointLightIndices[lightStart + sTilePointLightNum] = 0xffffffff;
        }   
    }

    {
        //スポットライトの番号を出力。
        uint lightStart = spNum * tileIndex;
        for(
            uint lightIndex = threadNoInTile;
            lightIndex < sTileSpotLightNum;
            lightIndex += TILE_SIZE
        ) {
            rwSpotLightIndices[lightStart + lightIndex] = sTileSpotLightIndices[lightIndex];
        }

        //最後に番兵を設定する。
        if(threadNoInTile == 0 && 
            sTileSpotLightNum < spNum
        ) {
            rwSpotLightIndices[lightStart + sTileSpotLightNum] = 0xffffffff;
        }
    }
}

[numthreads(TILE_WIDTH, TILE_HEIGHT, 1)]
void CSMain(
    uint3 groupId           : SV_GroupID,
    uint3 dispatchThreadId  : SV_DispatchThreadId,
    uint3 groupThreadId     : SV_GroupThreadID
) {

    //タイル内でのインデックスを求める。
    uint groupIndex = groupThreadId.y * TILE_WIDTH + groupThreadId.x;

    //共有メモリを初期化する。
    if(groupIndex == 0) {
        sTilePointLightNum = 0;
        sTileSpotLightNum = 0;
        sMinZ = 0x7F7FFFFF;
        sMaxZ = 0;
    }

    //このスレッドが担当するピクセルのカメラ空間での座標を計算する。
    uint2 frameUV = dispatchThreadId.xy;
    float3 posInView = ComputePositionInCamera(frameUV);

    //すべてのスレッドがここに到達するまで同期を取る。
    GroupMemoryBarrierWithGroupSync();

    //タイルの最大、最小深度を求める。
    InterlockedMin(sMinZ, asuint(posInView.z));
    InterlockedMax(sMaxZ, asuint(posInView.z));

    GroupMemoryBarrierWithGroupSync();

    //タイルの視錐台を構成する6つの平面を求める。
    float4 frustumPlanes[6];
    GetTileFrustumPlane(frustumPlanes, groupId);

    //タイルに含まれているポイントライトのインデックス配列を作成。
    CreatePointLightIndexArrayInTile(groupIndex, frustumPlanes);
    //タイルに含まれているスポットライトのインデックス配列を作成。
    CreateSpotLightIndexArrayInTile(groupIndex, frustumPlanes);

    GroupMemoryBarrierWithGroupSync();

    //ライトインデックスを出力バッファーに出力。
    WriteLightIndexInTileNoList(groupIndex, frameUV);
}