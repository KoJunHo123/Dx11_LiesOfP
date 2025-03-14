
texture2D g_InputBackTexture;
texture2D g_InputBloomTexture;
RWTexture2D<float4> g_OutputBackTexture;
StructuredBuffer<float> g_HDR : register(t1);

uint2 g_vRes;

// 요즘엔 이렇게 한다~
sampler Sampler_Default = sampler_state
{
    Filter = min_mag_mip_linear;
};

cbuffer ToneMapping
{
    float fMiddleGrey;
    float fLumWhiteSqr;
};

static const float3 LUM_FACTOR = float3(0.2126, 0.7152, 0.0722);

float3 ToneMapping(float3 vHDRColor)
{
    float fLScale = dot(vHDRColor, LUM_FACTOR);
    fLScale *= fMiddleGrey / g_HDR[0];
    fLScale = (fLScale + fLScale * fLScale / fLumWhiteSqr) / (1.f + fLScale);
    fLScale = clamp(fLScale, 0.5f, 1.1f);
    
    return vHDRColor * fLScale;
}

[numthreads(32, 32, 1)]
void CS_TONEMAPPING(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);
    
    float4 vBack = g_InputBackTexture.Load(int3(vPixel, 0));
    vBack = vector(ToneMapping(vBack.xyz),1.f);
    g_OutputBackTexture[vPixel] = vBack;
}

[numthreads(32, 32, 1)]
void CS_BLOOMADD(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);
    float2 vUV = float2(vPixel.x / g_vRes.x, vPixel.y / g_vRes.y);

    float4 vBack = g_InputBackTexture.Load(int3(vPixel, 0));
    float4 vBloom = g_InputBloomTexture.SampleLevel(Sampler_Default, vUV, 0);
    g_OutputBackTexture[vPixel] = vBack + vBloom;
}

[numthreads(32, 32, 1)]
void CS_COPYBAACKBUFFER(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint2 vPixel = uint2(dispatchThreadID.x, dispatchThreadID.y);
    float2 vUV = float2(vPixel.x / g_vRes.x, vPixel.y / g_vRes.y);
    
    float4 vBack = g_InputBackTexture.Load(int3(vPixel, 0));
    g_OutputBackTexture[vPixel] = vBack ;
}

technique11 DefaultTechnique
{
    pass ToneMapping
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_TONEMAPPING();
    }

    pass BloomAdd
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_BLOOMADD();
    }

    pass CopyBackBuffer
    {
        VertexShader = NULL;
        GeometryShader = NULL;
        PixelShader = NULL;
        ComputeShader = compile cs_5_0 CS_COPYBAACKBUFFER();
    }
}
