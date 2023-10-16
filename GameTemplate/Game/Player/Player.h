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
		enState_Idle,			//待機。
		enState_Walk,			//歩き。
		enState_Run,			//走り。
		enState_Crouch_Idle,	//しゃがみ待機。
		enState_Crouch_Walk,	//しゃがみ歩き。
		enState_Crouch_Jump,	//しゃがみジャンプ。
		enState_Jump,			//ジャンプ。
		enState_Dead,
		enState_Num,			//数。
	};

public:
	Player();
	~Player();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

public:
	/// <summary>
	/// 受けるダメージを設定。
	/// </summary>
	/// <param name="damage">受けるダメージ量</param>
	/// <param name="dir">のけぞる方向</param>
	void ReceiveDamage(const int damage, const Vector3& dir);

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

	/// <summary>
	/// ポータルに入っている状態を設定。
	/// </summary>
	/// <param name="flag"></param>
	void SetIsPortal(const bool flag)
	{
		m_characterController.SetIsPortal(flag);
	}

	/// <summary>
	/// キャラコンを取得。
	/// </summary>
	const CharacterController& GetCharacterController() const
	{
		return m_characterController;
	}

	/// <summary>
	/// 当たり判定を取得。
	/// </summary>
	/// <returns></returns>
	btCollisionObject* GetCollision()
	{
		return m_characterController.GetRigidBody()->GetBody();
	}

	/// <summary>
	/// ポータルへのワープ処理。
	/// </summary>
	void SetWarp(const Vector3& pos, const Quaternion& rot);

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

	/// <summary>
	/// 死亡状態の遷移処理。
	/// </summary>
	void ProcessDeadStateTransition();

private:
	ModelRender				m_modelRender;					//モデルレンダー。
	CharacterController		m_characterController;			//キャラクターコントローラー。
	AnimationClip			m_animationClips[enState_Num];	//アニメーションクリップ。
	PortalGun*				m_portalGun = nullptr;			//ポータルガン。
	GameCamera*				m_gameCamera = nullptr;			//ゲームカメラ。
	Vector3					m_position;						//座標。
	Vector3					m_moveSpeed;					//移動速度。
	Vector3					m_addMoveSpeed;					//追加する移動速度。
	Quaternion				m_rotation;						//回転。
	PlayerState				m_playerState = enState_Idle;	//プレイヤーステート。
	int						m_hp = 0;						//HP。
	float					m_walkSpeed = 0.0f;				//移動速度。
	float					m_gravityAccel = 0.0f;			//重力加速。
	float					m_deadTimer = 0.0f;				//死亡時間。


	FontRender a; //デバッグ用。
};

