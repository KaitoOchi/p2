#pragma once
class Stage;
class Player;
class GameCamera;
class PortalGun;

namespace
{
	const int TIMER_NUM = 6;					//�\�����鎞�Ԃ̌��B
	const int NUMBER_SPRITE_NUM = 10;			//�����̌��B
}

/// <summary>
/// �Q�[�����C���N���X�B
/// </summary>
class Game : public IGameObject
{
public:
	/// <summary>
	/// �Q�[���X�e�[�g�B
	/// </summary>
	enum GameState
	{
		enGameState_Game,
		enGameState_Result,
		enGameState_Reset,
	};

	/// <summary>
	/// �X�R�A�B
	/// </summary>
	struct GameScore
	{
		float	timer = 0.0f;		//�o�ߎ��ԁB
		int		stepNum = 0;		//�������񐔁B
		int		shotPortal = 0;	//�|�[�^���̔��ː��B
	};

public:
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

public:
	/// <summary>
	/// ���Z�b�g��ʒm�B
	/// </summary>
	void NotifyReset();
	/// <summary>
	/// �N���A��ʒm�B
	/// </summary>
	void NotifyClear();
	/// <summary>
	/// ���g���C��ʒm�B
	/// </summary>
	void NotifyRetry();
	/// <summary>
	/// �Q�[���I����ʒm�B
	/// </summary>
	void NotifyEnd();
	/// <summary>
	/// �������W���擾�B
	/// </summary>
	/// <returns></returns>
	const Vector3& GetRespawnPosition() const;
	/// <summary>
	/// �N���A���W���擾�B
	/// </summary>
	const Vector3& GetClearPosition() const;

	/// <summary>
	/// �X�e�[�g���擾�B
	/// </summary>
	/// <returns></returns>
	const GameState GetGameState() const
	{
		return m_gameState;
	}

	/// <summary>
	/// �������񐔂����Z�B
	/// </summary>
	void AddStepNum()
	{
		m_gameScore.stepNum++;
	}
	/// <summary>
	/// �|�[�^���̔��ː������Z�B
	/// </summary>
	void AddShotNum()
	{
		m_gameScore.shotPortal++;
	}

private:
	/// <summary>
	/// ���Z�b�g�����B
	/// </summary>
	void Reset();
	/// <summary>
	/// �v�������B
	/// </summary>
	void Timer();

private:
	FontRender			m_timerFontRender;					//���ԃX�v���C�g�B
	Stage*				m_stage = nullptr;					//�X�e�[�W�B
	Player*				m_player = nullptr;					//�v���C���[�B
	GameCamera*			m_gameCamera = nullptr;				//�J�����B
	PortalGun*			m_portalGun = nullptr;				//�|�[�^���K���B
	GameScore			m_gameScore;						//�X�R�A�B
	GameState			m_gameState = enGameState_Game;		//�Q�[���X�e�[�g�B
	float				m_resetTimer = 0.0f;				//���Z�b�g�p�^�C�}�[�B
};

