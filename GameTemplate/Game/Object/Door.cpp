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
	m_physicsStaticObject.Release();
}

void Door::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}