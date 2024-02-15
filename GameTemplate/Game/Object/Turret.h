#pragma once
class Player;

/// <summary>
/// タレット。
/// </summary>
class Turret : public IGameObject
{
public:
	/// <summary>
	/// タレットステート。
	/// </summary>
	enum TurretState
	{
		enTurretState_Idle,		//待機。
		enTurretState_Search,	//索敵。
		enTurretState_Shot,		//射撃。
		enTurretState_Missing,	//見失う。
		enTurretState_Grab,		//掴まれる。
		enTurretState_Dead,		//死亡。
	};

public:
	Turret();
	~Turret();
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
	/// 回転を設定。
	/// </summary>
	/// <param name="rot"></param>
	void SetRotation(const Quaternion& rot)
	{
		m_rotation = rot;
	}

	/// <summary>
	/// 前方向を設定。
	/// </summary>
	/// <param name="rot"></param>
	void SetForward(const Quaternion& rot)
	{
		m_rotation = rot;
		m_rotation.Apply(m_forward);
	}
	void SetForward(const Vector3& forward)
	{
		m_forward = forward;
		m_rotation.SetRotationYFromDirectionXZ(m_forward);

	}

private:
	/// <summary>
	/// 当たり判定の処理。
	/// </summary>
	void SearchPlayer();
	/// <summary>
	/// 壁との衝突判定。
	/// </summary>
	bool WallAndHit();
	/// <summary>
	/// ステート処理。
	/// </summary>
	void State();
	/// <summary>
	/// 待機ステートの遷移処理。
	/// </summary>
	void ProcessIdleStateTransition();
	/// <summary>
	/// 発見ステートの遷移処理。
	/// </summary>
	void ProcessSearchStateTransition();
	/// <summary>
	/// 発射ステートの遷移処理。
	/// </summary>
	void ProcessShotStateTransition();
	/// <summary>
	/// 見失うステートの遷移処理。
	/// </summary>
	void ProcessMissingStateTransition();
	/// <summary>
	/// 掴みステートの遷移処理。
	/// </summary>
	void ProcessGrabStateTransition();
	/// <summary>
	/// 死亡ステートの遷移処理。
	/// </summary>
	void ProcessDeadStateTransition();

private:
	ModelRender			m_modelRender;					//モデル。
	EffectEmitter*		m_laserEffect = nullptr;		//レーザーエフェクト。
	Player*				m_player = nullptr;				//プレイヤー。
	Vector3				m_position;						//座標。
	Vector3				m_forward;						//前方向。
	Vector3				m_oldPlayerPos;					//過去のプレイヤーの座標。
	Quaternion			m_rotation;						//回転。
	TurretState			m_state = enTurretState_Idle;	//ステート。
	bool				m_isHit = false;				//プレイヤーと衝突しているかどうか。
	float				m_progressTimer = 0.0f;			//攻撃後の硬直時間。

	FontRender a;	//デバッグ用。
};