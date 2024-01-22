#pragma once
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "ComponentBase.h"
#include "TransformComponent.h"

class GameObject
{
public:
	explicit GameObject(const std::string& name = "DefaultName", std::weak_ptr<TransformComponent> parent = {});
	void Update();

	std::string Name;
	std::shared_ptr<TransformComponent> Transform;

#pragma region Component Functions
	template <typename T>
	T* GetComponent()
	{
		if (!std::is_base_of<ComponentBase, T>() || _components.size() == 0)
			return nullptr;

		for (std::shared_ptr<ComponentBase> component : _components)
		{
			if (T* derivedComponent = dynamic_cast<T*>(component.get()); derivedComponent)
				return derivedComponent;
		}

		return nullptr;
	}

	template <typename T>
	std::optional<T*> TryGetComponent()
	{
		if (!std::is_base_of<ComponentBase, T>() || _components.size() == 0)
			return {};

		for (std::shared_ptr<ComponentBase> component : _components)
		{
			if (T* derivedComponent = dynamic_cast<T*>(component.get()); derivedComponent)
				return derivedComponent;
		}

		return {};
	}

	template <typename T>
	std::vector<T*> GetComponents()
	{
		if (!std::is_base_of<ComponentBase, T>() || _components.size() == 0)
			return {};

		std::vector<T*> components;
		for (std::shared_ptr<ComponentBase> component : _components)
		{
			if (T* derivedComponent = dynamic_cast<T*>(component.get()); derivedComponent)
				components.emplace_back(derivedComponent);
		}

		return components;
	}

	template <typename T>
	void AddComponent(T component)
	{
		_components.emplace_back(component);
	}
#pragma endregion
protected:
	std::vector<std::shared_ptr<ComponentBase>> _components = {};
};

