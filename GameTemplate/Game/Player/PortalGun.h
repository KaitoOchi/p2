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
	/// <param name="rotation">回転</param>
	void SetPortal(const PortalType type, const Vector3& position, const Quaternion& rotation);

private:
	ModelRender		m_portalFrameModelRender[enPortalType_Num];		//ポータルフレーム。
};

