#include "stdafx.h"
#include "Turret.h"
#include "Player.h"

namespace
{
	const Vector3	TURRET_HEIGHT = Vector3(0.0f, 50.0f, 0.0f);				//タレットの高さ。
	const int		DAMAGE_AMOUNT = 10;										//ダメージ量。
	const int		SHOOTING_RANGE = 10;									//発射時のばらつき。
	const float		SEARCH_LENGTH = pow(600.0f, 2.0f);						//索敵する距離。
	const float		TURRET_COLLISION_FORWARD = 10.0f;						//コリジョンの前方向。
	const float		SEARCH_COOLDOWN = 1.0f;									//発見時のクールダウン。
	const float		DAMAGE_COOLDOWN = 0.1f;									//射撃時の硬直時間。
	const float		MISSING_COOLDOWN = 3.0f;								//見失う際の経過時間。
	const float		TURRET_POV = (30.0f / 100.0f) * Math::PI;					//タレットの視野角。
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
	m_modelRender.Init("Assets/modelData/unityChan.tkm", 0, 0, enModelUpAxisZ, true, true, ModelRender::enModelInitMode_Dithering);
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(m_rotation);
	m_modelRender.Update();

	//コリジョン用のモデルを初期化。
	ModelInitData modelInitData;
	modelInitData.m_tkmFilePath = "Assets/modelData/object/turret/turret_collision.tkm";
	Model turretModel;
	turretModel.Init(modelInitData);

	//プレイヤーの検索。
	m_player = FindGO<Player>("player");

	return true;
}

void Turret::Update()
{
	SearchPlayer();

	State();

	m_modelRender.SetRotation(m_rotation);
	m_modelRender.Update();

}

/// <summary>
/// タレットの索敵処理。
/// </summary>
void Turret::SearchPlayer()
{
	//プレイヤーからタレットに伸びるベクトルを求める。
	Vector3 diff = m_player->GetPosition() - m_position;

	//プレイヤーとの距離が離れていたら。
	if (diff.LengthSq() >= SEARCH_LENGTH) {
		m_isHit = false;
		return;
	}

	diff.Normalize();
	//角度を計算。
	float angle = acosf(diff.Dot(m_forward));

	//プレイヤーが視界内にいなかったら。
	if (TURRET_POV <= fabsf(angle)) {
		m_isHit = false;
		return;
	}

	//壁と衝突しているなら。
	if (WallAndHit()) {
		m_isHit = false;
		return;
	}

	//当たっている判定。
	m_isHit = true;
}

/// <summary>
/// 壁との衝突処理。
/// </summary>
/// <returns></returns>
bool Turret::WallAndHit()
{
	Vector3 playerPos = m_player->GetPosition();
	playerPos.y += 75.0f;
	Vector3 turretPos = m_position;
	turretPos.y += 75.0f;

	PhysicsWorld::RayHitObject hit;

	//レイを飛ばす。
	bool isHit = PhysicsWorld::GetInstance()->RayTest(playerPos, turretPos, hit);

	//何も当たっていない。
	if (!isHit) {
		return false;
	}

	//プレイヤーと衝突しているなら。
	if (hit.collision == m_player->GetCollision()) {
		return false;
	}

	//壁と衝突している。
	return true;
}

/// <summary>
/// ステート処理。
/// </summary>
void Turret::State()
{
	switch (m_state)
	{
	case enTurretState_Idle:
		ProcessIdleStateTransition();
		break;

	case enTurretState_Search:
		ProcessSearchStateTransition();
		break;

	case enTurretState_Shot:
		ProcessShotStateTransition();
		break;

	case enTurretState_Missing:
		ProcessMissingStateTransition();
		break;

	case enTurretState_Grab:
		break;

	case enTurretState_Dead:
		break;
	}
}

void Turret::ProcessIdleStateTransition()
{
	//プレイヤーと当たり判定が衝突しているなら。
	if (m_isHit) {
		//発見ステートへ遷移する。
		m_state = enTurretState_Search;
		//クールダウンを設定。
		m_progressTimer = SEARCH_COOLDOWN;
	}
}

void Turret::ProcessSearchStateTransition()
{
	//発見してから時間が経過したら。
	if (m_progressTimer < 0.0f) {
		//射撃ステートへ遷移する。
		m_state = enTurretState_Shot;
		//クールダウンを設定。
		m_progressTimer = DAMAGE_COOLDOWN;
		//プレイヤーの座標を保存。
		m_oldPlayerPos = m_player->GetPosition();
		return;
	}

	m_progressTimer -= g_gameTime->GetFrameDeltaTime();

	//プレイヤーと当たり判定が衝突していないなら。
	if (!m_isHit) {
		//見失うステートへ遷移する。
		m_state = enTurretState_Missing;
		//クールダウンを設定。
		m_progressTimer = MISSING_COOLDOWN;
	}
}

void Turret::ProcessShotStateTransition()
{
	//硬直時間があるなら。
	if (m_progressTimer > 0.0f) {
		m_progressTimer -= g_gameTime->GetFrameDeltaTime();
		return;
	}

	//ばらつきを与えるため座標を少しずらす。
	float random = Math::Random(-SHOOTING_RANGE, SHOOTING_RANGE);
	m_oldPlayerPos.x += random;
	m_oldPlayerPos.z += random;

	PhysicsWorld::RayHitObject hit;
	//始点を設定。
	Vector3 startPos = m_position + TURRET_HEIGHT;
	//終点を設定。
	Vector3 endPos = m_oldPlayerPos + ((m_oldPlayerPos - startPos) * TURRET_COLLISION_FORWARD);
	//レイを飛ばす。
	bool isHit = PhysicsWorld::GetInstance()->RayTest(startPos, endPos, hit);

	//衝突したオブジェクトがプレイヤーなら。
	if (hit.collision == m_player->GetCollision()) {
		//のけぞる方向。
		Vector3 knockback = endPos - startPos;
		knockback.Normalize();
		//ダメージを与える。
		m_player->ReceiveDamage(DAMAGE_AMOUNT, knockback);

		//プレイヤーが死亡したら。
		if (m_player->GetPlayerState() == Player::enState_Dead) {
			m_state = enTurretState_Idle;
			m_progressTimer = 0.0f;
		}
	}

	//射撃エフェクトの回転量を設定。
	float rotX = -(m_position.y - m_position.y) + random;
	Quaternion rot;
	rot.SetRotationYFromDirectionXZ(endPos);
	rot.AddRotationDegX(rotX);

	//射撃エフェクトを再生。
	EffectEmitter* shotEffect = NewGO<EffectEmitter>(0);
	shotEffect->Init(EffectEmitter::enEffect_Turret_Shot);
	shotEffect->SetPosition(startPos);
	shotEffect->SetRotation(rot);
	shotEffect->Play();

	//クールダウンを設定。
	m_progressTimer = DAMAGE_COOLDOWN;
	//プレイヤーの座標を保存。
	m_oldPlayerPos = g_camera3D->GetPosition();

	//プレイヤーと当たり判定が衝突していないなら。
	if (!m_isHit) {
		//見失うステートへ遷移する。
		m_state = enTurretState_Missing;
		m_progressTimer = MISSING_COOLDOWN;
	}
}

void Turret::ProcessMissingStateTransition()
{
	//見失ってから時間が経過したら。
	if (m_progressTimer < 0.0f) {
		//待機ステートへ遷移する。
		m_state = enTurretState_Idle;
		return;
	}

	m_progressTimer -= g_gameTime->GetFrameDeltaTime();

	//プレイヤーと当たり判定が衝突しているなら。
	if (m_isHit) {
		//射撃ステートへ遷移する。
		m_state = enTurretState_Shot;
		//クールダウンを設定。
		m_progressTimer = DAMAGE_COOLDOWN;
		//プレイヤーの座標を保存。
		m_oldPlayerPos = m_player->GetPosition();
	}
}

void Turret::ProcessGrabStateTransition()
{

}

void Turret::ProcessDeadStateTransition()
{

}

void Turret::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);

	a.Draw(rc);
}