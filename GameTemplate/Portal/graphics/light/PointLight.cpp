#include "PortalEnginePreCompile.h"
#include "PointLight.h"
#include "../Game/Portal/PortalCamera.h"

namespace nsPortalEngine {

	void PointLight::Update(const int num)
	{
		m_pointLight.posInViewMainCamera = m_pointLight.ptPosition;
		m_pointLight.posInViewBluePortal = m_pointLight.ptPosition;
		m_pointLight.posInViewRedPortal = m_pointLight.ptPosition;
		g_camera3D->GetViewMatrix().Apply(m_pointLight.posInViewMainCamera);
		m_portalCamera->GetPortalCamera(0).GetViewMatrix().Apply(m_pointLight.posInViewBluePortal);
		m_portalCamera->GetPortalCamera(1).GetViewMatrix().Apply(m_pointLight.posInViewRedPortal);

		//定数バッファに値を入れる。
		RenderingEngine::GetInstance()->GetLightCB().pointLig[num] = m_pointLight;
	}

}