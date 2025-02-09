#include "Samplers/S0_BilinearSampler.hlsli"

#include "Structs/VS_ScreenQuadOut.hlsli"

#include "Structured Resources/T24_GenericTexture.hlsli"

float4 PS_Main(VS_ScreenQuadOut input) : SV_TARGET
{
	return float4(T24_GenericTexture.Sample(S0_BilinearSampler, input.TextureCoordinates));
}
