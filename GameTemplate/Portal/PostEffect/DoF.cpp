#include "PortalEnginePreCompile.h"
#include "DoF.h"

namespace nsPortalEngine {

	DoF::DoF()
	{

	}

	DoF::~DoF()
	{

	}

	void DoF::OnInit(RenderTarget& mainRenderTarget, RenderTarget& zprepassRenderTarget)
	{
		//�[�x�l�p�̃K�E�V�A���u���[���������B
		m_depthGaussianBlur.Init(&mainRenderTarget.GetRenderTargetTexture());

		//�{�P�摜�����p�̃X�v���C�g������������B
		SpriteInitData combineBokeImageSpriteInitData;
		combineBokeImageSpriteInitData.m_textures[0] = &m_depthGaussianBlur.GetBokeTexture();
		combineBokeImageSpriteInitData.m_textures[1] = &zprepassRenderTarget.GetRenderTargetTexture();
		combineBokeImageSpriteInitData.m_width = g_graphicsEngine->GetFrameBufferWidth();
		combineBokeImageSpriteInitData.m_height = g_graphicsEngine->GetFrameBufferHeight();

		combineBokeImageSpriteInitData.m_fxFilePath = "Assets/shader/postEffect/DoF.fx";
		combineBokeImageSpriteInitData.m_colorBufferFormat[0] = mainRenderTarget.GetColorBufferFormat(),
		combineBokeImageSpriteInitData.m_alphaBlendMode = AlphaBlendMode_Trans;

		m_combineBokeImageSprite.Init(combineBokeImageSpriteInitData);
	}

	void DoF::OnRender(RenderContext& rc, RenderTarget& mainRenderTarget)
	{
		//���C�������_�[�^�[�Q�b�g�̃{�P�摜���쐬�B
		m_depthGaussianBlur.ExecuteOnGPU(rc, 5.0f);

		//�����_�[�^�[�Q�b�g�̏������ݑ҂��B
		rc.WaitUntilToPossibleSetRenderTarget(mainRenderTarget);
		//���C�������_�[�^�[�Q�b�g�̐ݒ�B
		rc.SetRenderTargetAndViewport(mainRenderTarget);

		m_combineBokeImageSprite.Draw(rc);

		//���C�������_�[�^�[�Q�b�g�̏������ݏI���҂��B
		rc.WaitUntilFinishDrawingToRenderTarget(mainRenderTarget);
	}
}