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
	void SetPortal(const PortalType type, const Vector3& position);

	/// <summary>
	/// �|�[�^���K���̈ʒu��ݒ�B
	/// </summary>
	void SetPortalGunModel();

private:
	ModelRender		m_portalGunModelRender;							//�|�[�^���K�����f���B
	ModelRender		m_portalFrameModelRender[enPortalType_Num];		//�|�[�^���t���[�����f���B�B
	SpriteRender	m_crossHairSpriteRender;						//�N���X�w�A�B
};	

