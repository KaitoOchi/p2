#pragma once
#include "PortalFrame.h"
class PortalCamera;

/// <summary>
/// ポータルガンクラス。
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
	/// ポータルの座標を取得。
	/// </summary>
	const Vector3& GetPortalPosition(const PortalFrame::PortalType type) const
	{
		int num = static_cast<int>(type);
		return m_portalFrame[num]->GetPosition();
	}

	/// <summary>
	/// ポータルを設置。
	/// </summary>
	/// <param name="type">種類</param>
	void SetPortal(const PortalFrame::PortalType type);

private:
	/// <summary>
	/// ポータルガンの位置を設定。
	/// </summary>
	void SetPortalGunModel();

private:
	PortalFrame*	m_portalFrame[PORTAL_NUM];			//ポータル。
	PortalCamera*	m_portalCamera = nullptr;			//ポータル用カメラ。
	ModelRender		m_portalGunModelRender;				//ポータルガンモデル。
	SpriteRender	m_crossHairSpriteRender;			//クロスヘア。
};	

