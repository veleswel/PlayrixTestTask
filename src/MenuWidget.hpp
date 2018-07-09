#pragma once
#include "Fadeable.hpp"

class MenuWidget : public GUI::Widget, public Fadeable
{
public:
	MenuWidget(const std::string& name, rapidxml::xml_node<>* elem);

	void Draw() override;
	void Update(float dt) override;
	
	virtual void AcceptMessage(const Message& message) override;

protected:
	Utils::EGameWidgetState _state;
	Render::Texture* _background;
};
