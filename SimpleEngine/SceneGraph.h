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

	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

private:
	std::vector<std::shared_ptr<GameObject>> _sceneGraph {};

	void InitialiseSceneGraph(const nlohmann::json& json, const Microsoft::WRL::ComPtr<ID3D11Device>& device);

	std::shared_ptr<GameObject> RunInitialisationRecursive(nlohmann::json json, const Microsoft::WRL::ComPtr<ID3D11Device>& device, const std::weak_ptr<TransformComponent>& parent = {});
};
