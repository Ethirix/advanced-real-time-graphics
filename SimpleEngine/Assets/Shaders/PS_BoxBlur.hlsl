#include "Buffers/CB4_ExtraData.hlsli"

#include "Samplers/S0_BilinearSampler.hlsli"
#include "Structs/VS_ScreenQuadOut.hlsli"
#include "Structured Resources/T24_GenericTexture.hlsli"

float4 PS_Main(VS_ScreenQuadOut input) : SV_TARGET
{
	float4 blur = 0;
	float2 pixelOffset = float2(input.TextureCoordinates / input.Position.xy) * max(BlurSampleSize, 1);

	if (input.TextureCoordinates.x + pixelOffset.x > 1 || input.TextureCoordinates.x - pixelOffset.x < 0)
		pixelOffset.x = 0;
	if (input.TextureCoordinates.y + pixelOffset.y > 1 || input.TextureCoordinates.y - pixelOffset.y < 0)
		pixelOffset.y = 0;

	blur += T24_GenericTexture.Sample(S0_BilinearSampler, input.TextureCoordinates
	                                        + float2(-pixelOffset.x, pixelOffset.y));
	blur += T24_GenericTexture.Sample(S0_BilinearSampler, input.TextureCoordinates
	                                  + float2(0, pixelOffset.y));
	blur += T24_GenericTexture.Sample(S0_BilinearSampler, input.TextureCoordinates
	                                  + float2(pixelOffset.x, pixelOffset.y));
	blur += T24_GenericTexture.Sample(S0_BilinearSampler, input.TextureCoordinates
	                                  + float2(pixelOffset.x, 0));
	blur += T24_GenericTexture.Sample(S0_BilinearSampler, input.TextureCoordinates
	                                  + float2(pixelOffset.x, -pixelOffset.y));
	blur += T24_GenericTexture.Sample(S0_BilinearSampler, input.TextureCoordinates
	                                  + float2(0, -pixelOffset.y));
	blur += T24_GenericTexture.Sample(S0_BilinearSampler, input.TextureCoordinates
	                                  + float2(-pixelOffset.x, -pixelOffset.y));
	blur += T24_GenericTexture.Sample(S0_BilinearSampler, input.TextureCoordinates
	                                  + float2(-pixelOffset.x, 0));
	blur += T24_GenericTexture.Sample(S0_BilinearSampler, input.TextureCoordinates);

	blur /= 9;
		
    return blur;
}
