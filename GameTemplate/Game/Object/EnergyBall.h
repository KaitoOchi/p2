#pragma once
#include "Gimmick.h"
class EnergyBallGenerator;
class Player;
class Stage;

/// <summary>
/// エネルギーボール。
/// </summary>
class EnergyBall : public Gimmick
{
public:
	EnergyBall();
	~EnergyBall() override;
	bool Start();
	void Update();

public:
	/// <summary>
	/// 座標を設定。
	/// </summary>
	/// <param name="pos"></param>
	void SetPosition(const Vector3& pos)
	{
		m_position = pos;
	}

	/// <summary>
	/// 座標を取得。
	/// </summary>
	/// <returns></returns>
	const Vector3& GetPosition()
	{
		return m_position;
	}

	/// <summary>
	/// 方向を設定。
	/// </summary>
	/// <param name="dir"></param>
	void SetDirection(const Vector3& dir)
	{
		m_direction = dir;
	}

	/// <summary>
	/// 生成機を設定。
	/// </summary>
	/// <param name="generator"></param>
	void SetGenerator(EnergyBallGenerator* generator)
	{
		m_energyBallGenerator = generator;
	}

	/// <summary>
	/// 有効化。
	/// </summary>
	void Enable();

	/// <summary>
	/// コリジョンを取得。
	/// </summary>
	/// <returns></returns>
	CollisionObject* GetCollisionObject()
	{
		return m_collisionObject;
	}

	/// <summary>
	/// 削除処理。
	/// </summary>
	void Delete();

	/// <summary>
	/// ワープ処理。
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="direction">方向</param>
	void SetWarp(const Vector3& pos, const Vector3& direction);

	/// <summary>
	/// ギミックの成功処理。
	/// </summary>
	void Clear() override;

private:
	/// <summary>
	/// 移動処理。
	/// </summary>
	void Move();

	/// <summary>
	/// レイを飛ばす処理。
	/// </summary>
	void RayCast();

	/// <summary>
	/// 当たり判定の処理。
	/// </summary>
	void Collision();

	/// <summary>
	/// 無効化。
	/// </summary>
	void Disable();

private:
	Stage*					m_stage = nullptr;					//ステージ。
	CollisionObject*		m_collisionObject = nullptr;		//コリジョン。
	EffectEmitter*			m_ballEffect = nullptr;				//ボールエフェクト。
	EnergyBallGenerator*	m_energyBallGenerator = nullptr;	//生成機。
	Player*					m_player = nullptr;					//プレイヤー。
	Vector3					m_direction;						//移動する方向。
	Vector3					m_moveSpeed;						//移動速度。
	bool					m_isEnable = false;					//表示状態が有効かどうか。
	float					m_timer = 0.0f;						//タイマー。
};

