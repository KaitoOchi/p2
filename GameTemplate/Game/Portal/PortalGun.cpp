#include "stdafx.h"
#include "PortalGun.h"
#include "PortalCamera.h"

namespace
{
	const Vector3	PORTALGUN_POS = Vector3(0.0f, 50.0f, 0.0f);		//ポータルガンの座標。
	const float		PORTALGUN_ROT_Y = 1.15f;						//ポータルガンの上下の回転量。
	const float		CURSOR_SIZE = 64.0f;							//カーソルサイズ。
	const float		RAY_LENGTH = 5000.0f;							//レイの長さ。
}


PortalGun::PortalGun()
{
	for (int i = 0; i < PORTAL_NUM; i++) {
		m_portalFrame[i] = nullptr;
	}
}

PortalGun::~PortalGun()
{

}

bool PortalGun::Start()
{
	//ポータルを検索。
	std::vector<PortalFrame*> portalFrames;
	portalFrames = FindGOs<PortalFrame>("portalFrame");
	//ポータルカメラを検索。
	m_portalCamera = FindGO<PortalCamera>("portalCamera");
	
	//ポインタに格納。
	for (int i = 0; i < PORTAL_NUM; i++) {
		m_portalFrame[i] = portalFrames[i];
		m_portalCamera->SetPortalFramePointer(m_portalFrame[i], i);
	}

	//ポータルガンを初期化。
	//m_portalGunModelRender.Init("Assets/modelData/portal/portalGun.tkm");
	//m_portalGunModelRender.SetPosition(PORTALGUN_POS);
	//m_portalGunModelRender.Update();

	//クロスヘアを初期化。
	m_crossHairSpriteRender.Init("Assets/sprite/UI/Cursor.DDS", CURSOR_SIZE, CURSOR_SIZE);

	return true;
}

void PortalGun::Update()
{
	//青ポータルを設置。
	if (g_pad[0]->IsTrigger(enButtonLB1)) {
		SetPortal(PortalFrame::enPortalType_Blue);
	}
	//赤ポータルを設置。
	else if (g_pad[0]->IsTrigger(enButtonRB1)) {
		SetPortal(PortalFrame::enPortalType_Red);
	}

	//SetPortalGunModel();
}

void PortalGun::SetPortal(const PortalFrame::PortalType type)
{
	//カメラの前方向を取得。
	Vector3 forward = g_camera3D->GetForward();

	//レイの視点と終点を設定。
	Vector3 rayStartPos = g_camera3D->GetPosition();
	Vector3 rayEndPos = rayStartPos + (forward * RAY_LENGTH);
	//当たったオブジェクトの詳細。
	PhysicsWorld::RayHitObject hit;

	//レイを飛ばす。
	bool isHit = PhysicsWorld::GetInstance()->RayTest(rayStartPos, rayEndPos, hit);

	//当たらなかった場合、何もしない。
	if (isHit == false) {
		return;
	}
	
	//当たったオブジェクトの座標に合わせて設置する。
	Vector3 portalPos = hit.position;
	//ポータルを設置。
	int num = static_cast<int>(type);
	m_portalFrame[num]->SetPortalFrame(portalPos, hit.normal);
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
	m_portalGunModelRender.SetPosition(portalGunPos);
	m_portalGunModelRender.SetRotation(portalGunRot);
	m_portalGunModelRender.Update();
}

void PortalGun::Render(RenderContext& rc)
{
	//ポータルガンを描画。
	//m_portalGunModelRender.Draw(rc);

	//クロスヘアの描画。
	m_crossHairSpriteRender.Draw(rc);
}