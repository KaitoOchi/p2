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

	void DeletePortalFramePointer()
	{
		m_portalFrame[0] = nullptr;
		m_portalFrame[1] = nullptr;
	}


	void UpdateAngle();

private:
	/// <summary>
	/// �|�[�^���p�J�����̐ݒ�B
	/// </summary>
	void SetPortalCamera();
	/// <summary>
	/// �|�[�^���p�J�����̍��W��ݒ�B
	/// </summary>
	/// <param name="portalNum">�ݒ肷��|�[�^���ԍ�</param>
	void SetCameraPosition(const int portalNum);
	/// <summary>
	/// �|�[�^���p�J�����̒����_��ݒ�B
	/// </summary>
	/// <param name="portalNum">�ݒ肷��|�[�^���ԍ�</param>
	void SetCameraTarget(const int portalNum);

private:
	std::array<PortalFrame*, PORTAL_NUM> m_portalFrame;		//�|�[�^���B
	std::array<Camera,		 PORTAL_NUM> m_portalCamera;	//�|�[�^���J�����B
	std::array<Quaternion,	 PORTAL_NUM> m_rotation;		//�p�x�B
	std::array<float,		 PORTAL_NUM> m_angle;			//�p�x�B
};