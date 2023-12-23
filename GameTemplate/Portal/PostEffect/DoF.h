#pragma once
#include "PostEffect/PostEffectComponentBase.h"

namespace nsPortalEngine {

	/// <summary>
	/// 被写界深度。
	/// </summary>
	class DoF : public PostEffectComponentBase
	{
	public:
		DoF();
		~DoF();

		/// <summary>
		/// 初期化処理。
		/// </summary>
		/// <param name="mainRenderTarget"></param>
		void OnInit(RenderTarget* mainRenderTarget, RenderTarget& zprepassRenderTarget) override;

		/// <summary>
		/// 描画処理。
		/// </summary>
		/// <param name="rc"></param>
		/// <param name="mainRenderTarget"></param>
		void OnRender(RenderContext& rc, RenderTarget* mainRenderTarget) override;

	private:
		Sprite					m_combineBokeImageSprite;			//深度値用スプライト。
		GaussianBlur			m_depthGaussianBlur;				//深度値用ガウシアンブラー。
	};
}
