#include "GameObjectManager.h"

#include "PhongConstantBuffer.h"

bool GameObjectManager::Remove(GO gameObject)
{
	for(const std::vector<std::shared_ptr<GameObject>> children = 
		    gameObject->GetChildren(); std::shared_ptr<GameObject> child : children)
	{
		if (!child)
			continue;

		if (!gameObject->GetParent())
		{
			child->SetParent(nullptr);
			RootObjects.emplace_back(child);
		}
		else
		{
			child->SetParent(gameObject->GetParent());
		}
	}

	std::erase_if(RootObjects, 
	              [gameObject](const GO& r)
	              {
		              return r == gameObject;
	              });
	if (gameObject->GetParent())
	{
		std::erase_if(gameObject->GetParent()->GetChildren(),
	              [gameObject](const std::shared_ptr<GameObject>& t)
	              {
		              return t == gameObject;
	              });
	}

	gameObject->Transform = nullptr;

	return true;
}

void GameObjectManager::Add(GO gameObject)
{
	RootObjects.emplace_back(gameObject);
}

void GameObjectManager::Draw(ComPtr<ID3D11DeviceContext> context, ComPtr<ID3D11Buffer> constantBuffer, 
	ComPtr<ID3D11Buffer>& lightBuffer,
	BlinnConstantBuffer constantBufferData, LightBuffer lightBufferData)
{
	if (RootObjects.size() < 0)
		return;

	for (auto it = RootObjects.begin(); it != RootObjects.end(); ++it)
	{
		RecursiveDraw(*it, context, constantBuffer, lightBuffer, constantBufferData, lightBufferData);
	}
}

void GameObjectManager::Draw(ComPtr<ID3D11DeviceContext> context, 
	ComPtr<ID3D11Buffer> constantBuffer,
	PhongConstantBuffer constantBufferData)
{
	if (RootObjects.size() < 0)
		return;

	for (auto it = RootObjects.begin(); it != RootObjects.end(); ++it)
	{
		RecursiveDraw(*it, context, constantBuffer, constantBufferData);
	}
}

void GameObjectManager::RecursiveDraw(GO obj, ComPtr<ID3D11DeviceContext> context, ComPtr<ID3D11Buffer> constantBuffer,
                                      ComPtr<ID3D11Buffer>& lightBuffer, BlinnConstantBuffer constantBufferData, 
LightBuffer lightBufferData)
{
	obj->Draw(context, constantBuffer, lightBuffer, constantBufferData, lightBufferData);
	if (obj->GetChildren().size() < 0)
		return;

	for (auto it = obj->GetChildren().begin(); it != obj->GetChildren().end(); ++it)
	{
		RecursiveDraw(*it, context, constantBuffer, lightBuffer, constantBufferData, lightBufferData);
	}
}

void GameObjectManager::RecursiveDraw(GO obj, 
	ComPtr<ID3D11DeviceContext> context, 
	ComPtr<ID3D11Buffer> constantBuffer,
	PhongConstantBuffer constantBufferData)
{
	obj->Draw(context, constantBuffer, constantBufferData);
	if (obj->GetChildren().size() < 0)
		return;

	for (auto it = obj->GetChildren().begin(); it != obj->GetChildren().end(); ++it)
	{
		RecursiveDraw(*it, context, constantBuffer, constantBufferData);
	}
}

void GameObjectManager::Update(double deltaTime)
{
	if (RootObjects.size() < 0)
		return;

	for (const auto obj : RootObjects)
	{
		RecursiveUpdate(obj, deltaTime);
	}
}

void GameObjectManager::RecursiveUpdate(GO obj, double deltaTime)
{
	obj->Update(deltaTime);
	if (obj->GetChildren().size() < 0)
		return;

	for (auto it = obj->GetChildren().begin(); it != obj->GetChildren().end(); ++it)
	{
		RecursiveUpdate(*it, deltaTime);
	}
}
