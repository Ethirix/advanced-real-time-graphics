#include "GameObject.h"

std::shared_ptr<GameObject> GameObject::Create(const std::string& name, std::weak_ptr<TransformComponent> parent)
{
	auto go = std::make_shared<GameObject>(GameObject());
	go->Name = name;
	go->Transform = std::make_shared<TransformComponent>(go, parent);

	return go;
}

void GameObject::Update(double deltaTime)
{
	Transform->Update(deltaTime);
	for (std::shared_ptr<ComponentBase> component : _components)
		component->Update(deltaTime);
}

void GameObject::FixedUpdate(double fixedDeltaTime)
{
	Transform->FixedUpdate(fixedDeltaTime);
	for (std::shared_ptr<ComponentBase> component : _components)
		component->FixedUpdate(fixedDeltaTime);
}