#pragma once

namespace nsPortalEngine {

	class FontRender : public IRenderer
	{
	public:
		static const int MAX_TEXT_SIZE = 256;	//文字の最大数。

		FontRender() {};
		~FontRender() {};

		/// <summary>
		/// テキストを設定。
		/// </summary>
		/// <param name="text"></param>
		void SetText(const wchar_t* text)
		{
			swprintf_s(m_text, text);
		}

		/// <summary>
		/// テキストの取得。
		/// </summary>
		/// <returns></returns>
		const wchar_t* GetText() const
		{
			return m_text;
		}

		/// <summary>
		/// 座標を設定。
		/// </summary>
		/// <param name="pos"></param>
		void SetPosition(const Vector3& pos)
		{
			m_position = pos;
		}

		/// <summary>
		/// 座標の取得。
		/// </summary>
		/// <returns></returns>
		const Vector3& GetPosition() const
		{
			return m_position;
		}

		/// <summary>
		/// 回転を設定。
		/// </summary>
		/// <param name="rot"></param>
		void SetRotation(const float rot)
		{
			m_rotation = rot;
		}

		/// <summary>
		/// 回転の取得。
		/// </summary>
		/// <returns></returns>
		const float GetRotation() const
		{
			return m_rotation;
		}

		/// <summary>
		/// 拡大率を設定。
		/// </summary>
		/// <param name="scale"></param>
		void SetScale(const float scale)
		{
			m_scale = scale;
		}

		/// <summary>
		/// 拡大率の取得。
		/// </summary>
		/// <returns></returns>
		const float GetScale() const
		{
			return m_scale;
		}

		/// <summary>
		/// 色を設定。
		/// </summary>
		/// <param name="color"></param>
		void SetColor(const Vector4& color)
		{
			m_color = color;
		}

		/// <summary>
		/// 色の取得。
		/// </summary>
		/// <returns></returns>
		const Vector4& GetColor() const
		{
			return m_color;
		}

		/// <summary>
		/// ピボットの設定。
		/// </summary>
		/// <param name="pivot"></param>
		void SetPivot(const Vector2& pivot)
		{
			m_pivot = pivot;
		}

		/// <summary>
		/// ピボットの取得。
		/// </summary>
		/// <returns></returns>
		const Vector2& GetPivot() const
		{
			return m_pivot;
		}

		/// <summary>
		/// 影のパラメータを設定。
		/// </summary>
		/// <param name="isDrawShadow">影を描画するかどうか</param>
		/// <param name="shadowOffset">ピクセルのオフセット量</param>
		/// <param name="shadowColor">影の色</param>
		void SetShadowParam(
			const bool isDrawShadow,
			const float shadowOffset,
			const Vector4& shadowColor)
		{
			m_font.SetShadowParam(isDrawShadow, shadowOffset, shadowColor);
		}

		/// <summary>
		/// 描画処理。
		/// </summary>
		/// <param name="rc"></param>
		void Draw(RenderContext& rc);

	private:
		/// <summary>
		/// 2D描画パスから呼ばれる処理。
		/// </summary>
		/// <param name="rc"></param>
		void On2DRender(RenderContext& rc) override;

	private:
		Font	m_font;								//フォント。
		Vector2 m_pivot = Sprite::DEFAULT_PIVOT;	//ピボット。
		Vector3 m_position;							//座標。
		Vector4 m_color = g_vec4White;				//色。
		wchar_t m_text[MAX_TEXT_SIZE];				//文字。
		float	m_rotation = 0.0f;					//拡大率。
		float	m_scale = 1.0f;						//回転。
	};

}
