#include "PortalEnginePreCompile.h"
#include "Level3DRender.h"
#include "level3D/TklFile.h"

namespace nsPortalEngine {

	LevelRender::LevelRender()
	{

	}

	LevelRender::~LevelRender()
	{

	}

	void LevelRender::Init(
		const char* filePath,
		std::function<bool(LevelObjectData& objData)> hookFunc
	)
	{
		//tkl�t�@�C���̃��[�h
		m_tklFile.Load(filePath);

		MatrixTklToLevel();

		//���[�g�{�[���͂���Ȃ����߁Ai��1����n�߂邱�Ƃŏ��O����
		for (int i = 1; i < m_boneList.size(); i++) {
			auto bone = m_boneList[i].get();

			if (bone->GetParentBoneNo() == 0) {

				////�s�񂩂���W���쐬
				LevelObjectData levelObjData;
				bone->CalcWorldTRS(levelObjData.position, levelObjData.rotation, levelObjData.scale);

				//Zup��Yup�̕ύX�B
				float fix = levelObjData.position.y;
				levelObjData.position.y = levelObjData.position.z;
				levelObjData.position.z = -fix;

				fix = levelObjData.rotation.z;
				levelObjData.rotation.z = levelObjData.rotation.y;
				levelObjData.rotation.y = fix;

				std::swap(levelObjData.scale.y, levelObjData.scale.z);


				levelObjData.name = m_boneList[i]->GetName();
				std::wstring name = levelObjData.name;
				//@�����邩�ǂ�������
				int pos = static_cast<int>(name.find('@'));
				//@������Ȃ�A���̌��ɂ��鐔�����擾
				if (pos != std::wstring::npos) {
					std::wstring number = name.substr(static_cast<size_t>(pos) + 1);
					//wchar�^��int�^�ɕϊ�
					levelObjData.number = std::stoi(number.c_str());
				}

				//�p�X�̍쐬
				wchar_t filePath[256];
				swprintf_s(filePath, L"Assets/modelData/%s.tkm", levelObjData.name);
				//�}���`�o�C�g�ɕϊ�
				//�x�[�X�̕�����
				size_t origsize = wcslen(filePath) + 1;
				//�ϊ�����������
				size_t convertedChars = 0;
				//�}���`�o�C�g�i�[�p
				char strCon[] = "";
				//�T�C�Y
				size_t strConSize = (strlen(strCon) + 1) * 2;
				//�ϊ���̃T�C�Y
				const size_t newSize = origsize * 2;
				//�A����̃T�C�Y
				char* cFilePath = new char[newSize + strConSize];
				//�}���`�o�C�g�ɕϊ�����B����؂�Ȃ����̂͐؂�̂Ă�
				wcstombs_s(&convertedChars, cFilePath, newSize, filePath, _TRUNCATE);
				//������̘A��
				_mbscat_s((unsigned char*)cFilePath, newSize + strConSize, (unsigned char*)strCon);


				//Hook���o�^�ςȂ�}�b�v�`�b�v�͍쐬�s�v
				bool isHooked = false;
				//hookFunc�ɂȂɂ������Ă��Ȃ�=sobject�ɂ͂Ȃɂ��Ȃ�
				if (hookFunc != nullptr) {
					isHooked = hookFunc(levelObjData);
				}
			}
		}
	}

	void LevelRender::MatrixTklToLevel()
	{
		m_tklFile.QuaryObject([&](TklFile::SObject& tklObj) {
			//�R�s�[�B
			//�o�C���h�|�[�Y�B
			Matrix bindPoseMatrix;
			memcpy(bindPoseMatrix.m[0], &tklObj.bindPose[0], sizeof(tklObj.bindPose[0]));
			memcpy(bindPoseMatrix.m[1], &tklObj.bindPose[1], sizeof(tklObj.bindPose[1]));
			memcpy(bindPoseMatrix.m[2], &tklObj.bindPose[2], sizeof(tklObj.bindPose[2]));
			memcpy(bindPoseMatrix.m[3], &tklObj.bindPose[3], sizeof(tklObj.bindPose[3]));
			bindPoseMatrix.m[0][3] = 0.0f;
			bindPoseMatrix.m[1][3] = 0.0f;
			bindPoseMatrix.m[2][3] = 0.0f;
			bindPoseMatrix.m[3][3] = 1.0f;

			/// <summary>
			/// �o�C���h�|�[�Y�̋t�s��
			/// ��3��Vector3�ϊ��̍ۂɕs�v�Ȃ̂ŏ������B
			/// </summary>
			Matrix invBindPoseMatrix;
			memcpy(invBindPoseMatrix.m[0], &tklObj.invBindPose[0], sizeof(tklObj.invBindPose[0]));
			memcpy(invBindPoseMatrix.m[1], &tklObj.invBindPose[1], sizeof(tklObj.invBindPose[1]));
			memcpy(invBindPoseMatrix.m[2], &tklObj.invBindPose[2], sizeof(tklObj.invBindPose[2]));
			memcpy(invBindPoseMatrix.m[3], &tklObj.invBindPose[3], sizeof(tklObj.invBindPose[3]));
			invBindPoseMatrix.m[0][3] = 0.0f;
			invBindPoseMatrix.m[1][3] = 0.0f;
			invBindPoseMatrix.m[2][3] = 0.0f;
			invBindPoseMatrix.m[3][3] = 1.0f;

			wchar_t boneName[256];

			//tkl�I�u�W�F�N�g���̌^�����C�h�����ɕϊ�
			mbstowcs(boneName, tklObj.name.get(), 256);
			BonePtr bone = std::make_unique<Bone>(
				boneName,
				bindPoseMatrix,
				invBindPoseMatrix,
				tklObj.parentNo,
				tklObj.no
				);
			//�{�[����ς�
			m_boneList.push_back(std::move(bone));
			});
	}
}