#pragma once
class Stage;

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

public:
	/// <summary>
	/// リセットを通知。
	/// </summary>
	void NotifyReset();

	/// <summary>
	/// 復活地点を取得。
	/// </summary>
	/// <returns></returns>
	const Vector3& GetRespawnPoint() const;

private:
	Stage*					m_stage = nullptr;			//ステージ。
	PointLight				m_pointLight;				//ポイントライト。
	SpotLight				m_spotLight;				//スポットライト。
};

