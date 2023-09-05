#pragma once

namespace nsPortalEngine {

	struct CaslData;

	struct Level2DObjectData {

		Vector3		position = Vector3::Zero;			//���W�B
		Quaternion	rotation = Quaternion::Identity;	//��]�B
		Vector3		scale = Vector3::One;				//�g�嗦�B
		int			width = 0;							//�����B
		int			height = 0;							//�c���B
		const char* name;								//���O�B
		const char* ddsFilePath;						//dds�t�@�C���̃t�@�C���p�X�B
		Vector2		pivot = Sprite::DEFAULT_PIVOT;		//�s�{�b�g�B

		/// <summary>
		/// �����œn�����I�u�W�F�N�g���̃I�u�W�F�N�g�𒲂ׂ�B
		/// </summary>
		/// <param name="objName"></param>
		/// <returns></returns>
		bool EqualObjectName(const char* objName) const
		{
			return strcmp(objName, name) == 0;
		}

		/// <summary>
		/// ���O���O����v���邩���ׂ�B
		/// </summary>
		/// <param name="n"></param>
		/// <returns></returns>
		bool ForwardMatchName(const char* n) const
		{
			auto len = strlen(n);
			auto namelen = strlen(name);
			if (len > namelen) {
				//���O�������B�s��v�B
				return false;
			}
			return strncmp(n, name, len) == 0;
		}
	};

	/// <summary>
	/// ���x��2D�����_�[�N���X�B
	/// </summary>
	class Level2DRender : Noncopyable
	{
	public:
		/// <summary>
		/// ���x�����������B
		/// </summary>
		/// <param name="filePath">casl�t�@�C���̃t�@�C���p�X�B</param>
		/// <param name="hookFunc">�I�u�W�F�N�g���쎌����Ƃ��̏������t�b�N���邽�߂̊֐��B</param>
		void Init(const char* filePath, std::function<bool(Level2DObjectData& objData)> hookFunc);
	};
}

