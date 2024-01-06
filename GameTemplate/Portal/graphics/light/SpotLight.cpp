#include "PortalEnginePreCompile.h"
#include "graphics/light/SpotLight.h"
#include "../Game/Portal/PortalCamera.h"

namespace nsPortalEngine {

	void SpotLight::Update(const int num)
	{
		m_spotLight.posInViewMainCamera = m_spotLight.spPosition;
		m_spotLight.posInViewBluePortal = m_spotLight.spPosition;
		m_spotLight.posInViewRedPortal = m_spotLight.spPosition;
		g_camera3D->GetViewMatrix().Apply(m_spotLight.posInViewMainCamera);
		m_portalCamera->GetPortalCamera(0).GetViewMatrix().Apply(m_spotLight.posInViewBluePortal);
		m_portalCamera->GetPortalCamera(1).GetViewMatrix().Apply(m_spotLight.posInViewRedPortal);
		Matrix mainCameraRotInv = g_camera3D->GetCameraRotation();
		mainCameraRotInv.Inverse();
		mainCameraRotInv.Apply(m_spotLight.directionInViewMainCamera);
		Matrix bluePortalCameraRotInv = m_portalCamera->GetPortalCamera(0).GetCameraRotation();
		bluePortalCameraRotInv.Inverse();
		bluePortalCameraRotInv.Apply(m_spotLight.directionInViewBluePortal);
		Matrix redPortalCameraRotInv = m_portalCamera->GetPortalCamera(1).GetCameraRotation();
		redPortalCameraRotInv.Inverse();
		redPortalCameraRotInv.Apply(m_spotLight.directionInViewRedPortal);

		//定数バッファに値を入れる。
		RenderingEngine::GetInstance()->GetLightCB().spotLig[num] = m_spotLight;
	}

}