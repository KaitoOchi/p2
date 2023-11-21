#include "stdafx.h"
#include "Stage.h"

#include "Gimmick.h"
#include "Turret.h"
#include "EnergyBall.h"
#include "EnergyBallGenerator.h"
#include "EnergyBallReceiver.h"
#include "Door.h"

Stage::Stage()
{
	m_gimmickObjects.reserve(4);
}

Stage::~Stage()
{
	//タレットを削除。
	QueryGOs<Turret>("turret", [&](Turret* turret) {
		DeleteGO(turret);
		return true;
		});

	//エネルギーボールを削除。
	QueryGOs<EnergyBall>("energyBall", [&](EnergyBall* energyBall) {
		DeleteGO(energyBall);
		return true;
		});

	//エネルギーボール生成機を削除。
	QueryGOs<EnergyBallGenerator>("energyBallGenerator", [&](EnergyBallGenerator* energyBallGenerator) {
		DeleteGO(energyBallGenerator);
		return true;
		});

	//エネルギーボール受け取り機を削除。
	QueryGOs<EnergyBallReceiver>("energyBallReceiver", [&](EnergyBallReceiver* energyBallReceiver) {
		DeleteGO(energyBallReceiver);
		return true;
		});

	//ドアを削除。
	QueryGOs<Door>("door", [&](Door* door) {
		DeleteGO(door);
		return true;
		});

	m_gimmickObjects.clear();
}

bool Stage::Start()
{
	//地面モデルの設定。
	m_modelRender.Init("Assets/modelData/stage/tile.tkm", 0, 0, enModelUpAxisZ, true, true);
	//モデルから静的オブジェクトを作成。
	m_physicsStaticObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());

	InitLevelRender();

	return true;
}

void Stage::InitLevelRender()
{
	// レベルデザイン処理。
	m_levelRender.Init("Assets/level/stage/level00.tkl", [&](LevelObjectData& objData) {
		//名前がプレイヤーの時。
		if (objData.EqualObjectName(L"Player") == true) {
			//初期座標を設定。
			m_respawnPoint = objData.position;
			return true;
		}
		//名前がタレットの時。
		if (objData.EqualObjectName(L"Turret") == true) {
			Turret* turret = NewGO<Turret>(0, "turret");
			turret->SetPosition(objData.position);
			turret->SetRotation(objData.rotation);
			return true;
		}
		//名前がエネルギーボール生成機の時。
		if (objData.EqualObjectName(L"EnergyBallGenerator") == true) {
			EnergyBallGenerator* energyBallGenerator = NewGO<EnergyBallGenerator>(0, "energyBallGenerator");
			energyBallGenerator->SetPosition(objData.position);
			energyBallGenerator->SetRotation(objData.rotation);
			return true;
		}
		//名前がエネルギーボール受け取り機の時。
		if (objData.ForwardMatchName(L"EnergyBallReceiver") == true) {
			EnergyBallReceiver* energyBallReceiver = NewGO<EnergyBallReceiver>(0, "energyBallReceiver");
			energyBallReceiver->SetPosition(objData.position);
			energyBallReceiver->SetRotation(objData.rotation);
			energyBallReceiver->SetGimmickNumber(objData.number);
			return true;
		}
		//名前が扉の時。
		if (objData.ForwardMatchName(L"Door") == true) {
			Door* door = NewGO<Door>(0, "door");
			door->SetPosition(objData.position);
			door->SetRotation(objData.rotation);
			door->SetGimmickNumber(objData.number);
			m_gimmickObjects.emplace_back(door);
			return true;
		}
		return true;
		}
	);
}

void Stage::Update()
{

}

void Stage::ClearGimmickObject(const int num)
{
	for (int i = 0; i < m_gimmickObjects.size(); ++i) {
		//ギミックを解除する。
		if (m_gimmickObjects[i]->GetGimmickNumber() == num) {
			m_gimmickObjects[i]->Clear();
		}
	}
}

void Stage::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}