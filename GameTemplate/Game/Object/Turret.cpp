#include "stdafx.h"
#include "Turret.h"
#include "Player.h"

namespace
{
	const Vector3	TURRET_HEIGHT = Vector3(0.0f, 50.0f, 0.0f);		//タレットの高さ。
	const int		DAMAGE_AMOUNT = 10;								//ダメージ量。
	const float		RAY_LENGTH = 500.0f;							//レイの長さ。
	const float		DAMAGE_COOLDOWN = 0.5f;							//ダメージの硬直時間。
}

Turret::Turret()
{
	m_forward.z = 1.0f;
}

Turret::~Turret()
{

}

bool Turret::Start()
{
	//モデルの初期化。
	m_modelRender.Init("Assets/modelData/unityChan.tkm", 0, 0, enModelUpAxisZ, true, true);
	m_modelRender.SetPosition(m_position);
	m_modelRender.Update();

	//プレイヤーの検索。
	m_player = FindGO<Player>("player");

	return true;
}

void Turret::Update()
{
	//ダメージの硬直時間があるなら。
	if (m_damageTimer > 0.0f) {
		m_damageTimer -= g_gameTime->GetFrameDeltaTime();
		return;
	}

	PhysicsWorld::RayHitObject hit;
	Vector3 endPos = m_position + (m_forward * RAY_LENGTH) + TURRET_HEIGHT;
	bool isHit = PhysicsWorld::GetInstance()->RayTest(m_position, endPos, hit);

	//衝突したオブジェクトがプレイヤーなら。
	if (hit.collision == m_player->GetCollision()) {
		//のけぞる方向。
		Vector3 knockback = m_forward;
		//ダメージを与える。
		m_player->ReceiveDamage(DAMAGE_AMOUNT, knockback);
		//ダメージにクールダウンを設定。
		m_damageTimer = DAMAGE_COOLDOWN;
	}
}

void Turret::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}