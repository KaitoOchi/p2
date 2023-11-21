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

	m_gimmickObjects.clear();
}

bool Stage::Start()
{
	//�n�ʃ��f���̐ݒ�B
	m_modelRender.Init("Assets/modelData/stage/tile.tkm", 0, 0, enModelUpAxisZ, true, true);
	//���f������ÓI�I�u�W�F�N�g���쐬�B
	m_physicsStaticObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());

	InitLevelRender();

	return true;
}

void Stage::InitLevelRender()
{
	// ���x���f�U�C�������B
	m_levelRender.Init("Assets/level/stage/level00.tkl", [&](LevelObjectData& objData) {
		//���O���v���C���[�̎��B
		if (objData.EqualObjectName(L"Player") == true) {
			//�������W��ݒ�B
			m_respawnPoint = objData.position;
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
		//�M�~�b�N����������B
		if (m_gimmickObjects[i]->GetGimmickNumber() == num) {
			m_gimmickObjects[i]->Clear();
		}
	}
}

void Stage::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}