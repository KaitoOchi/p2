#pragma once
#include "Bloom.h"
#include "DoF.h"

namespace nsPortalEngine {

	/// <summary>
	/// �|�X�g�G�t�F�N�g�B
	/// </summary>
	class PostEffect : Noncopyable
	{
	public:
		/// <summary>
		/// �|�X�g�G�t�F�N�g�̎�ށB
		/// </summary>
		enum PostEffectType
		{
			enPostEffect_Bloom,		//�u���[���B
			enPostEffect_DoF,		//��ʊE�[�x�B
			enPostEffect_Num,
		};

	public:
		PostEffect();
		~PostEffect();

		/// <summary>
		/// �����������B
		/// </summary>
		/// <param name="mainRenderTarget"></param>
		void Init(RenderTarget& mainRenderTarget, RenderTarget& zprepassRenderTarget);

		/// <summary>
		/// �`�揈���B
		/// </summary>
		/// <param name="rc"></param>
		/// <param name="mainRenderTarget"></param>
		void Render(RenderContext& rc, RenderTarget& mainRenderTarget);

		/// <summary>
		/// �u���[����臒l��ݒ�B
		/// </summary>
		/// <param name="value"></param>
		void SetBloomThreshold(const float value)
		{
			m_bloom->SetThreshold(value);
		}

		/// <summary>
		/// �u���[����臒l���擾�B
		/// </summary>
		/// <returns></returns>
		const float GetBloomThreshold()
		{
			return m_bloom->GetThreshold();
		}

		/// <summary>
		/// �L�����B
		/// </summary>
		/// <param name="entype"></param>
		void Enable(const PostEffectType entype)
		{
			int num = static_cast<int>(entype);
			m_postEffectType[num]->Enable();
		}

		/// <summary>
		/// �������B
		/// </summary>
		/// <param name="entype"></param>
		void Disable(const PostEffectType entype)
		{
			int num = static_cast<int>(entype);
			m_postEffectType[num]->Disable();
		}

	private:
		std::vector<PostEffectComponentBase*>	m_postEffectType;		//�|�X�g�G�t�F�N�g�̔z��B
		Bloom*									m_bloom = nullptr;		//�u���[���B
		DoF*									m_dof = nullptr;		//��ʊE�[�x�B
	};
}

