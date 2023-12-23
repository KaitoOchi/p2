////////////////////////////////////////////////
// �f�B�t�@�[�h���C�e�B���O�̃��W�X�^�B
////////////////////////////////////////////////
static const int PORTAL_NUM = 2;

Texture2D<float4> g_albedoTexture                   : register(t0);     //�A���x�h�J���[�B
Texture2D<float4> g_normalTexture                   : register(t1);     //�@���}�b�v�B
Texture2D<float4> g_metallicShadowSmoothTexture     : register(t2);     //���^���b�N�X���[�X�B
Texture2D<float4> g_param                           : register(t3);     //�p�����[�^�B
Texture2D<float4> g_shadowMap                       : register(t4);     //�V���h�E�}�b�v
Texture2D<float4> g_portalRenderTexture[PORTAL_NUM] : register(t5);     //�|�[�^���e�N�X�`���B
StructuredBuffer<uint> pointLightListInTile         : register(t20);    //�|�C���g���C�g�p��UAV�B
StructuredBuffer<uint> spotLightListInTile          : register(t21);    //�X�|�b�g���C�g�p��UAV�B
sampler           g_sampler                         : register(s0);		//�T���v���B