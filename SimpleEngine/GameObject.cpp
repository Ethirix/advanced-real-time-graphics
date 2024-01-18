#include "GameObject.h"

GameObject::GameObject(const std::string& name, TransformComponent* parent)
{
	Name = std::move(name);
	Transform = std::make_unique<TransformComponent>(this, parent);
}
