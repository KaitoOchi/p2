#pragma once
class PortalGun;
class GameCamera;
class Game;

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
		enState_Jump,			//�W�����v�B
		enState_JumpEnd,		//�W�����v�I���B
		enState_Dead,			//���S�B
		enState_End,			//�I���B
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
	/// ���Z�b�g�����B
	/// </summary>
	void Reset();

	/// <summary>
	/// �ǉ��̈ړ����x��ݒ�B
	/// </summary>
	/// <param name="moveSpeed"></param>
	void SetAddMoveSpeed(const Vector3& moveSpeed)
	{
		m_addMoveSpeed = moveSpeed;
	}

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
		m_characterController->SetIsPortal(flag);
	}

	/// <summary>
	/// �v���C���[�X�e�[�g��ݒ�B
	/// </summary>
	/// <param name="state"></param>
	void SetPlayerState(const PlayerState& state)
	{
		m_playerState = state;
	}

	/// <summary>
	/// �v���C���[�X�e�[�g���擾�B
	/// </summary>
	/// <returns></returns>
	const PlayerState GetPlayerState() const
	{
		return m_playerState;
	}

	/// <summary>
	/// �L�����R�����擾�B
	/// </summary>
	const CharacterController* GetCharacterController() const
	{
		return m_characterController;
	}

	/// <summary>
	/// �����蔻����擾�B
	/// </summary>
	/// <returns></returns>
	btCollisionObject* GetCollision()
	{
		return m_characterController->GetRigidBody()->GetBody();
	}

	/// <summary>
	/// �|�[�^���ւ̃��[�v�����B
	/// </summary>
	void SetWarp(const Vector3& pos, const float angle);

private:
	//�A�j���[�V�����C�x���g�p�̊֐�
	void OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName);

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
	/// ���Ⴊ�ݏ����B
	/// </summary>
	void Crouch();
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
	/// �W�����v��Ԃ̑J�ڏ����B
	/// </summary>
	void ProcessJumpStateTransition();
	/// <summary>
	/// �W�����v�I����Ԃ̑J�ڏ����B
	/// </summary>
	void ProcessJumpEndStateTransition();
	/// <summary>
	/// �f�o�b�O�����B
	/// </summary>
	void Debug();

private:
	ModelRender				m_modelRender;					//���f�������_�[�B
	SpriteRender			m_damageSpriteRender;			//�_���[�W���󂯂��Ƃ��̃X�v���C�g�B
	CharacterController* m_characterController = nullptr;	//�L�����N�^�[�R���g���[���[�B
	AnimationClip			m_animationClips[enState_Num];	//�A�j���[�V�����N���b�v�B
	Game*					m_game = nullptr;				//�Q�[���N���X�B
	GameCamera*				m_gameCamera = nullptr;			//�Q�[���J�����B
	Vector3					m_position;						//���W�B
	Vector3					m_moveSpeed;					//�ړ����x�B
	Vector3					m_addMoveSpeed;					//�ǉ�����ړ����x�B
	Quaternion				m_rotation;						//��]�B
	PlayerState				m_playerState = enState_Idle;	//�v���C���[�X�e�[�g�B
	bool					m_isCrouch = false;				//���Ⴊ�ݏ�Ԃ��ǂ����B
	int						m_hp = 0;						//HP�B
	float					m_walkSpeed = 0.0f;				//�ړ����x�B
	float					m_gravityAccel = 0.0f;			//�d�͉����B
	float					m_damageTimer = 0.0f;			//�_���[�W�^�C�}�[�B


	FontRender a; //�f�o�b�O�p�B
};

