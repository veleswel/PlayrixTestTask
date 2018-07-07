#pragma once

class GameObject: public RefCounter
{
public:
	virtual ~GameObject();
	
protected:
	GameObject();
	GameObject(const GameObject& gameObject) = delete;
	GameObject& operator=(const GameObject& gameObject) = delete;
	
public:
	virtual void Draw();
	virtual void Update(float dt);
	
	void SetPosition(const FPoint& position);
	void SetPosition(float x, float y);
	const FPoint& GetPosition() const;
	const FPoint GetPositionTransformed() const;
	
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
	Render::Texture* _texture;

	FPoint _position;
	
	float _angle;
	
	float _scale;
	
	FPoint _anchorPoint;
	FPoint _anchorPointTransform;
};
