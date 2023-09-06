#include "stdafx.h"
#include "GameCamera.h"

namespace
{
	const float TARGET_FORWARD = 50000.0f;	//�����_�̑O�����B
	const float ROT_SPEED = 0.03f;			//��]���x�B
	const float CAMERA_HEIGHT = 75.0f;		//�J�����̍����B
	const float ANGLE_Y = 1.5f;			//�J�����̏㉺��]�̏���l
}


GameCamera::GameCamera()
{

}

GameCamera::~GameCamera()
{

}

void GameCamera::Init()
{
	g_camera3D->SetNear(1.0f);
	g_camera3D->SetFar(10000.0f);

	g_camera3D->SetTarget(Vector3(0.0f, 0.0, 1.0f) * TARGET_FORWARD);
	g_camera3D->Update();
}

void GameCamera::Update()
{
	g_camera3D->Update();
}

void GameCamera::SetPosition(const Vector3& pos)
{
	Vector3 cameraPos = pos;
	cameraPos.y += CAMERA_HEIGHT;

	g_camera3D->SetPosition(cameraPos);
}

void GameCamera::SetRotation(const Vector3& input)
{
	//�X�e�B�b�N�̓��͂����Z�B
	Vector3 rotSpeed = input;
	m_rotSpeed.x += rotSpeed.x * ROT_SPEED;
	m_rotSpeed.y += rotSpeed.y * ROT_SPEED;

	//�㉺��]�̌��E�l�͈͓̔��Ɏ��߂�B
	m_rotSpeed.y = min(max(m_rotSpeed.y, -ANGLE_Y), ANGLE_Y);

	//�����_����]������B
	m_targetPos.x = cos(m_rotSpeed.x) * cos(m_rotSpeed.y);
	m_targetPos.y = sin(m_rotSpeed.y);
	m_targetPos.z = sin(m_rotSpeed.x) * cos(m_rotSpeed.y);

	m_targetPos *= TARGET_FORWARD;

	g_camera3D->SetTarget(m_targetPos);
}