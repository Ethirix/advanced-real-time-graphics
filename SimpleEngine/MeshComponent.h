#pragma once
#include "ComponentBase.h"
#include "Mesh.h"

class MeshComponent final : public ComponentBase
{
public:
	MeshComponent();

	void FixedUpdate() override;
	void Update() override;

	std::shared_ptr<Mesh> Mesh;
};
