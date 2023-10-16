#pragma once
class Player;

class Turret : public IGameObject
{
public:
	Turret();
	~Turret();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

public:
	
	void SetPosition(const Vector3& pos)
	{
		m_position = pos;
	}

private:
	ModelRender		m_modelRender;			//���f���B
	Player*			m_player = nullptr;		//�v���C���[�B
	Vector3			m_position;				//���W�B
	Vector3			m_forward;				//�O�����B
	Quaternion		m_rotation;				//��]�B
	float			m_damageTimer = 0.0f;	//�U����̍d�����ԁB
};