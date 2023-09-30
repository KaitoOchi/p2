#include "stdafx.h"
#include "PortalGun.h"

namespace
{
	const Vector3	PORTALGUN_POS = Vector3(0.0f, 70.0f, 0.0f);		//ポータルガンの座標。
	const float		PORTALGUN_ROT_Y = 1.15f;						//ポータルガンの上下の回転量。
	const float		CURSOR_SIZE = 64.0f;							//カーソルサイズ。
}


PortalGun::PortalGun()
{

}

PortalGun::~PortalGun()
{

}

bool PortalGun::Start()
{
	//ポータルガンを初期化。
	//m_portalGunModelRender.Init("Assets/modelData/portal/portalGun.tkm");
	//m_portalGunModelRender.SetPosition(PORTALGUN_POS);
	//m_portalGunModelRender.Update();

	for (int i = 0; i < enPortalType_Num; i++) {
		//ポータルフレームを初期化。
		m_portalFrameModelRender[i].Init("Assets/modelData/stage/portalFrame.tkm");
		m_portalFrameModelRender[i].SetPosition(Vector3(-200.0f * i, 75.0f, -100.0f));
		m_portalFrameModelRender[i].Update();

		//テクスチャをポータル用レンダーターゲットに設定する。
		m_portalFrameModelRender[i].GetModel().ChangeAlbedoMap("", RenderingEngine::GetInstance()->GetPortalRenderTarget(i).GetRenderTargetTexture());
	}

	//クロスヘアを初期化。
	m_crossHairSpriteRender.Init("Assets/sprite/UI/Cursor.DDS", CURSOR_SIZE, CURSOR_SIZE);

	return true;
}

void PortalGun::Update()
{
	SetPortalGunModel();
}

void PortalGun::SetPortal(const PortalType type, const Vector3& position)
{
	//カメラの前方向を取得。
	Vector3 forward = g_camera3D->GetForward();

	//ポータルの種類を設定。
	int portalNum = static_cast<int>(type);
	
	//視点の前方向にポータルを設置。
	Vector3 portalPos = position + forward * 1000.0f;

	//プレイヤーの回転に合わせる。
	Quaternion portalRot;
	portalRot.SetRotationYFromDirectionXZ(forward);
	portalRot.AddRotationDegY(360.0f);

	//ポータルを設定。
	m_portalFrameModelRender[portalNum].SetPosition(portalPos);
	m_portalFrameModelRender[portalNum].SetRotation(portalRot);
	m_portalFrameModelRender[portalNum].Update();
}

void PortalGun::SetPortalGunModel()
{
	//カメラの前方向を取得。
	Vector3 forward = g_camera3D->GetForward();

	//ポータルガンの座標を設定。
	Vector3 portalGunPos = g_camera3D->GetPosition();

	//ポータルガンの回転を設定。
	Quaternion portalGunRot;
	portalGunRot.SetRotationYFromDirectionXZ(forward);
	portalGunRot.AddRotationX(-forward.y * PORTALGUN_ROT_Y);

	//ポータルガンを設定。
	//m_portalGunModelRender.SetPosition(portalGunPos);
	//m_portalGunModelRender.SetRotation(portalGunRot);
	//m_portalGunModelRender.Update();
}

void PortalGun::Render(RenderContext& rc)
{
	//ポータルガンを描画。
	//m_portalGunModelRender.Draw(rc);

	//ポータルを描画。
	m_portalFrameModelRender[0].Draw(rc);
	m_portalFrameModelRender[1].Draw(rc);

	//クロスヘアの描画。
	m_crossHairSpriteRender.Draw(rc);
}