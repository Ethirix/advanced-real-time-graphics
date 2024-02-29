#pragma once
#include <d3d11.h>
#include <nlohmann\json.hpp>
#include <wrl/client.h>

#include "ColliderComponent.h"
#include "GameObject.h"
#include "CollisionResponse.h"

class SceneGraph
{
public:
	static void Initialize(const std::string& path, const Microsoft::WRL::ComPtr<ID3D11Device>& device);

	static std::weak_ptr<GameObject> GetObjectAtPosition(unsigned index);

	static void Update(double deltaTime);
	static void FixedUpdate(double fixedDeltaTime);
	static void Draw(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);
	static int Size();

	static std::list<CollisionResponse> CheckColliders(const std::shared_ptr<ColliderComponent>& collider);

	template <typename T>
	static std::optional<std::weak_ptr<T>> TryGetComponentFromObjects()
	{
		for (std::shared_ptr<GameObject> obj : _sceneGraph)
		{
			if (auto component = obj->TryGetComponent<T>(); component.has_value())
				return component.value();
		}

		return {};
	}

	template <typename T>
	static std::vector<std::weak_ptr<T>> GetComponentsFromObjects()
	{
		std::vector<std::weak_ptr<T>> components = {};

		for (std::shared_ptr<GameObject> obj : _sceneGraph)
		{
			if (std::optional<std::weak_ptr<T>> component = obj->TryGetComponent<T>(); component.has_value())
				components.emplace_back(component.value());
		}

		return components;
	}

private:
	inline static std::vector<std::shared_ptr<GameObject>> _sceneGraph {};

	static void InitialiseSceneGraph(const nlohmann::json& json, const Microsoft::WRL::ComPtr<ID3D11Device>& device);

	static std::shared_ptr<GameObject> RunInitialisationRecursive(nlohmann::json json, const Microsoft::WRL::ComPtr<ID3D11Device>& device, const std::weak_ptr<TransformComponent>& parent = {});
};
