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

		//�V�F�[�_�[�����[�h�B
		m_lightCullingShader.LoadCS("Assets/shader/preProcess/lightCulling.fx", "CSMain");

		//���[�g�V�O�l�`�����쐬�B
		m_rootSignature.Init(D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP);

		//���C�g�J�����O�p�̃p�C�v���C���X�e�[�g���쐬�B
		D3D12_COMPUTE_PIPELINE_STATE_DESC  psoDesc = { 0 };
		psoDesc.pRootSignature = m_rootSignature.Get();
		psoDesc.CS = CD3DX12_SHADER_BYTECODE(m_lightCullingShader.GetCompiledBlob());
		psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		psoDesc.NodeMask = 0;
		m_pipelineState.Init(psoDesc);

		//�萔�o�b�t�@���������B
		m_cameraDataCB.Init(sizeof(LightCullingCameraData), nullptr);

		//���C�g�J�����O�p�̃f�B�X�N���v�^�q�[�v���쐬�B
		m_descriptorHeap.RegistShaderResource(0, zprepassRenderTargetTexture);
		m_descriptorHeap.RegistUnorderAccessResource(0, pointLightNoListInTileUAV);
		m_descriptorHeap.RegistUnorderAccessResource(1, spotLightNoListInTileUAV);
		m_descriptorHeap.RegistConstantBuffer(0, m_cameraDataCB);
		m_descriptorHeap.RegistConstantBuffer(1, lightCB);
		m_descriptorHeap.Commit();
	}

	void LightCulling::Execute(RenderContext& rc)
	{
		//�|�C���g���C�g�̏��𑗂邽�߂̒萔�o�b�t�@���쐬�B
		LightCullingCameraData lightCullingCameraData;
		lightCullingCameraData.mProj = m_camera->GetProjectionMatrix();
		lightCullingCameraData.mProjInv.Inverse(lightCullingCameraData.mProj);
		lightCullingCameraData.mCameraRot = m_camera->GetCameraRotation();
		lightCullingCameraData.screenParam.x = m_camera->GetNear();
		lightCullingCameraData.screenParam.y = m_camera->GetFar();
		lightCullingCameraData.screenParam.z = FRAME_BUFFER_W;
		lightCullingCameraData.screenParam.w = FRAME_BUFFER_H;
		m_cameraDataCB.CopyToVRAM(lightCullingCameraData);

		//���C�g�J�����O�̃R���s���[�g�V�F�[�_�[���f�B�X�p�b�`�B
		rc.SetComputeRootSignature(m_rootSignature);
		rc.SetComputeDescriptorHeap(m_descriptorHeap);
		rc.SetPipelineState(m_pipelineState);

		// �t���[���o�b�t�@�̕��ƍ�����TILE_WIDTH�ATILE_HEIGHT�̔{���ɐ؂�グ��B
		UINT FRAME_BUFFER_W_ROUNDUP = ((FRAME_BUFFER_W + TILE_WIDTH - 1) / TILE_WIDTH) * TILE_WIDTH;
		UINT FRAME_BUFFER_H_ROUNDUP = ((FRAME_BUFFER_H + TILE_HEIGHT - 1) / TILE_HEIGHT) * TILE_HEIGHT;

		rc.Dispatch(
			FRAME_BUFFER_W_ROUNDUP / TILE_WIDTH,
			FRAME_BUFFER_H_ROUNDUP / TILE_HEIGHT,
			1
		);

		//���\�[�X�o���A�B
		rc.TransitionResourceState(
			m_pointLightNoListInTileUAV->GetD3DResoruce(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);
	}
}