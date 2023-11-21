#include "stdafx.h"
#include "PortalCamera.h"
#include "PortalFrame.h"

namespace
{
	const float TARGET_FORWARD = 10000.0f;			//注視点の前方向。
}

PortalCamera::PortalCamera()
{
	//ポータルフレームを初期化。
	for (int i = 0; i < PORTAL_NUM; i++) {
		m_portalFrame[i] = nullptr;
		m_angle[i] = 0.0f;
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

		//2つのポータルの角度を計算。
		//float portalAngle = acosf(m_portalFrame[thisPortalNum]->GetNormal().Dot(m_portalFrame[anotherPortalNum]->GetNormal()));

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

	//Vector3 cameraPos = diff;
	////注視点を回転させる。
	//cameraPos.x = cos(m_angle[portalNum]);
	//cameraPos.z = sin(m_angle[portalNum]);

	//cameraPos = m_portalFrame[1 - portalNum]->GetPosition() - cameraPos;

	//カメラの座標はもう一方のポータルの座標 - プレイヤーからポータルに向かうベクトルの反対。
	Vector3 cameraPos = m_portalFrame[1 - portalNum]->GetPosition() - diff;
	m_portalCamera[portalNum].SetPosition(cameraPos);
}

/// <summary>
/// ポータル用カメラの注視点を設定。
/// </summary>
void PortalCamera::SetCameraTarget(const int portalNum)
{
	//正規化したプレイヤーの注視点を求める。
	Vector3 playerTarget = g_camera3D->GetTarget() - m_portalFrame[portalNum]->GetPosition();
	playerTarget.Normalize();

	Vector3 targetPos = playerTarget;
	//注視点を回転させる。
	targetPos.x = cos(m_angle[portalNum]);
	targetPos.z = sin(m_angle[portalNum]);
	targetPos.Normalize();

	targetPos = m_portalFrame[1 - portalNum]->GetPosition() + (targetPos * TARGET_FORWARD);

	//カメラの注視点はもう一方のポータルの座標 + プレイヤーからポータルに向かうベクトル。
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
	}
}