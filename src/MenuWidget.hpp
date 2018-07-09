#pragma once
#include "Utils.hpp"
#include "Core/ControllerManager.h"

class FadeTransitionController: public IController
{
//	IController();
//	IController(const std::string& name);
//
//	float local_time;
//	bool finished;
//	bool paused;
//
//	const std::string& getName() const;
//

public:
	FadeTransitionController()
	{
		
	}
	
	FadeTransitionController(const std::string& name)
		: IController(name)
	{
		
	}
	
	virtual ~FadeTransitionController()
	{
		
	}
	
public:
	virtual void Update(float dt) override
	{
		
	};
	
	virtual bool isFinish() override
	{
		return _isFinished
	};
	
protected:
	float _start;
	float _finish;
	
	bool _isFinished;
};

class MenuWidget : public GUI::Widget
{
public:
	MenuWidget(const std::string& name, rapidxml::xml_node<>* elem);

	void Draw() override;
	void Update(float dt) override;
	
	virtual void AcceptMessage(const Message& message) override;

protected:
	Utils::EGameState _state;
	Render::Texture* _background;
	float _mult;
};
