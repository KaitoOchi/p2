#include "stdafx.h"
#include "Player.h"
#include "PortalGun.h"

namespace
{
	const float WALK_SPEED = 100.0f;			//�ړ����x�B
	const float DASH_SPEED = 300.0f;			//�_�b�V�����x�B
	const float CROUCH_SPEED = 40.0f;			//���Ⴊ�ݑ��x�B
	const float JUMP_POWER = 125.0f;			//�W�����v�ʁB
	const float GRAVITY = 10.0f;				//�d�́B
	const float GRAVITY_ACCEL = 0.5f;			//�d�͉����B
	const float INTERPOLATE_TIME = 0.2f;		//���`��ԁB
}

Player::Player()
{

}

Player::~Player()
{

}

bool Player::Start()
{
	//�A�j���[�V�����N���b�v
	m_animationClips[enState_Idle].Load("Assets/animData/UnityChan/idle.tka");
	m_animationClips[enState_Idle].SetLoopFlag(true);
	m_animationClips[enState_Walk].Load("Assets/animData/UnityChan/walk.tka");
	m_animationClips[enState_Walk].SetLoopFlag(true);
	m_animationClips[enState_Run].Load("Assets/animData/UnityChan/run.tka");
	m_animationClips[enState_Run].SetLoopFlag(true);
	m_animationClips[enState_Jump].Load("Assets/animData/UnityChan/jump.tka");
	m_animationClips[enState_Jump].SetLoopFlag(false);

	//���f���̐ݒ�B
	m_modelRender.Init("Assets/modelData/unityChan.tkm", m_animationClips, enState_Num, enModelUpAxisY, true, true);
	m_modelRender.Update();

	m_characterController.Init(25.0f, 50.0f, m_position);

	//�J�����̐ݒ�B
	m_gameCamera.Init();

	//�|�[�^���K���̐ݒ�B
	m_portalGun = NewGO<PortalGun>(0, "portalGun");

	//�����̐ݒ�B
	a.SetPosition(Vector3(400.0f, 0.0f, 0.0f));
	a.SetShadowParam(true, 0.5f, Vector4::Black);

	return true;
}

void Player::Update()
{
	Input();

	PlayAnimation();

	State();

	m_modelRender.Update();


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
}

void Player::Input()
{
	m_moveSpeed = Vector3::Zero;

	MoveXZ();

	MoveY();

	m_position = m_characterController.Execute(m_moveSpeed, 1.0f / 60.0f);
	//m_modelRender.SetPosition(m_position);

	//�J�����̍��W��ݒ�B
	if (m_playerState == enState_Crouch_Idle || 
		m_playerState == enState_Crouch_Walk ) {
		m_gameCamera.SetCrouchState(true);
	}
	else {
		m_gameCamera.SetCrouchState(false);
	}
	m_gameCamera.SetPosition(m_position);

	Rotation();

	Shot();
}

void Player::MoveXZ()
{
	//���X�e�B�b�N�̓��͂��擾�B
	Vector3 input;
	input.x = g_pad[0]->GetLStickXF();
	input.z = g_pad[0]->GetLStickYF();

	//���͂��Ȃ��Ȃ�A�������Ȃ��B
	if (fabsf(input.x) < 0.01f && fabsf(input.z) < 0.01f) {
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

		//�_�b�V��������B
		if (m_playerState == enState_Run) {
			m_walkSpeed = DASH_SPEED;
		}
		else if (m_playerState == enState_Crouch_Walk) {
			m_walkSpeed = CROUCH_SPEED;
		}
		else {
			m_walkSpeed = WALK_SPEED;
		}
	}

	m_moveSpeed *= m_walkSpeed;
}

void Player::MoveY()
{
	if (m_playerState == enState_Jump ||
		m_playerState == enState_Crouch_Jump) {
		//�W�����v����B
		m_moveSpeed.y += JUMP_POWER;
	}

	//�n�ʂɂ��Ȃ��Ȃ�B
	if (!m_characterController.IsOnGround()) {
		//�d�͂𔭐��B
		m_moveSpeed.y -= GRAVITY * m_gravityAccel;
		m_gravityAccel += GRAVITY_ACCEL;
	}
}

void Player::Rotation()
{
	//�J�����̉�]��ݒ�B
	m_gameCamera.SetRotation();

	//�J�����̑O�����ɉ�]����B
	m_rotation.SetRotationYFromDirectionXZ(g_camera3D->GetForward());
	m_modelRender.SetRotation(m_rotation);
}

void Player::Shot()
{
	//�|�[�^����ݒu�B
	if (g_pad[0]->IsTrigger(enButtonLB1)) {
		m_portalGun->SetPortal(PortalGun::enPortalType_Blue, m_position, m_rotation);
	}
	//�ԃ|�[�^����ݒu�B
	else if (g_pad[0]->IsTrigger(enButtonRB1)) {
		m_portalGun->SetPortal(PortalGun::enPortalType_Red, m_position, m_rotation);
	}
}

void Player::PlayAnimation()
{
	switch (m_playerState)
	{
	case enState_Idle:
		m_modelRender.PlayAnimation(enState_Idle, INTERPOLATE_TIME);
		break;

	case enState_Walk:
		m_modelRender.PlayAnimation(enState_Walk, INTERPOLATE_TIME);
		break;

	case enState_Run:
		m_modelRender.PlayAnimation(enState_Run, INTERPOLATE_TIME);
		break;

	case enState_Crouch_Idle:

		break;

	case enState_Jump:
		m_modelRender.PlayAnimation(enState_Jump, INTERPOLATE_TIME);
		break;
	}
}

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

	case enState_Run:
		ProcessRunStateTransition();
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
	}
}

void Player::ProcessCommonStateTransition()
{
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

	//�_�b�V���{�^���������ꂽ��B
	if (g_pad[0]->IsPress(enButtonX)) {
		m_playerState = enState_Run;
		return;
	}

	//�ړ����x������Ȃ�B
	if (fabsf(m_moveSpeed.x) > 0.001f || fabsf(m_moveSpeed.z) > 0.001f) {
		m_playerState = enState_Walk;
		return;
	}

	m_playerState = enState_Idle;
}

void Player::ProcessIdleStateTransition()
{
	ProcessCommonStateTransition();
}

void Player::ProcessWalkStateTransition()
{
	ProcessCommonStateTransition();
}

void Player::ProcessRunStateTransition()
{
	ProcessCommonStateTransition();
}

void Player::ProcessCrouchIdleStateTransition()
{
	ProcessCommonStateTransition();
}

void Player::ProcessCrouchWalkStateTransition()
{
	ProcessCommonStateTransition();
}

void Player::ProcessCrouchJumpStateTransition()
{
	//�n�ʂɂ�����B
	if (m_characterController.IsOnGround()) {

		//�d�͉����x���������B
		m_gravityAccel = 0.0f;

		ProcessCommonStateTransition();
	}
}

void Player::ProcessJumpStateTransition()
{
	//�n�ʂɂ�����B
	if (m_characterController.IsOnGround()) {

		//�d�͉����x���������B
		m_gravityAccel = 0.0f;

		ProcessCommonStateTransition();
	}
}

void Player::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
	a.Draw(rc);
}