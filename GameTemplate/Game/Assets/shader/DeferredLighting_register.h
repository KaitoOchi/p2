////////////////////////////////////////////////
// �f�B�t�@�[�h���C�e�B���O�p���W�X�^�B
////////////////////////////////////////////////
static const int PORTAL_NUM = 2;

Texture2D<float4> g_albedoTexture                   : register(t0);     //�A���x�h�B
Texture2D<float4> g_normalTexture                   : register(t1);     //�@���B
Texture2D<float4> g_metallicShadowSmoothTexture     : register(t2);     //���^���b�N�X���[�X�B
Texture2D<float4> g_param                           : register(t3);     //�e��p�����[�^�B
Texture2D<float4> g_shadowMap                       : register(t4);     //�V���h�E�}�b�v�B
Texture2D<float4> g_portalRenderTexture[PORTAL_NUM] : register(t5);     //�|�[�^���B
StructuredBuffer<uint> pointLightListInTile         : register(t20);    //�^�C�����Ƃ̃|�C���g���C�g�̃C���f�b�N�X�̃��X�g�B
StructuredBuffer<uint> spotLightListInTile          : register(t21);    //�^�C�����Ƃ̃X�|�b�g���C�g�̃C���f�b�N�X�̃��X�g�B
sampler           g_sampler                         : register(s0);		//�T���v���[�B