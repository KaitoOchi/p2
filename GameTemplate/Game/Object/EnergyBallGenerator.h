#pragma once
class EnergyBall;

/// <summary>
/// �G�l���M�[�{�[�������@�B
/// </summary>
class EnergyBallGenerator : public IGameObject
{
public:
	EnergyBallGenerator();
	~EnergyBallGenerator();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

public:
	/// <summary>
	/// ���W��ݒ�B
	/// </summary>
	/// <param name="pos"></param>
	void SetPosition(const Vector3& pos)
	{
		m_position = pos;
	}

	/// <summary>
	/// ��]��ݒ�B
	/// </summary>
	/// <param name="rot"></param>
	void SetRotation(const Quaternion& rot)
	{
		m_rotation = rot;
		m_direction = CalcQuaternionToEuler(m_rotation);
		float tmp = m_direction.x;
		m_direction.x = m_direction.y;
		m_direction.y = m_direction.z;
		m_direction.z = tmp;
		m_direction.Normalize();
	}

	/// <summary>
	/// �G�l���M�[�{�[����������ʒm�B
	/// </summary>
	void NotifyDisable()
	{
		m_isEnableEnergyBall = false;
		m_generateTimer = 0.0f;
	}

	/// <summary>
	/// �������B
	/// </summary>
	void Disable();

private:
	EnergyBall*		m_energyBall = nullptr;				//�G�l���M�[�{�[���B
	Vector3			m_position;							//���W�B
	Vector3			m_direction;						//�����B
	Quaternion		m_rotation;							//��]�B
	bool			m_isEnableEnergyBall = false;		//�Ή�����G�l���M�[�{�[�����L�����ǂ����B
	bool			m_isClear = false;					//�N���A�������ǂ����B
	float			m_generateTimer = 0.0f;				//�����ɂ����鎞�ԁB
};

