#include "PortalEnginePreCompile.h"
#include "graphics/light/SpotLight.h"

namespace nsPortalEngine {

	void SpotLight::Update(const int num)
	{
		//�萔�o�b�t�@�ɒl������B
		RenderingEngine::GetInstance()->GetLightCB().spotLig[num] = m_spotLight;
	}

}