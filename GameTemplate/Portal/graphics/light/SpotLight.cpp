#include "PortalEnginePreCompile.h"
#include "graphics/light/SpotLight.h"

namespace nsPortalEngine {

	void SpotLight::Update(const int num)
	{
		//定数バッファに値を入れる。
		RenderingEngine::GetInstance()->GetLightCB().spotLig[num] = m_spotLight;
	}

}