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
		//tklファイルのロード
		m_tklFile.Load(filePath);

		MatrixTklToLevel();

		//ルートボーンはいらないため、iを1から始めることで除外する
		for (int i = 1; i < m_boneList.size(); i++) {
			auto bone = m_boneList[i].get();

			if (bone->GetParentBoneNo() == 0) {

				////行列から座標を作成
				LevelObjectData levelObjData;
				bone->CalcWorldTRS(levelObjData.position, levelObjData.rotation, levelObjData.scale);

				//ZupとYupの変更。
				float fix = levelObjData.position.y;
				levelObjData.position.y = levelObjData.position.z;
				levelObjData.position.z = -fix;

				fix = levelObjData.rotation.z;
				levelObjData.rotation.z = levelObjData.rotation.y;
				levelObjData.rotation.y = fix;

				std::swap(levelObjData.scale.y, levelObjData.scale.z);


				levelObjData.name = m_boneList[i]->GetName();
				std::wstring name = levelObjData.name;
				//@があるかどうか検索
				int pos = static_cast<int>(name.find('@'));
				//@があるなら、その後ろにある数字を取得
				if (pos != std::wstring::npos) {
					std::wstring number = name.substr(static_cast<size_t>(pos) + 1);
					//wchar型をint型に変換
					levelObjData.number = std::stoi(number.c_str());
				}

				//パスの作成
				wchar_t filePath[256];
				swprintf_s(filePath, L"Assets/modelData/%s.tkm", levelObjData.name);
				//マルチバイトに変換
				//ベースの文字列
				size_t origsize = wcslen(filePath) + 1;
				//変換した文字列
				size_t convertedChars = 0;
				//マルチバイト格納用
				char strCon[] = "";
				//サイズ
				size_t strConSize = (strlen(strCon) + 1) * 2;
				//変換後のサイズ
				const size_t newSize = origsize * 2;
				//連結後のサイズ
				char* cFilePath = new char[newSize + strConSize];
				//マルチバイトに変換する。入り切らないものは切り捨てる
				wcstombs_s(&convertedChars, cFilePath, newSize, filePath, _TRUNCATE);
				//文字列の連結
				_mbscat_s((unsigned char*)cFilePath, newSize + strConSize, (unsigned char*)strCon);


				//Hookが登録済ならマップチップは作成不要
				bool isHooked = false;
				//hookFuncになにも入っていない=sobjectにはなにもない
				if (hookFunc != nullptr) {
					isHooked = hookFunc(levelObjData);
				}
			}
		}
	}

	void LevelRender::MatrixTklToLevel()
	{
		m_tklFile.QuaryObject([&](TklFile::SObject& tklObj) {
			//コピー。
			//バインドポーズ。
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
			/// バインドポーズの逆行列
			/// 列3はVector3変換の際に不要なので初期化。
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

			//tklオブジェクト名の型をワイド文字に変換
			mbstowcs(boneName, tklObj.name.get(), 256);
			BonePtr bone = std::make_unique<Bone>(
				boneName,
				bindPoseMatrix,
				invBindPoseMatrix,
				tklObj.parentNo,
				tklObj.no
				);
			//ボーンを積む
			m_boneList.push_back(std::move(bone));
			});
	}
}