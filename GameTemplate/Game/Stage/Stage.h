#pragma once
#include "LevelRender/Level3DRender.h"
class Gimmick;
class EnergyBall;

/// <summary>
/// �X�e�[�W�N���X�B
/// </summary>
class Stage : public IGameObject
{
public:
	Stage();
	~Stage();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

public:
	/// <summary>
	/// �������W���擾�B
	/// </summary>
	const Vector3& GetRespawnPosition() const
	{
		return m_respawnPosition;
	}

	/// <summary>
	/// �N���A���W���擾�B
	/// </summary>
	const Vector3& GetClearPosition() const
	{
		return m_clearPosition;
	}

	/// <summary>
	/// �����ÓI�I�u�W�F�N�g�̎擾�B
	/// </summary>
	/// <returns></returns>
	PhysicsStaticObject& GetPhysicsStaticObject()
	{
		return m_physicsStaticObject;
	}

	/// <summary>
	/// �G�l���M�[�{�[���I�u�W�F�N�g��z��ɒǉ��B
	/// </summary>
	/// <param name="obj"></param>
	void AddEnergyBallObject(EnergyBall* obj)
	{
		m_energyBallObjects.emplace_back(obj);
	}

	/// <summary>
	/// �G�l���M�[�{�[���I�u�W�F�N�g��z�񂩂�폜�B
	/// </summary>
	/// <param name="obj"></param>
	void RemoveEnergyBallObject(EnergyBall* obj)
	{
		for (int i = 0; i < m_energyBallObjects.size(); i++) {
			if (m_energyBallObjects[i] == obj) {
				m_energyBallObjects.erase(m_energyBallObjects.begin() + i);
			}
		}
	}

	/// <summary>
	/// �G�l���M�[�{�[���I�u�W�F�N�g�z����擾�B
	/// </summary>
	/// <returns></returns>
	std::vector<EnergyBall*> GetEnergyBallObjects()
	{
		return m_energyBallObjects;
	}

	/// <summary>
	/// �M�~�b�N�������B
	/// </summary>
	/// <param name="num"></param>
	void ClearGimmickObject(const int num);

private:
	/// <summary>
	/// ���x���f�U�C�����������B
	/// </summary>
	void InitLevelRender();

private:
	std::vector<Gimmick*>		m_gimmickObjects;			//�����p�̃M�~�b�N�I�u�W�F�N�g�B
	std::vector<EnergyBall*>	m_energyBallObjects;		//�G�l���M�[�{�[���I�u�W�F�N�g�B
	std::vector<PointLight*>	m_pointLightObjects;		//�|�C���g���C�g�I�u�W�F�N�g�B
	std::vector<SpotLight*>		m_spotLightObjects;			//�X�|�b�g���C�g�I�u�W�F�N�g�B
	PhysicsStaticObject			m_physicsStaticObject;		//�����ÓI�I�u�W�F�N�g�B
	ModelRender					m_decorationModelRender;	//�������f���B
	ModelRender					m_physicsModelRender;		//�������f���B
	LevelRender					m_levelRender;				//���x�������_�[�B
	Vector3						m_respawnPosition;			//���X�|�[�����W�B
	Vector3						m_clearPosition;			//�N���A���W�B
};