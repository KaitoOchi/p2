#include "stdafx.h"
#include "Player.h"

namespace
{
	const float MOVE_SPEED = 5.0f;			//�ړ����x�B
}

Player::Player()
{

}

Player::~Player()
{

}

bool Player::Start()
{
	m_modelRender.Init("Assets/modelData/unityChan.tkm");
	m_modelRender.Update();

	return true;
}

void Player::Update()
{
	Input();

	State();

	wchar_t text[256];
	swprintf_s(text, 256,
		L"CX:%.2f, \nCZ:%.2f",
		g_camera3D->GetForward().x,
		g_camera3D->GetForward().z
	);
	a.SetText(text);
}

void Player::Input()
{
	Move();

	Rotation();

	m_modelRender.Update();

	m_gameCamera.Update();
}

void Player::Move()
{
	//���X�e�B�b�N�̓��͂��擾�B
	Vector3 input;
	input.x = g_pad[0]->GetLStickXF();
	input.z = g_pad[0]->GetLStickYF();

	//�J�����̑O�������擾�B
	Vector3 cameraForward = g_camera3D->GetForward();
	cameraForward.y = 0.0f;
	cameraForward.Normalize();

	//�J�����̉E�������擾�B
	Vector3 cameraRight = g_camera3D->GetRight();
	cameraRight.y = 0.0f;
	cameraRight.Normalize();

	//�ړ����x�ɃJ�����̕������l�����ĉ��Z�B
	Vector3 moveSpeed;
	moveSpeed += cameraForward * input.z;
	moveSpeed += cameraRight * input.x;

	//���K��������B
	moveSpeed.Normalize();

	m_position += moveSpeed * MOVE_SPEED;
	m_modelRender.SetPosition(m_position);

	//�J�����̍��W��ݒ�B
	m_gameCamera.SetPosition(m_position);
}

void Player::Rotation()
{
	//�J�����̉�]��ݒ�B
	m_gameCamera.SetRotation();
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

	case enState_Crouch:
		ProcessCrouchStateTransition();
		break;

	case enState_Jump:
		ProcessJumpStateTransition();
		break;
	}
}

void Player::ProcessCommonStateTransition()
{

}

void Player::ProcessIdleStateTransition()
{
	ProcessCommonStateTransition();
}

void Player::ProcessWalkStateTransition()
{
	ProcessCommonStateTransition
}

void Player::ProcessRunStateTransition()
{
	ProcessCommonStateTransition
}

void Player::ProcessCrouchStateTransition()
{

}

void Player::ProcessJumpStateTransition()
{

}

void Player::Render(RenderContext& rc)
{
	//m_modelRender.Draw(rc);
	a.Draw(rc);
}