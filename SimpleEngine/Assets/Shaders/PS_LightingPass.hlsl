#include "Functions/Lighting.hlsli"
#include "Structs/LightingOut.hlsli"
#include "Structs/VS_ScreenQuadOut.hlsli"

#include "Structured Resources/T16_DeferredAlbedoTexture.hlsli"
#include "Structured Resources/T17_DeferredNormalTexture.hlsli"
#include "Structured Resources/T18_DeferredDepthTexture.hlsli"
#include "Structured Resources/T19_DeferredWorldPositionTexture.hlsli"

struct PSLightPassOut
{
    float3 Diffuse : SV_TARGET0;
    float3 Specular : SV_TARGET1;
};

PSLightPassOut PS_Main(VS_ScreenQuadOut input) : SV_TARGET
{
    PSLightPassOut output = (PSLightPassOut) 0;

    float4 position = T19_DeferredWorldPositionTexture.Sample(S0_BilinearSampler, input.TextureCoordinates);
    float4 normal = T17_DeferredNormalTexture.Sample(S0_BilinearSampler, input.TextureCoordinates);
    float specularExponent = T17_DeferredNormalTexture.Sample(S0_BilinearSampler, input.TextureCoordinates).a;
	
    //float depth = T18_DeferredDepthTexture.Sample(S0_BilinearSampler, input.TextureCoordinates).x;

    //float4 pos = float4(input.TextureCoordinates.x * 2.0f - 1.0f, 
				//	   (1.0f - input.TextureCoordinates.y) * 2.0f - 1.0f,
				//	   depth, 1);

    //output.Diffuse = pos;
    //return output;

    //pos = mul(pos, InverseViewProjection);
    //pos = float4(pos.xyz / pos.w, 1);

    //output.Diffuse = pos;
    //return output;
	
    LightingOut lighting = CalculateLighting(position.xyz, normalize(normal.rgb), specularExponent);

    output.Diffuse = lighting.DiffuseOut;
    output.Specular = lighting.SpecularOut;

    return output;
}
