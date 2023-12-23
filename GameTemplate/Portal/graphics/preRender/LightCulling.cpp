#include "PortalEnginePreCompile.h"
#include "LightCulling.h"

namespace nsPortalEngine {

	LightCulling::LightCulling()
	{

	}

	LightCulling::~LightCulling()
	{

	}

	void LightCulling::Init(
		ConstantBuffer& lightCB,
		Texture& zprepassRenderTargetTexture,
		RWStructuredBuffer& pointLightNoListInTileUAV,
		RWStructuredBuffer& spotLightNoListInTileUAV
	) {
		m_pointLightNoListInTileUAV = &pointLightNoListInTileUAV;
		m_spotLightNoListInTileUAV = &spotLightNoListInTileUAV;

		//シェーダーをロード。
		m_lightCullingShader.LoadCS("Assets/shader/preProcess/lightCulling.fx", "CSMain");

		//ルートシグネチャを作成。
		m_rootSignature.Init(D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP);

		//ライトカリング用のパイプラインステートを作成。
		D3D12_COMPUTE_PIPELINE_STATE_DESC  psoDesc = { 0 };
		psoDesc.pRootSignature = m_rootSignature.Get();
		psoDesc.CS = CD3DX12_SHADER_BYTECODE(m_lightCullingShader.GetCompiledBlob());
		psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		psoDesc.NodeMask = 0;
		m_pipelineState.Init(psoDesc);

		//定数バッファを初期化。
		m_cameraDataCB.Init(sizeof(LightCullingCameraData), nullptr);

		//ライトカリング用のディスクリプタヒープを作成。
		m_descriptorHeap.RegistShaderResource(0, zprepassRenderTargetTexture);
		m_descriptorHeap.RegistUnorderAccessResource(0, pointLightNoListInTileUAV);
		m_descriptorHeap.RegistUnorderAccessResource(1, spotLightNoListInTileUAV);
		m_descriptorHeap.RegistConstantBuffer(0, m_cameraDataCB);
		m_descriptorHeap.RegistConstantBuffer(1, lightCB);
		m_descriptorHeap.Commit();
	}

	void LightCulling::Execute(RenderContext& rc)
	{
		//ポイントライトの情報を送るための定数バッファを作成。
		LightCullingCameraData lightCullingCameraData;
		lightCullingCameraData.mProj = m_camera->GetProjectionMatrix();
		lightCullingCameraData.mProjInv.Inverse(lightCullingCameraData.mProj);
		lightCullingCameraData.mCameraRot = m_camera->GetCameraRotation();
		lightCullingCameraData.screenParam.x = m_camera->GetNear();
		lightCullingCameraData.screenParam.y = m_camera->GetFar();
		lightCullingCameraData.screenParam.z = FRAME_BUFFER_W;
		lightCullingCameraData.screenParam.w = FRAME_BUFFER_H;
		m_cameraDataCB.CopyToVRAM(lightCullingCameraData);

		//ライトカリングのコンピュートシェーダーをディスパッチ。
		rc.SetComputeRootSignature(m_rootSignature);
		rc.SetComputeDescriptorHeap(m_descriptorHeap);
		rc.SetPipelineState(m_pipelineState);

		// フレームバッファの幅と高さをTILE_WIDTH、TILE_HEIGHTの倍数に切り上げる。
		UINT FRAME_BUFFER_W_ROUNDUP = ((FRAME_BUFFER_W + TILE_WIDTH - 1) / TILE_WIDTH) * TILE_WIDTH;
		UINT FRAME_BUFFER_H_ROUNDUP = ((FRAME_BUFFER_H + TILE_HEIGHT - 1) / TILE_HEIGHT) * TILE_HEIGHT;

		rc.Dispatch(
			FRAME_BUFFER_W_ROUNDUP / TILE_WIDTH,
			FRAME_BUFFER_H_ROUNDUP / TILE_HEIGHT,
			1
		);

		//リソースバリア。
		rc.TransitionResourceState(
			m_pointLightNoListInTileUAV->GetD3DResoruce(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);
	}
}