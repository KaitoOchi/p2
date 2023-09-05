#include "PortalEnginePreCompile.h"
#include "PointLight.h"

namespace nsPortalEngine {

	void PointLight::Update()
	{
		RenderingEngine::GetInstance()->GetLightCB().pointLig = m_pointLight;
	}

}