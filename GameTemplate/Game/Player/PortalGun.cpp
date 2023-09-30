#include "stdafx.h"
#include "PortalGun.h"

namespace
{
	const Vector3	PORTALGUN_POS = Vector3(0.0f, 70.0f, 0.0f);		//�|�[�^���K���̍��W�B
	const float		PORTALGUN_ROT_Y = 1.15f;						//�|�[�^���K���̏㉺�̉�]�ʁB
	const float		CURSOR_SIZE = 64.0f;							//�J�[�\���T�C�Y�B
}


PortalGun::PortalGun()
{

}

PortalGun::~PortalGun()
{

}

bool PortalGun::Start()
{
	//�|�[�^���K�����������B
	//m_portalGunModelRender.Init("Assets/modelData/portal/portalGun.tkm");
	//m_portalGunModelRender.SetPosition(PORTALGUN_POS);
	//m_portalGunModelRender.Update();

	for (int i = 0; i < enPortalType_Num; i++) {
		//�|�[�^���t���[�����������B
		m_portalFrameModelRender[i].Init("Assets/modelData/stage/portalFrame.tkm");
		m_portalFrameModelRender[i].SetPosition(Vector3(-200.0f * i, 75.0f, -100.0f));
		m_portalFrameModelRender[i].Update();

		//�e�N�X�`�����|�[�^���p�����_�[�^�[�Q�b�g�ɐݒ肷��B
		m_portalFrameModelRender[i].GetModel().ChangeAlbedoMap("", RenderingEngine::GetInstance()->GetPortalRenderTarget(i).GetRenderTargetTexture());
	}

	//�N���X�w�A���������B
	m_crossHairSpriteRender.Init("Assets/sprite/UI/Cursor.DDS", CURSOR_SIZE, CURSOR_SIZE);

	return true;
}

void PortalGun::Update()
{
	SetPortalGunModel();
}

void PortalGun::SetPortal(const PortalType type, const Vector3& position)
{
	//�J�����̑O�������擾�B
	Vector3 forward = g_camera3D->GetForward();

	//�|�[�^���̎�ނ�ݒ�B
	int portalNum = static_cast<int>(type);
	
	//���_�̑O�����Ƀ|�[�^����ݒu�B
	Vector3 portalPos = position + forward * 1000.0f;

	//�v���C���[�̉�]�ɍ��킹��B
	Quaternion portalRot;
	portalRot.SetRotationYFromDirectionXZ(forward);
	portalRot.AddRotationDegY(360.0f);

	//�|�[�^����ݒ�B
	m_portalFrameModelRender[portalNum].SetPosition(portalPos);
	m_portalFrameModelRender[portalNum].SetRotation(portalRot);
	m_portalFrameModelRender[portalNum].Update();
}

void PortalGun::SetPortalGunModel()
{
	//�J�����̑O�������擾�B
	Vector3 forward = g_camera3D->GetForward();

	//�|�[�^���K���̍��W��ݒ�B
	Vector3 portalGunPos = g_camera3D->GetPosition();

	//�|�[�^���K���̉�]��ݒ�B
	Quaternion portalGunRot;
	portalGunRot.SetRotationYFromDirectionXZ(forward);
	portalGunRot.AddRotationX(-forward.y * PORTALGUN_ROT_Y);

	//�|�[�^���K����ݒ�B
	//m_portalGunModelRender.SetPosition(portalGunPos);
	//m_portalGunModelRender.SetRotation(portalGunRot);
	//m_portalGunModelRender.Update();
}

void PortalGun::Render(RenderContext& rc)
{
	//�|�[�^���K����`��B
	//m_portalGunModelRender.Draw(rc);

	//�|�[�^����`��B
	m_portalFrameModelRender[0].Draw(rc);
	m_portalFrameModelRender[1].Draw(rc);

	//�N���X�w�A�̕`��B
	m_crossHairSpriteRender.Draw(rc);
}