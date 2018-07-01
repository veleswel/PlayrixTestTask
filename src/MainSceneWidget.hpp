//
//  MainSceneWidget.hpp
//  Test
//
//  Created by Alexey Vlasenko on 6/29/18.
//

#pragma once
#include "Cannon.hpp"
#include "Projectile.hpp"

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
	FPoint CalculateProjectileStartPosition() const;

	void DestroyProjectile(const ProjectilePtr& projectile);
	
	void UpdateBubbles(float dt);
	void UpdateProjectiles(float dt);
	
	void UpdateCannon(float dt);

protected:
	static const float Speed;
	
protected:
	const FPoint _startPosition;
	
	float _timer;

	CannonPtr _cannon;
	std::list<ProjectilePtr> _launchedProjectiles;
	
	EffectsContainer _effCont;
};
