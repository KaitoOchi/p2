#pragma once
#include "IRenderer.h"
#include "graphics/light/DirectionalLight.h"
#include "graphics/light/PointLight.h"
#include "graphics/light/SpotLight.h"
#include "PostEffect/PostEffect.h"

namespace nsPortalEngine {

	namespace
	{
		const int PORTAL_NUM = 2;			//ポータルの数。
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
			DirectionalLight::DirectionLig directionLig;
			Vector3 eyePos;
			float pad0 = 0.0f;
			PointLight::PointLig pointLig[POINT_LIGHT_NUM];
			SpotLight::SpotLig spotLig[SPOT_LIGHT_NUM];
			ShadowCB shadowCB;
			int ptNum = POINT_LIGHT_NUM;
			int spNum = SPOT_LIGHT_NUM;
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
		/// シャドウマップ用ガウシアンブラーを取得。
		/// </summary>
		/// <returns></returns>
		GaussianBlur& GetShadowBlur()
		{
			return m_shadowBlur;
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
		/// シャドウマップ用レンダーターゲットを取得。
		/// </summary>
		/// <returns></returns>
		RenderTarget& GetShadowRenderTarget()
		{
			return m_shadowMapRenderTarget;
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
		/// レンダーターゲットの初期化処理。
		/// </summary>
		void InitRenderTarget();

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
		/// シャドウマップの描画処理。
		/// </summary>
		/// <param name="rc"></param>
		void RenderShadowMap(RenderContext& rc);

		/// <summary>
		/// フォワードレンダリングの描画処理。
		/// </summary>
		void DrawForwardRendering(RenderContext& rc);

		/// <summary>
		/// 2Dの描画処理。
		/// </summary>
		/// <param name="rc"></param>
		void Draw2D(RenderContext& rc);

	private:
		static RenderingEngine* m_instance;							//インスタンス。
		std::vector<IRenderer*> m_renderObjects;					//レンダーオブジェクト。
		RenderTarget			m_mainRenderTarget;					//メインレンダーターゲット。
		RenderTarget			m_2DRenderTarget;					//2D用レンダーターゲット。
		RenderTarget			m_portalRenderTarget[PORTAL_NUM];	//ポータル用レンダーターゲット。
		RenderTarget			m_shadowMapRenderTarget;			//シャドウマップ用レンダーターゲット。
		Sprite					m_2DSprite;                         //2Dスプライト。
		Sprite					m_mainSprite;						//メインスプライト。
		GaussianBlur			m_shadowBlur;						//シャドウ用ガウシアンブラー。
		Camera					m_lightCamera;						//ライトカメラ。
		Camera					m_portalCamera[PORTAL_NUM];			//ポータルカメラ。
		PostEffect				m_postEffect;						//ポストエフェクト。
		LightCB					m_lightCB;							//ライト用構造体。
	};

}
