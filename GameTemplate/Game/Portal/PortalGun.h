#pragma once
#include "PortalFrame.h"
class PortalCamera;
class Game;

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

	/// <summary>
	/// �|�[�^���̃��Z�b�g�����B
	/// </summary>
	void ResetPortal();

private:
	/// <summary>
	/// �|�[�^���K���̈ʒu��ݒ�B
	/// </summary>
	void SetPortalGunModel();

private:
	std::array<PortalFrame*, PORTAL_NUM>	m_portalFrame;					//�|�[�^���B
	PortalCamera*							m_portalCamera = nullptr;		//�|�[�^���p�J�����B
	Game*									m_game = nullptr;				//�Q�[���V�[���B
	ModelRender								m_portalGunModelRender;			//�|�[�^���K�����f���B
	std::array<SpriteRender, 3>				m_cursorSpriteRender;			//�N���X�w�A�B
	float									m_shotDuration = 0.0f;			//���ˊԊu�B
};	

