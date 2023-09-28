#include "PortalEnginePreCompile.h"
#include "PostEffect.h"

namespace nsPortalEngine {

	PostEffect::PostEffect()
	{

	}

	PostEffect::~PostEffect()
	{

	}

	void PostEffect::Init(RenderTarget& mainRenderTarget)
	{
		m_bloom.Init(mainRenderTarget);
	}

	void PostEffect::Render(RenderContext& rc, RenderTarget& mainRenderTarget)
	{
		m_bloom.Render(rc, mainRenderTarget);


		//�����ŃG�t�F�N�g�h���[�B
		EffectEngine::GetInstance()->Draw();
	}
}