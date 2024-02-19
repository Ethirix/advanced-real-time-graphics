#include "SceneGraph.h"
#include <fstream>

#include "AABBColliderComponent.h"
#include "CameraComponent.h"
#include "LightComponent.h"
#include "MeshComponent.h"
#include "PhysicsComponent.h"
#include "PlaneColliderComponent.h"
#include "SphereColliderComponent.h"

void SceneGraph::Initialize(const std::string& path, const Microsoft::WRL::ComPtr<ID3D11Device>& device)
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
			auto meshComponent = std::make_shared<MeshComponent>(
				obj, component, device);

			obj->AddComponent(meshComponent);
		}
		else if (type == "PhysicsComponent")
		{
			auto physicsComponent = std::make_shared<PhysicsComponent>(
				obj, component);
			obj->AddComponent(physicsComponent);
		}
		else if (type == "CameraComponent")
		{
			auto cameraComponent = std::make_shared<CameraComponent>(
				obj, component);

			obj->AddComponent(cameraComponent);
		}
		else if (type == "LightComponent")
		{
			auto lightComponent = std::make_shared<LightComponent>(
				obj, component);

			obj->AddComponent(lightComponent);
		}
		else if (type == "SphereColliderComponent")
		{
			auto sphereColliderComponent = std::make_shared<SphereColliderComponent>(
				obj, component);

			obj->AddComponent(sphereColliderComponent);
		}
		else if (type == "AABBColliderComponent")
		{
			auto aabbColliderComponent = std::make_shared<AABBColliderComponent>(
				obj, component);

			obj->AddComponent(aabbColliderComponent);
		}
		else if (type == "PlaneColliderComponent")
		{
			auto planeColliderComponent = std::make_shared<PlaneColliderComponent>(
				obj, component);

			obj->AddComponent(planeColliderComponent);
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

GameObject* SceneGraph::GetObjectAtPosition(unsigned index)
{
	return _sceneGraph[index].get();
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

std::list<CollisionResponse> SceneGraph::CheckColliders(const std::shared_ptr<ColliderComponent>& collider)
{
	std::list<CollisionResponse> responses = {};
	switch (collider->Type)
	{
	case COLLIDER_SPHERE:
		{
			std::shared_ptr<SphereColliderComponent> sphere =
			std::dynamic_pointer_cast<SphereColliderComponent>(collider);
		
			for (std::weak_ptr<ColliderComponent> coll: GetComponentsFromObjects<ColliderComponent>())
			{
				if (coll.lock() == collider)
					continue;
				if (CollisionResponse response = sphere->CollidesWith(coll.lock()); 
					response.Collider)
					responses.emplace_back(response);
			}
		}
		break;
	case COLLIDER_AABB:
		{
			std::shared_ptr<AABBColliderComponent> aabb =
				std::dynamic_pointer_cast<AABBColliderComponent>(collider);

			for (std::weak_ptr<ColliderComponent> coll : GetComponentsFromObjects<ColliderComponent>())
			{
				if (coll.lock() == collider)
					continue;

				if (CollisionResponse response = aabb->CollidesWith(coll.lock()); 
					response.Collider)
					responses.emplace_back(response);
			}
		}
		break;
		case COLLIDER_PLANE:
		{
			std::shared_ptr<PlaneColliderComponent> plane =
				std::dynamic_pointer_cast<PlaneColliderComponent>(collider);

			for (std::weak_ptr<ColliderComponent> coll : GetComponentsFromObjects<ColliderComponent>())
			{
				if (coll.lock() == collider)
					continue;

				if (CollisionResponse response = plane->CollidesWith(coll.lock()); 
					response.Collider)
					responses.emplace_back(response);
			}
		}
		break;
	default:
		break;
	}

	return responses;
}
