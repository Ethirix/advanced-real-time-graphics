#include "GameObject.h"

GameObject::GameObject(const std::string& name, std::weak_ptr<TransformComponent> parent)
{
	Name = name;
	Transform = std::make_shared<TransformComponent>(this, parent);
}
