#include "PortalEnginePreCompile.h"
#include "graphics/light/SpotLight.h"

namespace nsPortalEngine {

	void SpotLight::Update(const int num)
	{
		RenderingEngine::GetInstance()->GetLightCB().spotLig[num] = m_spotLight;
	}

}