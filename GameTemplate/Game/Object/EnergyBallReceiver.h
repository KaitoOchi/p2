#pragma once
#include "Gimmick.h"

class EnergyBallReceiver : public Gimmick
{
public:
	EnergyBallReceiver();
	~EnergyBallReceiver() override;
	bool Start();
	void Update();
	void Render(RenderContext& rc);

public:
	/// <summary>
	/// ギミックの成功処理。
	/// </summary>
	void Clear() override;

private:
	Stage*				m_stage = nullptr;				//ステージ。
	CollisionObject*	m_collisionObject = nullptr;	//コリジョン。
};

