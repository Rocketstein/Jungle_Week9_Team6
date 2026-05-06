#include "Common/Functions.hlsli"
#include "Common/ConstantBuffers.hlsli"
#include "Common/SystemResources.hlsli"
#include "Common/SystemSamplers.hlsli"


#define SceneColor SceneColorTexture
#define Sampler LinearClampSampler

cbuffer PostProcessMaterialBuffer : register(b2)
{
    float HitEffectIntensity;
    float3 _Pad0;
    float4 FadeColor;
    float FadeAmount;
    float3 _Pad1;
}

PS_Input_UV VS(uint vertexID : SV_VertexID)
{
    return FullscreenTriangleVS(vertexID);
}

float4 PS(PS_Input_UV input) : SV_TARGET
{
    float4 color = SceneColor.Sample(Sampler, input.uv);
    float hitIntensity = saturate(HitEffectIntensity);
    float fadeAmount = saturate(FadeAmount);
    float3 fadeColor = FadeColor.a > 0.0f ? FadeColor.rgb : float3(0.8f, 0.0f, 0.0f);
    
    if (hitIntensity > 0.0f)
    {
        float2 dist = (input.uv - 0.5f) * 2.0f;
        float vignette = length(dist);

        float hitEffect = pow(saturate(vignette * 0.8f), 4.0f) * hitIntensity;
        color.rgb = lerp(color.rgb, fadeColor, hitEffect);
        color.rgb += fadeColor * hitIntensity * 0.05f;
    }

    if (fadeAmount > 0.0f)
    {
        color.rgb = lerp(color.rgb, fadeColor, fadeAmount);
    }
    
    return color;
}
