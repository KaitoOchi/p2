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

private:
	PhysicsStaticObject		m_physicsStaticObject;		//物理静的オブジェクト。
	ModelRender				m_modelRender;				//地面モデル。
};