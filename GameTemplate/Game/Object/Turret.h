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
	ModelRender		m_modelRender;			//モデル。
	Player*			m_player = nullptr;		//プレイヤー。
	Vector3			m_position;				//座標。
	Vector3			m_forward;				//前方向。
	Quaternion		m_rotation;				//回転。
	float			m_damageTimer = 0.0f;	//攻撃後の硬直時間。
};