#pragma once
#include "PostEffect/PostEffectComponentBase.h"

namespace nsPortalEngine {

	/// <summary>
	/// ��ʊE�[�x�B
	/// </summary>
	class DoF : public PostEffectComponentBase
	{
	public:
		DoF();
		~DoF();

		/// <summary>
		/// �����������B
		/// </summary>
		/// <param name="mainRenderTarget"></param>
		void OnInit(RenderTarget* mainRenderTarget, RenderTarget& zprepassRenderTarget) override;

		/// <summary>
		/// �`�揈���B
		/// </summary>
		/// <param name="rc"></param>
		/// <param name="mainRenderTarget"></param>
		void OnRender(RenderContext& rc, RenderTarget* mainRenderTarget) override;

	private:
		Sprite					m_combineBokeImageSprite;			//�[�x�l�p�X�v���C�g�B
		GaussianBlur			m_depthGaussianBlur;				//�[�x�l�p�K�E�V�A���u���[�B
	};
}
