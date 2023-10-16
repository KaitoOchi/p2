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
		//深度値用のガウシアンブラーを初期化。
		m_depthGaussianBlur.Init(&mainRenderTarget.GetRenderTargetTexture());

		//ボケ画像合成用のスプライトを初期化する。
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
		//メインレンダーターゲットのボケ画像を作成。
		m_depthGaussianBlur.ExecuteOnGPU(rc, 5.0f);

		//レンダーターゲットの書き込み待ち。
		rc.WaitUntilToPossibleSetRenderTarget(mainRenderTarget);
		//メインレンダーターゲットの設定。
		rc.SetRenderTargetAndViewport(mainRenderTarget);

		m_combineBokeImageSprite.Draw(rc);

		//メインレンダーターゲットの書き込み終了待ち。
		rc.WaitUntilFinishDrawingToRenderTarget(mainRenderTarget);
	}
}