#include "stdafx.h"
#include "GameCamera.h"
#include "Player.h"

namespace
{
	const float CAMERA_NEAR = 1.0f;				//近平面。
	const float CAMERA_FAR = 10000.0f;			//遠平面。
	const float TARGET_FORWARD = 250.0f;		//注視点の前方向。
	const float ROT_SPEED = 0.03f;				//回転速度。
	const float CAMERA_HEIGHT = 75.0f;			//カメラの高さ。
	const float CAMERA_HEIGHT_CROUCH = 20.0f;	//しゃがみ状態の高さ。
	const float ANGLE_Y = 1.2f;					//カメラの上下回転の上限値
	const float CROUCH_SPEED = 5.0f;			//しゃがみ速度。
	const float CROUCH_TIMER_START = 0.0f;		//しゃがみ開始の時間。
	const float CROUCH_TIMER_END = 1.0f;		//しゃがみ終了の時間。
}


GameCamera::GameCamera()
{

}

GameCamera::~GameCamera()
{

}

bool GameCamera::Start()
{
	//プレイヤーオブジェクトを検索。
	m_player = FindGO<Player>("player");

	//近平面と遠平面を設定。
	g_camera3D->SetNear(CAMERA_NEAR);
	g_camera3D->SetFar(CAMERA_FAR);

	return true;
}

void GameCamera::Update()
{
	if (m_isDead) {
		return;
	}

	//プレイヤーが死亡したら。
	if (m_player->GetPlayerState() == Player::enState_Dead) {
		m_isCrouch = false;
		m_isDead = true;
		g_camera3D->Move(Vector3(0.0f, -50.0f, 0.0f));
		g_camera3D->SetUp(Vector3(-1.0f, 0.0f, 0.0f));
		g_camera3D->Update();
		return;
	}

	Move();

	Rotation();

	g_camera3D->Update();
}

void GameCamera::Move()
{
	//プレイヤーの座標を取得。
	m_position = m_player->GetPosition();

	//しゃがみの座標を設定。
	if (m_crouchTimer >= CROUCH_TIMER_START && m_crouchTimer <= CROUCH_TIMER_END) {
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
		//しゃがんでいる時間が経過するまで。
		if (m_crouchTimer < CROUCH_TIMER_END) {
			//線形補間でしゃがみを開始する。
			m_crouchTimer += g_gameTime->GetFrameDeltaTime() * CROUCH_SPEED;
			m_position.Lerp(m_crouchTimer, m_firstCrouchPos, m_lastCrouchPos);
		}
		else {
			m_crouchTimer = CROUCH_TIMER_END;
			m_position.y += CAMERA_HEIGHT_CROUCH;
		}
	}
	//しゃがみ状態でないなら。
	else {
		//しゃがみ終了の時間まで。
		if (m_crouchTimer > CROUCH_TIMER_START) {
			//線形補間でしゃがみを解除する。
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

void GameCamera::Rotation()
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

	//注視点を正規化する。
	m_targetPos.Normalize();
	//注視点を前方向に移動させる。
	m_targetPos *= TARGET_FORWARD;
	//注視点をプレイヤーの方向に移動させる。
	m_targetPos += m_position;

	g_camera3D->SetTarget(m_targetPos);
}

void GameCamera::SetWarp(const Quaternion& rot)
{
	//クォータニオンをオイラー角に変換。
	Vector3 euler = CalcQuaternionToEuler(rot);
	//float y = Math::DegToRad(euler.y + 180.0f);
	float y = Math::DegToRad(euler.y + 180.0f);

	m_rotSpeed.x += y;

	Rotation();
}