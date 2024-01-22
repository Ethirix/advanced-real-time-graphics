#include "SceneGraph.h"
#include <fstream>

#include "MeshComponent.h"

SceneGraph::SceneGraph(const std::string& path)
{
	std::ifstream fileStream(path);
	nlohmann::json json = nlohmann::json::parse(fileStream);
	fileStream.close();

	InitializeSceneGraph(json);
}

void SceneGraph::InitializeSceneGraph(nlohmann::json json, TransformComponent* parent)
{
	for (auto objs : json)
	{
		GameObject obj = GameObject(objs["Name"], parent);
		nlohmann::json position = json["Position"];
		obj.Transform->SetPosition(position["X"], position["Y"], position["Z"]);
		nlohmann::json scale = json["Scale"];
		obj.Transform->SetScale(scale["X"], scale["Y"], scale["Z"]);
		nlohmann::json rotation = json["Rotation"];
		obj.Transform->SetRotation(rotation["X"], rotation["Y"], rotation["Z"]);

		for (auto component : json["Components"])
		{
			std::string type = component["Type"];

			if (type == "MeshComponent")
			{
				MeshComponent meshComponent = MeshComponent();
				obj.AddComponent(meshComponent);
			}
			
		}
	}
}
