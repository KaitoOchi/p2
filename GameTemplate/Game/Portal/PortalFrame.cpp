#include "stdafx.h"
#include "PortalFrame.h"
#include "Player.h"
#include "GameCamera.h"
#include "PortalCamera.h"

namespace
{
	const Vector3	PORTAL_COLLISION_SIZE = Vector3(60.0f, 106.0f, 1.0f);		//�R���W�����T�C�Y�B
	const float		PORTAL_POS_NORMAL = 0.1f;									//�|�[�^���ݒu���̍��W�B
	const float		PORTAL_ROT_X = 180.0f;										//�|�[�^���ݒu���̏㉺��]�B
	const float		PORTAL_LENGTH = 100.0f;										//�|�[�^���̏Փ˔�����J�n����͈́B
}


PortalFrame::PortalFrame()
{
	m_position = Vector3(0.0f, -1000.0f, 0.0f);
}

PortalFrame::~PortalFrame()
{
	DeleteGO(m_collisionObject);
}

bool PortalFrame::Start()
{
	//�v���C���[�I�u�W�F�N�g�������B
	m_player = FindGO<Player>("player");
	m_gameCamera = FindGO<GameCamera>("gameCamera");
	m_portalCamera = FindGO<PortalCamera>("portalCamera");

	//�����蔻����������B
	m_collisionObject = NewGO<CollisionObject>(0, "collision");
	m_collisionObject->CreateBox(m_position, m_rotation, PORTAL_COLLISION_SIZE);
	m_collisionObject->SetName("portal_red");

	//�|�[�^���t���[�����������B
	//ModelInitData���g�p���ď���������B
	ModelInitData modelInitData;
	modelInitData.m_tkmFilePath = "Assets/modelData/portal/portalFrame.tkm";
	modelInitData.m_fxFilePath = "Assets/shader/preProcess/RenderToGBuffer.fx";

	if (m_portalType == enPortalType_Blue) {
		modelInitData.m_psEntryPointFunc = "PSBluePortal";
	}
	else if (m_portalType == enPortalType_Red) {
		modelInitData.m_psEntryPointFunc = "PSRedPortal";
	}

	modelInitData.m_expandConstantBuffer = &RenderingEngine::GetInstance()->GetLightCB();
	modelInitData.m_expandConstantBufferSize = sizeof(RenderingEngine::GetInstance()->GetLightCB());
	modelInitData.m_expandShaderResoruceView[0] = &RenderingEngine::GetInstance()->GetZPrepassRenderTarget().GetRenderTargetTexture();

	m_portalFrameModelRender.InitModelInitData(modelInitData);
	m_portalFrameModelRender.SetPosition(m_position);
	m_portalFrameModelRender.Update();

	//�|�[�^���t���[���̓����蔻����������B
	m_portalFrameCollisionModelRender.Init("Assets/modelData/portal/portalFrameCollision.tkm");
	m_physicsStaticObject.CreateFromModel(m_portalFrameCollisionModelRender.GetModel(), m_portalFrameCollisionModelRender.GetModel().GetWorldMatrix(), 0.0f);
	m_physicsStaticObject.SetPosition(m_position);
	m_physicsStaticObject.GetbtCollisionObject()->setUserIndex(enCollisionAttr_PortalFrame);


	return true;
}

void PortalFrame::Update()
{
	if (!m_isEnable) {
		return;
	}

	Collision();
}

/// <summary>
/// �����蔻��̏����B
/// </summary>
void PortalFrame::Collision()
{
	//�����蔻�肪�����Ȃ�B
	if (!m_collisionObject->IsEnable()) {
		return;
	}

	//�|�[�^���ƃv���C���[�̃x�N�g�����͈͊O�Ȃ�A�Փ˔�����s��Ȃ��B
	Vector3 length = m_player->GetPosition() - m_position;
	if (length.LengthSq() > PORTAL_LENGTH * PORTAL_LENGTH) {
		return;
	}

	//�v���C���[�ƐڐG���Ă���Ȃ�B
	if (m_collisionObject->IsHit(const_cast<CharacterController&>(*m_player->GetCharacterController()))) {
		if (!m_isCollisionHit) {
			//�L�����N�^�[�R���g���[���[���|�[�^���ɓ�������Ԃɂ���B
			m_player->SetIsPortal(true);
			m_isCollisionHit = true;
		}

		//��r�p�̍��W��ݒ�B
		float compareSmallPos = 0.0f;
		float compareLargePos = 0.0f;

		if (m_normal.x >= 1.0f) {
			compareLargePos = m_position.x;
			compareSmallPos = m_player->GetPosition().x;
		}
		else if (m_normal.x <= -1.0f) {
			compareLargePos = m_player->GetPosition().x;
			compareSmallPos = m_position.x;
		}
		else if (m_normal.z >= 1.0f) {
			compareLargePos = m_position.z;
			compareSmallPos = m_player->GetPosition().z;
		}
		else if (m_normal.z <= -1.0f) {
			compareLargePos = m_player->GetPosition().z;
			compareSmallPos = m_position.z;
		}
		else if (m_normal.y >= 1.0f) {
			compareLargePos = m_position.y;
			compareSmallPos = m_player->GetPosition().y;
		}
		else if (m_normal.y <= -1.0f) {
			compareLargePos = m_player->GetPosition().y;
			compareSmallPos = m_position.y;
		}

		//�v���C���[�̍��W���|�[�^���̍��W�𒴂��Ă�����B
		if (compareLargePos > compareSmallPos) {
			IsHit();
		}
	}
	//�Փ˂��Ă��Ȃ��Ȃ�B
	else {
		if (m_isCollisionHit) {
			m_player->SetIsPortal(false);
			m_isCollisionHit = false;
		}
	}
}

/// <summary>
/// ���������Ƃ��̏����B
/// </summary>
void PortalFrame::IsHit()
{
	//2�̃|�[�^���̊p�x���v�Z�B
	float angle = acosf(m_normal.Dot(m_anotherPortalFrame->GetNormal()));
	angle = Math::RadToDeg(angle);

	//�O�ς����߁Ay�����g���ā}��␳����B
	Vector3 cross = m_normal;
	cross.Cross(m_anotherPortalFrame->GetNormal());
	if (cross.y <= -1.0f) {
		angle *= -1.0f;
	}

	//��������̃|�[�^���̍��W�܂ňړ�����B
	m_player->SetWarp(m_anotherPortalFrame->GetPosition(), angle);

	m_isCollisionHit = false;
}

/// <summary>
/// �|�[�^���t���[����ݒ�B
/// </summary>
/// <param name="pos">���W</param>
/// <param name="normal">�@��</param>
void PortalFrame::SetPortalFrame(const Vector3& pos, const Vector3& normal)
{
	m_isEnable = true;
	m_position = pos;
	m_normal = normal;

	//�@���̕����ɏ����ړ�������B
	m_position += m_normal * PORTAL_POS_NORMAL;

	//���������I�u�W�F�N�g�̖@���ɍ��킹�ĉ�]����B
	Quaternion portalRot;
	portalRot.SetRotationYFromDirectionXZ(m_normal);
	portalRot.AddRotationDegX(-m_normal.y * PORTAL_ROT_X);
	m_rotation = portalRot;

	//�@����Y���Ȃ�A�J�����̉�]�ɉ����ă|�[�^������]������B
	if (m_normal.y < -0.99f) {
		m_rotation.AddRotationZ(Math::PI / 2.0f - fabsf(m_gameCamera->GetRotRadian().x));
	}
	else if (m_normal.y > 0.99f) {
		m_rotation.AddRotationZ(Math::PI / 2.0f - m_gameCamera->GetRotRadian().x);
	}

	//�|�[�^���J�������X�V�B
	m_portalCamera->UpdateAngle();

	//�|�[�^�����f����ݒ�B
	m_portalFrameModelRender.SetPosition(m_position);
	m_portalFrameModelRender.SetRotation(m_rotation);
	m_portalFrameModelRender.Update();

	//�|�[�^���̓����蔻���ݒ�B
	m_collisionObject->SetPosition(m_position);
	m_collisionObject->SetRotation(m_rotation);

	//�����ÓI�I�u�W�F�N�g��ݒ�B
	m_physicsStaticObject.SetPosition(m_position);
	m_physicsStaticObject.SetRotation(m_rotation);
}

void PortalFrame::Render(RenderContext& rc)
{
	if (m_isEnable) {
		m_portalFrameModelRender.Draw(rc);
	}
}