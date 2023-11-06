#pragma once

/// <summary>
/// ステージクラス。
/// </summary>
class Stage : public IGameObject
{
public:
	Stage();
	~Stage();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

public:
	/// <summary>
	/// 復活地点を取得。
	/// </summary>
	/// <returns></returns>
	const Vector3& GetRespawnPoint() const
	{
		return m_respawnPoint;
	}

private:
	PhysicsStaticObject		m_physicsStaticObject;		//物理静的オブジェクト。
	ModelRender				m_modelRender;				//地面モデル。
	Vector3					m_respawnPoint;				//リスポーン地点。
};