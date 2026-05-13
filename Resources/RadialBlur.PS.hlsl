Texture2D<float4> gInputTexture : register(t0);
Texture2D<float4> gUnused1 : register(t1);
Texture2D<float4> gUnused2 : register(t2);
SamplerState gSampler : register(s0);

cbuffer PostProcessParams : register(b0)
{
    float gIntensity;
    float gCenterX;
    float gCenterY;
    float gBlurWidth;
    float gSampleCount;
    float gAux5;
    float gAux6;
    float gAux7;
    float gAux8;
    float gAux9;
    float gAux10;
    float gAux11;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET
{
    float2 uv = saturate(input.uv);
    float2 center = saturate(float2(gCenterX, gCenterY));
    float2 direction = uv - center;

    int sampleCount = clamp((int)round(gSampleCount), 2, 32);
    float blurWidth = max(gBlurWidth, 0.0f) * saturate(gIntensity);
    float3 sum = 0.0f.xxx;
    float totalWeight = 0.0f;

    [loop]
    for (int sampleIndex = 0; sampleIndex < sampleCount; ++sampleIndex)
    {
        float t = sampleCount <= 1 ? 0.0f : (float)sampleIndex / (float)(sampleCount - 1);
        float sampleOffset = (t - 0.5f) * blurWidth;
        float2 sampleUv = saturate(uv + direction * sampleOffset);
        float weight = 1.0f - abs(t - 0.5f);
        sum += gInputTexture.Sample(gSampler, sampleUv).rgb * weight;
        totalWeight += weight;
    }

    float4 source = gInputTexture.Sample(gSampler, uv);
    float3 blurred = sum * rcp(max(totalWeight, 0.0001f));
    return float4(lerp(source.rgb, blurred, saturate(gIntensity)), source.a);
}
