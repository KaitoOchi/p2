#include "PortalEnginePreCompile.h"
#include "graphics/light/SpotLight.h"

namespace nsPortalEngine {

	void SpotLight::Update()
	{
		RenderingEngine::GetInstance()->GetLightCB().spotLig = m_spotLight;
	}

}