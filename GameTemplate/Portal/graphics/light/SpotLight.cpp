#include "PortalEnginePreCompile.h"
#include "graphics/light/SpotLight.h"

namespace nsPortalEngine {

	void SpotLight::Update(const int num)
	{
		m_spotLight.posInView = m_spotLight.spPosition;
		g_camera3D->GetViewMatrix().Apply(m_spotLight.posInView);
		Matrix cameraRotInv = g_camera3D->GetCameraRotation();
		cameraRotInv.Inverse();
		cameraRotInv.Apply(m_spotLight.directionInView);

		//定数バッファに値を入れる。
		RenderingEngine::GetInstance()->GetLightCB().spotLig[num] = m_spotLight;
	}

}