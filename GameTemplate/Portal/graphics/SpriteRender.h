#pragma once

namespace nsPortalEngine {

	/// <summary>
	/// スプライトレンダー。
	/// </summary>
	class SpriteRender : public IRenderer
	{
	public:
		/// <summary>
		/// 初期化処理。
		/// </summary>
		/// <param name="filePath"></param>
		/// <param name="w"></param>
		/// <param name="h"></param>
		/// <param name="alpha"></param>
		void Init(
			const char* filePath,
			const float w,
			const float h,
			const AlphaBlendMode alpha = AlphaBlendMode_Trans
		);

		/// <summary>
		/// 座標を設定。
		/// </summary>
		/// <param name="pos"></param>
		void SetPosition(const Vector3& pos)
		{
			m_position = pos;
			m_position.z = 0.0f;
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
		void SetRotation(const Quaternion& rot)
		{
			m_rotation = rot;
		}

		/// <summary>
		/// 回転の取得。
		/// </summary>
		/// <returns></returns>
		const Quaternion& GetRotation() const
		{
			return m_rotation;
		}

		/// <summary>
		/// 拡大率を設定。
		/// </summary>
		/// <param name="scale"></param>
		void SetScale(const Vector3& scale)
		{
			m_scale = scale;
			m_scale.z = 0.0f;
		}

		/// <summary>
		/// 拡大率を取得。
		/// </summary>
		/// <returns></returns>
		const Vector3& GetScale() const
		{
			return m_scale;
		}

		/// <summary>
		/// ピボットを設定。
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
		/// 乗算カラーを設定。
		/// </summary>
		/// <param name="mulColor"></param>
		void SetMulColor(const Vector4& mulColor)
		{
			m_sprite.SetMulColor(mulColor);
		}

		/// <summary>
		/// 乗算カラーの取得。
		/// </summary>
		/// <returns></returns>
		const Vector4& GetMulColor() const
		{
			return m_sprite.GetMulColor();
		}

		/// <summary>
		/// 更新処理。
		/// </summary>
		void Update()
		{
			m_sprite.Update(
				m_position,
				m_rotation,
				m_scale,
				m_pivot
			);
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
		Sprite		m_sprite;							//スプライト。
		Vector2		m_pivot = Sprite::DEFAULT_PIVOT;	//座標。
		Vector3		m_position;							//拡大率。
		Vector3		m_scale = Vector3::One;				//回転。
		Quaternion	m_rotation;							//ピボット。
	};
}

