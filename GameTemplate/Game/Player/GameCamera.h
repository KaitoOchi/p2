#pragma once

class GameCamera : Noncopyable
{
public:
	GameCamera();
	~GameCamera();

	/// <summary>
	/// �����������B
	/// </summary>
	void Init();

	/// <summary>
	/// �X�V�����B
	/// </summary>
	void Update();

	/// <summary>
	/// �ړ������B
	/// </summary>
	void SetPosition(const Vector3& pos);

	/// <summary>
	/// ��]�����B
	/// </summary>
	void SetRotation();

	/// <summary>
	/// ���Ⴊ�ݏ�Ԃ��ǂ�����ݒ�B
	/// </summary>
	/// <param name="isCrouch"></param>
	void SetCrouchState(const bool isCrouch)
	{
		m_isCrouch = isCrouch;
	}

private:
	Vector3		m_position;				//���W�B
	Vector3		m_targetPos;			//�����_�B
	Vector3		m_rotSpeed;				//��]�ʁB
	Vector3		m_firstCrouchPos;		//���Ⴊ�݊J�n�̍��W�B
	Vector3		m_lastCrouchPos;		//���Ⴊ�ݏI���̍��W�B			
	bool		m_isCrouch;				//���Ⴊ�ݏ�Ԃ��B
	float		m_crouchTimer = 0.0f;	//���Ⴊ�ݏ�Ԃ̃^�C�}�[�B
};

