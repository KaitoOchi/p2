#include "stdafx.h"
#include "Title.h"


Title::Title()
{

}

Title::~Title()
{

}

bool Title::Start()
{
	m_menuFontRender.SetText(L"new game \nload game \nsetting \nexit");

	return true;
}

void Title::Update()
{

}

void Title::Render(RenderContext& rc)
{
	m_menuFontRender.Draw(rc);
}