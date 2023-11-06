#include "stdafx.h"
#include "Turret.h"
#include "Player.h"

namespace
{
	const Vector3	TURRET_HEIGHT = Vector3(0.0f, 50.0f, 0.0f);				//�^���b�g�̍����B
	const int		DAMAGE_AMOUNT = 10;										//�_���[�W�ʁB
	const int		SHOOTING_RANGE = 10;									//���ˎ��̂΂���B
	const float		SEARCH_LENGTH = pow(600.0f, 2.0f);						//���G���鋗���B
	const float		TURRET_COLLISION_FORWARD = 10.0f;						//�R���W�����̑O�����B
	const float		SEARCH_COOLDOWN = 1.0f;									//�������̃N�[���_�E���B
	const float		DAMAGE_COOLDOWN = 0.1f;									//�ˌ����̍d�����ԁB
	const float		MISSING_COOLDOWN = 3.0f;								//�������ۂ̌o�ߎ��ԁB
	const float		TURRET_POV = (30.0f / 100.0f) * Math::PI;					//�^���b�g�̎���p�B
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
	m_modelRender.SetRotation(m_rotation);
	m_modelRender.Update();

	//�R���W�����p�̃��f�����������B
	ModelInitData modelInitData;
	modelInitData.m_tkmFilePath = "Assets/modelData/object/turret/turret_collision.tkm";
	Model turretModel;
	turretModel.Init(modelInitData);

	//�v���C���[�̌����B
	m_player = FindGO<Player>("player");

	return true;
}

void Turret::Update()
{
	SearchPlayer();

	State();

	m_modelRender.SetRotation(m_rotation);
	m_modelRender.Update();

}

/// <summary>
/// �^���b�g�̍��G�����B
/// </summary>
void Turret::SearchPlayer()
{
	//�v���C���[����^���b�g�ɐL�т�x�N�g�������߂�B
	Vector3 diff = m_player->GetPosition() - m_position;

	//�v���C���[�Ƃ̋���������Ă�����B
	if (diff.LengthSq() >= SEARCH_LENGTH) {
		m_isHit = false;
		return;
	}

	diff.Normalize();
	//�p�x���v�Z�B
	float angle = acosf(diff.Dot(m_forward));

	//�v���C���[�����E���ɂ��Ȃ�������B
	if (TURRET_POV <= fabsf(angle)) {
		m_isHit = false;
		return;
	}

	//�ǂƏՓ˂��Ă���Ȃ�B
	if (WallAndHit()) {
		m_isHit = false;
		return;
	}

	//�������Ă��锻��B
	m_isHit = true;
}

/// <summary>
/// �ǂƂ̏Փˏ����B
/// </summary>
/// <returns></returns>
bool Turret::WallAndHit()
{
	Vector3 playerPos = m_player->GetPosition();
	playerPos.y += 75.0f;
	Vector3 turretPos = m_position;
	turretPos.y += 75.0f;

	PhysicsWorld::RayHitObject hit;

	//���C���΂��B
	bool isHit = PhysicsWorld::GetInstance()->RayTest(playerPos, turretPos, hit);

	//�����������Ă��Ȃ��B
	if (!isHit) {
		return false;
	}

	//�v���C���[�ƏՓ˂��Ă���Ȃ�B
	if (hit.collision == m_player->GetCollision()) {
		return false;
	}

	//�ǂƏՓ˂��Ă���B
	return true;
}

/// <summary>
/// �X�e�[�g�����B
/// </summary>
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
		Vector3 knockback = endPos - startPos;
		knockback.Normalize();
		//�_���[�W��^����B
		m_player->ReceiveDamage(DAMAGE_AMOUNT, knockback);

		//�v���C���[�����S������B
		if (m_player->GetPlayerState() == Player::enState_Dead) {
			m_state = enTurretState_Idle;
			m_progressTimer = 0.0f;
		}
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
	m_oldPlayerPos = g_camera3D->GetPosition();

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