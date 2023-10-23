#pragma once
class PortalFrame;

/// <summary>
/// �|�[�^���p�J�����B
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
	/// �|�[�^���p�J�����̎擾�B
	/// </summary>
	/// <param name="num"></param>
	/// <returns></returns>
	Camera& GetPortalCamera(const int num)
	{
		return m_portalCamera[num];
	}

	/// <summary>
	/// �|�[�^���̃|�C���^��ݒ�B
	/// </summary>
	/// <param name="portal"></param>
	void SetPortalFramePointer(PortalFrame* portal, const int num)
	{
		m_portalFrame[num] = portal;
	}

private:
	/// <summary>
	/// �|�[�^���p�J�����̐ݒ�B
	/// </summary>
	void SetPortalCamera();

private:
	std::array<PortalFrame*, PORTAL_NUM>	m_portalFrame;		//�|�[�^���B
	std::array<Camera, PORTAL_NUM>			m_portalCamera;		//�|�[�^���J�����B
};

