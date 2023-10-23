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

private:
	PhysicsStaticObject		m_physicsStaticObject;		//�����ÓI�I�u�W�F�N�g�B
	ModelRender				m_modelRender;				//�n�ʃ��f���B
};