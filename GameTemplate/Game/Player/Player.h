#pragma once
class PortalGun;
class GameCamera;
class Game;

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
		enState_Jump,			//ジャンプ。
		enState_JumpEnd,		//ジャンプ終了。
		enState_Dead,			//死亡。
		enState_End,			//終了。
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
	/// リセット処理。
	/// </summary>
	void Reset();

	/// <summary>
	/// 追加の移動速度を設定。
	/// </summary>
	/// <param name="moveSpeed"></param>
	void SetAddMoveSpeed(const Vector3& moveSpeed)
	{
		m_addMoveSpeed = moveSpeed;
	}

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
		m_characterController->SetIsPortal(flag);
	}

	/// <summary>
	/// プレイヤーステートを設定。
	/// </summary>
	/// <param name="state"></param>
	void SetPlayerState(const PlayerState& state)
	{
		m_playerState = state;
	}

	/// <summary>
	/// プレイヤーステートを取得。
	/// </summary>
	/// <returns></returns>
	const PlayerState GetPlayerState() const
	{
		return m_playerState;
	}

	/// <summary>
	/// キャラコンを取得。
	/// </summary>
	const CharacterController* GetCharacterController() const
	{
		return m_characterController;
	}

	/// <summary>
	/// 当たり判定を取得。
	/// </summary>
	/// <returns></returns>
	btCollisionObject* GetCollision()
	{
		return m_characterController->GetRigidBody()->GetBody();
	}

	/// <summary>
	/// ポータルへのワープ処理。
	/// </summary>
	void SetWarp(const Vector3& pos, const float angle);

private:
	//アニメーションイベント用の関数
	void OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName);

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
	/// しゃがみ処理。
	/// </summary>
	void Crouch();
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
	/// ジャンプ状態の遷移処理。
	/// </summary>
	void ProcessJumpStateTransition();
	/// <summary>
	/// ジャンプ終了状態の遷移処理。
	/// </summary>
	void ProcessJumpEndStateTransition();
	/// <summary>
	/// デバッグ処理。
	/// </summary>
	void Debug();

private:
	ModelRender				m_modelRender;					//モデルレンダー。
	SpriteRender			m_damageSpriteRender;			//ダメージを受けたときのスプライト。
	CharacterController* m_characterController = nullptr;	//キャラクターコントローラー。
	AnimationClip			m_animationClips[enState_Num];	//アニメーションクリップ。
	Game*					m_game = nullptr;				//ゲームクラス。
	GameCamera*				m_gameCamera = nullptr;			//ゲームカメラ。
	Vector3					m_position;						//座標。
	Vector3					m_moveSpeed;					//移動速度。
	Vector3					m_addMoveSpeed;					//追加する移動速度。
	Quaternion				m_rotation;						//回転。
	PlayerState				m_playerState = enState_Idle;	//プレイヤーステート。
	bool					m_isCrouch = false;				//しゃがみ状態かどうか。
	int						m_hp = 0;						//HP。
	float					m_walkSpeed = 0.0f;				//移動速度。
	float					m_gravityAccel = 0.0f;			//重力加速。
	float					m_damageTimer = 0.0f;			//ダメージタイマー。


	FontRender a; //デバッグ用。
};

