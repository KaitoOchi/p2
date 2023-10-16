#pragma once
#include "Bloom.h"
#include "DoF.h"

namespace nsPortalEngine {

	/// <summary>
	/// ポストエフェクト。
	/// </summary>
	class PostEffect : Noncopyable
	{
	public:
		/// <summary>
		/// ポストエフェクトの種類。
		/// </summary>
		enum PostEffectType
		{
			enPostEffect_Bloom,		//ブルーム。
			enPostEffect_DoF,		//被写界深度。
			enPostEffect_Num,
		};

	public:
		PostEffect();
		~PostEffect();

		/// <summary>
		/// 初期化処理。
		/// </summary>
		/// <param name="mainRenderTarget"></param>
		void Init(RenderTarget& mainRenderTarget, RenderTarget& zprepassRenderTarget);

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
			m_bloom->SetThreshold(value);
		}

		/// <summary>
		/// ブルームの閾値を取得。
		/// </summary>
		/// <returns></returns>
		const float GetBloomThreshold()
		{
			return m_bloom->GetThreshold();
		}

		/// <summary>
		/// 有効化。
		/// </summary>
		/// <param name="entype"></param>
		void Enable(const PostEffectType entype)
		{
			int num = static_cast<int>(entype);
			m_postEffectType[num]->Enable();
		}

		/// <summary>
		/// 無効化。
		/// </summary>
		/// <param name="entype"></param>
		void Disable(const PostEffectType entype)
		{
			int num = static_cast<int>(entype);
			m_postEffectType[num]->Disable();
		}

	private:
		std::vector<PostEffectComponentBase*>	m_postEffectType;		//ポストエフェクトの配列。
		Bloom*									m_bloom = nullptr;		//ブルーム。
		DoF*									m_dof = nullptr;		//被写界深度。
	};
}

