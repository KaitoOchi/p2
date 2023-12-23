#pragma once
#include "PortalFrame.h"
class PortalCamera;
class Game;

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

	/// <summary>
	/// ポータルのリセット処理。
	/// </summary>
	void ResetPortal();

private:
	/// <summary>
	/// ポータルガンの位置を設定。
	/// </summary>
	void SetPortalGunModel();

private:
	std::array<PortalFrame*, PORTAL_NUM>	m_portalFrame;					//ポータル。
	PortalCamera*							m_portalCamera = nullptr;		//ポータル用カメラ。
	Game*									m_game = nullptr;				//ゲームシーン。
	ModelRender								m_portalGunModelRender;			//ポータルガンモデル。
	std::array<SpriteRender, 3>				m_cursorSpriteRender;			//クロスヘア。
	float									m_shotDuration = 0.0f;			//発射間隔。
};	

