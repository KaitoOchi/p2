#pragma once
class Stage;

class Gimmick : public IGameObject
{
public:
	/// <summary>
	/// ギミックの種類。
	/// </summary>
	enum GimmickType
	{
		enGimmickType_Sender,		//送信側。
		enGimmickType_Receiver,		//受取側。
		enGimmickType_Defuse,		//解除。
	};

public:
	Gimmick();
	~Gimmick() = 0;
	bool Start();
	void Update();
	void Render(RenderContext& rc);

public:
	/// <summary>
	/// 座標の設定。
	/// </summary>
	/// <param name="pos"></param>
	void SetPosition(const Vector3& pos)
	{
		m_position = pos;
	}

	/// <summary>
	/// 回転の設定。
	/// </summary>
	/// <param name="rot"></param>
	void SetRotation(const Quaternion& rot)
	{
		m_rotation = rot;
	}

	/// <summary>
	/// ギミック番号の設定。
	/// </summary>
	/// <param name="num"></param>
	void SetGimmickNumber(const int num)
	{
		m_gimmickNum = num;
	}

	/// <summary>
	/// ギミック番号の取得。
	/// </summary>
	/// <returns></returns>
	const int GetGimmickNumber() const
	{
		return m_gimmickNum;
	}

	/// <summary>
	/// ギミックの成功処理。
	/// </summary>
	virtual void Clear() = 0;

protected:
	Vector3				m_position;						//座標。
	Quaternion			m_rotation;						//回転。
	GimmickType			m_gimmickType;					//ギミックの種類、
	bool				m_isClear = false;				//クリアしたかどうか。
	int					m_gimmickNum = 0;				//ギミック番号。
};

