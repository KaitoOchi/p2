#pragma once

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
	/// �����n�_���擾�B
	/// </summary>
	/// <returns></returns>
	const Vector3& GetRespawnPoint() const
	{
		return m_respawnPoint;
	}

private:
	PhysicsStaticObject		m_physicsStaticObject;		//�����ÓI�I�u�W�F�N�g�B
	ModelRender				m_modelRender;				//�n�ʃ��f���B
	Vector3					m_respawnPoint;				//���X�|�[���n�_�B
};