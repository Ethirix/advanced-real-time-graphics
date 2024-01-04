#pragma once
#include <d3d11.h>
#include <memory>
#include <vector>
#include <wrl/client.h>

#include "BlinnConstantBuffer.h"
#include "LightBuffer.h"
#include "PhongConstantBuffer.h"
#include "Transform.h"

#define SP_GAMEOBJECT std::shared_ptr<GameObject>
#define WP_GAMEOBJECT std::weak_ptr<GameObject>

class GameObject : public std::enable_shared_from_this<GameObject>
{
	friend class GameObjectFactory;
public:
	SP_GAMEOBJECT GetPtr() { return shared_from_this(); }

	virtual void Draw(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, 
		const Microsoft::WRL::ComPtr<ID3D11Buffer>& constantBuffer,
		const Microsoft::WRL::ComPtr<ID3D11Buffer>& lightBuffer, 
		BlinnConstantBuffer constantBufferData, LightBuffer lightBufferData) {}
	virtual void Draw(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
		const Microsoft::WRL::ComPtr<ID3D11Buffer>& constantBuffer,
		PhongConstantBuffer constantBufferData) {}

	virtual void Update(double deltaTime) {}

	void SetParent(const SP_GAMEOBJECT& parent);
	void AddChild(SP_GAMEOBJECT child);

	[[nodiscard]] std::vector<SP_GAMEOBJECT>& GetChildren();
	[[nodiscard]] SP_GAMEOBJECT GetParent();

	std::shared_ptr<Transform> Transform = std::make_shared<::Transform>();

protected:
	virtual ~GameObject() = default;

	SP_GAMEOBJECT _parent = {};
	std::vector<SP_GAMEOBJECT> _children = {};
};

