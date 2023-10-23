#pragma once
#include "IRenderer.h"
#include "graphics/light/DirectionalLight.h"
#include "graphics/light/PointLight.h"
#include "graphics/light/SpotLight.h"
#include "PostEffect/PostEffect.h"
#include "../Game/Portal/PortalCamera.h"

namespace nsPortalEngine {

	namespace
	{
		const int POINT_LIGHT_NUM = 4;		//ポイントライトの数。
		const int SPOT_LIGHT_NUM = 4;		//スポットライトの数。
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
			float pad0 = 0.0f;
			PointLight::PointLig pointLig[POINT_LIGHT_NUM];		//ポイントライト。
			SpotLight::SpotLig spotLig[SPOT_LIGHT_NUM];			//スポットライト。
			ShadowCB shadowCB;									//シャドウ。
			Matrix mViewProjInv;								//ビュープロジェクション行列の逆行列。
			int ptNum = POINT_LIGHT_NUM;						//ポイントライトの数。
			int spNum = SPOT_LIGHT_NUM;							//スポットライトの数。
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

		/// <summary>
		/// メインレンダーターゲットに戻す。
		/// </summary>
		/// <param name="rc"></param>
		void SetMainRenderTargetAndDepthStencilBuffer(RenderContext& rc)
		{
			rc.SetRenderTarget(m_mainRenderTarget.GetRTVCpuDescriptorHandle(), m_zprepassRenderTarget.GetDSVCpuDescriptorHandle());
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
		/// ポータル用レンダーターゲットを取得。
		/// </summary>
		/// <param name="num"></param>
		/// <returns></returns>
		RenderTarget& GetPortalRenderTarget(const int num)
		{
			return m_portalRenderTarget[num];
		}

		/// <summary>
		/// ZPrepass用レンダーターゲットを取得。
		/// </summary>
		/// <returns></returns>
		RenderTarget& GetZPrepassRenderTarget()
		{
			return m_zprepassRenderTarget;
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
		/// ポータル用レンダーターゲットの初期化処理。
		/// </summary>
		void InitPortalRenderTarget();
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
			enGBufferMetaricShadowSmooth,   // rにメタリック、gに影パラメータ、bにライティングパラメータ、aにスムース。
			enGBufferNum,                   // G-Bufferの数。
		};

	private:
		static RenderingEngine* m_instance;									//インスタンス。
		std::vector<IRenderer*> m_renderObjects;							//レンダーオブジェクト。
		RenderTarget			m_gBuffer[1 + PORTAL_NUM][enGBufferNum];	//GBuffer。
		RenderTarget			m_mainRenderTarget;							//メインレンダーターゲット。
		RenderTarget			m_2DRenderTarget;							//2D用レンダーターゲット。
		RenderTarget			m_portalRenderTarget[PORTAL_NUM];			//ポータル用レンダーターゲット。
		RenderTarget			m_shadowMapRenderTarget;					//シャドウマップ用レンダーターゲット。
		RenderTarget			m_zprepassRenderTarget;						//ZPrepass用レンダーターゲット。
		Sprite					m_deferredLightingSprite;					//ディファードライティング用スプライト。
		Sprite					m_portalSprite[PORTAL_NUM];					//ポータル用スプライト。
		Sprite					m_2DSprite;									//2Dスプライト。
		Sprite					m_mainSprite;								//メインスプライト。
		GaussianBlur			m_shadowBlur;								//シャドウ用ガウシアンブラー。
		Camera					m_lightCamera;								//ライトカメラ。
		PostEffect				m_postEffect;								//ポストエフェクト。
		LightCB					m_lightCB;									//ライト用構造体。
		PortalCamera*			m_portalCamera = nullptr;					//ポータル用カメラ。
	};
}
