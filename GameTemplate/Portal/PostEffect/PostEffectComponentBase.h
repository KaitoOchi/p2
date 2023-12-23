#pragma once

namespace nsPortalEngine
{
	/// <summary>
	/// �|�X�g�G�t�F�N�g�̊��N���X�B
	/// </summary>
	class PostEffectComponentBase : Noncopyable
	{
	public:
		PostEffectComponentBase();
		virtual ~PostEffectComponentBase();

		/// <summary>
		/// �����������B
		/// </summary>
		/// <param name="mainRenderTarget"></param>
		/// <param name="zprepassRenderTarget"></param>
		void Init(RenderTarget* mainRenderTarget, RenderTarget& zprepassRenderTarget);

		/// <summary>
		/// �`�揈���B
		/// </summary>
		/// <param name="rc"></param>
		/// <param name="mainRenderTarget"></param>
		void Render(RenderContext& rc, RenderTarget* mainRenderTarget);

		/// <summary>
		/// �L���������B
		/// </summary>
		void Enable()
		{
			m_isEnable = true;
		}

		/// <summary>
		/// �����������B
		/// </summary>
		void Disable()
		{
			m_isEnable = false;
		}

	private:
		/// <summary>
		/// �����������B
		/// </summary>
		/// <param name="mainRenderTarget"></param>
		/// <param name="zprepassRenderTarget"></param>
		virtual void OnInit(RenderTarget* mainRenderTarget, RenderTarget& zprepassRenderTarget) = 0;

		/// <summary>
		/// �`�揈���B
		/// </summary>
		/// <param name="rc"></param>
		/// <param name="mainRenderTarget"></param>
		virtual void OnRender(RenderContext& rc, RenderTarget* mainRenderTarget) = 0;

	private:
		bool m_isEnable = true;		//�L�����ǂ����B
	};
}

