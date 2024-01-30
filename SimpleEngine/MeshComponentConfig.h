#pragma once
#include <string>

#include "MeshType.h"

//PATHS ARE RELATIVE

struct MeshComponentConfig
{
	std::string MeshPath;
	std::string MaterialPath;
	std::string VertexShaderPath;
	std::string PixelShaderPath;
	MeshType MeshType;
};
