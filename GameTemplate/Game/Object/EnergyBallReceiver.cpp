#include "stdafx.h"
#include "EnergyBallReceiver.h"
#include "Stage.h"
#include "EnergyBall.h"

namespace
{
	const Vector3 COLLISION_SIZE = Vector3(100.0f, 100.0f, 100.0f);
}

EnergyBallReceiver::EnergyBallReceiver()
{

}

EnergyBallReceiver::~EnergyBallReceiver()
{

}

bool EnergyBallReceiver::Start()
{
	m_gimmickType = enGimmickType_Receiver;

	m_stage = FindGO<Stage>("stage");

	//���f���̐ݒ�B
	m_modelRender.Init("Assets/modelData/object/energyBall/energyBallReceiver.tkm");
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(m_rotation);
	m_modelRender.Update();

	//�R���W�����̐ݒ�B
	m_collisionObject = NewGO<CollisionObject>(0);
	m_collisionObject->CreateBox(m_position, m_rotation, COLLISION_SIZE);
	m_collisionObject->SetName("energyBallReceiver");

	return true;
}

void EnergyBallReceiver::Update()
{
	if (m_isClear) {
		return;
	}

	for (auto& col : m_stage->GetEnergyBallObjects())
	{
		//�G�l���M�[�{�[���ƐڐG������B
		if (m_collisionObject->IsHit(col->GetCollisionObject())) {
			//�N���A����B
			m_stage->RemoveEnergyBallObject(col);
			col->Clear();
			Clear();
		}
	}
}

void EnergyBallReceiver::Clear()
{
	m_stage->ClearGimmickObject(m_gimmickNum);
	m_isClear = true;
}

void EnergyBallReceiver::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}