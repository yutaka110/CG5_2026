#include "AppPostProcessPanel.h"

#include "PostProcessStack.h"

#include "../../externals/imgui/imgui.h"

void DrawPostProcessPanel(
    PostProcessStack& postProcessStack) {
    std::vector<PostProcessPass>& passes = postProcessStack.MutablePasses();
    for (PostProcessPass& pass : passes) {
        if (pass.pipeline == "BoxBlurVertical") {
            continue;
        }
        if (pass.pipeline == "GaussianBlurVertical") {
            continue;
        }
        if (pass.pipeline == "BoxBlurHorizontal") {
            PostProcessPass* verticalPass = nullptr;
            for (PostProcessPass& candidate : passes) {
                if (candidate.pipeline == "BoxBlurVertical") {
                    verticalPass = &candidate;
                    break;
                }
            }

            ImGui::PushID(pass.name.c_str());
            bool enabled = pass.enabled && (verticalPass == nullptr || verticalPass->enabled);
            if (ImGui::Checkbox("BoxBlur", &enabled)) {
                pass.enabled = enabled;
                if (verticalPass != nullptr) {
                    verticalPass->enabled = enabled;
                }
            }
            ImGui::SameLine();
            float intensity = pass.intensity;
            if (ImGui::SliderFloat("Intensity", &intensity, 0.0f, 4.0f)) {
                pass.intensity = intensity;
                if (verticalPass != nullptr) {
                    verticalPass->intensity = intensity;
                }
            }
            const char* kernels[] = { "3x3", "5x5" };
            int kernel = pass.parameters.boxBlurKernelRadius >= 1.5f ? 1 : 0;
            if (ImGui::Combo("Kernel", &kernel, kernels, _countof(kernels))) {
                const float kernelRadius = kernel == 0 ? 1.0f : 2.0f;
                pass.parameters.boxBlurKernelRadius = kernelRadius;
                if (verticalPass != nullptr) {
                    verticalPass->parameters.boxBlurKernelRadius = kernelRadius;
                }
            }
            ImGui::Text("  separable box blur: horizontal -> vertical scale=%.2f", pass.resolutionScale);
            ImGui::PopID();
            continue;
        }
        if (pass.pipeline == "GaussianBlurHorizontal") {
            PostProcessPass* verticalPass = nullptr;
            for (PostProcessPass& candidate : passes) {
                if (candidate.pipeline == "GaussianBlurVertical") {
                    verticalPass = &candidate;
                    break;
                }
            }

            ImGui::PushID(pass.name.c_str());
            bool enabled = pass.enabled && (verticalPass == nullptr || verticalPass->enabled);
            if (ImGui::Checkbox("GaussianBlur", &enabled)) {
                pass.enabled = enabled;
                if (verticalPass != nullptr) {
                    verticalPass->enabled = enabled;
                }
            }
            ImGui::SameLine();
            float intensity = pass.intensity;
            if (ImGui::SliderFloat("Intensity", &intensity, 0.0f, 4.0f)) {
                pass.intensity = intensity;
                if (verticalPass != nullptr) {
                    verticalPass->intensity = intensity;
                }
            }
            const char* kernels[] = { "3x3", "5x5", "7x7", "9x9" };
            int kernel = static_cast<int>(pass.parameters.gaussianBlurKernelRadius) - 1;
            kernel = kernel < 0 ? 0 : (kernel > 3 ? 3 : kernel);
            if (ImGui::Combo("Kernel", &kernel, kernels, _countof(kernels))) {
                const float kernelRadius = static_cast<float>(kernel + 1);
                pass.parameters.gaussianBlurKernelRadius = kernelRadius;
                if (verticalPass != nullptr) {
                    verticalPass->parameters.gaussianBlurKernelRadius = kernelRadius;
                }
            }
            float sigma = pass.parameters.gaussianBlurSigma;
            if (ImGui::SliderFloat("Sigma", &sigma, 0.3f, 6.0f)) {
                pass.parameters.gaussianBlurSigma = sigma;
                if (verticalPass != nullptr) {
                    verticalPass->parameters.gaussianBlurSigma = sigma;
                }
            }
            ImGui::Text("  separable gaussian blur: horizontal -> vertical scale=%.2f", pass.resolutionScale);
            ImGui::PopID();
            continue;
        }

        ImGui::PushID(pass.name.c_str());
        ImGui::Checkbox(pass.name.c_str(), &pass.enabled);
        ImGui::SameLine();
        ImGui::SliderFloat("Intensity", &pass.intensity, 0.0f, 4.0f);
        if (pass.pipeline == "BloomExtract") {
            ImGui::SliderFloat("Threshold Min", &pass.parameters.bloomThresholdMin, 0.0f, 2.0f);
            ImGui::SliderFloat("Threshold Max", &pass.parameters.bloomThresholdMax, 0.0f, 4.0f);
            ImGui::SliderFloat("Soft Knee", &pass.parameters.bloomSoftKnee, 0.01f, 1.0f);
        } else if (pass.pipeline == "BloomUpsample") {
            ImGui::SliderFloat("Blend", &pass.parameters.bloomUpsampleBlend, 0.0f, 1.5f);
            ImGui::SliderFloat("Soft Knee", &pass.parameters.bloomUpsampleSoftKnee, 0.01f, 1.0f);
        } else if (pass.pipeline == "BlurHorizontal" || pass.pipeline == "BlurVertical") {
            ImGui::SliderFloat("Blur Radius", &pass.parameters.blurRadius, 1.0f, 8.0f);
        } else if (pass.pipeline == "DistortionComposite") {
            ImGui::SliderFloat("Distortion Scale", &pass.parameters.distortionScale, 0.0f, 0.1f);
        } else if (pass.pipeline == "ToneMapping") {
            ImGui::SliderFloat("Exposure", &pass.parameters.toneExposure, 0.1f, 4.0f);
        } else if (pass.pipeline == "GlowComposite") {
            ImGui::SliderFloat("Glow Weight", &pass.parameters.glowWeight, 0.0f, 4.0f);
            ImGui::ColorEdit3("Glow Tint", &pass.parameters.glowTintR);
        } else if (pass.pipeline == "RadialBlur") {
            ImGui::SliderFloat("Center X", &pass.parameters.radialCenterX, 0.0f, 1.0f);
            ImGui::SliderFloat("Center Y", &pass.parameters.radialCenterY, 0.0f, 1.0f);
            ImGui::SliderFloat("Blur Width", &pass.parameters.radialBlurWidth, 0.0f, 0.5f);
            ImGui::SliderFloat("Samples", &pass.parameters.radialSampleCount, 2.0f, 32.0f, "%.0f");
        } else if (pass.pipeline == "PrewittOutline") {
            ImGui::SliderFloat("Threshold", &pass.parameters.outlineThreshold, 0.001f, 0.5f);
            ImGui::SliderFloat("Thickness", &pass.parameters.outlineThickness, 1.0f, 4.0f);
            ImGui::SliderFloat("Softness", &pass.parameters.outlineSoftness, 0.001f, 0.25f);
            ImGui::SliderFloat("Depth Weight", &pass.parameters.outlineDepthWeight, 0.0f, 64.0f);
            ImGui::SliderFloat("Normal Weight", &pass.parameters.outlineNormalWeight, 0.0f, 8.0f);
            ImGui::SliderFloat("Object Weight", &pass.parameters.outlineObjectWeight, 0.0f, 8.0f);
            ImGui::ColorEdit3("Outline Color", &pass.parameters.outlineColorR);
        } else if (pass.pipeline == "Grayscale") {
            const char* modes[] = { "Average", "BT.709" };
            int mode = pass.parameters.grayscaleMode >= 0.5f ? 1 : 0;
            if (ImGui::Combo("Mode", &mode, modes, _countof(modes))) {
                pass.parameters.grayscaleMode = static_cast<float>(mode);
            }
        } else if (pass.pipeline == "Vignette") {
            ImGui::SliderFloat("Radius", &pass.parameters.vignetteRadius, 0.0f, 1.5f);
            ImGui::SliderFloat("Softness", &pass.parameters.vignetteSoftness, 0.01f, 1.0f);
            ImGui::SliderFloat("Power", &pass.parameters.vignettePower, 0.1f, 4.0f);
        } else if (pass.pipeline == "DissolvePreview") {
            ImGui::SliderFloat("Threshold", &pass.parameters.dissolvePreviewThreshold, 0.0f, 1.0f);
            ImGui::SliderFloat("Edge Width", &pass.parameters.dissolvePreviewEdgeWidth, 0.001f, 0.25f);
            ImGui::SliderFloat("Noise Scale", &pass.parameters.dissolvePreviewNoiseScale, 1.0f, 32.0f);
            ImGui::ColorEdit3("Fill Color", &pass.parameters.dissolvePreviewFillR);
            ImGui::ColorEdit3("Edge Color", &pass.parameters.dissolvePreviewEdgeR);
            ImGui::SliderFloat("Plane Width", &pass.parameters.dissolvePreviewPlaneScaleX, 0.1f, 1.0f);
            ImGui::SliderFloat("Plane Height", &pass.parameters.dissolvePreviewPlaneScaleY, 0.1f, 1.0f);
        } else if (pass.pipeline == "RandomPreview") {
            const char* modes[] = { "Grayscale", "Multiply" };
            int mode = pass.parameters.randomMode >= 0.5f ? 1 : 0;
            if (ImGui::Combo("Mode", &mode, modes, _countof(modes))) {
                pass.parameters.randomMode = static_cast<float>(mode);
            }
            ImGui::SliderFloat("Scale", &pass.parameters.randomScale, 1.0f, 1024.0f, "%.0f");
            ImGui::SliderFloat("Speed", &pass.parameters.randomSpeed, 0.0f, 20.0f, "%.1f");
            ImGui::Text("  Speed 0 keeps the random pattern fixed.");
        }
        ImGui::Text("  %s -> %s pipeline=%s scale=%.2f",
            pass.inputResource.c_str(),
            pass.outputResource.c_str(),
            pass.pipeline.c_str(),
            pass.resolutionScale);
        ImGui::PopID();
    }
}
