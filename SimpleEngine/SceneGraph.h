#pragma once
#include "GameObject.h"
#include <nlohmann\json.hpp>

class SceneGraph
{
public:
	explicit SceneGraph(const std::string& path);

	GameObject* operator[](std::size_t index) { return _sceneGraph[index].get(); }
	const GameObject* operator[](std::size_t index) const { return _sceneGraph[index].get(); }

private:
	std::vector<std::unique_ptr<GameObject>> _sceneGraph;

	void InitializeSceneGraph(nlohmann::json json, TransformComponent* parent = nullptr);
};
