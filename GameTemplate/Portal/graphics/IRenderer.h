#pragma once

namespace nsPortalEngine {

	/// <summary>
	/// �����_���[�̊��N���X�B
	/// </summary>
	class IRenderer : Noncopyable
	{
	public:
		/// <summary>
		/// �t�H���[�h�����_�[�p�X����Ă΂�鏈���B
		/// </summary>
		/// <param name="rc"></param>
		virtual void OnForwardRender(RenderContext& rc)
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

