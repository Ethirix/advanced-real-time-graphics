#pragma once
#include "ComponentBase.h"
#include "Mesh.h"

#define PATH const std::string&

class MeshComponent final : public ComponentBase
{
public:
	MeshComponent(PATH meshPath, PATH materialPath, PATH vertexShaderPath, PATH pixelShaderPath);

	void FixedUpdate() override;
	void Update() override;

	std::shared_ptr<Mesh> Mesh;
};
