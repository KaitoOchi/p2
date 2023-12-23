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
	void Dead();

	/// <summary>
	/// リセット処理。
	/// </summary>
	void Reset();

	/// <summary>
	/// 終了処理。
	/// </summary>
	void End();

private:
	/// <summary>
	/// 移動処理。
	/// </summary>
	void Move();
	/// <summary>
	/// 回転処理。
	/// </summary>
	void Rotation();
	/// <summary>
	/// 画面揺れの処理。
	/// </summary>
	void Shake();

private:
	Vector3		m_position;				//座標。
	Vector3		m_targetPos;			//注視点。
	Vector3		m_rotSpeed;				//回転量。
	Vector3		m_firstCrouchPos;		//しゃがみ開始の座標。
	Vector3		m_lastCrouchPos;		//しゃがみ終了の座標。
	Vector3		m_shakeMoveSpeedTmp;	//画面揺れの保存用。
	bool		m_isCrouch = false;		//しゃがみ状態か。
	bool		m_isDead = false;		//死亡状態か。
	bool		m_isIdle = false;		//待機状態か。
	float		m_crouchTimer = 0.0f;	//しゃがみ状態のタイマー。
	float		m_shakeTimer = 0.0f;	//画面揺れ用タイマー。
};

