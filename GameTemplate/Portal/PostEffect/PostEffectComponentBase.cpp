#include "PortalEnginePreCompile.h"
#include "PostEffectComponentBase.h"

namespace nsPortalEngine
{
	PostEffectComponentBase::PostEffectComponentBase()
	{

	}

	PostEffectComponentBase::~PostEffectComponentBase()
	{

	}

	void PostEffectComponentBase::Init(RenderTarget& mainRenderTarget, RenderTarget& zprepassRenderTarget)
	{
		OnInit(mainRenderTarget, zprepassRenderTarget);
	}

	void PostEffectComponentBase::Render(RenderContext& rc, RenderTarget& mainRenderTarget)
	{
		if (!m_isEnable) {
			return;
		}

		//�`�揈���B
		OnRender(rc, mainRenderTarget);
	}
}