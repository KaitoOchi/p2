#pragma once

/// <summary>
/// リザルトシーン。
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
	/// スコアを設定。
	/// </summary>
	/// <param name="score"></param>
	void SetScore(const int score)
	{
		m_score = score;
	}

private:
	/// <summary>
	/// フェードアウト処理。
	/// </summary>
	void FadeOut();
	/// <summary>
	/// スコアの計算処理。。
	/// </summary>
	void CalcScore();
	/// <summary>
	/// モードセレクト処理。。
	/// </summary>
	void Select();

private:
	/// <summary>
	/// リザルトステート。
	/// </summary>
	enum ResultState
	{
		enResultState_FadeOut,
		enResultState_CalcScore,
		enResultState_Select,
		enResultState_End,
	};

	SpriteRender	m_backGroundSpriteRender;				//背景画像。
	FontRender		m_scoreFontRender;						//スコア文字。
	ResultState		m_resultState = enResultState_FadeOut;	//リザルトステート。
	int				m_score = 0;							//スコア。
	int				m_cursor = 0;							//カーソル。
	float			m_timer = 0.0f;							//タイマー。
};

