#pragma once

class JumpBoard : public IGameObject
{
public:
	JumpBoard();
	~JumpBoard();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

private:
};

