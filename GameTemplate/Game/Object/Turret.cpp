#include "stdafx.h"
#include "Turret.h"
#include "Player.h"

namespace
{
	const Vector3	TURRET_HEIGHT = Vector3(0.0f, 50.0f, 0.0f);		//�^���b�g�̍����B
	const int		DAMAGE_AMOUNT = 10;								//�_���[�W�ʁB
	const float		RAY_LENGTH = 500.0f;							//���C�̒����B
	const float		DAMAGE_COOLDOWN = 0.5f;							//�_���[�W�̍d�����ԁB
}

Turret::Turret()
{
	m_forward.z = 1.0f;
}

Turret::~Turret()
{

}

bool Turret::Start()
{
	//���f���̏������B
	m_modelRender.Init("Assets/modelData/unityChan.tkm", 0, 0, enModelUpAxisZ, true, true);
	m_modelRender.SetPosition(m_position);
	m_modelRender.Update();

	//�v���C���[�̌����B
	m_player = FindGO<Player>("player");

	return true;
}

void Turret::Update()
{
	//�_���[�W�̍d�����Ԃ�����Ȃ�B
	if (m_damageTimer > 0.0f) {
		m_damageTimer -= g_gameTime->GetFrameDeltaTime();
		return;
	}

	PhysicsWorld::RayHitObject hit;
	Vector3 endPos = m_position + (m_forward * RAY_LENGTH) + TURRET_HEIGHT;
	bool isHit = PhysicsWorld::GetInstance()->RayTest(m_position, endPos, hit);

	//�Փ˂����I�u�W�F�N�g���v���C���[�Ȃ�B
	if (hit.collision == m_player->GetCollision()) {
		//�̂���������B
		Vector3 knockback = m_forward;
		//�_���[�W��^����B
		m_player->ReceiveDamage(DAMAGE_AMOUNT, knockback);
		//�_���[�W�ɃN�[���_�E����ݒ�B
		m_damageTimer = DAMAGE_COOLDOWN;
	}
}

void Turret::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}