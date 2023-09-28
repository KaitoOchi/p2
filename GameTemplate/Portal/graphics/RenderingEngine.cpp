#include "PortalEnginePreCompile.h"
#include "RenderingEngine.h"

namespace nsPortalEngine {

	namespace
	{
		const Vector3 SHADOW_CAMERA_POS = Vector3(0.0f, 300.0f, 0.0f);			//シャドウカメラの位置。
		const float RENDER_TARGET_CLEAR_COLOR[4] = { 0.5f, 0.5f, 0.5f, 1.0f };	//クリアカラー。
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
		m_lightCB.directionLig.ligColor = Vector3(0.1f, 0.1f, 0.1f);
		m_lightCB.directionLig.ambient = 0.2f;

		InitRenderTarget();

		m_postEffect.Init(m_mainRenderTarget);
	}

	void RenderingEngine::InitRenderTarget()
	{
		InitMainRenderTarget();

		Init2DRenderTarget();

		InitPortalRenderTarget();

		InitShadowMapRenderTarget();
	}

	void RenderingEngine::InitMainRenderTarget()
	{
		//メインレンダーターゲットを設定
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

		//最終合成用スプライトの初期化
		SpriteInitData finalSpriteInitData;
		finalSpriteInitData.m_textures[0] = &m_2DRenderTarget.GetRenderTargetTexture();
		finalSpriteInitData.m_width = m_mainRenderTarget.GetWidth();
		finalSpriteInitData.m_height = m_mainRenderTarget.GetHeight();
		finalSpriteInitData.m_fxFilePath = "Assets/shader/sprite.fx";
		finalSpriteInitData.m_alphaBlendMode = AlphaBlendMode_None;
		finalSpriteInitData.m_colorBufferFormat[0] = m_mainRenderTarget.GetColorBufferFormat();

		m_2DSprite.Init(finalSpriteInitData);

		//2D統合用スプライトの初期化
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

			//ポータル用レンダーターゲットを設定。
			m_portalRenderTarget[i].Create(
				g_graphicsEngine->GetFrameBufferWidth(),
				g_graphicsEngine->GetFrameBufferHeight(),
				1,
				1,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				DXGI_FORMAT_D32_FLOAT,
				const_cast<float*>(RENDER_TARGET_CLEAR_COLOR)
			);

			m_portalCamera[0].SetPosition(Vector3(200.0f, 300.0f, 300.0f));
			m_portalCamera[0].SetTarget(Vector3(0.0f, 0.0f, 0.0f));
			m_portalCamera[0].Update();
		}
	}

	void RenderingEngine::InitShadowMapRenderTarget()
	{
		// カメラの位置を設定。これはライトの位置
		m_lightCamera.SetPosition(g_camera3D->GetPosition() + SHADOW_CAMERA_POS);
		// カメラの注視点を設定。これがライトが照らしている場所
		m_lightCamera.SetTarget(g_camera3D->GetPosition());
		// 上方向を設定。
		m_lightCamera.SetUp(1, -1, 1);

		// ライトビュープロジェクション行列を計算している
		m_lightCB.shadowCB.mLVP = m_lightCamera.GetViewProjectionMatrix();
		m_lightCB.shadowCB.lightPos = m_lightCamera.GetPosition();

		//シャドウマップ用レンダーターゲットの初期化
		float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_shadowMapRenderTarget.Create(
			5192,
			5192,
			1,
			1,
			DXGI_FORMAT_R32G32_FLOAT,
			DXGI_FORMAT_D32_FLOAT,
			clearColor
		);

		//シャドウマップ用のガウシアンブラーを初期化
		m_shadowBlur.Init(&m_shadowMapRenderTarget.GetRenderTargetTexture());
	}

	void RenderingEngine::Execute(RenderContext& rc)
	{
		//定数バッファに視点の位置を保存。
		m_lightCB.eyePos = g_camera3D->GetPosition();

		RenderShadowMap(rc);

		DrawForwardRendering(rc);

		m_postEffect.Render(rc, m_mainRenderTarget);

		Draw2D(rc);

		m_renderObjects.clear();
	}

	void RenderingEngine::RenderShadowMap(RenderContext& rc)
	{
		// カメラの位置を設定。
		m_lightCamera.SetPosition(Vector3(g_camera3D->GetPosition() + SHADOW_CAMERA_POS));
		m_lightCamera.SetTarget(g_camera3D->GetPosition());
		m_lightCamera.Update();

		m_lightCB.shadowCB.mLVP = m_lightCamera.GetViewProjectionMatrix();
		m_lightCB.shadowCB.lightPos = m_lightCamera.GetPosition();

		//シャドウマップ用のレンダーターゲットの書き込み待ち。
		rc.WaitUntilToPossibleSetRenderTarget(m_shadowMapRenderTarget);
		rc.SetRenderTargetAndViewport(m_shadowMapRenderTarget);
		rc.ClearRenderTargetView(m_shadowMapRenderTarget);

		//描画処理。
		for (auto& renderObj : m_renderObjects) {
			renderObj->OnRenderShadowMap(rc, m_lightCamera);
		}

		rc.WaitUntilFinishDrawingToRenderTarget(m_shadowMapRenderTarget);

		m_shadowBlur.ExecuteOnGPU(rc, 5.0f);
	}

	void RenderingEngine::DrawForwardRendering(RenderContext& rc)
	{
		////ポータルの数だけオフスクリーンレンダリングを行う。
		//for (int i = 0; i < PORTAL_NUM; i++) {

		//	//ポータル用レンダーターゲットの書き込み待ち。
		//	rc.WaitUntilToPossibleSetRenderTarget(m_portalRenderTarget[i]);
		//	//ポータル用レンダーターゲットの設定。
		//	rc.SetRenderTargetAndViewport(m_portalRenderTarget[i]);
		//	//ポータル用レンダーターゲットをクリア。
		//	rc.ClearRenderTargetView(m_portalRenderTarget[i]);

		//	//オフスクリーンレンダリング。
		//	for (auto& renderObj : m_renderObjects)
		//	{
		//		renderObj->OnForwardRender(rc);
		//	}

		//	//ポータル用レンダーターゲットの書き込み終了待ち。
		//	rc.WaitUntilFinishDrawingToRenderTarget(m_portalRenderTarget[i]);
		//}


		//メインレンダーターゲットの書き込み待ち。
		rc.WaitUntilToPossibleSetRenderTarget(m_mainRenderTarget);
		//メインレンダーターゲットの設定。
		rc.SetRenderTargetAndViewport(m_mainRenderTarget);
		//メインレンダーターゲットの設定。
		rc.ClearRenderTargetView(m_mainRenderTarget);

		//通常描画処理。
		for (auto& renderObj : m_renderObjects)
		{
			renderObj->OnForwardRender(rc);
		}

		//メインレンダーターゲットの書き込み終了待ち。
		rc.WaitUntilFinishDrawingToRenderTarget(m_mainRenderTarget);
	}

	void RenderingEngine::Draw2D(RenderContext& rc)
	{
		//レンダリングターゲット書き込み待ち。
		rc.WaitUntilToPossibleSetRenderTarget(m_2DRenderTarget);
		//レンダリングターゲットの設定。
		rc.SetRenderTargetAndViewport(m_2DRenderTarget);
		//レンダリングターゲットをクリア。
		rc.ClearRenderTargetView(m_2DRenderTarget);

		m_mainSprite.Draw(rc);

		//描画処理。
		for (auto& renderObj : m_renderObjects)
		{
			renderObj->On2DRender(rc);
		}

		rc.WaitUntilFinishDrawingToRenderTarget(m_2DRenderTarget);

		//レンダーターゲットの切り替え。
		rc.SetRenderTarget(
			g_graphicsEngine->GetCurrentFrameBuffuerRTV(),
			g_graphicsEngine->GetCurrentFrameBuffuerDSV()
		);

		rc.WaitUntilToPossibleSetRenderTarget(m_mainRenderTarget);

		//最終2Dスプライトの描画。
		m_2DSprite.Draw(rc);

		rc.WaitUntilFinishDrawingToRenderTarget(m_mainRenderTarget);
	}
}