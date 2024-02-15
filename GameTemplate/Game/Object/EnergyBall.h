#pragma once
#include "Gimmick.h"
class EnergyBallGenerator;
class Player;
class Stage;

/// <summary>
/// �G�l���M�[�{�[���B
/// </summary>
class EnergyBall : public Gimmick
{
public:
	EnergyBall();
	~EnergyBall() override;
	bool Start();
	void Update();

public:
	/// <summary>
	/// ���W��ݒ�B
	/// </summary>
	/// <param name="pos"></param>
	void SetPosition(const Vector3& pos)
	{
		m_position = pos;
	}

	/// <summary>
	/// ���W���擾�B
	/// </summary>
	/// <returns></returns>
	const Vector3& GetPosition()
	{
		return m_position;
	}

	/// <summary>
	/// ������ݒ�B
	/// </summary>
	/// <param name="dir"></param>
	void SetDirection(const Vector3& dir)
	{
		m_direction = dir;
	}

	/// <summary>
	/// �����@��ݒ�B
	/// </summary>
	/// <param name="generator"></param>
	void SetGenerator(EnergyBallGenerator* generator)
	{
		m_energyBallGenerator = generator;
	}

	/// <summary>
	/// �L�����B
	/// </summary>
	void Enable();

	/// <summary>
	/// �R���W�������擾�B
	/// </summary>
	/// <returns></returns>
	CollisionObject* GetCollisionObject()
	{
		return m_collisionObject;
	}

	/// <summary>
	/// �폜�����B
	/// </summary>
	void Delete();

	/// <summary>
	/// ���[�v�����B
	/// </summary>
	/// <param name="pos">���W</param>
	/// <param name="direction">����</param>
	void SetWarp(const Vector3& pos, const Vector3& direction);

	/// <summary>
	/// �M�~�b�N�̐��������B
	/// </summary>
	void Clear() override;

private:
	/// <summary>
	/// �ړ������B
	/// </summary>
	void Move();

	/// <summary>
	/// ���C���΂������B
	/// </summary>
	void RayCast();

	/// <summary>
	/// �����蔻��̏����B
	/// </summary>
	void Collision();

	/// <summary>
	/// �������B
	/// </summary>
	void Disable();

private:
	Stage*					m_stage = nullptr;					//�X�e�[�W�B
	CollisionObject*		m_collisionObject = nullptr;		//�R���W�����B
	EffectEmitter*			m_ballEffect = nullptr;				//�{�[���G�t�F�N�g�B
	EnergyBallGenerator*	m_energyBallGenerator = nullptr;	//�����@�B
	Player*					m_player = nullptr;					//�v���C���[�B
	Vector3					m_direction;						//�ړ���������B
	Vector3					m_moveSpeed;						//�ړ����x�B
	bool					m_isEnable = false;					//�\����Ԃ��L�����ǂ����B
	float					m_timer = 0.0f;						//�^�C�}�[�B
};

