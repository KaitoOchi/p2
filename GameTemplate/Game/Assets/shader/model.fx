/*!
 * @brief	シンプルなモデルシェーダー。
 */

 struct DirectionLig
 {
	float3 dirDirection;	//ライトの方向。
	float3 dirColor;		//ライトのカラー。
	float ambient;			//環境光。
 };

 struct PointLig
 {
	float3 ptPosition;		//ライトの位置。
	float3 ptColor;			//ライトのカラー。
	float ptRange;			//ライトの影響範囲。
 };

 struct SpotLig
 {
	float3 spPosition;		//ライトの位置。
	float3 spColor;			//ライトのカラー。
	float spRange;			//ライトの影響範囲。
	float3 spDirection;		//ライトの方向。
	float spAngle;			//ライトの射出角度。
 };

 //シャドウ用の構造体
struct ShadowCB
{
	float3 lightPos;		//ライトの座標
	float4x4 mLVP;			//ライトビュープロジェクション行列
};


////////////////////////////////////////////////
// 定数バッファ。
////////////////////////////////////////////////
//モデル用の定数バッファ
cbuffer ModelCb : register(b0){
	float4x4 mWorld;
	float4x4 mView;
	float4x4 mProj;
};

cbuffer LightCB : register(b1) {

	DirectionLig dirLig;		//ディレクションライト用の構造体。
	float3 eyePos;				//視点の位置。
	PointLig ptLig[4];			//ポイントライト用の構造体。
	SpotLig spLig[4];			//スポットライト用の構造体。
	ShadowCB shadow;
	int ptNum;					//ポイントライトの数。
	int spNum;					//スポットライトの数。
}

////////////////////////////////////////////////
// 構造体
////////////////////////////////////////////////
//スキニング用の頂点データをひとまとめ。
struct SSkinVSIn{
	int4  Indices  	: BLENDINDICES0;
    float4 Weights  : BLENDWEIGHT0;
};
//頂点シェーダーへの入力。
struct SVSIn{
	float4 pos 		: POSITION;		//モデルの頂点座標。
	float3 normal 	: NORMAL;		//法線。
	float3 tangent	: TANGENT;		//接ベクトル。
	float3 biNormal : BINORMAL;		//従ベクトル。
	float2 uv 		: TEXCOORD0;	//UV座標。
	SSkinVSIn skinVert;				//スキン用のデータ。
};
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
// グローバル変数。
////////////////////////////////////////////////
Texture2D<float4> g_albedo : register(t0);				//アルベドマップ。
Texture2D<float4> g_normal : register(t1);				//ノーマルマップ。
Texture2D<float4> g_specular : register(t2);			//スペキュラマップ。
Texture2D<float4> g_shadowMap : register(t10);			//シャドウマップ。
StructuredBuffer<float4x4> g_boneMatrix : register(t3);	//ボーン行列。
sampler g_sampler : register(s0);	//サンプラステート。

float3 CalcNormal(float3 normal, float2 uv, float3 tangent, float3 biNormal);
float CalcSpecular(float2 uv);
float3 CalcDirectionLight(float3 normal, float3 worldPos, float2 uv);
float3 CalcLambertDiffuse(float3 lightDirection, float3 lightColor, float3 normal);
float3 CalcPhongSpecular(float3 lightDirection, float3 lightColor, float3 normal, float3 worldPos, float2 uv);
float3 CalcPointLight(float3 worldPos, float3 normal, float2 uv);
float3 CalcSpotLight(float3 worldPos, float3 normal, float2 uv);
float3 CalcLimLight(float3 normal, float normalInViewZ);
float4 ShadowMap(float4 posInLVP, float4 albedo);

////////////////////////////////////////////////
// 関数定義。
////////////////////////////////////////////////

/// <summary>
//スキン行列を計算する。
/// </summary>
float4x4 CalcSkinMatrix(SSkinVSIn skinVert)
{
	float4x4 skinning = 0;	
	float w = 0.0f;
	[unroll]
    for (int i = 0; i < 3; i++)
    {
        skinning += g_boneMatrix[skinVert.Indices[i]] * skinVert.Weights[i];
        w += skinVert.Weights[i];
    }
    
    skinning += g_boneMatrix[skinVert.Indices[3]] * (1.0f - w);
	
    return skinning;
}

/// <summary>
/// 頂点シェーダーのコア関数。
/// </summary>
SPSIn VSMainCore(SVSIn vsIn, uniform bool hasSkin)
{
	SPSIn psIn;
	float4x4 m;
	if( hasSkin ){
		m = CalcSkinMatrix(vsIn.skinVert);
	}else{
		m = mWorld;
	}
	psIn.pos = mul(m, vsIn.pos);
	psIn.pos = mul(mView, psIn.pos);
	psIn.pos = mul(mProj, psIn.pos);

	//法線を求める。
	psIn.normal = normalize(mul(m, vsIn.normal));
	psIn.tangent = normalize(mul(m, vsIn.tangent));
	psIn.biNormal = normalize(mul(m, vsIn.biNormal));

	//カメラ空間の法線を求める。
	psIn.normalInView = mul(mView, psIn.normal);

	//ワールド座標を求める。
	psIn.worldPos = mul(m, vsIn.pos);

	//ライトビュースクリーン空間の座標を求める。
	float4 worldPos = mul(m, vsIn.pos);
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
	return VSMainCore(vsIn, false);
}

/// <summary>
/// スキンありメッシュの頂点シェーダーのエントリー関数。
/// </summary>
SPSIn VSSkinMain( SVSIn vsIn ) 
{
	return VSMainCore(vsIn, true);
}

/// <summary>
/// ピクセルシェーダーのエントリー関数。
/// </summary>
float4 PSMainCore( SPSIn psIn, uniform bool hasShadow)
{
	//法線を計算。
	float3 normal = CalcNormal(psIn.normal, psIn.uv, psIn.tangent, psIn.biNormal);

	//ディレクションライトを計算。
	float3 directionLight = CalcDirectionLight(normal, psIn.worldPos, psIn.uv);

	//ポイントライトを計算。
	float3 pointLight = CalcPointLight(psIn.worldPos, normal, psIn.uv);

	//スポットライトを計算。
	float3 spotLight = CalcSpotLight(psIn.worldPos, normal, psIn.uv);

	//リムライトを計算。
	float3 limPower = CalcLimLight(normal, psIn.normalInView.z);

	float3 lig = directionLight
				+ dirLig.ambient
				+ pointLight
				+ spotLight
				+ limPower;

	float4 albedoColor = g_albedo.Sample(g_sampler, psIn.uv);

	if(hasShadow){
		albedoColor =  ShadowMap(psIn.posInLVP, albedoColor);
	}
	albedoColor.xyz *= lig;


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
/// 法線を計算。
/// </summary>
float3 CalcNormal(float3 normal, float2 uv, float3 tangent, float3 biNormal)
{
	//法線マップからタンジェントスペースの法線をサンプリングする。
	float3 localNormal = g_normal.Sample(g_sampler, uv).xyz;

	//タンジェントスペースの法線を0～1の範囲から-1～1の範囲に復元する。
	localNormal = (localNormal - 0.5f) *2.0f;

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
	float specPower = g_specular.Sample(g_sampler, uv).r;

	return specPower * 2.0f;
}

/// <summary>
/// ディレクションライトを計算。
/// </summary>
float3 CalcDirectionLight(float3 normal, float3 worldPos, float2 uv)
{
	//拡散反射光を求める。
	float3 diffuseLig = CalcLambertDiffuse(dirLig.dirDirection, dirLig.dirColor, normal);

	//鏡面反射光を求める。
	float3 specularLig = CalcPhongSpecular(dirLig.dirDirection, dirLig.dirColor, normal, worldPos, uv);

	return diffuseLig + specularLig;
}

/// <summary>
/// Lambert拡散反射を計算。
/// </summary>
float3 CalcLambertDiffuse(float3 lightDirection, float3 lightColor, float3 normal)
{
	//ピクセルの法線とライトの方向の内積を計算する。
	float t = dot(normal, lightDirection);

	//内積の結果に-1を乗算する。
	t *= -1.0f;

	//内積の結果がマイナスなら0にする。
	t = max(t, 0.0f);

	return lightColor * t;
}

/// <summary>
/// Phong鏡面反射を計算。
/// </summary>
float3 CalcPhongSpecular(float3 lightDirection, float3 lightColor, float3 normal, float3 worldPos, float2 uv)
{
	//反射ベクトルを求める。
	float3 refVec = reflect(lightDirection, normal);

	//光が当たったサーフェイスから視点に伸びるベクトルを求める。
	float3 toEye = eyePos - worldPos;

	//正規化する。
	toEye = normalize(toEye);

	//鏡面反射の強さを求める。
	float t  = dot(refVec, toEye);

	//内積の結果がマイナスなら0にする。
	t = max(t, 0.0f);

	//鏡面反射の強さを求める。
	t = pow(t, 2.0f);

	//ライトの色と鏡面反射の強さを乗算する。
	float3 specDirection = lightColor * t;

	//スペキュラマップを求める
	specDirection += CalcSpecular(uv);

	return specDirection;
}

/// <summary>
/// ポイントライトを計算。
/// </summary>
float3 CalcPointLight(float3 worldPos, float3 normal, float2 uv)
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
			ligDir,
			ptLig[i].ptColor,
			normal,
			worldPos,
			uv
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
float3 CalcSpotLight(float3 worldPos, float3 normal, float2 uv)
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
			ligDir,
			spLig[i].spColor,
			normal,
			worldPos,
			uv
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

/// <summary>
/// リムライトを計算。
/// </summary>
float3 CalcLimLight(float3 normal, float normalInViewZ)
{
	//サーフェイスの法線と光の入射方向に依存するリムの強さを求める。
	float power1 = 1.0f - max(0.0f, dot(dirLig.dirDirection, normal));

	//サーフェイスの法線と視線の方向に依存するリムの強さを求める。
	float power2 = 1.0f - max(0.0f, normalInViewZ * -1.0f);

	//最終的なリムの強さを求める。
	float limPower = power1 * power2;

	//強さの変化を指数関数的にする。
	limPower = pow(limPower, 3.0f);

	return limPower * dirLig.dirColor;
}

/// <summary>
/// シャドウを計算
/// </summary>
float4 ShadowMap(float4 posInLVP, float4 albedo)
{
    //ライトビュースクリーン空間からUV空間に座標変換
    float2 shadowMapUV = posInLVP.xy / posInLVP.w;
    shadowMapUV *= float2(0.5f, -0.5f);
    shadowMapUV += 0.5f;

    //ライトビュースクリーン空間でのZ値を計算する
    float zInLVP = posInLVP.z;

    if(shadowMapUV.x > 0.0f && shadowMapUV.x < 1.0f
        && shadowMapUV.y > 0.0f && shadowMapUV.y < 1.0f)
    {
		//VSMの実装
		//シャドウマップから値をサンプリング
		float2 shadowValue = g_shadowMap.Sample(g_sampler,shadowMapUV).xy;

		//このピクセルが遮蔽されているか調べる
		if(zInLVP > shadowValue.r && zInLVP <= 5.0f){

			//遮蔽されているなら、チェビシェフの不等式を利用して光が当たる確率を求める
			float depth_sq = shadowValue.x * shadowValue.x;

			//分散が大きいほど、varianceの数値は大きくなる
			float variance = min( max(shadowValue.y - depth_sq, 0.0001f), 1.0f);

			//このピクセルのライトから見た深度地とシャドウマップの平均の深度地の差を求める
			float md = zInLVP - shadowValue.x;

			//光が届く確率を求める
			float lit_factor = variance / (variance + md * md);

			//シャドウカラーを求める
			float3 shadowColor = albedo.xyz * 0.5f;

			//光が当たる確率を使って通常カラーとシャドウカラーを線形補間
			albedo.xyz = lerp( shadowColor, albedo.xyz, lit_factor);
		}
    }

	return albedo;
}
