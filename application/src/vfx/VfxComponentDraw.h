#pragma once

#include <d3d12.h>
#include "../../EffectSystem.h"
#include "vfx/VfxRenderContext.h"

namespace vfx {
struct VfxRendererResourceSet;

struct ComponentDrawParams {
    float depthFadeSoftness = 0.02f;
    float distortionDepthAttenuation = 1.0f;
    float particleEdgeSoftness = 0.5f;
    float trailTailFade = 1.35f;
    float dissolveThreshold = 0.0f;
    float dissolveEdgeWidth = 0.05f;
    Vector3 dissolveEdgeColor = {1.0f, 0.42f, 0.12f};
    float dissolveEnabled = 0.0f;
    float dissolvePreviewFillEnabled = 0.0f;
    Vector3 dissolvePreviewFillColor = {0.0f, 1.0f, 0.0f};
};

ComponentDrawParams ResolveParticleDrawParams(
    const EffectParticleSettings* settings,
    const EffectParticleSettings* fallback,
    const ComponentDrawParams& defaults);

ComponentDrawParams ResolveTrailDrawParams(
    const EffectTrailSettings* settings,
    const ComponentDrawParams& defaults);

ComponentDrawParams ResolveDistortionDrawParams(
    const EffectDistortionSettings* settings,
    const ComponentDrawParams& defaults);

void DrawIndirectSpriteComponents(
    ID3D12GraphicsCommandList* commandList,
    const VfxRenderContext& context,
    ID3D12PipelineState* pipelineState,
    const ComponentDrawParams& drawParams,
    const VfxRendererResourceSet* rendererResources = nullptr);
} // namespace vfx
