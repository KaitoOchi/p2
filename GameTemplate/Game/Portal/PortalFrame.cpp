#include "stdafx.h"
#include "PortalFrame.h"
#include "Player.h"
#include "EnergyBall.h"
#include "GameCamera.h"
#include "PortalCamera.h"
#include "Stage.h"

namespace
{
	const Vector3	PORTAL_COLLISION_SIZE = Vector3(80.0f, 106.0f, 1.0f);		//�R���W�����T�C�Y�B
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
	DeleteGO(m_frameEffect);
}

bool PortalFrame::Start()
{
	//�v���C���[�I�u�W�F�N�g�������B
	m_player = FindGO<Player>("player");
	m_gameCamera = FindGO<GameCamera>("gameCamera");
	m_portalCamera = FindGO<PortalCamera>("portalCamera");
	m_stage = FindGO<Stage>("stage");

	//�����蔻����������B
	m_collisionObject = NewGO<CollisionObject>(0, "collision");
	m_collisionObject->CreateBox(m_position, m_rotation, PORTAL_COLLISION_SIZE);
	m_collisionObject->SetName("portal_red");
	m_collisionObject->SetIsEnable(false);

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
	if (!m_anotherPortalFrame->GetIsEnable()) {
		return;
	}

	CollisionPlayer();
	CollisionEnergyBall();
}

/// <summary>
/// �v���C���[�Ƃ̓����蔻�菈���B
/// </summary>
void PortalFrame::CollisionPlayer()
{
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
			IsHitPlayer();
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
/// �G�l���M�[�{�[���Ƃ̓����蔻�菈���B
/// </summary>
void PortalFrame::CollisionEnergyBall()
{
	for (auto& ball : m_stage->GetEnergyBallObjects())
	{
		//�|�[�^���ƃv���C���[�̃x�N�g�����͈͊O�Ȃ�A�Փ˔�����s��Ȃ��B
		Vector3 length = ball->GetPosition() - m_position;
		if (length.LengthSq() > PORTAL_LENGTH * PORTAL_LENGTH) {
			return;
		}

		//�G�l���M�[�{�[���ƐڐG���Ă���Ȃ�B
		if (m_collisionObject->IsHit(ball->GetCollisionObject())) {
			//��r�p�̍��W��ݒ�B
			float compareSmallPos = 0.0f;
			float compareLargePos = 0.0f;

			if (m_normal.x >= 1.0f) {
				compareLargePos = m_position.x;
				compareSmallPos = ball->GetPosition().x;
			}
			else if (m_normal.x <= -1.0f) {
				compareLargePos = ball->GetPosition().x;
				compareSmallPos = m_position.x;
			}
			else if (m_normal.z >= 1.0f) {
				compareLargePos = m_position.z;
				compareSmallPos = ball->GetPosition().z;
			}
			else if (m_normal.z <= -1.0f) {
				compareLargePos = ball->GetPosition().z;
				compareSmallPos = m_position.z;
			}
			else if (m_normal.y >= 1.0f) {
				compareLargePos = m_position.y;
				compareSmallPos = ball->GetPosition().y;
			}
			else if (m_normal.y <= -1.0f) {
				compareLargePos = ball->GetPosition().y;
				compareSmallPos = m_position.y;
			}

			//�v���C���[�̍��W���|�[�^���̍��W�𒴂��Ă�����B
			if (compareLargePos > compareSmallPos) {
				IsHitEnergyBall(ball);
			}
		}
	}
}

/// <summary>
/// �v���C���[�ɓ��������Ƃ��̏����B
/// </summary>
void PortalFrame::IsHitPlayer()
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
/// �G�l���M�[�{�[���ɓ��������Ƃ��̏����B
/// </summary>
void PortalFrame::IsHitEnergyBall(EnergyBall* ball)
{
	//��������̃|�[�^���̍��W�܂ňړ�����B
	ball->SetWarp(m_anotherPortalFrame->GetPosition(), m_anotherPortalFrame->GetNormal());
}

//�Փ˂����Ƃ��ɌĂ΂��֐��I�u�W�F�N�g(�n�ʗp)
struct SweepResultGround : public btCollisionWorld::ConvexResultCallback
{
	bool isHit = false;									//�Փ˃t���O�B
	bool isHitMe = false;								//���g�ɏՓ˂������ǂ����B
	bool isResultGround = true;							//�n�ʏՓ˂𒲂ׂĂ��邩�ǂ����B
	Vector3 hitPos = Vector3(0.0f, -FLT_MAX, 0.0f);		//�Փ˓_�B
	Vector3 startPos;									//���C�̎n�_�B
	Vector3 hitNormal;									//�Փ˓_�̖@���B
	btCollisionObject* me = nullptr;					//�������g�B�������g�Ƃ̏Փ˂����O���邽�߂̃����o�B
	float dist = FLT_MAX;								//�Փ˓_�܂ł̋����B��ԋ߂��Փ˓_�����߂邽�߁BFLT_MAX�͒P���x�̕��������_����肤��ő�̒l�B

	//�Փ˂����Ƃ��ɌĂ΂��R�[���o�b�N�֐��B
	virtual	btScalar	addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace)
	{
		if (isHitMe) {
			return 0.0f;
		}
		if (convexResult.m_hitCollisionObject == me
			|| convexResult.m_hitCollisionObject->getUserIndex() == EnCollisionAttr::enCollisionAttr_PortalFrame
			) {
			//�����ɏՓ˂����B
			isHitMe = true;
			isHit = false;
			return 0.0f;
		}
		if (convexResult.m_hitCollisionObject->getInternalType() == btCollisionObject::CO_GHOST_OBJECT) {
			//�S�[�X�g�I�u�W�F�N�g�ɏՓ˂����B
			return 0.0f;
		}

		//�n�ʂƓV��ǂ���̏Փ˂𒲂ׂĂ��邩�B
		float up = 1.0f;
		if (isResultGround == false) {
			up = -1.0f;
		}

		//�Փ˓_�̖@�������������Ă���B
		Vector3 hitNormalTmp = *(Vector3*)&convexResult.m_hitNormalLocal;
		//������Ɩ@���̂Ȃ��p�x�����߂�B
		float angle = hitNormalTmp.y * up;
		angle = fabsf(acosf(angle));
		if (angle < Math::PI * 0.3f		//�n�ʂ̌X�΂�54�x��菬�����̂Œn�ʂƂ݂Ȃ��B
			|| convexResult.m_hitCollisionObject->getUserIndex() == enCollisionAttr_Ground //�������̓R���W�����������n�ʂƎw�肳��Ă���B
			) {
			//�Փ˂��Ă���B
			isHit = true;
			Vector3 hitPosTmp = *(Vector3*)&convexResult.m_hitPointLocal;
			//�Փ˓_�̋��������߂�B�B
			Vector3 vDist;
			vDist.Subtract(hitPosTmp, startPos);
			float distTmp = vDist.Length();
			if (dist > distTmp) {
				//���̏Փ˓_�̕����߂��̂ŁA�ŋߖT�̏Փ˓_���X�V����B
				hitPos = hitPosTmp;
				hitNormal = *(Vector3*)&convexResult.m_hitNormalLocal;
				dist = distTmp;
			}
		}
		return 0.0f;
	}
};

//�Փ˂����Ƃ��ɌĂ΂��֐��I�u�W�F�N�g(�Ǘp)
struct SweepResultWall : public btCollisionWorld::ConvexResultCallback
{
	bool isHit = false;						//�Փ˃t���O�B
	bool isHitMe = false;					//���g�ɏՓ˂������ǂ����B
	Vector3 hitPos;							//�Փ˓_�B
	Vector3 startPos;						//���C�̎n�_�B
	float dist = FLT_MAX;					//�Փ˓_�܂ł̋����B��ԋ߂��Փ˓_�����߂邽�߁BFLT_MAX�͒P���x�̕��������_����肤��ő�̒l�B
	Vector3 hitNormal;						//�Փ˓_�̖@���B
	btCollisionObject* me = nullptr;		//�������g�B�������g�Ƃ̏Փ˂����O���邽�߂̃����o�B

	//�Փ˂����Ƃ��ɌĂ΂��R�[���o�b�N�֐��B
	virtual	btScalar	addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace)
	{
		if (isHitMe) {
			return 0.0f;
		}
		if (convexResult.m_hitCollisionObject == me
			|| convexResult.m_hitCollisionObject->getUserIndex() == EnCollisionAttr::enCollisionAttr_PortalFrame
			) {
			//�����ɏՓ˂����B
			isHitMe = true;
			isHit = false;
			return 0.0f;
		}
		if (convexResult.m_hitCollisionObject->getInternalType() == btCollisionObject::CO_GHOST_OBJECT) {
			//�S�[�X�g�I�u�W�F�N�g�ɏՓ˂����B
			return 0.0f;
		}

		//�Փ˓_�̖@�������������Ă���B
		Vector3 hitNormalTmp = *(Vector3*)&convexResult.m_hitNormalLocal;

		//������ƏՓ˓_�̖@���̂Ȃ��p�x�����߂�B
		float angle = fabsf(acosf(hitNormalTmp.y));
		if (angle >= Math::PI * 0.3f		//�n�ʂ̌X�΂�54�x�ȏ�Ȃ̂ŕǂƂ݂Ȃ��B
			|| convexResult.m_hitCollisionObject->getUserIndex() == enCollisionAttr_Character	//�������̓R���W�����������L�����N�^�Ȃ̂ŕǂƂ݂Ȃ��B
			) {
			isHit = true;
			Vector3 hitPosTmp = *(Vector3*)&convexResult.m_hitPointLocal;

			//��_�Ƃ̋����𒲂ׂ�B
			Vector3 vDist;
			vDist.Subtract(hitPosTmp, startPos);
			vDist.y = 0.0f;
			float distTmp = vDist.Length();
			if (distTmp < dist) {
				//���̏Փ˓_�̕����߂��̂ŁA�ŋߖT�̏Փ˓_���X�V����B
				hitPos = hitPosTmp;
				dist = distTmp;
				hitNormal = hitNormalTmp;
			}
		}
		return 0.0f;
	}
};

/// <summary>
/// �|�[�^�����ݒu�ł��邩�ǂ����B
/// </summary>
/// <param name="pos"></param>
/// <param name="normal"></param>
/// <returns></returns>
const bool PortalFrame::IsCanPut(const Vector3& pos, const Vector3& normal)
{
	bool isHit = false;
	Vector3 startPos = pos;
	startPos.x += fabsf(normal.x) * 5.0f;
	startPos.z += fabsf(normal.z) * 5.0f;


	Vector3 finalPos = pos;

	////�E�����𒲂ׂ�B
	//{
	//	btTransform start, end;
	//	start.setIdentity();
	//	end.setIdentity();
	//	//���_�͍��W�̒��S�B
	//	start.setOrigin(btVector3(startPos.x, startPos.y, startPos.z));
	//	//�I�_�̓R���W�����̉E�ӁB
	//	end.setOrigin(btVector3(
	//		startPos.x - ((PORTAL_COLLISION_SIZE.x / 2.0f) * fabsf(normal.z)),
	//		startPos.y,
	//		startPos.z - ((PORTAL_COLLISION_SIZE.x / 2.0f) * fabsf(normal.x))
	//	));

	//	SweepResultWall callback;
	//	callback.me = &m_collisionObject->GetbtCollisionObject();
	//	callback.startPos = startPos;

	//	PhysicsWorld::GetInstance()->ConvexSweepTest((const btConvexShape*)m_collisionObject->GetGhostObject().GetBody(), start, end, callback);

	//	//�Փ˂����B
	//	if (callback.isHit) {
	//		finalPos.x = (callback.hitPos.x - (PORTAL_COLLISION_SIZE.x / 2.0f) * fabsf(normal.z));
	//		finalPos.z = (callback.hitPos.z - (PORTAL_COLLISION_SIZE.x / 2.0f) * fabsf(normal.x));
	//		isHit = callback.isHit;
	//	}
	//}

	////�������𒲂ׂ�B
	//{
	//	//�E�����𒲂ׂ�B
	//	btTransform start, end;
	//	start.setIdentity();
	//	end.setIdentity();
	//	//���_�͍��W�̒��S�B
	//	start.setOrigin(btVector3(pos.x, pos.y, pos.z));
	//	//�I�_�̓R���W�����̍��ӁB
	//	end.setOrigin(btVector3(
	//		startPos.x + ((PORTAL_COLLISION_SIZE.x / 2.0f) * fabsf(normal.z)),
	//		startPos.y,
	//		startPos.z + ((PORTAL_COLLISION_SIZE.x / 2.0f) * fabsf(normal.x))
	//	));

	//	SweepResultWall callback;
	//	callback.me = &m_collisionObject->GetbtCollisionObject();
	//	callback.startPos = startPos;

	//	PhysicsWorld::GetInstance()->ConvexSweepTest((const btConvexShape*)m_collisionObject->GetGhostObject().GetBody(), start, end, callback);

	//	//�Փ˂����B
	//	if (callback.isHit) {
	//		finalPos.x = (callback.hitPos.x + (PORTAL_COLLISION_SIZE.x / 2.0f) * fabsf(normal.z));
	//		finalPos.z = (callback.hitPos.z + (PORTAL_COLLISION_SIZE.x / 2.0f) * fabsf(normal.x));
	//		isHit = callback.isHit;
	//	}
	//}

	////������𒲂ׂ�B
	//{
	//	btTransform start, end;
	//	start.setIdentity();
	//	end.setIdentity();
	//	//���_�͍��W�̒��S�B
	//	start.setOrigin(btVector3(startPos.x, startPos.y, startPos.z));
	//	//�I�_�̓R���W�����̏�ӁB
	//	end.setOrigin(btVector3(
	//		startPos.x,
	//		startPos.y + (PORTAL_COLLISION_SIZE.y / 2.0f),
	//		startPos.z));

	//	SweepResultGround callback;
	//	callback.me = &m_collisionObject->GetbtCollisionObject();
	//	callback.isResultGround = false;
	//	callback.startPos = startPos;

	//	PhysicsWorld::GetInstance()->ConvexSweepTest((const btConvexShape*)m_collisionObject->GetGhostObject().GetBody(), start, end, callback);

	//	//�Փ˂����B
	//	if (callback.isHit) {
	//		finalPos.y = callback.hitPos.y - (PORTAL_COLLISION_SIZE.y / 2.0f);
	//		isHit = callback.isHit;
	//	}
	//}

	//�������𒲂ׂ�B
	{
		btTransform start, end;
		start.setIdentity();
		end.setIdentity();
		//���_�͍��W�̒��S�B
		start.setOrigin(btVector3(startPos.x, startPos.y, startPos.z));
		//�I�_�̓R���W�����̉��ӁB
		end.setOrigin(btVector3(
			startPos.x,
			startPos.y - (PORTAL_COLLISION_SIZE.y / 2.0f),
			startPos.z
		));

		SweepResultGround callback;
		callback.me = &m_collisionObject->GetbtCollisionObject();
		callback.isResultGround = true;
		callback.startPos = startPos;

		PhysicsWorld::GetInstance()->ConvexSweepTest((const btConvexShape*)m_collisionObject->GetGhostObject().GetBody(), start, end, callback);

		//�Փ˂����B
		if (callback.isHit) {
			finalPos.y = callback.hitPos.y + (PORTAL_COLLISION_SIZE.y / 2.0f);
			isHit = callback.isHit;
		}
	}

	if (isHit) {
		m_position = finalPos;
	}

	return isHit;
}

void PortalFrame::Reset()
{
	m_position = Vector3(0.0f, -1000.0f, 0.0f);
	m_collisionObject->SetPosition(m_position);
	m_collisionObject->SetIsEnable(false);
	m_portalFrameModelRender.SetPosition(m_position);
	m_portalFrameModelRender.Update();
	m_physicsStaticObject.SetPosition(m_position);
	m_isEnable = false;

	if (m_frameEffect != nullptr) {
		m_frameEffect->Stop();
		DeleteGO(m_frameEffect);
	}
}


/// <summary>
/// �|�[�^���t���[����ݒ�B
/// </summary>
/// <param name="pos">���W</param>
/// <param name="normal">�@��</param>
void PortalFrame::SetPortalFrame(const Vector3& pos, const Vector3& normal)
{
	//if (IsCanPut(pos, normal) == false) {
	//	return;
	//}
	m_position = pos;

	m_isEnable = true;
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
	m_collisionObject->SetIsEnable(true);

	//�����ÓI�I�u�W�F�N�g��ݒ�B
	m_physicsStaticObject.SetPosition(m_position);
	m_physicsStaticObject.SetRotation(m_rotation);

	if (m_frameEffect != nullptr) {
		m_frameEffect->Stop();
		DeleteGO(m_frameEffect);
	}

	//�g�g�݃G�t�F�N�g���Đ��B
	m_frameEffect = NewGO<EffectEmitter>(0);
	m_frameEffect->Init(EffectEmitter::enEffect_PortalFrame_Blue);
	m_frameEffect->SetPosition(m_position);
	m_frameEffect->SetRotation(m_rotation);
	m_frameEffect->Play();
}

void PortalFrame::Render(RenderContext& rc)
{
	if (m_isEnable) {
		m_portalFrameModelRender.Draw(rc);
	}
}