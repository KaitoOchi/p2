#include "PortalEnginePreCompile.h"
#include "PointLight.h"

namespace nsPortalEngine {

	void PointLight::Update(const int num)
	{
		//�萔�o�b�t�@�ɒl������B
		RenderingEngine::GetInstance()->GetLightCB().pointLig[num] = m_pointLight;
	}

}