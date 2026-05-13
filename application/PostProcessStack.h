#pragma once

#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

struct PostProcessPass {
    struct Parameters {
        float bloomThresholdMin = 0.45f;
        float bloomThresholdMax = 1.1f;
        float bloomSoftKnee = 0.2f;
        float bloomUpsampleBlend = 1.0f;
        float bloomUpsampleSoftKnee = 0.35f;
        float blurRadius = 4.0f;
        float distortionScale = 0.02f;
        float toneExposure = 1.0f;
        float glowWeight = 1.0f;
        float glowTintR = 1.0f;
        float glowTintG = 1.0f;
        float glowTintB = 1.0f;
        float grayscaleMode = 1.0f;
        float vignetteRadius = 0.75f;
        float vignetteSoftness = 0.35f;
        float vignettePower = 1.0f;
        float boxBlurKernelRadius = 1.0f;
        float gaussianBlurKernelRadius = 2.0f;
        float gaussianBlurSigma = 1.5f;
        float outlineThreshold = 0.08f;
        float outlineThickness = 1.0f;
        float outlineSoftness = 0.04f;
        float outlineColorR = 0.0f;
        float outlineColorG = 0.0f;
        float outlineColorB = 0.0f;
        float outlineDepthWeight = 8.0f;
        float outlineNormalWeight = 0.75f;
        float outlineObjectWeight = 1.0f;
        float radialCenterX = 0.5f;
        float radialCenterY = 0.5f;
        float radialBlurWidth = 0.08f;
        float radialSampleCount = 12.0f;
    };

    std::string name;
    std::string inputResource = "SceneColor";
    std::string outputResource = "PostColor";
    std::string pipeline = "FullscreenComposite";
    std::string secondaryInputResource = "VfxAccumulation";
    std::string tertiaryInputResource = "SceneColor";
    bool enabled = true;
    float intensity = 1.0f;
    float resolutionScale = 1.0f;
    Parameters parameters{};
};

struct PostProcessExecutionPass {
    PostProcessPass pass;
};

struct PostProcessExecutionPlan {
    std::vector<PostProcessExecutionPass> passes;
    std::string finalOutputResource = "SceneColor";
};

class PostProcessStack {
public:
    void ResetToVfxDefaults();
    void SetEnabled(const std::string& name, bool enabled);
    void SetIntensity(const std::string& name, float intensity);
    void TriggerRadialBlurEvent(float centerX, float centerY, float intensity, float durationSeconds);
    void UpdateRuntimeEffects(float deltaTime);
    bool IsEnabled(const std::string& name) const;
    float Intensity(const std::string& name) const;
    std::string FinalOutputResource() const;
    PostProcessExecutionPlan BuildExecutionPlan() const;
    const std::vector<PostProcessPass>& Passes() const { return passes_; }
    std::vector<PostProcessPass>& MutablePasses() { return passes_; }

private:
    PostProcessPass* FindPass(std::string_view name);
    const PostProcessPass* FindPass(std::string_view name) const;
    void RestoreRadialBlurManualState();

    struct RuntimeRadialBlurState {
        bool active = false;
        float centerX = 0.5f;
        float centerY = 0.5f;
        float intensity = 1.0f;
        float durationSeconds = 0.35f;
        float elapsedSeconds = 0.0f;
        bool restoreEnabled = false;
        float restoreIntensity = 1.0f;
        float restoreCenterX = 0.5f;
        float restoreCenterY = 0.5f;
    };

    std::vector<PostProcessPass> passes_;
    RuntimeRadialBlurState runtimeRadialBlur_{};
};
