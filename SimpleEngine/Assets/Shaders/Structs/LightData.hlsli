struct LightData
{
    float4 Position;
    float3 DiffuseColor;
    float DiffusePower;
    float3 SpecularColor;
    float SpecularPower;
    float3 AmbientColor;
    float ConstantAttenuation;
    float LinearAttenuation;
    float QuadraticAttenuation;
    float LightRadius;
    float ___LDPad0;
};
