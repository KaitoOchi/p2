#pragma once
class Player;

/// <summary>
/// �^���b�g�B
/// </summary>
class Turret : public IGameObject
{
public:
	/// <summary>
	/// �^���b�g�X�e�[�g�B
	/// </summary>
	enum TurretState
	{
		enTurretState_Idle,		//�ҋ@�B
		enTurretState_Search,	//���G�B
		enTurretState_Shot,		//�ˌ��B
		enTurretState_Missing,	//�������B
		enTurretState_Grab,		//�͂܂��B
		enTurretState_Dead,		//���S�B
	};

public:
	Turret();
	~Turret();
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
	}

	/// <summary>
	/// �O������ݒ�B
	/// </summary>
	/// <param name="rot"></param>
	void SetForward(const Quaternion& rot)
	{
		m_rotation = rot;
		m_rotation.Apply(m_forward);
	}
	void SetForward(const Vector3& forward)
	{
		m_forward = forward;
		m_rotation.SetRotationYFromDirectionXZ(m_forward);

	}

private:
	/// <summary>
	/// �����蔻��̏����B
	/// </summary>
	void SearchPlayer();
	/// <summary>
	/// �ǂƂ̏Փ˔���B
	/// </summary>
	bool WallAndHit();
	/// <summary>
	/// �X�e�[�g�����B
	/// </summary>
	void State();
	/// <summary>
	/// �ҋ@�X�e�[�g�̑J�ڏ����B
	/// </summary>
	void ProcessIdleStateTransition();
	/// <summary>
	/// �����X�e�[�g�̑J�ڏ����B
	/// </summary>
	void ProcessSearchStateTransition();
	/// <summary>
	/// ���˃X�e�[�g�̑J�ڏ����B
	/// </summary>
	void ProcessShotStateTransition();
	/// <summary>
	/// �������X�e�[�g�̑J�ڏ����B
	/// </summary>
	void ProcessMissingStateTransition();
	/// <summary>
	/// �͂݃X�e�[�g�̑J�ڏ����B
	/// </summary>
	void ProcessGrabStateTransition();
	/// <summary>
	/// ���S�X�e�[�g�̑J�ڏ����B
	/// </summary>
	void ProcessDeadStateTransition();

private:
	ModelRender			m_modelRender;					//���f���B
	EffectEmitter*		m_laserEffect = nullptr;		//���[�U�[�G�t�F�N�g�B
	Player*				m_player = nullptr;				//�v���C���[�B
	Vector3				m_position;						//���W�B
	Vector3				m_forward;						//�O�����B
	Vector3				m_oldPlayerPos;					//�ߋ��̃v���C���[�̍��W�B
	Quaternion			m_rotation;						//��]�B
	TurretState			m_state = enTurretState_Idle;	//�X�e�[�g�B
	bool				m_isHit = false;				//�v���C���[�ƏՓ˂��Ă��邩�ǂ����B
	float				m_progressTimer = 0.0f;			//�U����̍d�����ԁB

	FontRender a;	//�f�o�b�O�p�B
};