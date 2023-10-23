////////////////////////////////////////////////
// �f�B�t�@�[�h���C�e�B���O�̃��W�X�^�B
////////////////////////////////////////////////
Texture2D<float4> g_albedoTexture                   : register(t0);     //�A���x�h�B
Texture2D<float4> g_normalTexture                   : register(t1);     //�@���B
Texture2D<float4> g_metallicShadowSmoothTexture     : register(t2);     //r�Ƀ��^���b�N�Ag�ɃV���h�E�Aa�ɃX���[�X�B
Texture2D<float4> g_shadowMap                       : register(t3);    //�V���h�E�}�b�v�B
sampler           g_sampler                         : register(s0);		//�T���v���X�e�[�g�B