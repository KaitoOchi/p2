#include "PortalEnginePreCompile.h"
#include "PortalEngine.h"

namespace nsPortalEngine {

	PortalEngine* PortalEngine::m_instance = nullptr;

	PortalEngine::PortalEngine()
	{

	}

	PortalEngine::~PortalEngine()
	{
		g_engine = nullptr;

		RenderingEngine::DeleteInstance();

		CollisionObjectManager::DeleteInstance();
	}

	void PortalEngine::Init(const InitData& initData)
	{
		//g_engine = &m_k2EngineLow;

		m_k2EngineLow.Init(
			initData.hwnd,
			initData.frameBufferWidth,
			initData.frameBufferHeight
		);

		//カメラの設定。
		g_camera3D->SetPosition({ 0.0f, 100.0f, -200.0f });
		g_camera3D->SetTarget({ 0.0f, 50.0f, 0.0f });
		g_camera3D->Update();

		//レンダリングエンジンの生成。
		RenderingEngine::CreateInstance();

		//コリジョンオブジェクトマネージャーの生成。
		CollisionObjectManager::CreateInstance();
	}

	void PortalEngine::Execute()
	{
		auto& renderContext = g_graphicsEngine->GetRenderContext();

		// フレームの開始時に呼び出す必要がある処理を実行
		g_engine->BeginFrame();

		// ゲームオブジェクトマネージャーの更新処理を呼び出す。
		g_engine->ExecuteUpdate();

		// ゲームオブジェクトマネージャーの描画処理を呼び出す。
		g_engine->ExecuteRender();

		//レンダリングエンジンを実行。
		RenderingEngine::GetInstance()->Execute(renderContext);

		// デバッグ描画処理を実行する。
		g_engine->DebubDrawWorld();

		// フレームの終了時に呼び出す必要がある処理を実行。
		g_engine->EndFrame();
	}
}