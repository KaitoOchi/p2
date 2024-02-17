#include "stdafx.h"
#include "Door.h"

Door::Door()
{

}

Door::~Door()
{

}

bool Door::Start()
{
	m_gimmickType = enGimmickType_Defuse;

	//アニメーションクリップ。
	m_animationClips[enState_Idle].Load("Assets/animData/door/idle.tka");
	m_animationClips[enState_Idle].SetLoopFlag(false);
	m_animationClips[enState_Open].Load("Assets/animData/door/open.tka");
	m_animationClips[enState_Open].SetLoopFlag(true);

	//モデルを設定。
	m_modelRender.Init("Assets/modelData/object/door/door.tkm");
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(m_rotation);
	m_modelRender.Update();
	//物理静的オブジェクトを設定。
	m_collisionModelRender.Init("Assets/modelData/object/door/door_collision.tkm");
	m_collisionModelRender.SetPosition(m_position);
	m_collisionModelRender.SetRotation(m_rotation);
	m_collisionModelRender.Update();
	m_physicsStaticObject.CreateFromModel(m_collisionModelRender.GetModel(), m_collisionModelRender.GetModel().GetWorldMatrix());

	return true;
}

void Door::Update()
{
}

void Door::Clear()
{
	//m_modelRender.PlayAnimation(enState_Open, 0.0f);

	m_physicsStaticObject.Release();

	DeleteGO(this);
}

void Door::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}