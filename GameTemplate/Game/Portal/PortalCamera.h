#pragma once
class PortalFrame;

/// <summary>
/// ポータル用カメラ。
/// </summary>
class PortalCamera : public IGameObject
{
public:
	PortalCamera();
	~PortalCamera();
	bool Start();
	void Update();

public:
	/// <summary>
	/// ポータル用カメラの取得。
	/// </summary>
	/// <param name="num"></param>
	/// <returns></returns>
	Camera& GetPortalCamera(const int num)
	{
		return m_portalCamera[num];
	}

	/// <summary>
	/// ポータルのポインタを設定。
	/// </summary>
	/// <param name="portal"></param>
	void SetPortalFramePointer(PortalFrame* portal, const int num)
	{
		m_portalFrame[num] = portal;
	}

private:
	/// <summary>
	/// ポータル用カメラの設定。
	/// </summary>
	void SetPortalCamera();

private:
	std::array<PortalFrame*, PORTAL_NUM>	m_portalFrame;		//ポータル。
	std::array<Camera, PORTAL_NUM>			m_portalCamera;		//ポータルカメラ。
};

