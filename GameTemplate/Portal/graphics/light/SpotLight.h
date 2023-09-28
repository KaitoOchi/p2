#pragma once

namespace nsPortalEngine {

	class SpotLight : Noncopyable
	{
	public:
		/// <summary>
		/// �X�|�b�g���C�g�p�̍\����
		/// </summary>
		struct SpotLig
		{
			Vector3 spPosition;		//���W
			float pad0 = 0.0f;
			Vector3 spColor;		//�J���[
			float spRange = 0.0f;	//�e���͈�
			Vector3 spDirection;	//����
			float spAngle = 0.0f;	//�ˏo�p�x
		};

	public:
		SpotLight() {};
		~SpotLight() {};

		/// <summary>
		/// �X�|�b�g���C�g��ݒ�B
		/// </summary>
		/// <param name="num">�ԍ�</param>
		/// <param name="pos">���W</param>
		/// <param name="color">�J���[</param>
		/// <param name="range">�e���͈�</param>
		/// <param name="dir">����</param>
		/// <param name="angle">�ˏo�p�x</param>
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
		/// �ԍ���ݒ�B
		/// </summary>
		/// <returns></returns>
		void SetNumber(const int num)
		{
			m_spNum = num;
		}

		/// <summary>
		/// ���W��ݒ�B
		/// </summary>
		/// <param name="pos"></param>
		void SetPosition(const Vector3& pos)
		{
			m_spotLight.spPosition = pos;
		}

		/// <summary>
		/// ���W���擾�B
		/// </summary>
		/// <returns></returns>
		const Vector3& GetPosition() const
		{
			return m_spotLight.spPosition;
		}

		/// <summary>
		/// �J���[��ݒ�B
		/// </summary>
		/// <param name="color"></param>
		void SetColor(const Vector3& color)
		{
			m_spotLight.spColor = color;
		}

		/// <summary>
		/// �J���[���擾�B
		/// </summary>
		/// <returns></returns>
		const Vector3& GetColor() const
		{
			return m_spotLight.spColor;
		}

		/// <summary>
		/// �e���͈͂�ݒ�B
		/// </summary>
		/// <param name="range"></param>
		void SetRange(const float range)
		{
			m_spotLight.spRange = range;
		}

		/// <summary>
		/// �e���͈͂��擾�B
		/// </summary>
		/// <returns></returns>
		const float GetRange() const
		{
			return m_spotLight.spRange;
		}

		/// <summary>
		/// ������ݒ�B
		/// </summary>
		/// <param name="dir"></param>
		void SetDirection(const Vector3& dir)
		{
			m_spotLight.spDirection = dir;
			m_spotLight.spDirection.Normalize();
		}

		/// <summary>
		/// �������擾�B
		/// </summary>
		/// <returns></returns>
		const Vector3& GetDirection() const
		{
			return m_spotLight.spDirection;
		}

		/// <summary>
		/// �ˏo�p�x��ݒ�B
		/// </summary>
		/// <param name="angle"></param>
		void SetAngle(const float angle)
		{
			m_spotLight.spAngle = Math::DegToRad(angle);
		}

		/// <summary>
		/// �ˏo�p�x���擾�B
		/// </summary>
		/// <returns></returns>
		const float GetAngle() const
		{
			return m_spotLight.spAngle;
		}

		/// <summary>
		/// �\���̂��擾�B
		/// </summary>
		/// <returns></returns>
		SpotLig& GetSpotLight()
		{
			return m_spotLight;
		}

		/// <summary>
		/// �X�V�����B
		/// </summary>
		void Update(const int num);

	private:
		SpotLig		m_spotLight;		//�X�|�b�g���C�g
		int			m_spNum = 0;	//�ԍ�
	};
}