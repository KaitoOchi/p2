#pragma once
#include "level2D/Level2D.h"
class Fade;

/// <summary>
/// ���U���g�V�[���B
/// </summary>
class Result : public IGameObject
{
public:
	Result();
	~Result();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	/// <summary>
	/// �X�R�A��ݒ�B
	/// </summary>
	/// <param name="score"></param>
	void SetScore(
		const float time,
		const int stepNum,
		const int shotNum
	) {
		m_scoreTime = time;
		m_scoreStepNum = stepNum;
		m_scoreShotNum = shotNum;
	}

private:
	/// <summary>
	/// �t�F�[�h�A�E�g�����B
	/// </summary>
	void FadeOut();
	/// <summary>
	/// �X�R�A�̌v�Z�����B�B
	/// </summary>
	void CalcScore();
	/// <summary>
	/// ���[�h�Z���N�g�����B�B
	/// </summary>
	void Select();
	/// <summary>
	/// �Q�[���I�������B
	/// </summary>
	void GameEnd();
	/// <summary>
	/// �J�[�\���̍X�V�����B
	/// </summary>
	void CursorUpdate();

private:
	/// <summary>
	/// ���U���g�X�e�[�g�B
	/// </summary>
	enum ResultState
	{
		enResultState_FadeOut,
		enResultState_CalcScore,
		enResultState_Select,
		enResultState_End,
	};

	Level2D						m_level2DRender;
	SpriteRender				m_backGroundSpriteRender;				//�w�i�摜�B
	SpriteRender				m_pressSpriteRender;					//���g���C�A�^�C�g���摜�B
	SpriteRender				m_cursorSpriteRender;
	std::array<SpriteRender, 4> m_scoreSpriteRender;					//�X�R�A�摜�B
	std::array<FontRender, 4>	m_scoreFontRender;						//�X�R�A�����B
	ResultState					m_resultState = enResultState_FadeOut;	//���U���g�X�e�[�g�B
	Fade*						m_fade = nullptr;						//�t�F�[�h�B
	std::array<Vector3, 2>		m_cursorPos;
	std::array<bool, 5>			m_isActive;								//�\����Ԃ��L�����ǂ����B
	bool						m_isFadeOut = false;					//�t�F�[�h�I�����ǂ����B
	int							m_cursor = 0;							//�J�[�\���B
	int							m_scoreStepNum = 0;
	int							m_scoreShotNum = 0;
	float						m_timer = 0.0f;							//�^�C�}�[�B
	float						m_scoreTime = 0.0f;
};

