////////////////////////////////////////////////
// ポータル用シェーダー。
////////////////////////////////////////////////

////////////////////////////////////////////////
// 定数バッファ。
////////////////////////////////////////////////
//モデル用の定数バッファ。
cbuffer ModelCb : register(b0){
	float4x4 mWorld;
	float4x4 mView;
	float4x4 mProj;
};

////////////////////////////////////////////////
// 構造体
////////////////////////////////////////////////
//頂点シェーダーへの入力。
struct SVSIn{
	float4 pos 		: POSITION;		//モデルの頂点座標。
	float2 uv 		: TEXCOORD0;	//UV座標。
};
//ピクセルシェーダーへの入力。
struct SPSIn{
	float4 pos 			: SV_POSITION;	//スクリーン空間でのピクセルの座標。
	float2 uv 			: TEXCOORD0;	//uv座標。
};

////////////////////////////////////////////////
// グローバル変数。
////////////////////////////////////////////////
Texture2D<float4>           g_albedoMap : register(t0);			    //アルベドマップ。
Texture2D<float4>			g_renderTexture : register(t10);		//ポータル越しのカメラ背景。
sampler                     g_sampler : register(s0);				//サンプラステート。

///////////////////////////////////////////////////
// 関数
///////////////////////////////////////////////////
/// <summary>
/// 頂点シェーダーのコア関数。
/// </summary>
SPSIn VSMainCore(SVSIn vsIn)
{
	SPSIn psIn;
	psIn.pos = mul(mWorld, vsIn.pos);
	psIn.pos = mul(mView, psIn.pos);
	psIn.pos = mul(mProj, psIn.pos);
	psIn.uv = vsIn.uv;

	return psIn;
}

/// <summary>
/// 頂点シェーダーのエントリー関数。
/// </summary>
SPSIn VSMain(SVSIn vsIn)
{
	return VSMainCore(vsIn);
}

/// <summary>
/// スキンありメッシュの頂点シェーダーのエントリー関数。
/// </summary>
SPSIn VSSkinMain( SVSIn vsIn ) 
{
	return VSMainCore(vsIn);
}

/// <summary>
/// ピクセルシェーダーのエントリー関数。
/// </summary>
float4 PSMainCore(SPSIn psIn)
{
	//アルベドカラーをサンプリング。
	float4 albedoColor = g_albedoMap.Sample(g_sampler, psIn.uv);

    clip(albedoColor.a - 0.001f);

	albedoColor = g_renderTexture.Sample(g_sampler,psIn.uv);

	return albedoColor;
}

/// <summary>
/// ピクセルシェーダーのエントリー関数。
/// </summary>
float4 PSMain( SPSIn psIn ) : SV_Target0
{
	return PSMainCore(psIn);
}