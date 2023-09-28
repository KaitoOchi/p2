#pragma once
#include "Bloom.h"

namespace nsPortalEngine {

	/// <summary>
	/// �|�X�g�G�t�F�N�g�B
	/// </summary>
	class PostEffect : Noncopyable
	{
	public:
		PostEffect();
		~PostEffect();

		/// <summary>
		/// �����������B
		/// </summary>
		/// <param name="mainRenderTarget"></param>
		void Init(RenderTarget& mainRenderTarget);

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
			m_bloom.SetThreshold(value);
		}

		/// <summary>
		/// �u���[����臒l���擾�B
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

