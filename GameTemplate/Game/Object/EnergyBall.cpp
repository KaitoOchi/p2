#include "stdafx.h"
#include "EnergyBall.h"
#include "EnergyBallGenerator.h"
#include "Player.h"
#include "Stage.h"
#include "PortalFrame.h"

namespace
{
	const int	DAMAGE_AMOUNT = 999;		//�_���[�W�ʁB
	const float COLLISION_RADIUS = 20.0f;	//�R���W�����̔��a�B
	const float MOVE_SPEED = 200.0f;		//�ړ����x�B
	const float MOVE_TIMER = 15.0f;			//�\�����ԁB
}


EnergyBall::EnergyBall()
{

}

EnergyBall::~EnergyBall()
{

}

bool EnergyBall::Start()
{
	m_gimmickType = enGimmickType_Sender;

	m_player = FindGO<Player>("player");
	m_stage = FindGO<Stage>("stage");
	
	//�R���W�����̐ݒ�B
	m_collisionObject = NewGO<CollisionObject>(0);
	m_collisionObject->CreateSphere(m_position, m_rotation, COLLISION_RADIUS);
	m_collisionObject->SetName("energyBall");

	//�z��Ɏ��g��ǉ��B
	m_stage->AddEnergyBallObject(this);

	//�����@���Ȃ��Ȃ�
	if (m_energyBallGenerator == nullptr) {
		//�������Ԃ𖳌��ɂ���B
		m_timer = FLT_MIN;
	}

	return true;
}

void EnergyBall::Update()
{
	if (!m_isEnable) {
		return;
	}

	m_timer += g_gameTime->GetFrameDeltaTime();

	//���Ԃ��o�߂�����B
	if (m_timer > MOVE_TIMER) {
		Disable();
	}

	Move();

	Collision();
}

/// <summary>
/// �ړ������B
/// </summary>
void EnergyBall::Move()
{
	m_moveSpeed = Vector3::Zero;

	//�ړ�����v�Z����B
	m_moveSpeed = m_direction * MOVE_SPEED * g_gameTime->GetFrameDeltaTime();

	RayCast();

	m_ballEffect->SetPosition(m_position);
	m_ballEffect->Update();

	m_collisionObject->SetPosition(m_position);
	m_collisionObject->Update();
}

/// <summary>
/// ���C���΂������B
/// </summary>
void EnergyBall::RayCast()
{
	//todo dbeug
	Vector3 nextPosition = m_position + m_moveSpeed;

	PhysicsWorld::RayHitObject hit;   

	//���C���΂��B
	const bool isHit = PhysicsWorld::GetInstance()->RayCastHit(m_position, nextPosition, &m_collisionObject->GetbtCollisionObject(), hit);

	//�����������Ă��Ȃ��B
	if (!isHit) {
		m_position = nextPosition;
		return;
	}

	m_direction = hit.normal;
}

/// <summary>
/// �����蔻��̏����B
/// </summary>
void EnergyBall::Collision()
{
	//�v���C���[�ƐڐG������B
	if (m_collisionObject->IsHit(const_cast<CharacterController&>(*m_player->GetCharacterController()))) {
		//�_���[�W��^����B
		m_player->ReceiveDamage(DAMAGE_AMOUNT, Vector3::Zero);
	}

	////�v���C���[�ƐڐG������B
	//if (m_collisionObject->IsHit(m_stage->GetPhysicsStaticObject())) {
	//	//�_���[�W��^����B
	//	m_player->ReceiveDamage(DAMAGE_AMOUNT, Vector3::Zero);
	//}
}

/// <summary>
/// �L�����B
/// </summary>
void EnergyBall::Enable()
{
	m_isEnable = true;

	//�G�t�F�N�g���Đ��B
	m_ballEffect = NewGO<EffectEmitter>(0);
	m_ballEffect->Init(EffectEmitter::enEffect_EnergyBall);
	m_ballEffect->Play();

	//�����蔻���L�����B
	m_collisionObject->SetIsEnable(true);
}

/// <summary>
/// �������B
/// </summary>
void EnergyBall::Disable()
{
	m_isEnable = false;

	m_timer = 0.0f;

	//�G�t�F�N�g���~�B
	m_ballEffect->Stop();
	DeleteGO(m_ballEffect);

	//�����蔻��𖳌����B
	m_collisionObject->SetIsEnable(false);

	if (m_energyBallGenerator != nullptr) {
		//�����@�ɔ�\����ʒm�B
		m_energyBallGenerator->NotifyDisable();
	}
}

/// <summary>
/// ���[�v�����B
/// </summary>
/// <param name="pos">���W</param>
/// <param name="direction">����</param>
void EnergyBall::SetWarp(const Vector3& pos, const Vector3& direction)
{
	m_position = pos;
	m_rotation.SetRotation(m_direction, direction);
	m_direction = direction;
}

void EnergyBall::Clear()
{
	Disable();

	//�z�񂩂玩�g���폜�B
	m_stage->RemoveEnergyBallObject(this);

	if (m_energyBallGenerator != nullptr) {
		m_energyBallGenerator->Disable();
	}
}