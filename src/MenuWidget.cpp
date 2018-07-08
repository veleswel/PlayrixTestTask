#include "stdafx.h"
#include "MenuWidget.hpp"
#include "GUI/SmoothButton.h"

MenuWidget::MenuWidget(const std::string& name, rapidxml::xml_node<>* elem)
	: Widget(name)
	, _state(Utils::EGameState::EMenu)
{
	
}

void MenuWidget::Draw()
{
	GUI::Widget::Draw();
}

void MenuWidget::Update(float dt)
{
	GUI::Widget::Update(dt);
}

void MenuWidget::AcceptMessage(const Message& message)
{
	const std::string& publisher = message.getPublisher();
	const std::string& data = message.getData();

	const int screenW = Render::device.Width();
	const int screenH = Render::device.Height();

	Layer* topLayer = Core::guiManager.getLayer("MenuLayer");
	auto startBtn = topLayer->getWidget("start");
	auto continueBtn = topLayer->getWidget("continue");

	if (publisher == "Layer" && data == "Init")
	{
		if (_state == Utils::EGameState::EMenu)
		{
			continueBtn->setVisible(false);
			startBtn->setPosition(IPoint(screenW / 2 - 45, screenH / 2 - 34));
		}
		else if (_state == Utils::EGameState::EPlaying)
		{
			_state = Utils::EGameState::EPause;

			startBtn->setPosition(IPoint(screenW / 2 - 45, screenH / 2 + 30));
			
			continueBtn->setPosition(IPoint(screenW / 2 - 45, screenH / 2 - 30 - 34));
			continueBtn->setVisible(true);
		}
	}
	else if (publisher == "start" && data == "press")
	{
		const Message startNewGame(getName(), "startNewGame");
		Core::guiManager.getLayer("MainLayer")->getWidget("MainSceneWidget")->AcceptMessage(startNewGame);
		
		Core::mainScreen.popLayer();
		Core::mainScreen.pushLayer("MainLayer");

		_state = Utils::EGameState::EPlaying;
	}
	else if (publisher == "continue" && data == "press")
	{
		Core::mainScreen.popLayer();
		Core::mainScreen.pushLayer("MainLayer");

		_state = Utils::EGameState::EPlaying;
	}
}