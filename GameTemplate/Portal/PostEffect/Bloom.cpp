#include "PortalEnginePreCompile.h"
#include "Bloom.h"

namespace nsPortalEngine {

	Bloom::Bloom()
	{

	}

	Bloom::~Bloom()
	{

	}

	void Bloom::OnInit(RenderTarget& mainRenderTarget, RenderTarget& zprepassRenderTarget)
	{
		//�����_�[�^�[�Q�b�g�̏������B
		m_luminanceRenderTarget.Create(
			mainRenderTarget.GetWidth(),
			mainRenderTarget.GetHeight(),
			1,
			1,
			mainRenderTarget.GetColorBufferFormat(),
			DXGI_FORMAT_D32_FLOAT
		);

		//�K�E�V�A���u���[���������B
		m_gaussianBlur[0].Init(&m_luminanceRenderTarget.GetRenderTargetTexture());
		m_gaussianBlur[1].Init(&m_gaussianBlur[0].GetBokeTexture());
		m_gaussianBlur[2].Init(&m_gaussianBlur[1].GetBokeTexture());
		m_gaussianBlur[3].Init(&m_gaussianBlur[2].GetBokeTexture());

		//�P�x���o�p�̃X�v���C�g���������B
		InitLuminanceSprite(mainRenderTarget);

		//���Z�����p�X�v���C�g���������B
		InitFinalSprite(mainRenderTarget);

		//���C�������_�����O�^�[�Q�b�g�ɍ����p�̃X�v���C�g���������B
		SpriteInitData initData;
		initData.m_width = mainRenderTarget.GetWidth();
		initData.m_height = mainRenderTarget.GetHeight();
		initData.m_fxFilePath = "Assets/shader/Sprite.fx";
		initData.m_textures[0] = &mainRenderTarget.GetRenderTargetTexture();
		m_copyMainRtSprite.Init(initData);
	}

	void Bloom::InitLuminanceSprite(RenderTarget& mainRenderTarget)
	{
		//�P�x���o�p�̃X�v���C�g��ݒ�B
		SpriteInitData spriteInitData;
		spriteInitData.m_fxFilePath = "Assets/shader/postEffect/Bloom.fx";
		spriteInitData.m_vsEntryPointFunc = "VSMain";
		spriteInitData.m_psEntryPoinFunc = "PSLuminance";

		spriteInitData.m_expandConstantBuffer = &m_luminanceCB;
		spriteInitData.m_expandConstantBufferSize = sizeof(m_luminanceCB);

		spriteInitData.m_width = mainRenderTarget.GetWidth();
		spriteInitData.m_height = mainRenderTarget.GetHeight();
		spriteInitData.m_textures[0] = &mainRenderTarget.GetRenderTargetTexture();
		// �`�����ރ����_�����O�^�[�Q�b�g�̃t�H�[�}�b�g���w�肷��B
		spriteInitData.m_colorBufferFormat[0] = mainRenderTarget.GetColorBufferFormat();

		//�P�x���o�p�̃X�v���C�g���������B
		m_luminanceSprite.Init(spriteInitData);
	}

	void Bloom::InitFinalSprite(RenderTarget& mainRenderTarget)
	{
		//�{�P�摜���������ď������ނ��߂̃X�v���C�g���������B
		SpriteInitData finalSpriteInitData;
		finalSpriteInitData.m_textures[0] = &m_gaussianBlur[0].GetBokeTexture();
		finalSpriteInitData.m_textures[1] = &m_gaussianBlur[1].GetBokeTexture();
		finalSpriteInitData.m_textures[2] = &m_gaussianBlur[2].GetBokeTexture();
		finalSpriteInitData.m_textures[3] = &m_gaussianBlur[3].GetBokeTexture();

		// �X�v���C�g�̕��ƍ�����luminnceRenderTarget�Ɠ����B
		finalSpriteInitData.m_width = mainRenderTarget.GetWidth();
		finalSpriteInitData.m_height = mainRenderTarget.GetHeight();

		finalSpriteInitData.m_fxFilePath = "Assets/shader/postEffect/Bloom.fx";
		finalSpriteInitData.m_psEntryPoinFunc = "PSBloomFinal";

		//�A���t�@�u�����f�B���O���[�h�����Z�ɂ���B
		finalSpriteInitData.m_alphaBlendMode = AlphaBlendMode_Add;
		finalSpriteInitData.m_colorBufferFormat[0] = mainRenderTarget.GetColorBufferFormat();

		//���Z�����p�̃X�v���C�g���������B
		m_finalSprite.Init(finalSpriteInitData);
	}

	void Bloom::OnRender(RenderContext& rc, RenderTarget& mainRenderTarget)
	{
		// �P�x���o
		// �P�x���o�p�̃����_�����O�^�[�Q�b�g�ɕύX
		rc.WaitUntilToPossibleSetRenderTarget(m_luminanceRenderTarget);

		// �����_�����O�^�[�Q�b�g��ݒ�
		rc.SetRenderTargetAndViewport(m_luminanceRenderTarget);

		// �����_�����O�^�[�Q�b�g���N���A
		rc.ClearRenderTargetView(m_luminanceRenderTarget);

		// �P�x���o���s��
		m_luminanceSprite.Draw(rc);

		// �����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
		rc.WaitUntilFinishDrawingToRenderTarget(m_luminanceRenderTarget);

		m_gaussianBlur[0].ExecuteOnGPU(rc, BLUR_POWER);
		m_gaussianBlur[1].ExecuteOnGPU(rc, BLUR_POWER);
		m_gaussianBlur[2].ExecuteOnGPU(rc, BLUR_POWER);
		m_gaussianBlur[3].ExecuteOnGPU(rc, BLUR_POWER);

		// 4���̃{�P�摜���������ă��C�������_�����O�^�[�Q�b�g�ɉ��Z����
		// �����_�����O�^�[�Q�b�g�Ƃ��ė��p�ł���܂ő҂�
		rc.WaitUntilToPossibleSetRenderTarget(mainRenderTarget);
		// �����_�����O�^�[�Q�b�g��ݒ�
		rc.SetRenderTargetAndViewport(mainRenderTarget);

		// �ŏI����
		m_finalSprite.Draw(rc);

		// �����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
		rc.WaitUntilFinishDrawingToRenderTarget(mainRenderTarget);

		// ���C�������_�����O�^�[�Q�b�g�̊G���t���[���o�b�t�@�[�ɃR�s�[
		rc.SetRenderTarget(
			g_graphicsEngine->GetCurrentFrameBuffuerRTV(),
			g_graphicsEngine->GetCurrentFrameBuffuerDSV()
		);

		// �|�X�g�G�t�F�N�g�̌��ʂ����C�������_�����O�^�[�Q�b�g�ɔ��f�B
		m_copyMainRtSprite.Draw(rc);
	}
}