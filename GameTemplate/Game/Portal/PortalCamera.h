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

	void DeletePortalFramePointer()
	{
		m_portalFrame[0] = nullptr;
		m_portalFrame[1] = nullptr;
	}


	void UpdateAngle();

private:
	/// <summary>
	/// ポータル用カメラの設定。
	/// </summary>
	void SetPortalCamera();
	/// <summary>
	/// ポータル用カメラの座標を設定。
	/// </summary>
	/// <param name="portalNum">設定するポータル番号</param>
	void SetCameraPosition(const int portalNum);
	/// <summary>
	/// ポータル用カメラの注視点を設定。
	/// </summary>
	/// <param name="portalNum">設定するポータル番号</param>
	void SetCameraTarget(const int portalNum);

private:
	std::array<PortalFrame*, PORTAL_NUM> m_portalFrame;		//ポータル。
	std::array<Camera,		 PORTAL_NUM> m_portalCamera;	//ポータルカメラ。
	std::array<Quaternion,	 PORTAL_NUM> m_rotation;		//角度。
	std::array<float,		 PORTAL_NUM> m_angle;			//角度。
};