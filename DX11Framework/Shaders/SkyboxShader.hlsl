TextureCube Skybox : register(t0);
SamplerState BilinearSampler : register(s0);

cbuffer SkyboxCBuffer : register(b0)
{
    float4x4 Projection;
    float4x4 View;
    float4x4 World;
}

struct SkyboxVSOut
{
    float4 Position : SV_POSITION;
    float3 TextureCoordinates : TEXCOORD;
};

SkyboxVSOut VS_main(
	float3 Position : POSITION,
	float3 NormalVector : NORMAL,
	float2 TexCoords : TEXCOORDS
)
{
    SkyboxVSOut output = (SkyboxVSOut) 0;
    float4 Pos4 = float4(Position, 1);
    output.Position = mul(Pos4, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    output.Position = output.Position.xyww;
    output.TextureCoordinates = Position;

    return output;
}

float4 PS_main(SkyboxVSOut input) : SV_TARGET
{
    return Skybox.Sample(BilinearSampler, input.TextureCoordinates);
}