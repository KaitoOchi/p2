#pragma once

namespace nsPortalEngine {

	class SpotLight : Noncopyable
	{
	public:
		/// <summary>
		/// スポットライト用の構造体
		/// </summary>
		struct SpotLig
		{
			Vector3 spPosition;		//座標
			float pad0 = 0.0f;
			Vector3 spColor;		//カラー
			float spRange = 0.0f;	//影響範囲
			Vector3 spDirection;	//方向
			float spAngle = 0.0f;	//射出角度
		};

	public:
		SpotLight() {};
		~SpotLight() {};

		/// <summary>
		/// スポットライトを設定。
		/// </summary>
		/// <param name="num">番号</param>
		/// <param name="pos">座標</param>
		/// <param name="color">カラー</param>
		/// <param name="range">影響範囲</param>
		/// <param name="dir">方向</param>
		/// <param name="angle">射出角度</param>
		void SetSpotLight(const int num, const Vector3& pos, const Vector3& color, const float range, const Vector3& dir, const float angle)
		{
			SetNumber(num);
			SetPosition(pos);
			SetColor(color);
			SetRange(range);
			SetDirection(dir);
			SetAngle(angle);
			Update(num);
		}

		/// <summary>
		/// 番号を設定。
		/// </summary>
		/// <returns></returns>
		void SetNumber(const int num)
		{
			m_spNum = num;
		}

		/// <summary>
		/// 座標を設定。
		/// </summary>
		/// <param name="pos"></param>
		void SetPosition(const Vector3& pos)
		{
			m_spotLight.spPosition = pos;
		}

		/// <summary>
		/// 座標を取得。
		/// </summary>
		/// <returns></returns>
		const Vector3& GetPosition() const
		{
			return m_spotLight.spPosition;
		}

		/// <summary>
		/// カラーを設定。
		/// </summary>
		/// <param name="color"></param>
		void SetColor(const Vector3& color)
		{
			m_spotLight.spColor = color;
		}

		/// <summary>
		/// カラーを取得。
		/// </summary>
		/// <returns></returns>
		const Vector3& GetColor() const
		{
			return m_spotLight.spColor;
		}

		/// <summary>
		/// 影響範囲を設定。
		/// </summary>
		/// <param name="range"></param>
		void SetRange(const float range)
		{
			m_spotLight.spRange = range;
		}

		/// <summary>
		/// 影響範囲を取得。
		/// </summary>
		/// <returns></returns>
		const float GetRange() const
		{
			return m_spotLight.spRange;
		}

		/// <summary>
		/// 方向を設定。
		/// </summary>
		/// <param name="dir"></param>
		void SetDirection(const Vector3& dir)
		{
			m_spotLight.spDirection = dir;
			m_spotLight.spDirection.Normalize();
		}

		/// <summary>
		/// 方向を取得。
		/// </summary>
		/// <returns></returns>
		const Vector3& GetDirection() const
		{
			return m_spotLight.spDirection;
		}

		/// <summary>
		/// 射出角度を設定。
		/// </summary>
		/// <param name="angle"></param>
		void SetAngle(const float angle)
		{
			m_spotLight.spAngle = Math::DegToRad(angle);
		}

		/// <summary>
		/// 射出角度を取得。
		/// </summary>
		/// <returns></returns>
		const float GetAngle() const
		{
			return m_spotLight.spAngle;
		}

		/// <summary>
		/// 構造体を取得。
		/// </summary>
		/// <returns></returns>
		SpotLig& GetSpotLight()
		{
			return m_spotLight;
		}

		/// <summary>
		/// 更新処理。
		/// </summary>
		void Update(const int num);

	private:
		SpotLig		m_spotLight;		//スポットライト
		int			m_spNum = 0;	//番号
	};
}