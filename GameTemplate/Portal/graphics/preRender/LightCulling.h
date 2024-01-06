#pragma once

namespace nsPortalEngine {

	/// <summary>
	/// ���C�g�J�����O�B
	/// </summary>
	class LightCulling : Noncopyable
	{
	public:
		LightCulling();
		~LightCulling();

		/// <summary>
		/// �����������B
		/// </summary>
		/// <param name="lightCB">���C�g�p�萔�o�b�t�@</param>
		/// <param name="zprepassRenderTargetTexture">Zprepass�p�e�N�X�`��</param>
		/// <param name="pointLightNoListInTileUAV">�|�C���g���C�g�pUAV</param>
		/// <param name="spotLightNoListInTileUAV">�X�|�b�g���C�g�pUAV</param>
		void Init(
			ConstantBuffer& lightCB,
			Texture& zprepassRenderTargetTexture,
			RWStructuredBuffer& pointLightNoListInTileUAV,
			RWStructuredBuffer& spotLightNoListInTileUAV,
			const int portalNo
		);

		void SetCamera(Camera& camera)
		{
			m_camera = &camera;
		}

		/// <summary>
		/// ���s�����B
		/// </summary>
		/// <param name="rc"></param>
		void Execute(RenderContext& rc);

	private:
		/// <summary>
		/// ���C�g�J�����O�p�J�������B
		/// </summary>
		struct LightCullingCameraData
		{
			Matrix	mProj;			//�v���W�F�N�V�����s��B
			Matrix	mProjInv;		//�v���W�F�N�V�����s��̋t�s��B
			Matrix	mCameraRot;		//�J�����̉�]�s��B
			Vector4 screenParam;	//�X�N���[�����B
		};

	private:
		Shader				m_lightCullingShader;						//�V�F�[�_�[�B
		RootSignature		m_rootSignature;							//���[�g�V�O�l�`���B
		DescriptorHeap		m_descriptorHeap;							//�f�B�X�N���v�^�q�[�v�B
		PipelineState		m_pipelineState;							//�p�C�v���C���X�e�[�g�B
		RWStructuredBuffer* m_pointLightNoListInTileUAV = nullptr;		//�|�C���g���C�g�pUAV�B
		RWStructuredBuffer* m_spotLightNoListInTileUAV = nullptr;		//�X�|�b�g���C�g�pUAV�B
		ConstantBuffer		m_cameraDataCB;								//���C�g�J�����O�p�萔�o�b�t�@�B
		Camera*				m_camera = nullptr;							//�J�����f�[�^�B
	};

}
