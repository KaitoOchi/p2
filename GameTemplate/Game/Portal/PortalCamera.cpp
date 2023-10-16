#include "stdafx.h"
#include "PortalCamera.h"
#include "PortalFrame.h"

PortalCamera::PortalCamera()
{
	for (int i = 0; i < PORTAL_NUM; i++) {
		m_portalFrame[i] = nullptr;
	}
}

PortalCamera::~PortalCamera()
{

}

bool PortalCamera::Start()
{
	return true;
}

void PortalCamera::Update()
{
	SetPortalCamera();
}

void PortalCamera::SetPortalCamera()
{
	//ポインタが指定されていないなら。
	if (m_portalFrame[0] == nullptr || m_portalFrame[1] == nullptr) {
		return;
	}

	for (int thisPortalNum = 0; thisPortalNum < PORTAL_NUM; thisPortalNum++) {

		//表示状態が有効でないなら。
		if (!m_portalFrame[thisPortalNum]->GetIsEnable()) {
			break;
		}

		//もう一方のポータル番号。
		int anotherPortalNum = 1 - thisPortalNum;

		//このポータルの座標。
		Vector3 thisPortalPos = m_portalFrame[thisPortalNum]->GetPosition();
		//このポータルの法線。
		Vector3 thisPortalNormal = m_portalFrame[thisPortalNum]->GetNormal();
		//もう一方のポータルの座標。
		Vector3 anotherPortalPos = m_portalFrame[anotherPortalNum]->GetPosition();
		//もう一方のポータルの法線。
		Vector3 anotherPortalNormal = m_portalFrame[anotherPortalNum]->GetNormal();

		//プレイヤーからポータルに向かうベクトルを計算。
		Vector3 diff = thisPortalPos - g_camera3D->GetPosition();
		diff.y = 0.0f;

		//カメラの近平面を設定。
		m_portalCamera[thisPortalNum].SetNear(diff.Length());

		//カメラの座標を設定。
		//カメラの座標はもう一方のポータルの座標 - プレイヤーからポータルに向かうベクトルの反対。
		Vector3 cameraPos = diff * -1.0f;
		cameraPos += anotherPortalPos;
		m_portalCamera[thisPortalNum].SetPosition(cameraPos);

		//正規化をする。
		diff.Normalize();

		//カメラの注視点を設定。
		//カメラの注視点はもう一方のポータルの座標 + プレイヤーからポータルに向かうベクトル。
		Vector3 targetPos = diff * 100.0f;
		targetPos += anotherPortalPos;

		m_portalCamera[thisPortalNum].SetTarget(targetPos);
		m_portalCamera[thisPortalNum].Update();
	}
}