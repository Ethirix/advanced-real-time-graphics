#include "SceneGraph.h"
#include <fstream>

#include "MeshComponent.h"

SceneGraph::SceneGraph(const std::string& path, const Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
	std::ifstream fileStream(path);
	nlohmann::json json = nlohmann::json::parse(fileStream);
	fileStream.close();

	InitializeSceneGraph(json, device);
}

void SceneGraph::InitializeSceneGraph(const nlohmann::json& json, 
	const Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
	for (auto& objs : json["GameObjects"])
	{
		auto obj = RunInitializationRecursive(objs, device);
		_sceneGraph.emplace_back(std::move(obj));
	}
}

std::shared_ptr<GameObject> SceneGraph::RunInitializationRecursive(
	nlohmann::json json,
	const Microsoft::WRL::ComPtr<ID3D11Device>& device,
	std::weak_ptr<TransformComponent> parent)
{
	auto obj = std::make_shared<GameObject>(json["Name"], parent);
	nlohmann::json position = json["Position"];
	obj->Transform->SetPosition(position["X"], position["Y"], position["Z"]);
	nlohmann::json scale = json["Scale"];
	obj->Transform->SetScale(scale["X"], scale["Y"], scale["Z"]);
	nlohmann::json rotation = json["Rotation"];
	obj->Transform->SetRotation(rotation["X"], rotation["Y"], rotation["Z"]);

	for (auto component : json["Components"])
	{
		if (std::string type = component["Type"]; type == "MeshComponent")
		{
			auto meshComponent = MeshComponent(
				obj.get(),
				device,
				component["MeshPath"],
				component["MaterialPath"],
				component["VertexShaderPath"],
				component["PixelShaderPath"],
				component["MeshType"]
			);
			
			obj->AddComponent(meshComponent);
		}
		else if (type == "PhysicsComponent")
		{
			//TODO: Add physics component
		}
	}

	for (auto children : json["Children"])
	{
		auto child = RunInitializationRecursive(children, device, obj->Transform);
		_sceneGraph.emplace_back(std::move(child));
	}

	return obj;
}

