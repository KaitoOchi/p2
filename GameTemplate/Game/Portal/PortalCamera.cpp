#include "stdafx.h"
#include "PortalCamera.h"
#include "PortalFrame.h"

PortalCamera::PortalCamera()
{
	//ポータルフレームを初期化。
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

/// <summary>
/// ポータル用カメラの設定。
/// </summary>
void PortalCamera::SetPortalCamera()
{
	if (g_pad[0]->IsTrigger(enButtonB)) {
		int a = 0;
		a = 0;
	}

	//ポインタが指定されていないなら。
	if (m_portalFrame[0] == nullptr || m_portalFrame[1] == nullptr) {
		return;
	}

	//それぞれのカメラ情報を更新する。
	for (int thisPortalNum = 0; thisPortalNum < PORTAL_NUM; thisPortalNum++) {

		//表示状態が有効でないなら。
		if (!m_portalFrame[thisPortalNum]->GetIsEnable()) {
			break;
		}

		//カメラの座標を設定。
		SetCameraPosition(thisPortalNum);
		//カメラの注視点を設定。
		SetCameraTarget(thisPortalNum);

		m_portalCamera[thisPortalNum].Update();
	}
}

/// <summary>
/// ポータル用カメラの座標を設定。
/// </summary>
void PortalCamera::SetCameraPosition(const int portalNum)
{
	//プレイヤーからポータルに向かうベクトルを計算。
	Vector3 diff = m_portalFrame[portalNum]->GetPosition() - g_camera3D->GetPosition();
	diff.y = 0.0f;

	//注視点を回転させる。
	m_rotation[portalNum].Apply(diff);

	//カメラの座標はもう一方のポータルの座標 - プレイヤーからポータルに向かうベクトルの反対。
	Vector3 cameraPos = m_portalFrame[1 - portalNum]->GetPosition() + diff;
	m_portalCamera[portalNum].SetPosition(cameraPos);
}

/// <summary>
/// ポータル用カメラの注視点を設定。
/// </summary>
void PortalCamera::SetCameraTarget(const int portalNum)
{
	//ポータルからプレイヤーの注視点に伸びるベクトルを計算。
	Vector3 playerTarget = g_camera3D->GetPosition() - g_camera3D->GetTarget();

	//注視点を回転させる。
	m_rotation[portalNum].Apply(playerTarget);
	playerTarget.y *= -1.0f;

	//カメラの注視点はもう一方のポータルの座標 + プレイヤーからポータルに向かうベクトル。
	Vector3 targetPos = m_portalFrame[1 - portalNum]->GetPosition() + playerTarget;
	m_portalCamera[portalNum].SetTarget(targetPos);
}

void PortalCamera::UpdateAngle()
{
	for(int portalNum = 0; portalNum < PORTAL_NUM; portalNum++){
		//このポータルの法線。
		Vector3 thisPortalNormal = m_portalFrame[portalNum]->GetNormal();
		//もう一方のポータルの法線。
		Vector3 anotherPortalNormal = m_portalFrame[1 - portalNum]->GetNormal();

		//2つのポータルの角度を計算。
		m_angle[portalNum] = acosf(thisPortalNormal.Dot(anotherPortalNormal));

		//外積を求め、y軸を使って±を補正する。
		Vector3 cross = thisPortalNormal;
		cross.Cross(anotherPortalNormal);
		if (cross.y <= -1.0f) {
			m_angle[portalNum] *= -1.0f;
		}

		//2つのポータルの回転を計算。
		m_rotation[portalNum].SetRotation(thisPortalNormal, anotherPortalNormal);
	}
}