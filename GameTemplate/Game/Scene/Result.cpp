#include "stdafx.h"
#include "Result.h"
#include "Game.h"
#include "Title.h"
#include "Fade.h"

namespace
{
	const Vector3	SCORE_FONT_POSITION = { -100.0f, -300.0f, 0.0f };
	const int		SCORE_NUM = 4;
	const int		SCORE_POINT_DEFAULT = 9999;						//スコアの初期値。
	const int		STEP_SUB_POINT = 2;								//歩き一歩毎の減点数。
	const int		SHOT_SUB_POINT = 50;							//ショット一発毎の減点数。
}

Result::Result()
{
	memset(&m_isActive, 0, sizeof(m_isActive));
}

Result::~Result()
{

}

bool Result::Start()
{
	m_level2DRender.Init("Assets/level/UI/result.casl", [&](Level2DObjectData& objData) {
		//名前が背景なら。
		if (objData.EqualObjectName("backGround") == true) {
			return true;
		}
		//名前がタイマー画像なら。
		if (objData.EqualObjectName("result_timer") == true) {
			m_scoreSpriteRender[0].Init(objData.ddsFilePath, objData.width, objData.height);
			m_scoreSpriteRender[0].SetPosition(objData.position);
			m_scoreSpriteRender[0].Update();
			return true;
		}
		//名前が歩き画像なら。
		if (objData.EqualObjectName("result_footstep") == true) {
			m_scoreSpriteRender[1].Init(objData.ddsFilePath, objData.width, objData.height);
			m_scoreSpriteRender[1].SetPosition(objData.position);
			m_scoreSpriteRender[1].Update();
			return true;
		}
		//名前がポータル画像なら。
		if (objData.EqualObjectName("result_portal") == true) {
			m_scoreSpriteRender[2].Init(objData.ddsFilePath, objData.width, objData.height);
			m_scoreSpriteRender[2].SetPosition(objData.position);
			m_scoreSpriteRender[2].Update();
			return true;
		}
		//名前が横線なら。
		if (objData.EqualObjectName("result_stick") == true) {
			m_scoreSpriteRender[3].Init(objData.ddsFilePath, objData.width, objData.height);
			m_scoreSpriteRender[3].SetPosition(objData.position);
			m_scoreSpriteRender[3].SetScale(objData.scale);
			m_scoreSpriteRender[3].Update();
			return true;
		}
		//名前がタイマースコアなら。
		if (objData.EqualObjectName("timer_score") == true) {
			m_scoreFontRender[0].SetPosition(objData.position);
			return true;
		}
		//名前が歩きスコアなら。
		if (objData.EqualObjectName("footstep_score") == true) {
			m_scoreFontRender[1].SetPosition(objData.position);
			return true;
		}
		//名前がポータルスコアなら。
		if (objData.EqualObjectName("portal_score") == true) {
			m_scoreFontRender[2].SetPosition(objData.position);
			return true;
		}
		//名前が最終スコアなら。
		if (objData.EqualObjectName("final_score") == true) {
			m_scoreFontRender[3].SetPosition(objData.position);
			return true;
		}
		//名前が選択画像なら。
		if (objData.EqualObjectName("result_press") == true) {
			m_pressSpriteRender.Init(objData.ddsFilePath, objData.width, objData.height);
			m_pressSpriteRender.SetPosition(objData.position);
			m_pressSpriteRender.Update();
			return true;
		}
		//名前がカーソル座標なら。
		if (objData.EqualObjectName("cursor_pos0") == true) {
			m_cursorPos[0] = objData.position;
			return true;
		}
		//名前がカーソル座標なら。
		if (objData.EqualObjectName("cursor_pos1") == true) {
			m_cursorPos[1] = objData.position;
			return true;
		}
		return true;
	});

	//背景画像の設定。
	m_backGroundSpriteRender.Init("Assets/sprite/UI/black.dds", 1920.0f, 1080.0f);
	m_backGroundSpriteRender.SetMulColor(Vector4(1.0f, 1.0f, 1.0f, 0.0f));
	m_backGroundSpriteRender.Update();

	//カーソル画像の設定。
	m_cursorSpriteRender.Init("Assets/sprite/UI/result_cursor.dds", 195.0f, 225.0f);
	m_cursorSpriteRender.SetScale(Vector3(0.1f, 0.1f, 0.0f));
	CursorUpdate();

	//スコアを計算。 
	int score = SCORE_POINT_DEFAULT;
	score -= static_cast<int>(m_scoreTime);
	score -= m_scoreStepNum * STEP_SUB_POINT;
	score -= m_scoreShotNum * SHOT_SUB_POINT;

	int scoreText[3];
	scoreText[0] = m_scoreStepNum;
	scoreText[1] = m_scoreShotNum;
	scoreText[2] = score;

	for (int i = 0; i < SCORE_NUM; i++) {
		//スコア文字の設定。
		wchar_t text[256];
		if (i == 0) {
			//現在の時間。
			float timer = m_scoreTime;
			//分を計算。
			int m = timer / 60;
			//秒を計算。
			float s = static_cast<int>(timer) % 60;
			//ミリ秒を計算。
			s += timer - static_cast<int>(timer);
			swprintf_s(text, 255,
				L"%d:%05.02f",
				m,
				s
			);
		}
		else {
			swprintf_s(text, 256,
				L"%d",
				scoreText[i - 1]
			);
		}
		m_scoreFontRender[i].SetText(text);
	}

	m_fade = FindGO<Fade>("fade");

	return true;
}

void Result::Update()
{
	switch (m_resultState)
	{
	case enResultState_FadeOut:
		FadeOut();
		break;

	case enResultState_CalcScore:
		CalcScore();
		break;
		
	case enResultState_Select:
		Select();
		break;

	case enResultState_End:
		GameEnd();
		break;
	}
}

void Result::FadeOut()
{
	m_timer += g_gameTime->GetFrameDeltaTime();
	m_backGroundSpriteRender.SetMulColor(Vector4(1.0f, 1.0f, 1.0f, m_timer));
	m_backGroundSpriteRender.Update();

	if (m_timer > 0.5f) {
		m_timer = 0.5f;
		m_resultState = enResultState_CalcScore;
	}
}

void Result::CalcScore()
{
	if (m_timer >= 5.0f) {
		m_resultState = enResultState_Select;
		m_cursor = 0;
		return;
	}

	m_cursor = m_timer;
	m_isActive[m_cursor] = true;

	m_timer += g_gameTime->GetFrameDeltaTime();
}

void Result::Select()
{
	if (g_pad[0]->IsTrigger(enButtonLeft)) {
		m_cursor--;
		CursorUpdate();
	}
	else if (g_pad[0]->IsTrigger(enButtonRight)) {
		m_cursor++;
		CursorUpdate();
	}
	else if (g_pad[0]->IsTrigger(enButtonA)) {
		m_resultState = enResultState_End;
	}
}

void Result::CursorUpdate()
{
	m_cursor = max(min(m_cursor, 1), 0);
	m_cursorSpriteRender.SetPosition(m_cursorPos[m_cursor]);
	m_cursorSpriteRender.Update();
}

void Result::GameEnd()
{
	if (m_isFadeOut) {
		if (!m_fade->IsFade()) {

			Game* game = FindGO<Game>("game");

			if (m_cursor == 0) {
				game->NotifyRetry();
			}
			else if (m_cursor == 1) {
				NewGO<Title>(0, "title");
				game->NotifyEnd();
			}
			DeleteGO(this);
		}
	}
	else {
		m_fade->StartFadeOut();
		m_isFadeOut = true;
	}
}

void Result::Render(RenderContext& rc)
{
	m_backGroundSpriteRender.Draw(rc);

	for (int i = 0; i < SCORE_NUM; i++) {
		if (m_isActive[i] == true) {
			m_scoreSpriteRender[i].Draw(rc);
			m_scoreFontRender[i].Draw(rc);
		}
	}

	if (m_isActive[4] == true) {
		m_pressSpriteRender.Draw(rc);
		m_cursorSpriteRender.Draw(rc);
	}
}