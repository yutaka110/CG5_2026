#include "PostProcessStack.h"

#include <algorithm>
#include <utility>

namespace {
constexpr const char* kPostProcessOutputResource = "PostProcessOutput";
constexpr const char* kPostProcessSwapOutputResource = "PostProcessSwapOutput";
constexpr const char* kPostProcessPreviousInputResource = "PostProcessPreviousInput";

std::string ResolvePostProcessInputResource(
    std::string_view resource,
    std::string_view currentOutput,
    std::string_view previousInput) {
    if (resource == kPostProcessOutputResource) {
        return std::string(currentOutput);
    }
    if (resource == kPostProcessPreviousInputResource) {
        return std::string(previousInput);
    }
    return std::string(resource);
}

std::string ResolvePostProcessOutputResource(std::string_view resource, std::string_view currentOutput) {
    if (resource != kPostProcessSwapOutputResource) {
        return std::string(resource);
    }
    return currentOutput == "PostColorA" ? "PostColorB" : "PostColorA";
}
} // namespace

void PostProcessStack::ResetToVfxDefaults() {
    passes_.clear();
    runtimeRadialBlur_ = {};
    PostProcessPass bloomExtract{};
    bloomExtract.name = "BloomExtract";
    bloomExtract.inputResource = "VfxAccumulation";
    bloomExtract.outputResource = "BloomExtractHalf";
    bloomExtract.pipeline = "BloomExtract";
    bloomExtract.secondaryInputResource = "VfxAccumulation";
    bloomExtract.tertiaryInputResource = "VfxAccumulation";
    bloomExtract.enabled = true;
    bloomExtract.intensity = 1.0f;
    bloomExtract.resolutionScale = 0.5f;
    bloomExtract.parameters.bloomThresholdMin = 0.45f;
    bloomExtract.parameters.bloomThresholdMax = 1.1f;
    bloomExtract.parameters.bloomSoftKnee = 0.2f;
    passes_.push_back(bloomExtract);

    PostProcessPass bloomDownsampleQuarter{};
    bloomDownsampleQuarter.name = "BloomDownsampleQuarter";
    bloomDownsampleQuarter.inputResource = "BloomExtractHalf";
    bloomDownsampleQuarter.outputResource = "BloomQuarterA";
    bloomDownsampleQuarter.pipeline = "BloomDownsample";
    bloomDownsampleQuarter.secondaryInputResource = "BloomExtractHalf";
    bloomDownsampleQuarter.tertiaryInputResource = "BloomExtractHalf";
    bloomDownsampleQuarter.enabled = true;
    bloomDownsampleQuarter.intensity = 1.0f;
    bloomDownsampleQuarter.resolutionScale = 0.25f;
    passes_.push_back(bloomDownsampleQuarter);

    PostProcessPass bloomBlurHorizontalQuarter{};
    bloomBlurHorizontalQuarter.name = "BloomBlurHorizontalQuarter";
    bloomBlurHorizontalQuarter.inputResource = "BloomQuarterA";
    bloomBlurHorizontalQuarter.outputResource = "BloomQuarterB";
    bloomBlurHorizontalQuarter.pipeline = "BlurHorizontal";
    bloomBlurHorizontalQuarter.secondaryInputResource = "BloomQuarterA";
    bloomBlurHorizontalQuarter.tertiaryInputResource = "BloomQuarterA";
    bloomBlurHorizontalQuarter.enabled = true;
    bloomBlurHorizontalQuarter.intensity = 1.0f;
    bloomBlurHorizontalQuarter.resolutionScale = 0.25f;
    bloomBlurHorizontalQuarter.parameters.blurRadius = 6.0f;
    passes_.push_back(bloomBlurHorizontalQuarter);

    PostProcessPass bloomBlurVerticalQuarter{};
    bloomBlurVerticalQuarter.name = "BloomBlurVerticalQuarter";
    bloomBlurVerticalQuarter.inputResource = "BloomQuarterB";
    bloomBlurVerticalQuarter.outputResource = "BloomQuarterA";
    bloomBlurVerticalQuarter.pipeline = "BlurVertical";
    bloomBlurVerticalQuarter.secondaryInputResource = "BloomQuarterB";
    bloomBlurVerticalQuarter.tertiaryInputResource = "BloomQuarterB";
    bloomBlurVerticalQuarter.enabled = true;
    bloomBlurVerticalQuarter.intensity = 1.0f;
    bloomBlurVerticalQuarter.resolutionScale = 0.25f;
    bloomBlurVerticalQuarter.parameters.blurRadius = 6.0f;
    passes_.push_back(bloomBlurVerticalQuarter);

    PostProcessPass bloomDownsampleEighth{};
    bloomDownsampleEighth.name = "BloomDownsampleEighth";
    bloomDownsampleEighth.inputResource = "BloomQuarterA";
    bloomDownsampleEighth.outputResource = "BloomEighthA";
    bloomDownsampleEighth.pipeline = "BloomDownsample";
    bloomDownsampleEighth.secondaryInputResource = "BloomQuarterA";
    bloomDownsampleEighth.tertiaryInputResource = "BloomQuarterA";
    bloomDownsampleEighth.enabled = true;
    bloomDownsampleEighth.intensity = 1.0f;
    bloomDownsampleEighth.resolutionScale = 0.125f;
    passes_.push_back(bloomDownsampleEighth);

    PostProcessPass bloomBlurHorizontalEighth{};
    bloomBlurHorizontalEighth.name = "BloomBlurHorizontalEighth";
    bloomBlurHorizontalEighth.inputResource = "BloomEighthA";
    bloomBlurHorizontalEighth.outputResource = "BloomEighthB";
    bloomBlurHorizontalEighth.pipeline = "BlurHorizontal";
    bloomBlurHorizontalEighth.secondaryInputResource = "BloomEighthA";
    bloomBlurHorizontalEighth.tertiaryInputResource = "BloomEighthA";
    bloomBlurHorizontalEighth.enabled = true;
    bloomBlurHorizontalEighth.intensity = 1.0f;
    bloomBlurHorizontalEighth.resolutionScale = 0.125f;
    bloomBlurHorizontalEighth.parameters.blurRadius = 4.0f;
    passes_.push_back(bloomBlurHorizontalEighth);

    PostProcessPass bloomBlurVerticalEighth{};
    bloomBlurVerticalEighth.name = "BloomBlurVerticalEighth";
    bloomBlurVerticalEighth.inputResource = "BloomEighthB";
    bloomBlurVerticalEighth.outputResource = "BloomEighthA";
    bloomBlurVerticalEighth.pipeline = "BlurVertical";
    bloomBlurVerticalEighth.secondaryInputResource = "BloomEighthB";
    bloomBlurVerticalEighth.tertiaryInputResource = "BloomEighthB";
    bloomBlurVerticalEighth.enabled = true;
    bloomBlurVerticalEighth.intensity = 1.0f;
    bloomBlurVerticalEighth.resolutionScale = 0.125f;
    bloomBlurVerticalEighth.parameters.blurRadius = 4.0f;
    passes_.push_back(bloomBlurVerticalEighth);

    PostProcessPass bloomUpsampleQuarter{};
    bloomUpsampleQuarter.name = "BloomUpsampleQuarter";
    bloomUpsampleQuarter.inputResource = "BloomEighthA";
    bloomUpsampleQuarter.outputResource = "BloomQuarterComposite";
    bloomUpsampleQuarter.pipeline = "BloomUpsample";
    bloomUpsampleQuarter.secondaryInputResource = "BloomQuarterA";
    bloomUpsampleQuarter.tertiaryInputResource = "BloomQuarterA";
    bloomUpsampleQuarter.enabled = true;
    bloomUpsampleQuarter.intensity = 1.0f;
    bloomUpsampleQuarter.resolutionScale = 0.25f;
    bloomUpsampleQuarter.parameters.bloomUpsampleBlend = 0.85f;
    bloomUpsampleQuarter.parameters.bloomUpsampleSoftKnee = 0.35f;
    passes_.push_back(bloomUpsampleQuarter);

    PostProcessPass bloomUpsampleHalf{};
    bloomUpsampleHalf.name = "BloomUpsampleHalf";
    bloomUpsampleHalf.inputResource = "BloomQuarterComposite";
    bloomUpsampleHalf.outputResource = "BloomComposite";
    bloomUpsampleHalf.pipeline = "BloomUpsample";
    bloomUpsampleHalf.secondaryInputResource = "BloomExtractHalf";
    bloomUpsampleHalf.tertiaryInputResource = "BloomExtractHalf";
    bloomUpsampleHalf.enabled = true;
    bloomUpsampleHalf.intensity = 1.0f;
    bloomUpsampleHalf.resolutionScale = 0.5f;
    bloomUpsampleHalf.parameters.bloomUpsampleBlend = 0.7f;
    bloomUpsampleHalf.parameters.bloomUpsampleSoftKnee = 0.25f;
    passes_.push_back(bloomUpsampleHalf);

    PostProcessPass distortionComposite{};
    distortionComposite.name = "DistortionComposite";
    distortionComposite.inputResource = "VfxAccumulation";
    distortionComposite.outputResource = "PostColorA";
    distortionComposite.pipeline = "DistortionComposite";
    distortionComposite.secondaryInputResource = "SceneColor";
    distortionComposite.tertiaryInputResource = "VfxAccumulation";
    distortionComposite.enabled = true;
    distortionComposite.intensity = 1.0f;
    distortionComposite.resolutionScale = 1.0f;
    distortionComposite.parameters.distortionScale = 0.02f;
    passes_.push_back(distortionComposite);

    PostProcessPass toneMapping{};
    toneMapping.name = "ToneMapping";
    toneMapping.inputResource = "PostColorA";
    toneMapping.outputResource = "PostColorB";
    toneMapping.pipeline = "ToneMapping";
    toneMapping.secondaryInputResource = "PostColorA";
    toneMapping.tertiaryInputResource = "PostColorA";
    toneMapping.enabled = true;
    toneMapping.intensity = 1.0f;
    toneMapping.resolutionScale = 1.0f;
    toneMapping.parameters.toneExposure = 1.0f;
    passes_.push_back(toneMapping);

    PostProcessPass glowComposite{};
    glowComposite.name = "GlowComposite";
    glowComposite.inputResource = "BloomComposite";
    glowComposite.outputResource = "PostColorA";
    glowComposite.pipeline = "GlowComposite";
    glowComposite.secondaryInputResource = "VfxAccumulation";
    glowComposite.tertiaryInputResource = "PostColorB";
    glowComposite.enabled = true;
    glowComposite.intensity = 1.0f;
    glowComposite.resolutionScale = 1.0f;
    glowComposite.parameters.glowWeight = 1.0f;
    glowComposite.parameters.glowTintR = 1.0f;
    glowComposite.parameters.glowTintG = 0.95f;
    glowComposite.parameters.glowTintB = 1.2f;
    passes_.push_back(glowComposite);

    PostProcessPass radialBlur{};
    radialBlur.name = "RadialBlur";
    radialBlur.inputResource = kPostProcessOutputResource;
    radialBlur.outputResource = kPostProcessSwapOutputResource;
    radialBlur.pipeline = "RadialBlur";
    radialBlur.secondaryInputResource = kPostProcessOutputResource;
    radialBlur.tertiaryInputResource = kPostProcessOutputResource;
    radialBlur.enabled = false;
    radialBlur.intensity = 1.0f;
    radialBlur.resolutionScale = 1.0f;
    radialBlur.parameters.radialCenterX = 0.5f;
    radialBlur.parameters.radialCenterY = 0.5f;
    radialBlur.parameters.radialBlurWidth = 0.08f;
    radialBlur.parameters.radialSampleCount = 12.0f;
    passes_.push_back(radialBlur);

    PostProcessPass boxBlurHorizontal{};
    boxBlurHorizontal.name = "BoxBlurHorizontal";
    boxBlurHorizontal.inputResource = kPostProcessOutputResource;
    boxBlurHorizontal.outputResource = kPostProcessSwapOutputResource;
    boxBlurHorizontal.pipeline = "BoxBlurHorizontal";
    boxBlurHorizontal.secondaryInputResource = kPostProcessOutputResource;
    boxBlurHorizontal.tertiaryInputResource = kPostProcessOutputResource;
    boxBlurHorizontal.enabled = false;
    boxBlurHorizontal.intensity = 1.0f;
    boxBlurHorizontal.resolutionScale = 1.0f;
    boxBlurHorizontal.parameters.boxBlurKernelRadius = 1.0f;
    passes_.push_back(boxBlurHorizontal);

    PostProcessPass boxBlurVertical{};
    boxBlurVertical.name = "BoxBlurVertical";
    boxBlurVertical.inputResource = kPostProcessOutputResource;
    boxBlurVertical.outputResource = "PostBlurComposite";
    boxBlurVertical.pipeline = "BoxBlurVertical";
    boxBlurVertical.secondaryInputResource = kPostProcessPreviousInputResource;
    boxBlurVertical.tertiaryInputResource = kPostProcessPreviousInputResource;
    boxBlurVertical.enabled = false;
    boxBlurVertical.intensity = 1.0f;
    boxBlurVertical.resolutionScale = 1.0f;
    boxBlurVertical.parameters.boxBlurKernelRadius = 1.0f;
    passes_.push_back(boxBlurVertical);

    PostProcessPass gaussianBlurHorizontal{};
    gaussianBlurHorizontal.name = "GaussianBlurHorizontal";
    gaussianBlurHorizontal.inputResource = kPostProcessOutputResource;
    gaussianBlurHorizontal.outputResource = kPostProcessSwapOutputResource;
    gaussianBlurHorizontal.pipeline = "GaussianBlurHorizontal";
    gaussianBlurHorizontal.secondaryInputResource = kPostProcessOutputResource;
    gaussianBlurHorizontal.tertiaryInputResource = kPostProcessOutputResource;
    gaussianBlurHorizontal.enabled = false;
    gaussianBlurHorizontal.intensity = 1.0f;
    gaussianBlurHorizontal.resolutionScale = 1.0f;
    gaussianBlurHorizontal.parameters.gaussianBlurKernelRadius = 2.0f;
    gaussianBlurHorizontal.parameters.gaussianBlurSigma = 1.5f;
    passes_.push_back(gaussianBlurHorizontal);

    PostProcessPass gaussianBlurVertical{};
    gaussianBlurVertical.name = "GaussianBlurVertical";
    gaussianBlurVertical.inputResource = kPostProcessOutputResource;
    gaussianBlurVertical.outputResource = "PostGaussianComposite";
    gaussianBlurVertical.pipeline = "GaussianBlurVertical";
    gaussianBlurVertical.secondaryInputResource = kPostProcessPreviousInputResource;
    gaussianBlurVertical.tertiaryInputResource = kPostProcessPreviousInputResource;
    gaussianBlurVertical.enabled = false;
    gaussianBlurVertical.intensity = 1.0f;
    gaussianBlurVertical.resolutionScale = 1.0f;
    gaussianBlurVertical.parameters.gaussianBlurKernelRadius = 2.0f;
    gaussianBlurVertical.parameters.gaussianBlurSigma = 1.5f;
    passes_.push_back(gaussianBlurVertical);

    PostProcessPass prewittOutline{};
    prewittOutline.name = "PrewittOutline";
    prewittOutline.inputResource = kPostProcessOutputResource;
    prewittOutline.outputResource = kPostProcessSwapOutputResource;
    prewittOutline.pipeline = "PrewittOutline";
    prewittOutline.secondaryInputResource = kPostProcessOutputResource;
    prewittOutline.tertiaryInputResource = kPostProcessOutputResource;
    prewittOutline.enabled = false;
    prewittOutline.intensity = 1.0f;
    prewittOutline.resolutionScale = 1.0f;
    prewittOutline.parameters.outlineThreshold = 0.08f;
    prewittOutline.parameters.outlineThickness = 1.0f;
    prewittOutline.parameters.outlineSoftness = 0.04f;
    prewittOutline.parameters.outlineColorR = 0.0f;
    prewittOutline.parameters.outlineColorG = 0.0f;
    prewittOutline.parameters.outlineColorB = 0.0f;
    prewittOutline.parameters.outlineDepthWeight = 8.0f;
    prewittOutline.parameters.outlineNormalWeight = 0.75f;
    prewittOutline.parameters.outlineObjectWeight = 1.0f;
    passes_.push_back(prewittOutline);

    PostProcessPass grayscale{};
    grayscale.name = "Grayscale";
    grayscale.inputResource = kPostProcessOutputResource;
    grayscale.outputResource = kPostProcessSwapOutputResource;
    grayscale.pipeline = "Grayscale";
    grayscale.secondaryInputResource = kPostProcessOutputResource;
    grayscale.tertiaryInputResource = kPostProcessOutputResource;
    grayscale.enabled = false;
    grayscale.intensity = 1.0f;
    grayscale.resolutionScale = 1.0f;
    grayscale.parameters.grayscaleMode = 1.0f;
    passes_.push_back(grayscale);

    PostProcessPass vignette{};
    vignette.name = "Vignette";
    vignette.inputResource = kPostProcessOutputResource;
    vignette.outputResource = kPostProcessSwapOutputResource;
    vignette.pipeline = "Vignette";
    vignette.secondaryInputResource = kPostProcessOutputResource;
    vignette.tertiaryInputResource = kPostProcessOutputResource;
    vignette.enabled = true;
    vignette.intensity = 1.0f;
    vignette.resolutionScale = 1.0f;
    vignette.parameters.vignetteRadius = 0.75f;
    vignette.parameters.vignetteSoftness = 0.35f;
    vignette.parameters.vignettePower = 1.0f;
    passes_.push_back(vignette);

    PostProcessPass dissolvePreview{};
    dissolvePreview.name = "DissolvePreviewPlane";
    dissolvePreview.inputResource = kPostProcessOutputResource;
    dissolvePreview.outputResource = kPostProcessSwapOutputResource;
    dissolvePreview.pipeline = "DissolvePreview";
    dissolvePreview.secondaryInputResource = kPostProcessOutputResource;
    dissolvePreview.tertiaryInputResource = kPostProcessOutputResource;
    dissolvePreview.enabled = false;
    dissolvePreview.intensity = 1.0f;
    dissolvePreview.resolutionScale = 1.0f;
    dissolvePreview.parameters.dissolvePreviewThreshold = 0.5f;
    dissolvePreview.parameters.dissolvePreviewEdgeWidth = 0.04f;
    dissolvePreview.parameters.dissolvePreviewNoiseScale = 8.0f;
    dissolvePreview.parameters.dissolvePreviewFillR = 0.0f;
    dissolvePreview.parameters.dissolvePreviewFillG = 1.0f;
    dissolvePreview.parameters.dissolvePreviewFillB = 0.0f;
    dissolvePreview.parameters.dissolvePreviewEdgeR = 1.0f;
    dissolvePreview.parameters.dissolvePreviewEdgeG = 0.35f;
    dissolvePreview.parameters.dissolvePreviewEdgeB = 0.2f;
    dissolvePreview.parameters.dissolvePreviewPlaneScaleX = 0.7f;
    dissolvePreview.parameters.dissolvePreviewPlaneScaleY = 0.55f;
    passes_.push_back(dissolvePreview);

    PostProcessPass randomPreview{};
    randomPreview.name = "RandomPreview";
    randomPreview.inputResource = kPostProcessOutputResource;
    randomPreview.outputResource = kPostProcessSwapOutputResource;
    randomPreview.pipeline = "RandomPreview";
    randomPreview.secondaryInputResource = kPostProcessOutputResource;
    randomPreview.tertiaryInputResource = kPostProcessOutputResource;
    randomPreview.enabled = true;
    randomPreview.intensity = 0.1f;
    randomPreview.resolutionScale = 1.0f;
    randomPreview.parameters.randomMode = 0.0f;
    randomPreview.parameters.randomScale = 256.0f;
    randomPreview.parameters.randomSpeed = 0.0f;
    randomPreview.parameters.randomTime = 0.0f;
    passes_.push_back(randomPreview);
}

void PostProcessStack::SetEnabled(const std::string& name, bool enabled) {
    for (PostProcessPass& pass : passes_) {
        if (pass.name == name) {
            pass.enabled = enabled;
            return;
        }
    }
}

void PostProcessStack::SetIntensity(const std::string& name, float intensity) {
    for (PostProcessPass& pass : passes_) {
        if (pass.name == name) {
            pass.intensity = intensity;
            return;
        }
    }
}

void PostProcessStack::TriggerRadialBlurEvent(
    float centerX,
    float centerY,
    float intensity,
    float durationSeconds) {
    PostProcessPass* radialBlur = FindPass("RadialBlur");
    if (radialBlur == nullptr) {
        return;
    }

    if (!runtimeRadialBlur_.active) {
        runtimeRadialBlur_.restoreEnabled = radialBlur->enabled;
        runtimeRadialBlur_.restoreIntensity = radialBlur->intensity;
        runtimeRadialBlur_.restoreCenterX = radialBlur->parameters.radialCenterX;
        runtimeRadialBlur_.restoreCenterY = radialBlur->parameters.radialCenterY;
    }

    runtimeRadialBlur_.active = true;
    runtimeRadialBlur_.centerX = std::clamp(centerX, 0.0f, 1.0f);
    runtimeRadialBlur_.centerY = std::clamp(centerY, 0.0f, 1.0f);
    runtimeRadialBlur_.intensity = (std::max)(0.0f, intensity);
    runtimeRadialBlur_.durationSeconds = (std::max)(0.001f, durationSeconds);
    runtimeRadialBlur_.elapsedSeconds = 0.0f;

    radialBlur->enabled = true;
    radialBlur->intensity = runtimeRadialBlur_.intensity;
    radialBlur->parameters.radialCenterX = runtimeRadialBlur_.centerX;
    radialBlur->parameters.radialCenterY = runtimeRadialBlur_.centerY;
}

void PostProcessStack::UpdateRuntimeEffects(float deltaTime) {
    const float safeDeltaTime = (std::max)(0.0f, deltaTime);
    if (PostProcessPass* randomPreview = FindPass("RandomPreview")) {
        if (randomPreview->enabled) {
            randomPreview->parameters.randomTime += safeDeltaTime;
        }
    }

    if (!runtimeRadialBlur_.active) {
        return;
    }

    PostProcessPass* radialBlur = FindPass("RadialBlur");
    if (radialBlur == nullptr) {
        runtimeRadialBlur_.active = false;
        return;
    }

    runtimeRadialBlur_.elapsedSeconds += safeDeltaTime;
    const float t = std::clamp(
        runtimeRadialBlur_.elapsedSeconds / runtimeRadialBlur_.durationSeconds,
        0.0f,
        1.0f);
    if (t >= 1.0f) {
        RestoreRadialBlurManualState();
        runtimeRadialBlur_.active = false;
        return;
    }

    const float fade = 1.0f - t;
    radialBlur->enabled = true;
    radialBlur->intensity = runtimeRadialBlur_.intensity * fade;
    radialBlur->parameters.radialCenterX = runtimeRadialBlur_.centerX;
    radialBlur->parameters.radialCenterY = runtimeRadialBlur_.centerY;
}

bool PostProcessStack::IsEnabled(const std::string& name) const {
    for (const PostProcessPass& pass : passes_) {
        if (pass.name == name) {
            return pass.enabled;
        }
    }
    return false;
}

float PostProcessStack::Intensity(const std::string& name) const {
    for (const PostProcessPass& pass : passes_) {
        if (pass.name == name) {
            return pass.enabled ? pass.intensity : 0.0f;
        }
    }
    return 0.0f;
}

std::string PostProcessStack::FinalOutputResource() const {
    return BuildExecutionPlan().finalOutputResource;
}

PostProcessExecutionPlan PostProcessStack::BuildExecutionPlan() const {
    PostProcessExecutionPlan plan{};
    std::unordered_set<std::string> availableResources = {
        "SceneColor",
        "VfxAccumulation",
    };
    std::string previousInputResource = plan.finalOutputResource;

    for (const PostProcessPass& pass : passes_) {
        if (!pass.enabled) {
            continue;
        }

        PostProcessPass resolvedPass = pass;
        resolvedPass.inputResource =
            ResolvePostProcessInputResource(pass.inputResource, plan.finalOutputResource, previousInputResource);
        resolvedPass.outputResource = ResolvePostProcessOutputResource(pass.outputResource, plan.finalOutputResource);
        resolvedPass.secondaryInputResource =
            ResolvePostProcessInputResource(
                pass.secondaryInputResource,
                plan.finalOutputResource,
                previousInputResource);
        resolvedPass.tertiaryInputResource =
            ResolvePostProcessInputResource(
                pass.tertiaryInputResource,
                plan.finalOutputResource,
                previousInputResource);

        const bool hasPrimaryInput = availableResources.contains(resolvedPass.inputResource);
        const bool hasSecondaryInput = availableResources.contains(resolvedPass.secondaryInputResource);
        const bool hasTertiaryInput = availableResources.contains(resolvedPass.tertiaryInputResource);
        if (!hasPrimaryInput || !hasSecondaryInput || !hasTertiaryInput) {
            continue;
        }

        PostProcessExecutionPass executionPass{};
        executionPass.pass = std::move(resolvedPass);
        previousInputResource = executionPass.pass.inputResource;
        plan.finalOutputResource = executionPass.pass.outputResource;
        availableResources.insert(executionPass.pass.outputResource);
        plan.passes.push_back(std::move(executionPass));
    }

    return plan;
}

PostProcessPass* PostProcessStack::FindPass(std::string_view name) {
    for (PostProcessPass& pass : passes_) {
        if (pass.name == name) {
            return &pass;
        }
    }
    return nullptr;
}

const PostProcessPass* PostProcessStack::FindPass(std::string_view name) const {
    for (const PostProcessPass& pass : passes_) {
        if (pass.name == name) {
            return &pass;
        }
    }
    return nullptr;
}

void PostProcessStack::RestoreRadialBlurManualState() {
    PostProcessPass* radialBlur = FindPass("RadialBlur");
    if (radialBlur == nullptr) {
        return;
    }

    radialBlur->enabled = runtimeRadialBlur_.restoreEnabled;
    radialBlur->intensity = runtimeRadialBlur_.restoreIntensity;
    radialBlur->parameters.radialCenterX = runtimeRadialBlur_.restoreCenterX;
    radialBlur->parameters.radialCenterY = runtimeRadialBlur_.restoreCenterY;
}
