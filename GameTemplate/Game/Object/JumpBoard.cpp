#include "stdafx.h"
#include "JumpBoard.h"
#include "Player.h"

namespace
{
	const Vector3	COLLISION_SIZE = Vector3(40.0f, 10.0f, 80.0f);	//コリジョンサイズ。
	const float		MIDDLE_POSITION_HEIGHT = 400.0f;				//中間地点の高さ。
	const float		TIMER_SPEED = 0.5f;								//時間の速度。
}

JumpBoard::JumpBoard()
{

}

JumpBoard::~JumpBoard()
{

}

bool JumpBoard::Start()
{
	m_player = FindGO<Player>("player");

	//中間地点を設定。
	m_middlePos = ((m_landingPos - m_position) / 2.0f) + m_position;
	m_middlePos.y += MIDDLE_POSITION_HEIGHT;

	//回転を設定。
	Quaternion rot;
	rot.SetRotation(Vector3::AxisZ, m_position);

	//モデルを設定。
	m_modelRender.Init("Assets/modelData/object/jumpBoard/jumpBoard.tkm");
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(rot);
	m_modelRender.Update();

	//コリジョンを設定。
	m_collisionObject = NewGO<CollisionObject>(0);
	m_collisionObject->CreateBox(m_position, rot, COLLISION_SIZE);
	m_collisionObject->SetName("JumpBoard");

	return true;
}

void JumpBoard::Update()
{
	if (m_isJump) {
		Jump();
	}
	else {
		Collision();
	}
}

/// <summary>
/// 当たり判定処理。
/// </summary>
void JumpBoard::Collision()
{
	if (m_collisionObject->IsHit(const_cast<CharacterController&>(*m_player->GetCharacterController()))) {
		m_isJump = true;
	}
}

/// <summary>
/// ジャンプ処理。
/// </summary>
void JumpBoard::Jump()
{
	if (m_timer > 1.0f) {
		m_isJump = false;
		m_timer = 0.0f;
		memset(m_bezierPos, 0, sizeof(m_bezierPos));
		return;
	}

	m_timer += g_gameTime->GetFrameDeltaTime() * TIMER_SPEED;

	Vector3 moveSpeed = m_bezierPos[2];

	//ベジェ曲線を用いて放物線状に移動する。
	m_bezierPos[0].Lerp(m_timer, m_position, m_middlePos);
	m_bezierPos[1].Lerp(m_timer, m_middlePos, m_landingPos);
	m_bezierPos[2].Lerp(m_timer, m_bezierPos[0], m_bezierPos[1]);

	moveSpeed = m_bezierPos[2] - moveSpeed;
	moveSpeed.Scale(50.0f);

	m_player->SetAddMoveSpeed(moveSpeed);
}

void JumpBoard::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}
