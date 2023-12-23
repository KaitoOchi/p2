#include "PortalEnginePreCompile.h"
#include "RenderingEngine.h"

namespace nsPortalEngine {

	namespace
	{
		const Vector3	SHADOW_CAMERA_POS = Vector3(200.0f, 3000.0f, 200.0f);		//�V���h�E�J�����̈ʒu�B
		const int		SHADOW_TEXTURE_SIZE = 1024;								//�V���h�E�e�N�X�`���̃T�C�Y�B
		const float		RENDER_TARGET_CLEAR_COLOR[4] = { 0.5f, 0.5f, 0.5f, 0.0f };	//�N���A�J���[�B
		const float		SHADOW_BLUR_POWER = 5.0f;									//�V���h�E�u���[�̋����B
		const char*		DEFERREDLIGHTING_SPRITE_PS[3] = {
															"PSMain",
															"PSMainBluePortal",
															"PSMainRedPortal",
														};
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
		//�f�B���N�V�������C�g�̐ݒ�B
		m_lightCB.directionLig.ligDirection = Vector3(0.0f, -1.0f, 0.0f);
		m_lightCB.directionLig.ligColor = Vector3(0.1f, 0.1f, 0.1f);
		m_lightCB.directionLig.ambient = 0.2f;

		InitRenderTarget();

		InitDeferredLightingSprite();

		//�|�X�g�G�t�F�N�g�̏������B
		m_postEffect.Init(m_mainRenderTarget, m_zprepassRenderTarget[0]);
		m_postEffect.SetBloomThreshold(3.0f);
		m_postEffect.Disable(PostEffect::enPostEffect_DoF);

		//���C�g�J�����O�̏������B
		for (int i = 0; i <= PORTAL_NUM ; i++) {
			m_lightCulling[i].Init(
				m_deferredLightingSprite[0].GetExpandConstantBufferGPU(),
				m_zprepassRenderTarget[i].GetRenderTargetTexture(),
				m_pointLightNoListInTileUAV[i],
				m_spotLightNoListInTileUAV[i]
			);
		}
		m_lightCulling[0].SetCamera(*g_camera3D);
		m_lightCulling[1].SetCamera(m_portalCamera->GetPortalCamera(0));
		m_lightCulling[2].SetCamera(m_portalCamera->GetPortalCamera(1));
	}

	void RenderingEngine::InitDeferredLightingSprite()
	{
		SpriteInitData spriteInitData;
		spriteInitData.m_width = g_graphicsEngine->GetFrameBufferWidth();
		spriteInitData.m_height = g_graphicsEngine->GetFrameBufferHeight();

		//�e�e�N�X�`���B
		spriteInitData.m_textures[enGBufferNum] = &m_shadowBlur.GetBokeTexture();
		//�|�[�^���e�N�X�`���B
		spriteInitData.m_textures[enGBufferNum + 1] = &m_mainRenderTarget[1].GetRenderTargetTexture();
		spriteInitData.m_textures[enGBufferNum + 2] = &m_mainRenderTarget[2].GetRenderTargetTexture();

		spriteInitData.m_fxFilePath = "Assets/shader/DeferredLighting.fx";
		spriteInitData.m_expandConstantBuffer = &m_lightCB;
		spriteInitData.m_expandConstantBufferSize = sizeof(m_lightCB);
		spriteInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;


		for (int i = 0; i <= PORTAL_NUM; i++) {
			//�^�C�����Ƃ̃|�C���g���C�g�̔ԍ��̃��X�g���o�͂���UAV���������B
			m_pointLightNoListInTileUAV[i].Init(
				sizeof(int),
				POINT_LIGHT_NUM * TILE_NUM,
				nullptr
			);
			//�^�C�����Ƃ̃X�|�b�g���C�g�̔ԍ��̃��X�g���o�͂���UAV���������B
			m_spotLightNoListInTileUAV[i].Init(
				sizeof(int),
				SPOT_LIGHT_NUM * TILE_NUM,
				nullptr
			);

			//�|�C���g���C�g�ƃX�|�b�g���C�g��UAV�B
			spriteInitData.m_expandShaderResoruceView[0] = &m_pointLightNoListInTileUAV[i];
			spriteInitData.m_expandShaderResoruceView[1] = &m_spotLightNoListInTileUAV[i];

			// �f�B�t�@�[�h���C�e�B���O�Ŏg�p����e�N�X�`����ݒ�B
			for (int gBufferNum = 0; gBufferNum < enGBufferNum; gBufferNum++) {
				spriteInitData.m_textures[gBufferNum] = &m_gBuffer[i][gBufferNum].GetRenderTargetTexture();
			}

			spriteInitData.m_psEntryPoinFunc = DEFERREDLIGHTING_SPRITE_PS[i];

			// �������f�[�^���g���ăX�v���C�g���쐬�B
			m_deferredLightingSprite[i].Init(spriteInitData);
		}
	}

	void RenderingEngine::InitRenderTarget()
	{
		InitMainRenderTarget();

		InitZPrepassRenderTarget();

		InitShadowMapRenderTarget();

		InitGBuffer();

		Init2DRenderTarget();
	}

	void RenderingEngine::InitGBuffer()
	{
		int frameBufferW = g_graphicsEngine->GetFrameBufferWidth();
		int frameBufferH = g_graphicsEngine->GetFrameBufferHeight();

		//���C��+�|�[�^���̐������쐬����B
		for (int i = 0; i <= PORTAL_NUM; i++) {
			//�A���x�h�J���[�o�͗p�����_�[�^�[�Q�b�g�̍쐬�B
			m_gBuffer[i][enGBufferAlbedoDepth].Create(
				frameBufferW,
				frameBufferH,
				1,
				1,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				DXGI_FORMAT_D32_FLOAT,
				const_cast<float*>(RENDER_TARGET_CLEAR_COLOR)
			);

			//�@���o�͗p�����_�[�^�[�Q�b�g�̍쐬�B
			m_gBuffer[i][enGBufferNormal].Create(
				frameBufferW,
				frameBufferH,
				1,
				1,
				DXGI_FORMAT_R8G8B8A8_SNORM,
				DXGI_FORMAT_UNKNOWN
			);


			//���^���b�N�X���[�X�o�͗p�����_�[�^�[�Q�b�g�̍쐬�B
			m_gBuffer[i][enGBufferMetaricShadowSmooth].Create(
				frameBufferW,
				frameBufferH,
				1,
				1,
				DXGI_FORMAT_R8G8B8A8_UNORM,
				DXGI_FORMAT_UNKNOWN
			);

			//�p�����[�^�o�͗p�����_�[�^�[�Q�b�g�̍쐬�B
			m_gBuffer[i][enGBufferShadowLightParam].Create(
				frameBufferW,
				frameBufferH,
				1,
				1,
				DXGI_FORMAT_R8G8B8A8_UNORM,
				DXGI_FORMAT_UNKNOWN
			);
		}
	}

	void RenderingEngine::InitMainRenderTarget()
	{
		float clearColor[4] = { 0.0f,0.0f,0.0f,0.0f };

		//���C�������_�[�^�[�Q�b�g��ݒ�B
		for (int i = 0; i <= PORTAL_NUM; i++) {
			m_mainRenderTarget[i].Create(
				g_graphicsEngine->GetFrameBufferWidth(),
				g_graphicsEngine->GetFrameBufferHeight(),
				1,
				1,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				DXGI_FORMAT_D32_FLOAT,
				const_cast<float*>(RENDER_TARGET_CLEAR_COLOR)
			);
		}
	}

	void RenderingEngine::Init2DRenderTarget()
	{
		//2D�p�����_�[�^�[�Q�b�g��ݒ�B
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
		finalSpriteInitData.m_width = m_mainRenderTarget[0].GetWidth();
		finalSpriteInitData.m_height = m_mainRenderTarget[0].GetHeight();
		finalSpriteInitData.m_fxFilePath = "Assets/shader/sprite.fx";
		finalSpriteInitData.m_alphaBlendMode = AlphaBlendMode_None;
		finalSpriteInitData.m_colorBufferFormat[0] = m_mainRenderTarget[0].GetColorBufferFormat();

		m_2DSprite.Init(finalSpriteInitData);

		//2D�����p�X�v���C�g�̏������B
		SpriteInitData spriteInitData;
		spriteInitData.m_textures[0] = &m_mainRenderTarget[0].GetRenderTargetTexture();
		spriteInitData.m_width = m_2DRenderTarget.GetWidth();
		spriteInitData.m_height = m_2DRenderTarget.GetHeight();
		spriteInitData.m_fxFilePath = "Assets/shader/sprite.fx";
		spriteInitData.m_alphaBlendMode = AlphaBlendMode_None;
		spriteInitData.m_colorBufferFormat[0] = m_2DRenderTarget.GetColorBufferFormat();

		m_mainSprite.Init(spriteInitData);
	}

	void RenderingEngine::InitShadowMapRenderTarget()
	{
		//�J�����̐ݒ�B
		//�ߕ��ʂ�ݒ�B
		m_lightCamera.SetNear(10.0f);
		//�����ʂ�ݒ�B
		m_lightCamera.SetFar(5000.0f);
		//�������ݒ�B
		m_lightCamera.SetUp(0, 1, 0);
		//�ˉe�s��̍X�V���@��ݒ�B
		m_lightCamera.SetUpdateProjMatrixFunc(Camera::enUpdateProjMatrixFunc_Ortho);
		m_lightCamera.SetWidth(SHADOW_TEXTURE_SIZE * 2);
		m_lightCamera.SetHeight(SHADOW_TEXTURE_SIZE * 2);
		m_lightCamera.Update();

		//�V���h�E�}�b�v�p�����_�[�^�[�Q�b�g�̏������B
		float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_shadowMapRenderTarget.Create(
			SHADOW_TEXTURE_SIZE,
			SHADOW_TEXTURE_SIZE,
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
		for (int i = 0; i <= PORTAL_NUM; i++) {
			//ZPrepass�p�����_�[�^�[�Q�b�g�̏������B
			m_zprepassRenderTarget[i].Create(
				g_graphicsEngine->GetFrameBufferWidth(),
				g_graphicsEngine->GetFrameBufferHeight(),
				1,
				1,
				m_mainRenderTarget[0].GetColorBufferFormat(),
				DXGI_FORMAT_D32_FLOAT
			);
		}
	}

	void RenderingEngine::UpdateCB()
	{
		//�萔�o�b�t�@�Ɏ��_�̈ʒu��ۑ��B
		m_lightCB.eyePos = g_camera3D->GetPosition();
		m_lightCB.eyePosBlue = m_portalCamera->GetPortalCamera(0).GetPosition();
		m_lightCB.eyePosRed = m_portalCamera->GetPortalCamera(1).GetPosition();

		//�V���h�E�p�J�����̈ʒu��ݒ�B
		m_lightCamera.SetPosition(g_camera3D->GetPosition() + SHADOW_CAMERA_POS);
		m_lightCamera.SetTarget(g_camera3D->GetPosition());
		m_lightCamera.Update();

		//�萔�o�b�t�@�ɃJ�����̃v���W�F�N�V�����s��ƍ��W��ۑ��B
		m_lightCB.shadowCB.mLVP = m_lightCamera.GetViewProjectionMatrix();
		m_lightCB.shadowCB.lightPos = m_lightCamera.GetTarget();
		//�r���[�v���W�F�N�V�����s��̋t�s���ۑ��B
		m_lightCB.mViewProjInv[0].Inverse(g_camera3D->GetViewProjectionMatrix());
		m_lightCB.mViewProjInv[1].Inverse(m_portalCamera->GetPortalCamera(0).GetViewProjectionMatrix());
		m_lightCB.mViewProjInv[2].Inverse(m_portalCamera->GetPortalCamera(1).GetViewProjectionMatrix());
	}

	void RenderingEngine::Execute(RenderContext& rc)
	{
		//�萔�o�b�t�@�̍X�V�B
		UpdateCB();

		//ZPrepass�̕`��B
		DrawZPrepass(rc);

		//�V���h�E�}�b�v�̕`��B
		DrawShadowMap(rc);

		for (int i = 0; i <= PORTAL_NUM; i++) {
			m_lightCulling[i].Execute(rc);
		}

		//GBuffer�̕`��B
		DrawGBuffer(rc);

		//�f�B�t�@�[�h���C�e�B���O�B
		DrawDeferredLighting(rc);

		//�t�H���[�h���C�e�B���O�B
		DrawForwardLighting(rc);

		//�|�X�g�G�t�F�N�g�B
		m_postEffect.Render(rc, m_mainRenderTarget);

		//2D�̕`��B
		Draw2D(rc);

		m_renderObjects.clear();
	}

	void RenderingEngine::DrawZPrepass(RenderContext& rc)
	{
		for (int i = 0; i <= PORTAL_NUM; i++) {
			// �����_�����O�^�[�Q�b�g�Ƃ��Đݒ�ł���悤�ɂȂ�܂ő҂B
			rc.WaitUntilToPossibleSetRenderTarget(m_zprepassRenderTarget[i]);
			// �����_�����O�^�[�Q�b�g��ݒ�B
			rc.SetRenderTargetAndViewport(m_zprepassRenderTarget[i]);
			// �����_�����O�^�[�Q�b�g���N���A�B
			rc.ClearRenderTargetView(m_zprepassRenderTarget[i]);

			if (i != 0) {
				for (auto& renderObj : m_renderObjects) {
					renderObj->OnRenderToZPrepass(rc, i, m_portalCamera->GetPortalCamera(i - 1));
				}
			}
			else {
				for (auto& renderObj : m_renderObjects) {
					renderObj->OnRenderToZPrepass(rc, i, *g_camera3D);
				}
			}

			//�����_�[�^�[�Q�b�g�̏������ݏI���҂��B
			rc.WaitUntilFinishDrawingToRenderTarget(m_zprepassRenderTarget[i]);
		}
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

		m_shadowBlur.ExecuteOnGPU(rc, SHADOW_BLUR_POWER);
	}

	void RenderingEngine::DrawGBuffer(RenderContext& rc)
	{
		//���C��+�|�[�^���̐������`�悷��B
		for (int i = PORTAL_NUM; i >= 0; i--) {

			RenderTarget* rts[enGBufferNum] = {
				&m_gBuffer[i][enGBufferAlbedoDepth],
				&m_gBuffer[i][enGBufferNormal],
				&m_gBuffer[i][enGBufferMetaricShadowSmooth],
				&m_gBuffer[i][enGBufferShadowLightParam]
			};

			int rtsNum = static_cast<int>(enGBufferNum);

			//�����_�[�^�[�Q�b�g�̏������ݑ҂��B
			rc.WaitUntilToPossibleSetRenderTargets(rtsNum, rts);
			//�����_�[�^�[�Q�b�g�̐ݒ�B
			rc.SetRenderTargetsAndViewport(rtsNum, rts);
			//�����_�[�^�[�Q�b�g���N���A�B
			rc.ClearRenderTargetViews(rtsNum, rts);

			if (i != 0) {
				//�|�[�^���p�̕`��B
				for (auto& renderObj : m_renderObjects)
				{
					renderObj->OnPortalRender(rc, i - 1, m_portalCamera->GetPortalCamera(i - 1));
				}
			}
			else {
				//���C����GBuffer��`��B
				for (auto& renderObj : m_renderObjects)
				{
					renderObj->OnRenderGBuffer(rc);
				}
			}

			//�����_�[�^�[�Q�b�g�̏������ݏI���҂��B
			rc.WaitUntilFinishDrawingToRenderTargets(rtsNum, rts);
		}
	}

	void RenderingEngine::DrawDeferredLighting(RenderContext& rc)
	{
		//�|�[�^���̐������X�v���C�g�̕`����s���B
		for (int i = 0; i <= PORTAL_NUM; i++) {

			//���C�������_�[�^�[�Q�b�g�̏������ݑ҂��B
			rc.WaitUntilToPossibleSetRenderTarget(m_mainRenderTarget[i]);
			//���C�������_�[�^�[�Q�b�g�̐ݒ�B
			rc.SetRenderTargetAndViewport(m_mainRenderTarget[i]);
			//���C�������_�[�^�[�Q�b�g�̐ݒ�B
			rc.ClearRenderTargetView(m_mainRenderTarget[i]);

			// G-Buffer�̓��e�����ɂ��ăf�B�t�@�[�h���C�e�B���O
			m_deferredLightingSprite[i].Draw(rc);

			//���C�������_�[�^�[�Q�b�g�̏������ݏI���҂��B
			rc.WaitUntilFinishDrawingToRenderTarget(m_mainRenderTarget[i]);
		}
	}

	void RenderingEngine::DrawForwardLighting(RenderContext& rc)
	{
		//���C�������_�[�^�[�Q�b�g�̏������ݑ҂��B
		rc.WaitUntilToPossibleSetRenderTarget(m_mainRenderTarget[0]);
		//���C�������_�[�^�[�Q�b�g�̐ݒ�B
		rc.SetRenderTargetAndViewport(m_mainRenderTarget[0]);

		//�ʏ�`�揈���B
		for (auto& renderObj : m_renderObjects)
		{
			renderObj->OnForwardRender(rc);
		}

		//���C�������_�[�^�[�Q�b�g�̏������ݏI���҂��B
		rc.WaitUntilFinishDrawingToRenderTarget(m_mainRenderTarget[0]);
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

		//2D�摜�̕`�揈���B
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

		rc.WaitUntilToPossibleSetRenderTarget(m_mainRenderTarget[0]);

		//�ŏI��ʂ̕`��B
		m_2DSprite.Draw(rc);

		rc.WaitUntilFinishDrawingToRenderTarget(m_mainRenderTarget[0]);
	}
}