#include "stdafx.h"
#include "GameCamera.h"

namespace
{
	const float TARGET_FORWARD = 10.0f;		//注視点の前方向。
	const float SPHERE_RADIUS = 1000.0f;	//球の半径。
	const float ROT_SPEED = 0.03f;			//回転速度。
	const float CAMERA_HEIGHT = 75.0f;		//カメラの高さ。
}


GameCamera::GameCamera()
{
	m_rotSpeed.x = -1.65f;
	m_rotSpeed.z = 1.65f;

	g_camera3D->SetTarget(g_camera3D->GetForward() * TARGET_FORWARD);
	g_camera3D->Update();
}

GameCamera::~GameCamera()
{

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

void GameCamera::SetRotation()
{
	//右スティックの入力を取得。
	Vector3 input;
	input.x = -g_pad[0]->GetRStickXF();
	input.z = g_pad[0]->GetRStickYF();

	//スティックの入力を加算。
	m_rotSpeed.x += input.x * ROT_SPEED;
	m_rotSpeed.z += input.z * ROT_SPEED;

	//注視点を回転させる。
	m_targetPos.x = cos(m_rotSpeed.x) * SPHERE_RADIUS;
	m_targetPos.z = sin(m_rotSpeed.x) * SPHERE_RADIUS;

	g_camera3D->SetTarget(m_targetPos);
}