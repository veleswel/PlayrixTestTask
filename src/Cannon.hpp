//
//  Cannon.hpp
//  Test
//
//  Created by Alexey Vlasenko on 6/30/18.
//

#pragma once
#include "GameObject.hpp"

class Cannon: public GameObject
{
public:
	Cannon();
	virtual ~Cannon();
	
	void Draw();
	void Update(float dt);
	
protected:
	void Init();
	
protected:
	Render::Texture* _texture;
};

typedef boost::intrusive_ptr<Cannon> CannonPtr;
