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
	//タレットの射撃エフェクト。
	EffectEngine::GetInstance()->ResistEffect(EffectEmitter::enEffect_Turret_Shot, u"Assets/effect/turret/turret_shot.efk");
	//エナジーボールのエフェクト。
	EffectEngine::GetInstance()->ResistEffect(EffectEmitter::enEffect_EnergyBall, u"Assets/effect/energyBall/energyBall.efk");


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
		portalFrame[i] = NewGO<PortalFrame>(2, "portalFrame");

		//ポータルカメラにポータルのポインタを設定。
		portalCamera->SetPortalFramePointer(portalFrame[i], i);
	}
	portalFrame[0]->SetAnotherPortal(portalFrame[1]);
	portalFrame[0]->SetPortalType(PortalFrame::enPortalType_Blue);
	portalFrame[1]->SetAnotherPortal(portalFrame[0]);
	portalFrame[1]->SetPortalType(PortalFrame::enPortalType_Red);

	//ステージを生成。
	m_stage = NewGO<Stage>(0, "stage");

	//ポイントライトを設定。
	m_pointLight.SetPointLight(
		0,
		Vector3(-100.0f, 50.0f, -100.0f),
		Vector3(0.0f, 0.0f, 1.0f),
		200.0f
	);
	RenderingEngine::GetInstance()->GetLightCB().ptNum = 1;

	//スポットライトを設定。
	m_spotLight.SetSpotLight(
		0,
		Vector3(-100.0f, 20.0f, 100.0f),
		Vector3(0.0f, 2.0f, 2.0f),
		300.0f,
		Vector3(1.0f, -1.0f, 1.0f),
		70.0f
	);
	RenderingEngine::GetInstance()->GetLightCB().spNum = 1;

	return true;
}

void Game::Update()
{

}

void Game::NotifyReset()
{

}

const Vector3& Game::GetRespawnPoint() const
{
	return m_stage->GetRespawnPoint();
}

void Game::Render(RenderContext& rc)
{
}