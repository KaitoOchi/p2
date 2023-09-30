#pragma once

/// <summary>
/// �Q�[�����C���N���X�B
/// </summary>
class Game : public IGameObject
{
public:
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

private:
	PhysicsStaticObject		m_physicsStaticObject;
	ModelRender				m_modelRender;
	PointLight				m_pointLight;
	SpotLight				m_spotLight;
};

