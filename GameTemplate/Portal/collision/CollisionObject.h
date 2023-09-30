#pragma once
#include "physics/PhysicsGhostObject.h"

namespace nsPortalEngine {

	/// <summary>
	/// コリジョンオブジェクトクラス。
	/// </summary>
	class CollisionObject : public IGameObject
	{
	public:
		CollisionObject();
		~CollisionObject();
		bool Start();
		void Update();

		/// <summary>
		/// ボックス形状のゴーストオブジェクトを作成。
		/// </summary>
		/// <param name="pos"></param>
		/// <param name="rot"></param>
		/// <param name="size"></param>
		void CreateBox(const Vector3& pos, const Quaternion& rot, const Vector3& size)
		{
			m_physicsGhostObject.CreateBox(pos, rot, size);
		}

		/// <summary>
		/// カプセル形状のゴーストオブジェクトを作成。
		/// </summary>
		/// <param name="pos"></param>
		/// <param name="rot"></param>
		/// <param name="radius"></param>
		/// <param name="height"></param>
		void CreateCapsule(const Vector3& pos, const Quaternion& rot, const float radius, const float height)
		{
			m_physicsGhostObject.CreateCapsule(pos, rot, radius, height);
		}

		/// <summary>
		/// 球形状のゴーストオブジェクトを作成。
		/// </summary>
		/// <param name="pos"></param>
		/// <param name="rot"></param>
		/// <param name="radius"></param>
		void CreateSphere(const Vector3& pos, const Quaternion& rot, const float radius)
		{
			m_physicsGhostObject.CreateSphere(pos, rot, radius);
		}

		/// <summary>
		/// メッシュ形状のゴーストオブジェクトを作成。
		/// </summary>
		/// <param name="pos"></param>
		/// <param name="rot"></param>
		/// <param name="model"></param>
		/// <param name="worldMatrix"></param>
		void CreateMesh(const Vector3& pos, const Quaternion& rot, const Model& model, const Matrix& worldMatrix)
		{
			m_physicsGhostObject.CreateMesh(pos, rot, model, worldMatrix);
		}

		/// <summary>
		/// 自動削除される時間を設定。
		/// </summary>
		/// <param name="time"></param>
		void SetTimeLimit(const float time)
		{
			m_timer = time;
		}

		/// <summary>
		/// 名前を設定。
		/// </summary>
		/// <param name="name"></param>
		void SetName(const char* name)
		{
			m_name = name;
		}

		/// <summary>
		/// 名前を取得。
		/// </summary>
		/// <returns></returns>
		const char* GetName() const
		{
			return m_name;
		}

		/// <summary>
		/// 座標を設定。
		/// </summary>
		/// <param name="pos"></param>
		void SetPosition(const Vector3& pos)
		{
			m_physicsGhostObject.SetPosition(pos);
		}

		/// <summary>
		/// 回転を設定。
		/// </summary>
		/// <param name="rot"></param>
		void SetRotation(const Quaternion& rot)
		{
			m_physicsGhostObject.SetRotation(rot);
		}

		/// <summary>
		/// 行列を設定。
		/// </summary>
		/// <param name="matrix"></param>
		void SetWorldMatrix(const Matrix& matrix)
		{
			Vector3 position;
			position.x = matrix.m[3][0];
			position.y = matrix.m[3][1];
			position.z = matrix.m[3][2];
			SetPosition(position);

			Quaternion rotation;
			rotation.SetRotation(matrix);
			SetRotation(rotation);
		}

		/// <summary>
		/// 自動で削除するかどうかを設定。
		/// </summary>
		/// <param name="isFlag"></param>
		void SetIsEnableAutoDelete(const bool isFlag)
		{
			m_isEnableAutoDelete = isFlag;
		}

		/// <summary>
		/// コリジョンオブジェクト同士の当たり判定。
		/// </summary>
		/// <param name="collisionObject"></param>
		/// <returns></returns>
		const bool IsHit(CollisionObject* collisionObject) const
		{
			bool isCollision = false;
			PhysicsWorld::GetInstance()->ContactTest(&collisionObject->GetbtCollisionObject(), [&](const btCollisionObject& contactObject) {
				if (m_physicsGhostObject.IsSelf(contactObject) == true) {
					isCollision = true;
				}
			});
			return isCollision;
		}

		/// <summary>
		/// コリジョンオブジェクトとキャラコンの当たり判定。
		/// </summary>
		/// <param name="characterController"></param>
		/// <returns></returns>
		const bool IsHit(CharacterController& characterController) const
		{
			bool isCollision = false;
			PhysicsWorld::GetInstance()->ContactTest(characterController, [&](const btCollisionObject& contactObject) {
				if (m_physicsGhostObject.IsSelf(contactObject) == true) {
					isCollision = true;
				}
			});
			return isCollision;
		}

		/// <summary>
		/// コリジョンオブジェクトとフィジックススタティックオブジェクトの当たり判定。
		/// </summary>
		/// <param name="physicsStaticObject"></param>
		/// <returns></returns>
		const bool IsHit(PhysicsStaticObject& physicsStaticObject)
		{
			bool isCollision = false;
			PhysicsWorld::GetInstance()->ContactTest(physicsStaticObject.GetbtCollisionObject(), [&](const btCollisionObject& contactObject) {
				if (m_physicsGhostObject.IsSelf(contactObject) == true) {
					isCollision = true;
				}
			});
			return isCollision;
		}

		/// <summary>
		/// ゴーストオブジェクトを取得。
		/// </summary>
		/// <returns></returns>
		PhysicsGhostObject& GetGhostObject()
		{
			return m_physicsGhostObject;
		}

		/// <summary>
		/// btコリジョンオブジェクトを取得。
		/// </summary>
		/// <returns></returns>
		btCollisionObject& GetbtCollisionObject()
		{
			return m_physicsGhostObject.GetbtCollisionObject();
		}

		/// <summary>
		/// 当たり判定が有効かどうかを設定。
		/// </summary>
		/// <param name="isEnable"></param>
		void SetIsEnable(const bool isEnable)
		{
			m_isEnable = isEnable;
		}

		/// <summary>
		/// 当たり判定が有効かどうかを取得。
		/// </summary>
		/// <returns></returns>
		const bool IsEnable() const
		{
			return m_isEnable;
		}

	private:
		PhysicsGhostObject	m_physicsGhostObject;			//ゴーストオブジェクト。
		const char*			m_name = nullptr;				//名前。
		bool				m_isEnable = true;				//当たり判定があるか。
		bool				m_isEnableAutoDelete = false;	//trueなら時間経過で削除。
		float				m_timer = 0.0f;					//タイマー。
	};


	/// <summary>
	/// コリジョンオブジェクトマネージャークラス。
	/// </summary>
	class CollisionObjectManager : Noncopyable
	{
	public:
		CollisionObjectManager();
		~CollisionObjectManager();

		/// <summary>
		/// インスタンスの生成。
		/// </summary>
		static void CreateInstance()
		{
			if (m_instance != nullptr) {
				std::abort();
			}
			m_instance = new CollisionObjectManager;
		}

		/// <summary>
		/// インスタンスの取得。
		/// </summary>
		static CollisionObjectManager* GetInstance()
		{
			return m_instance;
		}

		/// <summary>
		/// インスタンスの削除。
		/// </summary>
		/// <param name="collisionObject"></param>
		static void DeleteInstance()
		{
			delete m_instance;
		}

		/// <summary>
		/// コリジョンオブジェクトを追加。
		/// </summary>
		/// <param name="collisionObject"></param>
		void AddCollisionObject(CollisionObject* collisionObject)
		{
			m_collisionObjectVector.emplace_back(collisionObject);
		}

		/// <summary>
		/// 名前が完全一致するコリジョンオブジェクトを検索する。
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		CollisionObject* FindCollisionObject(const char* name) const
		{
			for (auto collisionObject : m_collisionObjectVector)
			{
				//名前が完全一致なら。
				if (strcmp(collisionObject->GetName(), name) == 0)
				{
					//当たり判定が有効なら。
					if (collisionObject->IsEnable() == true)
					{
						return collisionObject;
					}
				}
			}
			return nullptr;
		}

		/// <summary>
		/// 名前が前方一致するコリジョンオブジェクトを検索する。
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		const CollisionObject* FindMatchForwardNameCollisionObject(const char* name) const
		{
			for (auto collisionObject : m_collisionObjectVector)
			{
				auto len = strlen(name);
				auto namelen = strlen(collisionObject->GetName());
				if (len > namelen) {
					//名前が長い。
					continue;
				}
				//名前が前方一致なら。
				if (strncmp(name, collisionObject->GetName(), len) == 0)
				{
					//当たり判定が有効なら。
					if (collisionObject->IsEnable() == true)
					{
						return collisionObject;
					}
				}
			}
			return nullptr;
		}

		/// <summary>
		/// 名前が完全一致するコリジョンオブジェクトを検索する。こちらは複数。
		/// </summary>
		/// <param name="name">名前。</param>
		/// <returns>コリジョンオブジェクトの配列。</returns>
		const std::vector<CollisionObject*>& FindCollisionObjects(const char* name)
		{
			m_findMatchForwardNameCollisionObjectVector.clear();
			for (auto collisionObject : m_collisionObjectVector)
			{
				//名前が完全一致なら。
				if (strcmp(collisionObject->GetName(), name) == 0)
				{
					//当たり判定が有効なら。
					if (collisionObject->IsEnable() == true)
					{
						m_findMatchForwardNameCollisionObjectVector.emplace_back(collisionObject);
					}
				}
			}
			return m_findMatchForwardNameCollisionObjectVector;
		}
		/// <summary>
		/// 名前が前方一致するコリジョンオブジェクトを検索する。こちらは複数。
		/// </summary>
		/// <param name="name">名前。</param>
		/// <returns>コリジョンオブジェクトの配列。</returns>
		const std::vector<CollisionObject*>& FindMatchForwardNameCollisionObjects(const char* name)
		{
			m_findsCollisionObjectVector.clear();
			for (auto collisionObject : m_collisionObjectVector)
			{
				auto len = strlen(name);
				auto namelen = strlen(collisionObject->GetName());
				if (len > namelen) {
					//名前が長い。
					continue;
				}
				//名前が前方一致なら。
				if (strncmp(name, collisionObject->GetName(), len) == 0)
				{
					//当たり判定が有効なら。
					if (collisionObject->IsEnable() == true)
					{
						m_findsCollisionObjectVector.emplace_back(collisionObject);
					}
				}
			}
			return m_findsCollisionObjectVector;
		}

		/// <summary>
		/// 配列からコリジョンオブジェクトを削除。
		/// </summary>
		/// <param name="deleteCollisionObject">削除したいコリジョンオブジェクト。</param>
		void RemoveCollisionObject(CollisionObject* deleteCollisionObject)
		{
			for (auto it = m_collisionObjectVector.begin(); it != m_collisionObjectVector.end();) {
				// 条件一致した要素を削除する
				if (*it == deleteCollisionObject) {
					// 削除された要素の次を指すイテレータが返される。
					it = m_collisionObjectVector.erase(it);
				}
				// 要素削除をしない場合に、イテレータを進める
				else {
					++it;
				}
			}
		}

	private:
		static CollisionObjectManager* m_instance;
		std::vector<CollisionObject*> m_collisionObjectVector;
		std::vector<CollisionObject*> m_findsCollisionObjectVector;
		std::vector<CollisionObject*> m_findMatchForwardNameCollisionObjectVector;
	};
}