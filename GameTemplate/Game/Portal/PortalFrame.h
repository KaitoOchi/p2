#pragma once
class Player;

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
	/// �����蔻���\���B
	/// </summary>
	void EnableCollision()
	{
		m_collisionObject->SetIsEnable(true);
	}

	/// <summary>
	/// �����蔻����\���B
	/// </summary>
	/// <returns></returns>
	void DisableCollision()
	{
		m_collisionObject->SetIsEnable(false);
	}

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
	/// ���������Ƃ��̏����B
	/// </summary>
	void IsHit();

private:
	ModelRender			m_portalFrameModelRender;			//�|�[�^���t���[�����f���B
	ModelRender			m_portalFrameCollisionModelRender;	//�|�[�^���t���[���̓����蔻�胂�f���B
	PhysicsStaticObject m_physicsStaticObject;				//�|�[�^���̕����ÓI�I�u�W�F�N�g�B
	CollisionObject*	m_collisionObject = nullptr;		//�R���W�����B
	Player*				m_player = nullptr;					//�v���C���[�B
	PortalFrame*		m_anotherPortalFrame = nullptr;		//�ʂ̃|�[�^���B
	Vector3				m_position;							//���W�B
	Vector3				m_normal;							//�@���B
	Quaternion			m_rotation;							//��]�B
	PortalType			m_portalType = enPortalType_Num;	//�|�[�^���̃^�C�v�B
	bool				m_isEnable = false;					//�\�����Ă��邩�ǂ����B
	bool				m_isCollisionHit = false;			//�����蔻��ɓ����������ǂ����B
};