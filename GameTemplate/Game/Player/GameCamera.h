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

	/// <summary>
	/// 移動処理。
	/// </summary>
	void Move();

	/// <summary>
	/// 回転処理。
	/// </summary>
	void Rotation();

	/// <summary>
	/// しゃがみ状態かどうかを設定。
	/// </summary>
	/// <param name="isCrouch"></param>
	void SetCrouchState(const bool isCrouch)
	{
		m_isCrouch = isCrouch;
	}

private:
	Player*		m_player = nullptr;
	Vector3		m_position;				//座標。
	Vector3		m_targetPos;			//注視点。
	Vector3		m_rotSpeed;				//回転量。
	Vector3		m_firstCrouchPos;		//しゃがみ開始の座標。
	Vector3		m_lastCrouchPos;		//しゃがみ終了の座標。
	bool		m_isCrouch;				//しゃがみ状態か。
	float		m_crouchTimer = 0.0f;	//しゃがみ状態のタイマー。
};

