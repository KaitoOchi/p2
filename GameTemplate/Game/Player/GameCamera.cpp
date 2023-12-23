#include "stdafx.h"
#include "GameCamera.h"

namespace
{
	const float CAMERA_NEAR = 1.0f;				//�ߕ��ʁB
	const float CAMERA_FAR = 10000.0f;			//�����ʁB
	const float TARGET_FORWARD = 250.0f;		//�����_�̑O�����B
	const float ROT_SPEED = 0.03f;				//��]���x�B
	const float CAMERA_HEIGHT = 75.0f;			//�J�����̍����B
	const float CAMERA_HEIGHT_CROUCH = 20.0f;	//���Ⴊ�ݏ�Ԃ̍����B
	const float ANGLE_Y = 1.56f;				//�J�����̏㉺��]�̏���l
	const float CROUCH_SPEED = 5.0f;			//���Ⴊ�ݑ��x�B
	const float CROUCH_TIMER_START = 0.0f;		//���Ⴊ�݊J�n�̎��ԁB
	const float CROUCH_TIMER_END = 1.0f;		//���Ⴊ�ݏI���̎��ԁB
	const float WARP_ANGLE_ADD = 180.0f;		//���[�v���ɉ��Z�����]�ʁB
	const float SHAKE_SPEED = 5.0f;				//��ʗh��̑��x�B
	const float SHAKE_POWER = 4.0f;			//��ʗh��̗ʁB
}


GameCamera::GameCamera()
{
	m_rotSpeed.x = Math::DegToRad(90);
	m_shakeMoveSpeedTmp = m_position;
}

GameCamera::~GameCamera()
{

}

bool GameCamera::Start()
{
	//�ߕ��ʂƉ����ʂ�ݒ�B
	g_camera3D->SetNear(CAMERA_NEAR);
	g_camera3D->SetFar(CAMERA_FAR);

	return true;
}

void GameCamera::Update()
{
	if (m_isDead) {
		return;
	}

	Move();

	Rotation();

	Shake();

	g_camera3D->Update();
}

/// <summary>
/// �ړ������B
/// </summary>
void GameCamera::Move()
{
	//���Ⴊ�݂̍��W��ݒ�B
	if (m_crouchTimer >= CROUCH_TIMER_START && m_crouchTimer <= CROUCH_TIMER_END) {
		//���Ⴊ�݂̎n�_��ݒ�B
		Vector3 cameraPos = m_position;
		cameraPos.y += CAMERA_HEIGHT;
		m_firstCrouchPos = cameraPos;

		//���Ⴊ�݂̏I�_��ݒ�B
		cameraPos = m_position;
		cameraPos.y += CAMERA_HEIGHT_CROUCH;
		m_lastCrouchPos = cameraPos;
	}

	//���Ⴊ�ݏ�ԂȂ�B
	if (m_isCrouch) {
		//���Ⴊ��ł��鎞�Ԃ��o�߂���܂ŁB
		if (m_crouchTimer < CROUCH_TIMER_END) {
			//���`��Ԃł��Ⴊ�݂��J�n����B
			m_crouchTimer += g_gameTime->GetFrameDeltaTime() * CROUCH_SPEED;
			m_position.Lerp(m_crouchTimer, m_firstCrouchPos, m_lastCrouchPos);
		}
		else {
			m_crouchTimer = CROUCH_TIMER_END;
			m_position.y += CAMERA_HEIGHT_CROUCH;
		}
	}
	//���Ⴊ�ݏ�ԂłȂ��Ȃ�B
	else {
		//���Ⴊ�ݏI���̎��Ԃ܂ŁB
		if (m_crouchTimer > CROUCH_TIMER_START) {
			//���`��Ԃł��Ⴊ�݂���������B
			m_crouchTimer -= g_gameTime->GetFrameDeltaTime() * CROUCH_SPEED;
			m_position.Lerp(m_crouchTimer, m_firstCrouchPos, m_lastCrouchPos);
		}
		else {
			m_crouchTimer = CROUCH_TIMER_START;
			m_position.y += CAMERA_HEIGHT;
		}
	}
}

/// <summary>
/// ��]�����B
/// </summary>
void GameCamera::Rotation()
{
	//�E�X�e�B�b�N�̓��͂��擾�B
	Vector3 input;
	input.x = -g_pad[0]->GetRStickXF();
	input.y = g_pad[0]->GetRStickYF();

	//�X�e�B�b�N�̓��͂����Z�B
	m_rotSpeed.x += input.x * ROT_SPEED;
	m_rotSpeed.y += input.y * ROT_SPEED;

	//�㉺��]�̌��E�l��͈͓��Ɏ��߂�B
	m_rotSpeed.y = min(max(m_rotSpeed.y, -ANGLE_Y), ANGLE_Y);

	//�����_����]������B
	m_targetPos.x = cos(m_rotSpeed.x) * cos(m_rotSpeed.y);
	m_targetPos.y = sin(m_rotSpeed.y);
	m_targetPos.z = sin(m_rotSpeed.x) * cos(m_rotSpeed.y);

	//�����_�𐳋K������B
	m_targetPos.Normalize();
	//�����_��O�����Ɉړ�������B
	m_targetPos *= TARGET_FORWARD;
	//�����_���v���C���[�̕����Ɉړ�������B
	m_targetPos += m_position;

	g_camera3D->SetTarget(m_targetPos);
}

void GameCamera::Shake()
{
	m_shakeTimer += g_gameTime->GetFrameDeltaTime();

	Vector2 input;
	input.x = g_pad[0]->GetLStickXF();
	input.y = g_pad[0]->GetLStickYF();
	float shakeValue = max(fabsf(input.x), fabsf(input.y));

	Vector3 shakeMoveSpeed;

	//�X�e�B�b�N�̓��͂�����Ȃ�B
	if (shakeValue) {
		if (m_isIdle) {
			m_shakeTimer = 0.0f;
			m_isIdle = false;
		}

		//�h���ǉ��B
		shakeMoveSpeed.y = sin(m_shakeTimer * shakeValue * SHAKE_SPEED) * SHAKE_POWER;
		m_shakeMoveSpeedTmp = shakeMoveSpeed;
	}
	else {
		if (!m_isIdle) {
			m_shakeTimer = 0.0f;
			m_isIdle = true;
		}

		//1�b�𒴂��Ȃ��悤�ɂ���B
		m_shakeTimer = min(m_shakeTimer, 1.0f);

		//���`��ԂŌ��̈ʒu�ɖ߂��B
		shakeMoveSpeed.Lerp(m_shakeTimer, m_shakeMoveSpeedTmp + m_position, m_position);
		shakeMoveSpeed -= m_position;
	}

	//�h��̈ړ��ʂ����Z�B
	shakeMoveSpeed += m_position;
	g_camera3D->SetPosition(shakeMoveSpeed);
}

/// <summary>
/// ���[�v�����B
/// </summary>
/// <param name="angle">��]��</param>
void GameCamera::SetWarp(const float angle)
{
	//�|�[�^���̊p�x�ɉ����ĉ�]��ύX�B
	m_rotSpeed.x += Math::DegToRad(WARP_ANGLE_ADD - angle);
	//m_rotSpeed.y += Math::DegToRad(-angle);

	Rotation();
}

/// <summary>
/// ���S�����B
/// </summary>
void GameCamera::Dead()
{
	m_isCrouch = false;
	m_isDead = true;
	g_camera3D->Move(Vector3(0.0f, -50.0f, 0.0f));
	g_camera3D->SetUp(Vector3(-1.0f, 0.0f, 0.0f));
	g_camera3D->Update();
}

/// <summary>
/// ���Z�b�g�����B
/// </summary>
void GameCamera::Reset()
{
	m_isDead = false;
	m_isIdle = false;
	m_rotSpeed.x = Math::DegToRad(90);
	m_shakeMoveSpeedTmp = Vector3::Zero;
	m_shakeTimer = 0.0f;
	g_camera3D->SetUp(Vector3(0.0f, 1.0f, 0.0f));
	g_camera3D->Update();
}

/// <summary>
/// �I�������B
/// </summary>
void GameCamera::End()
{
	m_isCrouch = false;
	m_isDead = true;
}