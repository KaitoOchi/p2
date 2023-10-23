#include "stdafx.h"
#include "Fade.h"


namespace
{
	const float FADE_MAX = 1.0f;			//�t�F�[�h�̍ő�l�B
	const float FADE_MIN = 0.0f;			//�t�F�[�h�̍ŏ��l�B
	const float FADE_SPEED = 1.0f;			//�t�F�[�h���x�B
}

Fade::Fade()
{
}

Fade::~Fade()
{

}

bool Fade::Start()
{
	//�Ó]���̉摜
	m_spriteRender.Init("Assets/sprite/UI/black.dds", 1920.0f, 1080.0f);

	return true;
}

void Fade::Update()
{
	switch (m_state)
	{
		//�t�F�[�h�C�����Ȃ�
	case enState_FadeIn:

		m_alpha -= FADE_SPEED * g_gameTime->GetFrameDeltaTime();

		if (m_alpha <= FADE_MIN)
		{
			m_alpha = FADE_MIN;
			m_state = enState_Idle;
		}
		break;

		//�t�F�[�h�A�E�g���Ȃ�
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

	//�w�i�摜��ݒ�
	m_spriteRender.SetMulColor(alpha);
	m_spriteRender.Update();
}

void Fade::Render(RenderContext& rc)
{
	//�t�F�[�h���I�����Ă���Ȃ�
	if (m_alpha < FADE_MIN) {
		return;
	}

	//�w�i�摜�̕`��
	m_spriteRender.Draw(rc);
}