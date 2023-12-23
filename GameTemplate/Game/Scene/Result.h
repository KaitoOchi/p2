#pragma once

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
	void SetScore(const int score)
	{
		m_score = score;
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

	SpriteRender	m_backGroundSpriteRender;				//�w�i�摜�B
	FontRender		m_scoreFontRender;						//�X�R�A�����B
	ResultState		m_resultState = enResultState_FadeOut;	//���U���g�X�e�[�g�B
	int				m_score = 0;							//�X�R�A�B
	int				m_cursor = 0;							//�J�[�\���B
	float			m_timer = 0.0f;							//�^�C�}�[�B
};

