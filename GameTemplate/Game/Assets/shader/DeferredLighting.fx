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
/// <param name="worldPos">ワールド座標</param>
/// <param name="toEye">視点の位置</param>
/// <param name="normal">法線</param>
/// <param name="specCOlor">スペキュラカラー</param>
/// <param name="metallic">金属度</param>
/// <param name="smooth">スムース</param>
float3 CalcPointLight(
    float3 worldPos,
    float3 toEye,
    float3 normal,
    float3 specColor,
    float metallic,
    float smooth
)
{
	float3 finalPtLig = 0.0f;

	for(int i = 0; i < ptNum; i++) {
		//サーフェイスに入射するポイントライトの正規化ベクトルを計算する。
		float3 ligDir = normalize(worldPos - ptLig[i].ptPosition);

        //ライトを計算。
        float3 lig = CalcDirectionLight(
            toEye,
            normal,
            ligDir,
            ptLig[i].ptColor,
            specColor,
            metallic,
            smooth
        );

		//距離による影響率を計算する。
		//ポイントライトとの距離を計算する。
		float distance = length(worldPos - ptLig[i].ptPosition);
		//影響率は距離に比例して小さくなっていく。
		float affect = 1.0f - min(1.0f, distance / ptLig[i].ptRange);
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
/// <param name="worldPos">ワールド座標</param>
/// <param name="toEye">視点の位置</param>
/// <param name="normal">法線</param>
/// <param name="specCOlor">スペキュラカラー</param>
/// <param name="metallic">金属度</param>
/// <param name="smooth">スムース</param>
float3 CalcSpotLight(
    float3 worldPos,
    float3 toEye,
    float3 normal,
    float3 specColor,
    float metallic,
    float smooth
)
{
	float3 finalSpLig = 0.0f;

	for(int i = 0; i < spNum; i++) {
		//サーフェイスに入射するスポットライトの正規化ベクトルを計算する。
		float3 ligDir = normalize(worldPos - spLig[i].spPosition);

        //ライトを計算。
        float3 lig = CalcDirectionLight(
            toEye,
            normal,
            ligDir,
            spLig[i].spColor,
            specColor,
            metallic,
            smooth
        );

		//距離による影響率を計算する。
		//スポットライトとの距離を計算する。
		float distance = length(worldPos - spLig[i].spPosition);
		//影響率は距離に比例して小さくなっていく。
		float affect = 1.0f - min(1.0f, distance / spLig[i].spRange);
		//影響の仕方を指数関数的にする。
		affect = pow(affect, 2.0f);
		//影響率を乗算して反射光を弱める。
		lig *= affect;

		//入射光と射出方向の角度を求める。
		float angle = dot(ligDir, spLig[i].spDirection);
		angle = abs(acos(angle));
		//角度による影響率を求める。
		affect = max(0.0f, 1.0f - 1.0f / spLig[i].spAngle * angle);
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
    //法線。
    float4 normal = g_normalTexture.Sample(g_sampler, psIn.uv);
    //カメラ空間の法線。
    float normalInViewZ = normal.a;
    //メタリックシャドウスムース。
    float4 metallicShadowSmooth = g_metallicShadowSmoothTexture.Sample(g_sampler, psIn.uv);
    //メタリック。
    float metallic = metallicShadowSmooth.r;
    //影生成用のパラメータ。
    float shadowParam = metallicShadowSmooth.g;
    //ライティングパラメータ。
    float lightingParam = metallicShadowSmooth.b;
    //スムース。
    float smooth = metallicShadowSmooth.a;

    //ライティングなし。
    if(lightingParam <= 0.0f) {
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
        worldPos,
        toEye,
        normal.xyz,
        specColor,
        metallic,
        smooth
    );

    //スポットライトを計算。
    lig += CalcSpotLight(
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