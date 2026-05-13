#pragma once

#include <cstdint>
#include <functional>

struct AppRuntimeState;
class EffectRuntime;
struct Vector3;

struct VfxRuntimeControlsPanelInput {
    AppRuntimeState* runtimeState = nullptr;
    EffectRuntime* effectRuntime = nullptr;
    uint32_t* trailMeshStreamStartupTelemetryFrames = nullptr;
    std::function<void(float centerX, float centerY, float intensity, float durationSeconds)> onRadialBlurEvent;
    std::function<void(const Vector3& worldPosition, float intensity, float durationSeconds)> onRadialBlurWorldEvent;
};

void DrawSceneLightingControlsPanel(
    AppRuntimeState& runtimeState);

void DrawMaterialSettingsPanel(
    AppRuntimeState& runtimeState,
    const std::function<void()>& onAddParticle);

void DrawMaterialSettingsControlsPanel(
    AppRuntimeState& runtimeState,
    const std::function<void()>& onAddParticle);

void DrawVfxRuntimeControlsPanel(
    const VfxRuntimeControlsPanelInput& input);
