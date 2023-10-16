#include "stdafx.h"
#include "Game.h"

#include "Player.h"
#include "PortalFrame.h"
#include "PortalCamera.h"
#include "Stage.h"


Game::Game()
{
}

Game::~Game()
{
	//ポータルフレームを削除。
	QueryGOs<PortalFrame>("portalFrame", [&](PortalFrame* portalFrame) {
		DeleteGO(portalFrame);
		return true;
	});
}

bool Game::Start()
{
	//フレームレートを固定
	//g_engine->SetFrameRateMode(K2EngineLow::enFrameRateMode_Fix, 60);

	//当たり判定を有効化
	PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();

	//ポータルカメラを検索。
	PortalCamera* portalCamera = FindGO<PortalCamera>("portalCamera");

	//プレイヤーを生成。
	NewGO<Player>(1, "player");

	//ポータルを生成。
	PortalFrame* portalFrame[PORTAL_NUM];
	for (int i = 0; i < PORTAL_NUM; i++) {
		portalFrame[i] = NewGO<PortalFrame>(0, "portalFrame");

		//ポータルカメラにポータルのポインタを設定。
		portalCamera->SetPortalFramePointer(portalFrame[i], i);
	}
	portalFrame[0]->SetAnotherPortal(portalFrame[1]);
	portalFrame[0]->SetPortalType(PortalFrame::enPortalType_Blue);
	portalFrame[1]->SetAnotherPortal(portalFrame[0]);
	portalFrame[1]->SetPortalType(PortalFrame::enPortalType_Red);

	//ステージを生成。
	NewGO<Stage>(0, "stage");

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
}