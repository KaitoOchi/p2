#pragma once
#include "Bloom.h"

namespace nsPortalEngine {

	/// <summary>
	/// ポストエフェクト。
	/// </summary>
	class PostEffect : Noncopyable
	{
	public:
		PostEffect();
		~PostEffect();

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
		/// ブルームの閾値を設定。
		/// </summary>
		/// <param name="value"></param>
		void SetBloomThreshold(const float value)
		{
			m_bloom.SetThreshold(value);
		}

		/// <summary>
		/// ブルームの閾値を取得。
		/// </summary>
		/// <returns></returns>
		const float GetBloomThreshold()
		{
			return m_bloom.GetThreshold();
		}

	private:
		Bloom m_bloom;
	};
}

