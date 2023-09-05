#pragma once

namespace nsPortalEngine {

	/// <summary>
	/// ポイントライト。
	/// </summary>
	class PointLight : Noncopyable
	{
	public:
		/// <summary>
		/// ポイントライト用の構造体
		/// </summary>
		struct PointLig
		{
			Vector3 ptPosition;		//座標
			float pad0 = 0.0f;
			Vector3 ptColor;		//カラー
			float ptRange = 0.0f;	//影響範囲
		};

		PointLight() {};
		~PointLight() {};

		/// <summary>
		/// ポイントライトを設定。
		/// </summary>
		/// <param name="num">番号</param>
		/// <param name="pos">座標</param>
		/// <param name="color">カラー</param>
		/// <param name="range">影響範囲</param>
		void SetPointLight(const int num, const Vector3& pos, const Vector3& color, const float range)
		{
			SetNumber(num);
			SetPosition(pos);
			SetColor(color);
			SetRange(range);
			Update();
		}

		/// <summary>
		/// ポイントライトの番号を設定
		/// </summary>
		void SetNumber(const int num)
		{
			m_ptNum = num;
		}

		/// <summary>
		/// 座標を設定。
		/// </summary>
		void SetPosition(const Vector3& pos)
		{
			m_pointLight.ptPosition = pos;
		}

		/// <summary>
		/// 座標を取得。
		/// </summary>
		/// <returns></returns>
		const Vector3& GetPosition() const
		{
			return m_pointLight.ptPosition;
		}

		/// <summary>
		/// カラーを設定。
		/// </summary>
		void SetColor(const Vector3& color)
		{
			m_pointLight.ptColor = color;
		}

		/// <summary>
		/// カラーを取得。
		/// </summary>
		/// <param name="range"></param>
		const Vector3& GetColor() const
		{
			return m_pointLight.ptColor;
		}

		/// <summary>
		/// 影響範囲を設定。
		/// </summary>
		void SetRange(const float range)
		{
			m_pointLight.ptRange = range;
		}

		/// <summary>
		/// 影響範囲を取得。
		/// </summary>
		const float GetRange() const
		{
			return m_pointLight.ptRange;
		}

		/// <summary>
		/// 構造体を取得
		/// </summary>
		PointLig& GetPointLight()
		{
			return m_pointLight;
		}

		/// <summary>
		/// 更新処理。
		/// </summary>
		void Update();

	private:
		PointLig	m_pointLight;		//ポイントライト
		int			m_ptNum = 0;	//番号
	};
}

