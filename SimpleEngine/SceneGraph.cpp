#include "SceneGraph.h"
#include <fstream>

#include "CameraComponent.h"
#include "LightComponent.h"
#include "MeshComponent.h"

SceneGraph::SceneGraph(const std::string& path, const Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
	std::ifstream fileStream(path);
	nlohmann::json json = nlohmann::json::parse(fileStream);
	fileStream.close();

	InitialiseSceneGraph(json, device);
}

void SceneGraph::InitialiseSceneGraph(const nlohmann::json& json, 
	const Microsoft::WRL::ComPtr<ID3D11Device>& device)
{
	for (auto& objs : json["GameObjects"])
	{
		auto obj = RunInitialisationRecursive(objs, device);
		_sceneGraph.emplace_back(std::move(obj));
	}
}

std::shared_ptr<GameObject> SceneGraph::RunInitialisationRecursive(
	nlohmann::json json,
	const Microsoft::WRL::ComPtr<ID3D11Device>& device,
	const std::weak_ptr<TransformComponent>& parent)
{
	auto obj = GameObject::Create(json["Name"], parent);
	nlohmann::json position = json["Position"];
	obj->Transform->SetPosition(position["X"], position["Y"], position["Z"]);
	nlohmann::json scale = json["Scale"];
	obj->Transform->SetScale(scale["X"], scale["Y"], scale["Z"]);
	nlohmann::json rotation = json["Rotation"];
	obj->Transform->SetRotation(rotation["X"], rotation["Y"], rotation["Z"]);

	for (nlohmann::json component : json["Components"])
	{
		if (std::string type = component["Type"]; type == "MeshComponent")
		{
			nlohmann::json textures = component["TexturePaths"];
			auto meshComponent = std::make_shared<MeshComponent>(
				obj,
				device,
				component["MeshPath"],
				component["MaterialPath"],
				component["VertexShaderPath"],
				component["PixelShaderPath"],
				component["MeshType"],
				textures["Diffuse"],
				textures["Displacement"],
				textures["Normal"],
				textures["Specular"]
			);

			obj->AddComponent(meshComponent);
		}
		else if (type == "PhysicsComponent")
		{
			//TODO: Add physics component
		}
		else if (type == "CameraComponent")
		{
			auto cameraComponent = std::make_shared<CameraComponent>(
				obj,
				component["FieldOfView"],
				DirectX::XMFLOAT3(
					component["At"]["X"], 
					component["At"]["Y"], 
					component["At"]["Z"]),
				DirectX::XMFLOAT3(
					component["Up"]["X"], 
					component["Up"]["Y"],
					component["Up"]["Z"]),
				component["NearPlane"],
				component["FarPlane"],
				component["MovementSpeed"],
				component["RotationSpeed"]
			);

			obj->AddComponent(cameraComponent);
		}
		else if (type == "LightComponent")
		{
			nlohmann::json diffuseColour = component["DiffuseColour"];
			nlohmann::json specularColour = component["SpecularColour"];
			nlohmann::json ambientColour = component["AmbientColour"];
			DirectX::XMFLOAT3 pos = obj->Transform->GetPosition();
			auto lightComponent = std::make_shared<LightComponent>(
				obj,
				LightData(
					DirectX::XMFLOAT4(pos.x, pos.y, pos.z, 1),
					DirectX::XMFLOAT3(diffuseColour["R"], diffuseColour["G"], diffuseColour["B"]),
					component["DiffusePower"],
					DirectX::XMFLOAT3(specularColour["R"], specularColour["G"], specularColour["B"]),
					component["SpecularPower"],
					DirectX::XMFLOAT3(ambientColour["R"], ambientColour["G"], ambientColour["B"]),
					component["ConstantAttenuation"],
					component["LinearAttenuation"],
					component["QuadraticAttenuation"],
					component["LightRadius"]
				)
			);

			obj->AddComponent(lightComponent);
		}
	}

	for (nlohmann::json children : json["Children"])
	{
		auto child = RunInitialisationRecursive(children, device, obj->Transform);
		obj->Transform->Children.emplace_back(child->Transform);
		_sceneGraph.emplace_back(std::move(child));
	}

	return obj;
}

void SceneGraph::Draw(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
{
	for (std::shared_ptr<GameObject> object : _sceneGraph)
	{
		auto meshComponent = object->TryGetComponent<MeshComponent>();
		if (!meshComponent.has_value())
			continue;

		meshComponent.value().lock()->Draw(context);
	}
}

void SceneGraph::Update(double deltaTime)
{
	for (std::shared_ptr<GameObject> gameObject : _sceneGraph)
		gameObject->Update(deltaTime);
}

void SceneGraph::FixedUpdate(double fixedDeltaTime)
{
	for (std::shared_ptr<GameObject> gameObject : _sceneGraph)
		gameObject->FixedUpdate(fixedDeltaTime);
}