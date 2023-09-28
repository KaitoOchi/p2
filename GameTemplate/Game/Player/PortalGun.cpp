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

		//�|�[�^���t���[�����������B
		m_portalFrameModelRender[i].Init("Assets/modelData/stage/portalFrame.tkm");
		m_portalFrameModelRender[i].SetPosition(Vector3(-200.0f * i, 75.0f, -100.0f));
		m_portalFrameModelRender[i].Update();

		//�e�N�X�`�����|�[�^���p�����_�[�^�[�Q�b�g�ɐݒ肷��B
		//m_portalFrameModelRender[i].GetModel().ChangeAlbedoMap("", RenderingEngine::GetInstance()->GetPortalRenderTarget(i).GetRenderTargetTexture());
	}

	return true;
}

void PortalGun::Update()
{

}

void PortalGun::SetPortal(const PortalType type, const Vector3& position, const Quaternion& rotation)
{
	//�|�[�^���̎�ނ�ݒ�B
	int portalNum = static_cast<int>(type);
	
	//���_�̑O�����Ƀ|�[�^����ݒu�B
	Vector3 portalPos = position + g_camera3D->GetForward() * 100.0f;

	//�v���C���[�̉�]�ɍ��킹��B
	Quaternion portalRot = rotation;

	//�|�[�^����ݒ�B
	m_portalFrameModelRender[portalNum].SetPosition(portalPos);
	m_portalFrameModelRender[portalNum].SetRotation(portalRot);
	m_portalFrameModelRender[portalNum].Update();
}

void PortalGun::Render(RenderContext& rc)
{
	//�|�[�^����`��B
	m_portalFrameModelRender[0].Draw(rc);
	m_portalFrameModelRender[1].Draw(rc);
}