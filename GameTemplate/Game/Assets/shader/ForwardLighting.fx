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
};

////////////////////////////////////////////////
// 頂点シェーダーの共通処理。
////////////////////////////////////////////////
#include "ModelVSCommon.h"

////////////////////////////////////////////////
// レジスタ。
////////////////////////////////////////////////
#include "ForwardLighting_register.h"

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

///////////////////////////////////////////////////
// 関数
//////////////////////////////////////////////////
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
) {
	float3 finalPtLig = 0.0f;

	//このピクセルが含まれているタイルの番号を計算する。
	uint numCellX = (1600 + TILE_WIDTH - 1) / TILE_WIDTH;
	uint tileIndex = floor(pos.x / TILE_WIDTH) + floor(pos.y / TILE_HEIGHT) * numCellX;

	//含まれるタイツの影響の開始位置と終了位置を計算する。
	uint lightStart = tileIndex * ptNum;
	uint lightEnd = lightStart + ptNum;

	for(
		uint lightListIndex = lightStart;
		lightListIndex < lightEnd;
		lightListIndex++
	) {
		uint ligNo = pointLightListInTile[lightListIndex];

		if(ligNo == 0xffffffff){
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
) {
	float3 finalSpLig = 0.0f;

	//このピクセルが含まれているタイルの番号を計算する。
	uint numCellX = (1600 + TILE_WIDTH - 1) / TILE_WIDTH;
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

		//スポットライトとの距離を計算する。
		float3 distance = length(worldPos - spLig[ligNo].spPosition);
		//影響率は距離に比例して小さくなっていく。
		float affect = 1.0f - min(1.0f, distance / spLig[ligNo].spRange);
		//影響の仕方を指数関数的にする。
		affect = pow(affect, 2.0f);
		//影響率を乗算して反射光を弱める。
		lig *= affect;

		//入射光と射出方向の角度を求める。
		float angle = dot(ligDir, spLig[ligNo].spDirection);
		//-1.0f～1.0fの範囲内に。
		angle = abs(acos(angle));
		//角度による影響率を求める。
		affect = max(0.0f, 1.0f - 1.0f / spLig[ligNo].spAngle * angle);
		//影響の仕方を指数関数的にする
		affect = pow(affect, 2.0f);
		//角度による影響率を反射光に乗算して、影響を弱める。
		lig *= affect;

		finalSpLig += lig;
	}
	return finalSpLig;
}


/// <summary>
/// 頂点シェーダーのコア関数。
/// </summary>
SPSIn VSMainCore(SVSIn vsIn, float4x4 mWorldLocal)
{
	SPSIn psIn;
	psIn.pos = CalcVertexPositionInWorldSpace(vsIn.pos, mWorldLocal);
	psIn.pos = mul(mView, psIn.pos);
	psIn.pos = mul(mProj, psIn.pos);

	//法線を求める。
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
	psIn.normalInView = mul(mView, psIn.normal);

	//ライトビュースクリーン空間の座標を求める。
	float4 worldPos = mul(mWorldLocal, vsIn.pos);
	psIn.posInLVP = mul(shadow.mLVP, worldPos);
	//頂点のライトから見た深度値を求める。
	psIn.posInLVP.z = length(worldPos.xyz - shadow.lightPos) / 1000.0f;

	//ワールド座標を求める。
	psIn.worldPos = worldPos.xyz;

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
float4 PSMainCore(SPSIn psIn, uniform bool hasShadow)
{
	//各種マップをサンプリング。
	//アルベドカラー。
	float4 albedoColor = g_albedoMap.Sample(g_sampler, psIn.uv);
	//法線を計算。
	float3 normal = CalcNormal(psIn.normal, psIn.tangent, psIn.biNormal, psIn.uv);
	//スペキュラ。
	float3 specularColor = albedoColor.xyz;
	//メタリック。
	float metallic = g_metallicSmoothMap.Sample(g_sampler, psIn.uv).r;
	//スムース。
	float smooth = g_metallicSmoothMap.Sample(g_sampler, psIn.uv).a;

	//視線に向かって伸びるベクトルを計算。
	float3 toEye = normalize(eyePos[0] - psIn.worldPos);

	float shadowRate = 0.0f;
	if(hasShadow){
	 	shadowRate = ShadowMap(g_shadowMap, psIn.posInLVP);
	}

	//ディレクションライトを計算。
	float3 lig = CalcDirectionLight(
		toEye,
		normal,
		dirLig.dirDirection,
		dirLig.dirColor,
		specularColor,
		metallic,
		smooth
	) * (1.0f - shadowRate);

/*
	//ポイントライトを計算。
	lig += CalcPointLight(
		psIn.pos.xyz,
		psIn.worldPos,
		toEye,
		normal,
		specularColor,
		metallic,
		smooth
	);

	//スポットライトを計算。
	lig += CalcSpotLight(
		psIn.pos.xyz,
		psIn.worldPos,
		toEye,
		normal,
		specularColor,
		metallic,
		smooth
	);

	*/

	//リムライトを計算。
	lig += CalcLimLight(
		normal, 
		psIn.normalInView.z,
		dirLig.dirDirection,
		dirLig.dirColor
	);

	lig += dirLig.ambient * albedoColor.xyz;

	float4 finalColor;
	finalColor.xyz = lig;
	finalColor.w = albedoColor.w;

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