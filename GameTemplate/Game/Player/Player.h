#pragma once
class PortalGun;
class GameCamera;

/// <summary>
/// �v���C���[�B
/// </summary>
class Player : public IGameObject
{
public:
	/// <summary>
	/// �v���C���[�X�e�[�g�B
	/// </summary>
	enum PlayerState
	{
		enState_Idle,
		enState_Walk,
		enState_Run,
		enState_Crouch_Idle,
		enState_Crouch_Walk,
		enState_Crouch_Jump,
		enState_Jump,
		enState_Num,
	};

public:
	Player();
	~Player();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

public:
	/// <summary>
	/// ���W���擾�B
	/// </summary>
	const Vector3& GetPosition() const
	{
		return m_position;
	}

	/// <summary>
	/// ��]���擾�B
	/// </summary>
	const Quaternion& GetRotation() const
	{
		return m_rotation;
	}

private:
	/// <summary>
	/// ���͏����B
	/// </summary>
	void Input();

	/// <summary>
	/// XZ���̈ړ������B
	/// </summary>
	void MoveXZ();

	/// <summary>
	/// Y���̈ړ������B
	/// </summary>
	void MoveY();

	/// <summary>
	/// ��]�����B
	/// </summary>
	void Rotation();

	/// <summary>
	/// �|�[�^���̔��ˏ����B
	/// </summary>
	void Shot();

	/// <summary>
	/// �A�j���[�V���������B
	/// </summary>
	void PlayAnimation();

	/// <summary>
	/// �X�e�[�g�����B
	/// </summary>
	void State();

	/// <summary>
	/// ���ʑJ�ڏ����B
	/// </summary>
	void ProcessCommonStateTransition();

	/// <summary>
	/// �ҋ@��Ԃ̑J�ڏ����B
	/// </summary>
	void ProcessIdleStateTransition();

	/// <summary>
	/// ������Ԃ̑J�ڏ����B
	/// </summary>
	void ProcessWalkStateTransition();

	/// <summary>
	/// �����Ԃ̑J�ڏ����B
	/// </summary>
	void ProcessRunStateTransition();

	/// <summary>
	/// ���Ⴊ�ݑҋ@��Ԃ̑J�ڏ����B
	/// </summary>
	void ProcessCrouchIdleStateTransition();

	/// <summary>
	/// ���Ⴊ�ݕ�����Ԃ̑J�ڏ����B
	/// </summary>
	void ProcessCrouchWalkStateTransition();

	/// <summary>
	/// ���Ⴊ�݃W�����v��Ԃ̑J�ڏ����B
	/// </summary>
	void ProcessCrouchJumpStateTransition();

	/// <summary>
	/// �W�����v��Ԃ̑J�ڏ����B
	/// </summary>
	void ProcessJumpStateTransition();

private:
	ModelRender				m_modelRender;					//���f�������_�[�B
	CharacterController		m_characterController;			//�L�����N�^�[�R���g���[���[�B
	AnimationClip			m_animationClips[enState_Num];	//�A�j���[�V�����N���b�v�B

	PortalGun*				m_portalGun = nullptr;			//�|�[�^���K���B
	GameCamera*				m_gameCamera = nullptr;			//�Q�[���J�����B

	Vector3					m_position;						//���W�B
	Vector3					m_moveSpeed;					//�ړ����x�B
	Quaternion				m_rotation;						//��]�B
	PlayerState				m_playerState = enState_Idle;	//�v���C���[�X�e�[�g�B

	float					m_walkSpeed = 0.0f;				//�ړ����x�B
	float					m_gravityAccel = 0.0f;			//�d�͉����B


	FontRender a;
};

