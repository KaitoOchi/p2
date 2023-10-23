#pragma once

namespace nsPortalEngine {

	/// <summary>
	/// �����_���[�̊��N���X�B
	/// </summary>
	class IRenderer : Noncopyable
	{
	public:
		/// <summary>
		/// GBuffer�p�X����Ă΂�鏈���B
		/// </summary>
		/// <param name="rc"></param>
		virtual void OnRenderGBuffer(RenderContext& rc)
		{

		}

		/// <summary>
		/// �t�H���[�h�����_�[�p�X����Ă΂�鏈���B
		/// </summary>
		/// <param name="rc"></param>
		virtual void OnForwardRender(RenderContext& rc)
		{

		}

		/// <summary>
		/// �|�[�^���p�p�X����Ă΂�鏈���B
		/// </summary>
		/// <param name="rc"></param>
		virtual void OnPortalRender(RenderContext& rc, const int num, Camera& camera)
		{

		}

		/// <summary>
		/// �V���h�E�}�b�v�p�p�X����Ă΂�鏈���B
		/// </summary>
		/// <param name="rc"></param>
		virtual void OnRenderShadowMap(RenderContext& rc, Camera& camera)
		{

		}

		/// <summary>
		/// ZPrepass�p�p�X����Ă΂�鏈���B
		/// </summary>
		/// <param name="rc"></param>
		virtual void OnRenderToZPrepass(RenderContext& rc)
		{

		}

		/// <summary>
		/// 2D�`��p�X����Ă΂�鏈���B
		/// </summary>
		/// <param name="rc"></param>
		virtual void On2DRender(RenderContext& rc)
		{

		}
	};
}

