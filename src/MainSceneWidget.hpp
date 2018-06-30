//
//  MainSceneWidget.hpp
//  Test
//
//  Created by Alexey Vlasenko on 6/29/18.
//

#pragma once

class MainSceneWidget: public GUI::Widget
{
public:
	MainSceneWidget(const std::string& name, rapidxml::xml_node<>* elem);
	
	void Draw() override;
	void Update(float dt) override;
	
	void AcceptMessage(const Message& message) override;
	
	bool MouseDown(const IPoint& mouse_pos) override;
	void MouseMove(const IPoint& mouse_pos) override;
	void MouseUp(const IPoint& mouse_pos) override;
	
	void KeyPressed(int keyCode) override;
	
protected:
	void Init();
	
	void LaunchProjectile(const IPoint& position);
	void DestroyProjectile();
	
	void UpdateBubbles(float dt);
	void UpdateProjectiles(float dt);
	
protected:
	static const float Speed;
	
protected:
	const FPoint _startPosition;
	
	float _timer;

	Render::Texture* _cannon;
	
	Render::Texture* _projectile;
	FPoint _projPosition;
	float _angle;
	
	bool _isProjectileLaunched;
	
	EffectsContainer _effCont;
};
