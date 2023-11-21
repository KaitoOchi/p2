#pragma once
#include "Gimmick.h"

class EnergyBallReceiver : public Gimmick
{
public:
	EnergyBallReceiver();
	~EnergyBallReceiver() override;
	bool Start();
	void Update();
	void Render(RenderContext& rc);

public:
	/// <summary>
	/// �M�~�b�N�̐��������B
	/// </summary>
	void Clear() override;

private:
	Stage*				m_stage = nullptr;				//�X�e�[�W�B
	CollisionObject*	m_collisionObject = nullptr;	//�R���W�����B
};

