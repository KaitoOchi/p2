#pragma once

namespace nsPortalEngine {

	/// <summary>
	/// ポータルエンジン。
	/// </summary>
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

		/// <summary>
		/// インスタンスの初期化。
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
		/// インスタンスの削除。
		/// </summary>
		static void DeleteInstance()
		{
			delete m_instance;
		}

		/// <summary>
		/// インスタンスの取得。
		/// </summary>
		/// <returns></returns>
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
		static PortalEngine* m_instance;	//インスタンス。
		K2EngineLow m_k2EngineLow;			//簡易エンジン。
	};
}

