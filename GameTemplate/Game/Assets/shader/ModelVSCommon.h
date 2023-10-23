///////////////////////////////////////
// モデルの頂点シェーダー関係の共通ヘッダー。
///////////////////////////////////////


///////////////////////////////////////
// 定数バッファ。
///////////////////////////////////////
//スキニング用の頂点データをひとまとめ。
struct SSkinVSIn{
	int4  Indices  	: BLENDINDICES0;
    float4 Weights  : BLENDWEIGHT0;
};

//モデル用の定数バッファ。
cbuffer ModelCb : register(b0){
	float4x4 mWorld;
	float4x4 mView;
	float4x4 mProj;
};

///////////////////////////////////////
// 構造体。
///////////////////////////////////////
// 頂点シェーダーへの入力
struct SVSIn
{
    float4 pos      : POSITION;     //座標。
    float3 normal   : NORMAL;       //法線。
    float3 tangent  : TANGENT;      //接ベクトル。
    float3 biNormal : BINORMAL;     //従ベクトル。
    float2 uv       : TEXCOORD0;    //UV座標。
    SSkinVSIn skinVert;             //スキン用のデータ。
};

////////////////////////////////////////////////
// グローバル変数。
////////////////////////////////////////////////
StructuredBuffer<float4x4>  g_boneMatrix : register(t3);    //ボーン行列。

///////////////////////////////////////
// 関数
///////////////////////////////////////
/// <summary>
/// スキン行列を計算する。
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
/// ワールド空間の頂点座標を計算する。
/// </summary>
/// <param name="vertexPos">頂点座標</param>
/// <param name="mWorld">ワールド行列</param>
float4 CalcVertexPositionInWorldSpace(float4 vertexPos, float4x4 mWorld)
{
    float4 pos;
    pos = mul(mWorld, vertexPos);  // モデルの頂点をワールド座標系に変換。
    return pos;
}

/// <summary>
/// ワールドスペースの法線、接ベクトル、従ベクトルを計算する。
/// </summary>
/// <param name="oNormal">法線の出力先</param>
/// <param name="oTangent">接ベクトルの出力先</param>
/// <param name="oBiNormal">従ベクトルの出力先</param>
/// <param name="mWorld">ワールド行列</param>
/// <param name="iNormal">法線</param>
/// <param name="iTanget">接ベクトル</param>
/// <param name="iBiNormal">従ベクトル</param>
void CalcVertexNormalTangentBiNormalInWorldSpace( 
    out float3 oNormal, 
    out float3 oTangent, 
    out float3 oBiNormal,
    float4x4 mWorld,
    float3 iNormal,
    float3 iTangent,
    float3 iBiNormal
)
{
    float3x3 m3x3 = (float3x3)mWorld;
    oNormal = normalize(mul(m3x3, iNormal));
    oTangent = normalize(mul(m3x3, iTangent));
    oBiNormal = normalize(mul(m3x3, iBiNormal));
}