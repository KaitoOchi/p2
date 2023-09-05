#pragma once
#include "GameCamera.h"

class Player : public IGameObject
{
public:
	/// <summary>
	/// プレイヤーステート。
	/// </summary>
	enum PlayerState
	{
		enState_Idle,
		enState_Walk,
		enState_Run,
		enState_Crouch,
		enState_Jump,
	};

public:
	Player();
	~Player();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

private:
	/// <summary>
	/// 入力処理。
	/// </summary>
	void Input();

	/// <summary>
	/// 移動処理。
	/// </summary>
	void Move();

	/// <summary>
	/// 回転処理。
	/// </summary>
	void Rotation();

	/// <summary>
	/// ステート処理。
	/// </summary>
	void State();

	/// <summary>
	/// 共通遷移処理。
	/// </summary>
	void ProcessCommonStateTransition();

	/// <summary>
	/// 待機状態の遷移処理。
	/// </summary>
	void ProcessIdleStateTransition();

	/// <summary>
	/// 歩き状態の遷移処理。
	/// </summary>
	void ProcessWalkStateTransition();

	/// <summary>
	/// 走り状態の遷移処理。
	/// </summary>
	void ProcessRunStateTransition();

	/// <summary>
	/// しゃがみ状態の遷移処理。
	/// </summary>
	void ProcessCrouchStateTransition();

	/// <summary>
	/// ジャンプ状態の遷移処理。
	/// </summary>
	void ProcessJumpStateTransition();

private:
	ModelRender			m_modelRender;		//モデルレンダー。
	Vector3				m_position;			//座標。
	Quaternion			m_rotation;			//回転。
	GameCamera			m_gameCamera;		//ゲームカメラ。
	PlayerState			m_playerState;		//プレイヤーステート。


	FontRender a;
};

