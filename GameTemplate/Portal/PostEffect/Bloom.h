#pragma once

namespace nsPortalEngine {

	namespace
	{
		const int GAUSSIANBLUE_NUM = 4;		//ガウシアンブラーの数。
		const float BLUR_POWER = 5.0f;		//ブラーの力。
	}

	class Bloom : Noncopyable
	{
	public:
		/// <summary>
		/// ブルーム用構造体。
		/// </summary>
		struct LuminanceCB
		{
			float threshold = 1.0f;
		};

	public:
		Bloom();
		~Bloom();

		/// <summary>
		/// 初期化処理。
		/// </summary>
		/// <param name="mainRenderTarget"></param>
		void Init(RenderTarget& mainRenderTarget);

		/// <summary>
		/// 描画処理。
		/// </summary>
		/// <param name="rc"></param>
		/// <param name="mainRenderTarget"></param>
		void Render(RenderContext& rc, RenderTarget& mainRenderTarget);

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
		RenderTarget	m_luminanceRenderTarget;			//ブルーム用レンダーターゲット。
		GaussianBlur	m_gaussianBlur[GAUSSIANBLUE_NUM];	//ガウシアンブラー。
		LuminanceCB		m_luminanceCB;						//ブルーム用定数バッファ。
		Sprite			m_luminanceSprite;					//ブルーム用スプライト。
		Sprite			m_finalSprite;						//最終合成用のスプライト。
		Sprite			m_copyMainRtSprite;					//メインレンダリングターゲットに描画するためのスプライト。
	};

}
