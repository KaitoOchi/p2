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
	/// ドアのアニメーション。
	/// </summary>
	enum AnimationState
	{
		enState_Idle,
		enState_Open,
		enState_Num,
	};

private:
	ModelRender				m_modelRender;				//モデル。
	ModelRender				m_collisionModelRender;		//当たり判定用モデル。
	PhysicsStaticObject		m_physicsStaticObject;		//物理静的オブジェクト。
	AnimationClip			m_animationClips[enState_Num];	//アニメーションクリップ。
};

