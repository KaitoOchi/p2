#pragma once
class Player;

/// <summary>
/// ゲームカメラ。
/// </summary>
class GameCamera : public IGameObject
{
public:
	GameCamera();
	~GameCamera();
	bool Start();
	void Update();

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
	/// 回転率を取得。
	/// </summary>
	/// <returns></returns>
	const Vector3& GetRotRadian() const
	{
		return m_rotSpeed;
	}

	/// <summary>
	/// しゃがみ状態かどうかを設定。
	/// </summary>
	/// <param name="isCrouch"></param>
	void SetCrouchState(const bool isCrouch)
	{
		m_isCrouch = isCrouch;
	}

	/// <summary>
	/// ポータルへのワープ処理。
	/// </summary>
	/// <param name="angle">回転量</param>
	void SetWarp(const float angle);

	/// <summary>
	/// 死亡状態にする。
	/// </summary>
	void Dead()
	{
		m_isCrouch = false;
		m_isDead = true;
		g_camera3D->Move(Vector3(0.0f, -50.0f, 0.0f));
		g_camera3D->SetUp(Vector3(-1.0f, 0.0f, 0.0f));
		g_camera3D->Update();
	}

	/// <summary>
	/// リセット処理。
	/// </summary>
	void Reset()
	{
		m_isDead = false;
		g_camera3D->SetUp(Vector3(0.0f, 1.0f, 0.0f));
		g_camera3D->Update();
	}

private:
	/// <summary>
	/// 移動処理。
	/// </summary>
	void Move();
	/// <summary>
	/// 回転処理。
	/// </summary>
	void Rotation();

private:
	Vector3		m_position;				//座標。
	Vector3		m_targetPos;			//注視点。
	Vector3		m_rotSpeed;				//回転量。
	Vector3		m_firstCrouchPos;		//しゃがみ開始の座標。
	Vector3		m_lastCrouchPos;		//しゃがみ終了の座標。
	bool		m_isCrouch = false;		//しゃがみ状態か。
	bool		m_isDead = false;		//死亡状態か。
	float		m_crouchTimer = 0.0f;	//しゃがみ状態のタイマー。
};

