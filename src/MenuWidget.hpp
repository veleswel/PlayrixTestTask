#pragma once

// Виджет для представления меню игры. Содержит фон, прочие декоративные элементы, а так же шейдер, 
// который реализует fade эффект при запуске игры 

class MenuWidget : public GUI::Widget
{
public:
	MenuWidget(const std::string& name, rapidxml::xml_node<>* elem);

	void Draw() override;
	void Update(float dt) override;
	
	virtual void AcceptMessage(const Message& message) override;

protected:
	void Init();
	void DrawPlay();

protected:
	static const int PlayOffset;

protected:
	Render::TexturePtr _background;
	
	Render::TexturePtr _play;
	IPoint _playPosition;
	
	Render::ShaderProgramPtr _fade;
	float _fading;
};
