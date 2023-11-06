#include "stdafx.h"
#include "Player.h"
#include "PortalGun.h"
#include "PortalFrame.h"
#include "GameCamera.h"
#include "Game.h"

namespace
{
	const int		MAX_HP = 100;									//�ő�HP�B
	const float		CHARACON_RADIUS = 10.0f;						//�L�����R���̔��a�B
	const float		CHARACON_HEIGHT = 60.0f;						//�L�����R���̍����B
	const float		WALK_SPEED = 120.0f;							//�ړ����x�B
	const float		CROUCH_SPEED = 40.0f;							//���Ⴊ�ݑ��x�B
	const float		JUMP_POWER = 140.0f;							//�W�����v�ʁB
	const float		GRAVITY = 10.0f;								//�d�́B
	const float		GRAVITY_ACCEL = 0.75f;							//�d�͉����B
	const float		INTERPOLATE_TIME = 0.2f;						//���`��ԁB
	const float		KNOCKBACK_POWER = 300.0f;						//�_���[�W���󂯂��Ƃ��̃m�b�N�o�b�N�p���[�B
	const float		ADDMOVESPEED_MIN = 0.1f;						//�ǉ��̈ړ����x���I������l�B
	const float		ADDMOVESPEED_DIV = 5.0f;						//�ǉ��̈ړ����x�����Z����l�B
	const float		DAMAGE_TIME = 0.3f;								//�_���[�W���󂯂鎞�ԁB
	const float		DEAD_TIME = 2.0f;								//���S���ԁB
}

Player::Player()
{
}

Player::~Player()
{
	DeleteGO(m_portalGun);
	DeleteGO(m_gameCamera);
}

bool Player::Start()
{
	//�Q�[���N���X�̌����B
	m_game = FindGO<Game>("game");

	//�A�j���[�V�����N���b�v�B
	m_animationClips[enState_Idle].Load("Assets/animData/UnityChan/idle.tka");
	m_animationClips[enState_Idle].SetLoopFlag(true);
	m_animationClips[enState_Walk].Load("Assets/animData/UnityChan/walk.tka");
	m_animationClips[enState_Walk].SetLoopFlag(true);
	m_animationClips[enState_Jump].Load("Assets/animData/UnityChan/jump.tka");
	m_animationClips[enState_Jump].SetLoopFlag(false);

	//���f���̐ݒ�B
	m_modelRender.Init(
		"Assets/modelData/unityChan.tkm",
		m_animationClips,
		enState_Num,
		enModelUpAxisY,
		true,
		true,
		ModelRender::enModelInitMode_UnDrawMainCamera
	);
	m_modelRender.Update();

	//�_���[�W�摜�̐ݒ�B
	m_damageSpriteRender.Init("Assets/sprite/UI/damage.DDS", 1600.0f, 900.0f);
	m_damageSpriteRender.SetMulColor(Vector4(1.0f, 1.0f, 1.0f, m_damageTimer));
	m_damageSpriteRender.Update();

	//�L�����N�^�[�R���g���[���[�̐ݒ�B
	m_characterController.Init(CHARACON_RADIUS, CHARACON_HEIGHT, m_position);

	//�|�[�^���K���̐ݒ�B
	m_portalGun = NewGO<PortalGun>(3, "portalGun");
	//�J�����̐ݒ�B
	m_gameCamera = NewGO<GameCamera>(2, "gameCamera");

	//�����̐ݒ�B
	a.SetPosition(Vector3(400.0f, 0.0f, 0.0f));
	a.SetShadowParam(true, 0.5f, Vector4::Black);

	//���Z�b�g�����B
	Reset();

	return true;
}

void Player::Update()
{
	//�v���C���[�����S���Ă��Ȃ��Ȃ�B
	if (m_playerState != enState_Dead) {

		Input();

		PlayAnimation();

		m_modelRender.Update();
	}

	State();

	////////////// todo debug
	Vector3 vec = g_camera3D->GetForward();
	wchar_t text[256];
	swprintf_s(text, 256,
		L"PlayerX:%.2f \nPlayerY:%.2f \nPlayerZ:%.2f \nForwardX:%.2f \nForwardY:%.2f \nForwardZ:%.2f \nAnim:%d",
		m_position.x,
		m_position.y,
		m_position.z,
		vec.x,
		vec.y,
		vec.z,
		m_playerState
	);
	a.SetText(text);
	/////////////

	if (m_damageTimer > 0.0f) {
		m_damageTimer -= g_gameTime->GetFrameDeltaTime();
		m_damageSpriteRender.SetMulColor(Vector4(1.0f, 1.0f, 1.0f, m_damageTimer));
		m_damageSpriteRender.Update();
	}
}

/// <summary>
/// ���͏����B
/// </summary>
void Player::Input()
{
	//�ړ����x���������B
	m_moveSpeed = Vector3::Zero;

	MoveXZ();

	MoveY();

	//�ǉ��̈ړ����x�����Z�B
	m_moveSpeed += m_addMoveSpeed;
	//�L�����R���̎��s�B
	m_position = m_characterController.Execute(m_moveSpeed, 1.0f / 60.0f);
	m_modelRender.SetPosition(m_position);

	Rotation();

	if (m_addMoveSpeed.x < ADDMOVESPEED_MIN &&
		m_addMoveSpeed.y < ADDMOVESPEED_MIN &&
		m_addMoveSpeed.z < ADDMOVESPEED_MIN
	){
		//�ǉ��̈ړ����x���������B
		m_addMoveSpeed = Vector3::Zero;
	}
	else {
		//�ǉ��̈ړ����x�����Z�B
		m_addMoveSpeed -= m_addMoveSpeed / ADDMOVESPEED_DIV;
	}
}

/// <summary>
/// XZ�����̈ړ������B
/// </summary>
void Player::MoveXZ()
{
	//���X�e�B�b�N�̓��͂��擾�B
	Vector3 input;
	input.x = g_pad[0]->GetLStickXF();
	input.z = g_pad[0]->GetLStickYF();

	//���͂��Ȃ��Ȃ�A�������Ȃ��B
	if (fabsf(input.x) < 0.01f &&
		fabsf(input.z) < 0.01f
	) {
		return;
	}

	//�J�����̑O�������擾�B
	Vector3 cameraForward = g_camera3D->GetForward();
	cameraForward.y = 0.0f;
	cameraForward.Normalize();

	//�J�����̉E�������擾�B
	Vector3 cameraRight = g_camera3D->GetRight();
	cameraRight.y = 0.0f;
	cameraRight.Normalize();

	//�ړ����x�ɃJ�����̕������l�����ĉ��Z�B
	m_moveSpeed += cameraForward * input.z;
	m_moveSpeed += cameraRight * input.x;

	//���K��������B
	m_moveSpeed.Normalize();

	//�W�����v���łȂ��Ȃ�A
	if (m_playerState != enState_Jump &&
		m_playerState != enState_Crouch_Jump) {

		//���Ⴊ�ݕ����B
		if (m_playerState == enState_Crouch_Walk) {
			m_walkSpeed = CROUCH_SPEED;
		}
		else {
			m_walkSpeed = WALK_SPEED;
		}
	}

	//�ړ����x���X�e�[�g�ɉ����ď�Z�B
	m_moveSpeed *= m_walkSpeed;
}

/// <summary>
/// Y�����̈ړ������B
/// </summary>
void Player::MoveY()
{
	//�W�����v���Ȃ�B
	if (m_playerState == enState_Jump ||
		m_playerState == enState_Crouch_Jump) {
		//�ړ����x�����Z�B
		m_moveSpeed.y += JUMP_POWER;
	}

	//�n�ʂɂ��Ȃ��Ȃ�B
	if (!m_characterController.IsOnGround()) {
		//�d�͂𔭐��B
		m_moveSpeed.y -= GRAVITY * m_gravityAccel;
		m_gravityAccel += GRAVITY_ACCEL;
	}
}

/// <summary>
/// ��]�����B
/// </summary>
void Player::Rotation()
{
	//�J�����̑O�����ɉ�]����B
	m_rotation.SetRotationYFromDirectionXZ(g_camera3D->GetForward());
	m_modelRender.SetRotation(m_rotation);
}

/// <summary>
/// ���[�v�����B
/// </summary>
/// <param name="pos">���W</param>
/// <param name="angle">��]��</param>
void Player::SetWarp(const Vector3& pos, const float angle)
{
	//���W��ݒ�B
	m_position = pos;
	m_characterController.SetPosition(m_position);
	//�J���������[�v������B
	m_gameCamera->SetWarp(angle);
}

/// <summary>
/// �A�j���[�V���������B
/// </summary>
void Player::PlayAnimation()
{
	//�A�j���[�V�������Đ��B
	switch (m_playerState)
	{
	case enState_Idle:
		m_modelRender.PlayAnimation(enState_Idle, INTERPOLATE_TIME);
		break;

	case enState_Walk:
		m_modelRender.PlayAnimation(enState_Walk, INTERPOLATE_TIME);
		break;

	case enState_Crouch_Idle:

		break;

	case enState_Jump:
		m_modelRender.PlayAnimation(enState_Jump, INTERPOLATE_TIME);
		break;
	}
}

/// <summary>
/// �X�e�[�g�����B
/// </summary>
void Player::State()
{
	switch (m_playerState)
	{
	case enState_Idle:
		ProcessIdleStateTransition();
		break;

	case enState_Walk:
		ProcessWalkStateTransition();
		break;

	case enState_Crouch_Idle:
		ProcessCrouchIdleStateTransition();
		break;

	case enState_Crouch_Walk:
		ProcessCrouchWalkStateTransition();
		break;

	case enState_Crouch_Jump:
		ProcessCrouchJumpStateTransition();
		break;

	case enState_Jump:
		ProcessJumpStateTransition();
		break;

	case enState_Dead:
		ProcessDeadStateTransition();
	}
}

/// <summary>
/// �ʏ�̑J�ڏ����B
/// </summary>
void Player::ProcessCommonStateTransition()
{
	//�J�����̍��W��ݒ�B
	if (m_playerState == enState_Crouch_Idle ||
		m_playerState == enState_Crouch_Walk) {
		m_gameCamera->SetCrouchState(true);
	}
	else {
		m_gameCamera->SetCrouchState(false);
	}

	//���Ⴊ�݃{�^���������ꂽ��B
	if (g_pad[0]->IsPress(enButtonY)) {

		//�W�����v�{�^���������ꂽ��B
		if (g_pad[0]->IsTrigger(enButtonA)) {
			m_playerState = enState_Crouch_Jump;
			return;
		}

		//�ړ����x������Ȃ�B
		if (fabsf(m_moveSpeed.x) > 0.001f || fabsf(m_moveSpeed.z) > 0.001f) {
			m_playerState = enState_Crouch_Walk;
			return;
		}

		m_playerState = enState_Crouch_Idle;
		return;
	}

	//�W�����v�{�^���������ꂽ��B
	if (g_pad[0]->IsTrigger(enButtonA)) {
		m_playerState = enState_Jump;
		return;
	}

	//�ړ����x������Ȃ�B
	if (fabsf(m_moveSpeed.x) > 0.001f || fabsf(m_moveSpeed.z) > 0.001f) {
		m_playerState = enState_Walk;
		return;
	}

	m_playerState = enState_Idle;
}

/// <summary>
/// �ҋ@��Ԃ̑J�ڏ����B
/// </summary>
void Player::ProcessIdleStateTransition()
{
	ProcessCommonStateTransition();
}

/// <summary>
/// ������Ԃ̑J�ڏ����B
/// </summary>
void Player::ProcessWalkStateTransition()
{
	ProcessCommonStateTransition();
}

/// <summary>
/// ���Ⴊ�ݑҋ@��Ԃ̑J�ڏ����B
/// </summary>
void Player::ProcessCrouchIdleStateTransition()
{
	ProcessCommonStateTransition();
}

/// <summary>
/// ���Ⴊ�ݕ�����Ԃ̑J�ڏ����B
/// </summary>
void Player::ProcessCrouchWalkStateTransition()
{
	ProcessCommonStateTransition();
}

/// <summary>
/// ���Ⴊ�݃W�����v��Ԃ̑J�ڏ����B
/// </summary>
void Player::ProcessCrouchJumpStateTransition()
{
	//�n�ʂɂ�����B
	if (m_characterController.IsOnGround()) {

		//�d�͉����x���������B
		m_gravityAccel = 0.0f;

		ProcessCommonStateTransition();
	}
}

/// <summary>
/// �W�����v��Ԃ̑J�ڏ����B
/// </summary>
void Player::ProcessJumpStateTransition()
{
	//�n�ʂɂ�����B
	if (m_characterController.IsOnGround()) {

		//�d�͉����x���������B
		m_gravityAccel = 0.0f;

		ProcessCommonStateTransition();
	}
}

/// <summary>
/// ���S��Ԃ̑J�ڏ����B
/// </summary>
void Player::ProcessDeadStateTransition()
{
	m_deadTimer += g_gameTime->GetFrameDeltaTime();

	//���S���Ԃ��o�߂�����B
	if (m_deadTimer > DEAD_TIME) {
		Reset();
		return;
	}
}

/// <summary>
/// �_���[�W���󂯂鏈���B
/// </summary>
/// <param name="damage">�^����_���[�W��</param>
/// <param name="dir">�̂��������</param>
void Player::ReceiveDamage(const int damage, const Vector3& dir)
{
	if (m_damageTimer > 0.0f || 
		m_playerState == enState_Dead
	) {
		return;
	}

	//�_���[�W�����炷�B
	m_hp -= damage;

	//�_���[�W��0�ɂȂ�����B
	if (m_hp < 0) {
		//���S��Ԃɂ���B
		m_playerState = enState_Dead;
		m_gameCamera->Dead();
	}

	m_damageTimer = DAMAGE_TIME;

	//���ɂ̂�����B
	Vector3 knockback = dir * KNOCKBACK_POWER;
	knockback.y = 0.0f;
	m_addMoveSpeed += knockback;
}

/// <summary>
/// ���Z�b�g�����B
/// </summary>
void Player::Reset()
{
	//�l���������B
	m_hp = MAX_HP;
	m_walkSpeed = 0.0f;
	m_gravityAccel = 0.0f;
	m_damageTimer = 0.0f;
	m_deadTimer = 0.0f;
	m_playerState = enState_Idle;
	m_addMoveSpeed = Vector3::Zero;

	//�Q�[���Ƀ��Z�b�g��Ԃ�ʒm�B
	m_game->NotifyReset();

	//�J���������Z�b�g�B
	m_gameCamera->Reset();

	//���W���������B
	m_position = m_game->GetRespawnPoint();
	m_characterController.SetPosition(m_position);
}

void Player::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
	m_damageSpriteRender.Draw(rc);
	a.Draw(rc);
}