/*!
 * @brief	シンプルなモデルシェーダー。
 */

////////////////////////////////////////////////
// 構造体
////////////////////////////////////////////////
//ピクセルシェーダーへの入力。
struct SPSIn{
	float4 pos 			: SV_POSITION;	//スクリーン空間でのピクセルの座標。
	float3 normal 		: NORMAL;		//法線。
	float3 tangent		: TANGENT;		//接ベクトル。
	float3 biNormal 	: BINORMAL;		//従ベクトル。
	float2 uv 			: TEXCOORD0;	//uv座標。
	float3 worldPos		: TEXCOORD1;	//ワールド座標。
	float3 normalInView : TEXCOORD2;	//カメラ空間の法線。
	float4 posInLVP		: TEXCOORD3;	//ライトビュースクリーン空間でのピクセルの座標。
	float4 posInProj 	: TEXCOORD4; 	//頂点の正規化スクリーン座標。
};

////////////////////////////////////////////////
// 頂点シェーダーの共通処理。
////////////////////////////////////////////////
#include "ModelVSCommon.h"

////////////////////////////////////////////////
// 定数、グローバル変数。
////////////////////////////////////////////////
#include "PBRLighting_const.h"

////////////////////////////////////////////////
// ライト用定数バッファの構造体。
////////////////////////////////////////////////
#include "PBRLighting_struct.h"

////////////////////////////////////////////////
// PBRライティング。
////////////////////////////////////////////////
#include "Lighting_function.h"

////////////////////////////////////////////////
// シャドウイング。
////////////////////////////////////////////////
#include "Shadowing.h"

////////////////////////////////////////////////
// 定数バッファ。
////////////////////////////////////////////////
//ライト用の定数バッファ。
cbuffer LightCB : register(b1) {

	DirectionLig dirLig;		//ディレクションライト用の構造体。
	float3 eyePos;				//視点の位置。
	PointLig ptLig[4];			//ポイントライト用の構造体。
	SpotLig spLig[4];			//スポットライト用の構造体。
	ShadowCB shadow;			//シャドウ用の構造体。
	float4x4 mViewProjInv;      //ビュープロジェクション行列の逆行列。
	int ptNum;					//ポイントライトの数。
	int spNum;					//スポットライトの数。
}

///////////////////////////////////////////////////
// 関数
//////////////////////////////////////////////////
float3 CalcPointLight(float3 worldPos, float3 toEye, float3 normal, float metallic);
float3 CalcSpotLight(float3 worldPos, float3 toEye, float3 normal, float metallic);

/// <summary>
/// 法線を計算。
/// </summary>
float3 CalcNormal(float3 normal, float2 uv, float3 tangent, float3 biNormal)
{
	//法線マップからタンジェントスペースの法線をサンプリングする。
	float3 localNormal = g_normalMap.Sample(g_sampler, uv).xyz;

	//タンジェントスペースの法線を0～1の範囲から-1～1の範囲に復元する。
	localNormal = (localNormal * 2.0f) - 1.0f;

	//タンジェントスペースの法線をワールドスペースの法線に変換する。
	float3 newNormal = tangent * localNormal.x
					+ biNormal * localNormal.y
					+ normal * localNormal.z;

	return newNormal;
}

/// <summary>
/// スペキュラを計算。
/// </summary>
float CalcSpecular(float2 uv)
{
	float specPower = g_metallicSmoothMap.Sample(g_sampler, uv).r;

	return specPower * 2.0f;
}

/// <summary>
/// 頂点シェーダーのコア関数。
/// </summary>
SPSIn VSMainCore(SVSIn vsIn, float4x4 mWorldLocal)
{
	SPSIn psIn;
	psIn.pos = mul(mWorldLocal, vsIn.pos);
	psIn.pos = mul(mView, psIn.pos);
	psIn.pos = mul(mProj, psIn.pos);

	//法線を求める。
	psIn.normal = normalize(mul(mWorldLocal, vsIn.normal));
	psIn.tangent = normalize(mul(mWorldLocal, vsIn.tangent));
	psIn.biNormal = normalize(mul(mWorldLocal, vsIn.biNormal));

	//カメラ空間の法線を求める。
	psIn.normalInView = mul(mView, psIn.normal);

	//ワールド座標を求める。
	psIn.worldPos = mul(mWorldLocal, vsIn.pos);

	//ライトビュースクリーン空間の座標を求める。
	float4 worldPos = mul(mWorldLocal, vsIn.pos);
	psIn.posInLVP = mul(shadow.mLVP, worldPos);
	//頂点のライトから見た深度値を求める。
	psIn.posInLVP.z = length(worldPos.xyz - shadow.lightPos) / 1000.0f;

	//頂点の正規化スクリーン座標系の座標をピクセルシェーダーに渡す。
	psIn.posInProj = psIn.pos;

	//uv座標の設定。
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
/// ピクセルシェーダーのエントリー関数。
/// </summary>
float4 PSMainCore( SPSIn psIn, uniform bool hasShadow)
{
	//各種マップをサンプリング。
	//アルベドカラー。
	float4 albedoColor = g_albedoMap.Sample(g_sampler, psIn.uv);
	//法線を計算。
	float3 normal = CalcNormal(psIn.normal, psIn.uv, psIn.tangent, psIn.biNormal);
	//スペキュラ。
	float3 specularColor = albedoColor.xyz;
	//メタリック。
	float metallic = g_metallicSmoothMap.Sample(g_sampler, psIn.uv).r;
	//スムース。
	float smooth = g_metallicSmoothMap.Sample(g_sampler, psIn.uv).a;

	//視線に向かって伸びるベクトルを計算。
	float3 toEye = normalize(eyePos - psIn.worldPos);

	//ディレクションライトを計算。
	float3 directionLight = CalcDirectionLight(
		toEye,
		normal,
		dirLig.dirDirection,
		dirLig.dirColor,
		specularColor,
		metallic,
		smooth
	);

	//ポイントライトを計算。
	//float3 pointLight = CalcPointLight(psIn.worldPos, toEye, normal, metallic);

	//スポットライトを計算。
	//float3 spotLight = CalcSpotLight(psIn.worldPos, toEye, normal, metallic);

	//リムライトを計算。
	float3 limPower = CalcLimLight(normal, psIn.normalInView.z, dirLig.dirDirection, dirLig.dirColor);

	float3 lig = directionLight
				+ dirLig.ambient
//				+ pointLight
//				+ spotLight
				+ limPower;

	float shadowParam = 0.0f;
	if(hasShadow){
		shadowParam = ShadowMap(g_shadowMap, psIn.posInLVP);
	}
	albedoColor.xyz *= lig * (1.0f - shadowParam);


	return albedoColor;
}

/// <summary>
/// シャドウ無しのエントリー関数。
/// </summary>
float4 PSMain( SPSIn psIn ) : SV_Target0
{
	return PSMainCore(psIn, false);
}

/// <summary>
/// シャドウ有りのエントリー関数。
/// </summary>
float4 PSMainShadow( SPSIn psIn ) : SV_Target0
{
	return PSMainCore(psIn, true);
}

/// <summary>
/// ポイントライトを計算。
/// </summary>
float3 CalcPointLight(float3 worldPos, float3 toEye, float3 normal, float metallic)
{
	float3 finalPtLig = (0.0f, 0.0f, 0.0f);

	for(int i = 0; i < ptNum; i++) {

		//サーフェイスに入射するポイントライトの光の向きを計算する。
		float3 ligDir = worldPos - ptLig[i].ptPosition;

		//正規化する。
		ligDir = normalize(ligDir);

		//減衰なしのLambert拡散反射光を計算する。
		float3 diffPoint = CalcLambertDiffuse(
			ligDir,
			ptLig[i].ptColor,
			normal
		);

		//減衰なしのPhong鏡面反射校を計算する。
		float3 specPoint = CalcPhongSpecular(
			toEye,
			ligDir,
			ptLig[i].ptColor,
			normal,
			metallic
		);

		//距離による影響率を計算する。
		//ポイントライトとの距離を計算する。
		float distance = length(worldPos - ptLig[i].ptPosition);

		//影響率は距離に比例して小さくなっていく。
		float affect = 1.0f - (1.0f / ptLig[i].ptRange * distance);

		//影響力がマイナスなら当たらない。
		if(affect <= 0.0f){
			continue;
		}
		
		//影響を指数関数的にする。
		affect = pow(affect, 2.0f);

		//拡散反射光と鏡面反射校に減衰率を乗算して影響を弱める。
		diffPoint *= affect;
		specPoint *= affect;

		finalPtLig += diffPoint + specPoint;
	}

	return finalPtLig;
}

/// <summary>
/// スポットライトを計算。
/// </summary>
float3 CalcSpotLight(float3 worldPos, float3 toEye, float3 normal, float metallic)
{
	float3 finalSpLig = (0.0f, 0.0f, 0.0f);

	for(int i = 0; i < spNum; i++) {

		//ピクセルの座標 - スポットライトの座標を計算。
		float3 ligDir = worldPos - spLig[i].spPosition;

		//正規化して大きさ1のベクトルにする。
		ligDir = normalize(ligDir);

		//減衰無しのLambert拡散反射光を計算する。
		float3 diffSpot = CalcLambertDiffuse(
			ligDir,
			spLig[i].spColor,
			normal
		);

		//減衰無しのPhong鏡面反射光を計算する。
		float3 specSpot = CalcPhongSpecular(
			toEye,
			ligDir,
			spLig[i].spColor,
			normal,
			metallic
		);

		//スポットライトとの距離を計算する。
		float3 distance = length(worldPos - spLig[i].spPosition);

		//影響率は距離に比例して小さくなっていく。
		float affect = 1.0f - (1.0f / spLig[i].spRange * distance);

		//影響力がマイナスなら当たらない。
		if (affect <= 0.0f) {
			continue;
		}

		//影響の仕方を指数関数的にする。
		affect = pow(affect, 2.0f);

		//影響率を乗算して反射光を弱める。
		diffSpot *= affect;
		specSpot *= affect;

		//入射光と射出方向の角度を求める。
		float angle = dot(ligDir, spLig[i].spDirection);

		//-1.0f～1.0fの範囲内に。
		angle = min(max(angle, -1.0f), 1.0f);

		angle = abs(acos(angle));

		//角度による影響率を求める。
		affect = 1.0f - (1.0f / spLig[i].spAngle * angle);

		//影響率がマイナスにならないように補正をかける。
		affect = max(affect, 0.0f);

		//影響の仕方を指数関数的にする
		affect = pow(affect, 1.0f);

		//角度による影響率を反射光に乗算して、影響を弱める。
		diffSpot *= affect;
		specSpot *= affect;

		finalSpLig += diffSpot + specSpot;
	}

	return finalSpLig;
}
