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
    float distToEye     : TEXCOORD3;    //カメラから見た距離。
};

//ピクセルシェーダーからの出力。
struct SPSOut
{
    float4 albedo               : SV_TARGET0;   //アルベド。
    float4 normal               : SV_TARGET1;   //法線。
    float4 metallicShadowSmooth : SV_TARGET2;   //rにメタリック、gに影パラメータ、aにスムース。
    float4 param                : SV_Target3;   //rにシャドウ、gにライト。
};

////////////////////////////////////////////////
// 頂点シェーダーの共通処理。
////////////////////////////////////////////////
#include "../ModelVSCommon.h"

////////////////////////////////////////////////
// シェーダーの関数。
////////////////////////////////////////////////
#include "../Shader_function.h"

////////////////////////////////////////////////
// ライトの構造体。
////////////////////////////////////////////////
#include "../PBRLighting_struct.h"

///////////////////////////////////////
// グローバル変数。
///////////////////////////////////////
Texture2D<float4>   g_albedo                : register(t0);     //アルベドマップ。
Texture2D<float4>   g_normal                : register(t1);     //法線マップ。
Texture2D<float4>   g_metallicSmooth        : register(t2);     //メタリックスムースマップ。
Texture2D<float4>   g_param                 : register(t3);     //パラメータ。
Texture2D<float4>   g_depthTexture          : register(t10);    //深度値。
sampler             g_sampler               : register(s0);		//サンプラー。

///////////////////////////////////////
// 関数
///////////////////////////////////////
/// <summary>
/// 法線を計算。
/// </summary>
float3 CalcNormal(
    float3 normal,
    float3 tangent,
    float3 biNormal,
    float2 uv
) {
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
    psIn.posInProj.xy /= psIn.posInProj.w;

    //カメラからの距離を計算する。
    float3 objPosInCamera = mul(mWorldLocal, mWorldLocal[3]);
    psIn.distToEye = length(objPosInCamera - eyePos[0]);

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
SPSIn VSSkinMain(SVSIn vsIn) 
{
	return VSMainCore(vsIn, CalcSkinMatrix(vsIn.skinVert));
}

/// <summary>
/// モデル用ピクセルシェーダーのエントリーポイント。
/// </summary>
SPSOut PSMainCore(SPSIn psIn, SPSOut psOut, int hasShadow)
{
    //アルベドカラーと深度値を出力。
    psOut.albedo = g_albedo.Sample(g_sampler, psIn.uv);
    psOut.albedo.w = psIn.pos.z;

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
    psOut.param.r = min(255.0f, 255.0f * hasShadow);
    //ライティングパラメータ。
    psOut.param.g = 255.0f;
    //マスクパラメータ。
    psOut.param.b = 0.0f;

    return psOut;
}

/// <summary>
/// シャドウ無しのエントリー関数。
/// </summary>
SPSOut PSMain( SPSIn psIn )
{
    //共通処理。
    SPSOut psOut;
    psOut = PSMainCore(psIn, psOut, 0);

    return psOut;
}

/// <summary>
/// シャドウ有りのエントリー関数。
/// </summary>
SPSOut PSMainShadow( SPSIn psIn )
{
    //共通処理。
    SPSOut psOut;
    psOut = PSMainCore(psIn, psOut, 1);

    return psOut;
}

/// <summary>
/// 輪郭線有りのエントリー関数。
/// </summary>
SPSOut PSMainOutline( SPSIn psIn )
{
    //共通処理。
    SPSOut psOut;
    psOut = PSMainCore(psIn, psOut, 1);
    
    //輪郭線。
    psOut.albedo = Outline(psIn.posInProj, g_depthTexture, g_sampler, psOut.albedo);

	return psOut;
}

/// <summary>
/// ディザリング有りのエントリー関数。
/// </summary>
SPSOut PSMainDithering( SPSIn psIn )
{
    //共通処理。
    SPSOut psOut;
    psOut = PSMainCore(psIn, psOut, 1);
    
    //ディザリング。
    Dithering(psIn.pos.xy, psIn.distToEye);

	return psOut;
}

/// <summary>
/// ポータルフレーム用のエントリー関数。
/// </summary>
SPSOut PSPortalFrameCore(SPSIn psIn, uniform float maskParam)
{
    SPSOut psOut;

    //アルベドカラーをサンプリング。
	float4 albedoColor = g_albedo.Sample(g_sampler, psIn.uv);
    clip(albedoColor.a - 0.001f);

    psOut.albedo = Outline(psIn.posInProj, g_depthTexture, g_sampler, psOut.albedo);

    //ライティングパラメータ。
    psOut.param.g = 0.0f;
    //マスクパラメータ。
    psOut.param.b = maskParam;

	return psOut;
}

SPSOut PSBluePortal(SPSIn psIn)
{
    return PSPortalFrameCore(psIn, 0.1f);
}

SPSOut PSRedPortal(SPSIn psIn)
{
    return PSPortalFrameCore(psIn, 0.2f);
}
