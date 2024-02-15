#pragma once
#include "level2D/Level2D.h"
class Fade;
class Stage;

/// <summary>
/// タイトル画面。
/// </summary>
class Title : public IGameObject
{
public:
	Title();
	~Title();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

private:
	/// <summary>
	/// カーソルの更新処理。
	/// </summary>
	void CursorUpdate();

	/// <summary>
	/// カメラの更新処理。
	/// </summary>
	void CameraUpdate();

private:
	/// <summary>
	/// タイトルステート。
	/// </summary>
	enum TitleState
	{
		enState_Title,
		enState_Guide,
		enState_Exit,
	};

private:
	Level2D						m_level2DRender;		//2Dレベル。
	SpriteRender				m_guideSpriteRender;	//ガイド画像。
	std::array<SpriteRender, 3> m_menuSpriteRender;		//メニュー画像。
	SpriteRender				m_cursorSpriteRender;	//カーソル画像。
	Fade*						m_fade = nullptr;		//フェード。
	Stage*						m_stage = nullptr;		//ステージ。
	TitleState					m_state = enState_Title;//タイトルステート。
	Vector3						m_cameraTarget;			//カメラ注視点。
	bool						m_isInited = false;		//レベルの初期化が完了したか。
	bool						m_isFadeOut = false;	//フェードアウト中かどうか。
	int							m_cursor = 0;			//カーソル番号。
	float						m_cameraTimer = 0.0f;	//タイマー。

};

