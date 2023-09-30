#include "PortalEnginePreCompile.h"
#include "CollisionObject.h"


namespace nsPortalEngine {

	CollisionObjectManager* CollisionObjectManager::m_instance = nullptr;

	CollisionObject::CollisionObject()
	{

	}

	CollisionObject::~CollisionObject()
	{
		if (CollisionObjectManager::GetInstance() != nullptr) {
			CollisionObjectManager::GetInstance()->RemoveCollisionObject(this);
		}
	}

	bool CollisionObject::Start()
	{
		CollisionObjectManager::GetInstance()->AddCollisionObject(this);
		return true;
	}

	void CollisionObject::Update()
	{
		if (m_isEnableAutoDelete == false) {
			return;
		}

		if (m_timer > 0.0f) {
			DeleteGO(this);
		}
		m_timer -= g_gameTime->GetFrameDeltaTime();
	}

	CollisionObjectManager::CollisionObjectManager()
	{
	}

	CollisionObjectManager::~CollisionObjectManager()
	{
		m_collisionObjectVector.clear();
	}
}