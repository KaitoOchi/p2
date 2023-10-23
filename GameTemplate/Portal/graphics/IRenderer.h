#pragma once

namespace nsPortalEngine {

	/// <summary>
	/// レンダラーの基底クラス。
	/// </summary>
	class IRenderer : Noncopyable
	{
	public:
		/// <summary>
		/// GBufferパスから呼ばれる処理。
		/// </summary>
		/// <param name="rc"></param>
		virtual void OnRenderGBuffer(RenderContext& rc)
		{

		}

		/// <summary>
		/// フォワードレンダーパスから呼ばれる処理。
		/// </summary>
		/// <param name="rc"></param>
		virtual void OnForwardRender(RenderContext& rc)
		{

		}

		/// <summary>
		/// ポータル用パスから呼ばれる処理。
		/// </summary>
		/// <param name="rc"></param>
		virtual void OnPortalRender(RenderContext& rc, const int num, Camera& camera)
		{

		}

		/// <summary>
		/// シャドウマップ用パスから呼ばれる処理。
		/// </summary>
		/// <param name="rc"></param>
		virtual void OnRenderShadowMap(RenderContext& rc, Camera& camera)
		{

		}

		/// <summary>
		/// ZPrepass用パスから呼ばれる処理。
		/// </summary>
		/// <param name="rc"></param>
		virtual void OnRenderToZPrepass(RenderContext& rc)
		{

		}

		/// <summary>
		/// 2D描画パスから呼ばれる処理。
		/// </summary>
		/// <param name="rc"></param>
		virtual void On2DRender(RenderContext& rc)
		{

		}
	};
}

