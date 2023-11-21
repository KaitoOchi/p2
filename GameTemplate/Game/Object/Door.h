#pragma once
#include "Gimmick.h"

class Door : public Gimmick
{
public:
	Door();
	~Door() override;
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	void Clear() override;

private:
	ModelRender				m_modelRender;				//���f���B
	ModelRender				m_collisionModelRender;		//�����蔻��p���f���B
	PhysicsStaticObject		m_physicsStaticObject;		//�����ÓI�I�u�W�F�N�g�B
};

