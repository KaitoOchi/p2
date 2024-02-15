#pragma once
#include "level2D/Level2D.h"
class Fade;
class Stage;

/// <summary>
/// �^�C�g����ʁB
/// </summary>
class Title : public IGameObject
{
public:
	Title();
	~Title();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

private:
	/// <summary>
	/// �J�[�\���̍X�V�����B
	/// </summary>
	void CursorUpdate();

	/// <summary>
	/// �J�����̍X�V�����B
	/// </summary>
	void CameraUpdate();

private:
	/// <summary>
	/// �^�C�g���X�e�[�g�B
	/// </summary>
	enum TitleState
	{
		enState_Title,
		enState_Guide,
		enState_Exit,
	};

private:
	Level2D						m_level2DRender;		//2D���x���B
	SpriteRender				m_guideSpriteRender;	//�K�C�h�摜�B
	std::array<SpriteRender, 3> m_menuSpriteRender;		//���j���[�摜�B
	SpriteRender				m_cursorSpriteRender;	//�J�[�\���摜�B
	Fade*						m_fade = nullptr;		//�t�F�[�h�B
	Stage*						m_stage = nullptr;		//�X�e�[�W�B
	TitleState					m_state = enState_Title;//�^�C�g���X�e�[�g�B
	Vector3						m_cameraTarget;			//�J���������_�B
	bool						m_isInited = false;		//���x���̏������������������B
	bool						m_isFadeOut = false;	//�t�F�[�h�A�E�g�����ǂ����B
	int							m_cursor = 0;			//�J�[�\���ԍ��B
	float						m_cameraTimer = 0.0f;	//�^�C�}�[�B

};

