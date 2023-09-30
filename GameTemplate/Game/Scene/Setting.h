#pragma once

class Setting : public IGameObject
{
public:
	Setting();
	~Setting();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

private:
};

