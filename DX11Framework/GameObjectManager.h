#pragma once

#include <memory>

#include "BlinnConstantBuffer.h"
#include "RenderObject.h"

#define GO std::shared_ptr<GameObject>
#define TRANSFORM std::shared_ptr<Transform>

class GameObjectManager
{
public:
	std::list<GO> RootObjects;

	void Add(GO gameObject);
	bool Remove(GO gameObject);

	void Draw(ComPtr<ID3D11DeviceContext> context, ComPtr<ID3D11Buffer> constantBuffer,
		ComPtr<ID3D11Buffer>& lightBuffer,
		BlinnConstantBuffer constantBufferData, LightBuffer lightBufferData);

	void Draw(ComPtr<ID3D11DeviceContext> context,
		ComPtr<ID3D11Buffer> constantBuffer,
		PhongConstantBuffer constantBufferData);
	

	void Update(double deltaTime);

	template<typename T>
	std::optional<std::shared_ptr<T>> FindFirstObjectOfType(bool recursive = false, GO parent = nullptr)
	{
		if (!recursive)
		{
			if (!parent)
			{
				for (GO rootObject : RootObjects)
					if (std::shared_ptr<T> obj = std::dynamic_pointer_cast<T>(rootObject))
						return obj;
			}
			else
			{
				for (GO childGameObject : parent->GetChildren())
					if (std::shared_ptr<T> obj = std::dynamic_pointer_cast<T>(childGameObject))
						return obj;
			}
			return {};
		}
		//else
		if (!parent)
		{
			for (GO rootObject : RootObjects)
			{
				std::shared_ptr<T> obj = std::dynamic_pointer_cast<T>(rootObject);
				if (obj)
					return obj;

				if (auto recObj = FindFirstObjectOfType<T>(true, rootObject); recObj.has_value())
					return recObj;
			}
		}
		else
		{
			for (GO childGameObject : parent->GetChildren())
			{
				std::shared_ptr<T> obj = std::dynamic_pointer_cast<T>(childGameObject);
				if (obj)
					return obj;

				
				if (auto recObj = FindFirstObjectOfType<T>(true, childGameObject); recObj.has_value())
					return recObj;
			}
		}

		return {};
	}

	template<typename T>
	std::vector<std::shared_ptr<T>> FindObjectsOfType(bool recursive = false, GO parent = nullptr)
	{
		std::vector<std::shared_ptr<T>> objects = {};
		if (!recursive)
		{
			if (!parent)
			{
				for (GO rootObject : RootObjects)
					if (std::shared_ptr<T> obj = std::dynamic_pointer_cast<T>(rootObject))
						objects.emplace_back(obj);
			}
			else
			{
				for (GO childGameObject : parent->GetChildren())
					if (std::shared_ptr<T> obj = std::dynamic_pointer_cast<T>(childGameObject))
						objects.emplace_back(obj);
			}
			return objects;
		}
		//else
		if (!parent)
		{
			for (GO rootObject : RootObjects)
			{
				std::shared_ptr<T> obj = std::dynamic_pointer_cast<T>(rootObject);
				if (obj)
					objects.emplace_back(obj);

				auto recObj = FindObjectsOfType<T>(true, rootObject);
				objects.insert(objects.end(), recObj.begin(), recObj.end());
			}
		}
		else
		{
			for (GO childGameObject : parent->GetChildren())
			{
				std::shared_ptr<T> obj = std::dynamic_pointer_cast<T>(childGameObject);
				if (obj)
					objects.emplace_back(obj);

				
				auto recObj = FindObjectsOfType<T>(true, childGameObject);
				objects.insert(objects.end(), recObj.begin(), recObj.end());
			}
		}

		return objects;
	}

private:
	void RecursiveDraw(GO obj, ComPtr<ID3D11DeviceContext> context, ComPtr<ID3D11Buffer> constantBuffer,
		ComPtr<ID3D11Buffer>& lightBuffer,
		BlinnConstantBuffer constantBufferData, LightBuffer lightBufferData);

	void RecursiveDraw(GO obj,
		ComPtr<ID3D11DeviceContext> context,
		ComPtr<ID3D11Buffer> constantBuffer,
		PhongConstantBuffer constantBufferData);

	void RecursiveUpdate(GO obj, double deltaTime);
};

