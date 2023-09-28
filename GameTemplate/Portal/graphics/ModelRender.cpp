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
		EnModelUpAxis enModelUpAxis,
		const bool isShadowCaster,
		const bool isShadowReceiver)
	{
		InitSkeleton(filePath);

		InitAnimation(animationClips, numAnimationClips);

		InitModel(filePath, enModelUpAxis, isShadowCaster, isShadowReceiver);

		UpdateWorldMatrix();
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

	void ModelRender::InitModel(const char* filePath, EnModelUpAxis enModelUpAxis, const bool isShadowCaster, const bool isShadowReceiver)
	{
		//�ʏ탂�f�����������B
		ModelInitData modelInitData;
		modelInitData.m_tkmFilePath = filePath;
		modelInitData.m_modelUpAxis = enModelUpAxis;
		modelInitData.m_fxFilePath = "Assets/shader/model.fx";
		modelInitData.m_expandConstantBuffer = &RenderingEngine::GetInstance()->GetLightCB();
		modelInitData.m_expandConstantBufferSize = sizeof(RenderingEngine::GetInstance()->GetLightCB());
		modelInitData.m_expandShaderResoruceView[0] = &RenderingEngine::GetInstance()->GetShadowBlur().GetBokeTexture();

		//�A�j���[�V�����L�胂�f���Ȃ�
		if (m_skeleton.IsInited()) {
			//�X�P���g�����w�肷��B
			modelInitData.m_skeleton = &m_skeleton;
			modelInitData.m_vsSkinEntryPointFunc = "VSSkinMain";
		}

		//�e��ݒ�
		if (isShadowReceiver) {
			modelInitData.m_psEntryPointFunc = "PSMainShadow";
		}
		else {
			modelInitData.m_psEntryPointFunc = "PSMain";
		}

		m_model.Init(modelInitData);
		m_portalModel.Init(modelInitData);


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


		if (m_skeleton.IsInited()) {
			//�X�P���g�����w�肷��B
			shadowModelInitData.m_skeleton = &m_skeleton;
			shadowModelInitData.m_vsSkinEntryPointFunc = "VSSkinMain";
		}

		m_shadowModel.Init(shadowModelInitData);
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
		if (m_portalModel.IsInited()) {
			m_portalModel.UpdateWorldMatrix(m_position, m_rotation, m_scale);
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

	void ModelRender::OnPortalRender(RenderContext& rc, Camera& camera)
	{
		if (m_model.IsInited()) {
			m_portalModel.Draw(rc, camera, 1);
		}
	}
}