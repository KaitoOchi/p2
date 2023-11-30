#include "PortalEnginePreCompile.h"
#include "PointLight.h"

namespace nsPortalEngine {

	void PointLight::Update(const int num)
	{
		m_pointLight.posInView = m_pointLight.ptPosition;
		g_camera3D->GetViewMatrix().Apply(m_pointLight.posInView);

		//�萔�o�b�t�@�ɒl������B
		RenderingEngine::GetInstance()->GetLightCB().pointLig[num] = m_pointLight;
	}

}