#include "GameObject.h"

GameObject::GameObject(const std::string& name, TransformComponent* parent)
{
	Name = name;
	Transform = std::make_unique<TransformComponent>(this, parent);
}
