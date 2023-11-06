#pragma once
class Stage;

/// <summary>
/// �Q�[�����C���N���X�B
/// </summary>
class Game : public IGameObject
{
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
	/// �����n�_���擾�B
	/// </summary>
	/// <returns></returns>
	const Vector3& GetRespawnPoint() const;

private:
	Stage*					m_stage = nullptr;			//�X�e�[�W�B
	PointLight				m_pointLight;				//�|�C���g���C�g�B
	SpotLight				m_spotLight;				//�X�|�b�g���C�g�B
};

