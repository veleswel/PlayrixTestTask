#include "stdafx.h"
#include "MenuWidget.hpp"
#include "GameStateHandler.hpp"

MenuWidget::MenuWidget(const std::string& name, rapidxml::xml_node<>* elem)
	: Widget(name)
	, _background(nullptr)
{
	_background = Core::resourceManager.Get<Render::Texture>("background");
	_fade = Core::resourceManager.Get<Render::ShaderProgram>("fade");
	_fading = 0.f;
}

void MenuWidget::Draw()
{
	GUI::Widget::Draw();
	
	_fade->Bind();
	_fade->SetUniform("u_modelview", math::Matrix4::Identity);
	_fade->SetUniform("sampler", 0);
	_fade->SetUniform("multiplier", _fading);
	_background->Draw();
	_fade->Unbind();
}

void MenuWidget::Update(float dt)
{
	GUI::Widget::Update(dt);
	if (_fading < 1.f)
	{
		_fading += dt * .5f;
	}
	else
	{
		_fading = 1.f;
	}
}

void MenuWidget::AcceptMessage(const Message& message)
{
	const std::string& publisher = message.getPublisher();
	const std::string& data = message.getData();

	const int screenW = Render::device.Width();
	const int screenH = Render::device.Height();

	LayerPtr topLayer = Core::guiManager.getLayer("MenuLayer");
	GUI::WidgetPtr startBtn = topLayer->getWidget("new");
	GUI::WidgetPtr continueBtn = topLayer->getWidget("continue");

	const EGameState currentState = GameStateHandler::GetInstance().GetGameState();
	
	if (publisher == "Layer" && data == "Init")
	{
		if (currentState == EGameState::EMenu || currentState == EGameState::EFinish)
		{
			continueBtn->setVisible(false);
			startBtn->setPosition(IPoint(screenW / 2 - startBtn->getWidth() / 2, screenH / 2 - startBtn->getHeight() / 2));
		}
		else if (currentState == EGameState::EPause)
		{
			startBtn->setPosition(IPoint(screenW / 2 - startBtn->getWidth() / 2, screenH / 2 - startBtn->getHeight() / 2 + 30));
			
			continueBtn->setPosition(IPoint(screenW / 2 - continueBtn->getWidth() / 2, screenH / 2 - continueBtn->getHeight() - 30));
			continueBtn->setVisible(true);
		}
	}
	else if (publisher == "new" && data == "press")
	{
		Core::mainScreen.popLayer();
		Core::mainScreen.pushLayer("MainLayer");
		
		const Message startNewGame(getName(), "startNewGame");
		Core::guiManager.getLayer("MainLayer")->getWidget("MainSceneWidget")->AcceptMessage(startNewGame);
	}
	else if (publisher == "continue" && data == "press")
	{
		Core::mainScreen.popLayer();
		Core::mainScreen.pushLayer("MainLayer");

		const Message continueGame(getName(), "continueGame");
		Core::guiManager.getLayer("MainLayer")->getWidget("MainSceneWidget")->AcceptMessage(continueGame);
	}
}
