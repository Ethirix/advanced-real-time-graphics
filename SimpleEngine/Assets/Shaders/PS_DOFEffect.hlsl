#include "Buffers/CB0_ObjectCameraData.hlsli"
#include "Buffers/CB4_ExtraData.hlsli"

#include "Samplers/S0_BilinearSampler.hlsli"

#include "Structs/VS_ScreenQuadOut.hlsli"

#include "Structured Resources/T24_GenericTexture.hlsli"
#include "Structured Resources/T25_GenericTexture.hlsli"
#include "Structured Resources/T26_DepthTexture.hlsli"

float4 PS_Main(VS_ScreenQuadOut input) : SV_TARGET
{
	float4 base = T24_GenericTexture.Sample(S0_BilinearSampler, input.TextureCoordinates);
	float4 blurred = T25_GenericTexture.Sample(S0_BilinearSampler, input.TextureCoordinates);
	float depth = T26_DepthTexture.Sample(S0_BilinearSampler, input.TextureCoordinates).r;
	depth = NearZ * FarZ / (FarZ + depth * (NearZ - FarZ));

	//if (depth > 10)
	//	return blurred;
	//else
	//	return base;

	float blurFactor = smoothstep(FocalDepth, FocalDepth + FocalBlendDistance, depth);

	return lerp(base, blurred, saturate(blurFactor));
}
