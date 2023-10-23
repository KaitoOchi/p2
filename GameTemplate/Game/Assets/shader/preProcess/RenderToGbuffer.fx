
///////////////////////////////////////
// 構造体。
///////////////////////////////////////
//ピクセルシェーダーへの入力。
struct SPSIn
{
    float4 pos          : SV_POSITION;  //座標。
    float3 normal       : NORMAL;       //法線。
    float3 tangent      : TANGENT;      //接ベクトル。
    float3 biNormal     : BINORMAL;     //従ベクトル。
    float2 uv           : TEXCOORD0;    //UV座標。
    float normalInViewZ : TEXCOORD1;    //カメラ空間の法線Z。
    float4 posInProj    : TEXCOORD2;    //頂点の正規化スクリーン座標系。
};

//ピクセルシェーダーからの出力。
struct SPSOut
{
    float4 albedo               : SV_TARGET0;   //アルベド。
    float4 normal               : SV_TARGET1;   //法線。
    float4 metallicShadowSmooth : SV_TARGET2;   //rにメタリック、gに影パラメータ、aにスムース。
};

////////////////////////////////////////////////
// 頂点シェーダーの共通処理。
////////////////////////////////////////////////
#include "../ModelVSCommon.h"

////////////////////////////////////////////////
// シェーダーの関数。
////////////////////////////////////////////////
#include "../Shader_function.h"

///////////////////////////////////////
// グローバル変数。
///////////////////////////////////////
Texture2D<float4>   g_albedo            : register(t0);     //アルベドマップ。
Texture2D<float4>   g_normal            : register(t1);     //法線マップ。
Texture2D<float4>   g_metallicSmooth    : register(t2);     //メタリックスムースマップ。
Texture2D<float4>   g_portalRenderTexture : register(t10);  //ポータルフレーム。
Texture2D<float4>   g_depthTexture      : register(t11);    //深度値。
sampler             g_sampler           : register(s0);     //サンプラー。

// 近傍8テクセルへのUVオフセット
const float2 uvOffset[8] = {
    float2(           0.0f,  1.0f / 900.0f), //上
    float2(           0.0f, -1.0f / 900.0f), //下
    float2( 1.0f / 1600.0f,           0.0f), //右
    float2(-1.0f / 1600.0f,           0.0f), //左
    float2( 1.0f / 1600.0f,  1.0f / 900.0f), //右上
    float2(-1.0f / 1600.0f,  1.0f / 900.0f), //左上
    float2( 1.0f / 1600.0f, -1.0f / 900.0f), //右下
    float2(-1.0f / 1600.0f, -1.0f / 900.0f)  //左下
};

///////////////////////////////////////
// 関数
///////////////////////////////////////
/// <summary>
/// 法線を計算。
/// </summary>
float3 CalcNormal(float3 normal, float3 tangent, float3 biNormal, float2 uv)
{
	//法線マップからタンジェントスペースの法線をサンプリングする。
	float3 localNormal = g_normal.Sample(g_sampler, uv).xyz;

	//タンジェントスペースの法線を0～1の範囲から-1～1の範囲に復元する。
	localNormal = (localNormal * 2.0f) - 1.0f;

	//タンジェントスペースの法線をワールドスペースの法線に変換する。
	float3 newNormal = tangent * localNormal.x
					+ biNormal * localNormal.y
					+ normal * localNormal.z;

	return newNormal;
}

/// <summary>
/// 輪郭線を計算。
/// </summary>
float Outline(float3 pos, float4 posInProj)
{
    // 近傍8テクセルの深度値を計算して、エッジを抽出する
    // 正規化スクリーン座標系からUV座標系に変換する
    float2 uv = (posInProj.xy / posInProj.w) * float2( 0.5f, -0.5f) + 0.5f;

    // このピクセルの深度値を取得
    float depth = g_depthTexture.Sample(g_sampler, uv).x;

    // 近傍8テクセルの深度値の平均値を計算する
    float depth2 = 0.0f;
    for( int i = 0; i < 8; i++)
    {
        depth2 += g_depthTexture.Sample(g_sampler, uv + uvOffset[i]).x;
    }
    depth2 /= 8.0f;

    // 自身の深度値と近傍8テクセルの深度値の差を調べる
    if(abs(depth - depth2) > 0.01f)
    {
        // 深度値が結構違う場合はピクセルカラーを黒にする
        return 0.0f;
    }

    // 普通のテクスチャ
    return 1.0f ;
}

//モデル用頂点シェーダーのエントリーポイント。
SPSIn VSMainCore(SVSIn vsIn, float4x4 mWorldLocal)
{
    SPSIn psIn;
    psIn.pos = CalcVertexPositionInWorldSpace(vsIn.pos, mWorldLocal);
    psIn.pos = mul(mView, psIn.pos);
    psIn.pos = mul(mProj, psIn.pos);

    //ワールド座標の法線、接ベクトル、従ベクトルを計算する。
    CalcVertexNormalTangentBiNormalInWorldSpace(
        psIn.normal,
        psIn.tangent,
        psIn.biNormal,
        mWorldLocal,
        vsIn.normal,
        vsIn.tangent,
        vsIn.biNormal
    );

	//カメラ空間の法線を求める。
	float4 normalInView = mul(mView, psIn.normal);
    psIn.normalInViewZ = normalInView.z;

    //頂点の正規化スクリーン座標系の座標をピクセルシェーダーにわたす
	psIn.posInProj = psIn.pos;

    psIn.uv = vsIn.uv;

    return psIn;
}

/// <summary>
/// スキンなしメッシュ用の頂点シェーダーのエントリー関数。
/// </summary>
SPSIn VSMain(SVSIn vsIn)
{
	return VSMainCore(vsIn, mWorld);
}

/// <summary>
/// スキンありメッシュの頂点シェーダーのエントリー関数。
/// </summary>
SPSIn VSSkinMain( SVSIn vsIn ) 
{
	return VSMainCore(vsIn, CalcSkinMatrix(vsIn.skinVert));
}

/// <summary>
/// モデル用ピクセルシェーダーのエントリーポイント。
/// </summary>
SPSOut PSMainCore(SPSIn psIn, int hasShadow)
{
    //GBufferに出力。
    SPSOut psOut;

    //アルベドカラーと深度値を出力。
    psOut.albedo = g_albedo.Sample(g_sampler, psIn.uv);
    psOut.albedo.w = psIn.pos.z;

    if(hasShadow == 2) {
        psOut.albedo = psOut.albedo * Outline(psIn.pos.xyz, psIn.posInProj);
    }
    else if(hasShadow == 3) {
        Dithering(psIn.pos.xyz);
    }

    //法線を出力。
    psOut.normal.xyz = CalcNormal(
        psIn.normal,
        psIn.tangent,
        psIn.biNormal,
        psIn.uv
    );
    //カメラ空間の法線を出力。
    psOut.normal.w = psIn.normalInViewZ;
    //メタリックスムースを出力。
    psOut.metallicShadowSmooth = g_metallicSmooth.Sample(g_sampler, psIn.uv);
    //影パラメータ。
    psOut.metallicShadowSmooth.g = min(255.0f, 255.0f * hasShadow);
    //ライティングパラメータ。
    psOut.metallicShadowSmooth.b = 255.0f;

    return psOut;
}

/// <summary>
/// シャドウ無しのエントリー関数。
/// </summary>
SPSOut PSMain( SPSIn psIn )
{
	return PSMainCore(psIn, 0);
}

/// <summary>
/// シャドウ有りのエントリー関数。
/// </summary>
SPSOut PSMainShadow( SPSIn psIn )
{
	return PSMainCore(psIn, 1);
}

/// <summary>
/// 輪郭線有りのエントリー関数。
/// </summary>
SPSOut PSMainOutline( SPSIn psIn )
{
	return PSMainCore(psIn, 2);
}

/// <summary>
/// ディザリング有りのエントリー関数。
/// </summary>
SPSOut PSMainDithering( SPSIn psIn )
{
	return PSMainCore(psIn, 3);
}

/// <summary>
/// ポータルフレーム用のエントリー関数。
/// </summary>
SPSOut PSMainPortalFrame(SPSIn psIn)
{
    SPSOut psOut;

    //アルベドカラーをサンプリング。
	float4 albedoColor = g_albedo.Sample(g_sampler, psIn.uv);

    clip(albedoColor.a - 0.001f);

	psOut.albedo = g_portalRenderTexture.Sample(g_sampler,psIn.uv);
    psOut.albedo *= Outline(psIn.pos.xyz, psIn.posInProj);

    //ライティングパラメータ。
    psOut.metallicShadowSmooth.b = 0.0f;

	return psOut;
}