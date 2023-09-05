#include "PortalEnginePreCompile.h"
#include "Level2DRender.h"
#include "Level2D/CaslFile.h"

namespace nsPortalEngine {

	void Level2DRender::Init(
		const char* filePath,
		std::function<bool(Level2DObjectData& objData)> hookFunc
	)
	{
		//Caslファイルをロードする
		CaslFile casl;
		casl.Load(filePath);

		//構築
		for (int i = 0; i < casl.GetNumCaslData(); i++) {

			auto caslData = casl.GetCaslData(i);

			Level2DObjectData objData;
			objData.scale.x = caslData->scale.x;
			objData.scale.y = caslData->scale.y;
			objData.position.x = caslData->position.x;
			objData.position.y = caslData->position.y;
			objData.width = caslData->width;
			objData.height = caslData->height;
			objData.name = caslData->name.get();
			objData.ddsFilePath = caslData->ddsFilePath.get();
			objData.pivot = caslData->pivot;

			auto isHook = false;
			if (hookFunc != nullptr) {
				//hook関数が指定されているのでhook関数を呼び出す。
				isHook = hookFunc(objData);
			}
		}
	}
}
