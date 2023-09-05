#include "PortalEnginePreCompile.h"
#include "graphics/light/DirectionalLight.h"

namespace nsPortalEngine {

	void DirectionalLight::Update()
	{
		RenderingEngine::GetInstance()->GetLightCB().directionLig = m_directionLig;
	}

}