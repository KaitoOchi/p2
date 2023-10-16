#include "stdafx.h"
#include "PortalGun.h"
#include "PortalCamera.h"

namespace
{
	const Vector3	PORTALGUN_POS = Vector3(0.0f, 50.0f, 0.0f);		//�|�[�^���K���̍��W�B
	const float		PORTALGUN_ROT_Y = 1.15f;						//�|�[�^���K���̏㉺�̉�]�ʁB
	const float		CURSOR_SIZE = 64.0f;							//�J�[�\���T�C�Y�B
	const float		RAY_LENGTH = 5000.0f;							//���C�̒����B
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
	//�|�[�^���������B
	std::vector<PortalFrame*> portalFrames;
	portalFrames = FindGOs<PortalFrame>("portalFrame");
	//�|�[�^���J�����������B
	m_portalCamera = FindGO<PortalCamera>("portalCamera");
	
	//�|�C���^�Ɋi�[�B
	for (int i = 0; i < PORTAL_NUM; i++) {
		m_portalFrame[i] = portalFrames[i];
		m_portalCamera->SetPortalFramePointer(m_portalFrame[i], i);
	}

	//�|�[�^���K�����������B
	//m_portalGunModelRender.Init("Assets/modelData/portal/portalGun.tkm");
	//m_portalGunModelRender.SetPosition(PORTALGUN_POS);
	//m_portalGunModelRender.Update();

	//�N���X�w�A���������B
	m_crossHairSpriteRender.Init("Assets/sprite/UI/Cursor.DDS", CURSOR_SIZE, CURSOR_SIZE);

	return true;
}

void PortalGun::Update()
{
	//�|�[�^����ݒu�B
	if (g_pad[0]->IsTrigger(enButtonLB1)) {
		SetPortal(PortalFrame::enPortalType_Blue);
	}
	//�ԃ|�[�^����ݒu�B
	else if (g_pad[0]->IsTrigger(enButtonRB1)) {
		SetPortal(PortalFrame::enPortalType_Red);
	}

	//SetPortalGunModel();
}

void PortalGun::SetPortal(const PortalFrame::PortalType type)
{
	//�J�����̑O�������擾�B
	Vector3 forward = g_camera3D->GetForward();

	//���C�̎��_�ƏI�_��ݒ�B
	Vector3 rayStartPos = g_camera3D->GetPosition();
	Vector3 rayEndPos = rayStartPos + (forward * RAY_LENGTH);
	//���������I�u�W�F�N�g�̏ڍׁB
	PhysicsWorld::RayHitObject hit;

	//���C���΂��B
	bool isHit = PhysicsWorld::GetInstance()->RayTest(rayStartPos, rayEndPos, hit);

	//������Ȃ������ꍇ�A�������Ȃ��B
	if (isHit == false) {
		return;
	}
	
	//���������I�u�W�F�N�g�̍��W�ɍ��킹�Đݒu����B
	Vector3 portalPos = hit.position;
	//�|�[�^����ݒu�B
	int num = static_cast<int>(type);
	m_portalFrame[num]->SetPortalFrame(portalPos, hit.normal);
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
	m_portalGunModelRender.SetPosition(portalGunPos);
	m_portalGunModelRender.SetRotation(portalGunRot);
	m_portalGunModelRender.Update();
}

void PortalGun::Render(RenderContext& rc)
{
	//�|�[�^���K����`��B
	//m_portalGunModelRender.Draw(rc);

	//�N���X�w�A�̕`��B
	m_crossHairSpriteRender.Draw(rc);
}