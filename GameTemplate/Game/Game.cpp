#include "stdafx.h"
#include "Game.h"

#include "Player.h"


Game::Game()
{

}

Game::~Game()
{

}

bool Game::Start()
{
	//フレームレートを固定
	//g_engine->SetFrameRateMode(K2EngineLow::enFrameRateMode_Fix, 60);

	m_modelRender.Init("Assets/modelData/stage/tile.tkm", 0, 0, enModelUpAxisZ, false, true);
	m_modelRender.SetPosition(Vector3(0.0f, 0.0f, 0.0f));
	//m_modelRender.SetScale(Vector3(0.1f, 0.1f, 0.1f));
	m_modelRender.Update();

	m_physicsStaticObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());

	NewGO<Player>(1, "player");

	//ポイントライトを設定。
	m_pointLight.SetPointLight(
		0,
		Vector3(-100.0f, 1.0f, 100.0f),
		Vector3(1.0f, 0.0f, 0.0f),
		100.0f
	);
	RenderingEngine::GetInstance()->GetLightCB().ptNum = 1;

	//スポットライトを設定。
	m_spotLight.SetSpotLight(
		0,
		Vector3(-100.0f, 10.0f, -100.0f),
		Vector3(0.0f, 5.0f, 5.0f),
		100.0f,
		Vector3(1.0f, -1.0f, 1.0f),
		45.0f
	);
	RenderingEngine::GetInstance()->GetLightCB().spNum = 1;

	return true;
}

void Game::Update()
{

}

void Game::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}