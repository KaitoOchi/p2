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
		//レンダーターゲットの初期化。
		m_luminanceRenderTarget.Create(
			mainRenderTarget.GetWidth(),
			mainRenderTarget.GetHeight(),
			1,
			1,
			mainRenderTarget.GetColorBufferFormat(),
			DXGI_FORMAT_D32_FLOAT
		);

		//ガウシアンブラーを初期化。
		m_gaussianBlur[0].Init(&m_luminanceRenderTarget.GetRenderTargetTexture());
		m_gaussianBlur[1].Init(&m_gaussianBlur[0].GetBokeTexture());
		m_gaussianBlur[2].Init(&m_gaussianBlur[1].GetBokeTexture());
		m_gaussianBlur[3].Init(&m_gaussianBlur[2].GetBokeTexture());

		//輝度抽出用のスプライトを初期化。
		InitLuminanceSprite(mainRenderTarget);

		//加算合成用スプライトを初期化。
		InitFinalSprite(mainRenderTarget);

		//メインレンダリングターゲットに合成用のスプライトを初期化。
		SpriteInitData initData;
		initData.m_width = mainRenderTarget.GetWidth();
		initData.m_height = mainRenderTarget.GetHeight();
		initData.m_fxFilePath = "Assets/shader/Sprite.fx";
		initData.m_textures[0] = &mainRenderTarget.GetRenderTargetTexture();
		m_copyMainRtSprite.Init(initData);
	}

	void Bloom::InitLuminanceSprite(RenderTarget& mainRenderTarget)
	{
		//輝度抽出用のスプライトを設定。
		SpriteInitData spriteInitData;
		spriteInitData.m_fxFilePath = "Assets/shader/postEffect/Bloom.fx";
		spriteInitData.m_vsEntryPointFunc = "VSMain";
		spriteInitData.m_psEntryPoinFunc = "PSLuminance";

		spriteInitData.m_expandConstantBuffer = &m_luminanceCB;
		spriteInitData.m_expandConstantBufferSize = sizeof(m_luminanceCB);

		spriteInitData.m_width = mainRenderTarget.GetWidth();
		spriteInitData.m_height = mainRenderTarget.GetHeight();
		spriteInitData.m_textures[0] = &mainRenderTarget.GetRenderTargetTexture();
		// 描き込むレンダリングターゲットのフォーマットを指定する。
		spriteInitData.m_colorBufferFormat[0] = mainRenderTarget.GetColorBufferFormat();

		//輝度抽出用のスプライトを初期化。
		m_luminanceSprite.Init(spriteInitData);
	}

	void Bloom::InitFinalSprite(RenderTarget& mainRenderTarget)
	{
		//ボケ画像を合成して書き込むためのスプライトを初期化。
		SpriteInitData finalSpriteInitData;
		finalSpriteInitData.m_textures[0] = &m_gaussianBlur[0].GetBokeTexture();
		finalSpriteInitData.m_textures[1] = &m_gaussianBlur[1].GetBokeTexture();
		finalSpriteInitData.m_textures[2] = &m_gaussianBlur[2].GetBokeTexture();
		finalSpriteInitData.m_textures[3] = &m_gaussianBlur[3].GetBokeTexture();

		// スプライトの幅と高さはluminnceRenderTargetと同じ。
		finalSpriteInitData.m_width = mainRenderTarget.GetWidth();
		finalSpriteInitData.m_height = mainRenderTarget.GetHeight();

		finalSpriteInitData.m_fxFilePath = "Assets/shader/postEffect/Bloom.fx";
		finalSpriteInitData.m_psEntryPoinFunc = "PSBloomFinal";

		//アルファブレンディングモードを加算にする。
		finalSpriteInitData.m_alphaBlendMode = AlphaBlendMode_Add;
		finalSpriteInitData.m_colorBufferFormat[0] = mainRenderTarget.GetColorBufferFormat();

		//加算合成用のスプライトを初期化。
		m_finalSprite.Init(finalSpriteInitData);
	}

	void Bloom::OnRender(RenderContext& rc, RenderTarget& mainRenderTarget)
	{
		// 輝度抽出
		// 輝度抽出用のレンダリングターゲットに変更
		rc.WaitUntilToPossibleSetRenderTarget(m_luminanceRenderTarget);

		// レンダリングターゲットを設定
		rc.SetRenderTargetAndViewport(m_luminanceRenderTarget);

		// レンダリングターゲットをクリア
		rc.ClearRenderTargetView(m_luminanceRenderTarget);

		// 輝度抽出を行う
		m_luminanceSprite.Draw(rc);

		// レンダリングターゲットへの書き込み終了待ち
		rc.WaitUntilFinishDrawingToRenderTarget(m_luminanceRenderTarget);

		m_gaussianBlur[0].ExecuteOnGPU(rc, BLUR_POWER);
		m_gaussianBlur[1].ExecuteOnGPU(rc, BLUR_POWER);
		m_gaussianBlur[2].ExecuteOnGPU(rc, BLUR_POWER);
		m_gaussianBlur[3].ExecuteOnGPU(rc, BLUR_POWER);

		// 4枚のボケ画像を合成してメインレンダリングターゲットに加算合成
		// レンダリングターゲットとして利用できるまで待つ
		rc.WaitUntilToPossibleSetRenderTarget(mainRenderTarget);
		// レンダリングターゲットを設定
		rc.SetRenderTargetAndViewport(mainRenderTarget);

		// 最終合成
		m_finalSprite.Draw(rc);

		// レンダリングターゲットへの書き込み終了待ち
		rc.WaitUntilFinishDrawingToRenderTarget(mainRenderTarget);

		// メインレンダリングターゲットの絵をフレームバッファーにコピー
		rc.SetRenderTarget(
			g_graphicsEngine->GetCurrentFrameBuffuerRTV(),
			g_graphicsEngine->GetCurrentFrameBuffuerDSV()
		);

		// ポストエフェクトの結果をメインレンダリングターゲットに反映。
		m_copyMainRtSprite.Draw(rc);
	}
}