#pragma once
class PortalGun;
class GameCamera;

/// <summary>
/// プレイヤー。
/// </summary>
class Player : public IGameObject
{
public:
	/// <summary>
	/// プレイヤーステート。
	/// </summary>
	enum PlayerState
	{
		enState_Idle,
		enState_Walk,
		enState_Run,
		enState_Crouch_Idle,
		enState_Crouch_Walk,
		enState_Crouch_Jump,
		enState_Jump,
		enState_Num,
	};

public:
	Player();
	~Player();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

public:
	/// <summary>
	/// 座標を取得。
	/// </summary>
	const Vector3& GetPosition() const
	{
		return m_position;
	}

	/// <summary>
	/// 回転を取得。
	/// </summary>
	const Quaternion& GetRotation() const
	{
		return m_rotation;
	}

private:
	/// <summary>
	/// 入力処理。
	/// </summary>
	void Input();

	/// <summary>
	/// XZ軸の移動処理。
	/// </summary>
	void MoveXZ();

	/// <summary>
	/// Y軸の移動処理。
	/// </summary>
	void MoveY();

	/// <summary>
	/// 回転処理。
	/// </summary>
	void Rotation();

	/// <summary>
	/// ポータルの発射処理。
	/// </summary>
	void Shot();

	/// <summary>
	/// アニメーション処理。
	/// </summary>
	void PlayAnimation();

	/// <summary>
	/// ステート処理。
	/// </summary>
	void State();

	/// <summary>
	/// 共通遷移処理。
	/// </summary>
	void ProcessCommonStateTransition();

	/// <summary>
	/// 待機状態の遷移処理。
	/// </summary>
	void ProcessIdleStateTransition();

	/// <summary>
	/// 歩き状態の遷移処理。
	/// </summary>
	void ProcessWalkStateTransition();

	/// <summary>
	/// 走り状態の遷移処理。
	/// </summary>
	void ProcessRunStateTransition();

	/// <summary>
	/// しゃがみ待機状態の遷移処理。
	/// </summary>
	void ProcessCrouchIdleStateTransition();

	/// <summary>
	/// しゃがみ歩き状態の遷移処理。
	/// </summary>
	void ProcessCrouchWalkStateTransition();

	/// <summary>
	/// しゃがみジャンプ状態の遷移処理。
	/// </summary>
	void ProcessCrouchJumpStateTransition();

	/// <summary>
	/// ジャンプ状態の遷移処理。
	/// </summary>
	void ProcessJumpStateTransition();

private:
	ModelRender				m_modelRender;					//モデルレンダー。
	CharacterController		m_characterController;			//キャラクターコントローラー。
	AnimationClip			m_animationClips[enState_Num];	//アニメーションクリップ。

	PortalGun*				m_portalGun = nullptr;			//ポータルガン。
	GameCamera*				m_gameCamera = nullptr;			//ゲームカメラ。

	Vector3					m_position;						//座標。
	Vector3					m_moveSpeed;					//移動速度。
	Quaternion				m_rotation;						//回転。
	PlayerState				m_playerState = enState_Idle;	//プレイヤーステート。

	float					m_walkSpeed = 0.0f;				//移動速度。
	float					m_gravityAccel = 0.0f;			//重力加速。


	FontRender a;
};

