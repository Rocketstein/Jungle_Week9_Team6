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
    float intensity = saturate(max(HitEffectIntensity, FadeAmount));
    
    if (intensity <= 0.0f)
    {
        return color;
    }
    

    float2 dist = (input.uv - 0.5f) * 2.0f;
    float vignette = length(dist);
    
    // Apply exponential curve for sharp edges and multiply by intensity
    float hitEffect = pow(saturate(vignette * 0.8f), 4.0) * intensity;
    
    // Blend with camera fade color. 
    float3 hitColor = FadeColor.a > 0.0f ? FadeColor.rgb : float3(0.8f, 0.0f, 0.0f);
    color.rgb = lerp(color.rgb, hitColor, hitEffect);
    
    // Subtle overall tint based on intensity
    color.rgb += hitColor * intensity * 0.05f;
    
    return color;
}
