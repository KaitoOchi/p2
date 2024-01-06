#include "stdafx.h"
#include "Stage.h"

#include "Player.h"
#include "Gimmick.h"
#include "Turret.h"
#include "EnergyBall.h"
#include "EnergyBallGenerator.h"
#include "EnergyBallReceiver.h"
#include "Door.h"
#include "JumpBoard.h"
#include "PortalCamera.h"

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

	//ジャンプ板を削除。
	QueryGOs<JumpBoard>("jumpBoard", [&](JumpBoard* jumpBoard) {
		DeleteGO(jumpBoard);
		return true;
		});

	m_gimmickObjects.clear();
}

bool Stage::Start()
{
	m_portalCamera = FindGO<PortalCamera>("portalCamera");

	//モデルの作成。
	m_decorationModelRender.Init("Assets/modelData/stage/level00/level00_decoration.tkm", 0, 0, enModelUpAxisZ, false, true);
	m_physicsModelRender.Init("Assets/modelData/stage/level00/level00_collision.tkm", 0, 0, enModelUpAxisZ, false, true);
	//モデルから静的オブジェクトを作成。
	m_physicsStaticObject.CreateFromModel(m_physicsModelRender.GetModel(), m_physicsModelRender.GetModel().GetWorldMatrix());

	InitLevelRender();

	return true;
}

void Stage::InitLevelRender()
{
	//ポイントライトとスポットライトの数。
	int ptNum = 0;
	int spNum = 0;	

	std::map<int, JumpBoard*> jumpBoardObjects;
	std::map<int, Vector3> landingPosObjects;

	// レベルデザイン処理。
	m_levelRender.Init("Assets/level/stage/level00.tkl", [&](LevelObjectData& objData) {
		//名前がプレイヤーの時。
		if (objData.EqualObjectName(L"Player") == true) {
			//初期座標を設定。
			m_respawnPosition = objData.position;
			Player* player = FindGO<Player>("player");
			player->Reset();
			return true;
		}
		//名前がクリア地点の時。
		if (objData.EqualObjectName(L"ClearPosition") == true) {
			//クリア座標を設定。
			m_clearPosition = objData.position;
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
		//名前がジャンプ台の時。
		if (objData.ForwardMatchName(L"JumpBoard") == true) {
			JumpBoard* jumpBoard = NewGO<JumpBoard>(0, "jumpBoard");
			jumpBoard->SetPosition(objData.position);
			jumpBoard->SetRotation(objData.rotation);
			jumpBoardObjects.emplace(objData.number, jumpBoard);
			jumpBoard->SetLandingPosition(Vector3(0.0f, 0.0f, 1250.0f));
			return true;
		}
		//名前が着地地点の時。
		if (objData.ForwardMatchName(L"LandingPos") == true) {
			landingPosObjects.emplace(objData.number, objData.position);
			return true;
		}
		//名前が黒ポイントライトなら。
		if (objData.ForwardMatchName(L"pointLight_black") == true) {
			PointLight* ptLight = new PointLight;
			ptLight->SetPortalCameraPointer(m_portalCamera);
			ptLight->SetPointLight(
				ptNum,
				objData.position,
				Vector3(-10.0f, -10.0f, -10.0f),
				objData.number
			);
			m_pointLightObjects.emplace_back(ptLight);
			ptNum++;
			return true;
		}
		//名前がポイントライトなら。
		if (objData.ForwardMatchName(L"pointLight") == true) {
			PointLight* ptLight = new PointLight;
			ptLight->SetPortalCameraPointer(m_portalCamera);
			ptLight->SetPointLight(
				ptNum,
				objData.position,
				objData.scale,
				objData.number
			);
			m_pointLightObjects.emplace_back(ptLight);
			ptNum++;
			return true;
		}
		//名前がスポットライトなら。
		if (objData.ForwardMatchName(L"spotLight") == true) {
			Vector3 dir = Vector3::AxisX;
			objData.rotation.Apply(dir);
			dir.Normalize();
			SpotLight* spLight = new SpotLight;
			spLight->SetPortalCameraPointer(m_portalCamera);
			spLight->SetSpotLight(
				spNum,
				objData.position,
				objData.scale,
				objData.number,
				dir,
				120.0f
			);
			m_spotLightObjects.emplace_back(spLight);
			spNum++;
			return true;
		}
		return true;
		}
	);
	RenderingEngine::GetInstance()->GetLightCB().ptNum = ptNum;
	RenderingEngine::GetInstance()->GetLightCB().spNum = spNum;

	//ジャンプ台と着地地点の同期。
	auto itr = landingPosObjects.begin();
	for (auto& obj : jumpBoardObjects)
	{
		obj.second->SetLandingPosition(itr->second);
		itr = std::next(itr, 1);
	}
	jumpBoardObjects.clear();
	landingPosObjects.clear();
}

void Stage::Update()
{
	for (auto& light : m_pointLightObjects)
	{
		light->Update();
	}

	for (auto& light : m_spotLightObjects)
	{
		light->Update();
	}
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
	m_decorationModelRender.Draw(rc);
}