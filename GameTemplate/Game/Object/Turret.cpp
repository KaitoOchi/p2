#include "stdafx.h"
#include "Turret.h"
#include "Player.h"

namespace
{
	const Vector3	TURRET_HEIGHT = Vector3(0.0f, 25.0f, 0.0f);				//�^���b�g�̍����B
	const Vector3	TURRET_COLLISION_SIZE = Vector3(200.0f, 20.0f, 200.0f);	//���G�͈́B
	const int		DAMAGE_AMOUNT = 10;										//�_���[�W�ʁB
	const int		SHOOTING_RANGE = 5;										//���ˎ��̂΂���B
	const float		TURRET_COLLISION_FORWARD = 200.0f;						//�R���W�����̑O�����B
	const float		SEARCH_COOLDOWN = 1.0f;									//�������̃N�[���_�E���B
	const float		DAMAGE_COOLDOWN = 0.1f;									//�ˌ����̍d�����ԁB
	const float		MISSING_COOLDOWN = 3.0f;								//�������ۂ̌o�ߎ��ԁB
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
	m_modelRender.Init("Assets/modelData/unityChan.tkm", 0, 0, enModelUpAxisZ, true, true, ModelRender::enModelInitMode_Dithering);
	m_modelRender.SetPosition(m_position);
	m_modelRender.Update();

	//�����蔻����������B
	m_collisionObject = NewGO<CollisionObject>(0, "collision");
	m_collisionObject->CreateBox(m_position, m_rotation, TURRET_COLLISION_SIZE);
	m_collisionObject->SetName("portal_red");
	m_collisionObject->SetPosition(m_position + (m_forward * TURRET_COLLISION_FORWARD) + TURRET_HEIGHT);

	//�v���C���[�̌����B
	m_player = FindGO<Player>("player");

	return true;
}

void Turret::Update()
{
	Collision();

	State();

	wchar_t text[256];
	swprintf_s(text, 256,
		L"State:%d \nTime:%0.2f",
		m_state,
		m_progressTimer
	);
	a.SetPosition(Vector3(-300.0f, -200.0f, 0.0f));
	a.SetText(text);

}

void Turret::Collision()
{
	//�v���C���[�ƐڐG���Ă���Ȃ�B
	if (m_collisionObject->IsHit(const_cast<CharacterController&>(m_player->GetCharacterController()))) {
		m_isHit = true;
	}
	else {
		m_isHit = false;
	}
}

void Turret::State()
{
	switch (m_state)
	{
	case enTurretState_Idle:
		ProcessIdleStateTransition();
		break;

	case enTurretState_Search:
		ProcessSearchStateTransition();
		break;

	case enTurretState_Shot:
		ProcessShotStateTransition();
		break;

	case enTurretState_Missing:
		ProcessMissingStateTransition();
		break;

	case enTurretState_Grab:
		break;

	case enTurretState_Dead:
		break;
	}
}

void Turret::ProcessIdleStateTransition()
{
	//�v���C���[�Ɠ����蔻�肪�Փ˂��Ă���Ȃ�B
	if (m_isHit) {
		//�����X�e�[�g�֑J�ڂ���B
		m_state = enTurretState_Search;
		//�N�[���_�E����ݒ�B
		m_progressTimer = SEARCH_COOLDOWN;
	}
}

void Turret::ProcessSearchStateTransition()
{
	//�������Ă��玞�Ԃ��o�߂�����B
	if (m_progressTimer < 0.0f) {
		//�ˌ��X�e�[�g�֑J�ڂ���B
		m_state = enTurretState_Shot;
		//�N�[���_�E����ݒ�B
		m_progressTimer = DAMAGE_COOLDOWN;
		//�v���C���[�̍��W��ۑ��B
		m_oldPlayerPos = m_player->GetPosition();
		return;
	}

	m_progressTimer -= g_gameTime->GetFrameDeltaTime();

	//�v���C���[�Ɠ����蔻�肪�Փ˂��Ă��Ȃ��Ȃ�B
	if (!m_isHit) {
		//�������X�e�[�g�֑J�ڂ���B
		m_state = enTurretState_Missing;
		//�N�[���_�E����ݒ�B
		m_progressTimer = MISSING_COOLDOWN;
	}
}

void Turret::ProcessShotStateTransition()
{
	//�d�����Ԃ�����Ȃ�B
	if (m_progressTimer > 0.0f) {
		m_progressTimer -= g_gameTime->GetFrameDeltaTime();
		return;
	}

	//�΂����^���邽�ߍ��W���������炷�B
	float random = Math::Random(-SHOOTING_RANGE, SHOOTING_RANGE);
	m_oldPlayerPos.x += random;
	m_oldPlayerPos.z += random;

	PhysicsWorld::RayHitObject hit;
	//�n�_��ݒ�B
	Vector3 startPos = m_position + TURRET_HEIGHT;
	//�I�_��ݒ�B
	Vector3 endPos = m_oldPlayerPos + ((m_oldPlayerPos - startPos) * TURRET_COLLISION_FORWARD);
	//���C���΂��B
	bool isHit = PhysicsWorld::GetInstance()->RayTest(startPos, endPos, hit);

	//�Փ˂����I�u�W�F�N�g���v���C���[�Ȃ�B
	if (hit.collision == m_player->GetCollision()) {
		//�̂���������B
		Vector3 knockback = m_forward;
		//�_���[�W��^����B
		m_player->ReceiveDamage(DAMAGE_AMOUNT, knockback);
	}

	//�ˌ��G�t�F�N�g�̉�]�ʂ�ݒ�B
	float rotX = -(m_position.y - m_position.y) + random;
	Quaternion rot;
	rot.SetRotationYFromDirectionXZ(endPos);
	rot.AddRotationDegX(rotX);

	//�ˌ��G�t�F�N�g���Đ��B
	EffectEmitter* shotEffect = NewGO<EffectEmitter>(0);
	shotEffect->Init(EffectEmitter::enEffect_Turret_Shot);
	shotEffect->SetPosition(startPos);
	shotEffect->SetRotation(rot);
	shotEffect->Play();

	//�N�[���_�E����ݒ�B
	m_progressTimer = DAMAGE_COOLDOWN;
	//�v���C���[�̍��W��ۑ��B
	m_oldPlayerPos = m_player->GetPosition();

	//�v���C���[�Ɠ����蔻�肪�Փ˂��Ă��Ȃ��Ȃ�B
	if (!m_isHit) {
		//�������X�e�[�g�֑J�ڂ���B
		m_state = enTurretState_Missing;
		m_progressTimer = MISSING_COOLDOWN;
	}
}

void Turret::ProcessMissingStateTransition()
{
	//�������Ă��玞�Ԃ��o�߂�����B
	if (m_progressTimer < 0.0f) {
		//�ҋ@�X�e�[�g�֑J�ڂ���B
		m_state = enTurretState_Idle;
		return;
	}

	m_progressTimer -= g_gameTime->GetFrameDeltaTime();

	//�v���C���[�Ɠ����蔻�肪�Փ˂��Ă���Ȃ�B
	if (m_isHit) {
		//�ˌ��X�e�[�g�֑J�ڂ���B
		m_state = enTurretState_Shot;
		//�N�[���_�E����ݒ�B
		m_progressTimer = DAMAGE_COOLDOWN;
		//�v���C���[�̍��W��ۑ��B
		m_oldPlayerPos = m_player->GetPosition();
	}
}

void Turret::ProcessGrabStateTransition()
{

}

void Turret::ProcessDeadStateTransition()
{

}

void Turret::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);

	a.Draw(rc);
}