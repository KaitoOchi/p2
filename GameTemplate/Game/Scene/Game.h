#pragma once

/// <summary>
/// ゲームメインクラス。
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
	PointLight				m_pointLight;				//ポイントライト。
	SpotLight				m_spotLight;				//スポットライト。
};

