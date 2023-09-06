#pragma once

class GameCamera : Noncopyable
{
public:
	GameCamera();
	~GameCamera();

	/// <summary>
	/// �����������B
	/// </summary>
	void Init();

	/// <summary>
	/// �X�V�����B
	/// </summary>
	void Update();

	/// <summary>
	/// �ړ������B
	/// </summary>
	void SetPosition(const Vector3& pos);

	/// <summary>
	/// ��]�����B
	/// </summary>
	void SetRotation(const Vector3& rotSpeed);

private:
	Vector3 m_targetPos;
	Vector3	m_rotSpeed;		//��]�ʁB
};

