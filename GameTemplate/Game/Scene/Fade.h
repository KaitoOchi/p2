#pragma once


/// <summary>
/// フェードクラス。
/// </summary>
class Fade : public IGameObject
{
public:
	Fade();
	~Fade();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

	/// <summary>
	/// フェードイン処理。
	/// </summary>
	void StartFadeIn()
	{
		m_state = enState_FadeIn;
	}
	
	/// <summary>
	/// フェードアウト処理。
	/// </summary>
	void StartFadeOut()
	{
		m_state = enState_FadeOut;
	}
	
	/// <summary>
	/// フェード中かどうか。
	/// </summary>
	/// <returns></returns>
	const bool IsFade() const
	{
		return m_state != enState_Idle;
	}

private:
	/// <summary>
	/// 画像の更新処理。
	/// </summary>
	void SpriteUpdate();

private:
	enum EnState {
		enState_FadeIn,			//フェードイン中
		enState_FadeOut,		//フェードアウト中
		enState_Idle,			//待機中
	};

	SpriteRender	m_spriteRender;					//背景画像
	EnState			m_state = enState_Idle;			//状態
	float			m_alpha = 1.0f;					//透明度
	float			m_rotTimer = 0.0f;				//回転量
};

