#pragma once
class GameObject;

class ComponentBase
{
public:
	explicit ComponentBase(GameObject* owningGameObject);

	virtual ~ComponentBase();

	virtual void Update();
	virtual void FixedUpdate();

	GameObject* GameObject;
};
