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
	/// <summary>
	/// �h�A�̃A�j���[�V�����B
	/// </summary>
	enum AnimationState
	{
		enState_Idle,
		enState_Open,
		enState_Num,
	};

private:
	ModelRender				m_modelRender;				//���f���B
	ModelRender				m_collisionModelRender;		//�����蔻��p���f���B
	PhysicsStaticObject		m_physicsStaticObject;		//�����ÓI�I�u�W�F�N�g�B
	AnimationClip			m_animationClips[enState_Num];	//�A�j���[�V�����N���b�v�B
};

