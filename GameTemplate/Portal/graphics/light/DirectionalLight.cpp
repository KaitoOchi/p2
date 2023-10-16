#include "PortalEnginePreCompile.h"
#include "graphics/light/DirectionalLight.h"

namespace nsPortalEngine {

	void DirectionalLight::Update()
	{
		//定数バッファに値を入れる。
		RenderingEngine::GetInstance()->GetLightCB().directionLig = m_directionLig;
	}

}