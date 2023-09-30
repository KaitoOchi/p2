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

	/// <summary>
	/// �ړ������B
	/// </summary>
	void Move();

	/// <summary>
	/// ��]�����B
	/// </summary>
	void Rotation();

	/// <summary>
	/// ���Ⴊ�ݏ�Ԃ��ǂ�����ݒ�B
	/// </summary>
	/// <param name="isCrouch"></param>
	void SetCrouchState(const bool isCrouch)
	{
		m_isCrouch = isCrouch;
	}

private:
	Player*		m_player = nullptr;
	Vector3		m_position;				//���W�B
	Vector3		m_targetPos;			//�����_�B
	Vector3		m_rotSpeed;				//��]�ʁB
	Vector3		m_firstCrouchPos;		//���Ⴊ�݊J�n�̍��W�B
	Vector3		m_lastCrouchPos;		//���Ⴊ�ݏI���̍��W�B
	bool		m_isCrouch;				//���Ⴊ�ݏ�Ԃ��B
	float		m_crouchTimer = 0.0f;	//���Ⴊ�ݏ�Ԃ̃^�C�}�[�B
};

