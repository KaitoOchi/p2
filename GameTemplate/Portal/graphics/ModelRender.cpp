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
		const bool isShadowReceiver,
		const ModelInitMode enModelInitMode
	)
	{

		InitSkeleton(filePath);

		InitAnimation(animationClips, numAnimationClips);

		InitModel(filePath, enModelUpAxis, isShadowCaster, isShadowReceiver, enModelInitMode);

		UpdateWorldMatrix();
	}

	void ModelRender::InitModelInitData(ModelInitData& modelInitData)
	{
		InitZPrepassModel(modelInitData.m_tkmFilePath, modelInitData.m_modelUpAxis);

		m_gBufferModel.Init(modelInitData);

		//ポータル用のモデルを初期化。
		for (int i = 0; i < PORTAL_NUM; i++) {
			modelInitData.m_fxFilePath = "Assets/shader/preProcess/RenderToGBuffer.fx";
			m_portalModel[i].Init(modelInitData);
		}

		UpdateWorldMatrix();
	}

	void ModelRender::InitSkeleton(const char* filePath)
	{
		//スケルトンデータを読み込む
		std::string skeletonFilePath = filePath;
		int pos = (int)skeletonFilePath.find(".tkm");
		skeletonFilePath.replace(pos, 4, ".tks");
		m_skeleton.Init(skeletonFilePath.c_str());
	}

	void ModelRender::InitAnimation(AnimationClip* animationClips, const int numAnimationClips)
	{
		if (animationClips != nullptr) {
			//アニメーションの初期化。
			m_animation.Init(m_skeleton, animationClips, numAnimationClips);
		}
	}

	void ModelRender::InitModel(
		const char* filePath,
		const EnModelUpAxis enModelUpAxis,
		const bool isShadowCaster,
		const bool isShadowReceiver,
		const ModelInitMode enModelInitMode
	)
	{
		//通常モデルを初期化。
		ModelInitData modelInitData;
		modelInitData.m_tkmFilePath = filePath;
		modelInitData.m_modelUpAxis = enModelUpAxis;
		modelInitData.m_expandConstantBuffer = &RenderingEngine::GetInstance()->GetLightCB();
		modelInitData.m_expandConstantBufferSize = sizeof(RenderingEngine::GetInstance()->GetLightCB());
		modelInitData.m_expandShaderResoruceView[0] = &RenderingEngine::GetInstance()->GetZPrepassRenderTarget().GetRenderTargetTexture();
		modelInitData.m_expandShaderResoruceView[1] = &RenderingEngine::GetInstance()->GetShadowBokeTexture();

		//スケルトンを設定。
		SetModelHasSkeleton(modelInitData);

		//影を設定。
		if (isShadowReceiver) {
			modelInitData.m_psEntryPointFunc = "PSMainShadow";
		}
		else {
			modelInitData.m_psEntryPointFunc = "PSMain";
		}

		switch (enModelInitMode)
		{
		//ディファードライティング。
		case enModelInitMode_DiferredLighting:
			modelInitData.m_fxFilePath = "Assets/shader/preProcess/RenderToGBuffer.fx";
			m_gBufferModel.Init(modelInitData);

			InitZPrepassModel(filePath, enModelUpAxis);
			break;

		//フォワードライティング。
		case enModelInitMode_ForwardLighting:
			modelInitData.m_fxFilePath = "Assets/shader/ForwardLighting.fx";
			//ポイントライトとスポットライトのUAV。
			modelInitData.m_expandShaderResoruceView[2] = &RenderingEngine::GetInstance()->GetPointLightNoListInTileUAV();
			modelInitData.m_expandShaderResoruceView[3] = &RenderingEngine::GetInstance()->GetSpotLightNoListInTileUAV();
			m_forwardRenderModel.Init(modelInitData);

			InitZPrepassModel(filePath, enModelUpAxis);
			break;

		//メインカメラに描画しない。
		case enModelInitMode_UnDrawMainCamera:
			break;

		//輪郭線。
		case enModelInitMode_Outline:
			modelInitData.m_fxFilePath = "Assets/shader/preProcess/RenderToGBuffer.fx";
			modelInitData.m_psEntryPointFunc = "PSMainOutline";
			m_gBufferModel.Init(modelInitData);
			InitZPrepassModel(filePath, enModelUpAxis);
			break;

		//ディザリング。
		case enModelInitMode_Dithering:
			modelInitData.m_fxFilePath = "Assets/shader/preProcess/RenderToGBuffer.fx";
			modelInitData.m_psEntryPointFunc = "PSMainDithering";
			m_gBufferModel.Init(modelInitData);

			InitZPrepassModel(filePath, enModelUpAxis);
			break;
		}

		//ポータル用のモデルを初期化。
		for (int i = 0; i < PORTAL_NUM; i++) {
			modelInitData.m_fxFilePath = "Assets/shader/preProcess/RenderToGBuffer.fx";
			m_portalModel[i].Init(modelInitData);
		}

		//影を与えるなら。
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
		shadowModelInitData.m_fxFilePath = "Assets/shader/preProcess/shadowMap.fx";
		shadowModelInitData.m_tkmFilePath = tkmFilePath;
		shadowModelInitData.m_modelUpAxis = modelUpAxis;
		shadowModelInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32_FLOAT;
		shadowModelInitData.m_expandConstantBuffer = &RenderingEngine::GetInstance()->GetShadowCB();
		shadowModelInitData.m_expandConstantBufferSize = sizeof(RenderingEngine::GetInstance()->GetShadowCB());

		//スケルトンを設定。
		SetModelHasSkeleton(shadowModelInitData);

		//シャドウモデルを初期化。
		m_shadowModel.Init(shadowModelInitData);
	}

	void ModelRender::InitZPrepassModel(
		const char* tkmFilePath,
		EnModelUpAxis modelUpAxis)
	{
		ModelInitData zprepassModelInitData;
		zprepassModelInitData.m_fxFilePath = "Assets/shader/preProcess/zPrepass.fx";
		zprepassModelInitData.m_tkmFilePath = tkmFilePath;
		zprepassModelInitData.m_modelUpAxis = modelUpAxis;
		zprepassModelInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32_FLOAT;

		//スケルトンを設定。
		SetModelHasSkeleton(zprepassModelInitData);

		//ZPrepassモデルを初期化。
		for (int i = 0; i <= PORTAL_NUM; i++) {
			m_zprepassModel[i].Init(zprepassModelInitData);
		}
	}

	void ModelRender::SetModelHasSkeleton(ModelInitData& modelInitData)
	{
		//アニメーション有りモデルなら。
		if (m_skeleton.IsInited()) {
			//スケルトンを指定する。
			modelInitData.m_skeleton = &m_skeleton;
			modelInitData.m_vsSkinEntryPointFunc = "VSSkinMain";
		}
	}

	void ModelRender::UpdateWorldMatrix()
	{
		//モデルの更新処理。
		if (m_gBufferModel.IsInited()) {
			m_gBufferModel.UpdateWorldMatrix(m_position, m_rotation, m_scale);
		}
		if (m_forwardRenderModel.IsInited()) {
			m_forwardRenderModel.UpdateWorldMatrix(m_position, m_rotation, m_scale);
		}
		if (m_shadowModel.IsInited()) {
			m_shadowModel.UpdateWorldMatrix(m_position, m_rotation, m_scale);
		}
		if (m_zprepassModel[0].IsInited()) {
			for (int i = 0; i <= PORTAL_NUM; i++) {
				m_zprepassModel[i].UpdateWorldMatrix(m_position, m_rotation, m_scale);
			}
		}
		if (m_portalModel[0].IsInited()) {
			for (int i = 0; i < PORTAL_NUM; i++) {
				m_portalModel[i].UpdateWorldMatrix(m_position, m_rotation, m_scale);
			}
		}
	}

	void ModelRender::Update()
	{
		//モデルの更新処理。
		UpdateWorldMatrix();

		//スケルトンの更新処理。
		if (m_skeleton.IsInited()) {
			m_skeleton.Update(m_portalModel[0].GetWorldMatrix());
		}

		//アニメーションの更新処理。
		m_animation.Progress(g_gameTime->GetFrameDeltaTime() * m_animationSpeed);
	}

	void ModelRender::Draw(RenderContext& rc)
	{
		RenderingEngine::GetInstance()->AddRenderObject(this);
	}

	void ModelRender::OnRenderGBuffer(RenderContext& rc)
	{
		if (m_gBufferModel.IsInited()) {
			m_gBufferModel.Draw(rc);
		}
	}

	void ModelRender::OnForwardRender(RenderContext& rc)
	{
		if (m_forwardRenderModel.IsInited()) {
			m_forwardRenderModel.Draw(rc, 1);
		}
	}

	void ModelRender::OnRenderShadowMap(RenderContext& rc, Camera& camera)
	{
		if (m_shadowModel.IsInited()) {
			m_shadowModel.Draw(rc, camera, 1);
		}
	}

	void ModelRender::OnRenderToZPrepass(RenderContext& rc, const int num, Camera& camera)
	{
		if (m_zprepassModel[num].IsInited()) {
			m_zprepassModel[num].Draw(rc, camera, 1);
		}
	}

	void ModelRender::OnPortalRender(RenderContext& rc, const int num, Camera& camera)
	{
		if (m_portalModel[num].IsInited()) {
			m_portalModel[num].Draw(rc, camera, 1);
		}
	}
}