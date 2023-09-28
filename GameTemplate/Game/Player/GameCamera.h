#pragma once

class GameCamera : Noncopyable
{
public:
	GameCamera();
	~GameCamera();

	/// <summary>
	/// 初期化処理。
	/// </summary>
	void Init();

	/// <summary>
	/// 更新処理。
	/// </summary>
	void Update();

	/// <summary>
	/// 移動処理。
	/// </summary>
	void SetPosition(const Vector3& pos);

	/// <summary>
	/// 回転処理。
	/// </summary>
	void SetRotation();

	/// <summary>
	/// しゃがみ状態かどうかを設定。
	/// </summary>
	/// <param name="isCrouch"></param>
	void SetCrouchState(const bool isCrouch)
	{
		m_isCrouch = isCrouch;
	}

private:
	Vector3		m_position;				//座標。
	Vector3		m_targetPos;			//注視点。
	Vector3		m_rotSpeed;				//回転量。
	Vector3		m_firstCrouchPos;		//しゃがみ開始の座標。
	Vector3		m_lastCrouchPos;		//しゃがみ終了の座標。			
	bool		m_isCrouch;				//しゃがみ状態か。
	float		m_crouchTimer = 0.0f;	//しゃがみ状態のタイマー。
};

