cbuffer cb : register(b0)
{
	float4x4 mvp;
	float4 mulColor;
}

struct VSInput
{
	float4 pos	: POSITION;
	float2 uv	: TEXCOORD0;
};

struct PSInput
{
	float4 pos	: SV_POSITION;
	float2 uv	: TEXCOORD0;
};

//���_�V�F�[�_�[
PSInput VSMain(VSInput In)
{
	PSInput psIn;
	psIn.pos = mul(mvp, In.pos);
	psIn.uv = In.uv;

	return psIn;
}

Texture2D<float4> mainRenderTargetTexture : register(t0);	//���C�������_�����O�^�[�Q�b�g�̃e�N�X�`��
sampler Sampler : register(s0);

cbuffer SamplingLuminanceCb : register(b1)
{
	float threshold;
};

//�P�x���o�p
float4 PSLuminance(PSInput In) : SV_Target0
{
	//���C�������_�����O�^�[�Q�b�g����J���[���T���v�����O
	float4 color = mainRenderTargetTexture.Sample(Sampler, In.uv);

	//�T���v�����O�����J���[�̖��邳���v�Z
	float t = dot(color.xyz, float3(0.2125f, 0.7154f, 0.0721f));

	clip(t - threshold);
	t -= threshold;
	return color;
}

Texture2D<float> g_bokeTexture_0 : register(t0);
Texture2D<float> g_bokeTexture_1 : register(t1);
Texture2D<float> g_bokeTexture_2 : register(t2);
Texture2D<float> g_bokeTexture_3 : register(t3);

float4 SetColor(float4 color);

float4 PSBloomFinal(PSInput In) : SV_Target0
{
	//ボケ画像をサンプリングしてする。
	float4 combineColor = g_bokeTexture_0.Sample(Sampler, In.uv);
	combineColor += g_bokeTexture_1.Sample(Sampler, In.uv);
	combineColor += g_bokeTexture_2.Sample(Sampler, In.uv);
	combineColor += g_bokeTexture_3.Sample(Sampler, In.uv);

	//平均を取って出力。
	combineColor /= 4.0f;
	combineColor.a = 1.0f;
	return combineColor;
}