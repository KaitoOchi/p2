#pragma once
#include "IRenderer.h"
#include "graphics/light/DirectionalLight.h"
#include "graphics/light/PointLight.h"
#include "graphics/light/SpotLight.h"

namespace nsPortalEngine {

	/// <summary>
	/// �����_�����O�G���W���B
	/// </summary>
	class RenderingEngine : Noncopyable
	{
	public:
		struct LightCB
		{
			DirectionalLight::DirectionLig directionLig;
			Vector3 eyePos;
			PointLight::PointLig pointLig;
			SpotLight::SpotLig spotLig;
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
		/// ���C�g�p�萔�o�b�t�@���擾�B
		/// </summary>
		/// <returns></returns>
		LightCB& GetLightCB()
		{
			return m_lightCB;
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
		/// �t�H���[�h�����_�����O�̕`�揈���B
		/// </summary>
		void DrawForwardRendering(RenderContext& rc);

		/// <summary>
		/// 2D�̕`�揈���B
		/// </summary>
		/// <param name="rc"></param>
		void Draw2D(RenderContext& rc);

	private:
		static RenderingEngine* m_instance;
		std::vector<IRenderer*> m_renderObjects;
		LightCB m_lightCB;
	};

}
