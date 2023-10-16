#include "PortalEnginePreCompile.h"
#include "CollisionObject.h"


namespace nsPortalEngine {

	namespace
	{
		const int COLLISION_RESERVE_NUM = 16;		//�z��ɂ��炩���ߊm�ۂ��鐔�B
	}

	CollisionObjectManager* CollisionObjectManager::m_instance = nullptr;

	CollisionObject::CollisionObject()
	{

	}

	CollisionObject::~CollisionObject()
	{
		//�R���W������z�񂩂�폜�B
		if (CollisionObjectManager::GetInstance() != nullptr) {
			CollisionObjectManager::GetInstance()->RemoveCollisionObject(this);
		}
	}

	bool CollisionObject::Start()
	{
		//�R���W������z��ɒǉ��B
		CollisionObjectManager::GetInstance()->AddCollisionObject(this);
		return true;
	}

	void CollisionObject::Update()
	{
		//�����폜�łȂ��Ȃ�B
		if (m_isEnableAutoDelete == false) {
			return;
		}

		//���Ԃ��o�߂�����B
		if (m_timer < 0.0f) {
			DeleteGO(this);
		}
		m_timer -= g_gameTime->GetFrameDeltaTime();
	}

	CollisionObjectManager::CollisionObjectManager()
	{
		m_collisionObjectVector.reserve(COLLISION_RESERVE_NUM);
		m_findsCollisionObjectVector.reserve(COLLISION_RESERVE_NUM);
		m_findMatchForwardNameCollisionObjectVector.reserve(COLLISION_RESERVE_NUM);
	}

	CollisionObjectManager::~CollisionObjectManager()
	{
		m_collisionObjectVector.clear();
		m_findsCollisionObjectVector.clear();
		m_findMatchForwardNameCollisionObjectVector.clear();
	}
}