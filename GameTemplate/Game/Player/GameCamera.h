#pragma once

class GameCamera : Noncopyable
{
public:
	GameCamera();
	~GameCamera();

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
	void SetRotation();

private:
	Vector3 m_targetPos;
	Vector3 m_rotSpeed;
};

