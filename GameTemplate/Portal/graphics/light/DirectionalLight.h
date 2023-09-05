#pragma once

namespace nsPortalEngine{

	/// <summary>
	/// ディレクションライト。
	/// </summary>
	class DirectionalLight : Noncopyable
	{
	public:
		/// <summary>
		/// ディレクションライト用の構造体。
		/// </summary>
		struct DirectionLig
		{
			Vector3 ligDirection;
			float pad0 = 0.0f;
			Vector3 ligColor;
			float ambient = 0.0f;
		};

	public:
		DirectionalLight() {};
		~DirectionalLight() {};

		/// <summary>
		/// ディレクションライトを設定。
		/// </summary>
		/// <param name="dir"></param>
		/// <param name="color"></param>
		void SetDirectionLight(const Vector3& dir, const Vector3& color)
		{
			SetDirection(dir);
			SetColor(color);
			Update();
		}

		/// <summary>
		/// ライトの方向を設定。
		/// </summary>
		/// <param name="dir"></param>
		void SetDirection(const Vector3& dir)
		{
			m_directionLig.ligDirection = dir;
		}

		/// <summary>
		/// ライトの色を設定。
		/// </summary>
		/// <param name="color"></param>
		void SetColor(const Vector3& color)
		{
			m_directionLig.ligColor = color;
		}

		/// <summary>
		/// 環境光を設定。
		/// </summary>
		/// <param name="amb"></param>
		void SetAmbientLight(const float amb)
		{
			m_directionLig.ambient = amb;
		}

		/// <summary>
		/// 更新処理。
		/// </summary>
		void Update();

		/// <summary>
		/// 構造体を取得。
		/// </summary>
		/// <returns></returns>
		DirectionLig& GetDirectionLig()
		{
			return m_directionLig;
		}

	private:
		DirectionLig m_directionLig;
	};
}

