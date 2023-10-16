#pragma once
#include "PostEffect/PostEffectComponentBase.h"

namespace nsPortalEngine {

	namespace
	{
		const int GAUSSIANBLUE_NUM = 4;		//�K�E�V�A���u���[�̐��B
		const float BLUR_POWER = 3.0f;		//�u���[�̗́B
	}

	/// <summary>
	/// �u���[���B
	/// </summary>
	class Bloom : public PostEffectComponentBase
	{
	public:
		/// <summary>
		/// �u���[���p�\���́B
		/// </summary>
		struct LuminanceCB
		{
			float threshold = 1.0f;		//臒l�B
		};

	public:
		Bloom();
		virtual ~Bloom();

		/// <summary>
		/// �����������B
		/// </summary>
		/// <param name="mainRenderTarget"></param>
		void OnInit(RenderTarget& mainRenderTarget, RenderTarget& zprepassRenderTarget) override;

		/// <summary>
		/// �`�揈���B
		/// </summary>
		/// <param name="rc"></param>
		/// <param name="mainRenderTarget"></param>
		void OnRender(RenderContext& rc, RenderTarget& mainRenderTarget) override;

		/// <summary>
		/// 臒l��ݒ�B
		/// </summary>
		/// <param name="value"></param>
		void SetThreshold(const float value)
		{
			m_luminanceCB.threshold = value;
		}

		/// <summary>
		/// 臒l���擾�B
		/// </summary>
		/// <returns></returns>
		const float GetThreshold()
		{
			return m_luminanceCB.threshold;
		}

	private:
		/// <summary>
		/// �P�x���o�p�̃X�v���C�g���������B
		/// </summary>
		/// <param name="mainRenderTarget"></param>
		void InitLuminanceSprite(RenderTarget& mainRenderTarget);

		/// <summary>
		/// �ŏI�����p�̃X�v���C�g���������B
		/// </summary>
		/// <param name="mainRenderTarget"></param>
		void InitFinalSprite(RenderTarget& mainRenderTarget);

	private:
		RenderTarget	m_luminanceRenderTarget;			//�P�x���o�p�����_�[�^�[�Q�b�g�B
		GaussianBlur	m_gaussianBlur[GAUSSIANBLUE_NUM];	//�K�E�V�A���u���[�B
		LuminanceCB		m_luminanceCB;						//�u���[���p�萔�o�b�t�@�B
		Sprite			m_luminanceSprite;					//�P�x���o�p�X�v���C�g�B
		Sprite			m_finalSprite;						//�ŏI�����p�̃X�v���C�g�B
		Sprite			m_copyMainRtSprite;					//���C�������_�����O�^�[�Q�b�g�ɕ`�悷�邽�߂̃X�v���C�g�B
	};

}
