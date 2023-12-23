#include "PortalEnginePreCompile.h"
#include "RenderingEngine.h"

namespace nsPortalEngine {

	namespace
	{
		const Vector3	SHADOW_CAMERA_POS = Vector3(200.0f, 3000.0f, 200.0f);		//シャドウカメラの位置。
		const int		SHADOW_TEXTURE_SIZE = 1024;								//シャドウテクスチャのサイズ。
		const float		RENDER_TARGET_CLEAR_COLOR[4] = { 0.5f, 0.5f, 0.5f, 0.0f };	//クリアカラー。
		const float		SHADOW_BLUR_POWER = 5.0f;									//シャドウブラーの強さ。
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
		//ディレクションライトの設定。
		m_lightCB.directionLig.ligDirection = Vector3(0.0f, -1.0f, 0.0f);
		m_lightCB.directionLig.ligColor = Vector3(0.1f, 0.1f, 0.1f);
		m_lightCB.directionLig.ambient = 0.2f;

		InitRenderTarget();

		InitDeferredLightingSprite();

		//ポストエフェクトの初期化。
		m_postEffect.Init(m_mainRenderTarget, m_zprepassRenderTarget[0]);
		m_postEffect.SetBloomThreshold(3.0f);
		m_postEffect.Disable(PostEffect::enPostEffect_DoF);

		//ライトカリングの初期化。
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

		//影テクスチャ。
		spriteInitData.m_textures[enGBufferNum] = &m_shadowBlur.GetBokeTexture();
		//ポータルテクスチャ。
		spriteInitData.m_textures[enGBufferNum + 1] = &m_mainRenderTarget[1].GetRenderTargetTexture();
		spriteInitData.m_textures[enGBufferNum + 2] = &m_mainRenderTarget[2].GetRenderTargetTexture();

		spriteInitData.m_fxFilePath = "Assets/shader/DeferredLighting.fx";
		spriteInitData.m_expandConstantBuffer = &m_lightCB;
		spriteInitData.m_expandConstantBufferSize = sizeof(m_lightCB);
		spriteInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;


		for (int i = 0; i <= PORTAL_NUM; i++) {
			//タイルごとのポイントライトの番号のリストを出力するUAVを初期化。
			m_pointLightNoListInTileUAV[i].Init(
				sizeof(int),
				POINT_LIGHT_NUM * TILE_NUM,
				nullptr
			);
			//タイルごとのスポットライトの番号のリストを出力するUAVを初期化。
			m_spotLightNoListInTileUAV[i].Init(
				sizeof(int),
				SPOT_LIGHT_NUM * TILE_NUM,
				nullptr
			);

			//ポイントライトとスポットライトのUAV。
			spriteInitData.m_expandShaderResoruceView[0] = &m_pointLightNoListInTileUAV[i];
			spriteInitData.m_expandShaderResoruceView[1] = &m_spotLightNoListInTileUAV[i];

			// ディファードライティングで使用するテクスチャを設定。
			for (int gBufferNum = 0; gBufferNum < enGBufferNum; gBufferNum++) {
				spriteInitData.m_textures[gBufferNum] = &m_gBuffer[i][gBufferNum].GetRenderTargetTexture();
			}

			spriteInitData.m_psEntryPoinFunc = DEFERREDLIGHTING_SPRITE_PS[i];

			// 初期化データを使ってスプライトを作成。
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

		//メイン+ポータルの数だけ作成する。
		for (int i = 0; i <= PORTAL_NUM; i++) {
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


			//メタリックスムース出力用レンダーターゲットの作成。
			m_gBuffer[i][enGBufferMetaricShadowSmooth].Create(
				frameBufferW,
				frameBufferH,
				1,
				1,
				DXGI_FORMAT_R8G8B8A8_UNORM,
				DXGI_FORMAT_UNKNOWN
			);

			//パラメータ出力用レンダーターゲットの作成。
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

		//メインレンダーターゲットを設定。
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
		finalSpriteInitData.m_width = m_mainRenderTarget[0].GetWidth();
		finalSpriteInitData.m_height = m_mainRenderTarget[0].GetHeight();
		finalSpriteInitData.m_fxFilePath = "Assets/shader/sprite.fx";
		finalSpriteInitData.m_alphaBlendMode = AlphaBlendMode_None;
		finalSpriteInitData.m_colorBufferFormat[0] = m_mainRenderTarget[0].GetColorBufferFormat();

		m_2DSprite.Init(finalSpriteInitData);

		//2D統合用スプライトの初期化。
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
		//カメラの設定。
		//近平面を設定。
		m_lightCamera.SetNear(10.0f);
		//遠平面を設定。
		m_lightCamera.SetFar(5000.0f);
		//上方向を設定。
		m_lightCamera.SetUp(0, 1, 0);
		//射影行列の更新方法を設定。
		m_lightCamera.SetUpdateProjMatrixFunc(Camera::enUpdateProjMatrixFunc_Ortho);
		m_lightCamera.SetWidth(SHADOW_TEXTURE_SIZE * 2);
		m_lightCamera.SetHeight(SHADOW_TEXTURE_SIZE * 2);
		m_lightCamera.Update();

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
		for (int i = 0; i <= PORTAL_NUM; i++) {
			//ZPrepass用レンダーターゲットの初期化。
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
		//定数バッファに視点の位置を保存。
		m_lightCB.eyePos = g_camera3D->GetPosition();
		m_lightCB.eyePosBlue = m_portalCamera->GetPortalCamera(0).GetPosition();
		m_lightCB.eyePosRed = m_portalCamera->GetPortalCamera(1).GetPosition();

		//シャドウ用カメラの位置を設定。
		m_lightCamera.SetPosition(g_camera3D->GetPosition() + SHADOW_CAMERA_POS);
		m_lightCamera.SetTarget(g_camera3D->GetPosition());
		m_lightCamera.Update();

		//定数バッファにカメラのプロジェクション行列と座標を保存。
		m_lightCB.shadowCB.mLVP = m_lightCamera.GetViewProjectionMatrix();
		m_lightCB.shadowCB.lightPos = m_lightCamera.GetTarget();
		//ビュープロジェクション行列の逆行列を保存。
		m_lightCB.mViewProjInv[0].Inverse(g_camera3D->GetViewProjectionMatrix());
		m_lightCB.mViewProjInv[1].Inverse(m_portalCamera->GetPortalCamera(0).GetViewProjectionMatrix());
		m_lightCB.mViewProjInv[2].Inverse(m_portalCamera->GetPortalCamera(1).GetViewProjectionMatrix());
	}

	void RenderingEngine::Execute(RenderContext& rc)
	{
		//定数バッファの更新。
		UpdateCB();

		//ZPrepassの描画。
		DrawZPrepass(rc);

		//シャドウマップの描画。
		DrawShadowMap(rc);

		for (int i = 0; i <= PORTAL_NUM; i++) {
			m_lightCulling[i].Execute(rc);
		}

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
		for (int i = 0; i <= PORTAL_NUM; i++) {
			// レンダリングターゲットとして設定できるようになるまで待つ。
			rc.WaitUntilToPossibleSetRenderTarget(m_zprepassRenderTarget[i]);
			// レンダリングターゲットを設定。
			rc.SetRenderTargetAndViewport(m_zprepassRenderTarget[i]);
			// レンダリングターゲットをクリア。
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

			//レンダーターゲットの書き込み終了待ち。
			rc.WaitUntilFinishDrawingToRenderTarget(m_zprepassRenderTarget[i]);
		}
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
				&m_gBuffer[i][enGBufferMetaricShadowSmooth],
				&m_gBuffer[i][enGBufferShadowLightParam]
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
		for (int i = 0; i <= PORTAL_NUM; i++) {

			//メインレンダーターゲットの書き込み待ち。
			rc.WaitUntilToPossibleSetRenderTarget(m_mainRenderTarget[i]);
			//メインレンダーターゲットの設定。
			rc.SetRenderTargetAndViewport(m_mainRenderTarget[i]);
			//メインレンダーターゲットの設定。
			rc.ClearRenderTargetView(m_mainRenderTarget[i]);

			// G-Bufferの内容を元にしてディファードライティング
			m_deferredLightingSprite[i].Draw(rc);

			//メインレンダーターゲットの書き込み終了待ち。
			rc.WaitUntilFinishDrawingToRenderTarget(m_mainRenderTarget[i]);
		}
	}

	void RenderingEngine::DrawForwardLighting(RenderContext& rc)
	{
		//メインレンダーターゲットの書き込み待ち。
		rc.WaitUntilToPossibleSetRenderTarget(m_mainRenderTarget[0]);
		//メインレンダーターゲットの設定。
		rc.SetRenderTargetAndViewport(m_mainRenderTarget[0]);

		//通常描画処理。
		for (auto& renderObj : m_renderObjects)
		{
			renderObj->OnForwardRender(rc);
		}

		//メインレンダーターゲットの書き込み終了待ち。
		rc.WaitUntilFinishDrawingToRenderTarget(m_mainRenderTarget[0]);
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

		rc.WaitUntilToPossibleSetRenderTarget(m_mainRenderTarget[0]);

		//最終画面の描画。
		m_2DSprite.Draw(rc);

		rc.WaitUntilFinishDrawingToRenderTarget(m_mainRenderTarget[0]);
	}
}