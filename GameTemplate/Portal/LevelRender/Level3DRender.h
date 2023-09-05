#pragma once
#include "level3D/TklFile.h"


namespace nsPortalEngine {

	struct LevelObjectData {

		Vector3			position;		//���W�B
		Quaternion		rotation;		//��]�B
		Vector3			scale;			//�g�嗦�B
		const wchar_t* name;			//���O�B
		int				number = 0;		//�ԍ��B

		/// <summary>
		/// �I�u�W�F�N�g������������(���S��v)�B
		/// </summary>
		/// <param name="objName">�I�u�W�F�N�g��</param>
		/// <returns>���S��v�Ȃ�true</returns>
		bool EqualObjectName(const wchar_t* objName)
		{
			return wcscmp(objName, name) == 0;
		}

		/// <summary>
		/// �I�u�W�F�N�g������������i�O����v�j�B
		/// </summary>
		/// <param name="objName">�I�u�W�F�N�g��</param>
		/// <returns>�O����v�Ȃ�true</returns>
		bool ForwardMatchName(const wchar_t* objName)
		{
			auto len = wcslen(objName);
			auto namelen = wcslen(name);

			if (len > namelen) {
				//���O�������̂ŕs��v
				return false;
			}
			return wcsncmp(objName, name, len) == 0;
		}
	};

	/// <summary>
	/// ���x�������_�[�N���X�B
	/// </summary>
	class LevelRender : Noncopyable
	{
	public:
		LevelRender();
		~LevelRender();

		/// <summary>
		/// �����������B
		/// </summary>
		/// <param name="filePath"></param>
		/// <param name="hookFunc"></param>
		void Init(const char* filePath, std::function<bool(LevelObjectData& objData)> hookFunc);

	private:
		/// <summary>
		/// Tkl�t�@�C���̍s���ϊ�����B
		/// </summary>
		void MatrixTklToLevel();

	private:
		using BonePtr = std::unique_ptr<Bone>;			//�{�[���̃|�C���^
		std::vector<BonePtr>	m_boneList;				//�{�[���̃��X�g
		TklFile					m_tklFile;				//tkl�t�@�C��
	};
}
