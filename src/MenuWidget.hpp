#pragma once

class MenuWidget : public GUI::Widget
{
public:
	MenuWidget(const std::string& name, rapidxml::xml_node<>* elem);

	void Draw() override;
	void Update(float dt) override;
	
	virtual void AcceptMessage(const Message& message) override;

protected:
	Render::Texture* _background;
	Render::ShaderProgramPtr _fade;
	float _fading;
};
