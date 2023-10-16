#pragma once
#include "PostEffect/PostEffectComponentBase.h"

namespace nsPortalEngine {

	namespace
	{
		const int GAUSSIANBLUE_NUM = 4;		//ガウシアンブラーの数。
		const float BLUR_POWER = 3.0f;		//ブラーの力。
	}

	/// <summary>
	/// ブルーム。
	/// </summary>
	class Bloom : public PostEffectComponentBase
	{
	public:
		/// <summary>
		/// ブルーム用構造体。
		/// </summary>
		struct LuminanceCB
		{
			float threshold = 1.0f;		//閾値。
		};

	public:
		Bloom();
		virtual ~Bloom();

		/// <summary>
		/// 初期化処理。
		/// </summary>
		/// <param name="mainRenderTarget"></param>
		void OnInit(RenderTarget& mainRenderTarget, RenderTarget& zprepassRenderTarget) override;

		/// <summary>
		/// 描画処理。
		/// </summary>
		/// <param name="rc"></param>
		/// <param name="mainRenderTarget"></param>
		void OnRender(RenderContext& rc, RenderTarget& mainRenderTarget) override;

		/// <summary>
		/// 閾値を設定。
		/// </summary>
		/// <param name="value"></param>
		void SetThreshold(const float value)
		{
			m_luminanceCB.threshold = value;
		}

		/// <summary>
		/// 閾値を取得。
		/// </summary>
		/// <returns></returns>
		const float GetThreshold()
		{
			return m_luminanceCB.threshold;
		}

	private:
		/// <summary>
		/// 輝度抽出用のスプライトを初期化。
		/// </summary>
		/// <param name="mainRenderTarget"></param>
		void InitLuminanceSprite(RenderTarget& mainRenderTarget);

		/// <summary>
		/// 最終合成用のスプライトを初期化。
		/// </summary>
		/// <param name="mainRenderTarget"></param>
		void InitFinalSprite(RenderTarget& mainRenderTarget);

	private:
		RenderTarget	m_luminanceRenderTarget;			//輝度抽出用レンダーターゲット。
		GaussianBlur	m_gaussianBlur[GAUSSIANBLUE_NUM];	//ガウシアンブラー。
		LuminanceCB		m_luminanceCB;						//ブルーム用定数バッファ。
		Sprite			m_luminanceSprite;					//輝度抽出用スプライト。
		Sprite			m_finalSprite;						//最終合成用のスプライト。
		Sprite			m_copyMainRtSprite;					//メインレンダリングターゲットに描画するためのスプライト。
	};

}
