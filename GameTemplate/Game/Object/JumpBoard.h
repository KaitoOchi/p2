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
	/// 座標を設定。
	/// </summary>
	/// <param name="pos"></param>
	void SetPosition(const Vector3& pos)
	{
		m_position = pos;
	}

	/// <summary>
	/// 着地地点を設定。
	/// </summary>
	/// <param name="pos"></param>
	void SetLandingPosition(const Vector3& pos)
	{
		m_landingPos = pos;
	}

private:
	/// <summary>
	/// 当たり判定処理。
	/// </summary>
	void Collision();

	/// <summary>
	/// ジャンプ処理。
	/// </summary>
	void Jump();

private:
	ModelRender			m_modelRender;					//モデル。
	CollisionObject*	m_collisionObject = nullptr;	//コリジョン。
	Player*				m_player = nullptr;				//プレイヤー。
	Vector3				m_position;						//座標。
	Vector3				m_middlePos;					//中間座標。
	Vector3				m_landingPos;					//着地座標。
	Vector3				m_bezierPos[3];					//ベジェ座標。
	bool				m_isJump = false;				//ジャンプ中かどうか。
	float				m_timer = 0.0f;					//タイマー。
};

