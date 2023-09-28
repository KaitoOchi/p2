#pragma once


/// <summary>
/// �t�F�[�h�N���X�B
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
	/// �t�F�[�h�C�������B
	/// </summary>
	void StartFadeIn()
	{
		m_state = enState_FadeIn;
	}
	
	/// <summary>
	/// �t�F�[�h�A�E�g�����B
	/// </summary>
	void StartFadeOut()
	{
		m_state = enState_FadeOut;
	}
	
	/// <summary>
	/// �t�F�[�h�����ǂ����B
	/// </summary>
	/// <returns></returns>
	const bool IsFade() const
	{
		return m_state != enState_Idle;
	}

private:
	/// <summary>
	/// �摜�̍X�V�����B
	/// </summary>
	void SpriteUpdate();

private:
	enum EnState {
		enState_FadeIn,			//�t�F�[�h�C����
		enState_FadeOut,		//�t�F�[�h�A�E�g��
		enState_Idle,			//�ҋ@��
	};

	SpriteRender	m_spriteRender;					//�w�i�摜
	EnState			m_state = enState_Idle;			//���
	float			m_alpha = 1.0f;					//�����x
	float			m_rotTimer = 0.0f;				//��]��
};

