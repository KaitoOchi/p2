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
	//�^���b�g���폜�B
	QueryGOs<Turret>("turret", [&](Turret* turret) {
		DeleteGO(turret);
		return true;
		});

	//�G�l���M�[�{�[�����폜�B
	QueryGOs<EnergyBall>("energyBall", [&](EnergyBall* energyBall) {
		DeleteGO(energyBall);
		return true;
		});

	//�G�l���M�[�{�[�������@���폜�B
	QueryGOs<EnergyBallGenerator>("energyBallGenerator", [&](EnergyBallGenerator* energyBallGenerator) {
		DeleteGO(energyBallGenerator);
		return true;
		});

	//�G�l���M�[�{�[���󂯎��@���폜�B
	QueryGOs<EnergyBallReceiver>("energyBallReceiver", [&](EnergyBallReceiver* energyBallReceiver) {
		DeleteGO(energyBallReceiver);
		return true;
		});

	//�h�A���폜�B
	QueryGOs<Door>("door", [&](Door* door) {
		DeleteGO(door);
		return true;
		});

	//�W�����v���폜�B
	QueryGOs<JumpBoard>("jumpBoard", [&](JumpBoard* jumpBoard) {
		DeleteGO(jumpBoard);
		return true;
		});

	m_gimmickObjects.clear();
}

bool Stage::Start()
{
	m_portalCamera = FindGO<PortalCamera>("portalCamera");

	//���f���̍쐬�B
	m_decorationModelRender.Init("Assets/modelData/stage/level00/level00_decoration.tkm", 0, 0, enModelUpAxisZ, false, true);
	m_physicsModelRender.Init("Assets/modelData/stage/level00/level00_collision.tkm", 0, 0, enModelUpAxisZ, false, true);
	//���f������ÓI�I�u�W�F�N�g���쐬�B
	m_physicsStaticObject.CreateFromModel(m_physicsModelRender.GetModel(), m_physicsModelRender.GetModel().GetWorldMatrix());

	InitLevelRender();

	return true;
}

void Stage::InitLevelRender()
{
	//�|�C���g���C�g�ƃX�|�b�g���C�g�̐��B
	int ptNum = 0;
	int spNum = 0;	

	std::map<int, JumpBoard*> jumpBoardObjects;
	std::map<int, Vector3> landingPosObjects;

	// ���x���f�U�C�������B
	m_levelRender.Init("Assets/level/stage/level00.tkl", [&](LevelObjectData& objData) {
		//���O���v���C���[�̎��B
		if (objData.EqualObjectName(L"Player") == true) {
			//�������W��ݒ�B
			m_respawnPosition = objData.position;
			Player* player = FindGO<Player>("player");
			player->Reset();
			return true;
		}
		//���O���N���A�n�_�̎��B
		if (objData.EqualObjectName(L"ClearPosition") == true) {
			//�N���A���W��ݒ�B
			m_clearPosition = objData.position;
			return true;
		}
		//���O���^���b�g�̎��B
		if (objData.EqualObjectName(L"Turret") == true) {
			Turret* turret = NewGO<Turret>(0, "turret");
			turret->SetPosition(objData.position);
			turret->SetRotation(objData.rotation);
			return true;
		}
		//���O���G�l���M�[�{�[�������@�̎��B
		if (objData.EqualObjectName(L"EnergyBallGenerator") == true) {
			EnergyBallGenerator* energyBallGenerator = NewGO<EnergyBallGenerator>(0, "energyBallGenerator");
			energyBallGenerator->SetPosition(objData.position);
			energyBallGenerator->SetRotation(objData.rotation);
			return true;
		}
		//���O���G�l���M�[�{�[���󂯎��@�̎��B
		if (objData.ForwardMatchName(L"EnergyBallReceiver") == true) {
			EnergyBallReceiver* energyBallReceiver = NewGO<EnergyBallReceiver>(0, "energyBallReceiver");
			energyBallReceiver->SetPosition(objData.position);
			energyBallReceiver->SetRotation(objData.rotation);
			energyBallReceiver->SetGimmickNumber(objData.number);
			return true;
		}
		//���O�����̎��B
		if (objData.ForwardMatchName(L"Door") == true) {
			Door* door = NewGO<Door>(0, "door");
			door->SetPosition(objData.position);
			door->SetRotation(objData.rotation);
			door->SetGimmickNumber(objData.number);
			m_gimmickObjects.emplace_back(door);
			return true;
		}
		//���O���W�����v��̎��B
		if (objData.ForwardMatchName(L"JumpBoard") == true) {
			JumpBoard* jumpBoard = NewGO<JumpBoard>(0, "jumpBoard");
			jumpBoard->SetPosition(objData.position);
			jumpBoard->SetRotation(objData.rotation);
			jumpBoardObjects.emplace(objData.number, jumpBoard);
			jumpBoard->SetLandingPosition(Vector3(0.0f, 0.0f, 1250.0f));
			return true;
		}
		//���O�����n�n�_�̎��B
		if (objData.ForwardMatchName(L"LandingPos") == true) {
			landingPosObjects.emplace(objData.number, objData.position);
			return true;
		}
		//���O�����|�C���g���C�g�Ȃ�B
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
		//���O���|�C���g���C�g�Ȃ�B
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
		//���O���X�|�b�g���C�g�Ȃ�B
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

	//�W�����v��ƒ��n�n�_�̓����B
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
		//�M�~�b�N����������B
		if (m_gimmickObjects[i]->GetGimmickNumber() == num) {
			m_gimmickObjects[i]->Clear();
		}
	}
}

void Stage::Render(RenderContext& rc)
{
	m_decorationModelRender.Draw(rc);
}