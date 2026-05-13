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
        }
        ImGui::Text("  %s -> %s pipeline=%s scale=%.2f",
            pass.inputResource.c_str(),
            pass.outputResource.c_str(),
            pass.pipeline.c_str(),
            pass.resolutionScale);
        ImGui::PopID();
    }
}
