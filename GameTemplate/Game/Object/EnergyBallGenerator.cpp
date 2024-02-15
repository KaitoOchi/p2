#include "stdafx.h"
#include "EnergyBallGenerator.h"
#include "EnergyBall.h"

namespace
{
	const float GENERATE_TIME = 3.0f;		//生成にかかる時間。
}

EnergyBallGenerator::EnergyBallGenerator()
{
	m_direction = Vector3(0.0f, 0.0f, -1.0f);
}

EnergyBallGenerator::~EnergyBallGenerator()
{

}

bool EnergyBallGenerator::Start()
{
	//エネルギーボールを生成。
	m_energyBall = NewGO<EnergyBall>(1, "energyBall");
	m_energyBall->SetGenerator(this);

	return true;
}

void EnergyBallGenerator::Update()
{
	if (m_isClear) {
		return;
	}

	//エネルギーボールが消失したら。
	if (!m_isEnableEnergyBall) {

		m_generateTimer += g_gameTime->GetFrameDeltaTime();

		if (m_generateTimer > GENERATE_TIME) {
			//エネルギーボールを有効にする。
			m_energyBall->Enable();
			m_energyBall->SetPosition(m_position);
			m_energyBall->SetDirection(m_direction);
			m_isEnableEnergyBall = true;
		}
	}
}

void EnergyBallGenerator::Enable()
{
	m_isClear = false;
}

void EnergyBallGenerator::Disable()
{
	m_isClear = true;
}

void EnergyBallGenerator::Render(RenderContext& rc)
{

}