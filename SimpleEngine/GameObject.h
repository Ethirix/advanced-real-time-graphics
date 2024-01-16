#pragma once
#include <algorithm>
#include <memory>
#include <optional>
#include <vector>

#include "ComponentBase.h"

class GameObject
{
public:
	void Update();

	template <typename T>
	std::optional<std::weak_ptr<T>> TryGetComponent()
	{
		if (!std::is_base_of<ComponentBase, T>())
			return {};

		std::for_each(_components.cbegin(), _components.cend(), 
			[](std::unique_ptr<ComponentBase> component) -> void	
			{
				T* derivedComponent = dynamic_cast<T*>(*component);

				if (derivedComponent)
				{
					
				}
			});
	}

	template <typename T>
	void AddComponent(T component)
	{
		_components.emplace_back(component);
	}
protected:
	std::vector<std::unique_ptr<ComponentBase>> _components = {};
};

