#include "Buffers/CB4_ExtraData.hlsli"

#include "Samplers/S0_BilinearSampler.hlsli"

#include "Structs/VS_ScreenQuadOut.hlsli"

#include "Structured Resources/T24_GenericTexture.hlsli"

float3 CalculatePixel(float3 target, float3 blend, uint mode)
{
	 switch (mode)
	{
	case 1:
		return min(target.rgb, blend.rgb);
	case 2:
		return target * blend;
	case 3:
		return 1 - (1 - target) / blend;
	case 4:
		return target + blend - 1;
	case 5:
		return max(target, blend);
	case 6:
		return 1 - (1 - target) * (1 - blend);
	case 7:
		return target / (1- blend);
	case 8:
		return target + blend;
	case 9:
		return abs(target - blend);
	case 10:
		return 0.5f - 2 * (target - 0.5f) * (blend * 0.5f);
	default:
		return target;
    }
}

float4 PS_Main(VS_ScreenQuadOut input) : SV_TARGET
{
	float4 baseColour = T24_GenericTexture.Sample(S0_BilinearSampler, input.TextureCoordinates);

    return float4(CalculatePixel(baseColour, ColourEffect, ColourMode), 1);
}
