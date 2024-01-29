#pragma once
#include <d3d11.h>
#include <nlohmann\json.hpp>
#include <wrl/client.h>

#include "GameObject.h"

class SceneGraph
{
public:
	explicit SceneGraph(const std::string& path, const Microsoft::WRL::ComPtr<ID3D11Device>& device);

	GameObject* operator[](std::size_t index) { return _sceneGraph[index].get(); }
	const GameObject* operator[](std::size_t index) const { return _sceneGraph[index].get(); }

	void Update(double deltaTime);
	void FixedUpdate(double fixedDeltaTime);
	void Draw(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);

	template <typename T>
	std::optional<std::weak_ptr<T>> TryGetComponentFromObjects()
	{
		for (std::shared_ptr<GameObject> obj : _sceneGraph)
		{
			if (auto component = obj->TryGetComponent<T>(); component.has_value())
				return component.value();
		}

		return {};
	}

	template <typename T>
	std::vector<std::weak_ptr<T>> GetComponentsFromObjects()
	{
		std::vector<std::weak_ptr<T>> components = {};

		for (std::shared_ptr<GameObject> obj : _sceneGraph)
		{
			if (auto component = obj->TryGetComponent<T>(); component.has_value())
				components.emplace_back(component);
		}

		return components;
	}

private:
	std::vector<std::shared_ptr<GameObject>> _sceneGraph {};

	void InitialiseSceneGraph(const nlohmann::json& json, const Microsoft::WRL::ComPtr<ID3D11Device>& device);

	std::shared_ptr<GameObject> RunInitialisationRecursive(nlohmann::json json, const Microsoft::WRL::ComPtr<ID3D11Device>& device, const std::weak_ptr<TransformComponent>& parent = {});
};
