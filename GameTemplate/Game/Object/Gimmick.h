#pragma once
class Stage;

class Gimmick : public IGameObject
{
public:
	/// <summary>
	/// �M�~�b�N�̎�ށB
	/// </summary>
	enum GimmickType
	{
		enGimmickType_Sender,		//���M���B
		enGimmickType_Receiver,		//��摤�B
		enGimmickType_Defuse,		//�����B
	};

public:
	Gimmick();
	~Gimmick() = 0;
	bool Start();
	void Update();
	void Render(RenderContext& rc);

public:
	/// <summary>
	/// ���W�̐ݒ�B
	/// </summary>
	/// <param name="pos"></param>
	void SetPosition(const Vector3& pos)
	{
		m_position = pos;
	}

	/// <summary>
	/// ��]�̐ݒ�B
	/// </summary>
	/// <param name="rot"></param>
	void SetRotation(const Quaternion& rot)
	{
		m_rotation = rot;
	}

	/// <summary>
	/// �M�~�b�N�ԍ��̐ݒ�B
	/// </summary>
	/// <param name="num"></param>
	void SetGimmickNumber(const int num)
	{
		m_gimmickNum = num;
	}

	/// <summary>
	/// �M�~�b�N�ԍ��̎擾�B
	/// </summary>
	/// <returns></returns>
	const int GetGimmickNumber() const
	{
		return m_gimmickNum;
	}

	/// <summary>
	/// �M�~�b�N�̐��������B
	/// </summary>
	virtual void Clear() = 0;

protected:
	Vector3				m_position;						//���W�B
	Quaternion			m_rotation;						//��]�B
	GimmickType			m_gimmickType;					//�M�~�b�N�̎�ށA
	bool				m_isClear = false;				//�N���A�������ǂ����B
	int					m_gimmickNum = 0;				//�M�~�b�N�ԍ��B
};

