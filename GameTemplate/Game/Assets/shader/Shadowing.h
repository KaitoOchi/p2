////////////////////////////////////////////////
// シャドウイング。
////////////////////////////////////////////////

/// <summary>
/// シャドウを計算。
/// </summary>
float ShadowMap(Texture2D<float4>shadowMap, float4 posInLVP)
{
    //ライトビュースクリーン空間からUV空間に座標変換。
    float2 shadowMapUV = posInLVP.xy / posInLVP.w;
    shadowMapUV *= float2(0.5f, -0.5f);
    shadowMapUV += 0.5f;

    //ライトビュースクリーン空間でのZ値を計算する。
    float zInLVP = posInLVP.z / posInLVP.w;

    if(shadowMapUV.x > 0.0f && shadowMapUV.x < 1.0f
        && shadowMapUV.y > 0.0f && shadowMapUV.y < 1.0f
		&& zInLVP < 0.98f && zInLVP > 0.02f)
    {
		//VSMの実装。
		//シャドウマップから値をサンプリング。
		float2 shadowValue = shadowMap.Sample(g_sampler, shadowMapUV).xy;

		//このピクセルが遮蔽されているか調べる。
		if(zInLVP > shadowValue.r && zInLVP <= 5.0f){

			//遮蔽されているなら、チェビシェフの不等式を利用して光が当たる確率を求める。
			float depth_sq = shadowValue.x * shadowValue.x;

			//分散が大きいほど、varianceの数値は大きくなる。
			float variance = min( max(shadowValue.y - depth_sq, 0.0001f), 1.0f);

			//このピクセルのライトから見た深度地とシャドウマップの平均の深度地の差を求める。
			float md = zInLVP - shadowValue.x;

			//光が届く確率を求める。
			float lit_factor = variance / (variance + md * md);

			return 1.0f - lit_factor;

			//シャドウカラーを求める。
			//float3 shadowColor = albedo.xyz * 0.5f;

			//光が当たる確率を使って通常カラーとシャドウカラーを線形補間。
			//albedo.xyz = lerp( shadowColor, albedo.xyz, lit_factor);
		}
    }

	return 0.0f;
}
