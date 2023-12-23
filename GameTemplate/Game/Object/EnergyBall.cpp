#include "stdafx.h"
#include "EnergyBall.h"
#include "EnergyBallGenerator.h"
#include "Player.h"
#include "Stage.h"
#include "PortalFrame.h"

namespace
{
	const int	DAMAGE_AMOUNT = 999;		//ダメージ量。
	const float COLLISION_RADIUS = 20.0f;	//コリジョンの半径。
	const float MOVE_SPEED = 200.0f;		//移動速度。
	const float MOVE_TIMER = 15.0f;			//表示時間。
}


EnergyBall::EnergyBall()
{

}

EnergyBall::~EnergyBall()
{

}

bool EnergyBall::Start()
{
	m_gimmickType = enGimmickType_Sender;

	m_player = FindGO<Player>("player");
	m_stage = FindGO<Stage>("stage");
	
	//コリジョンの設定。
	m_collisionObject = NewGO<CollisionObject>(0);
	m_collisionObject->CreateSphere(m_position, m_rotation, COLLISION_RADIUS);
	m_collisionObject->SetName("energyBall");

	//配列に自身を追加。
	m_stage->AddEnergyBallObject(this);

	//生成機がないなら
	if (m_energyBallGenerator == nullptr) {
		//生存時間を無限にする。
		m_timer = FLT_MIN;
	}

	return true;
}

void EnergyBall::Update()
{
	if (!m_isEnable) {
		return;
	}

	m_timer += g_gameTime->GetFrameDeltaTime();

	//時間が経過したら。
	if (m_timer > MOVE_TIMER) {
		Disable();
	}

	Move();

	Collision();
}

/// <summary>
/// 移動処理。
/// </summary>
void EnergyBall::Move()
{
	m_moveSpeed = Vector3::Zero;

	//移動先を計算する。
	m_moveSpeed = m_direction * MOVE_SPEED * g_gameTime->GetFrameDeltaTime();

	RayCast();

	m_ballEffect->SetPosition(m_position);
	m_ballEffect->Update();

	m_collisionObject->SetPosition(m_position);
	m_collisionObject->Update();
}

/// <summary>
/// レイを飛ばす処理。
/// </summary>
void EnergyBall::RayCast()
{
	//todo dbeug
	Vector3 nextPosition = m_position + m_moveSpeed;

	PhysicsWorld::RayHitObject hit;   

	//レイを飛ばす。
	const bool isHit = PhysicsWorld::GetInstance()->RayCastHit(m_position, nextPosition, &m_collisionObject->GetbtCollisionObject(), hit);

	//何も当たっていない。
	if (!isHit) {
		m_position = nextPosition;
		return;
	}

	m_direction = hit.normal;
}

/// <summary>
/// 当たり判定の処理。
/// </summary>
void EnergyBall::Collision()
{
	//プレイヤーと接触したら。
	if (m_collisionObject->IsHit(const_cast<CharacterController&>(*m_player->GetCharacterController()))) {
		//ダメージを与える。
		m_player->ReceiveDamage(DAMAGE_AMOUNT, Vector3::Zero);
	}

	////プレイヤーと接触したら。
	//if (m_collisionObject->IsHit(m_stage->GetPhysicsStaticObject())) {
	//	//ダメージを与える。
	//	m_player->ReceiveDamage(DAMAGE_AMOUNT, Vector3::Zero);
	//}
}

/// <summary>
/// 有効化。
/// </summary>
void EnergyBall::Enable()
{
	m_isEnable = true;

	//エフェクトを再生。
	m_ballEffect = NewGO<EffectEmitter>(0);
	m_ballEffect->Init(EffectEmitter::enEffect_EnergyBall);
	m_ballEffect->Play();

	//当たり判定を有効化。
	m_collisionObject->SetIsEnable(true);
}

/// <summary>
/// 無効化。
/// </summary>
void EnergyBall::Disable()
{
	m_isEnable = false;

	m_timer = 0.0f;

	//エフェクトを停止。
	m_ballEffect->Stop();
	DeleteGO(m_ballEffect);

	//当たり判定を無効化。
	m_collisionObject->SetIsEnable(false);

	if (m_energyBallGenerator != nullptr) {
		//生成機に非表示を通知。
		m_energyBallGenerator->NotifyDisable();
	}
}

/// <summary>
/// ワープ処理。
/// </summary>
/// <param name="pos">座標</param>
/// <param name="direction">方向</param>
void EnergyBall::SetWarp(const Vector3& pos, const Vector3& direction)
{
	m_position = pos;
	m_rotation.SetRotation(m_direction, direction);
	m_direction = direction;
}

void EnergyBall::Clear()
{
	Disable();

	//配列から自身を削除。
	m_stage->RemoveEnergyBallObject(this);

	if (m_energyBallGenerator != nullptr) {
		m_energyBallGenerator->Disable();
	}
}