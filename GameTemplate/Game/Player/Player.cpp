#include "stdafx.h"
#include "Player.h"
#include "PortalGun.h"
#include "PortalFrame.h"
#include "GameCamera.h"
#include "Game.h"
#include "Fade.h"

namespace
{
	const Vector3	MODEL_SCALE = Vector3(1.25f, 1.25f, 1.25f);		//モデルの拡大率。
	const int		MAX_HP = 100;									//最大HP。
	const float		CHARACON_RADIUS = 10.0f;						//キャラコンの半径。
	const float		CHARACON_HEIGHT = 60.0f;						//キャラコンの高さ。
	const float		WALK_SPEED = 180.0f;							//移動速度。
	const float		CROUCH_SPEED = 40.0f;							//しゃがみ速度。
	const float		JUMP_POWER = 140.0f;							//ジャンプ量。
	const float		GRAVITY = 10.0f;								//重力。
	const float		GRAVITY_ACCEL = 0.75f;							//重力加速。
	const float		INTERPOLATE_TIME = 0.2f;						//線形補間。
	const float		KNOCKBACK_POWER = 300.0f;						//ダメージを受けたときのノックバックパワー。
	const float		ADDMOVESPEED_MIN = 1.0f;						//追加の移動速度を終了する値。
	const float		ADDMOVESPEED_DIV = 5.0f;						//追加の移動速度を除算する値。
	const float		DAMAGE_TIME = 0.3f;								//ダメージを受ける時間。
}

Player::Player()
{
}

Player::~Player()
{
}

bool Player::Start()
{
	//オブジェクトの検索。
	m_game = FindGO<Game>("game");
	m_gameCamera = FindGO<GameCamera>("gameCamera");

	//アニメーションクリップ。
	m_animationClips[enState_Idle].Load("Assets/animData/player/idle.tka");
	m_animationClips[enState_Idle].SetLoopFlag(true);
	m_animationClips[enState_Walk].Load("Assets/animData/player/walk.tka");
	m_animationClips[enState_Walk].SetLoopFlag(true);
	m_animationClips[enState_Jump].Load("Assets/animData/player/jump_start.tka");
	m_animationClips[enState_Jump].SetLoopFlag(false);
	m_animationClips[enState_JumpEnd].Load("Assets/animData/player/jump_end.tka");
	m_animationClips[enState_JumpEnd].SetLoopFlag(false);

	//モデルの設定。
	m_modelRender.Init(
		"Assets/modelData/player.tkm",
		m_animationClips,
		enState_Num,
		enModelUpAxisZ,
		true,
		true,
		ModelRender::enModelInitMode_UnDrawMainCamera
	);
	m_modelRender.SetScale(MODEL_SCALE);
	m_modelRender.Update();
	m_modelRender.AddAnimationEvent([&](const wchar_t* clipName, const wchar_t* eventName)
		{
			OnAnimationEvent(clipName, eventName);
		});

	//ダメージ画像の設定。
	m_damageSpriteRender.Init("Assets/sprite/UI/damage.DDS", 1600.0f, 900.0f);
	m_damageSpriteRender.SetMulColor(Vector4(1.0f, 1.0f, 1.0f, m_damageTimer));
	m_damageSpriteRender.Update();

	//キャラクターコントローラーの設定。
	m_characterController = new CharacterController;
	m_characterController->Init(CHARACON_RADIUS, CHARACON_HEIGHT, m_position);

	//文字の設定。
	a.SetPosition(Vector3(400.0f, 0.0f, 0.0f));
	a.SetShadowParam(true, 0.5f, Vector4::Black);

	return true;
}

void Player::Update()
{
	if (m_playerState == enState_End ||
		m_playerState == enState_Dead
	) {
		return;
	}

	Input();

	PlayAnimation();

	m_modelRender.Update();

	Vector3 diff = m_game->GetClearPosition() - m_position;
	if (diff.LengthSq() < 100.0f * 100.0f) {
		m_game->NotifyClear();
		m_playerState = enState_End;
	}

	State();

#ifdef _DEBUG
	Debug();
#endif

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
	m_position = m_characterController->Execute(m_moveSpeed, 1.0f / 60.0f);
	m_modelRender.SetPosition(m_position);

	Rotation();

	Crouch();

	if (m_addMoveSpeed.x < ADDMOVESPEED_MIN &&
		m_addMoveSpeed.y < ADDMOVESPEED_MIN &&
		m_addMoveSpeed.z < ADDMOVESPEED_MIN
		) {
		//追加の移動速度を初期化。
		m_addMoveSpeed = Vector3::Zero;
	}
	else {
		//追加の移動速度を減算。
		m_addMoveSpeed -= m_addMoveSpeed / ADDMOVESPEED_DIV;
	}

	m_gameCamera->SetPosition(m_position);
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
		m_playerState != enState_JumpEnd
	) {
		//しゃがみ歩き。
		if (m_isCrouch) {
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
	if (m_playerState == enState_Jump) {
		//移動速度を加算。
		m_moveSpeed.y += JUMP_POWER;
	}

	//天井にぶつかったなら。
	if (m_characterController->IsOnSeiling()) {
		//ジャンプをやめる。
		m_moveSpeed.y = 0.0f;
		m_playerState = enState_JumpEnd;
	}

	//地面にいないなら。
	if (!m_characterController->IsOnGround()) {
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
/// しゃがみ処理。
/// </summary>
void Player::Crouch()
{
	//ジャンプ中なら。
	if (m_playerState == enState_Jump ||
		m_playerState == enState_JumpEnd
	) {
		return;
	}

	if (m_isCrouch) {
		//しゃがみボタンが離されたら。
		if (!g_pad[0]->IsPress(enButtonY)) {
			//しゃがみ状態から立ち上がれるなら。
			if (m_characterController->IsCanStandUp()) {
				//キャラコンの高さを上げる。
				delete m_characterController;
				m_characterController = new CharacterController;
				m_characterController->Init(CHARACON_RADIUS, CHARACON_HEIGHT, m_position);

				//カメラの座標を設定。
				m_gameCamera->SetCrouchState(false);
				m_isCrouch = false;
			}
		}
	}
	else {
		//しゃがみボタンが押されたら。
		if (g_pad[0]->IsTrigger(enButtonY)) {
			//キャラコンの高さを下げる。
			delete m_characterController;
			m_characterController = new CharacterController;
			m_characterController->Init(CHARACON_RADIUS, CHARACON_HEIGHT / 2.0f, m_position);

			//カメラの座標を設定。
			m_gameCamera->SetCrouchState(true);
			m_isCrouch = true;
		}
	}
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

	case enState_Jump:
		m_modelRender.PlayAnimation(enState_Jump, INTERPOLATE_TIME);
		break;

	case enState_JumpEnd:
		m_modelRender.PlayAnimation(enState_JumpEnd, INTERPOLATE_TIME);
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

	case enState_Jump:
		ProcessJumpStateTransition();
		break;

	case enState_JumpEnd:
		ProcessJumpEndStateTransition();
		break;
	}
}

/// <summary>
/// 通常の遷移処理。
/// </summary>
void Player::ProcessCommonStateTransition()
{
	//地面にいるなら。
	if (m_characterController->IsOnGround()) {
		//ジャンプボタンが押されたら。
		if (g_pad[0]->IsTrigger(enButtonA)) {
			m_playerState = enState_Jump;
			return;
		}
	}
	else {
		m_playerState = enState_JumpEnd;
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
/// ジャンプ状態の遷移処理。
/// </summary>
void Player::ProcessJumpStateTransition()
{
	//落下が始まったら。
	if (m_moveSpeed.y < 0.0f) {
		m_playerState = enState_JumpEnd;
	}

	//地面についたら。
	if (m_characterController->IsOnGround()) {
		//重力加速度を初期化。
		m_gravityAccel = 0.0f;
		ProcessCommonStateTransition();
	}
}

/// <summary>
/// ジャンプ終了状態の遷移処理。
/// </summary>
void Player::ProcessJumpEndStateTransition()
{
	//地面についたら。
	if (m_characterController->IsOnGround()) {
		//重力加速度を初期化。
		m_gravityAccel = 0.0f;
		ProcessCommonStateTransition();
	}
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
	m_characterController->SetPosition(m_position);
	//カメラもワープさせる。
	m_gameCamera->SetWarp(angle);
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
		m_game->NotifyReset();
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
	m_isCrouch = false;
	m_hp = MAX_HP;
	m_walkSpeed = 0.0f;
	m_gravityAccel = 0.0f;
	m_damageTimer = 0.0f;
	m_playerState = enState_Idle;
	m_addMoveSpeed = Vector3::Zero;

	//座標を初期化。
	m_position = m_game->GetRespawnPosition();
	m_characterController->SetPosition(m_position);

	//フェードインを開始。
	Fade* fade = FindGO<Fade>("fade");
	fade->StartFadeIn();
}


void Player::OnAnimationEvent(const wchar_t* clipName, const wchar_t* eventName)
{
	(void)clipName;
	//イベントなら
	if (wcscmp(eventName, L"step") == 0)
	{
		m_game->AddStepNum();
	}

}


/// <summary>
/// デバッグ処理。
/// </summary>
void Player::Debug()
{
	////////////// todo debug
	Vector3 vec = g_camera3D->GetForward();
	wchar_t text[256];
	swprintf_s(text, 256,
		L"PlayerX:%.2f \nPlayerY:%.2f \nPlayerZ:%.2f \nForwardX:%.2f \nForwardY:%.2f \nForwardZ:%.2f \nAnim:%d \nGround:%d \nSeiling:%d \nCrouch:%d",
		m_position.x,
		m_position.y,
		m_position.z,
		vec.x,
		vec.y,
		vec.z,
		m_playerState,
		m_characterController->IsOnGround(),
		m_characterController->IsOnSeiling(),
		m_isCrouch
	);
	a.SetText(text);
	/////////////
}

void Player::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
	m_damageSpriteRender.Draw(rc);
	a.Draw(rc);
}