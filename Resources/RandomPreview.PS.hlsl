Texture2D<float4> gInputTexture : register(t0);
Texture2D<float4> gUnused1 : register(t1);
Texture2D<float4> gUnused2 : register(t2);
SamplerState gSampler : register(s0);

cbuffer PostProcessParams : register(b0)
{
    float gIntensity;
    float gRandomMode;
    float gRandomScale;
    float gRandomSpeed;
    float gRandomTime;
    float gAux5;
    float gAux6;
    float gAux7;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float Rand2dTo1d(float2 seed)
{
    seed = frac(seed * float2(123.34f, 456.21f));
    seed += dot(seed, seed + 45.32f);
    return frac(seed.x * seed.y);
}

float4 main(PSInput input) : SV_TARGET
{
    float2 uv = saturate(input.uv);
    float4 color = gInputTexture.Sample(gSampler, uv);

    float scale = max(gRandomScale, 1.0f);
    float timeSeed = floor(gRandomTime * max(gRandomSpeed, 0.0f));
    float random = Rand2dTo1d(floor(uv * scale) + float2(timeSeed, timeSeed));

    if (gRandomMode < 0.5f) {
        return float4(lerp(color.rgb, random.xxx, saturate(gIntensity)), color.a);
    }

    float3 noisyColor = color.rgb * random.xxx;
    return float4(lerp(color.rgb, noisyColor, saturate(gIntensity)), color.a);
}
