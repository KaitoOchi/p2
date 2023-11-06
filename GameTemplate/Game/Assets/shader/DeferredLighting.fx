///////////////////////////////////////
// �f�B�t�@�[�h���C�e�B���O�B
///////////////////////////////////////
static const float PI = 3.1415926f;         // �~�����B

///////////////////////////////////////
// �\���́B
///////////////////////////////////////
//���_�V�F�[�_�[�ւ̓��́B
struct VSInput
{
    float4 pos  : POSITION;     //���W�B
    float2 uv   : TEXCOORD0;    //UV���W�B
};

//�s�N�Z���V�F�[�_�[�ւ̓��́B
struct PSInput
{
    float4 pos  : SV_POSITION;     //���W�B
    float2 uv   : TEXCOORD0;    //UV���W�B
};

///////////////////////////////////////
// ���f���̒��_�V�F�[�_�[�֌W�̋��ʃw�b�_�[�B
///////////////////////////////////////
#include "ModelVSCommon.h"

////////////////////////////////////////////////
// ���W�X�^�B
////////////////////////////////////////////////
#include "DeferredLighting_register.h"

////////////////////////////////////////////////
// ���C�g�p�萔�o�b�t�@�̍\���́B
////////////////////////////////////////////////
#include "PBRLighting_struct.h"

////////////////////////////////////////////////
// ���C�e�B���O�B
////////////////////////////////////////////////
#include "Lighting_function.h"

////////////////////////////////////////////////
// �V���h�E�C���O�B
////////////////////////////////////////////////
#include "Shadowing.h"

///////////////////////////////////////
// �萔�o�b�t�@�B
///////////////////////////////////////
//�萔�o�b�t�@�B
cbuffer cb : register(b0)
{
    float4x4 mvp; 
    float4 mulColor;
    float4 screenParam;
};


///////////////////////////////////////
// �֐�
///////////////////////////////////////
/// <summary>
/// �|�C���g���C�g���v�Z�B
/// </summary>
/// <param name="worldPos">���[���h���W</param>
/// <param name="toEye">���_�̈ʒu</param>
/// <param name="normal">�@��</param>
/// <param name="specCOlor">�X�y�L�����J���[</param>
/// <param name="metallic">�����x</param>
/// <param name="smooth">�X���[�X</param>
float3 CalcPointLight(
    float3 worldPos,
    float3 toEye,
    float3 normal,
    float3 specColor,
    float metallic,
    float smooth
)
{
	float3 finalPtLig = 0.0f;

	for(int i = 0; i < ptNum; i++) {
		//�T�[�t�F�C�X�ɓ��˂���|�C���g���C�g�̐��K���x�N�g�����v�Z����B
		float3 ligDir = normalize(worldPos - ptLig[i].ptPosition);

        //���C�g���v�Z�B
        float3 lig = CalcDirectionLight(
            toEye,
            normal,
            ligDir,
            ptLig[i].ptColor,
            specColor,
            metallic,
            smooth
        );

		//�����ɂ��e�������v�Z����B
		//�|�C���g���C�g�Ƃ̋������v�Z����B
		float distance = length(worldPos - ptLig[i].ptPosition);
		//�e�����͋����ɔ�Ⴕ�ď������Ȃ��Ă����B
		float affect = 1.0f - min(1.0f, distance / ptLig[i].ptRange);
		//�e�����w���֐��I�ɂ���B
		affect = pow(affect, 2.0f);
		//����������Z���ĉe������߂�B
        lig *= affect;

        finalPtLig += lig;
	}

	return finalPtLig;
}

/// <summary>
/// �X�|�b�g���C�g���v�Z�B
/// </summary>
/// <param name="worldPos">���[���h���W</param>
/// <param name="toEye">���_�̈ʒu</param>
/// <param name="normal">�@��</param>
/// <param name="specCOlor">�X�y�L�����J���[</param>
/// <param name="metallic">�����x</param>
/// <param name="smooth">�X���[�X</param>
float3 CalcSpotLight(
    float3 worldPos,
    float3 toEye,
    float3 normal,
    float3 specColor,
    float metallic,
    float smooth
)
{
	float3 finalSpLig = 0.0f;

	for(int i = 0; i < spNum; i++) {
		//�T�[�t�F�C�X�ɓ��˂���X�|�b�g���C�g�̐��K���x�N�g�����v�Z����B
		float3 ligDir = normalize(worldPos - spLig[i].spPosition);

        //���C�g���v�Z�B
        float3 lig = CalcDirectionLight(
            toEye,
            normal,
            ligDir,
            spLig[i].spColor,
            specColor,
            metallic,
            smooth
        );

		//�����ɂ��e�������v�Z����B
		//�X�|�b�g���C�g�Ƃ̋������v�Z����B
		float distance = length(worldPos - spLig[i].spPosition);
		//�e�����͋����ɔ�Ⴕ�ď������Ȃ��Ă����B
		float affect = 1.0f - min(1.0f, distance / spLig[i].spRange);
		//�e���̎d�����w���֐��I�ɂ���B
		affect = pow(affect, 2.0f);
		//�e��������Z���Ĕ��ˌ�����߂�B
		lig *= affect;

		//���ˌ��Ǝˏo�����̊p�x�����߂�B
		float angle = dot(ligDir, spLig[i].spDirection);
		angle = abs(acos(angle));
		//�p�x�ɂ��e���������߂�B
		affect = max(0.0f, 1.0f - 1.0f / spLig[i].spAngle * angle);
		//�e���̎d�����w���֐��I�ɂ���B
		affect = pow(affect, 2.0f);
		//�e��������Z���Ĕ��ˌ�����߂�B
		lig *= affect;

		finalSpLig += lig;
	}

	return finalSpLig;
}

//���_�V�F�[�_�[�B
PSInput VSMain(VSInput vsIn)
{
    PSInput psIn;

    psIn.pos = mul(mvp, vsIn.pos);
    psIn.uv = vsIn.uv;

    return psIn;
}

//�s�N�Z���V�F�[�_�[�B
float4 PSMainCore(PSInput psIn)
{
    //GBuffer�̓��e���g���ă��C�e�B���O�B
    //�A���x�h�J���[�B
    float4 albedoColor = g_albedoTexture.Sample(g_sampler, psIn.uv);
    //�@���B
    float4 normal = g_normalTexture.Sample(g_sampler, psIn.uv);
    //�J������Ԃ̖@���B
    float normalInViewZ = normal.a;
    //���^���b�N�V���h�E�X���[�X�B
    float4 metallicShadowSmooth = g_metallicShadowSmoothTexture.Sample(g_sampler, psIn.uv);
    //���^���b�N�B
    float metallic = metallicShadowSmooth.r;
    //�e�����p�̃p�����[�^�B
    float shadowParam = metallicShadowSmooth.g;
    //���C�e�B���O�p�����[�^�B
    float lightingParam = metallicShadowSmooth.b;
    //�X���[�X�B
    float smooth = metallicShadowSmooth.a;

    //���C�e�B���O�Ȃ��B
    if(lightingParam <= 0.0f) {
        return albedoColor;
    }

    //�X�y�L�����J���[�B
    float3 specColor = albedoColor.xyz;
    //���[���h���W���v�Z����B
    float3 worldPos = CalcWorldPosFromUVZ(psIn.uv, albedoColor.w, mViewProjInv);
    //���_�Ɍ������ĐL�т�x�N�g�����v�Z����B
    float3 toEye = normalize(eyePos - worldPos);
    //���C�g�r���[�X�N���[����Ԃ����߂�B
	float4 posInLVP = mul(shadow.mLVP, float4(worldPos, 1.0f));
    //�V���h�E�̌v�Z�B
    float shadowRate =  ShadowMap(g_shadowMap, posInLVP) * shadowParam;

    //�f�B���N�V�������C�g���v�Z�B
	float3 lig = CalcDirectionLight(
        toEye,
        normal.xyz,
        dirLig.dirDirection,
        dirLig.dirColor,
        specColor,
        metallic,
        smooth
    ) * (1.0f - shadowRate);

    //�|�C���g���C�g���v�Z�B
    lig += CalcPointLight(
        worldPos,
        toEye,
        normal.xyz,
        specColor,
        metallic,
        smooth
    );

    //�X�|�b�g���C�g���v�Z�B
    lig += CalcSpotLight(
        worldPos,
        toEye,
        normal.xyz,
        specColor,
        metallic,
        smooth
    );

    //�������C�g���v�Z�B
    lig += CalcLimLight(
        normal.xyz,
        normalInViewZ,
        dirLig.dirDirection,
        dirLig.dirColor
    );

    //�����Ń��C�g���グ����B
    lig += dirLig.ambient * albedoColor.xyz;

    float4 finalColor = 1.0f;
    finalColor.xyz = lig;
    return finalColor;
}

/// <summary>
/// �ʏ�̃G���g���[�֐��B
/// </summary>
float4 PSMain(PSInput psIn) : SV_Target0
{
    return PSMainCore(psIn);
}

/// <summary>
/// ���C�e�B���O�����̃G���g���[�֐��B
/// </summary>
float4 PSMainNotLighting(PSInput psIn) : SV_Target0
{
    return g_albedoTexture.Sample(g_sampler, psIn.uv);
}