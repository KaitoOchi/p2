#pragma once

namespace nsPortalEngine {

	/// <summary>
	/// �|�C���g���C�g�B
	/// </summary>
	class PointLight : Noncopyable
	{
	public:
		/// <summary>
		/// �|�C���g���C�g�p�̍\����
		/// </summary>
		struct PointLig
		{
			Vector3 ptPosition;		//���W
			float pad0 = 0.0f;
			Vector3 ptColor;		//�J���[
			float ptRange = 0.0f;	//�e���͈�
		};

		PointLight() {};
		~PointLight() {};

		/// <summary>
		/// �|�C���g���C�g��ݒ�B
		/// </summary>
		/// <param name="num">�ԍ�</param>
		/// <param name="pos">���W</param>
		/// <param name="color">�J���[</param>
		/// <param name="range">�e���͈�</param>
		void SetPointLight(const int num, const Vector3& pos, const Vector3& color, const float range)
		{
			SetNumber(num);
			SetPosition(pos);
			SetColor(color);
			SetRange(range);
			Update();
		}

		/// <summary>
		/// �|�C���g���C�g�̔ԍ���ݒ�
		/// </summary>
		void SetNumber(const int num)
		{
			m_ptNum = num;
		}

		/// <summary>
		/// ���W��ݒ�B
		/// </summary>
		void SetPosition(const Vector3& pos)
		{
			m_pointLight.ptPosition = pos;
		}

		/// <summary>
		/// ���W���擾�B
		/// </summary>
		/// <returns></returns>
		const Vector3& GetPosition() const
		{
			return m_pointLight.ptPosition;
		}

		/// <summary>
		/// �J���[��ݒ�B
		/// </summary>
		void SetColor(const Vector3& color)
		{
			m_pointLight.ptColor = color;
		}

		/// <summary>
		/// �J���[���擾�B
		/// </summary>
		/// <param name="range"></param>
		const Vector3& GetColor() const
		{
			return m_pointLight.ptColor;
		}

		/// <summary>
		/// �e���͈͂�ݒ�B
		/// </summary>
		void SetRange(const float range)
		{
			m_pointLight.ptRange = range;
		}

		/// <summary>
		/// �e���͈͂��擾�B
		/// </summary>
		const float GetRange() const
		{
			return m_pointLight.ptRange;
		}

		/// <summary>
		/// �\���̂��擾
		/// </summary>
		PointLig& GetPointLight()
		{
			return m_pointLight;
		}

		/// <summary>
		/// �X�V�����B
		/// </summary>
		void Update();

	private:
		PointLig	m_pointLight;		//�|�C���g���C�g
		int			m_ptNum = 0;	//�ԍ�
	};
}

