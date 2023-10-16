#include "PortalEnginePreCompile.h"
#include "graphics/ModelRender.h"

namespace nsPortalEngine {

	ModelRender::ModelRender()
	{

	}

	ModelRender::~ModelRender()
	{

	}

	void ModelRender::Init(
		const char* filePath,
		AnimationClip* animationClips,
		int numAnimationClips,
		const EnModelUpAxis enModelUpAxis,
		const bool isShadowCaster,
		const bool isShadowReceiver)
	{

		InitSkeleton(filePath);

		InitAnimation(animationClips, numAnimationClips);

		InitModel(filePath, enModelUpAxis, isShadowCaster, isShadowReceiver);

		InitZPrepassModel(filePath, enModelUpAxis);

		UpdateWorldMatrix();
	}

	void ModelRender::InitModelInitData(ModelInitData& modelInitData)
	{
		//�X�P���g����ݒ�B
		SetModelHasSkeleton(modelInitData);

		m_model.Init(modelInitData);
	}

	void ModelRender::InitSkeleton(const char* filePath)
	{
		//�X�P���g���f�[�^��ǂݍ���
		std::string skeletonFilePath = filePath;
		int pos = (int)skeletonFilePath.find(".tkm");
		skeletonFilePath.replace(pos, 4, ".tks");
		m_skeleton.Init(skeletonFilePath.c_str());
	}

	void ModelRender::InitAnimation(AnimationClip* animationClips, const int numAnimationClips)
	{
		if (animationClips != nullptr) {
			//�A�j���[�V�����̏������B
			m_animation.Init(m_skeleton, animationClips, numAnimationClips);
		}
	}

	void ModelRender::InitModel(const char* filePath, const EnModelUpAxis enModelUpAxis, const bool isShadowCaster, const bool isShadowReceiver)
	{
		//�ʏ탂�f�����������B
		ModelInitData modelInitData;
		modelInitData.m_tkmFilePath = filePath;
		modelInitData.m_modelUpAxis = enModelUpAxis;
		modelInitData.m_fxFilePath = "Assets/shader/model.fx";
		modelInitData.m_expandConstantBuffer = &RenderingEngine::GetInstance()->GetLightCB();
		modelInitData.m_expandConstantBufferSize = sizeof(RenderingEngine::GetInstance()->GetLightCB());
		modelInitData.m_expandShaderResoruceView[0] = &RenderingEngine::GetInstance()->GetShadowBlur().GetBokeTexture();

		//�X�P���g����ݒ�B
		SetModelHasSkeleton(modelInitData);

		//�e��ݒ�B
		if (isShadowReceiver) {
			modelInitData.m_psEntryPointFunc = "PSMainShadow";
		}
		else {
			modelInitData.m_psEntryPointFunc = "PSMain";
		}

		m_model.Init(modelInitData);

		for (int i = 0; i < PORTAL_NUM; i++) {
			m_portalModel[i].Init(modelInitData);
		}

		//�e��^����Ȃ�B
		if (isShadowCaster) {
			InitShadowModel(filePath, enModelUpAxis);
		}
	}

	void ModelRender::InitShadowModel(
		const char* tkmFilePath,
		EnModelUpAxis modelUpAxis
	)
	{
		ModelInitData shadowModelInitData;
		shadowModelInitData.m_fxFilePath = "Assets/shader/shadowMap.fx";
		shadowModelInitData.m_tkmFilePath = tkmFilePath;
		shadowModelInitData.m_modelUpAxis = modelUpAxis;
		shadowModelInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32_FLOAT;
		shadowModelInitData.m_expandConstantBuffer = &RenderingEngine::GetInstance()->GetShadowCB();
		shadowModelInitData.m_expandConstantBufferSize = sizeof(RenderingEngine::GetInstance()->GetShadowCB());

		//�X�P���g����ݒ�B
		SetModelHasSkeleton(shadowModelInitData);

		//�V���h�E���f�����������B
		m_shadowModel.Init(shadowModelInitData);
	}

	void ModelRender::InitZPrepassModel(
		const char* tkmFilePath,
		EnModelUpAxis modelUpAxis)
	{
		ModelInitData zprepassModelInitData;
		zprepassModelInitData.m_fxFilePath = "Assets/shader/zprepass.fx";
		zprepassModelInitData.m_tkmFilePath = tkmFilePath;
		zprepassModelInitData.m_modelUpAxis = modelUpAxis;
		zprepassModelInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32_FLOAT;

		//�X�P���g����ݒ�B
		SetModelHasSkeleton(zprepassModelInitData);

		//ZPrepass���f�����������B
		m_zprepassModel.Init(zprepassModelInitData);
	}

	void ModelRender::SetModelHasSkeleton(ModelInitData& modelInitData)
	{
		//�A�j���[�V�����L�胂�f���Ȃ�B
		if (m_skeleton.IsInited()) {
			//�X�P���g�����w�肷��B
			modelInitData.m_skeleton = &m_skeleton;
			modelInitData.m_vsSkinEntryPointFunc = "VSSkinMain";
		}
	}

	void ModelRender::UpdateWorldMatrix()
	{
		//���f���̍X�V�����B
		if (m_model.IsInited()) {
			m_model.UpdateWorldMatrix(m_position, m_rotation, m_scale);
		}
		if (m_shadowModel.IsInited()) {
			m_shadowModel.UpdateWorldMatrix(m_position, m_rotation, m_scale);
		}
		if (m_zprepassModel.IsInited()) {
			m_zprepassModel.UpdateWorldMatrix(m_position, m_rotation, m_scale);
		}
		if (m_portalModel[0].IsInited()) {
			for (int i = 0; i < PORTAL_NUM; i++) {
				m_portalModel[i].UpdateWorldMatrix(m_position, m_rotation, m_scale);
			}
		}
	}

	void ModelRender::Update()
	{
		//���f���̍X�V�����B
		UpdateWorldMatrix();

		//�X�P���g���̍X�V�����B
		if (m_skeleton.IsInited()) {
			m_skeleton.Update(m_model.GetWorldMatrix());
		}

		//�A�j���[�V�����̍X�V�����B
		m_animation.Progress(g_gameTime->GetFrameDeltaTime() * m_animationSpeed);
	}

	void ModelRender::Draw(RenderContext& rc)
	{
		RenderingEngine::GetInstance()->AddRenderObject(this);
	}

	void ModelRender::OnForwardRender(RenderContext& rc)
	{
		if (m_model.IsInited()) {
			m_model.Draw(rc, 1);
		}
	}

	void ModelRender::OnRenderShadowMap(RenderContext& rc, Camera& camera)
	{
		if (m_shadowModel.IsInited()) {
			m_shadowModel.Draw(rc, camera, 1);
		}
	}

	void ModelRender::OnRenderToZPrepass(RenderContext& rc)
	{
		if (m_zprepassModel.IsInited()) {
			m_zprepassModel.Draw(rc);
		}
	}

	void ModelRender::OnPortalRender(RenderContext& rc, const int num, Camera& camera)
	{
		if (m_portalModel[num].IsInited()) {
			m_portalModel[num].Draw(rc, camera, 1);
		}
	}
}