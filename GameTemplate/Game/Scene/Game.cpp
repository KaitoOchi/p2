#include "stdafx.h"
#include "Game.h"
#include "Player.h"
#include "GameCamera.h"
#include "PortalGun.h"
#include "PortalFrame.h"
#include "PortalCamera.h"
#include "Stage.h"
#include "Result.h"

namespace
{
	const Vector3	TIMER_FONT_POS = { -950.0f, 525.0f, 0.0f };		//画像の座標。
	const float		TIME_LIMIT = 3599.99f;							//時間制限。
	const float		RESET_TIME = 2.0f;								//リセット時間。
}


Game::Game()
{
	//タレットの射撃エフェクト。
	EffectEngine::GetInstance()->ResistEffect(EffectEmitter::enEffect_Turret_Shot, u"Assets/effect/turret/turret_shot.efk");
	//タレットのレーザーエフェクト。
	EffectEngine::GetInstance()->ResistEffect(EffectEmitter::enEffect_Turret_Laser, u"Assets/effect/turret/turret_laser.efk");
	//エナジーボールのエフェクト。
	EffectEngine::GetInstance()->ResistEffect(EffectEmitter::enEffect_EnergyBall, u"Assets/effect/energyBall/energyBall.efk");
	//ポータルの青枠組みエフェクト。
	EffectEngine::GetInstance()->ResistEffect(EffectEmitter::enEffect_PortalFrame_Blue, u"Assets/effect/portal/portalFrame_blue.efk");
	//ポータルの青枠組みエフェクト。
	EffectEngine::GetInstance()->ResistEffect(EffectEmitter::enEffect_PortalFrame_Red, u"Assets/effect/portal/portalFrame_blue.efk");
	//ポータルの発射エフェクト。
	EffectEngine::GetInstance()->ResistEffect(EffectEmitter::enEffect_PortalShot_Blue, u"Assets/effect/portal/portalShot_blue.efk");
	//ポータルの発射エフェクト。
	EffectEngine::GetInstance()->ResistEffect(EffectEmitter::enEffect_PortalShot_Red, u"Assets/effect/portal/portalShot_blue.efk");
}

Game::~Game()
{

}

bool Game::Start()
{
	//フレームレートを固定
	//g_engine->SetFrameRateMode(K2EngineLow::enFrameRateMode_Fix, 60);

	//当たり判定を有効化
	PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();

	//ポータルカメラを検索。
	PortalCamera* portalCamera = FindGO<PortalCamera>("portalCamera");

	//プレイヤーを生成。
	m_player = NewGO<Player>(1, "player");
	m_gameCamera = NewGO<GameCamera>(2, "gameCamera");
	m_portalGun = NewGO<PortalGun>(3, "portalGun");

	//ポータルを生成。
	PortalFrame* portalFrame[PORTAL_NUM];
	for (int i = 0; i < PORTAL_NUM; i++) {
		portalFrame[i] = NewGO<PortalFrame>(0, "portalFrame");

		//ポータルカメラにポータルのポインタを設定。
		portalCamera->SetPortalFramePointer(portalFrame[i], i);
	}
	portalFrame[0]->SetAnotherPortal(portalFrame[1]);
	portalFrame[0]->SetPortalType(PortalFrame::enPortalType_Blue);
	portalFrame[1]->SetAnotherPortal(portalFrame[0]);
	portalFrame[1]->SetPortalType(PortalFrame::enPortalType_Red);

	//ステージを生成。
	m_stage = NewGO<Stage>(4, "stage");
	m_stage->Init("Assets/level/stage/level00.tkl");

	//タイマー文字の設定。
	m_timerFontRender.SetPosition(TIMER_FONT_POS);
	m_timerFontRender.SetColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	m_timerFontRender.SetScale(1.5f);
	m_timerFontRender.SetShadowParam(true, 2.0f, Vector4(1.0f, 1.0f, 1.0f, 1.0f));

	return true;
}

void Game::Update()
{
	Reset();

	Timer();
}

/// <summary>
/// リセット処理。
/// </summary>
void Game::Reset()
{
	//リセット中でないなら。
	if (m_gameState != enGameState_Reset) {
		return;
	}

	m_resetTimer -= g_gameTime->GetFrameDeltaTime();

	if (m_resetTimer < 0.0f) {
		m_player->Reset();
		m_gameCamera->Reset();
		m_portalGun->ResetPortal();
		m_gameState = enGameState_Game;
	}
}

/// <summary>
/// 時間計測処理。
/// </summary>
void Game::Timer()
{
	//リザルト中なら。
	if (m_gameState == enGameState_Result) {
		return;
	}

	m_gameScore.timer += g_gameTime->GetFrameDeltaTime();
	//現在の時間。
	float timer = m_gameScore.timer;

	if (timer >= TIME_LIMIT) {
		m_gameScore.timer = TIME_LIMIT;
	}

	//分を計算。
	int m = timer / 60;
	//秒を計算。
	float s = static_cast<int>(timer) % 60;
	//ミリ秒を計算。
	s += timer - static_cast<int>(timer);

	wchar_t text[256];
	swprintf_s(text, 255, L"Time %d:%05.02f", m, s);
	m_timerFontRender.SetText(text);
}

void Game::NotifyReset()
{
	m_gameCamera->Dead();
	m_gameState = enGameState_Reset;
	m_resetTimer = RESET_TIME;
}

void Game::NotifyClear()
{
	m_gameState = enGameState_Result;

	m_gameCamera->End();

	//リザルトシーンを呼び出す。
	Result* result = NewGO<Result>(9, "result");
	result->SetScore(
		m_gameScore.timer,
		m_gameScore.stepNum,
		m_gameScore.shotPortal
	);
}

void Game::NotifyRetry()
{
	m_player->Reset();
	m_gameCamera->Reset();
	m_portalGun->ResetPortal();
	m_gameState = enGameState_Game;
	memset(&m_gameScore, 0, sizeof(GameScore));
}

void Game::NotifyEnd()
{
	//ポータルフレームを削除。
	QueryGOs<PortalFrame>("portalFrame", [&](PortalFrame* portalFrame) {
		portalFrame->Delete();
		DeleteGO(portalFrame);
		return true;
		});

	DeleteGO(m_player);
	DeleteGO(m_gameCamera);
	DeleteGO(m_portalGun);
	DeleteGO(m_stage);
	DeleteGO(this);

	//ポータルカメラを検索。
	PortalCamera* portalCamera = FindGO<PortalCamera>("portalCamera");
	portalCamera->DeletePortalFramePointer();
}

const Vector3& Game::GetRespawnPosition() const
{
	return m_stage->GetRespawnPosition();
}

const Vector3& Game::GetClearPosition() const
{
	return m_stage->GetClearPosition();
}

void Game::Render(RenderContext& rc)
{
	m_timerFontRender.Draw(rc);
}