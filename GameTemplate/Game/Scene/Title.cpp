#include "stdafx.h"
#include "Title.h"
#include "Fade.h"
#include "Game.h"
#include "Stage.h"

namespace
{
	const float CURSOR_POS_X = 150.0f;
	const float SHAKE_SPEED = 0.3f;
}


Title::Title()
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
	EffectEngine::GetInstance()->ResistEffect(EffectEmitter::enEffect_PortalFrame_Red, u"Assets/effect/portal/portalFrame_red.efk");
	//�|�[�^���̔��˃G�t�F�N�g�B
	EffectEngine::GetInstance()->ResistEffect(EffectEmitter::enEffect_PortalShot_Blue, u"Assets/effect/portal/portalShot_blue.efk");
	//�|�[�^���̔��˃G�t�F�N�g�B
	EffectEngine::GetInstance()->ResistEffect(EffectEmitter::enEffect_PortalShot_Red, u"Assets/effect/portal/portalShot_blue.efk");
}

Title::~Title()
{
	DeleteGO(m_stage);
}

bool Title::Start()
{
	RenderingEngine::GetInstance()->GetPostEffet().Enable(PostEffect::enPostEffect_DoF);

	m_level2DRender.Init("Assets/level/UI/title.casl", [&](Level2DObjectData& objData) {
		//���O��start�Ȃ�B
		if (objData.EqualObjectName("title_start") == true) {
			m_menuSpriteRender[0].Init(objData.ddsFilePath, objData.width, objData.height);
			m_menuSpriteRender[0].SetPosition(objData.position);
			m_menuSpriteRender[0].Update();
			m_cursorSpriteRender.Init("Assets/sprite/UI/result_cursor.dds", 48.75f, 56.25f);
			CursorUpdate();
			return true;
		}
		//���O��howto�Ȃ�B
		if (objData.EqualObjectName("title_howto") == true) {
			m_menuSpriteRender[1].Init(objData.ddsFilePath, objData.width, objData.height);
			m_menuSpriteRender[1].SetPosition(objData.position);
			m_menuSpriteRender[1].Update();
			return true;
		}
		//���O��exit�Ȃ�B
		if (objData.EqualObjectName("title_exit") == true) {
			m_menuSpriteRender[2].Init(objData.ddsFilePath, objData.width, objData.height);
			m_menuSpriteRender[2].SetPosition(objData.position);
			m_menuSpriteRender[2].Update();
			return true;
		}
		return true;
	});

	m_guideSpriteRender.Init("Assets/sprite/howto/guide.DDS", FRAME_BUFFER_W, FRAME_BUFFER_H);

	m_stage = NewGO<Stage>(0, "stage");
	m_stage->Init("Assets/level/stage/title.tkl");

	m_fade = FindGO<Fade>("fade");
	m_fade->StartFadeIn();

	return true;
}

void Title::Update()
{
	if (!m_isInited) {
		m_cameraTarget = g_camera3D->GetTarget();
		m_isInited = true;
	}

	if (m_isFadeOut) {

		if (!m_fade->IsFade()) {
			NewGO<Game>(0, "game");
			DeleteGO(this);
			RenderingEngine::GetInstance()->GetPostEffet().Disable(PostEffect::enPostEffect_DoF);
		}
	}
	else {
		//���{�^���ŉ��B
		if (g_pad[0]->IsTrigger(enButtonDown)) {
			m_cursor++;
			CursorUpdate();
		}
		//��{�^���ŏ�B
		if (g_pad[0]->IsTrigger(enButtonUp)) {
			m_cursor--;
			CursorUpdate();
		}
		//A�{�^���Ō���B
		if (g_pad[0]->IsTrigger(enButtonA)) {

			if (m_cursor == enState_Title) {
				m_fade->StartFadeOut();
				m_isFadeOut = true;
			}
			else if (m_cursor == enState_Guide) {
				m_state = enState_Guide;
			}
			else if (m_cursor == enState_Exit) {
				exit(0);
			}
		}
		//B�{�^���Ŗ߂�B
		if (g_pad[0]->IsTrigger(enButtonB)) {

			if (m_state == enState_Guide) {
				m_state = enState_Title;
				m_cursor = 0;
				CursorUpdate();
			}
		}
	}

	CameraUpdate();
}

void Title::CursorUpdate()
{
	m_cursor = max(min(2, m_cursor), 0);
	//�J�[�\�����W�̐ݒ�B
	Vector3 cursorPos = m_menuSpriteRender[m_cursor].GetPosition();
	cursorPos.x -= CURSOR_POS_X;
	m_cursorSpriteRender.SetPosition(cursorPos);
	m_cursorSpriteRender.Update();
}

void Title::CameraUpdate()
{
	m_cameraTimer += g_gameTime->GetFrameDeltaTime() * SHAKE_SPEED;

	//�J������h�炷�B
	Vector3 shakeMoveSpeed;
	shakeMoveSpeed.x = sin(m_cameraTimer);
	shakeMoveSpeed.z = sin(m_cameraTimer);
	m_cameraTarget += shakeMoveSpeed;

	g_camera3D->SetTarget(m_cameraTarget);
	g_camera3D->Update();
}

void Title::Render(RenderContext& rc)
{
	switch (m_state)
	{
	case enState_Title:
		m_cursorSpriteRender.Draw(rc);
		for (int i = 0; i < 3; ++i) {
			m_menuSpriteRender[i].Draw(rc);
		}
		break;

	case enState_Guide:
		m_guideSpriteRender.Draw(rc);
		break;
	}
}