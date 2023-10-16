#pragma once

namespace nsPortalEngine {

	/// <summary>
	/// �|�[�^���G���W���B
	/// </summary>
	class PortalEngine : Noncopyable
	{
	public:
		/// <summary>
		/// �������f�[�^�B
		/// </summary>
		struct InitData {
			HWND hwnd;
			UINT frameBufferWidth;
			UINT frameBufferHeight;
		};

	public:
		PortalEngine();
		~PortalEngine();

		/// <summary>
		/// �C���X�^���X�̏������B
		/// </summary>
		/// <param name="initData"></param>
		static void CreateInstance(const InitData& initData)
		{
			if (m_instance != nullptr) {
				std::abort();
			}

			m_instance = new PortalEngine;
			m_instance->Init(initData);
		}

		/// <summary>
		/// �C���X�^���X�̍폜�B
		/// </summary>
		static void DeleteInstance()
		{
			delete m_instance;
		}

		/// <summary>
		/// �C���X�^���X�̎擾�B
		/// </summary>
		/// <returns></returns>
		static PortalEngine* GetInstance()
		{
			return m_instance;
		}

		/// <summary>
		/// �G���W���̏������B
		/// </summary>
		/// <param name="initData"></param>
		void Init(const InitData& initData);

		/// <summary>
		/// ���s�����B
		/// </summary>
		void Execute();

	private:
		static PortalEngine* m_instance;	//�C���X�^���X�B
		K2EngineLow m_k2EngineLow;			//�ȈՃG���W���B
	};
}

