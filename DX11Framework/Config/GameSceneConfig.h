#pragma once
#include <DirectXMath.h>
#include <string>

#include "../MeshType.h"

struct BaseGameSceneConfig
{
	std::string Name;
};

struct RenderObjectInfo : public BaseGameSceneConfig
{
	std::string MeshPath;
	std::string TexturePath;
	MeshType MeshType;
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Rotation;
	DirectX::XMFLOAT3 Scale;
};

struct CameraInfo : public BaseGameSceneConfig
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 At;
	DirectX::XMFLOAT3 Up;
};

struct DirectionalLightInfo : public BaseGameSceneConfig
{
	DirectX::XMFLOAT3 LightPosition;
	DirectX::XMFLOAT3 DiffuseColor;
	float DiffusePower;
	DirectX::XMFLOAT3 SpecularColor;
	float SpecularPower;
	DirectX::XMFLOAT3 AmbientColor;
};

struct PointLightInfo : public DirectionalLightInfo
{
	float ConstantAttenuation;
	float LinearAttenuation;
	float QuadraticAttenuation;
	float LightRadius;
};

struct GameSceneConfig
{
	
};
