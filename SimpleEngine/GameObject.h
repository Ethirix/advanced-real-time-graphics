#pragma once
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "ComponentBase.h"
#include "TransformComponent.h"

class GameObject : std::enable_shared_from_this<GameObject>
{
public:
	static std::shared_ptr<GameObject> Create(const std::string& name = "DefaultName", std::weak_ptr<TransformComponent> parent = {});
	void Update(double deltaTime);
	void FixedUpdate(double fixedDeltaTime);

	std::shared_ptr<GameObject> This() { return shared_from_this(); }

	std::string Name;
	std::shared_ptr<TransformComponent> Transform;

#pragma region Component Functions
	template <typename T>
	std::weak_ptr<T> GetComponent()
	{
		if (!std::is_base_of<ComponentBase, T>() || _components.size() == 0)
			return {};

		for (std::shared_ptr<ComponentBase> component : _components)
		{
			if (std::shared_ptr<T> derivedComponent = std::dynamic_pointer_cast<T>(component); derivedComponent)
				return derivedComponent;
		}

		return {};
	}

	template <typename T>
	std::optional<std::weak_ptr<T>> TryGetComponent()
	{
		if (!std::is_base_of<ComponentBase, T>() || _components.size() == 0)
			return {};

		for (std::shared_ptr<ComponentBase> component : _components)
		{
			if (std::shared_ptr<T> derivedComponent = std::dynamic_pointer_cast<T>(component); derivedComponent)
				return derivedComponent;
		}

		return {};
	}

	template <typename T>
	std::vector<std::weak_ptr<T>> GetComponents()
	{
		if (!std::is_base_of<ComponentBase, T>() || _components.size() == 0)
			return {};

		std::vector<std::weak_ptr<T>> components;
		for (std::shared_ptr<ComponentBase> component : _components)
		{
			if (std::shared_ptr<T> derivedComponent = std::dynamic_pointer_cast<T>(component); derivedComponent)
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

