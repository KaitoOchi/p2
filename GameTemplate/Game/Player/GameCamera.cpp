#include "stdafx.h"
#include "GameCamera.h"

namespace
{
	const float CAMERA_NEAR = 1.0f;				//近平面。
	const float CAMERA_FAR = 10000.0f;			//遠平面。
	const float TARGET_FORWARD = 2500.0f;		//注視点の前方向。
	const float ROT_SPEED = 0.03f;				//回転速度。
	const float CAMERA_HEIGHT = 75.0f;			//カメラの高さ。
	const float CAMERA_HEIGHT_CROUCH = 20.0f;	//しゃがみ状態の高さ。
	const float ANGLE_Y = 1.5f;					//カメラの上下回転の上限値
	const float CROUCH_SPEED = 5.0f;			//しゃがみ速度。
}


GameCamera::GameCamera()
{

}

GameCamera::~GameCamera()
{

}

void GameCamera::Init()
{
	//近平面と遠平面を設定。
	g_camera3D->SetNear(CAMERA_NEAR);
	g_camera3D->SetFar(CAMERA_FAR);

	SetPosition(m_position);
	Update();
}

void GameCamera::Update()
{
	g_camera3D->Update();
}

void GameCamera::SetPosition(const Vector3& pos)
{
	//カメラの高さを設定。
	m_position = pos;

	//しゃがみの座標を設定。
	if (m_crouchTimer >= 0.0f && m_crouchTimer <= 1.0f) {
		//しゃがみの始点を設定。
		Vector3 cameraPos = m_position;
		cameraPos.y += CAMERA_HEIGHT;
		m_firstCrouchPos = cameraPos;

		//しゃがみの終点を設定。
		cameraPos = m_position;
		cameraPos.y += CAMERA_HEIGHT_CROUCH;
		m_lastCrouchPos = cameraPos;
	}

	//しゃがみ状態なら。
	if (m_isCrouch) {

		if (m_crouchTimer < 1.0f) {
			//線形補間でしゃがみを開始する。
			m_crouchTimer += g_gameTime->GetFrameDeltaTime() * CROUCH_SPEED;
			m_position.Lerp(m_crouchTimer, m_firstCrouchPos, m_lastCrouchPos);
		}
		else {
			m_crouchTimer = 1.0f;
			m_position.y += CAMERA_HEIGHT_CROUCH;
		}
	}
	//しゃがみ状態でないなら。
	else {

		if (m_crouchTimer > 0.0f) {
			//線形補間でしゃがみを解除する。
			m_crouchTimer -= g_gameTime->GetFrameDeltaTime() * CROUCH_SPEED;
			m_position.Lerp(m_crouchTimer, m_firstCrouchPos, m_lastCrouchPos);
		}
		else {
			m_crouchTimer = 0.0f;
			m_position.y += CAMERA_HEIGHT;
		}
	}

	g_camera3D->SetPosition(m_position);
}

void GameCamera::SetRotation()
{
	//右スティックの入力を取得。
	Vector3 input;
	input.x = -g_pad[0]->GetRStickXF();
	input.y = g_pad[0]->GetRStickYF();

	//スティックの入力を加算。
	m_rotSpeed.x += input.x * ROT_SPEED;
	m_rotSpeed.y += input.y * ROT_SPEED;

	//上下回転の限界値を範囲内に収める。
	m_rotSpeed.y = min(max(m_rotSpeed.y, -ANGLE_Y), ANGLE_Y);

	//注視点を回転させる。
	m_targetPos.x = cos(m_rotSpeed.x) * cos(m_rotSpeed.y);
	m_targetPos.y = sin(m_rotSpeed.y);
	m_targetPos.z = sin(m_rotSpeed.x) * cos(m_rotSpeed.y);

	//注視点を設定。
	m_targetPos.Normalize();
	m_targetPos *= TARGET_FORWARD;
	m_targetPos += m_position;

	g_camera3D->SetTarget(m_targetPos);
}