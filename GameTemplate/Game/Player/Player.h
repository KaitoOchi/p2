#pragma once
#include "GameCamera.h"

class Player : public IGameObject
{
public:
	/// <summary>
	/// �v���C���[�X�e�[�g�B
	/// </summary>
	enum PlayerState
	{
		enState_Idle,
		enState_Walk,
		enState_Run,
		enState_Crouch,
		enState_Jump,
	};

public:
	Player();
	~Player();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

private:
	/// <summary>
	/// ���͏����B
	/// </summary>
	void Input();

	/// <summary>
	/// �ړ������B
	/// </summary>
	void Move();

	/// <summary>
	/// ��]�����B
	/// </summary>
	void Rotation();

	/// <summary>
	/// �X�e�[�g�����B
	/// </summary>
	void State();

	/// <summary>
	/// ���ʑJ�ڏ����B
	/// </summary>
	void ProcessCommonStateTransition();

	/// <summary>
	/// �ҋ@��Ԃ̑J�ڏ����B
	/// </summary>
	void ProcessIdleStateTransition();

	/// <summary>
	/// ������Ԃ̑J�ڏ����B
	/// </summary>
	void ProcessWalkStateTransition();

	/// <summary>
	/// �����Ԃ̑J�ڏ����B
	/// </summary>
	void ProcessRunStateTransition();

	/// <summary>
	/// ���Ⴊ�ݏ�Ԃ̑J�ڏ����B
	/// </summary>
	void ProcessCrouchStateTransition();

	/// <summary>
	/// �W�����v��Ԃ̑J�ڏ����B
	/// </summary>
	void ProcessJumpStateTransition();

private:
	ModelRender			m_modelRender;		//���f�������_�[�B
	Vector3				m_position;			//���W�B
	Quaternion			m_rotation;			//��]�B
	GameCamera			m_gameCamera;		//�Q�[���J�����B
	PlayerState			m_playerState;		//�v���C���[�X�e�[�g�B


	FontRender a;
};

