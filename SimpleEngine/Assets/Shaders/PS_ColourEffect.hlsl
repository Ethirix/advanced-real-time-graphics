#include "Buffers/CB4_ExtraData.hlsli"

#include "Samplers/S0_BilinearSampler.hlsli"

#include "Structs/VS_ScreenQuadOut.hlsli"

#include "Structured Resources/T24_GenericTexture.hlsli"

float4 PS_Main(VS_ScreenQuadOut input) : SV_TARGET
{
	float4 baseColour = T24_GenericTexture.Sample(S0_BilinearSampler, input.TextureCoordinates);

	return float4(max(baseColour.rgb, (ColourEffect.rgb * ColourEffect.a)), 1);
}
