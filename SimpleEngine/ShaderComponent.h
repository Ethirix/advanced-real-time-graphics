#pragma once
#include "ComponentBase.h"
#include "ShaderData.h"

class ShaderComponent : public ComponentBase
{
	//~ShaderComponent() override {}
public:
	ShaderData<ID3D11PixelShader> PixelShader = {nullptr, Pixel, 0 };
	ShaderData<ID3D11VertexShader> VertexShader = {nullptr, Vertex, 0 };

private:
	ShaderData<ID3D11ComputeShader> _computeShader = {nullptr, Compute, 0 };
	ShaderData<ID3D11DomainShader> _domainShader = {nullptr, Domain, 0 };
	ShaderData<ID3D11GeometryShader> _geometryShader = {nullptr, Geometry, 0 };
	ShaderData<ID3D11HullShader> _hullShader = {nullptr, Hull, 0 };
};
