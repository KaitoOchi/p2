////////////////////////////////////////////////
// PBRライティング。
////////////////////////////////////////////////
static const float PI = 3.1415926f;         // 円周率。

/// <summary>
/// フレネル反射を考慮した拡散反射を計算。
/// </summary>
/// <param name="normal">法線</param>
/// <param name="dir">高原に向かうベクトル。光の方向と逆向きのベクトル</param>
/// <param name="toEye">視線に向かうベクトル</param>
float CalcDiffuseFromFresnel(float3 dir, float3 toEye, float3 normal, float smooth)
{
	//光源に向かうベクトルと視線に向かうベクトルのハーフベクトルを求める。
    float3 H = normalize(dir + toEye);
    
	//粗さを求める。
	float roughness = 1.0f - smooth;
	float energyBias = lerp(0.0f, 0.5f, roughness);
	float energyFactor = lerp(1.0f, 1.0f / 1.51f, roughness);

	//光源に向かうベクトルとハーフベクトルがどれだけ似ているかを内積で求める。
	float dotLH = saturate(dot(dir, H));

	//光が平行に入射したときの拡散反射量を求める。
	float Fd90 = energyBias + 2.0f * dotLH * dotLH * roughness;

	//法線と光源に向かうベクトルを利用して拡散反射率を求める。
	float dotNL = saturate(dot(normal, dir));
	float FL = (1 + (Fd90 - 1) * pow(1 - dotNL, 5));

	//法線と視点に向かうべう取るを利用して拡散反射率を求める。
	float dotNV = saturate(dot(normal, toEye));
	float FV = (1 + (Fd90 - 1) * pow(1 - dotNV, 5));

	//法線と光源への方向に依存する拡散反射率と
	//法線と視点ベクトルに依存する拡散反射率を乗算して、最終的な拡散反射率を求める。
	return (FL * FV * energyFactor);
}

/// <summary>
/// ベックマン分布を計算。
/// </summary>
float Beckmann(float m, float t)
{
    float t2 = t * t;
    float t4 = t * t * t * t;
    float m2 = m * m;
    float D = 1.0f / (4.0f * m2 * t4);
    D *= exp((-1.0f / m2) * (1.0f-t2)/ t2);
    return D;
}

/// <summary>
/// フレネルを計算。Schlick近似を使用。
/// </summary>
float SpcFresnel(float f0, float u)
{
    return f0 + (1-f0) * pow(1-u, 5);
}

/// <summary>
/// Cook-Torranceモデルの鏡面反射を計算。
/// </summary>
/// <param name="dir">光源に向かうベクトル</param>
/// <param name="toEye">視点に向かうベクトル</param>
/// <param name="normal">法線ベクトル</param>
/// <param name="smooth">滑らかさ</param>
float CookTorranceSpecular(
    float3 dir,
    float3 toEye,
    float3 normal,
    float smooth
)
{
    float microfacet = min(0.5f, 1.0f - smooth);

    // 金属度を垂直入射の時のフレネル反射率として扱う。
    // 金属度が高いほどフレネル反射は大きくなる。
    float f0 = 0.5f;

    // ライトに向かうベクトルと視線に向かうベクトルのハーフベクトルを求める。
    float3 H = normalize(dir + toEye);

    // 各種ベクトルがどれくらい似ているかを内積を利用して求める。
    float NdotH = max(0.001f, min(dot(normal, H), 1.0f));
    float VdotH = max(0.001f, min(dot(toEye, H),  1.0f));
    float NdotL = max(0.001f, min(dot(normal, dir), 1.0f));
    float NdotV = max(0.001f, min(dot(normal, toEye), 1.0f));

    // D項をベックマン分布を用いて計算する。
    float D = Beckmann(microfacet, NdotH);

    // F項をSchlick近似を用いて計算する。
    float F = SpcFresnel(f0, VdotH);

    // G項を求める。
    float G = min(1.0f, min(2 * NdotH * NdotV / VdotH, 2 * NdotH * NdotL / VdotH));

    // m項を求める。
    float m = PI * NdotV * NdotH;

    // ここまで求めた、値を利用して、Cook-Torranceモデルの鏡面反射を求める
    return saturate(F * D * G / m);
}