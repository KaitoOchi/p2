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

		//�J�����̐ݒ�B
		g_camera3D->SetPosition({ 0.0f, 100.0f, -200.0f });
		g_camera3D->SetTarget({ 0.0f, 50.0f, 0.0f });
		g_camera3D->Update();

		//�����_�����O�G���W���̐����B
		RenderingEngine::CreateInstance();

		//�R���W�����I�u�W�F�N�g�}�l�[�W���[�̐����B
		CollisionObjectManager::CreateInstance();
	}

	void PortalEngine::Execute()
	{
		auto& renderContext = g_graphicsEngine->GetRenderContext();

		// �t���[���̊J�n���ɌĂяo���K�v�����鏈�������s
		g_engine->BeginFrame();

		// �Q�[���I�u�W�F�N�g�}�l�[�W���[�̍X�V�������Ăяo���B
		g_engine->ExecuteUpdate();

		// �Q�[���I�u�W�F�N�g�}�l�[�W���[�̕`�揈�����Ăяo���B
		g_engine->ExecuteRender();

		//�����_�����O�G���W�������s�B
		RenderingEngine::GetInstance()->Execute(renderContext);

		// �f�o�b�O�`�揈�������s����B
		g_engine->DebubDrawWorld();

		// �t���[���̏I�����ɌĂяo���K�v�����鏈�������s�B
		g_engine->EndFrame();
	}
}