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
	//�|�C���^���w�肳��Ă��Ȃ��Ȃ�B
	if (m_portalFrame[0] == nullptr || m_portalFrame[1] == nullptr) {
		return;
	}

	for (int thisPortalNum = 0; thisPortalNum < PORTAL_NUM; thisPortalNum++) {

		//�\����Ԃ��L���łȂ��Ȃ�B
		if (!m_portalFrame[thisPortalNum]->GetIsEnable()) {
			break;
		}

		//��������̃|�[�^���ԍ��B
		int anotherPortalNum = 1 - thisPortalNum;

		//���̃|�[�^���̍��W�B
		Vector3 thisPortalPos = m_portalFrame[thisPortalNum]->GetPosition();
		//���̃|�[�^���̖@���B
		Vector3 thisPortalNormal = m_portalFrame[thisPortalNum]->GetNormal();
		//��������̃|�[�^���̍��W�B
		Vector3 anotherPortalPos = m_portalFrame[anotherPortalNum]->GetPosition();
		//��������̃|�[�^���̖@���B
		Vector3 anotherPortalNormal = m_portalFrame[anotherPortalNum]->GetNormal();

		//�v���C���[����|�[�^���Ɍ������x�N�g�����v�Z�B
		Vector3 diff = thisPortalPos - g_camera3D->GetPosition();
		diff.y = 0.0f;

		//�J�����̋ߕ��ʂ�ݒ�B
		m_portalCamera[thisPortalNum].SetNear(diff.Length());

		//�J�����̍��W��ݒ�B
		//�J�����̍��W�͂�������̃|�[�^���̍��W - �v���C���[����|�[�^���Ɍ������x�N�g���̔��΁B
		Vector3 cameraPos = diff * -1.0f;
		cameraPos += anotherPortalPos;
		m_portalCamera[thisPortalNum].SetPosition(cameraPos);

		//���K��������B
		diff.Normalize();

		//�J�����̒����_��ݒ�B
		//�J�����̒����_�͂�������̃|�[�^���̍��W + �v���C���[����|�[�^���Ɍ������x�N�g���B
		Vector3 targetPos = diff * 100.0f;
		targetPos += anotherPortalPos;

		m_portalCamera[thisPortalNum].SetTarget(targetPos);
		m_portalCamera[thisPortalNum].Update();
	}
}