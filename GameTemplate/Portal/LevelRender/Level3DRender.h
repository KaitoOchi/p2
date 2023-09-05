#pragma once
#include "level3D/TklFile.h"


namespace nsPortalEngine {

	struct LevelObjectData {

		Vector3			position;		//座標。
		Quaternion		rotation;		//回転。
		Vector3			scale;			//拡大率。
		const wchar_t* name;			//名前。
		int				number = 0;		//番号。

		/// <summary>
		/// オブジェクト名を検索する(完全一致)。
		/// </summary>
		/// <param name="objName">オブジェクト名</param>
		/// <returns>完全一致ならtrue</returns>
		bool EqualObjectName(const wchar_t* objName)
		{
			return wcscmp(objName, name) == 0;
		}

		/// <summary>
		/// オブジェクト名を検索する（前方一致）。
		/// </summary>
		/// <param name="objName">オブジェクト名</param>
		/// <returns>前方一致ならtrue</returns>
		bool ForwardMatchName(const wchar_t* objName)
		{
			auto len = wcslen(objName);
			auto namelen = wcslen(name);

			if (len > namelen) {
				//名前が長いので不一致
				return false;
			}
			return wcsncmp(objName, name, len) == 0;
		}
	};

	/// <summary>
	/// レベルレンダークラス。
	/// </summary>
	class LevelRender : Noncopyable
	{
	public:
		LevelRender();
		~LevelRender();

		/// <summary>
		/// 初期化処理。
		/// </summary>
		/// <param name="filePath"></param>
		/// <param name="hookFunc"></param>
		void Init(const char* filePath, std::function<bool(LevelObjectData& objData)> hookFunc);

	private:
		/// <summary>
		/// Tklファイルの行列を変換する。
		/// </summary>
		void MatrixTklToLevel();

	private:
		using BonePtr = std::unique_ptr<Bone>;			//ボーンのポインタ
		std::vector<BonePtr>	m_boneList;				//ボーンのリスト
		TklFile					m_tklFile;				//tklファイル
	};
}
