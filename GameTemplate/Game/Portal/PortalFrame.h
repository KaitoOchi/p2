#pragma once
class Player;

/// <summary>
/// ポータルフレーム。
/// </summary>
class PortalFrame : public IGameObject
{
public:
	/// <summary>
	/// ポータルの種類。
	/// </summary>
	enum PortalType
	{
		enPortalType_Blue,
		enPortalType_Red,
		enPortalType_Num,
	};

public:
	PortalFrame();
	~PortalFrame();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

public:
	/// <summary>
	/// 別のポータルを設定。
	/// </summary>
	void SetAnotherPortal(PortalFrame* anotherPortalFrame)
	{
		m_anotherPortalFrame = anotherPortalFrame;
	}

	/// <summary>
	/// ポータルの種類を設定。
	/// </summary>
	/// <param name="type"></param>
	void SetPortalType(const PortalType type)
	{
		m_portalType = type;
	}

	/// <summary>
	/// 座標を取得。
	/// </summary>
	const Vector3& GetPosition() const
	{
		return m_position;
	}

	/// <summary>
	/// 法線を取得。
	/// </summary>
	const Vector3& GetNormal() const
	{
		return m_normal;
	}

	/// <summary>
	/// 回転を取得。
	/// </summary>
	const Quaternion& GetRotation() const
	{
		return m_rotation;
	}

	/// <summary>
	/// 表示状態を取得。
	/// </summary>
	const bool GetIsEnable() const
	{
		return m_isEnable;
	}

	/// <summary>
	/// 当たり判定を表示。
	/// </summary>
	void EnableCollision()
	{
		m_collisionObject->SetIsEnable(true);
	}

	/// <summary>
	/// 当たり判定を非表示。
	/// </summary>
	/// <returns></returns>
	void DisableCollision()
	{
		m_collisionObject->SetIsEnable(false);
	}

	/// <summary>
	/// ポータルフレームの設置処理。
	/// </summary>
	void SetPortalFrame(const Vector3& pos, const Vector3& normal);

private:
	/// <summary>
	/// 当たり判定の処理。
	/// </summary>
	void Collision();

	/// <summary>
	/// 当たったときの処理。
	/// </summary>
	void IsHit();

private:
	ModelRender			m_portalFrameModelRender;			//ポータルフレームモデル。
	ModelRender			m_portalFrameCollisionModelRender;	//ポータルフレームの当たり判定モデル。
	PhysicsStaticObject m_physicsStaticObject;				//ポータルの物理静的オブジェクト。
	CollisionObject*	m_collisionObject = nullptr;		//コリジョン。
	Player*				m_player = nullptr;					//プレイヤー。
	PortalFrame*		m_anotherPortalFrame = nullptr;		//別のポータル。
	Vector3				m_position;							//座標。
	Vector3				m_normal;							//法線。
	Quaternion			m_rotation;							//回転。
	PortalType			m_portalType = enPortalType_Num;	//ポータルのタイプ。
	bool				m_isEnable = false;					//表示しているかどうか。
	bool				m_isCollisionHit = false;			//当たり判定に当たったかどうか。
};