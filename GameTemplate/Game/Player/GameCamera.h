#pragma once
class Player;

/// <summary>
/// �Q�[���J�����B
/// </summary>
class GameCamera : public IGameObject
{
public:
	GameCamera();
	~GameCamera();
	bool Start();
	void Update();

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
	/// ��]�����擾�B
	/// </summary>
	/// <returns></returns>
	const Vector3& GetRotRadian() const
	{
		return m_rotSpeed;
	}

	/// <summary>
	/// ���Ⴊ�ݏ�Ԃ��ǂ�����ݒ�B
	/// </summary>
	/// <param name="isCrouch"></param>
	void SetCrouchState(const bool isCrouch)
	{
		m_isCrouch = isCrouch;
	}

	/// <summary>
	/// �|�[�^���ւ̃��[�v�����B
	/// </summary>
	/// <param name="angle">��]��</param>
	void SetWarp(const float angle);

	/// <summary>
	/// ���S��Ԃɂ���B
	/// </summary>
	void Dead()
	{
		m_isCrouch = false;
		m_isDead = true;
		g_camera3D->Move(Vector3(0.0f, -50.0f, 0.0f));
		g_camera3D->SetUp(Vector3(-1.0f, 0.0f, 0.0f));
		g_camera3D->Update();
	}

	/// <summary>
	/// ���Z�b�g�����B
	/// </summary>
	void Reset()
	{
		m_isDead = false;
		g_camera3D->SetUp(Vector3(0.0f, 1.0f, 0.0f));
		g_camera3D->Update();
	}

private:
	/// <summary>
	/// �ړ������B
	/// </summary>
	void Move();
	/// <summary>
	/// ��]�����B
	/// </summary>
	void Rotation();

private:
	Vector3		m_position;				//���W�B
	Vector3		m_targetPos;			//�����_�B
	Vector3		m_rotSpeed;				//��]�ʁB
	Vector3		m_firstCrouchPos;		//���Ⴊ�݊J�n�̍��W�B
	Vector3		m_lastCrouchPos;		//���Ⴊ�ݏI���̍��W�B
	bool		m_isCrouch = false;		//���Ⴊ�ݏ�Ԃ��B
	bool		m_isDead = false;		//���S��Ԃ��B
	float		m_crouchTimer = 0.0f;	//���Ⴊ�ݏ�Ԃ̃^�C�}�[�B
};

