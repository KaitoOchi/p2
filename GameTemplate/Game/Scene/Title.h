#pragma once

/// <summary>
/// �^�C�g����ʁB
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
	FontRender m_menuFontRender;	//���j���[�����B
};

