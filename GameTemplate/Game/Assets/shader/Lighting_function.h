////////////////////////////////////////////////
// ライティング関数。
////////////////////////////////////////////////

////////////////////////////////////////////////
// PBRライティング。
////////////////////////////////////////////////
#include "PBRLighting.h"

/// <summary>
/// Lambert拡散反射光を計算する
/// </summary>
/// <param name="lightDirection">ライトの方向</param>
/// <param name="lightColor">ライトのカラー</param>
/// <param name="normal">法線</param>
float3 CalcLambertDiffuse(
    float3 lightDirection,
    float3 lightColor,
    float3 normal
)
{
	//ピクセルの法線とライトの方向の内積を計算する
	float t = saturate(dot(normal, lightDirection));

	float3 lambertFiffuse = (lightColor * t) / PI;

	return lambertFiffuse;
}

/// <summary>
/// Phong鏡面反射を計算。
/// </summary>
/// <param name="toEye">視点に向かって伸びるベクトル</param>
/// <param name="lightDirection">ライトの方向</param>
/// <param name="lightColor">ライトのカラー</param>
/// <param name="normal">法線</param>
/// <param name="worldPos">ワールド座標</param>
/// <param name="uv">UV座標</param>
float3 CalcPhongSpecular(
    float3 toEye,
    float3 lightDirection,
    float3 lightColor,
    float3 normal,
    float metallic
)
{
	//反射ベクトルを求める。
	float3 refVec = reflect(lightDirection, normal);

	//鏡面反射の強さを求める。
	float t  = dot(refVec, toEye);

	//内積の結果がマイナスなら0にする。
	t = max(t, 0.0f);

	//鏡面反射の強さを求める。
	t = pow(t, 2.0f);

	//ライトの色と鏡面反射の強さを乗算する。
	float3 specDirection = lightColor * t;

	//スペキュラマップを求める
	specDirection += (metallic * 2.0f);

	return specDirection;
}

/// <summary>
/// ディレクションライトを計算。
/// </summary>
/// <param name="toEye">視点に向かうベクトル</param>
/// <param name="normal">法線</param>
/// <param name="dirDirection">ディレクションライトの方向</param>
/// <param name="dirColor">ディレクションライトのカラー</param>
/// <param name="specularColor">スペキュラカラー</param>
/// <param name="metallic">メタリック</param>
/// <param name="smooth">スムース</param>
float3 CalcDirectionLight(
    float3 toEye,
    float3 normal,
    float3 dirDirection,
    float3 dirColor,
    float3 specularColor,
    float metallic,
    float smooth
)
{
	//フレネル反射を考慮した拡散反射を計算。
	float diffuseFromFrenel = CalcDiffuseFromFresnel(
		normal,
		-dirDirection,
		toEye,
		smooth
	);

	//正規化Lambert拡散反射を計算。
	float3 lambertDiffuse = CalcLambertDiffuse(
		-dirDirection,
		dirColor,
		normal
	);

	//最終的な拡散反射光を計算する。
	float3 diffuse = diffuseFromFrenel * lambertDiffuse;

	//Cook-Torranceモデルを利用した鏡面反射率を計算する。
	float3 specular = CookTorranceSpecular(
		-dirDirection,
		toEye,
		normal,
		smooth
	) * dirColor;

	//金属度が高ければ、鏡面反射はスペキュラカラー、低ければ白。
	//スペキュラカラーの強さを鏡面反射率として扱う。
	specular *= lerp(float3(1.0f, 1.0f, 1.0f), specularColor, metallic);

	//滑らかさを使って、拡散反射光と鏡面反射校を合成する。
	//滑らかさが高ければ、拡散反射は弱くなる。
	return diffuse * (1.0f - smooth) + specular;
}

/// <summary>
/// リムライトを計算。
/// </summary>
/// <param name="normal">法線</param>
/// <param name="normalInViewZ">カメラ空間の法線</param>
/// <param name="lightDirection">ライトの方向</param>
/// <param name="lightColor">ライトのカラー</param>
float3 CalcLimLight(float3 normal, float normalInViewZ, float3 dirDirection, float3 dirColor)
{
	//サーフェイスの法線と光の入射方向に依存するリムの強さを求める。
	float power1 = 1.0f - max(0.0f, dot(dirDirection, normal));
	//サーフェイスの法線と視線の方向に依存するリムの強さを求める。
	float power2 = 1.0f - max(0.0f, normalInViewZ * -1.0f);

	//最終的なリムの強さを求める。
	float limPower = power1 * power2;

	//強さの変化を指数関数的にする。
	limPower = pow(limPower, 5.0f);

	return limPower * dirColor;
}