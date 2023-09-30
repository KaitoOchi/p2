#pragma once
#include "physics/PhysicsGhostObject.h"

namespace nsPortalEngine {

	/// <summary>
	/// �R���W�����I�u�W�F�N�g�N���X�B
	/// </summary>
	class CollisionObject : public IGameObject
	{
	public:
		CollisionObject();
		~CollisionObject();
		bool Start();
		void Update();

		/// <summary>
		/// �{�b�N�X�`��̃S�[�X�g�I�u�W�F�N�g���쐬�B
		/// </summary>
		/// <param name="pos"></param>
		/// <param name="rot"></param>
		/// <param name="size"></param>
		void CreateBox(const Vector3& pos, const Quaternion& rot, const Vector3& size)
		{
			m_physicsGhostObject.CreateBox(pos, rot, size);
		}

		/// <summary>
		/// �J�v�Z���`��̃S�[�X�g�I�u�W�F�N�g���쐬�B
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
		/// ���`��̃S�[�X�g�I�u�W�F�N�g���쐬�B
		/// </summary>
		/// <param name="pos"></param>
		/// <param name="rot"></param>
		/// <param name="radius"></param>
		void CreateSphere(const Vector3& pos, const Quaternion& rot, const float radius)
		{
			m_physicsGhostObject.CreateSphere(pos, rot, radius);
		}

		/// <summary>
		/// ���b�V���`��̃S�[�X�g�I�u�W�F�N�g���쐬�B
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
		/// �����폜����鎞�Ԃ�ݒ�B
		/// </summary>
		/// <param name="time"></param>
		void SetTimeLimit(const float time)
		{
			m_timer = time;
		}

		/// <summary>
		/// ���O��ݒ�B
		/// </summary>
		/// <param name="name"></param>
		void SetName(const char* name)
		{
			m_name = name;
		}

		/// <summary>
		/// ���O���擾�B
		/// </summary>
		/// <returns></returns>
		const char* GetName() const
		{
			return m_name;
		}

		/// <summary>
		/// ���W��ݒ�B
		/// </summary>
		/// <param name="pos"></param>
		void SetPosition(const Vector3& pos)
		{
			m_physicsGhostObject.SetPosition(pos);
		}

		/// <summary>
		/// ��]��ݒ�B
		/// </summary>
		/// <param name="rot"></param>
		void SetRotation(const Quaternion& rot)
		{
			m_physicsGhostObject.SetRotation(rot);
		}

		/// <summary>
		/// �s���ݒ�B
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
		/// �����ō폜���邩�ǂ�����ݒ�B
		/// </summary>
		/// <param name="isFlag"></param>
		void SetIsEnableAutoDelete(const bool isFlag)
		{
			m_isEnableAutoDelete = isFlag;
		}

		/// <summary>
		/// �R���W�����I�u�W�F�N�g���m�̓����蔻��B
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
		/// �R���W�����I�u�W�F�N�g�ƃL�����R���̓����蔻��B
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
		/// �R���W�����I�u�W�F�N�g�ƃt�B�W�b�N�X�X�^�e�B�b�N�I�u�W�F�N�g�̓����蔻��B
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
		/// �S�[�X�g�I�u�W�F�N�g���擾�B
		/// </summary>
		/// <returns></returns>
		PhysicsGhostObject& GetGhostObject()
		{
			return m_physicsGhostObject;
		}

		/// <summary>
		/// bt�R���W�����I�u�W�F�N�g���擾�B
		/// </summary>
		/// <returns></returns>
		btCollisionObject& GetbtCollisionObject()
		{
			return m_physicsGhostObject.GetbtCollisionObject();
		}

		/// <summary>
		/// �����蔻�肪�L�����ǂ�����ݒ�B
		/// </summary>
		/// <param name="isEnable"></param>
		void SetIsEnable(const bool isEnable)
		{
			m_isEnable = isEnable;
		}

		/// <summary>
		/// �����蔻�肪�L�����ǂ������擾�B
		/// </summary>
		/// <returns></returns>
		const bool IsEnable() const
		{
			return m_isEnable;
		}

	private:
		PhysicsGhostObject	m_physicsGhostObject;			//�S�[�X�g�I�u�W�F�N�g�B
		const char*			m_name = nullptr;				//���O�B
		bool				m_isEnable = true;				//�����蔻�肪���邩�B
		bool				m_isEnableAutoDelete = false;	//true�Ȃ玞�Ԍo�߂ō폜�B
		float				m_timer = 0.0f;					//�^�C�}�[�B
	};


	/// <summary>
	/// �R���W�����I�u�W�F�N�g�}�l�[�W���[�N���X�B
	/// </summary>
	class CollisionObjectManager : Noncopyable
	{
	public:
		CollisionObjectManager();
		~CollisionObjectManager();

		/// <summary>
		/// �C���X�^���X�̐����B
		/// </summary>
		static void CreateInstance()
		{
			if (m_instance != nullptr) {
				std::abort();
			}
			m_instance = new CollisionObjectManager;
		}

		/// <summary>
		/// �C���X�^���X�̎擾�B
		/// </summary>
		static CollisionObjectManager* GetInstance()
		{
			return m_instance;
		}

		/// <summary>
		/// �C���X�^���X�̍폜�B
		/// </summary>
		/// <param name="collisionObject"></param>
		static void DeleteInstance()
		{
			delete m_instance;
		}

		/// <summary>
		/// �R���W�����I�u�W�F�N�g��ǉ��B
		/// </summary>
		/// <param name="collisionObject"></param>
		void AddCollisionObject(CollisionObject* collisionObject)
		{
			m_collisionObjectVector.emplace_back(collisionObject);
		}

		/// <summary>
		/// ���O�����S��v����R���W�����I�u�W�F�N�g����������B
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		CollisionObject* FindCollisionObject(const char* name) const
		{
			for (auto collisionObject : m_collisionObjectVector)
			{
				//���O�����S��v�Ȃ�B
				if (strcmp(collisionObject->GetName(), name) == 0)
				{
					//�����蔻�肪�L���Ȃ�B
					if (collisionObject->IsEnable() == true)
					{
						return collisionObject;
					}
				}
			}
			return nullptr;
		}

		/// <summary>
		/// ���O���O����v����R���W�����I�u�W�F�N�g����������B
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
					//���O�������B
					continue;
				}
				//���O���O����v�Ȃ�B
				if (strncmp(name, collisionObject->GetName(), len) == 0)
				{
					//�����蔻�肪�L���Ȃ�B
					if (collisionObject->IsEnable() == true)
					{
						return collisionObject;
					}
				}
			}
			return nullptr;
		}

		/// <summary>
		/// ���O�����S��v����R���W�����I�u�W�F�N�g����������B������͕����B
		/// </summary>
		/// <param name="name">���O�B</param>
		/// <returns>�R���W�����I�u�W�F�N�g�̔z��B</returns>
		const std::vector<CollisionObject*>& FindCollisionObjects(const char* name)
		{
			m_findMatchForwardNameCollisionObjectVector.clear();
			for (auto collisionObject : m_collisionObjectVector)
			{
				//���O�����S��v�Ȃ�B
				if (strcmp(collisionObject->GetName(), name) == 0)
				{
					//�����蔻�肪�L���Ȃ�B
					if (collisionObject->IsEnable() == true)
					{
						m_findMatchForwardNameCollisionObjectVector.emplace_back(collisionObject);
					}
				}
			}
			return m_findMatchForwardNameCollisionObjectVector;
		}
		/// <summary>
		/// ���O���O����v����R���W�����I�u�W�F�N�g����������B������͕����B
		/// </summary>
		/// <param name="name">���O�B</param>
		/// <returns>�R���W�����I�u�W�F�N�g�̔z��B</returns>
		const std::vector<CollisionObject*>& FindMatchForwardNameCollisionObjects(const char* name)
		{
			m_findsCollisionObjectVector.clear();
			for (auto collisionObject : m_collisionObjectVector)
			{
				auto len = strlen(name);
				auto namelen = strlen(collisionObject->GetName());
				if (len > namelen) {
					//���O�������B
					continue;
				}
				//���O���O����v�Ȃ�B
				if (strncmp(name, collisionObject->GetName(), len) == 0)
				{
					//�����蔻�肪�L���Ȃ�B
					if (collisionObject->IsEnable() == true)
					{
						m_findsCollisionObjectVector.emplace_back(collisionObject);
					}
				}
			}
			return m_findsCollisionObjectVector;
		}

		/// <summary>
		/// �z�񂩂�R���W�����I�u�W�F�N�g���폜�B
		/// </summary>
		/// <param name="deleteCollisionObject">�폜�������R���W�����I�u�W�F�N�g�B</param>
		void RemoveCollisionObject(CollisionObject* deleteCollisionObject)
		{
			for (auto it = m_collisionObjectVector.begin(); it != m_collisionObjectVector.end();) {
				// ������v�����v�f���폜����
				if (*it == deleteCollisionObject) {
					// �폜���ꂽ�v�f�̎����w���C�e���[�^���Ԃ����B
					it = m_collisionObjectVector.erase(it);
				}
				// �v�f�폜�����Ȃ��ꍇ�ɁA�C�e���[�^��i�߂�
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