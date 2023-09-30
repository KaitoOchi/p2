#pragma once

namespace nsPortalEngine {

	/// <summary>
	/// レンダラーの基底クラス。
	/// </summary>
	class IRenderer : Noncopyable
	{
	public:
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
		virtual void OnPortalRender(RenderContext& rc, const int num)
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
		/// 2D描画パスから呼ばれる処理。
		/// </summary>
		/// <param name="rc"></param>
		virtual void On2DRender(RenderContext& rc)
		{

		}
	};
}

