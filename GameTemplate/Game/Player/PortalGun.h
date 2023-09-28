#pragma once


/// <summary>
/// �|�[�^���K���N���X�B
/// </summary>
class PortalGun : public IGameObject
{
public:
	/// <summary>
	/// �|�[�^���̎�ށB
	/// </summary>
	enum PortalType
	{
		enPortalType_Blue,
		enPortalType_Red,
		enPortalType_Num,
	};

public:
	PortalGun();
	~PortalGun();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

public:
	/// <summary>
	/// �|�[�^����ݒu�B
	/// </summary>
	/// <param name="type">���</param>
	/// <param name="position">���W</param>
	/// <param name="rotation">��]</param>
	void SetPortal(const PortalType type, const Vector3& position, const Quaternion& rotation);

private:
	ModelRender		m_portalFrameModelRender[enPortalType_Num];		//�|�[�^���t���[���B
};

