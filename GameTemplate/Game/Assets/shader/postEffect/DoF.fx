/////////////////////////////////////////////////////////
// 被写界深度。
/////////////////////////////////////////////////////////

cbuffer cb : register(b0)
{
    float4x4 mvp;       // MVP行列
    float4 mulColor;    // 乗算カラー
};

struct VSInput
{
    float4 pos : POSITION;
    float2 uv  : TEXCOORD0;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

/*!
 * @brief 頂点シェーダー
 */
PSInput VSMain(VSInput In)
{
    PSInput psIn;
    psIn.pos = mul(mvp, In.pos);
    psIn.uv = In.uv;
    return psIn;
}

//step-11 ボケ画像と深度テクスチャにアクセスするための変数を追加
Texture2D<float4> bokeTexture : register(t0);
Texture2D<float4> depthTexture : register(t1);

sampler Sampler : register(s0);

float4 PSMain(PSInput In) : SV_Target0
{
    //カメラ空間での深度値をサンプリング。
    float depth = depthTexture.Sample(Sampler, In.uv).z;

    //深度値が200以下ならピクセルキル。
    clip(depth - 400);

    //ボケ画像をサンプリング。
    float4 boke = bokeTexture.Sample(Sampler, In.uv);

    //200からボケが始まり、800で最大。
    boke.a = min(1.0f, (depth - 500) / 1000.0f);

    return boke;
}
