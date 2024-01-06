#pragma once

namespace nsPortalEngine {

	/// <summary>
	/// ライトカリング。
	/// </summary>
	class LightCulling : Noncopyable
	{
	public:
		LightCulling();
		~LightCulling();

		/// <summary>
		/// 初期化処理。
		/// </summary>
		/// <param name="lightCB">ライト用定数バッファ</param>
		/// <param name="zprepassRenderTargetTexture">Zprepass用テクスチャ</param>
		/// <param name="pointLightNoListInTileUAV">ポイントライト用UAV</param>
		/// <param name="spotLightNoListInTileUAV">スポットライト用UAV</param>
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
		/// 実行処理。
		/// </summary>
		/// <param name="rc"></param>
		void Execute(RenderContext& rc);

	private:
		/// <summary>
		/// ライトカリング用カメラ情報。
		/// </summary>
		struct LightCullingCameraData
		{
			Matrix	mProj;			//プロジェクション行列。
			Matrix	mProjInv;		//プロジェクション行列の逆行列。
			Matrix	mCameraRot;		//カメラの回転行列。
			Vector4 screenParam;	//スクリーン情報。
		};

	private:
		Shader				m_lightCullingShader;						//シェーダー。
		RootSignature		m_rootSignature;							//ルートシグネチャ。
		DescriptorHeap		m_descriptorHeap;							//ディスクリプタヒープ。
		PipelineState		m_pipelineState;							//パイプラインステート。
		RWStructuredBuffer* m_pointLightNoListInTileUAV = nullptr;		//ポイントライト用UAV。
		RWStructuredBuffer* m_spotLightNoListInTileUAV = nullptr;		//スポットライト用UAV。
		ConstantBuffer		m_cameraDataCB;								//ライトカリング用定数バッファ。
		Camera*				m_camera = nullptr;							//カメラデータ。
	};

}
