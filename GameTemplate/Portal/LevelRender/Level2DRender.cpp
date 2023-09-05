#include "PortalEnginePreCompile.h"
#include "Level2DRender.h"
#include "Level2D/CaslFile.h"

namespace nsPortalEngine {

	void Level2DRender::Init(
		const char* filePath,
		std::function<bool(Level2DObjectData& objData)> hookFunc
	)
	{
		//Casl�t�@�C�������[�h����
		CaslFile casl;
		casl.Load(filePath);

		//�\�z
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
				//hook�֐����w�肳��Ă���̂�hook�֐����Ăяo���B
				isHook = hookFunc(objData);
			}
		}
	}
}
