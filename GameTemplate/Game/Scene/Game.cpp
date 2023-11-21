#include "stdafx.h"
#include "Game.h"
#include "Player.h"
#include "PortalFrame.h"
#include "PortalCamera.h"
#include "Stage.h"


Game::Game()
{
}

Game::~Game()
{
	//�|�[�^���t���[�����폜�B
	QueryGOs<PortalFrame>("portalFrame", [&](PortalFrame* portalFrame) {
		DeleteGO(portalFrame);
		return true;
	});
}

bool Game::Start()
{
	//�^���b�g�̎ˌ��G�t�F�N�g�B
	EffectEngine::GetInstance()->ResistEffect(EffectEmitter::enEffect_Turret_Shot, u"Assets/effect/turret/turret_shot.efk");
	//�G�i�W�[�{�[���̃G�t�F�N�g�B
	EffectEngine::GetInstance()->ResistEffect(EffectEmitter::enEffect_EnergyBall, u"Assets/effect/energyBall/energyBall.efk");


	//�t���[�����[�g���Œ�
	//g_engine->SetFrameRateMode(K2EngineLow::enFrameRateMode_Fix, 60);

	//�����蔻���L����
	PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();

	//�|�[�^���J�����������B
	PortalCamera* portalCamera = FindGO<PortalCamera>("portalCamera");

	//�v���C���[�𐶐��B
	NewGO<Player>(1, "player");

	//�|�[�^���𐶐��B
	PortalFrame* portalFrame[PORTAL_NUM];
	for (int i = 0; i < PORTAL_NUM; i++) {
		portalFrame[i] = NewGO<PortalFrame>(2, "portalFrame");

		//�|�[�^���J�����Ƀ|�[�^���̃|�C���^��ݒ�B
		portalCamera->SetPortalFramePointer(portalFrame[i], i);
	}
	portalFrame[0]->SetAnotherPortal(portalFrame[1]);
	portalFrame[0]->SetPortalType(PortalFrame::enPortalType_Blue);
	portalFrame[1]->SetAnotherPortal(portalFrame[0]);
	portalFrame[1]->SetPortalType(PortalFrame::enPortalType_Red);

	//�X�e�[�W�𐶐��B
	m_stage = NewGO<Stage>(0, "stage");

	//�|�C���g���C�g��ݒ�B
	m_pointLight.SetPointLight(
		0,
		Vector3(-100.0f, 50.0f, -100.0f),
		Vector3(0.0f, 0.0f, 1.0f),
		200.0f
	);
	RenderingEngine::GetInstance()->GetLightCB().ptNum = 1;

	//�X�|�b�g���C�g��ݒ�B
	m_spotLight.SetSpotLight(
		0,
		Vector3(-100.0f, 20.0f, 100.0f),
		Vector3(0.0f, 2.0f, 2.0f),
		300.0f,
		Vector3(1.0f, -1.0f, 1.0f),
		70.0f
	);
	RenderingEngine::GetInstance()->GetLightCB().spNum = 1;

	return true;
}

void Game::Update()
{

}

void Game::NotifyReset()
{

}

const Vector3& Game::GetRespawnPoint() const
{
	return m_stage->GetRespawnPoint();
}

void Game::Render(RenderContext& rc)
{
}