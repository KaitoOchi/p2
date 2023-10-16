#pragma once
class PortalFrame;

class PortalCamera : public IGameObject
{
public:
	PortalCamera();
	~PortalCamera();
	bool Start();
	void Update();

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

	/// <summary>
	/// ポータル用カメラの設定。
	/// </summary>
	void SetPortalCamera();

private:
	PortalFrame*			m_portalFrame[2];		//ポータル。
	Camera					m_portalCamera[2];		//ポータルカメラ。
};

