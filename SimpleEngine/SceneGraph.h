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

private:
	std::vector<std::shared_ptr<GameObject>> _sceneGraph {};

	void InitializeSceneGraph(const nlohmann::json& json, const Microsoft::WRL::ComPtr<ID3D11Device>& device);

	std::shared_ptr<GameObject> RunInitializationRecursive(nlohmann::json json, const Microsoft::WRL::ComPtr<ID3D11Device>& device, const std::weak_ptr<TransformComponent>& parent = {});
};
