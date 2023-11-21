#pragma once
class EnergyBall;

/// <summary>
/// エネルギーボール生成機。
/// </summary>
class EnergyBallGenerator : public IGameObject
{
public:
	EnergyBallGenerator();
	~EnergyBallGenerator();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

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
	/// 回転を設定。
	/// </summary>
	/// <param name="rot"></param>
	void SetRotation(const Quaternion& rot)
	{
		m_rotation = rot;
		m_direction = CalcQuaternionToEuler(m_rotation);
		float tmp = m_direction.x;
		m_direction.x = m_direction.y;
		m_direction.y = m_direction.z;
		m_direction.z = tmp;
		m_direction.Normalize();
	}

	/// <summary>
	/// エネルギーボールを消失を通知。
	/// </summary>
	void NotifyDisable()
	{
		m_isEnableEnergyBall = false;
		m_generateTimer = 0.0f;
	}

	/// <summary>
	/// 無効化。
	/// </summary>
	void Disable();

private:
	EnergyBall*		m_energyBall = nullptr;				//エネルギーボール。
	Vector3			m_position;							//座標。
	Vector3			m_direction;						//方向。
	Quaternion		m_rotation;							//回転。
	bool			m_isEnableEnergyBall = false;		//対応するエネルギーボールが有効かどうか。
	bool			m_isClear = false;					//クリアしたかどうか。
	float			m_generateTimer = 0.0f;				//生成にかかる時間。
};

