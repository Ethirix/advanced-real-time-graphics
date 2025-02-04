#include "Buffers/CB0_ObjectCameraData.hlsli"
#include "Samplers/S0_BilinearSampler.hlsli"
#include "Structs/VS_ScreenQuadOut.hlsli"
#include "Structured Resources/T24_FinalPass.hlsli"

float4 PS_Main(VS_ScreenQuadOut input) : SV_TARGET
{
    float depth = T24_FinalPass.Sample(S0_BilinearSampler, input.TextureCoordinates).r;
	float f = (2.0f * NearZ) / (FarZ + NearZ - depth * (FarZ - NearZ));

	return float4(f, f, f, 1);
}
