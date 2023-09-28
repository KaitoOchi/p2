#include "PortalEnginePreCompile.h"
#include "PointLight.h"

namespace nsPortalEngine {

	void PointLight::Update(const int num)
	{
		RenderingEngine::GetInstance()->GetLightCB().pointLig[num] = m_pointLight;
	}

}