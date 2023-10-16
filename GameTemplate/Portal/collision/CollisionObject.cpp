#include "PortalEnginePreCompile.h"
#include "CollisionObject.h"


namespace nsPortalEngine {

	namespace
	{
		const int COLLISION_RESERVE_NUM = 16;		//配列にあらかじめ確保する数。
	}

	CollisionObjectManager* CollisionObjectManager::m_instance = nullptr;

	CollisionObject::CollisionObject()
	{

	}

	CollisionObject::~CollisionObject()
	{
		//コリジョンを配列から削除。
		if (CollisionObjectManager::GetInstance() != nullptr) {
			CollisionObjectManager::GetInstance()->RemoveCollisionObject(this);
		}
	}

	bool CollisionObject::Start()
	{
		//コリジョンを配列に追加。
		CollisionObjectManager::GetInstance()->AddCollisionObject(this);
		return true;
	}

	void CollisionObject::Update()
	{
		//自動削除でないなら。
		if (m_isEnableAutoDelete == false) {
			return;
		}

		//時間が経過したら。
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