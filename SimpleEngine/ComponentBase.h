#pragma once
class GameObject;

class ComponentBase
{
public:
	explicit ComponentBase(GameObject* owningGameObject);

	virtual ~ComponentBase();

	virtual void Update();
	virtual void FixedUpdate();

	std::weak_ptr<GameObject> GameObject;
};
