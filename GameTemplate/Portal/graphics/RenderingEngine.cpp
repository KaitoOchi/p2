#include "PortalEnginePreCompile.h"
#include "RenderingEngine.h"

namespace nsPortalEngine {

	RenderingEngine* RenderingEngine::m_instance = nullptr;


	RenderingEngine::RenderingEngine()
	{
		m_renderObjects.reserve(256);
	}

	RenderingEngine::~RenderingEngine()
	{

	}

	void RenderingEngine::Init()
	{

	}

	void RenderingEngine::Execute(RenderContext& rc)
	{
		DrawForwardRendering(rc);

		Draw2D(rc);

		m_renderObjects.clear();
	}

	void RenderingEngine::DrawForwardRendering(RenderContext& rc)
	{
		//•`‰æˆ—B
		for (auto& renderObj : m_renderObjects)
		{
			renderObj->OnForwardRender(rc);
		}
	}

	void RenderingEngine::Draw2D(RenderContext& rc)
	{
		//•`‰æˆ—B
		for (auto& renderObj : m_renderObjects)
		{
			renderObj->On2DRender(rc);
		}
	}
}