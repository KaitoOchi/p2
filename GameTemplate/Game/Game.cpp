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

	m_modelRender.Init("Assets/modelData/stage/debug.tkm");
	m_modelRender.SetPosition(Vector3(0.0f, 0.0f, 0.0f));
	m_modelRender.SetScale(Vector3(0.1f, 0.1f, 0.1f));
	m_modelRender.Update();

	NewGO<Player>(0, "player");

	return true;
}

void Game::Update()
{

}

void Game::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}