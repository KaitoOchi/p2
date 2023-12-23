#pragma once
#include "LevelRender/Level3DRender.h"
class Gimmick;
class EnergyBall;

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
	/// 復活座標を取得。
	/// </summary>
	const Vector3& GetRespawnPosition() const
	{
		return m_respawnPosition;
	}

	/// <summary>
	/// クリア座標を取得。
	/// </summary>
	const Vector3& GetClearPosition() const
	{
		return m_clearPosition;
	}

	/// <summary>
	/// 物理静的オブジェクトの取得。
	/// </summary>
	/// <returns></returns>
	PhysicsStaticObject& GetPhysicsStaticObject()
	{
		return m_physicsStaticObject;
	}

	/// <summary>
	/// エネルギーボールオブジェクトを配列に追加。
	/// </summary>
	/// <param name="obj"></param>
	void AddEnergyBallObject(EnergyBall* obj)
	{
		m_energyBallObjects.emplace_back(obj);
	}

	/// <summary>
	/// エネルギーボールオブジェクトを配列から削除。
	/// </summary>
	/// <param name="obj"></param>
	void RemoveEnergyBallObject(EnergyBall* obj)
	{
		for (int i = 0; i < m_energyBallObjects.size(); i++) {
			if (m_energyBallObjects[i] == obj) {
				m_energyBallObjects.erase(m_energyBallObjects.begin() + i);
			}
		}
	}

	/// <summary>
	/// エネルギーボールオブジェクト配列を取得。
	/// </summary>
	/// <returns></returns>
	std::vector<EnergyBall*> GetEnergyBallObjects()
	{
		return m_energyBallObjects;
	}

	/// <summary>
	/// ギミックを解除。
	/// </summary>
	/// <param name="num"></param>
	void ClearGimmickObject(const int num);

private:
	/// <summary>
	/// レベルデザインを初期化。
	/// </summary>
	void InitLevelRender();

private:
	std::vector<Gimmick*>		m_gimmickObjects;			//解除用のギミックオブジェクト。
	std::vector<EnergyBall*>	m_energyBallObjects;		//エネルギーボールオブジェクト。
	std::vector<PointLight*>	m_pointLightObjects;		//ポイントライトオブジェクト。
	std::vector<SpotLight*>		m_spotLightObjects;			//スポットライトオブジェクト。
	PhysicsStaticObject			m_physicsStaticObject;		//物理静的オブジェクト。
	ModelRender					m_decorationModelRender;	//装飾モデル。
	ModelRender					m_physicsModelRender;		//物理モデル。
	LevelRender					m_levelRender;				//レベルレンダー。
	Vector3						m_respawnPosition;			//リスポーン座標。
	Vector3						m_clearPosition;			//クリア座標。
};