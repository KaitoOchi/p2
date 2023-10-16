#include "PortalEnginePreCompile.h"
#include "graphics/SpriteRender.h"

namespace nsPortalEngine {

	void SpriteRender::Init(const char* filePath, const float w, const float h, AlphaBlendMode alpha)
	{
		SpriteInitData initData;
		//DDS�t�@�C���p�X���w�肷��B
		initData.m_ddsFilePath[0] = filePath;
		//�V�F�[�_�[�̃t�@�C���p�X���w�肷��B
		initData.m_fxFilePath = "Assets/shader/sprite.fx";
		//�X�v���C�g�̕��ƍ������w�肷��B
		initData.m_width = static_cast<UINT>(w);
		initData.m_height = static_cast<UINT>(h);
		initData.m_alphaBlendMode = alpha;

		//�X�v���C�g���������B
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