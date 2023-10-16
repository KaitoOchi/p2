#include "PortalEnginePreCompile.h"
#include "graphics/SpriteRender.h"

namespace nsPortalEngine {

	void SpriteRender::Init(const char* filePath, const float w, const float h, AlphaBlendMode alpha)
	{
		SpriteInitData initData;
		//DDSファイルパスを指定する。
		initData.m_ddsFilePath[0] = filePath;
		//シェーダーのファイルパスを指定する。
		initData.m_fxFilePath = "Assets/shader/sprite.fx";
		//スプライトの幅と高さを指定する。
		initData.m_width = static_cast<UINT>(w);
		initData.m_height = static_cast<UINT>(h);
		initData.m_alphaBlendMode = alpha;

		//スプライトを初期化。
		m_sprite.Init(initData);

	}

	void SpriteRender::Draw(RenderContext& rc)
	{
		RenderingEngine::GetInstance()->AddRenderObject(this);
	}

	void SpriteRender::On2DRender(RenderContext& rc)
	{
		m_sprite.Draw(rc);
	}
}