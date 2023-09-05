#pragma once

namespace nsPortalEngine {

	struct CaslData;

	struct Level2DObjectData {

		Vector3		position = Vector3::Zero;			//座標。
		Quaternion	rotation = Quaternion::Identity;	//回転。
		Vector3		scale = Vector3::One;				//拡大率。
		int			width = 0;							//横幅。
		int			height = 0;							//縦幅。
		const char* name;								//名前。
		const char* ddsFilePath;						//ddsファイルのファイルパス。
		Vector2		pivot = Sprite::DEFAULT_PIVOT;		//ピボット。

		/// <summary>
		/// 引数で渡したオブジェクト名のオブジェクトを調べる。
		/// </summary>
		/// <param name="objName"></param>
		/// <returns></returns>
		bool EqualObjectName(const char* objName) const
		{
			return strcmp(objName, name) == 0;
		}

		/// <summary>
		/// 名前が前方一致するか調べる。
		/// </summary>
		/// <param name="n"></param>
		/// <returns></returns>
		bool ForwardMatchName(const char* n) const
		{
			auto len = strlen(n);
			auto namelen = strlen(name);
			if (len > namelen) {
				//名前が長い。不一致。
				return false;
			}
			return strncmp(n, name, len) == 0;
		}
	};

	/// <summary>
	/// レベル2Dレンダークラス。
	/// </summary>
	class Level2DRender : Noncopyable
	{
	public:
		/// <summary>
		/// レベルを初期化。
		/// </summary>
		/// <param name="filePath">caslファイルのファイルパス。</param>
		/// <param name="hookFunc">オブジェクトを作詞するときの処理をフックするための関数。</param>
		void Init(const char* filePath, std::function<bool(Level2DObjectData& objData)> hookFunc);
	};
}

