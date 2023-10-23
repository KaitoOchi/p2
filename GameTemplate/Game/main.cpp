#include "stdafx.h"
#include "system/system.h"

#include "Game.h"
#include "PortalCamera.h"


// priority
// 0..Default
// 1..Player
// 2..GameCamera, PortalFrame
// 3..PortalCamera, PortalGun
//

/// <summary>
/// メイン関数
/// </summary>
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	// ゲームの初期化。
	InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Game"));

	NewGO<Game>(0, "game");

	//ポータルカメラを生成。
	PortalCamera* portalCamera = NewGO<PortalCamera>(3, "portalCamera");
	//レンダリングエンジンにポータルカメラのポインタを入れる。
	RenderingEngine::GetInstance()->SetPortalCameraPointer(portalCamera);

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

