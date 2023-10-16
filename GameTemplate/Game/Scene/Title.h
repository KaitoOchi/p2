#pragma once

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
	FontRender m_menuFontRender;	//メニュー文字。
};

