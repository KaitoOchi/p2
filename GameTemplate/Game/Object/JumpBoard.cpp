#include "stdafx.h"
#include "JumpBoard.h"
#include "Player.h"

namespace
{
	const Vector3	COLLISION_SIZE = Vector3(40.0f, 10.0f, 80.0f);	//コリジョンサイズ。
	const float		MIDDLE_POSITION_HEIGHT = 800.0f;				//中間地点の高さ。
	const float		TIMER_SPEED = 0.5f;								//時間の速度。
	const float		JUMP_POWER = 65.0f;								//ジャンプ量。
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

	//モデルを設定。
	m_modelRender.Init("Assets/modelData/object/jumpBoard/jumpBoard.tkm");
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(m_rotation);
	m_modelRender.Update();

	//コリジョンを設定。
	m_collisionObject = NewGO<CollisionObject>(0);
	m_collisionObject->CreateBox(m_position, m_rotation, COLLISION_SIZE);
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
	if (m_player == nullptr) {
		return;
	}

	if (m_collisionObject->IsHit(const_cast<CharacterController&>(*m_player->GetCharacterController()))) {
		m_isJump = true;
		m_moveSpeed = m_position;
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

	//ベジェ曲線を用いて放物線状に移動する。
	m_bezierPos[0].Lerp(m_timer, m_position, m_middlePos);
	m_bezierPos[1].Lerp(m_timer, m_middlePos, m_landingPos);
	m_bezierPos[2].Lerp(m_timer, m_bezierPos[0], m_bezierPos[1]);

	m_moveSpeed = m_bezierPos[2] - m_moveSpeed;
	m_moveSpeed.Scale(JUMP_POWER);

	m_player->SetAddMoveSpeed(m_moveSpeed);
	m_moveSpeed = m_bezierPos[2];
}

void JumpBoard::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}
