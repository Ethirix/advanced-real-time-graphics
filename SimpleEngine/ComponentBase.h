#pragma once

class GameObject;
class ComponentBase
{
public:
	explicit ComponentBase(const std::weak_ptr<GameObject>& owningGameObject)
	: GameObject(owningGameObject) { }

	virtual void Update() = 0;
	virtual void FixedUpdate() = 0;

	std::weak_ptr<GameObject> GameObject = {};
};
