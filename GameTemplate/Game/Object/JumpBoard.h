#pragma once
class Player;

class JumpBoard : public IGameObject
{
public:
	JumpBoard();
	~JumpBoard();
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
	/// ���n�n�_��ݒ�B
	/// </summary>
	/// <param name="pos"></param>
	void SetLandingPosition(const Vector3& pos)
	{
		m_landingPos = pos;
	}

private:
	/// <summary>
	/// �����蔻�菈���B
	/// </summary>
	void Collision();

	/// <summary>
	/// �W�����v�����B
	/// </summary>
	void Jump();

private:
	ModelRender			m_modelRender;					//���f���B
	CollisionObject*	m_collisionObject = nullptr;	//�R���W�����B
	Player*				m_player = nullptr;				//�v���C���[�B
	Vector3				m_position;						//���W�B
	Vector3				m_middlePos;					//���ԍ��W�B
	Vector3				m_landingPos;					//���n���W�B
	Vector3				m_bezierPos[3];					//�x�W�F���W�B
	bool				m_isJump = false;				//�W�����v�����ǂ����B
	float				m_timer = 0.0f;					//�^�C�}�[�B
};

