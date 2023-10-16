#pragma once
#include "PortalFrame.h"
class PortalCamera;

/// <summary>
/// �|�[�^���K���N���X�B
/// </summary>
class PortalGun : public IGameObject
{
public:
	PortalGun();
	~PortalGun();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

public:
	/// <summary>
	/// �|�[�^���̍��W���擾�B
	/// </summary>
	const Vector3& GetPortalPosition(const PortalFrame::PortalType type) const
	{
		int num = static_cast<int>(type);
		return m_portalFrame[num]->GetPosition();
	}

	/// <summary>
	/// �|�[�^����ݒu�B
	/// </summary>
	/// <param name="type">���</param>
	void SetPortal(const PortalFrame::PortalType type);

private:
	/// <summary>
	/// �|�[�^���K���̈ʒu��ݒ�B
	/// </summary>
	void SetPortalGunModel();

private:
	PortalFrame*	m_portalFrame[PORTAL_NUM];			//�|�[�^���B
	PortalCamera*	m_portalCamera = nullptr;			//�|�[�^���p�J�����B
	ModelRender		m_portalGunModelRender;				//�|�[�^���K�����f���B
	SpriteRender	m_crossHairSpriteRender;			//�N���X�w�A�B
};	

