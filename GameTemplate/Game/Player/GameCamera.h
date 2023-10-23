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
	void SetWarp(const Quaternion& rot);

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
	Player*		m_player = nullptr;		//プレイヤー。
	Vector3		m_position;				//座標。
	Vector3		m_targetPos;			//注視点。
	Vector3		m_rotSpeed;				//回転量。
	Vector3		m_firstCrouchPos;		//しゃがみ開始の座標。
	Vector3		m_lastCrouchPos;		//しゃがみ終了の座標。
	bool		m_isCrouch = false;		//しゃがみ状態か。
	bool		m_isDead = false;		//死亡状態か。
	float		m_crouchTimer = 0.0f;	//しゃがみ状態のタイマー。
};

