#include "Buffers/CB1_Material.hlsli"
#include "Functions/Lighting.hlsli"
#include "Structs/LightingOut.hlsli"
#include "Structs/VS_ScreenQuadOut.hlsli"

#include "Structured Resources/T17_DeferredNormalTexture.hlsli"
#include "Structured Resources/T18_DeferredDepthTexture.hlsli"

struct PSLightPassOut
{
    float4 Diffuse : SV_TARGET0;
    float4 Specular : SV_TARGET1;
};

PSLightPassOut PS_Main(VS_ScreenQuadOut input) : SV_TARGET
{
    PSLightPassOut output = (PSLightPassOut) 0;

    Material material = CreateMaterial(DiffuseMaterial, AmbientMaterial, SpecularMaterial, SpecularExponent);

    float4 normal = T17_DeferredNormalTexture.Sample(S0_BilinearSampler, input.TextureCoordinates);
    float depth = T18_DeferredDepthTexture.Sample(S0_BilinearSampler, input.TextureCoordinates).x;

    float4 pos = float4(input.TextureCoordinates.x * 2.0f - 1.0f, 
					   (1.0f - input.TextureCoordinates.y) * 2.0f - 1.0f,
					   depth, 1);

    //output.Diffuse = pos;
    //return output;

    pos = mul(pos, InverseViewProjection);
    pos = float4(pos.xyz / pos.w, 1);

    //output.Diffuse = pos;
    //return output;
	
    LightingOut lighting = CalculateLighting(pos, normal.rgb, material);

    output.Diffuse = float4(lighting.DiffuseOut, 1);
    output.Specular = float4(lighting.SpecularOut * normal.a, 1);

    return output;
}
