#pragma once
#include "level2D/Level2D.h"
class Fade;

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
	/// <summary>
	/// ゲーム終了処理。
	/// </summary>
	void GameEnd();
	/// <summary>
	/// カーソルの更新処理。
	/// </summary>
	void CursorUpdate();

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

	Level2D						m_level2DRender;
	SpriteRender				m_backGroundSpriteRender;				//背景画像。
	SpriteRender				m_pressSpriteRender;					//リトライ、タイトル画像。
	SpriteRender				m_cursorSpriteRender;
	std::array<SpriteRender, 4> m_scoreSpriteRender;					//スコア画像。
	std::array<FontRender, 4>	m_scoreFontRender;						//スコア文字。
	ResultState					m_resultState = enResultState_FadeOut;	//リザルトステート。
	Fade*						m_fade = nullptr;						//フェード。
	std::array<Vector3, 2>		m_cursorPos;
	std::array<bool, 5>			m_isActive;								//表示状態が有効かどうか。
	bool						m_isFadeOut = false;					//フェード終了かどうか。
	int							m_cursor = 0;							//カーソル。
	int							m_scoreStepNum = 0;
	int							m_scoreShotNum = 0;
	float						m_timer = 0.0f;							//タイマー。
	float						m_scoreTime = 0.0f;
};

