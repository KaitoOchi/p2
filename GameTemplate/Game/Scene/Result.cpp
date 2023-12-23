#include "stdafx.h"
#include "Result.h"
#include "Game.h"
#include "Title.h"


Result::Result()
{

}

Result::~Result()
{

}

bool Result::Start()
{
	//背景画像の設定。
	m_backGroundSpriteRender.Init("Assets/sprite/UI/black.dds", 1920.0f, 1080.0f);
	m_backGroundSpriteRender.SetMulColor(Vector4(1.0f, 1.0f, 1.0f, 0.0f));
	m_backGroundSpriteRender.Update();

	//スコア文字の設定。
	wchar_t text[256];
	swprintf_s(text, 256,
		L"score:%d",
		m_score
	);
	m_scoreFontRender.SetText(text);
	m_scoreFontRender.SetColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));

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
		m_scoreFontRender.SetColor(Vector4::White);
	}
}

void Result::CalcScore()
{
	m_timer += g_gameTime->GetFrameDeltaTime();

	if (m_timer > 2.0f) {
		m_resultState = enResultState_Select;
	}
}

void Result::Select()
{
	if (g_pad[0]->IsTrigger(enButtonUp)) {
		m_cursor--;
		m_cursor = max(min(m_cursor, 1), 0);
	}
	else if (g_pad[0]->IsTrigger(enButtonDown)) {
		m_cursor++;
		m_cursor = max(min(m_cursor, 1), 0);
	}
	else if (g_pad[0]->IsTrigger(enButtonA)) {
		m_resultState = enResultState_End;

		Game* game = FindGO<Game>("game");

		if (m_cursor == 0) {
			game->NotifyReset();
		}
		else if (m_cursor == 1) {
			NewGO<Title>(0, "title");
			DeleteGO(game);
			DeleteGO(this);
		}
	}
}

void Result::Render(RenderContext& rc)
{
	m_backGroundSpriteRender.Draw(rc);
	m_scoreFontRender.Draw(rc);
}