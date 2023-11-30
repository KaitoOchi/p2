///////////////////////////////////////
// ディファードライティング。
///////////////////////////////////////
static const float PI = 3.1415926f;         // 円周率。

///////////////////////////////////////
// 構造体。
///////////////////////////////////////
//頂点シェーダーへの入力。
struct VSInput
{
    float4 pos  : POSITION;     //座標。
    float2 uv   : TEXCOORD0;    //UV座標。
};

//ピクセルシェーダーへの入力。
struct PSInput
{
    float4 pos  : SV_POSITION;     //座標。
    float2 uv   : TEXCOORD0;    //UV座標。
};

///////////////////////////////////////
// モデルの頂点シェーダー関係の共通ヘッダー。
///////////////////////////////////////
#include "ModelVSCommon.h"

////////////////////////////////////////////////
// レジスタ。
////////////////////////////////////////////////
#include "DeferredLighting_register.h"

////////////////////////////////////////////////
// ライト用定数バッファの構造体。
////////////////////////////////////////////////
#include "PBRLighting_struct.h"

////////////////////////////////////////////////
// ライティング。
////////////////////////////////////////////////
#include "Lighting_function.h"

////////////////////////////////////////////////
// シャドウイング。
////////////////////////////////////////////////
#include "Shadowing.h"

///////////////////////////////////////
// 定数バッファ。
///////////////////////////////////////
//定数バッファ。
cbuffer cb : register(b0)
{
    float4x4 mvp; 
    float4 mulColor;
    float4 screenParam;
};


///////////////////////////////////////
// 関数
///////////////////////////////////////
/// <summary>
/// ポイントライトを計算。
/// </summary>
/// <param name="pos">座標</param>
/// <param name="worldPos">ワールド座標</param>
/// <param name="toEye">視点の位置</param>
/// <param name="normal">法線</param>
/// <param name="specCOlor">スペキュラカラー</param>
/// <param name="metallic">金属度</param>
/// <param name="smooth">スムース</param>
float3 CalcPointLight(
    float3 pos,
    float3 worldPos,
    float3 toEye,
    float3 normal,
    float3 specColor,
    float metallic,
    float smooth
)
{
    float3 finalPtLig = 0.0f;

    //このピクセルが含まれているタイルの番号を計算する。
    uint numCellX = (screenParam.z + TILE_WIDTH - 1) / TILE_WIDTH;
    uint tileIndex = floor(pos.x / TILE_WIDTH) + floor(pos.y / TILE_HEIGHT) * numCellX;

    //含まれるタイルの影響の開始位置と終了位置を計算する。
    uint lightStart = tileIndex * ptNum;
    uint lightEnd = lightStart + ptNum;

    for(
        uint lightListIndex = lightStart;
        lightListIndex < lightEnd;
        lightListIndex++
    ) {
        uint ligNo = pointLightListInTile[lightListIndex];

        if(ligNo == 0xffffffff) {
            break;
        }

        //サーフェイスに入射するポイントライトの正規化ベクトルを計算する。
		float3 ligDir = normalize(worldPos - ptLig[ligNo].ptPosition);

        //ライトを計算。
        float3 lig = CalcDirectionLight(
            toEye,
            normal,
            ligDir,
            ptLig[ligNo].ptColor,
            specColor,
            metallic,
            smooth
        );

		//距離による影響率を計算する。
		//ポイントライトとの距離を計算する。
		float distance = length(worldPos - ptLig[ligNo].ptPosition);
		//影響率は距離に比例して小さくなっていく。
		float affect = 1.0f - min(1.0f, distance / ptLig[ligNo].ptRange);
		//影響を指数関数的にする。
		affect = pow(affect, 2.0f);
		//減衰率を乗算して影響を弱める。
        lig *= affect;

        finalPtLig += lig;
    }

	return finalPtLig;
}

/// <summary>
/// スポットライトを計算。
/// </summary>
/// <param name="pos">座標</param>
/// <param name="worldPos">ワールド座標</param>
/// <param name="toEye">視点の位置</param>
/// <param name="normal">法線</param>
/// <param name="specCOlor">スペキュラカラー</param>
/// <param name="metallic">金属度</param>
/// <param name="smooth">スムース</param>
float3 CalcSpotLight(
    float3 pos,
    float3 worldPos,
    float3 toEye,
    float3 normal,
    float3 specColor,
    float metallic,
    float smooth
)
{
	float3 finalSpLig = 0.0f;

    //このピクセルが含まれているタイルの番号を計算する。
    uint numCellX = (screenParam.z + TILE_WIDTH - 1) / TILE_WIDTH;
    uint tileIndex = floor(pos.x / TILE_WIDTH) + floor(pos.y / TILE_HEIGHT) * numCellX;

    //含まれるタイルの影響の開始位置と終了位置を計算する。
    uint lightStart = tileIndex * spNum;
    uint lightEnd = lightStart + spNum;

    for(
        uint lightListIndex = lightStart;
        lightListIndex < lightEnd;
        lightListIndex++
    ) {
        uint ligNo = spotLightListInTile[lightListIndex];

        if(ligNo == 0xffffffff) {
            break;
        }
        
        //サーフェイスに入射するスポットライトの正規化ベクトルを計算する。
		float3 ligDir = normalize(worldPos - spLig[ligNo].spPosition);

        //ライトを計算。
        float3 lig = CalcDirectionLight(
            toEye,
            normal,
            ligDir,
            spLig[ligNo].spColor,
            specColor,
            metallic,
            smooth
        );

		//距離による影響率を計算する。
		//スポットライトとの距離を計算する。
		float distance = length(worldPos - spLig[ligNo].spPosition);
		//影響率は距離に比例して小さくなっていく。
		float affect = 1.0f - min(1.0f, distance / spLig[ligNo].spRange);
		//影響の仕方を指数関数的にする。
		affect = pow(affect, 2.0f);
		//影響率を乗算して反射光を弱める。
		lig *= affect;

		//入射光と射出方向の角度を求める。
		float angle = dot(ligDir, spLig[ligNo].spDirection);
		angle = abs(acos(angle));
		//角度による影響率を求める。
		affect = max(0.0f, 1.0f - 1.0f / spLig[ligNo].spAngle * angle);
		//影響の仕方を指数関数的にする。
		affect = pow(affect, 2.0f);
		//影響率を乗算して反射光を弱める。
		lig *= affect;

		finalSpLig += lig;
    }
	return finalSpLig;
}

//頂点シェーダー。
PSInput VSMain(VSInput vsIn)
{
    PSInput psIn;

    psIn.pos = mul(mvp, vsIn.pos);
    psIn.uv = vsIn.uv;

    return psIn;
}

//ピクセルシェーダー。
float4 PSMainCore(PSInput psIn)
{
    //GBufferの内容を使ってライティング。
    //アルベドカラー。
    float4 albedoColor = g_albedoTexture.Sample(g_sampler, psIn.uv);

    clip(albedoColor.a - 0.000001f);

    //法線。
    float4 normal = g_normalTexture.Sample(g_sampler, psIn.uv);
    //カメラ空間の法線。
    float normalInViewZ = normal.a;
    //メタリックシャドウスムース。
    float4 metallicShadowSmooth = g_metallicShadowSmoothTexture.Sample(g_sampler, psIn.uv);
    //メタリック。
    float metallic = metallicShadowSmooth.r;
    //スムース。
    float smooth = metallicShadowSmooth.a;
    //各種パラメータ。
    float4 textureParam = g_param.Sample(g_sampler, psIn.uv);
    //影生成用のパラメータ。
    float shadowParam = textureParam.r;
    //ライティングパラメータ。
    float lightingParam = textureParam.g;
    //マスクパラメータ。
    int maskParam = ceil(textureParam.b * 10);

    //ライティングなし。
    if(lightingParam <= 0.0f) {
        for(int i = 0; i < maskParam; i++)
        {
            albedoColor = g_portalRenderTexture[i].Sample(g_sampler, psIn.uv);
        }
        return albedoColor;
    }

    //スペキュラカラー。
    float3 specColor = albedoColor.xyz;
    //ワールド座標を計算する。
    float3 worldPos = CalcWorldPosFromUVZ(psIn.uv, albedoColor.w, mViewProjInv);
    //視点に向かって伸びるベクトルを計算する。
    float3 toEye = normalize(eyePos - worldPos);
    //ライトビュースクリーン空間を求める。
	float4 posInLVP = mul(shadow.mLVP, float4(worldPos, 1.0f));
    //シャドウの計算。
    float shadowRate =  ShadowMap(g_shadowMap, posInLVP) * shadowParam;

    //ディレクションライトを計算。
	float3 lig = CalcDirectionLight(
        toEye,
        normal.xyz,
        dirLig.dirDirection,
        dirLig.dirColor,
        specColor,
        metallic,
        smooth
    ) * (1.0f - shadowRate);

    //ポイントライトを計算。
    lig += CalcPointLight(
        psIn.pos.xyz,
        worldPos,
        toEye,
        normal.xyz,
        specColor,
        metallic,
        smooth
    );

    //スポットライトを計算。
    lig += CalcSpotLight(
        psIn.pos.xyz,
        worldPos,
        toEye,
        normal.xyz,
        specColor,
        metallic,
        smooth
    );

    //リムライトを計算。
    lig += CalcLimLight(
        normal.xyz,
        normalInViewZ,
        dirLig.dirDirection,
        dirLig.dirColor
    );

    //環境光でライトを底上げする。
    lig += dirLig.ambient * albedoColor.xyz;

    float4 finalColor = 1.0f;
    finalColor.xyz = lig;
    return finalColor;
}

/// <summary>
/// 通常のエントリー関数。
/// </summary>
float4 PSMain(PSInput psIn) : SV_Target0
{
    return PSMainCore(psIn);
}

/// <summary>
/// ライティング無しのエントリー関数。
/// </summary>
float4 PSMainNotLighting(PSInput psIn) : SV_Target0
{
    return g_albedoTexture.Sample(g_sampler, psIn.uv);
}