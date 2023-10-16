#include "PortalEnginePreCompile.h"
#include "PointLight.h"

namespace nsPortalEngine {

	void PointLight::Update(const int num)
	{
		//定数バッファに値を入れる。
		RenderingEngine::GetInstance()->GetLightCB().pointLig[num] = m_pointLight;
	}

}