#pragma once

namespace nsPortalEngine{

	/// <summary>
	/// �f�B���N�V�������C�g�B
	/// </summary>
	class DirectionalLight : Noncopyable
	{
	public:
		/// <summary>
		/// �f�B���N�V�������C�g�p�̍\���́B
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
		/// �f�B���N�V�������C�g��ݒ�B
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
		/// ���C�g�̕�����ݒ�B
		/// </summary>
		/// <param name="dir"></param>
		void SetDirection(const Vector3& dir)
		{
			m_directionLig.ligDirection = dir;
		}

		/// <summary>
		/// ���C�g�̐F��ݒ�B
		/// </summary>
		/// <param name="color"></param>
		void SetColor(const Vector3& color)
		{
			m_directionLig.ligColor = color;
		}

		/// <summary>
		/// ������ݒ�B
		/// </summary>
		/// <param name="amb"></param>
		void SetAmbientLight(const float amb)
		{
			m_directionLig.ambient = amb;
		}

		/// <summary>
		/// �X�V�����B
		/// </summary>
		void Update();

		/// <summary>
		/// �\���̂��擾�B
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

