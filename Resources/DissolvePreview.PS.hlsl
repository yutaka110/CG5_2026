Texture2D<float4> gInputTexture : register(t0);
Texture2D<float4> gUnused1 : register(t1);
Texture2D<float4> gUnused2 : register(t2);
SamplerState gSampler : register(s0);

cbuffer PostProcessParams : register(b0)
{
    float gIntensity;
    float gThreshold;
    float gEdgeWidth;
    float gNoiseScale;
    float gFillColorR;
    float gFillColorG;
    float gFillColorB;
    float gEdgeColorR;
    float gEdgeColorG;
    float gEdgeColorB;
    float gPlaneScaleX;
    float gPlaneScaleY;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float Hash21(float2 p)
{
    p = frac(p * float2(123.34f, 456.21f));
    p += dot(p, p + 45.32f);
    return frac(p.x * p.y);
}

float ValueNoise(float2 uv)
{
    float2 cell = floor(uv);
    float2 local = frac(uv);
    float2 curve = local * local * (3.0f - 2.0f * local);

    float a = Hash21(cell);
    float b = Hash21(cell + float2(1.0f, 0.0f));
    float c = Hash21(cell + float2(0.0f, 1.0f));
    float d = Hash21(cell + float2(1.0f, 1.0f));

    return lerp(lerp(a, b, curve.x), lerp(c, d, curve.x), curve.y);
}

float Fbm(float2 uv)
{
    float value = 0.0f;
    float amplitude = 0.5f;
    for (int i = 0; i < 5; ++i) {
        value += ValueNoise(uv) * amplitude;
        uv = uv * 2.03f + 17.13f;
        amplitude *= 0.5f;
    }
    return saturate(value);
}

float3 PreviewBaseColor(float2 localUv)
{
    float terrain = Fbm(localUv * 5.0f + float2(2.7f, 8.1f));
    float ridge = Fbm(localUv * 16.0f + float2(0.4f, 3.9f));
    float3 grass = float3(0.28f, 0.42f, 0.22f);
    float3 rock = float3(0.66f, 0.62f, 0.50f);
    float3 dirt = float3(0.42f, 0.34f, 0.24f);
    float3 color = lerp(grass, rock, smoothstep(0.35f, 0.9f, terrain));
    color = lerp(color, dirt, smoothstep(0.72f, 0.95f, ridge) * 0.45f);
    color *= 0.85f + terrain * 0.35f;
    return saturate(color);
}

float4 main(PSInput input) : SV_TARGET
{
    float2 uv = saturate(input.uv);
    float4 scene = gInputTexture.Sample(gSampler, uv);

    float2 planeScale = clamp(float2(gPlaneScaleX, gPlaneScaleY), float2(0.05f, 0.05f), float2(1.0f, 1.0f));
    float2 planeMin = float2(0.5f, 0.5f) - planeScale * 0.5f;
    float2 planeMax = float2(0.5f, 0.5f) + planeScale * 0.5f;
    if (uv.x < planeMin.x || uv.x > planeMax.x || uv.y < planeMin.y || uv.y > planeMax.y) {
        return scene;
    }

    float2 localUv = saturate((uv - planeMin) / planeScale);
    float threshold = saturate(gThreshold);
    float edgeWidth = max(gEdgeWidth, 0.0001f);
    float noiseScale = max(gNoiseScale, 0.5f);
    float mask = Fbm(localUv * noiseScale + float2(5.2f, 1.3f));
    float dissolveEdge = 1.0f - smoothstep(threshold, threshold + edgeWidth, mask);
    float dissolved = mask < threshold ? 1.0f : 0.0f;

    float3 baseColor = PreviewBaseColor(localUv);
    float3 fillColor = float3(gFillColorR, gFillColorG, gFillColorB);
    float3 edgeColor = float3(gEdgeColorR, gEdgeColorG, gEdgeColorB);
    float3 preview = lerp(baseColor, fillColor, dissolved);
    preview = lerp(preview, edgeColor, dissolveEdge);

    float border =
        step(localUv.x, 0.01f) + step(localUv.y, 0.01f) +
        step(0.99f, localUv.x) + step(0.99f, localUv.y);
    preview = lerp(preview, float3(0.0f, 0.0f, 0.0f), saturate(border));

    return float4(lerp(scene.rgb, preview, saturate(gIntensity)), 1.0f);
}
