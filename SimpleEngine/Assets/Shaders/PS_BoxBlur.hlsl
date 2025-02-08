#include "Samplers/S0_BilinearSampler.hlsli"
#include "Structs/VS_ScreenQuadOut.hlsli"
#include "Structured Resources/T24_GenericTexture.hlsli"

float4 PS_Main(VS_ScreenQuadOut input) : SV_TARGET
{
    float2 pixelOffset = float2(input.TextureCoordinates / input.Position.xy);
    float4 blur = T24_GenericTexture.Sample(S0_BilinearSampler, input.TextureCoordinates 
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
