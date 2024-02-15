#pragma once
class Stage;
class Player;
class GameCamera;
class PortalGun;

namespace
{
	const int TIMER_NUM = 6;					//表示する時間の個数。
	const int NUMBER_SPRITE_NUM = 10;			//数字の個数。
}

/// <summary>
/// ゲームメインクラス。
/// </summary>
class Game : public IGameObject
{
public:
	/// <summary>
	/// ゲームステート。
	/// </summary>
	enum GameState
	{
		enGameState_Game,
		enGameState_Result,
		enGameState_Reset,
	};

	/// <summary>
	/// スコア。
	/// </summary>
	struct GameScore
	{
		float	timer = 0.0f;		//経過時間。
		int		stepNum = 0;		//歩いた回数。
		int		shotPortal = 0;	//ポータルの発射数。
	};

public:
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

public:
	/// <summary>
	/// リセットを通知。
	/// </summary>
	void NotifyReset();
	/// <summary>
	/// クリアを通知。
	/// </summary>
	void NotifyClear();
	/// <summary>
	/// リトライを通知。
	/// </summary>
	void NotifyRetry();
	/// <summary>
	/// ゲーム終了を通知。
	/// </summary>
	void NotifyEnd();
	/// <summary>
	/// 復活座標を取得。
	/// </summary>
	/// <returns></returns>
	const Vector3& GetRespawnPosition() const;
	/// <summary>
	/// クリア座標を取得。
	/// </summary>
	const Vector3& GetClearPosition() const;

	/// <summary>
	/// ステートを取得。
	/// </summary>
	/// <returns></returns>
	const GameState GetGameState() const
	{
		return m_gameState;
	}

	/// <summary>
	/// 歩いた回数を加算。
	/// </summary>
	void AddStepNum()
	{
		m_gameScore.stepNum++;
	}
	/// <summary>
	/// ポータルの発射数を加算。
	/// </summary>
	void AddShotNum()
	{
		m_gameScore.shotPortal++;
	}

private:
	/// <summary>
	/// リセット処理。
	/// </summary>
	void Reset();
	/// <summary>
	/// 計測処理。
	/// </summary>
	void Timer();

private:
	FontRender			m_timerFontRender;					//時間スプライト。
	Stage*				m_stage = nullptr;					//ステージ。
	Player*				m_player = nullptr;					//プレイヤー。
	GameCamera*			m_gameCamera = nullptr;				//カメラ。
	PortalGun*			m_portalGun = nullptr;				//ポータルガン。
	GameScore			m_gameScore;						//スコア。
	GameState			m_gameState = enGameState_Game;		//ゲームステート。
	float				m_resetTimer = 0.0f;				//リセット用タイマー。
};

