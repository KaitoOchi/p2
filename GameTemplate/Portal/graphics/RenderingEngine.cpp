#include "PortalEnginePreCompile.h"
#include "RenderingEngine.h"

namespace nsPortalEngine {

	namespace
	{
		const Vector3 SHADOW_CAMERA_POS = Vector3(0.0f, 3000.0f, 0.0f);		//�V���h�E�J�����̈ʒu�B
		const float RENDER_TARGET_CLEAR_COLOR[4] = { 0.5f, 0.5f, 0.5f, 1.0f };	//�N���A�J���[�B
	}

	RenderingEngine* RenderingEngine::m_instance = nullptr;


	RenderingEngine::RenderingEngine()
	{
		m_renderObjects.reserve(256);
	}

	RenderingEngine::~RenderingEngine()
	{

	}

	void RenderingEngine::Init()
	{
		m_lightCB.directionLig.ligDirection = Vector3(1.0f, -1.0f, 1.0f);
		m_lightCB.directionLig.ligColor = Vector3(0.3f, 0.1f, 0.1f);
		m_lightCB.directionLig.ambient = 0.3f;

		InitRenderTarget();

		m_postEffect.Init(m_mainRenderTarget, m_zprepassRenderTarget);
		//m_postEffect.SetBloomThreshold(50.0f);
	}

	void RenderingEngine::InitRenderTarget()
	{
		InitMainRenderTarget();

		Init2DRenderTarget();

		InitPortalRenderTarget();

		InitShadowMapRenderTarget();

		InitZPrepassRenderTarget();
	}

	void RenderingEngine::InitMainRenderTarget()
	{
		//���C�������_�[�^�[�Q�b�g��ݒ�B
		m_mainRenderTarget.Create(
			g_graphicsEngine->GetFrameBufferWidth(),
			g_graphicsEngine->GetFrameBufferHeight(),
			1,
			1,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			DXGI_FORMAT_D32_FLOAT,
			const_cast<float*>(RENDER_TARGET_CLEAR_COLOR)
		);
	}

	void RenderingEngine::Init2DRenderTarget()
	{
		float clearColor[4] = { 0.0f,0.0f,0.0f,0.0f };
		m_2DRenderTarget.Create(
			g_graphicsEngine->GetFrameBufferWidth(),
			g_graphicsEngine->GetFrameBufferHeight(),
			1,
			1,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_FORMAT_UNKNOWN,
			clearColor
		);

		//�ŏI�����p�X�v���C�g�̏������B
		SpriteInitData finalSpriteInitData;
		finalSpriteInitData.m_textures[0] = &m_2DRenderTarget.GetRenderTargetTexture();
		finalSpriteInitData.m_width = m_mainRenderTarget.GetWidth();
		finalSpriteInitData.m_height = m_mainRenderTarget.GetHeight();
		finalSpriteInitData.m_fxFilePath = "Assets/shader/sprite.fx";
		finalSpriteInitData.m_alphaBlendMode = AlphaBlendMode_None;
		finalSpriteInitData.m_colorBufferFormat[0] = m_mainRenderTarget.GetColorBufferFormat();

		m_2DSprite.Init(finalSpriteInitData);

		//2D�����p�X�v���C�g�̏������B
		SpriteInitData spriteInitData;
		spriteInitData.m_textures[0] = &m_mainRenderTarget.GetRenderTargetTexture();
		spriteInitData.m_width = m_2DRenderTarget.GetWidth();
		spriteInitData.m_height = m_2DRenderTarget.GetHeight();
		spriteInitData.m_fxFilePath = "Assets/shader/sprite.fx";
		spriteInitData.m_alphaBlendMode = AlphaBlendMode_None;
		spriteInitData.m_colorBufferFormat[0] = m_2DRenderTarget.GetColorBufferFormat();

		m_mainSprite.Init(spriteInitData);
	}

	void RenderingEngine::InitPortalRenderTarget()
	{
		for (int i = 0; i < PORTAL_NUM; i++) {

			//�|�[�^���p�����_�[�^�[�Q�b�g��ݒ�B
			m_portalRenderTarget[i].Create(
				g_graphicsEngine->GetFrameBufferWidth(),
				g_graphicsEngine->GetFrameBufferHeight(),
				1,
				1,
				m_mainRenderTarget.GetColorBufferFormat(),
				DXGI_FORMAT_D32_FLOAT,
				const_cast<float*>(RENDER_TARGET_CLEAR_COLOR)
			);
		}
	}

	void RenderingEngine::InitShadowMapRenderTarget()
	{
		//�J�����̐ݒ�B
		//�ߕ��ʂ�ݒ�B
		m_lightCamera.SetNear(10.0f);
		//�����ʂ�ݒ�B
		m_lightCamera.SetFar(5000.0f);
		//�������ݒ�B
		m_lightCamera.SetUp(1, 0, 0);
		//�ˉe�s��̍X�V���@��ݒ�B
		m_lightCamera.SetUpdateProjMatrixFunc(Camera::enUpdateProjMatrixFunc_Ortho);

		//�V���h�E�}�b�v�p�����_�[�^�[�Q�b�g�̏������B
		float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_shadowMapRenderTarget.Create(
			4096,
			4096,
			1,
			1,
			DXGI_FORMAT_R32G32_FLOAT,
			DXGI_FORMAT_D32_FLOAT,
			clearColor
		);

		//�V���h�E�}�b�v�p�̃K�E�V�A���u���[���������B
		m_shadowBlur.Init(&m_shadowMapRenderTarget.GetRenderTargetTexture());
	}

	void RenderingEngine::InitZPrepassRenderTarget()
	{
		//ZPrepass�p�����_�[�^�[�Q�b�g�̏������Bjb 
		m_zprepassRenderTarget.Create(
			g_graphicsEngine->GetFrameBufferWidth(),
			g_graphicsEngine->GetFrameBufferHeight(),
			1,
			1,
			m_mainRenderTarget.GetColorBufferFormat(),
			DXGI_FORMAT_D32_FLOAT
		);
	}

	void RenderingEngine::UpdateCB()
	{
		//�萔�o�b�t�@�Ɏ��_�̈ʒu��ۑ��B
		m_lightCB.eyePos = g_camera3D->GetPosition();

		//�V���h�E�p�J�����̈ʒu��ݒ�B
		m_lightCamera.SetPosition(g_camera3D->GetPosition() + SHADOW_CAMERA_POS);
		m_lightCamera.SetTarget(g_camera3D->GetPosition());
		m_lightCamera.Update();

		//�萔�o�b�t�@�ɃJ�����̃v���W�F�N�V�����s��ƍ��W��ۑ��B
		m_lightCB.shadowCB.mLVP = m_lightCamera.GetViewProjectionMatrix();
		m_lightCB.shadowCB.lightPos = m_lightCamera.GetTarget();
	}

	void RenderingEngine::Execute(RenderContext& rc)
	{
		//�萔�o�b�t�@�̍X�V�B
		UpdateCB();

		//ZPrepass�̕`��B
		DrawZPrepass(rc);

		//�V���h�E�}�b�v�̕`��B
		DrawShadowMap(rc);

		//�t�H���[�h�����_�����O�B
		DrawForwardRendering(rc);

		//�|�X�g�G�t�F�N�g�B
		m_postEffect.Render(rc, m_mainRenderTarget);

		//2D�̕`��B
		Draw2D(rc);

		m_renderObjects.clear();
	}

	void RenderingEngine::DrawZPrepass(RenderContext& rc)
	{
		// �����_�����O�^�[�Q�b�g�Ƃ��Đݒ�ł���悤�ɂȂ�܂ő҂B
		rc.WaitUntilToPossibleSetRenderTarget(m_zprepassRenderTarget);
		// �����_�����O�^�[�Q�b�g��ݒ�B
		rc.SetRenderTargetAndViewport(m_zprepassRenderTarget);
		// �����_�����O�^�[�Q�b�g���N���A�B
		rc.ClearRenderTargetView(m_zprepassRenderTarget);

		for (auto& renderObj : m_renderObjects) {
			renderObj->OnRenderToZPrepass(rc);
		}

		//�����_�[�^�[�Q�b�g�̏������ݏI���҂��B
		rc.WaitUntilFinishDrawingToRenderTarget(m_zprepassRenderTarget);
	}

	void RenderingEngine::DrawShadowMap(RenderContext& rc)
	{
		//�V���h�E�}�b�v�p�����_�[�^�[�Q�b�g�̏������ݑ҂��B
		rc.WaitUntilToPossibleSetRenderTarget(m_shadowMapRenderTarget);
		// �����_�����O�^�[�Q�b�g��ݒ�B
		rc.SetRenderTargetAndViewport(m_shadowMapRenderTarget);
		// �����_�����O�^�[�Q�b�g���N���A�B
		rc.ClearRenderTargetView(m_shadowMapRenderTarget);

		//�`�揈���B
		for (auto& renderObj : m_renderObjects) {
			renderObj->OnRenderShadowMap(rc, m_lightCamera);
		}

		//�����_�[�^�[�Q�b�g�̏������ݏI���҂��B
		rc.WaitUntilFinishDrawingToRenderTarget(m_shadowMapRenderTarget);

		m_shadowBlur.ExecuteOnGPU(rc, 5.0f);
	}

	void RenderingEngine::DrawForwardRendering(RenderContext& rc)
	{
		//�|�[�^���̐������I�t�X�N���[�������_�����O���s���B
		for (int i = 0; i < PORTAL_NUM; i++) {

			//�|�[�^���p�����_�[�^�[�Q�b�g�̏������ݑ҂��B
			rc.WaitUntilToPossibleSetRenderTarget(m_portalRenderTarget[i]);
			//�|�[�^���p�����_�[�^�[�Q�b�g�̐ݒ�B
			rc.SetRenderTargetAndViewport(m_portalRenderTarget[i]);
			//�|�[�^���p�����_�[�^�[�Q�b�g���N���A�B
			rc.ClearRenderTargetView(m_portalRenderTarget[i]);

			//�I�t�X�N���[�������_�����O�B
			for (auto& renderObj : m_renderObjects)
			{
				renderObj->OnPortalRender(rc, i, m_portalCamera->GetPortalCamera(i));
			}

			//�|�[�^���p�����_�[�^�[�Q�b�g�̏������ݏI���҂��B
			rc.WaitUntilFinishDrawingToRenderTarget(m_portalRenderTarget[i]);
		}


		//���C�������_�[�^�[�Q�b�g�̏������ݑ҂��B
		rc.WaitUntilToPossibleSetRenderTarget(m_mainRenderTarget);
		//���C�������_�[�^�[�Q�b�g�̐ݒ�B
		rc.SetRenderTargetAndViewport(m_mainRenderTarget);
		//���C�������_�[�^�[�Q�b�g�̐ݒ�B
		rc.ClearRenderTargetView(m_mainRenderTarget);

		//�ʏ�`�揈���B
		for (auto& renderObj : m_renderObjects)
		{
			renderObj->OnForwardRender(rc);
		}

		//���C�������_�[�^�[�Q�b�g�̏������ݏI���҂��B
		rc.WaitUntilFinishDrawingToRenderTarget(m_mainRenderTarget);
	}

	void RenderingEngine::Draw2D(RenderContext& rc)
	{
		//�����_�����O�^�[�Q�b�g�������ݑ҂��B
		rc.WaitUntilToPossibleSetRenderTarget(m_2DRenderTarget);
		//�����_�����O�^�[�Q�b�g�̐ݒ�B
		rc.SetRenderTargetAndViewport(m_2DRenderTarget);
		//�����_�����O�^�[�Q�b�g���N���A�B
		rc.ClearRenderTargetView(m_2DRenderTarget);

		m_mainSprite.Draw(rc);

		//�`�揈���B
		for (auto& renderObj : m_renderObjects)
		{
			renderObj->On2DRender(rc);
		}

		rc.WaitUntilFinishDrawingToRenderTarget(m_2DRenderTarget);

		//�����_�[�^�[�Q�b�g�̐؂�ւ��B
		rc.SetRenderTarget(
			g_graphicsEngine->GetCurrentFrameBuffuerRTV(),
			g_graphicsEngine->GetCurrentFrameBuffuerDSV()
		);

		rc.WaitUntilToPossibleSetRenderTarget(m_mainRenderTarget);

		//�ŏI2D�X�v���C�g�̕`��B
		m_2DSprite.Draw(rc);

		rc.WaitUntilFinishDrawingToRenderTarget(m_mainRenderTarget);
	}
}