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
		enState_Idle,			//�ҋ@�B
		enState_Walk,			//�����B
		enState_Run,			//����B
		enState_Crouch_Idle,	//���Ⴊ�ݑҋ@�B
		enState_Crouch_Walk,	//���Ⴊ�ݕ����B
		enState_Crouch_Jump,	//���Ⴊ�݃W�����v�B
		enState_Jump,			//�W�����v�B
		enState_Dead,
		enState_Num,			//���B
	};

public:
	Player();
	~Player();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

public:
	/// <summary>
	/// �󂯂�_���[�W��ݒ�B
	/// </summary>
	/// <param name="damage">�󂯂�_���[�W��</param>
	/// <param name="dir">�̂��������</param>
	void ReceiveDamage(const int damage, const Vector3& dir);

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

	/// <summary>
	/// �|�[�^���ɓ����Ă����Ԃ�ݒ�B
	/// </summary>
	/// <param name="flag"></param>
	void SetIsPortal(const bool flag)
	{
		m_characterController.SetIsPortal(flag);
	}

	/// <summary>
	/// �L�����R�����擾�B
	/// </summary>
	const CharacterController& GetCharacterController() const
	{
		return m_characterController;
	}

	/// <summary>
	/// �����蔻����擾�B
	/// </summary>
	/// <returns></returns>
	btCollisionObject* GetCollision()
	{
		return m_characterController.GetRigidBody()->GetBody();
	}

	/// <summary>
	/// �|�[�^���ւ̃��[�v�����B
	/// </summary>
	void SetWarp(const Vector3& pos, const Quaternion& rot);

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

	/// <summary>
	/// ���S��Ԃ̑J�ڏ����B
	/// </summary>
	void ProcessDeadStateTransition();

private:
	ModelRender				m_modelRender;					//���f�������_�[�B
	CharacterController		m_characterController;			//�L�����N�^�[�R���g���[���[�B
	AnimationClip			m_animationClips[enState_Num];	//�A�j���[�V�����N���b�v�B
	PortalGun*				m_portalGun = nullptr;			//�|�[�^���K���B
	GameCamera*				m_gameCamera = nullptr;			//�Q�[���J�����B
	Vector3					m_position;						//���W�B
	Vector3					m_moveSpeed;					//�ړ����x�B
	Vector3					m_addMoveSpeed;					//�ǉ�����ړ����x�B
	Quaternion				m_rotation;						//��]�B
	PlayerState				m_playerState = enState_Idle;	//�v���C���[�X�e�[�g�B
	int						m_hp = 0;						//HP�B
	float					m_walkSpeed = 0.0f;				//�ړ����x�B
	float					m_gravityAccel = 0.0f;			//�d�͉����B
	float					m_deadTimer = 0.0f;				//���S���ԁB


	FontRender a; //�f�o�b�O�p�B
};

