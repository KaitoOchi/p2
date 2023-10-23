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
		const int POINT_LIGHT_NUM = 4;		//�|�C���g���C�g�̐��B
		const int SPOT_LIGHT_NUM = 4;		//�X�|�b�g���C�g�̐��B
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
			float pad0 = 0.0f;
			PointLight::PointLig pointLig[POINT_LIGHT_NUM];		//�|�C���g���C�g�B
			SpotLight::SpotLig spotLig[SPOT_LIGHT_NUM];			//�X�|�b�g���C�g�B
			ShadowCB shadowCB;									//�V���h�E�B
			Matrix mViewProjInv;								//�r���[�v���W�F�N�V�����s��̋t�s��B
			int ptNum = POINT_LIGHT_NUM;						//�|�C���g���C�g�̐��B
			int spNum = SPOT_LIGHT_NUM;							//�X�|�b�g���C�g�̐��B
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

		/// <summary>
		/// ���C�������_�[�^�[�Q�b�g�ɖ߂��B
		/// </summary>
		/// <param name="rc"></param>
		void SetMainRenderTargetAndDepthStencilBuffer(RenderContext& rc)
		{
			rc.SetRenderTarget(m_mainRenderTarget.GetRTVCpuDescriptorHandle(), m_zprepassRenderTarget.GetDSVCpuDescriptorHandle());
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
		/// �|�[�^���p�����_�[�^�[�Q�b�g���擾�B
		/// </summary>
		/// <param name="num"></param>
		/// <returns></returns>
		RenderTarget& GetPortalRenderTarget(const int num)
		{
			return m_portalRenderTarget[num];
		}

		/// <summary>
		/// ZPrepass�p�����_�[�^�[�Q�b�g���擾�B
		/// </summary>
		/// <returns></returns>
		RenderTarget& GetZPrepassRenderTarget()
		{
			return m_zprepassRenderTarget;
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
		/// �|�[�^���p�����_�[�^�[�Q�b�g�̏����������B
		/// </summary>
		void InitPortalRenderTarget();
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
			enGBufferMetaricShadowSmooth,   // r�Ƀ��^���b�N�Ag�ɉe�p�����[�^�Ab�Ƀ��C�e�B���O�p�����[�^�Aa�ɃX���[�X�B
			enGBufferNum,                   // G-Buffer�̐��B
		};

	private:
		static RenderingEngine* m_instance;									//�C���X�^���X�B
		std::vector<IRenderer*> m_renderObjects;							//�����_�[�I�u�W�F�N�g�B
		RenderTarget			m_gBuffer[1 + PORTAL_NUM][enGBufferNum];	//GBuffer�B
		RenderTarget			m_mainRenderTarget;							//���C�������_�[�^�[�Q�b�g�B
		RenderTarget			m_2DRenderTarget;							//2D�p�����_�[�^�[�Q�b�g�B
		RenderTarget			m_portalRenderTarget[PORTAL_NUM];			//�|�[�^���p�����_�[�^�[�Q�b�g�B
		RenderTarget			m_shadowMapRenderTarget;					//�V���h�E�}�b�v�p�����_�[�^�[�Q�b�g�B
		RenderTarget			m_zprepassRenderTarget;						//ZPrepass�p�����_�[�^�[�Q�b�g�B
		Sprite					m_deferredLightingSprite;					//�f�B�t�@�[�h���C�e�B���O�p�X�v���C�g�B
		Sprite					m_portalSprite[PORTAL_NUM];					//�|�[�^���p�X�v���C�g�B
		Sprite					m_2DSprite;									//2D�X�v���C�g�B
		Sprite					m_mainSprite;								//���C���X�v���C�g�B
		GaussianBlur			m_shadowBlur;								//�V���h�E�p�K�E�V�A���u���[�B
		Camera					m_lightCamera;								//���C�g�J�����B
		PostEffect				m_postEffect;								//�|�X�g�G�t�F�N�g�B
		LightCB					m_lightCB;									//���C�g�p�\���́B
		PortalCamera*			m_portalCamera = nullptr;					//�|�[�^���p�J�����B
	};
}
