#pragma once

/* Базовый класс для все игровых объектов. 
Имеет текстуру, позицию, угол поворота, масштаб 
и точку привязки текстуры */

class GameObject: public RefCounter
{
public:
	virtual ~GameObject();
	
protected:
	GameObject();
	GameObject(const GameObject& gameObject) = delete;
	GameObject& operator=(const GameObject& gameObject) = delete;
	
public:
	virtual void Update(float dt) = 0;
	virtual void Draw();
	
	virtual void SetPosition(const FPoint& position);
	virtual void SetPosition(float x, float y);

	const FPoint& GetPosition() const;
	
	void SetAnchorPoint(const FPoint& point);
	const FPoint& GetAnchorPoint() const;
	
	void SetRotationAngle(float angle);
	float GetRotationAngle() const;
	
	void SetScale(float scale);
	float GetScale() const;
	
	const FRect GetOriginalTextureRect() const;
	const FRect GetScaledTextureRect() const;

protected:
	void Init(const std::string& textureName, const FPoint& position, float rotation);
	
protected:
	Render::TexturePtr _texture;

	FPoint _position;
	
	float _angle;
	
	float _scale;
	
	FPoint _anchorPoint;
	FPoint _anchorPointTransform;
};
