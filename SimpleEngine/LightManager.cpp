#include "LightManager.h"

#include "Buffers.h"
#include "DirectionalLightComponent.h"
#include "PointLightComponent.h"
#include "SceneGraph.h"
#include "SpotLightComponent.h"

void LightManager::UpdateLights(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	auto directionalLights = SceneGraph::GetComponentsFromObjects<DirectionalLightComponent>();
	Buffers::CBLighting.BufferData.TotalDirectionalLights = directionalLights.size();
	Buffers::CBLighting.BufferData.MaxDirectionalLights = MAX_DIRECTIONAL_LIGHTS;
	for (int i = 0; i < directionalLights.size() && i < MAX_DIRECTIONAL_LIGHTS; i++)
	{
		Buffers::SRVDirectionalLights.BufferData.DirectionalLights[i] = static_cast<DirectionalLightData>(directionalLights[i].lock());
	}

	auto pointLights = SceneGraph::GetComponentsFromObjects<PointLightComponent>();
	Buffers::CBLighting.BufferData.TotalPointLights = pointLights.size();
	Buffers::CBLighting.BufferData.MaxPointLights = MAX_POINT_LIGHTS;
	for (int i = 0; i < pointLights.size() && i < MAX_POINT_LIGHTS; i++)
	{
		Buffers::SRVPointLights.BufferData.PointLights[i] = static_cast<PointLightData>(pointLights[i].lock());
	}

	auto spotLights = SceneGraph::GetComponentsFromObjects<SpotLightComponent>();
	Buffers::CBLighting.BufferData.TotalSpotLights = spotLights.size();
	Buffers::CBLighting.BufferData.MaxSpotLights = MAX_SPOT_LIGHTS;
	for (int i = 0; i < spotLights.size() && i < MAX_SPOT_LIGHTS; i++)
	{
		Buffers::SRVSpotLights.BufferData.SpotLights[i] = static_cast<SpotLightData>(spotLights[i].lock());
	}

	D3D11_MAPPED_SUBRESOURCE lightingData, directionalLightSRVData, pointLightSRVData, spotLightSRVData;
	context->Map(Buffers::CBLighting.Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &lightingData);
	memcpy(lightingData.pData, &Buffers::CBLighting.BufferData, sizeof(Buffers::CBLighting.BufferData));
	context->Unmap(Buffers::CBLighting.Buffer.Get(), 0);

	context->Map(Buffers::SRVDirectionalLights.Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &directionalLightSRVData);
	memcpy(directionalLightSRVData.pData, &Buffers::SRVDirectionalLights.BufferData, sizeof(Buffers::SRVDirectionalLights.BufferData));
	context->Unmap(Buffers::SRVDirectionalLights.Buffer.Get(), 0);

	context->Map(Buffers::SRVPointLights.Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &pointLightSRVData);
	memcpy(pointLightSRVData.pData, &Buffers::SRVPointLights.BufferData, sizeof(Buffers::SRVPointLights.BufferData));
	context->Unmap(Buffers::SRVPointLights.Buffer.Get(), 0);

	context->Map(Buffers::SRVSpotLights.Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &spotLightSRVData);
	memcpy(spotLightSRVData.pData, &Buffers::SRVSpotLights.BufferData, sizeof(Buffers::SRVSpotLights.BufferData));
	context->Unmap(Buffers::SRVSpotLights.Buffer.Get(), 0);
}
