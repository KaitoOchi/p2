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
		ModelInitData modelInitData;
		modelInitData.m_tkmFilePath = filePath;
		modelInitData.m_modelUpAxis = enModelUpAxis;
		modelInitData.m_fxFilePath = "Assets/shader/model.fx";
		//modelInitData.m_expandConstantBuffer = 

		//�e�𗎂Ƃ��Ȃ�B
		if (isShadowReceiver) {

		}
		else {

		}

		//�e��^����Ȃ�B
		if (isShadowCaster) {

		}

		m_model.Init(modelInitData);
	}

	void ModelRender::UpdateWorldMatrix()
	{
		//���f���̍X�V�����B
		if (m_model.IsInited()) {
			m_model.UpdateWorldMatrix(m_position, m_rotation, m_scale);
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
}