#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

#define WP_GAMEOBJECT const std::weak_ptr<::GameObject>&
#define SP_GAMEOBJECT const std::shared_ptr<::GameObject>&

class GameObject;
class ComponentBase
{
public:
	explicit ComponentBase(WP_GAMEOBJECT owningGameObject)
	: GameObject(owningGameObject) { }

	virtual void Update(double deltaTime) = 0;
	virtual void FixedUpdate(double fixedDeltaTime) = 0;
	virtual void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) = 0;

	std::weak_ptr<GameObject> GameObject = {};
};
