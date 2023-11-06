#include "stdafx.h"
#include "Stage.h"

#include "Turret.h"

Stage::Stage()
{

}

Stage::~Stage()
{

}

bool Stage::Start()
{
	//�n�ʃ��f���̐ݒ�B
	m_modelRender.Init("Assets/modelData/stage/tile.tkm", 0, 0, enModelUpAxisZ, true, true);
	//���f������ÓI�I�u�W�F�N�g���쐬�B
	m_physicsStaticObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());
	m_physicsStaticObject.GetbtCollisionObject()->setUserIndex(enCollisionAttr_Wall);

	//�^���b�g�̐����B
	Turret* turret = NewGO<Turret>(0, "turret");
	turret->SetPosition(Vector3(-450.0f, 0.0f, -400.0f));
	Quaternion rot;
	rot.AddRotationDegY(180.0f);
	turret->SetRotation(rot);

	return true;
}

void Stage::Update()
{

}

void Stage::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}