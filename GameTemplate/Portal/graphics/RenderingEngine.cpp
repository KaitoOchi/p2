#include "PortalEnginePreCompile.h"
#include "RenderingEngine.h"

namespace nsPortalEngine {

	namespace
	{
		const Vector3	SHADOW_CAMERA_POS = Vector3(200.0f, 3000.0f, 200.0f);		//シャドウカメラの位置。
		const int		SHADOW_TEXTURE_SIZE = 4096.0f;								//シャドウテクスチャのサイズ。
		const float		RENDER_TARGET_CLEAR_COLOR[4] = { 0.5f, 0.5f, 0.5f, 1.0f };	//クリアカラー。
		const float		SHADOW_BLUR_POWER = 5.0f;									//シャドウブラーの強さ。
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
		//ディレクションライトの設定。
		m_lightCB.directionLig.ligDirection = Vector3(1.0f, -1.0f, 1.0f);
		m_lightCB.directionLig.ligColor = Vector3(0.2f, 0.1f, 0.1f);
		m_lightCB.directionLig.ambient = 0.5f;

		InitRenderTarget();

		InitDeferredLightingSprite();

		//ポストエフェクトの初期化。
		m_postEffect.Init(m_mainRenderTarget, m_zprepassRenderTarget);
		m_postEffect.SetBloomThreshold(3.0f);
	}

	void RenderingEngine::InitDeferredLightingSprite()
	{
		SpriteInitData spriteInitData;
		spriteInitData.m_width = g_graphicsEngine->GetFrameBufferWidth();
		spriteInitData.m_height = g_graphicsEngine->GetFrameBufferHeight();

		// ディファードライティングで使用するテクスチャを設定。
		for (int i = 0; i < enGBufferNum; i++) {
			spriteInitData.m_textures[i] = &m_gBuffer[0][i].GetRenderTargetTexture();
		}
		spriteInitData.m_textures[enGBufferNum] = &m_shadowBlur.GetBokeTexture();

		spriteInitData.m_fxFilePath = "Assets/shader/DeferredLighting.fx";
		spriteInitData.m_expandConstantBuffer = &m_lightCB;
		spriteInitData.m_expandConstantBufferSize = sizeof(m_lightCB);
		spriteInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		// 初期化データを使ってスプライトを作成。
		m_deferredLightingSprite.Init(spriteInitData);
	}

	void RenderingEngine::InitRenderTarget()
	{
		InitMainRenderTarget();

		InitZPrepassRenderTarget();

		InitShadowMapRenderTarget();

		InitGBuffer();

		InitPortalRenderTarget();

		Init2DRenderTarget();
	}

	void RenderingEngine::InitGBuffer()
	{
		int frameBufferW = g_graphicsEngine->GetFrameBufferWidth();
		int frameBufferH = g_graphicsEngine->GetFrameBufferHeight();

		//メイン+ポータルの数だけ作成する。
		for (int i = 0; i < 1 + PORTAL_NUM; i++) {
			//アルベドカラー出力用レンダーターゲットの作成。
			m_gBuffer[i][enGBufferAlbedoDepth].Create(
				frameBufferW,
				frameBufferH,
				1,
				1,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				DXGI_FORMAT_D32_FLOAT,
				const_cast<float*>(RENDER_TARGET_CLEAR_COLOR)
			);

			//法線出力用レンダーターゲットの作成。
			m_gBuffer[i][enGBufferNormal].Create(
				frameBufferW,
				frameBufferH,
				1,
				1,
				DXGI_FORMAT_R8G8B8A8_SNORM,
				DXGI_FORMAT_UNKNOWN
			);


			//メタリック、シャドウ、スムース出力用レンダーターゲットの作成。
			m_gBuffer[i][enGBufferMetaricShadowSmooth].Create(
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
		//メインレンダーターゲットを設定。
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
		//2D用レンダーターゲットを設定。
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

		//最終合成用スプライトの初期化。
		SpriteInitData finalSpriteInitData;
		finalSpriteInitData.m_textures[0] = &m_2DRenderTarget.GetRenderTargetTexture();
		finalSpriteInitData.m_width = m_mainRenderTarget.GetWidth();
		finalSpriteInitData.m_height = m_mainRenderTarget.GetHeight();
		finalSpriteInitData.m_fxFilePath = "Assets/shader/sprite.fx";
		finalSpriteInitData.m_alphaBlendMode = AlphaBlendMode_None;
		finalSpriteInitData.m_colorBufferFormat[0] = m_mainRenderTarget.GetColorBufferFormat();

		m_2DSprite.Init(finalSpriteInitData);

		//2D統合用スプライトの初期化。
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
				m_mainRenderTarget.GetColorBufferFormat(),
				DXGI_FORMAT_D32_FLOAT,
				const_cast<float*>(RENDER_TARGET_CLEAR_COLOR)
			);
		}

		SpriteInitData spriteInitData;
		spriteInitData.m_width = g_graphicsEngine->GetFrameBufferWidth();
		spriteInitData.m_height = g_graphicsEngine->GetFrameBufferHeight();

		spriteInitData.m_fxFilePath = "Assets/shader/DeferredLighting.fx";
		spriteInitData.m_expandConstantBuffer = &m_lightCB;
		spriteInitData.m_expandConstantBufferSize = sizeof(m_lightCB);
		spriteInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		for (int i = 0; i < PORTAL_NUM; i++) {
			// ディファードライティングで使用するテクスチャを設定。
			for (int j = 0; j < enGBufferNum; j++) {
				spriteInitData.m_textures[j] = &m_gBuffer[i + 1][j].GetRenderTargetTexture();
			}
			//影テクスチャ。
			spriteInitData.m_textures[enGBufferNum] = &m_shadowBlur.GetBokeTexture();

			// 初期化データを使ってスプライトを作成。
			m_portalSprite[i].Init(spriteInitData);
		}
	}

	void RenderingEngine::InitShadowMapRenderTarget()
	{
		//カメラの設定。
		//近平面を設定。
		m_lightCamera.SetNear(10.0f);
		//遠平面を設定。
		m_lightCamera.SetFar(5000.0f);
		//上方向を設定。
		m_lightCamera.SetUp(1, 0, 0);
		//射影行列の更新方法を設定。
		m_lightCamera.SetUpdateProjMatrixFunc(Camera::enUpdateProjMatrixFunc_Ortho);

		//シャドウマップ用レンダーターゲットの初期化。
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

		//シャドウマップ用のガウシアンブラーを初期化。
		m_shadowBlur.Init(&m_shadowMapRenderTarget.GetRenderTargetTexture());
	}

	void RenderingEngine::InitZPrepassRenderTarget()
	{
		//ZPrepass用レンダーターゲットの初期化。
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
		//定数バッファに視点の位置を保存。
		m_lightCB.eyePos = g_camera3D->GetPosition() + Vector3(0.0f, 0.0f, 0.0f);

		//シャドウ用カメラの位置を設定。
		m_lightCamera.SetPosition(g_camera3D->GetPosition() + SHADOW_CAMERA_POS);
		m_lightCamera.SetTarget(g_camera3D->GetPosition());
		m_lightCamera.Update();

		//定数バッファにカメラのプロジェクション行列と座標を保存。
		m_lightCB.shadowCB.mLVP = m_lightCamera.GetViewProjectionMatrix();
		m_lightCB.shadowCB.lightPos = m_lightCamera.GetTarget();
		//ビュープロジェクション行列の逆行列を保存。
		m_lightCB.mViewProjInv.Inverse(g_camera3D->GetViewProjectionMatrix());
	}

	void RenderingEngine::Execute(RenderContext& rc)
	{
		//定数バッファの更新。
		UpdateCB();

		//ZPrepassの描画。
		DrawZPrepass(rc);

		//シャドウマップの描画。
		DrawShadowMap(rc);

		//GBufferの描画。
		DrawGBuffer(rc);

		//ディファードライティング。
		DrawDeferredLighting(rc);

		//フォワードライティング。
		DrawForwardLighting(rc);

		//ポストエフェクト。
		m_postEffect.Render(rc, m_mainRenderTarget);

		//2Dの描画。
		Draw2D(rc);

		m_renderObjects.clear();
	}

	void RenderingEngine::DrawZPrepass(RenderContext& rc)
	{
		// レンダリングターゲットとして設定できるようになるまで待つ。
		rc.WaitUntilToPossibleSetRenderTarget(m_zprepassRenderTarget);
		// レンダリングターゲットを設定。
		rc.SetRenderTargetAndViewport(m_zprepassRenderTarget);
		// レンダリングターゲットをクリア。
		rc.ClearRenderTargetView(m_zprepassRenderTarget);

		for (auto& renderObj : m_renderObjects) {
			renderObj->OnRenderToZPrepass(rc);
		}

		//レンダーターゲットの書き込み終了待ち。
		rc.WaitUntilFinishDrawingToRenderTarget(m_zprepassRenderTarget);
	}

	void RenderingEngine::DrawShadowMap(RenderContext& rc)
	{
		//シャドウマップ用レンダーターゲットの書き込み待ち。
		rc.WaitUntilToPossibleSetRenderTarget(m_shadowMapRenderTarget);
		// レンダリングターゲットを設定。
		rc.SetRenderTargetAndViewport(m_shadowMapRenderTarget);
		// レンダリングターゲットをクリア。
		rc.ClearRenderTargetView(m_shadowMapRenderTarget);

		//描画処理。
		for (auto& renderObj : m_renderObjects) {
			renderObj->OnRenderShadowMap(rc, m_lightCamera);
		}

		//レンダーターゲットの書き込み終了待ち。
		rc.WaitUntilFinishDrawingToRenderTarget(m_shadowMapRenderTarget);

		m_shadowBlur.ExecuteOnGPU(rc, SHADOW_BLUR_POWER);
	}

	void RenderingEngine::DrawGBuffer(RenderContext& rc)
	{
		//メイン+ポータルの数だけ描画する。
		for (int i = PORTAL_NUM; i >= 0; i--) {

			RenderTarget* rts[enGBufferNum] = {
				&m_gBuffer[i][enGBufferAlbedoDepth],
				&m_gBuffer[i][enGBufferNormal],
				&m_gBuffer[i][enGBufferMetaricShadowSmooth]
			};

			int rtsNum = static_cast<int>(enGBufferNum);

			//レンダーターゲットの書き込み待ち。
			rc.WaitUntilToPossibleSetRenderTargets(rtsNum, rts);
			//レンダーターゲットの設定。
			rc.SetRenderTargetsAndViewport(rtsNum, rts);
			//レンダーターゲットをクリア。
			rc.ClearRenderTargetViews(rtsNum, rts);

			if (i != 0) {
				//ポータル用の描画。
				for (auto& renderObj : m_renderObjects)
				{
					renderObj->OnPortalRender(rc, i - 1, m_portalCamera->GetPortalCamera(i - 1));
				}
			}
			else {
				//メインのGBufferを描画。
				for (auto& renderObj : m_renderObjects)
				{
					renderObj->OnRenderGBuffer(rc);
				}
			}

			//レンダーターゲットの書き込み終了待ち。
			rc.WaitUntilFinishDrawingToRenderTargets(rtsNum, rts);
		}
	}

	void RenderingEngine::DrawDeferredLighting(RenderContext& rc)
	{
		//ポータルの数だけスプライトの描画を行う。
		for (int i = 0; i < PORTAL_NUM; i++) {

			//ポータル用レンダーターゲットの書き込み待ち。
			rc.WaitUntilToPossibleSetRenderTarget(m_portalRenderTarget[i]);
			//ポータル用レンダーターゲットの設定。
			rc.SetRenderTargetAndViewport(m_portalRenderTarget[i]);
			//ポータル用レンダーターゲットをクリア。
			rc.ClearRenderTargetView(m_portalRenderTarget[i]);

			//ポータル用スプライトの描画。
			m_portalSprite[i].Draw(rc);

			//ポータル用レンダーターゲットの書き込み終了待ち。
			rc.WaitUntilFinishDrawingToRenderTarget(m_portalRenderTarget[i]);
		}

		//メインレンダーターゲットの書き込み待ち。
		rc.WaitUntilToPossibleSetRenderTarget(m_mainRenderTarget);
		//メインレンダーターゲットの設定。
		rc.SetRenderTargetAndViewport(m_mainRenderTarget);
		//メインレンダーターゲットの設定。
		rc.ClearRenderTargetView(m_mainRenderTarget);

		// G-Bufferの内容を元にしてディファードライティング
		m_deferredLightingSprite.Draw(rc);

		//メインレンダーターゲットの書き込み終了待ち。
		rc.WaitUntilFinishDrawingToRenderTarget(m_mainRenderTarget);
	}

	void RenderingEngine::DrawForwardLighting(RenderContext& rc)
	{
		//メインレンダーターゲットの書き込み待ち。
		rc.WaitUntilToPossibleSetRenderTarget(m_mainRenderTarget);
		//メインレンダーターゲットの設定。
		rc.SetRenderTargetAndViewport(m_mainRenderTarget);

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

		//2D画像の描画処理。
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

		//最終画面の描画。
		m_2DSprite.Draw(rc);

		rc.WaitUntilFinishDrawingToRenderTarget(m_mainRenderTarget);
	}
}