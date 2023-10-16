#pragma once

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

private:
	PointLight				m_pointLight;				//�|�C���g���C�g�B
	SpotLight				m_spotLight;				//�X�|�b�g���C�g�B
};

