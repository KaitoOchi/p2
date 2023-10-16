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

	/// <summary>
	/// �|�[�^���p�J�����̐ݒ�B
	/// </summary>
	void SetPortalCamera();

private:
	PortalFrame*			m_portalFrame[2];		//�|�[�^���B
	Camera					m_portalCamera[2];		//�|�[�^���J�����B
};

