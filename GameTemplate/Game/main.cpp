#include "stdafx.h"
#include "system/system.h"

#include "Game.h"
#include "Title.h"
#include "PortalCamera.h"
#include "Fade.h"

/// <summary>
/// メイン関数
/// </summary>
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	// ゲームの初期化。
	InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Game"));

	//NewGO<Game>(0, "game");
	NewGO<Fade>(10, "fade");
	NewGO<Title>(0, "title");

	//ポータルカメラを生成。
	PortalCamera* portalCamera = NewGO<PortalCamera>(3, "portalCamera");
	//レンダリングエンジンにポータルカメラのポインタを入れる。
	RenderingEngine::GetInstance()->SetPortalCameraPointer(portalCamera);
	RenderingEngine::GetInstance()->SetLightCullingCamera(1, portalCamera->GetPortalCamera(0));
	RenderingEngine::GetInstance()->SetLightCullingCamera(2, portalCamera->GetPortalCamera(1));

	// ここからゲームループ。
	while (DispatchWindowMessage())
	{
		PortalEngine::GetInstance()->Execute();
	}

	DeleteGO(portalCamera);
	PortalEngine::DeleteInstance();
	CollisionObjectManager::DeleteInstance();

	return 0;
}

