#pragma once

namespace nsPortalEngine
{
	/// <summary>
	/// ポストエフェクトの基底クラス。
	/// </summary>
	class PostEffectComponentBase : Noncopyable
	{
	public:
		PostEffectComponentBase();
		virtual ~PostEffectComponentBase();

		/// <summary>
		/// 初期化処理。
		/// </summary>
		/// <param name="mainRenderTarget"></param>
		/// <param name="zprepassRenderTarget"></param>
		void Init(RenderTarget* mainRenderTarget, RenderTarget& zprepassRenderTarget);

		/// <summary>
		/// 描画処理。
		/// </summary>
		/// <param name="rc"></param>
		/// <param name="mainRenderTarget"></param>
		void Render(RenderContext& rc, RenderTarget* mainRenderTarget);

		/// <summary>
		/// 有効化処理。
		/// </summary>
		void Enable()
		{
			m_isEnable = true;
		}

		/// <summary>
		/// 無効化処理。
		/// </summary>
		void Disable()
		{
			m_isEnable = false;
		}

	private:
		/// <summary>
		/// 初期化処理。
		/// </summary>
		/// <param name="mainRenderTarget"></param>
		/// <param name="zprepassRenderTarget"></param>
		virtual void OnInit(RenderTarget* mainRenderTarget, RenderTarget& zprepassRenderTarget) = 0;

		/// <summary>
		/// 描画処理。
		/// </summary>
		/// <param name="rc"></param>
		/// <param name="mainRenderTarget"></param>
		virtual void OnRender(RenderContext& rc, RenderTarget* mainRenderTarget) = 0;

	private:
		bool m_isEnable = true;		//有効かどうか。
	};
}

