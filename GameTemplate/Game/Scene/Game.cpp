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

	int num = 0;

	for (auto& light : m_pointLight)
	{
		float x = Math::Random(-500, 500);
		float y = Math::Random(10, 50);
		float z = Math::Random(-500, 500);
		float r = Math::Random(0, 1);
		float g = Math::Random(0, 1);
		float b = Math::Random(0, 1);
		float range = Math::Random(50, 200);

		Vector3 color = Vector3(r, g, b);
		color.Div(2.0f);

		//ポイントライトを設定。
		light.SetPointLight(
			num,
			Vector3(x, y, z),
			color,
			range
		);

		num++;
	}
	RenderingEngine::GetInstance()->GetLightCB().ptNum = POINT_LIGHT_NUM;

	num = 0;

	for (auto& light : m_spotLight)
	{
		float x = Math::Random(-500, 500);
		float y = Math::Random(5, 10);
		float z = Math::Random(-500, 500);
		float r = Math::Random(0, 1);
		float g = Math::Random(0, 1);
		float b = Math::Random(0, 1);
		float range = Math::Random(50, 200);

		float dx = Math::Random(-1, 1);
		float dy = Math::Random(-1, 1);
		float dz = Math::Random(-1, 1);
		float angle = Math::Random(45, 90);

		Vector3 color = Vector3(r, g, b);

		//スポットライトを設定。
		light.SetSpotLight(
			num,
			Vector3(x, y, z),
			color,
			range,
			Vector3(dx, dy, dz),
			angle
		);

		num++;
	}
	RenderingEngine::GetInstance()->GetLightCB().ptNum = SPOT_LIGHT_NUM;

	return true;
}

void Game::Update()
{
	// ライトを回す
	Quaternion qRot;
	qRot.SetRotationDegY(1.0f);

	int num = 0;

	for (auto& light : m_pointLight)
	{
		Vector3 pos = light.GetPosition();
		qRot.Apply(pos);
		light.SetPosition(pos);
		light.Update(num);

		num++;
	}

	num = 0;

	for (auto& light : m_spotLight)
	{
		Vector3 pos = light.GetPosition();
		qRot.Apply(pos);
		light.SetPosition(pos);
		light.Update(num);

		num++;
	}

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