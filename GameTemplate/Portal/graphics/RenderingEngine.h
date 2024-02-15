#pragma once
#include "IRenderer.h"
#include "graphics/light/DirectionalLight.h"
#include "graphics/light/PointLight.h"
#include "graphics/light/SpotLight.h"
#include "PostEffect/PostEffect.h"
#include "preRender/lightCulling.h"
#include "../Game/Portal/PortalCamera.h"

#include  "ModelRender.h"

namespace nsPortalEngine {

	namespace
	{
		const int POINT_LIGHT_NUM = 256;		//ポイントライトの数。
		const int SPOT_LIGHT_NUM = 256;		//スポットライトの数。
		const int TILE_WIDTH = 16;			//タイルの幅。
		const int TILE_HEIGHT = 16;			//タイルの高さ。
		const int TILE_NUM = (FRAME_BUFFER_W / TILE_WIDTH) * (FRAME_BUFFER_H / TILE_HEIGHT);		//タイルの数。
	}

	/// <summary>
	/// レンダリングエンジン。
	/// </summary>
	class RenderingEngine : Noncopyable
	{
	public:
		/// <summary>
		/// シャドウ用定数バッファ。
		/// </summary>
		struct ShadowCB
		{
			Vector3 lightPos;
			float pad0;
			Matrix mLVP;
		};

		/// <summary>
		/// ライト用定数バッファ。
		/// </summary>
		struct LightCB
		{
			DirectionalLight::DirectionLig directionLig;		//ディレクションライト。
			Vector3 eyePos;										//視点の位置。
			float pad0;
			Vector3 eyePosBlue;									//視点の位置。
			float pad1;
			Vector3 eyePosRed;									//視点の位置。
			float pad2;
			PointLight::PointLig pointLig[POINT_LIGHT_NUM];		//ポイントライト。
			SpotLight::SpotLig spotLig[SPOT_LIGHT_NUM];			//スポットライト。
			ShadowCB shadowCB;									//シャドウ。
			Matrix mViewProjInv[3];								//ビュープロジェクション行列の逆行列。
			int ptNum = 0;										//ポイントライトの数。
			int spNum = 0;										//スポットライトの数。
		};

	private:
		RenderingEngine();
		~RenderingEngine();

	public:
		/// <summary>
		/// インスタンスの生成。
		/// </summary>
		static void CreateInstance()
		{
			if (m_instance != nullptr) {
				std::abort();
			}

			m_instance = new RenderingEngine;
			m_instance->Init();
		}

		/// <summary>
		/// インスタンスの破棄。
		/// </summary>
		static void DeleteInstance()
		{
			delete m_instance;
		}

		/// <summary>
		/// インスタンスの取得。
		/// </summary>
		/// <returns></returns>
		static RenderingEngine* GetInstance()
		{
			return m_instance;
		}

		/// <summary>
		/// レンダーオブジェクトを追加。
		/// </summary>
		/// <param name="renderObject"></param>
		void AddRenderObject(IRenderer* renderObject)
		{
			m_renderObjects.emplace_back(renderObject);
		}

		/// <summary>
		/// ポータルカメラのポインタを設定。
		/// </summary>
		/// <param name="portalCamera"></param>
		void SetPortalCameraPointer(PortalCamera* portalCamera)
		{
			m_portalCamera = portalCamera;
		}

		void SetLightCullingCamera(const int num, Camera& camera)
		{
			m_lightCulling[num].SetCamera(camera);
		}

		/// <summary>
		/// メインレンダーターゲットに戻す。
		/// </summary>
		/// <param name="rc"></param>
		void SetMainRenderTargetAndDepthStencilBuffer(RenderContext& rc)
		{
			rc.SetRenderTarget(m_mainRenderTarget[0].GetRTVCpuDescriptorHandle(), m_zprepassRenderTarget[0].GetDSVCpuDescriptorHandle());
		}

		/// <summary>
		/// ライト用定数バッファを取得。
		/// </summary>
		/// <returns></returns>
		LightCB& GetLightCB()
		{
			return m_lightCB;
		}

		/// <summary>
		/// シャドウ用の構造体を取得。
		/// </summary>
		/// <returns></returns>
		ShadowCB& GetShadowCB()
		{
			return m_lightCB.shadowCB;
		}

		/// <summary>
		/// ポイントライト用のUAVを取得。
		/// </summary>
		RWStructuredBuffer& GetPointLightNoListInTileUAV()
		{
			return m_pointLightNoListInTileUAV[0];
		}

		/// <summary>
		/// スポットライト用のUAVを取得。
		/// </summary>
		RWStructuredBuffer& GetSpotLightNoListInTileUAV()
		{
			return m_spotLightNoListInTileUAV[0];
		}

		/// <summary>
		/// ZPrepass用レンダーターゲットを取得。
		/// </summary>
		/// <returns></returns>
		RenderTarget& GetZPrepassRenderTarget()
		{
			return m_zprepassRenderTarget[0];
		}

		/// <summary>
		/// シャドウマップ用のボケ画像を取得。
		/// </summary>
		Texture& GetShadowBokeTexture()
		{
			return m_shadowBlur.GetBokeTexture();
		}

		/// <summary>
		/// ポストエフェクトを取得。
		/// </summary>
		/// <returns></returns>
		PostEffect& GetPostEffet()
		{
			return m_postEffect;
		}

		/// <summary>
		/// 実行処理。
		/// </summary>
		void Execute(RenderContext& rc);

	private:
		/// <summary>
		/// 初期化処理。
		/// </summary>
		void Init();
		/// <summary>
		/// ディファードライティング用スプライトの初期化処理。
		/// </summary>
		void InitDeferredLightingSprite();
		/// <summary>
		/// レンダーターゲットの初期化処理。
		/// </summary>
		void InitRenderTarget();
		/// <summary>
		/// GBufferの初期化処理。
		/// </summary>
		void InitGBuffer();
		/// <summary>
		/// メインレンダーターゲットの初期化処理。
		/// </summary>
		void InitMainRenderTarget();
		/// <summary>
		/// 2Dレンダーターゲットの初期化処理。
		/// </summary>
		void Init2DRenderTarget();
		/// <summary>
		/// シャドウ用レンダーターゲットの初期化処理。
		/// </summary>
		void InitShadowMapRenderTarget();
		/// <summary>
		/// ZPrepass用レンダーターゲットの初期化処理。
		/// </summary>
		void InitZPrepassRenderTarget();

		/// <summary>
		/// 定数バッファの更新処理。
		/// </summary>
		void UpdateCB();
		/// <summary>
		/// ZPrepassの描画処理。
		/// </summary>
		void DrawZPrepass(RenderContext& rc);
		/// <summary>
		/// シャドウマップの描画処理。
		/// </summary>
		/// <param name="rc"></param>
		void DrawShadowMap(RenderContext& rc);
		/// <summary>
		/// GBufferの描画処理。
		/// </summary>
		/// <param name="rc"></param>
		void DrawGBuffer(RenderContext& rc);
		/// <summary>
		/// ディファードライティングの描画処理。
		/// </summary>
		/// <param name="rc"></param>
		void DrawDeferredLighting(RenderContext& rc);
		/// <summary>
		/// フォワードライティングの描画処理。
		/// </summary>
		/// <param name="rc"></param>
		void DrawForwardLighting(RenderContext& rc);
		/// <summary>
		/// 2Dの描画処理。
		/// </summary>
		/// <param name="rc"></param>
		void Draw2D(RenderContext& rc);

	private:
		//GBufferの定義。
		enum EnGBuffer
		{
			enGBufferAlbedoDepth,           // rgbにアルベド、aに深度値。
			enGBufferNormal,                // rgbに法線、aにカメラ空間の法線。
			enGBufferMetaricShadowSmooth,   // rにメタリック、aにスムース。
			enGBufferShadowLightParam,		// rにシャドウ、gにライトパラメータ。
			enGBufferNum,                   // G-Bufferの数。
		};

	private:
		static RenderingEngine* m_instance;									//インスタンス。
		std::vector<IRenderer*> m_renderObjects;							//レンダーオブジェクト。
		RenderTarget			m_gBuffer[1 + PORTAL_NUM][enGBufferNum];	//GBuffer。
		RenderTarget			m_mainRenderTarget[1 + PORTAL_NUM];			//メインレンダーターゲット。
		RenderTarget			m_2DRenderTarget;							//2D用レンダーターゲット。
		RenderTarget			m_shadowMapRenderTarget;					//シャドウマップ用レンダーターゲット。
		RenderTarget			m_zprepassRenderTarget[1 + PORTAL_NUM];		//ZPrepass用レンダーターゲット。
		Sprite					m_deferredLightingSprite[1 + PORTAL_NUM];	//ディファードライティング用スプライト。
		Sprite					m_2DSprite;									//2Dスプライト。
		Sprite					m_mainSprite;								//メインスプライト。
		GaussianBlur			m_shadowBlur;								//シャドウ用ガウシアンブラー。
		Camera					m_lightCamera;								//ライトカメラ。
		PostEffect				m_postEffect;								//ポストエフェクト。
		LightCulling			m_lightCulling[1 + PORTAL_NUM];				//ライトカリング。
		LightCB					m_lightCB;									//ライト用構造体。
		RWStructuredBuffer		m_pointLightNoListInTileUAV[1 + PORTAL_NUM];//ポイントライト用UAV。
		RWStructuredBuffer		m_spotLightNoListInTileUAV[1 + PORTAL_NUM];	//スポットライト用UAV。
		PortalCamera*			m_portalCamera = nullptr;					//ポータル用カメラ。
	};
}
