#pragma once

class GameCamera : Noncopyable
{
public:
	GameCamera();
	~GameCamera();

	/// <summary>
	/// 更新処理。
	/// </summary>
	void Update();

	/// <summary>
	/// 移動処理。
	/// </summary>
	void SetPosition(const Vector3& pos);

	/// <summary>
	/// 回転処理。
	/// </summary>
	void SetRotation();

private:
	Vector3 m_targetPos;
	Vector3 m_rotSpeed;
};

