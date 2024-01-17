#pragma once
#include <memory>
#include <optional>
#include <vector>

#include "ComponentBase.h"

class GameObject
{
public:
	void Update();

	template <typename T>
	T* GetComponent()
	{
		if (!std::is_base_of<ComponentBase, T>() || _components.size() == 0)
			return nullptr;

		for (std::unique_ptr<ComponentBase> component : _components)
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

		for (std::unique_ptr<ComponentBase> component : _components)
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
		for (std::unique_ptr<ComponentBase> component : _components)
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
protected:
	std::vector<std::unique_ptr<ComponentBase>> _components = {};
};

