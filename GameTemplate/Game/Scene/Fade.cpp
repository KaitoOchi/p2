#include "stdafx.h"
#include "Fade.h"


namespace
{
	const float FADE_MAX = 1.0f;			//フェードの最大値。
	const float FADE_MIN = 0.0f;			//フェードの最小値。
	const float FADE_SPEED = 1.0f;			//フェード速度。
}

Fade::Fade()
{
}

Fade::~Fade()
{

}

bool Fade::Start()
{
	//暗転時の画像
	m_spriteRender.Init("Assets/sprite/UI/black.dds", 1920.0f, 1080.0f);

	return true;
}

void Fade::Update()
{
	switch (m_state)
	{
		//フェードイン中なら
	case enState_FadeIn:

		m_alpha -= FADE_SPEED * g_gameTime->GetFrameDeltaTime();

		if (m_alpha <= FADE_MIN)
		{
			m_alpha = FADE_MIN;
			m_state = enState_Idle;
		}
		break;

		//フェードアウト中なら
	case enState_FadeOut:

		m_alpha += FADE_SPEED * g_gameTime->GetFrameDeltaTime();

		if (m_alpha >= FADE_MAX)
		{
			m_alpha = FADE_MAX;
			m_state = enState_Idle;
		}
		break;

	case enState_Idle:
		break;
	}

	if (m_alpha < FADE_MIN) {
		return;
	}

	SpriteUpdate();
}

void Fade::SpriteUpdate()
{
	Vector4 alpha = Vector4::White;
	alpha.a = 0.0f;

	//背景画像を設定
	m_spriteRender.SetMulColor(alpha);
	m_spriteRender.Update();
}

void Fade::Render(RenderContext& rc)
{
	//フェードが終了しているなら
	if (m_alpha < FADE_MIN) {
		return;
	}

	//背景画像の描画
	m_spriteRender.Draw(rc);
}