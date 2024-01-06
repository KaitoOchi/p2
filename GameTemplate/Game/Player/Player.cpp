#include "stdafx.h"
#include "Player.h"
#include "PortalGun.h"
#include "PortalFrame.h"
#include "GameCamera.h"
#include "Game.h"
#include "Fade.h"

namespace
{
	const Vector3	MODEL_SCALE = Vector3(1.25f, 1.25f, 1.25f);		//���f���̊g�嗦�B
	const int		MAX_HP = 100;									//�ő�HP�B
	const float		CHARACON_RADIUS = 10.0f;						//�L�����R���̔��a�B
	const float		CHARACON_HEIGHT = 60.0f;						//�L�����R���̍����B
	const float		WALK_SPEED = 180.0f;							//�ړ����x�B
	const float		CROUCH_SPEED = 40.0f;							//���Ⴊ�ݑ��x�B
	const float		JUMP_POWER = 140.0f;							//�W�����v�ʁB
	const float		GRAVITY = 10.0f;								//�d�́B
	const float		GRAVITY_ACCEL = 0.75f;							//�d�͉����B
	const float		INTERPOLATE_TIME = 0.2f;						//���`��ԁB
	const float		KNOCKBACK_POWER = 300.0f;						//�_���[�W���󂯂��Ƃ��̃m�b�N�o�b�N�p���[�B
	const float		ADDMOVESPEED_MIN = 1.0f;						//�ǉ��̈ړ����x���I������l�B
	const float		ADDMOVESPEED_DIV = 5.0f;						//�ǉ��̈ړ����x�����Z����l�B
	const float		DAMAGE_TIME = 0.3f;								//�_���[�W���󂯂鎞�ԁB
}

Player::Player()
{
}

Player::~Player()
{
}

bool Player::Start()
{
	//�I�u�W�F�N�g�̌����B
	m_game = FindGO<Game>("game");
	m_gameCamera = FindGO<GameCamera>("gameCamera");

	//�A�j���[�V�����N���b�v�B
	m_animationClips[enState_Idle].Load("Assets/animData/player/idle.tka");
	m_animationClips[enState_Idle].SetLoopFlag(true);
	m_animationClips[enState_Walk].Load("Assets/animData/player/walk.tka");
	m_animationClips[enState_Walk].SetLoopFlag(true);
	m_animationClips[enState_Jump].Load("Assets/animData/player/jump_start.tka");
	m_animationClips[enState_Jump].SetLoopFlag(false);
	m_animationClips[enState_JumpEnd].Load("Assets/animData/player/jump_end.tka");
	m_animationClips[enState_JumpEnd].SetLoopFlag(false);

	//���f���̐ݒ�B
	m_modelRender.Init(
		"Assets/modelData/player.tkm",
		m_animationClips,
		enState_Num,
		enModelUpAxisZ,
		true,
		true,
		ModelRender::enModelInitMode_UnDrawMainCamera
	);
	m_modelRender.SetScale(MODEL_SCALE);
	m_modelRender.Update();
	m_modelRender.AddAnimationEvent([&](const wchar_t* clipName, const wchar_t* eventName)
		{
			OnAnimationEvent(clipName, eventName);
		});

	//�_���[�W�摜�̐ݒ�B
	m_damageSpriteRender.Init("Assets/sprite/UI/damage.DDS", 1600.0f, 900.0f);
	m_damageSpriteRender.SetMulColor(Vector4(1.0f, 1.0f, 1.0f, m_damageTimer));
	m_damageSpriteRender.Update();

	//�L�����N�^�[�R���g���[���[�̐ݒ�B
	m_characterController = new CharacterController;
	m_characterController->Init(CHARACON_RADIUS, CHARACON_HEIGHT, m_position);

	//�����̐ݒ�B
	a.SetPosition(Vector3(400.0f, 0.0f, 0.0f));
	a.SetShadowParam(true, 0.5f, Vector4::Black);

	return true;
}

void Player::Update()
{
	if (m_playerState == enState_End ||
		m_playerState == enState_Dead
	) {
		return;
	}

	Input();

	PlayAnimation();

	m_modelRender.Update();

	Vector3 diff = m_game->GetClearPosition() - m_position;
	if (diff.LengthSq() < 100.0f * 100.0f) {
		m_game->NotifyClear();
		m_playerState = enState_End;
	}

	State();

#ifdef _DEBUG
	Debug();
#endif

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
	m_position = m_characterController->Execute(m_moveSpeed, 1.0f / 60.0f);
	m_modelRender.SetPosition(m_position);

	Rotation();

	Crouch();

	if (m_addMoveSpeed.x < ADDMOVESPEED_MIN &&
		m_addMoveSpeed.y < ADDMOVESPEED_MIN &&
		m_addMoveSpeed.z < ADDMOVESPEED_MIN
		) {
		//�ǉ��̈ړ����x���������B
		m_addMoveSpeed = Vector3::Zero;
	}
	else {
		//�ǉ��̈ړ����x�����Z�B
		m_addMoveSpeed -= m_addMoveSpeed / ADDMOVESPEED_DIV;
	}

	m_gameCamera->SetPosition(m_position);
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
		m_playerState != enState_JumpEnd
	) {
		//���Ⴊ�ݕ����B
		if (m_isCrouch) {
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
	if (m_playerState == enState_Jump) {
		//�ړ����x�����Z�B
		m_moveSpeed.y += JUMP_POWER;
	}

	//�V��ɂԂ������Ȃ�B
	if (m_characterController->IsOnSeiling()) {
		//�W�����v����߂�B
		m_moveSpeed.y = 0.0f;
		m_playerState = enState_JumpEnd;
	}

	//�n�ʂɂ��Ȃ��Ȃ�B
	if (!m_characterController->IsOnGround()) {
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
/// ���Ⴊ�ݏ����B
/// </summary>
void Player::Crouch()
{
	//�W�����v���Ȃ�B
	if (m_playerState == enState_Jump ||
		m_playerState == enState_JumpEnd
	) {
		return;
	}

	if (m_isCrouch) {
		//���Ⴊ�݃{�^���������ꂽ��B
		if (!g_pad[0]->IsPress(enButtonY)) {
			//���Ⴊ�ݏ�Ԃ��痧���オ���Ȃ�B
			if (m_characterController->IsCanStandUp()) {
				//�L�����R���̍������グ��B
				delete m_characterController;
				m_characterController = new CharacterController;
				m_characterController->Init(CHARACON_RADIUS, CHARACON_HEIGHT, m_position);

				//�J�����̍��W��ݒ�B
				m_gameCamera->SetCrouchState(false);
				m_isCrouch = false;
			}
		}
	}
	else {
		//���Ⴊ�݃{�^���������ꂽ��B
		if (g_pad[0]->IsTrigger(enButtonY)) {
			//�L�����R���̍�����������B
			delete m_characterController;
			m_characterController = new CharacterController;
			m_characterController->Init(CHARACON_RADIUS, CHARACON_HEIGHT / 2.0f, m_position);

			//�J�����̍��W��ݒ�B
			m_gameCamera->SetCrouchState(true);
			m_isCrouch = true;
		}
	}
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

	case enState_Jump:
		m_modelRender.PlayAnimation(enState_Jump, INTERPOLATE_TIME);
		break;

	case enState_JumpEnd:
		m_modelRender.PlayAnimation(enState_JumpEnd, INTERPOLATE_TIME);
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

	case enState_Jump:
		ProcessJumpStateTransition();
		break;

	case enState_JumpEnd:
		ProcessJumpEndStateTransition();
		break;
	}
}

/// <summary>
/// �ʏ�̑J�ڏ����B
/// </summary>
void Player::ProcessCommonStateTransition()
{
	//�n�ʂɂ���Ȃ�B
	if (m_characterController->IsOnGround()) {
		//�W�����v�{�^���������ꂽ��B
		if (g_pad[0]->IsTrigger(enButtonA)) {
			m_playerState = enState_Jump;
			return;
		}
	}
	else {
		m_playerState = enState_JumpEnd;
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
/// �W�����v��Ԃ̑J�ڏ����B
/// </summary>
void Player::ProcessJumpStateTransition()
{
	//�������n�܂�����B
	if (m_moveSpeed.y < 0.0f) {
		m_playerState = enState_JumpEnd;
	}

	//�n�ʂɂ�����B
	if (m_characterController->IsOnGround()) {
		//�d�͉����x���������B
		m_gravityAccel = 0.0f;
		ProcessCommonStateTransition();
	}
}

/// <summary>
/// �W�����v�I����Ԃ̑J�ڏ����B
/// </summary>
void Player::ProcessJumpEndStateTransition()
{
	//�n�ʂɂ�����B
	if (m_characterController->IsOnGround()) {
		//�d�͉����x���������B
		m_gravityAccel = 0.0f;
		ProcessCommonStateTransition();
	}
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
	m_characterController->SetPosition(m_position);
	//�J���������[�v������B
	m_gameCamera->SetWarp(angle);
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
		m_game->NotifyReset();
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
	m_isCrouch = false;
	m_hp = MAX_HP;
	m_walkSpeed = 0.0f;
	m_gravityAccel = 0.0f;
	m_damageTimer = 0.0f;
	m_playerState = enState_Idle;
	m_addMoveSpeed = Vector3::Zero;

	//���W���������B
	m_position = m_game->GetRespawnPosition();
	m_characterController->SetPosition(m_position);

	//�t�F�[�h�C�����J�n�B
	Fade* fade = FindGO<Fade>("fade");
	fade->StartFadeIn();
}


void Player::OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName)
{
	(void)clipName;
	//�C�x���g�Ȃ�
	if (wcscmp(eventName, L"step") == 0)
	{
		m_game->AddStepNum();
	}

}


/// <summary>
/// �f�o�b�O�����B
/// </summary>
void Player::Debug()
{
	////////////// todo debug
	Vector3 vec = g_camera3D->GetForward();
	wchar_t text[256];
	swprintf_s(text, 256,
		L"PlayerX:%.2f \nPlayerY:%.2f \nPlayerZ:%.2f \nForwardX:%.2f \nForwardY:%.2f \nForwardZ:%.2f \nAnim:%d \nGround:%d \nSeiling:%d \nCrouch:%d",
		m_position.x,
		m_position.y,
		m_position.z,
		vec.x,
		vec.y,
		vec.z,
		m_playerState,
		m_characterController->IsOnGround(),
		m_characterController->IsOnSeiling(),
		m_isCrouch
	);
	a.SetText(text);
	/////////////
}

void Player::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
	m_damageSpriteRender.Draw(rc);
	a.Draw(rc);
}