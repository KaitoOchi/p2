#include "stdafx.h"
#include "Player.h"
#include "PortalGun.h"
#include "PortalFrame.h"
#include "GameCamera.h"
#include "Game.h"

namespace
{
	const int		MAX_HP = 100;									//最大HP。
	const float		CHARACON_RADIUS = 10.0f;						//キャラコンの半径。
	const float		CHARACON_HEIGHT = 60.0f;						//キャラコンの高さ。
	const float		WALK_SPEED = 120.0f;							//移動速度。
	const float		CROUCH_SPEED = 40.0f;							//しゃがみ速度。
	const float		JUMP_POWER = 140.0f;							//ジャンプ量。
	const float		GRAVITY = 10.0f;								//重力。
	const float		GRAVITY_ACCEL = 0.75f;							//重力加速。
	const float		INTERPOLATE_TIME = 0.2f;						//線形補間。
	const float		KNOCKBACK_POWER = 300.0f;						//ダメージを受けたときのノックバックパワー。
	const float		ADDMOVESPEED_MIN = 0.1f;						//追加の移動速度を終了する値。
	const float		ADDMOVESPEED_DIV = 5.0f;						//追加の移動速度を除算する値。
	const float		DAMAGE_TIME = 0.3f;								//ダメージを受ける時間。
	const float		DEAD_TIME = 2.0f;								//死亡時間。
}

Player::Player()
{
}

Player::~Player()
{
	DeleteGO(m_portalGun);
	DeleteGO(m_gameCamera);
}

bool Player::Start()
{
	//ゲームクラスの検索。
	m_game = FindGO<Game>("game");

	//アニメーションクリップ。
	m_animationClips[enState_Idle].Load("Assets/animData/UnityChan/idle.tka");
	m_animationClips[enState_Idle].SetLoopFlag(true);
	m_animationClips[enState_Walk].Load("Assets/animData/UnityChan/walk.tka");
	m_animationClips[enState_Walk].SetLoopFlag(true);
	m_animationClips[enState_Jump].Load("Assets/animData/UnityChan/jump.tka");
	m_animationClips[enState_Jump].SetLoopFlag(false);

	//モデルの設定。
	m_modelRender.Init(
		"Assets/modelData/unityChan.tkm",
		m_animationClips,
		enState_Num,
		enModelUpAxisY,
		true,
		true,
		ModelRender::enModelInitMode_UnDrawMainCamera
	);
	m_modelRender.Update();

	//ダメージ画像の設定。
	m_damageSpriteRender.Init("Assets/sprite/UI/damage.DDS", 1600.0f, 900.0f);
	m_damageSpriteRender.SetMulColor(Vector4(1.0f, 1.0f, 1.0f, m_damageTimer));
	m_damageSpriteRender.Update();

	//キャラクターコントローラーの設定。
	m_characterController.Init(CHARACON_RADIUS, CHARACON_HEIGHT, m_position);

	//ポータルガンの設定。
	m_portalGun = NewGO<PortalGun>(3, "portalGun");
	//カメラの設定。
	m_gameCamera = NewGO<GameCamera>(2, "gameCamera");

	//文字の設定。
	a.SetPosition(Vector3(400.0f, 0.0f, 0.0f));
	a.SetShadowParam(true, 0.5f, Vector4::Black);

	//リセット処理。
	Reset();

	return true;
}

void Player::Update()
{
	//プレイヤーが死亡していないなら。
	if (m_playerState != enState_Dead) {

		Input();

		PlayAnimation();

		m_modelRender.Update();
	}

	State();

	////////////// todo debug
	Vector3 vec = g_camera3D->GetForward();
	wchar_t text[256];
	swprintf_s(text, 256,
		L"PlayerX:%.2f \nPlayerY:%.2f \nPlayerZ:%.2f \nForwardX:%.2f \nForwardY:%.2f \nForwardZ:%.2f \nAnim:%d",
		m_position.x,
		m_position.y,
		m_position.z,
		vec.x,
		vec.y,
		vec.z,
		m_playerState
	);
	a.SetText(text);
	/////////////

	if (m_damageTimer > 0.0f) {
		m_damageTimer -= g_gameTime->GetFrameDeltaTime();
		m_damageSpriteRender.SetMulColor(Vector4(1.0f, 1.0f, 1.0f, m_damageTimer));
		m_damageSpriteRender.Update();
	}
}

/// <summary>
/// 入力処理。
/// </summary>
void Player::Input()
{
	//移動速度を初期化。
	m_moveSpeed = Vector3::Zero;

	MoveXZ();

	MoveY();

	//追加の移動速度を加算。
	m_moveSpeed += m_addMoveSpeed;
	//キャラコンの実行。
	m_position = m_characterController.Execute(m_moveSpeed, 1.0f / 60.0f);
	m_modelRender.SetPosition(m_position);

	Rotation();

	if (m_addMoveSpeed.x < ADDMOVESPEED_MIN &&
		m_addMoveSpeed.y < ADDMOVESPEED_MIN &&
		m_addMoveSpeed.z < ADDMOVESPEED_MIN
	){
		//追加の移動速度を初期化。
		m_addMoveSpeed = Vector3::Zero;
	}
	else {
		//追加の移動速度を減算。
		m_addMoveSpeed -= m_addMoveSpeed / ADDMOVESPEED_DIV;
	}
}

/// <summary>
/// XZ方向の移動処理。
/// </summary>
void Player::MoveXZ()
{
	//左スティックの入力を取得。
	Vector3 input;
	input.x = g_pad[0]->GetLStickXF();
	input.z = g_pad[0]->GetLStickYF();

	//入力がないなら、処理しない。
	if (fabsf(input.x) < 0.01f &&
		fabsf(input.z) < 0.01f
	) {
		return;
	}

	//カメラの前方向を取得。
	Vector3 cameraForward = g_camera3D->GetForward();
	cameraForward.y = 0.0f;
	cameraForward.Normalize();

	//カメラの右方向を取得。
	Vector3 cameraRight = g_camera3D->GetRight();
	cameraRight.y = 0.0f;
	cameraRight.Normalize();

	//移動速度にカメラの方向を考慮して加算。
	m_moveSpeed += cameraForward * input.z;
	m_moveSpeed += cameraRight * input.x;

	//正規化をする。
	m_moveSpeed.Normalize();

	//ジャンプ中でないなら、
	if (m_playerState != enState_Jump &&
		m_playerState != enState_Crouch_Jump) {

		//しゃがみ歩き。
		if (m_playerState == enState_Crouch_Walk) {
			m_walkSpeed = CROUCH_SPEED;
		}
		else {
			m_walkSpeed = WALK_SPEED;
		}
	}

	//移動速度をステートに応じて乗算。
	m_moveSpeed *= m_walkSpeed;
}

/// <summary>
/// Y方向の移動処理。
/// </summary>
void Player::MoveY()
{
	//ジャンプ中なら。
	if (m_playerState == enState_Jump ||
		m_playerState == enState_Crouch_Jump) {
		//移動速度を加算。
		m_moveSpeed.y += JUMP_POWER;
	}

	//地面にいないなら。
	if (!m_characterController.IsOnGround()) {
		//重力を発生。
		m_moveSpeed.y -= GRAVITY * m_gravityAccel;
		m_gravityAccel += GRAVITY_ACCEL;
	}
}

/// <summary>
/// 回転処理。
/// </summary>
void Player::Rotation()
{
	//カメラの前方向に回転する。
	m_rotation.SetRotationYFromDirectionXZ(g_camera3D->GetForward());
	m_modelRender.SetRotation(m_rotation);
}

/// <summary>
/// ワープ処理。
/// </summary>
/// <param name="pos">座標</param>
/// <param name="angle">回転量</param>
void Player::SetWarp(const Vector3& pos, const float angle)
{
	//座標を設定。
	m_position = pos;
	m_characterController.SetPosition(m_position);
	//カメラもワープさせる。
	m_gameCamera->SetWarp(angle);
}

/// <summary>
/// アニメーション処理。
/// </summary>
void Player::PlayAnimation()
{
	//アニメーションを再生。
	switch (m_playerState)
	{
	case enState_Idle:
		m_modelRender.PlayAnimation(enState_Idle, INTERPOLATE_TIME);
		break;

	case enState_Walk:
		m_modelRender.PlayAnimation(enState_Walk, INTERPOLATE_TIME);
		break;

	case enState_Crouch_Idle:

		break;

	case enState_Jump:
		m_modelRender.PlayAnimation(enState_Jump, INTERPOLATE_TIME);
		break;
	}
}

/// <summary>
/// ステート処理。
/// </summary>
void Player::State()
{
	switch (m_playerState)
	{
	case enState_Idle:
		ProcessIdleStateTransition();
		break;

	case enState_Walk:
		ProcessWalkStateTransition();
		break;

	case enState_Crouch_Idle:
		ProcessCrouchIdleStateTransition();
		break;

	case enState_Crouch_Walk:
		ProcessCrouchWalkStateTransition();
		break;

	case enState_Crouch_Jump:
		ProcessCrouchJumpStateTransition();
		break;

	case enState_Jump:
		ProcessJumpStateTransition();
		break;

	case enState_Dead:
		ProcessDeadStateTransition();
	}
}

/// <summary>
/// 通常の遷移処理。
/// </summary>
void Player::ProcessCommonStateTransition()
{
	//カメラの座標を設定。
	if (m_playerState == enState_Crouch_Idle ||
		m_playerState == enState_Crouch_Walk) {
		m_gameCamera->SetCrouchState(true);
	}
	else {
		m_gameCamera->SetCrouchState(false);
	}

	//しゃがみボタンが押されたら。
	if (g_pad[0]->IsPress(enButtonY)) {

		//ジャンプボタンが押されたら。
		if (g_pad[0]->IsTrigger(enButtonA)) {
			m_playerState = enState_Crouch_Jump;
			return;
		}

		//移動速度があるなら。
		if (fabsf(m_moveSpeed.x) > 0.001f || fabsf(m_moveSpeed.z) > 0.001f) {
			m_playerState = enState_Crouch_Walk;
			return;
		}

		m_playerState = enState_Crouch_Idle;
		return;
	}

	//ジャンプボタンが押されたら。
	if (g_pad[0]->IsTrigger(enButtonA)) {
		m_playerState = enState_Jump;
		return;
	}

	//移動速度があるなら。
	if (fabsf(m_moveSpeed.x) > 0.001f || fabsf(m_moveSpeed.z) > 0.001f) {
		m_playerState = enState_Walk;
		return;
	}

	m_playerState = enState_Idle;
}

/// <summary>
/// 待機状態の遷移処理。
/// </summary>
void Player::ProcessIdleStateTransition()
{
	ProcessCommonStateTransition();
}

/// <summary>
/// 歩き状態の遷移処理。
/// </summary>
void Player::ProcessWalkStateTransition()
{
	ProcessCommonStateTransition();
}

/// <summary>
/// しゃがみ待機状態の遷移処理。
/// </summary>
void Player::ProcessCrouchIdleStateTransition()
{
	ProcessCommonStateTransition();
}

/// <summary>
/// しゃがみ歩き状態の遷移処理。
/// </summary>
void Player::ProcessCrouchWalkStateTransition()
{
	ProcessCommonStateTransition();
}

/// <summary>
/// しゃがみジャンプ状態の遷移処理。
/// </summary>
void Player::ProcessCrouchJumpStateTransition()
{
	//地面についたら。
	if (m_characterController.IsOnGround()) {

		//重力加速度を初期化。
		m_gravityAccel = 0.0f;

		ProcessCommonStateTransition();
	}
}

/// <summary>
/// ジャンプ状態の遷移処理。
/// </summary>
void Player::ProcessJumpStateTransition()
{
	//地面についたら。
	if (m_characterController.IsOnGround()) {

		//重力加速度を初期化。
		m_gravityAccel = 0.0f;

		ProcessCommonStateTransition();
	}
}

/// <summary>
/// 死亡状態の遷移処理。
/// </summary>
void Player::ProcessDeadStateTransition()
{
	m_deadTimer += g_gameTime->GetFrameDeltaTime();

	//死亡時間が経過したら。
	if (m_deadTimer > DEAD_TIME) {
		Reset();
		return;
	}
}

/// <summary>
/// ダメージを受ける処理。
/// </summary>
/// <param name="damage">与えるダメージ量</param>
/// <param name="dir">のけぞる方向</param>
void Player::ReceiveDamage(const int damage, const Vector3& dir)
{
	if (m_damageTimer > 0.0f || 
		m_playerState == enState_Dead
	) {
		return;
	}

	//ダメージを減らす。
	m_hp -= damage;

	//ダメージが0になったら。
	if (m_hp < 0) {
		//死亡状態にする。
		m_playerState = enState_Dead;
		m_gameCamera->Dead();
	}

	m_damageTimer = DAMAGE_TIME;

	//後ろにのけぞる。
	Vector3 knockback = dir * KNOCKBACK_POWER;
	knockback.y = 0.0f;
	m_addMoveSpeed += knockback;
}

/// <summary>
/// リセット処理。
/// </summary>
void Player::Reset()
{
	//値を初期化。
	m_hp = MAX_HP;
	m_walkSpeed = 0.0f;
	m_gravityAccel = 0.0f;
	m_damageTimer = 0.0f;
	m_deadTimer = 0.0f;
	m_playerState = enState_Idle;
	m_addMoveSpeed = Vector3::Zero;

	//ゲームにリセット状態を通知。
	m_game->NotifyReset();

	//カメラをリセット。
	m_gameCamera->Reset();

	//座標を初期化。
	m_position = m_game->GetRespawnPoint();
	m_characterController.SetPosition(m_position);
}

void Player::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
	m_damageSpriteRender.Draw(rc);
	a.Draw(rc);
}