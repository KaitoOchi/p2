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
	const float ANGLE_Y = 1.56f;					//�J�����̏㉺��]�̏���l
	const float CROUCH_SPEED = 5.0f;			//���Ⴊ�ݑ��x�B
	const float CROUCH_TIMER_START = 0.0f;		//���Ⴊ�݊J�n�̎��ԁB
	const float CROUCH_TIMER_END = 1.0f;		//���Ⴊ�ݏI���̎��ԁB
	const float WARP_ANGLE_ADD = 180.0f;		//���[�v���ɉ��Z�����]�ʁB
}


GameCamera::GameCamera()
{

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

	g_camera3D->SetPosition(m_position);
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

/// <summary>
/// ���[�v�����B
/// </summary>
/// <param name="angle">��]��</param>
void GameCamera::SetWarp(const float angle)
{
	//�|�[�^���̊p�x�ɉ����ĉ�]��ύX�B
	m_rotSpeed.x += Math::DegToRad(WARP_ANGLE_ADD - angle);
	//m_rotSpeed.y += Math::DegToRad(-90.0f - angle);

	Rotation();
}