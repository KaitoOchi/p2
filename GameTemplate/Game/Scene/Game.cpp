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
	const Vector3	TIMER_FONT_POS = { -950.0f, 525.0f, 0.0f };		//�摜�̍��W�B
	const float		TIME_LIMIT = 3599.99f;							//���Ԑ����B
	const float		RESET_TIME = 2.0f;								//���Z�b�g���ԁB
}


Game::Game()
{
	//�^���b�g�̎ˌ��G�t�F�N�g�B
	EffectEngine::GetInstance()->ResistEffect(EffectEmitter::enEffect_Turret_Shot, u"Assets/effect/turret/turret_shot.efk");
	//�^���b�g�̃��[�U�[�G�t�F�N�g�B
	EffectEngine::GetInstance()->ResistEffect(EffectEmitter::enEffect_Turret_Laser, u"Assets/effect/turret/turret_laser.efk");
	//�G�i�W�[�{�[���̃G�t�F�N�g�B
	EffectEngine::GetInstance()->ResistEffect(EffectEmitter::enEffect_EnergyBall, u"Assets/effect/energyBall/energyBall.efk");
	//�|�[�^���̐g�g�݃G�t�F�N�g�B
	EffectEngine::GetInstance()->ResistEffect(EffectEmitter::enEffect_PortalFrame_Blue, u"Assets/effect/portal/portalFrame_blue.efk");
	//�|�[�^���̐g�g�݃G�t�F�N�g�B
	EffectEngine::GetInstance()->ResistEffect(EffectEmitter::enEffect_PortalFrame_Red, u"Assets/effect/portal/portalFrame_blue.efk");
	//�|�[�^���̔��˃G�t�F�N�g�B
	EffectEngine::GetInstance()->ResistEffect(EffectEmitter::enEffect_PortalShot_Blue, u"Assets/effect/portal/portalShot_blue.efk");
	//�|�[�^���̔��˃G�t�F�N�g�B
	EffectEngine::GetInstance()->ResistEffect(EffectEmitter::enEffect_PortalShot_Red, u"Assets/effect/portal/portalShot_blue.efk");
}

Game::~Game()
{

}

bool Game::Start()
{
	//�t���[�����[�g���Œ�
	//g_engine->SetFrameRateMode(K2EngineLow::enFrameRateMode_Fix, 60);

	//�����蔻���L����
	PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();

	//�|�[�^���J�����������B
	PortalCamera* portalCamera = FindGO<PortalCamera>("portalCamera");

	//�v���C���[�𐶐��B
	m_player = NewGO<Player>(1, "player");
	m_gameCamera = NewGO<GameCamera>(2, "gameCamera");
	m_portalGun = NewGO<PortalGun>(3, "portalGun");

	//�|�[�^���𐶐��B
	PortalFrame* portalFrame[PORTAL_NUM];
	for (int i = 0; i < PORTAL_NUM; i++) {
		portalFrame[i] = NewGO<PortalFrame>(0, "portalFrame");

		//�|�[�^���J�����Ƀ|�[�^���̃|�C���^��ݒ�B
		portalCamera->SetPortalFramePointer(portalFrame[i], i);
	}
	portalFrame[0]->SetAnotherPortal(portalFrame[1]);
	portalFrame[0]->SetPortalType(PortalFrame::enPortalType_Blue);
	portalFrame[1]->SetAnotherPortal(portalFrame[0]);
	portalFrame[1]->SetPortalType(PortalFrame::enPortalType_Red);

	//�X�e�[�W�𐶐��B
	m_stage = NewGO<Stage>(4, "stage");
	m_stage->Init("Assets/level/stage/level00.tkl");

	//�^�C�}�[�����̐ݒ�B
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
/// ���Z�b�g�����B
/// </summary>
void Game::Reset()
{
	//���Z�b�g���łȂ��Ȃ�B
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
/// ���Ԍv�������B
/// </summary>
void Game::Timer()
{
	//���U���g���Ȃ�B
	if (m_gameState == enGameState_Result) {
		return;
	}

	m_gameScore.timer += g_gameTime->GetFrameDeltaTime();
	//���݂̎��ԁB
	float timer = m_gameScore.timer;

	if (timer >= TIME_LIMIT) {
		m_gameScore.timer = TIME_LIMIT;
	}

	//�����v�Z�B
	int m = timer / 60;
	//�b���v�Z�B
	float s = static_cast<int>(timer) % 60;
	//�~���b���v�Z�B
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

	//���U���g�V�[�����Ăяo���B
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
	//�|�[�^���t���[�����폜�B
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

	//�|�[�^���J�����������B
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