#include "stdafx.h"
#include "PortalCamera.h"
#include "PortalFrame.h"

namespace
{
	const float TARGET_FORWARD = 10000.0f;			//�����_�̑O�����B
}

PortalCamera::PortalCamera()
{
	//�|�[�^���t���[�����������B
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
/// �|�[�^���p�J�����̐ݒ�B
/// </summary>
void PortalCamera::SetPortalCamera()
{
	if (g_pad[0]->IsTrigger(enButtonB)) {
		int a = 0;
		a = 0;
	}

	//�|�C���^���w�肳��Ă��Ȃ��Ȃ�B
	if (m_portalFrame[0] == nullptr || m_portalFrame[1] == nullptr) {
		return;
	}

	//���ꂼ��̃J���������X�V����B
	for (int thisPortalNum = 0; thisPortalNum < PORTAL_NUM; thisPortalNum++) {

		//�\����Ԃ��L���łȂ��Ȃ�B
		if (!m_portalFrame[thisPortalNum]->GetIsEnable()) {
			break;
		}

		//2�̃|�[�^���̊p�x���v�Z�B
		//float portalAngle = acosf(m_portalFrame[thisPortalNum]->GetNormal().Dot(m_portalFrame[anotherPortalNum]->GetNormal()));

		//�J�����̍��W��ݒ�B
		SetCameraPosition(thisPortalNum);
		//�J�����̒����_��ݒ�B
		SetCameraTarget(thisPortalNum);

		m_portalCamera[thisPortalNum].Update();
	}
}

/// <summary>
/// �|�[�^���p�J�����̍��W��ݒ�B
/// </summary>
void PortalCamera::SetCameraPosition(const int portalNum)
{
	//�v���C���[����|�[�^���Ɍ������x�N�g�����v�Z�B
	Vector3 diff = m_portalFrame[portalNum]->GetPosition() - g_camera3D->GetPosition();
	diff.y = 0.0f;

	//Vector3 cameraPos = diff;
	////�����_����]������B
	//cameraPos.x = cos(m_angle[portalNum]);
	//cameraPos.z = sin(m_angle[portalNum]);

	//cameraPos = m_portalFrame[1 - portalNum]->GetPosition() - cameraPos;

	//�J�����̍��W�͂�������̃|�[�^���̍��W - �v���C���[����|�[�^���Ɍ������x�N�g���̔��΁B
	Vector3 cameraPos = m_portalFrame[1 - portalNum]->GetPosition() - diff;
	m_portalCamera[portalNum].SetPosition(cameraPos);
}

/// <summary>
/// �|�[�^���p�J�����̒����_��ݒ�B
/// </summary>
void PortalCamera::SetCameraTarget(const int portalNum)
{
	//���K�������v���C���[�̒����_�����߂�B
	Vector3 playerTarget = g_camera3D->GetTarget() - m_portalFrame[portalNum]->GetPosition();
	playerTarget.Normalize();

	Vector3 targetPos = playerTarget;
	//�����_����]������B
	targetPos.x = cos(m_angle[portalNum]);
	targetPos.z = sin(m_angle[portalNum]);
	targetPos.Normalize();

	targetPos = m_portalFrame[1 - portalNum]->GetPosition() + (targetPos * TARGET_FORWARD);

	//�J�����̒����_�͂�������̃|�[�^���̍��W + �v���C���[����|�[�^���Ɍ������x�N�g���B
	m_portalCamera[portalNum].SetTarget(targetPos);

}

void PortalCamera::UpdateAngle()
{
	for(int portalNum = 0; portalNum < PORTAL_NUM; portalNum++){
		//���̃|�[�^���̖@���B
		Vector3 thisPortalNormal = m_portalFrame[portalNum]->GetNormal();
		//��������̃|�[�^���̖@���B
		Vector3 anotherPortalNormal = m_portalFrame[1 - portalNum]->GetNormal();

		//2�̃|�[�^���̊p�x���v�Z�B
		m_angle[portalNum] = acosf(thisPortalNormal.Dot(anotherPortalNormal));

		//�O�ς����߁Ay�����g���ā}��␳����B
		Vector3 cross = thisPortalNormal;
		cross.Cross(anotherPortalNormal);
		if (cross.y <= -1.0f) {
			m_angle[portalNum] *= -1.0f;
		}
	}
}