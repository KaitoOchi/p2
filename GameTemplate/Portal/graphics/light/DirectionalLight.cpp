#include "PortalEnginePreCompile.h"
#include "graphics/light/DirectionalLight.h"

namespace nsPortalEngine {

	void DirectionalLight::Update()
	{
		//�萔�o�b�t�@�ɒl������B
		RenderingEngine::GetInstance()->GetLightCB().directionLig = m_directionLig;
	}

}