#include "GameObject.h"

std::shared_ptr<GameObject> GameObject::Create(const std::string& name, std::weak_ptr<TransformComponent> parent)
{
	auto go = std::make_shared<GameObject>(GameObject());
	go->Name = name;
	go->Transform = std::make_shared<TransformComponent>(go, parent);

	return go;
}

//GameObject::GameObject(const std::string& name, std::weak_ptr<TransformComponent> parent)
//{
//	Name = name;
//	Transform = std::make_shared<TransformComponent>(This(), parent);
//}
