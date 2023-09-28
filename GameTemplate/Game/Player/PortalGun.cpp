#include "stdafx.h"
#include "PortalGun.h"


PortalGun::PortalGun()
{

}

PortalGun::~PortalGun()
{

}

bool PortalGun::Start()
{
	for (int i = 0; i < enPortalType_Num; i++) {

		//ポータルフレームを初期化。
		m_portalFrameModelRender[i].Init("Assets/modelData/stage/portalFrame.tkm");
		m_portalFrameModelRender[i].SetPosition(Vector3(-200.0f * i, 75.0f, -100.0f));
		m_portalFrameModelRender[i].Update();

		//テクスチャをポータル用レンダーターゲットに設定する。
		//m_portalFrameModelRender[i].GetModel().ChangeAlbedoMap("", RenderingEngine::GetInstance()->GetPortalRenderTarget(i).GetRenderTargetTexture());
	}

	return true;
}

void PortalGun::Update()
{

}

void PortalGun::SetPortal(const PortalType type, const Vector3& position, const Quaternion& rotation)
{
	//ポータルの種類を設定。
	int portalNum = static_cast<int>(type);
	
	//視点の前方向にポータルを設置。
	Vector3 portalPos = position + g_camera3D->GetForward() * 100.0f;

	//プレイヤーの回転に合わせる。
	Quaternion portalRot = rotation;

	//ポータルを設定。
	m_portalFrameModelRender[portalNum].SetPosition(portalPos);
	m_portalFrameModelRender[portalNum].SetRotation(portalRot);
	m_portalFrameModelRender[portalNum].Update();
}

void PortalGun::Render(RenderContext& rc)
{
	//ポータルを描画。
	m_portalFrameModelRender[0].Draw(rc);
	m_portalFrameModelRender[1].Draw(rc);
}