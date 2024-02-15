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
		const int POINT_LIGHT_NUM = 256;		//�|�C���g���C�g�̐��B
		const int SPOT_LIGHT_NUM = 256;		//�X�|�b�g���C�g�̐��B
		const int TILE_WIDTH = 16;			//�^�C���̕��B
		const int TILE_HEIGHT = 16;			//�^�C���̍����B
		const int TILE_NUM = (FRAME_BUFFER_W / TILE_WIDTH) * (FRAME_BUFFER_H / TILE_HEIGHT);		//�^�C���̐��B
	}

	/// <summary>
	/// �����_�����O�G���W���B
	/// </summary>
	class RenderingEngine : Noncopyable
	{
	public:
		/// <summary>
		/// �V���h�E�p�萔�o�b�t�@�B
		/// </summary>
		struct ShadowCB
		{
			Vector3 lightPos;
			float pad0;
			Matrix mLVP;
		};

		/// <summary>
		/// ���C�g�p�萔�o�b�t�@�B
		/// </summary>
		struct LightCB
		{
			DirectionalLight::DirectionLig directionLig;		//�f�B���N�V�������C�g�B
			Vector3 eyePos;										//���_�̈ʒu�B
			float pad0;
			Vector3 eyePosBlue;									//���_�̈ʒu�B
			float pad1;
			Vector3 eyePosRed;									//���_�̈ʒu�B
			float pad2;
			PointLight::PointLig pointLig[POINT_LIGHT_NUM];		//�|�C���g���C�g�B
			SpotLight::SpotLig spotLig[SPOT_LIGHT_NUM];			//�X�|�b�g���C�g�B
			ShadowCB shadowCB;									//�V���h�E�B
			Matrix mViewProjInv[3];								//�r���[�v���W�F�N�V�����s��̋t�s��B
			int ptNum = 0;										//�|�C���g���C�g�̐��B
			int spNum = 0;										//�X�|�b�g���C�g�̐��B
		};

	private:
		RenderingEngine();
		~RenderingEngine();

	public:
		/// <summary>
		/// �C���X�^���X�̐����B
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
		/// �C���X�^���X�̔j���B
		/// </summary>
		static void DeleteInstance()
		{
			delete m_instance;
		}

		/// <summary>
		/// �C���X�^���X�̎擾�B
		/// </summary>
		/// <returns></returns>
		static RenderingEngine* GetInstance()
		{
			return m_instance;
		}

		/// <summary>
		/// �����_�[�I�u�W�F�N�g��ǉ��B
		/// </summary>
		/// <param name="renderObject"></param>
		void AddRenderObject(IRenderer* renderObject)
		{
			m_renderObjects.emplace_back(renderObject);
		}

		/// <summary>
		/// �|�[�^���J�����̃|�C���^��ݒ�B
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
		/// ���C�������_�[�^�[�Q�b�g�ɖ߂��B
		/// </summary>
		/// <param name="rc"></param>
		void SetMainRenderTargetAndDepthStencilBuffer(RenderContext& rc)
		{
			rc.SetRenderTarget(m_mainRenderTarget[0].GetRTVCpuDescriptorHandle(), m_zprepassRenderTarget[0].GetDSVCpuDescriptorHandle());
		}

		/// <summary>
		/// ���C�g�p�萔�o�b�t�@���擾�B
		/// </summary>
		/// <returns></returns>
		LightCB& GetLightCB()
		{
			return m_lightCB;
		}

		/// <summary>
		/// �V���h�E�p�̍\���̂��擾�B
		/// </summary>
		/// <returns></returns>
		ShadowCB& GetShadowCB()
		{
			return m_lightCB.shadowCB;
		}

		/// <summary>
		/// �|�C���g���C�g�p��UAV���擾�B
		/// </summary>
		RWStructuredBuffer& GetPointLightNoListInTileUAV()
		{
			return m_pointLightNoListInTileUAV[0];
		}

		/// <summary>
		/// �X�|�b�g���C�g�p��UAV���擾�B
		/// </summary>
		RWStructuredBuffer& GetSpotLightNoListInTileUAV()
		{
			return m_spotLightNoListInTileUAV[0];
		}

		/// <summary>
		/// ZPrepass�p�����_�[�^�[�Q�b�g���擾�B
		/// </summary>
		/// <returns></returns>
		RenderTarget& GetZPrepassRenderTarget()
		{
			return m_zprepassRenderTarget[0];
		}

		/// <summary>
		/// �V���h�E�}�b�v�p�̃{�P�摜���擾�B
		/// </summary>
		Texture& GetShadowBokeTexture()
		{
			return m_shadowBlur.GetBokeTexture();
		}

		/// <summary>
		/// �|�X�g�G�t�F�N�g���擾�B
		/// </summary>
		/// <returns></returns>
		PostEffect& GetPostEffet()
		{
			return m_postEffect;
		}

		/// <summary>
		/// ���s�����B
		/// </summary>
		void Execute(RenderContext& rc);

	private:
		/// <summary>
		/// �����������B
		/// </summary>
		void Init();
		/// <summary>
		/// �f�B�t�@�[�h���C�e�B���O�p�X�v���C�g�̏����������B
		/// </summary>
		void InitDeferredLightingSprite();
		/// <summary>
		/// �����_�[�^�[�Q�b�g�̏����������B
		/// </summary>
		void InitRenderTarget();
		/// <summary>
		/// GBuffer�̏����������B
		/// </summary>
		void InitGBuffer();
		/// <summary>
		/// ���C�������_�[�^�[�Q�b�g�̏����������B
		/// </summary>
		void InitMainRenderTarget();
		/// <summary>
		/// 2D�����_�[�^�[�Q�b�g�̏����������B
		/// </summary>
		void Init2DRenderTarget();
		/// <summary>
		/// �V���h�E�p�����_�[�^�[�Q�b�g�̏����������B
		/// </summary>
		void InitShadowMapRenderTarget();
		/// <summary>
		/// ZPrepass�p�����_�[�^�[�Q�b�g�̏����������B
		/// </summary>
		void InitZPrepassRenderTarget();

		/// <summary>
		/// �萔�o�b�t�@�̍X�V�����B
		/// </summary>
		void UpdateCB();
		/// <summary>
		/// ZPrepass�̕`�揈���B
		/// </summary>
		void DrawZPrepass(RenderContext& rc);
		/// <summary>
		/// �V���h�E�}�b�v�̕`�揈���B
		/// </summary>
		/// <param name="rc"></param>
		void DrawShadowMap(RenderContext& rc);
		/// <summary>
		/// GBuffer�̕`�揈���B
		/// </summary>
		/// <param name="rc"></param>
		void DrawGBuffer(RenderContext& rc);
		/// <summary>
		/// �f�B�t�@�[�h���C�e�B���O�̕`�揈���B
		/// </summary>
		/// <param name="rc"></param>
		void DrawDeferredLighting(RenderContext& rc);
		/// <summary>
		/// �t�H���[�h���C�e�B���O�̕`�揈���B
		/// </summary>
		/// <param name="rc"></param>
		void DrawForwardLighting(RenderContext& rc);
		/// <summary>
		/// 2D�̕`�揈���B
		/// </summary>
		/// <param name="rc"></param>
		void Draw2D(RenderContext& rc);

	private:
		//GBuffer�̒�`�B
		enum EnGBuffer
		{
			enGBufferAlbedoDepth,           // rgb�ɃA���x�h�Aa�ɐ[�x�l�B
			enGBufferNormal,                // rgb�ɖ@���Aa�ɃJ������Ԃ̖@���B
			enGBufferMetaricShadowSmooth,   // r�Ƀ��^���b�N�Aa�ɃX���[�X�B
			enGBufferShadowLightParam,		// r�ɃV���h�E�Ag�Ƀ��C�g�p�����[�^�B
			enGBufferNum,                   // G-Buffer�̐��B
		};

	private:
		static RenderingEngine* m_instance;									//�C���X�^���X�B
		std::vector<IRenderer*> m_renderObjects;							//�����_�[�I�u�W�F�N�g�B
		RenderTarget			m_gBuffer[1 + PORTAL_NUM][enGBufferNum];	//GBuffer�B
		RenderTarget			m_mainRenderTarget[1 + PORTAL_NUM];			//���C�������_�[�^�[�Q�b�g�B
		RenderTarget			m_2DRenderTarget;							//2D�p�����_�[�^�[�Q�b�g�B
		RenderTarget			m_shadowMapRenderTarget;					//�V���h�E�}�b�v�p�����_�[�^�[�Q�b�g�B
		RenderTarget			m_zprepassRenderTarget[1 + PORTAL_NUM];		//ZPrepass�p�����_�[�^�[�Q�b�g�B
		Sprite					m_deferredLightingSprite[1 + PORTAL_NUM];	//�f�B�t�@�[�h���C�e�B���O�p�X�v���C�g�B
		Sprite					m_2DSprite;									//2D�X�v���C�g�B
		Sprite					m_mainSprite;								//���C���X�v���C�g�B
		GaussianBlur			m_shadowBlur;								//�V���h�E�p�K�E�V�A���u���[�B
		Camera					m_lightCamera;								//���C�g�J�����B
		PostEffect				m_postEffect;								//�|�X�g�G�t�F�N�g�B
		LightCulling			m_lightCulling[1 + PORTAL_NUM];				//���C�g�J�����O�B
		LightCB					m_lightCB;									//���C�g�p�\���́B
		RWStructuredBuffer		m_pointLightNoListInTileUAV[1 + PORTAL_NUM];//�|�C���g���C�g�pUAV�B
		RWStructuredBuffer		m_spotLightNoListInTileUAV[1 + PORTAL_NUM];	//�X�|�b�g���C�g�pUAV�B
		PortalCamera*			m_portalCamera = nullptr;					//�|�[�^���p�J�����B
	};
}
