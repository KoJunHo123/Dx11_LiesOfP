#include "Shader_Engine_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_CascadeViewMatrix[3], g_CascadeProjMatrix[3];

float g_fFar;
float4 g_fHashColor;

texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_ARMTexture;
texture2D g_EmessiveTexture;

float g_fEmessiveMask;

struct VS_IN_MODEL_INSTANCE
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    
	/* InputSlot : 1 */
    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook : TEXCOORD3;
    float4 vTranslation : TEXCOORD4;
    float4 vHashColor : TEXCOORD5;
};

struct VS_OUT_MODEL
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float4 vHashColor : TEXCOORD2;
};

VS_OUT_MODEL VS_MAIN(VS_IN_MODEL_INSTANCE In)
{
    VS_OUT_MODEL Out = (VS_OUT_MODEL) 0;
	
    matrix WorldMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    matrix matWV, matWVP;
	
    matWV = mul(WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    Out.vHashColor = In.vHashColor;
    
    return Out;
}

struct VS_OUT_NORMAL
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float4 vHashColor : TEXCOORD2;
};

VS_OUT_NORMAL VS_MAIN_NORMAL(VS_IN_MODEL_INSTANCE In)
{
    VS_OUT_NORMAL Out = (VS_OUT_NORMAL) 0;

    matrix matWV, matWVP;

    matrix WorldMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    matWV = mul(WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), WorldMatrix)).xyz;
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), WorldMatrix)).xyz;
    Out.vBinormal = normalize(cross(Out.vNormal, Out.vTangent));
    Out.vHashColor = In.vHashColor;

    return Out;
}

struct GS_IN_CASCADE
{
    float4 vPosition : SV_POSITION;
};

GS_IN_CASCADE VS_MAIN_CASCADE(VS_IN_MODEL_INSTANCE In)
{
    GS_IN_CASCADE Out = (GS_IN_CASCADE) 0;
    
    matrix WorldMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), WorldMatrix);
    return Out;
}

struct GS_OUT_CASCADE
{
    float4 vPosition : SV_POSITION;
    uint iRTVIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(9)]
void GS_MAIN(triangle GS_IN_CASCADE In[3], inout TriangleStream<GS_OUT_CASCADE> Container)
{
    GS_OUT_CASCADE Out[3];
    
    // 각 렌더타겟 별로 찍어두기
    for (uint i = 0; i < 3; ++i)
    {
        for (uint j = 0; j < 3; ++j)
        {
            // 뷰상의 위치
            float4 vViewPos = mul(In[j].vPosition, g_CascadeViewMatrix[i]);
            Out[j].vPosition = mul(vViewPos, g_CascadeProjMatrix[i]);
            Out[j].iRTVIndex = i;
            Container.Append(Out[j]);
        }
        Container.RestartStrip();
    }
}

struct PS_IN_MODEL
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float4 vHashColor : TEXCOORD2;
};

struct PS_OUT_MODEL
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vARM : SV_TARGET3;
    vector vEmessive : SV_TARGET4;
    vector vRimLight : SV_TARGET5;
    vector vPickDepth : SV_TARGET6;
    vector vPickObjectDepth : SV_TARGET7;
};

PS_OUT_MODEL PS_MAIN(PS_IN_MODEL In)
{
    PS_OUT_MODEL Out = (PS_OUT_MODEL) 0;
	
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
	
    if (0.3f >= vDiffuse.a)
        discard;

    vector vEmissive = g_EmessiveTexture.Sample(LinearClampSampler, In.vTexcoord) * g_fEmessiveMask;
	
    Out.vDiffuse = vDiffuse + vEmissive;

	/* -1.f ~ 1.f -> 0.f ~ 1.f */
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
    Out.vARM = g_ARMTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vEmessive = vEmissive;
    Out.vRimLight = vector(0.f, 0.f, 0.f, 0.f);
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, 0.f, 0.f, 1.f);
    Out.vPickObjectDepth = In.vHashColor;;
    
    return Out;
}

struct PS_IN_NORMAL
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float4 vHashColor : TEXCOORD2;
};

PS_OUT_MODEL PS_MAIN_NORMAL(PS_IN_NORMAL In)
{
    PS_OUT_MODEL Out = (PS_OUT_MODEL) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
	/* 로컬상의 변환되지 않은 노말벡터를 구했다. */
	/* 로컬스페이스 => 정점의로컬스페이스(x), 노멀벡터당 하나씩 로컬스페이스를 독립적으로 구성했다. */
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

    vNormal = normalize(mul(vNormal, WorldMatrix));

    if (0.3f >= vDiffuse.a)
        discard;

    vector vEmissive = g_EmessiveTexture.Sample(LinearClampSampler, In.vTexcoord) * g_fEmessiveMask;
	
    Out.vDiffuse = vDiffuse + vEmissive;
    
	/* -1.f ~ 1.f -> 0.f ~ 1.f */
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFar, 0.f, 0.f);
    Out.vARM = g_ARMTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vEmessive = vEmissive;
    Out.vRimLight = vector(0.f, 0.f, 0.f, 0.f);
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, 0.f, 0.f, 1.f);
    Out.vPickObjectDepth = In.vHashColor;;
    
    return Out;
}

struct PS_IN_CASCADE
{
    float4 vPosition : SV_POSITION;
    uint iRTVIndex : SV_RenderTargetArrayIndex; // 렌더타겟 배열 중에 어떤 것에 그릴거야?
};

float4 PS_MAIN_CASCADE(PS_IN_CASCADE In) : SV_Target0
{
    return float4(In.vPosition.z, 0.f, 0.f, 1.f);
}

technique11 DefaultTechnique
{
    pass NonAnimModel
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass NormalMapping
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_NORMAL();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NORMAL();
    }

    pass Cascade
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_CASCADE();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_CASCADE();
    }
}