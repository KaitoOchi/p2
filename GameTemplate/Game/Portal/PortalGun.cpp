#include "stdafx.h"
#include "PortalGun.h"
#include "PortalCamera.h"
#include "Game.h"

namespace
{
	const Vector3	PORTALGUN_POS = Vector3(10.0f, 0.0f, 20.0f);	//ポータルガンの座標。
	const float		PORTALGUN_ROT_Y = 1.15f;						//ポータルガンの上下の回転量。
	const float		CURSOR_SIZE = 64.0f;							//カーソルサイズ。
	const float		RAY_LENGTH = 5000.0f;							//レイの長さ。
	const float		SHOT_EFFECT_SPEED = 200.0f;						//エフェクトの再生速度。
	const float		SHOT_DURATION = 0.5f;							//発射間隔。
	const float		SHAKE_SPEED = 8.0f;								//画面揺れの速度。
	const float		SHAKE_POWER = 2.0f;								//画面揺れの量。
}


PortalGun::PortalGun()
{
	//ポータルフレームを初期化。
	for (int i = 0; i < PORTAL_NUM; i++) {
		m_portalFrame[i] = nullptr;
	}
}

PortalGun::~PortalGun()
{

}

bool PortalGun::Start()
{
	//ポータルを検索。
	std::vector<PortalFrame*> portalFrames;
	portalFrames = FindGOs<PortalFrame>("portalFrame");
	//ポータルカメラを検索。
	m_portalCamera = FindGO<PortalCamera>("portalCamera");
	//ゲームシーンを検索。
	m_game = FindGO<Game>("game");

	m_shakeMoveSpeedTmp = m_position;
	
	//ポインタに格納。
	for (int i = 0; i < PORTAL_NUM; i++) {
		m_portalFrame[i] = portalFrames[i];
		m_portalCamera->SetPortalFramePointer(m_portalFrame[i], i);
	}

	//ポータルガンを初期化。
	m_portalGunModelRender.Init("Assets/modelData/portal/portalGun.tkm", 0, 0, enModelUpAxisZ, true, true, ModelRender::enModelInitMode_ForwardLighting);
	m_portalGunModelRender.SetPosition(PORTALGUN_POS);
	m_portalGunModelRender.Update();

	//クロスヘアを初期化。
	m_cursorSpriteRender[0].Init("Assets/sprite/UI/cursor_blue.DDS", CURSOR_SIZE, CURSOR_SIZE);
	m_cursorSpriteRender[0].SetMulColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
	m_cursorSpriteRender[0].Update();
	m_cursorSpriteRender[1].Init("Assets/sprite/UI/cursor_red.DDS", CURSOR_SIZE, CURSOR_SIZE);
	m_cursorSpriteRender[1].SetMulColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
	m_cursorSpriteRender[1].Update();
	m_cursorSpriteRender[2].Init("Assets/sprite/UI/cursor.DDS", CURSOR_SIZE, CURSOR_SIZE);

	return true;
}

void PortalGun::Update()
{
	if (m_shotDuration <= 0.0f) {
		//青ポータルを設置。
		if (g_pad[0]->IsTrigger(enButtonLB1)) {
			SetPortal(PortalFrame::enPortalType_Blue);
		}
		//赤ポータルを設置。
		else if (g_pad[0]->IsTrigger(enButtonRB1)) {
			SetPortal(PortalFrame::enPortalType_Red);
		}
	}
	else {
		m_shotDuration -= g_gameTime->GetFrameDeltaTime();
	}

	SetPortalGunModel();
}

/// <summary>
/// ポータルの設置処理。
/// </summary>
/// <param name="type">ポータルタイプ</param>
void PortalGun::SetPortal(const PortalFrame::PortalType type)
{
	//カメラの前方向を取得。
	Vector3 forward = g_camera3D->GetForward();

	//当たったオブジェクトの詳細。
	PhysicsWorld::RayHitObject hit;
	//レイの視点と終点を設定。
	Vector3 rayStartPos = g_camera3D->GetPosition();
	Vector3 rayEndPos = rayStartPos + (forward * RAY_LENGTH);
	//レイを飛ばす。
	bool isHit = PhysicsWorld::GetInstance()->RayTest(rayStartPos, rayEndPos, hit);

	//当たらなかった場合、何もしない。
	if (isHit == false) {
		return;
	}

	//当たったオブジェクトの座標に合わせて設置する。
	Vector3 portalPos = hit.position;
	//ポータルを設置。
	int num = static_cast<int>(type);
	m_portalFrame[num]->SetPortalFrame(portalPos, hit.normal);

	//カーソルを設定。
	m_cursorSpriteRender[num].SetMulColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	m_cursorSpriteRender[num].Update();

	Quaternion rot;
	rot.SetRotation(Vector3::AxisZ, g_camera3D->GetForward());

	//発射エフェクトを再生。
	EffectEmitter* shotEffect = NewGO<EffectEmitter>(0);
	shotEffect->Init(EffectEmitter::enEffect_PortalShot_Blue);
	shotEffect->SetPosition(g_camera3D->GetPosition());
	shotEffect->SetRotation(rot);
	shotEffect->SetTime(SHOT_EFFECT_SPEED * g_gameTime->GetFrameDeltaTime());
	shotEffect->Play();

	//発射数を加算。
	m_game->AddShotNum();

	//発射間隔を設定。
	m_shotDuration = SHOT_DURATION;
}

void PortalGun::ResetPortal()
{
	for (int i = 0; i < PORTAL_NUM; i++) {
		//ポータルをリセット。
		m_portalFrame[i]->Reset();
		//カーソルを設定。
		m_cursorSpriteRender[i].SetMulColor(Vector4(1.0f, 1.0f, 1.0f, 0.0f));
		m_cursorSpriteRender[i].Update();
	}
}

void PortalGun::Reset()
{
	m_isIdle = false;
	m_shakeMoveSpeedTmp = Vector3::Zero;
	m_shakeTimer = 0.0f;
}

/// <summary>
/// モデルの設定処理。
/// </summary>
void PortalGun::SetPortalGunModel()
{
	Vector3 forward = g_camera3D->GetForward();

	Vector3 pos = PORTALGUN_POS;
	pos.Normalize();

	float rotY = forward.y * 90.0f;

	Quaternion rot;
	rot.SetRotationYFromDirectionXZ(forward);
	rot.AddRotationDegX(-rotY);
	rot.Apply(pos);
	pos *= 30.0f;

	//座標を設定。
	Vector3 portalGunPos = g_camera3D->GetPosition();
	portalGunPos += pos;
	portalGunPos.y -= 10.0f;

	//回転を設定。
	Quaternion portalGunRot;
	portalGunRot.SetRotationYFromDirectionXZ(forward);
	portalGunRot.AddRotationDegX(-rotY);

	m_position = portalGunPos;

	Shake();

	//ポータルガンを設定。
	m_portalGunModelRender.SetPosition(m_position);
	m_portalGunModelRender.SetRotation(portalGunRot);
	m_portalGunModelRender.Update();
}

void PortalGun::Shake()
{
	m_shakeTimer += g_gameTime->GetFrameDeltaTime() * SHAKE_SPEED;

	Vector2 input;
	input.x = g_pad[0]->GetLStickXF();
	input.y = g_pad[0]->GetLStickYF();
	float shakeValue = max(fabsf(input.x), fabsf(input.y));

	Vector3 shakeMoveSpeed;

	//スティックの入力があるなら。
	if (shakeValue) {
		if (m_isIdle) {
			m_shakeTimer = 0.0f;
			m_isIdle = false;
		}

		//揺れを追加。
		shakeMoveSpeed.y = sin(m_shakeTimer * shakeValue) * SHAKE_POWER;
		m_shakeMoveSpeedTmp = shakeMoveSpeed;
	}
	else {
		if (!m_isIdle) {
			m_shakeTimer = 0.0f;
			m_isIdle = true;
		}

		//1秒を超えないようにする。
		m_shakeTimer = min(m_shakeTimer, 1.0f);

		//線形補間で元の位置に戻す。
		shakeMoveSpeed.Lerp(m_shakeTimer, m_shakeMoveSpeedTmp + m_position, m_position);
		shakeMoveSpeed -= m_position;
	}

	//揺れの移動量を加算。
	m_position += shakeMoveSpeed;
	m_portalGunModelRender.SetPosition(shakeMoveSpeed);
}

void PortalGun::Render(RenderContext& rc)
{
	//ポータルガンを描画。
	m_portalGunModelRender.Draw(rc);

	//クロスヘアの描画。
	m_cursorSpriteRender[2].Draw(rc);
	m_cursorSpriteRender[0].Draw(rc);
	m_cursorSpriteRender[1].Draw(rc);
}