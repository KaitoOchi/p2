#include "PortalEnginePreCompile.h"
#include "PostEffect.h"

namespace nsPortalEngine {

	PostEffect::PostEffect()
	{
		m_bloom = new Bloom;
		m_dof = new DoF;
		m_postEffectType.reserve(4);
	}

	PostEffect::~PostEffect()
	{
		delete m_bloom;
		delete m_dof;
		m_postEffectType.clear();
	}

	void PostEffect::Init(RenderTarget* mainRenderTarget, RenderTarget& zprepassRenderTarget)
	{
		m_bloom->Init(mainRenderTarget, zprepassRenderTarget);
		m_postEffectType.emplace_back(m_bloom);

		m_dof->Init(mainRenderTarget, zprepassRenderTarget);
		m_postEffectType.emplace_back(m_dof);
	}

	void PostEffect::Render(RenderContext& rc, RenderTarget* mainRenderTarget)
	{
		m_bloom->Render(rc, mainRenderTarget);

		RenderTarget* rts[] = {
			&mainRenderTarget[0],
			&mainRenderTarget[1],
			&mainRenderTarget[2]
		};

		RenderingEngine::GetInstance()->SetMainRenderTargetAndDepthStencilBuffer(rc);

		rc.WaitUntilToPossibleSetRenderTarget(mainRenderTarget[0]);

		//ここでエフェクトドロー。
		EffectEngine::GetInstance()->Draw();

		rc.WaitUntilFinishDrawingToRenderTarget(mainRenderTarget[0]);

		m_dof->Render(rc, mainRenderTarget);
	}
}