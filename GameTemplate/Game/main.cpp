#include "stdafx.h"
#include "system/system.h"

#include "Game.h"
#include "PortalCamera.h"
#include "Fade.h"


// priority
// 0..Default
// 1..Player
// 2..GameCamera, PortalFrame
// 3..PortalCamera, PortalGun
//

/// <summary>
/// ���C���֐�
/// </summary>
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	// �Q�[���̏������B
	InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Game"));

	NewGO<Game>(0, "game");
	NewGO<Fade>(10, "fade");

	//�|�[�^���J�����𐶐��B
	PortalCamera* portalCamera = NewGO<PortalCamera>(3, "portalCamera");
	//�����_�����O�G���W���Ƀ|�[�^���J�����̃|�C���^������B
	RenderingEngine::GetInstance()->SetPortalCameraPointer(portalCamera);
	RenderingEngine::GetInstance()->SetLightCullingCamera(1, portalCamera->GetPortalCamera(0));
	RenderingEngine::GetInstance()->SetLightCullingCamera(2, portalCamera->GetPortalCamera(1));

	// ��������Q�[�����[�v�B
	while (DispatchWindowMessage())
	{
		PortalEngine::GetInstance()->Execute();
	}

	DeleteGO(portalCamera);
	PortalEngine::DeleteInstance();
	CollisionObjectManager::DeleteInstance();

	return 0;
}

