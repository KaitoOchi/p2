///////////////////////////////////////
// ZPrepass
///////////////////////////////////////


///////////////////////////////////////
// 構造体。
///////////////////////////////////////
//スキニング用の頂点データをひとまとめ。
struct SSkinVSIn{
	int4  Indices  	: BLENDINDICES0;
    float4 Weights  : BLENDWEIGHT0;
};

// 頂点シェーダーへの入力
struct SVSIn
{
    float4 pos : POSITION;          //頂点座標。
    float2 uv  : TEXCOORD0;    		//UV座標。
    SSkinVSIn skinVert;				//スキン用のデータ。
};

// ピクセルシェーダーへの入力
struct SPSIn
{
    float4 pos : SV_POSITION;       //座標。
	float2 uv  : TEXCOORD0;    		//UV座標。
    float3 depth : TEXCOORD1;       //深度値。xにはプロジェクション空間、yにはカメラ空間での正規化されたZ値、zにはカメラ空間でのZ値
};

///////////////////////////////////////
// 定数バッファ。
///////////////////////////////////////
// モデル用の定数バッファー
cbuffer ModelCb : register(b0)
{
    float4x4 mWorld;
    float4x4 mView;
    float4x4 mProj;
};

///////////////////////////////////////
// 関数
///////////////////////////////////////
Texture2D<float4> g_albedo : register(t0);
StructuredBuffer<float4x4> g_boneMatrix : register(t3);		//ボーン行列。
sampler g_sampler : register(s0);

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

// モデル用の頂点シェーダーのエントリーポイント
SPSIn VSMainCore(SVSIn vsIn, uniform bool hasSkin)
{
	SPSIn psIn;
	float4x4 m;
	if( hasSkin ){
		m = CalcSkinMatrix(vsIn.skinVert);
	}else{
		m = mWorld;
	}

    psIn.pos = mul(m, vsIn.pos); // モデルの頂点をワールド座標系に変換
    psIn.pos = mul(mView, psIn.pos); // ワールド座標系からカメラ座標系に変換
    psIn.depth.z = psIn.pos.z;
    psIn.pos = mul(mProj, psIn.pos); // カメラ座標系からスクリーン座標系に変換
    psIn.depth.x = psIn.pos.z / psIn.pos.w;
    psIn.depth.y = saturate( psIn.pos.w / 1000.0f );

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
/// ピクセルシェーダーのコア関数。
/// </summary>
float4 PSMain(SPSIn psIn) : SV_Target0
{
    //アルベドカラーをサンプリング。
	float4 albedoColor = g_albedo.Sample(g_sampler, psIn.uv);

    clip(albedoColor.a - 0.001f);

    return float4( psIn.pos.z, psIn.depth.y, psIn.depth.x, psIn.depth.z );
}