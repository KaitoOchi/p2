#pragma once
class Player;
class EnergyBall;
class GameCamera;
class PortalCamera;
class Stage;

/// <summary>
/// �|�[�^���t���[���B
/// </summary>
class PortalFrame : public IGameObject
{
public:
	/// <summary>
	/// �|�[�^���̎�ށB
	/// </summary>
	enum PortalType
	{
		enPortalType_Blue,
		enPortalType_Red,
		enPortalType_Num,
	};

public:
	PortalFrame();
	~PortalFrame();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

public:
	/// <summary>
	/// �ʂ̃|�[�^����ݒ�B
	/// </summary>
	void SetAnotherPortal(PortalFrame* anotherPortalFrame)
	{
		m_anotherPortalFrame = anotherPortalFrame;
	}

	/// <summary>
	/// �|�[�^���̎�ނ�ݒ�B
	/// </summary>
	/// <param name="type"></param>
	void SetPortalType(const PortalType type)
	{
		m_portalType = type;
	}

	/// <summary>
	/// ���W���擾�B
	/// </summary>
	const Vector3& GetPosition() const
	{
		return m_position;
	}

	/// <summary>
	/// �@�����擾�B
	/// </summary>
	const Vector3& GetNormal() const
	{
		return m_normal;
	}

	/// <summary>
	/// ��]���擾�B
	/// </summary>
	const Quaternion& GetRotation() const
	{
		return m_rotation;
	}

	/// <summary>
	/// �\����Ԃ��擾�B
	/// </summary>
	const bool GetIsEnable() const
	{
		return m_isEnable;
	}

	/// <summary>
	/// ���Z�b�g�����B
	/// </summary>
	void Reset();

	/// <summary>
	/// �|�[�^���t���[���̐ݒu�����B
	/// </summary>
	void SetPortalFrame(const Vector3& pos, const Vector3& normal);

private:
	/// <summary>
	/// �����蔻��̏����B
	/// </summary>
	void Collision();
	/// <summary>
	/// �v���C���[�Ƃ̓����蔻��B
	/// </summary>
	void CollisionPlayer();
	/// <summary>
	/// �G�l���M�[�{�[���Ƃ̓����蔻��B
	/// </summary>
	void CollisionEnergyBall();
	/// <summary>
	/// �v���C���[�ɓ��������Ƃ��̏����B
	/// </summary>
	void IsHitPlayer();
	/// <summary>
	/// �G�l���M�[�{�[���ɓ��������Ƃ��̏����B
	/// </summary>
	void IsHitEnergyBall(EnergyBall* ball);
	/// <summary>
	/// �|�[�^�����ݒu�ł��邩�ǂ����B
	/// </summary>
	/// <param name="pos"></param>
	/// <param name="normal"></param>
	/// <returns></returns>
	const bool IsCanPut(const Vector3& pos, const Vector3& normal);

private:
	ModelRender					m_portalFrameModelRender;			//�|�[�^���t���[�����f���B
	ModelRender					m_portalFrameCollisionModelRender;	//�|�[�^���t���[���̓����蔻�胂�f���B
	PhysicsStaticObject			m_physicsStaticObject;				//�|�[�^���̕����ÓI�I�u�W�F�N�g�B
	CollisionObject*			m_collisionObject = nullptr;		//�R���W�����B
	EffectEmitter*				m_frameEffect = nullptr;			//�g�g�݃G�t�F�N�g�B
	Player*						m_player = nullptr;					//�v���C���[�B
	GameCamera*					m_gameCamera = nullptr;				//�Q�[���J�����B
	PortalFrame*				m_anotherPortalFrame = nullptr;		//�ʂ̃|�[�^���B
	PortalCamera*				m_portalCamera = nullptr;			//�|�[�^���J�����B
	Stage*						m_stage = nullptr;					//�X�e�[�W�B
	Vector3						m_displayPosition;					//�|�[�^���̕\���ʒu
	Vector3						m_position;							//���W�B
	Vector3						m_normal;							//�@���B
	Quaternion					m_rotation;							//��]�B
	PortalType					m_portalType = enPortalType_Num;	//�|�[�^���̃^�C�v�B
	bool						m_isEnable = false;					//�\�����Ă��邩�ǂ����B
	bool						m_isCollisionHit = false;			//�����蔻��ɓ����������ǂ����B
};