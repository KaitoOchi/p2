#pragma once


/// <summary>
/// ポータルガンクラス。
/// </summary>
class PortalGun : public IGameObject
{
public:
	/// <summary>
	/// ポータルの種類。
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
	/// ポータルを設置。
	/// </summary>
	/// <param name="type">種類</param>
	/// <param name="position">座標</param>
	void SetPortal(const PortalType type, const Vector3& position);

	/// <summary>
	/// ポータルガンの位置を設定。
	/// </summary>
	void SetPortalGunModel();

private:
	ModelRender		m_portalGunModelRender;							//ポータルガンモデル。
	ModelRender		m_portalFrameModelRender[enPortalType_Num];		//ポータルフレームモデル。。
	SpriteRender	m_crossHairSpriteRender;						//クロスヘア。
};	

