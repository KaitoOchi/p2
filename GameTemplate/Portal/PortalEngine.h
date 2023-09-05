#pragma once

namespace nsPortalEngine {

	class PortalEngine : Noncopyable
	{
	public:
		/// <summary>
		/// 初期化データ。
		/// </summary>
		struct InitData {
			HWND hwnd;
			UINT frameBufferWidth;
			UINT frameBufferHeight;
		};

	public:
		PortalEngine();
		~PortalEngine();

		static void CreateInstance(const InitData& initData)
		{
			if (m_instance != nullptr) {
				std::abort();
			}

			m_instance = new PortalEngine;
			m_instance->Init(initData);
		}

		static void DeleteInstance()
		{
			delete m_instance;
		}

		static PortalEngine* GetInstance()
		{
			return m_instance;
		}

		/// <summary>
		/// エンジンの初期化。
		/// </summary>
		/// <param name="initData"></param>
		void Init(const InitData& initData);

		/// <summary>
		/// 実行処理。
		/// </summary>
		void Execute();

	private:
		static PortalEngine* m_instance;
		K2EngineLow m_k2EngineLow;
	};
}

