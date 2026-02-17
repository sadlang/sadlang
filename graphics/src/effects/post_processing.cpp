// ==============================================================================
// post_processing.cpp - Post-Processing Effects Implementation
// ==============================================================================

#include "../../include/effects/post_processing.h"
#include <glad/glad.h>
#include <algorithm>
#include <cmath>

namespace sad {
namespace graphics {

// ==============================================================================
// Shader Source Code
// ==============================================================================

namespace {

// Fullscreen quad vertex shader (shared by all effects)
const char* COPY_VS = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;
out vec2 TexCoord;
void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    TexCoord = aTexCoord;
}
)";

const char* COPY_FS = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D uTexture;
void main() {
    FragColor = texture(uTexture, TexCoord);
}
)";

const char* BLOOM_EXTRACT_FS = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D uTexture;
uniform float uThreshold;
void main() {
    vec4 color = texture(uTexture, TexCoord);
    float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > uThreshold)
        FragColor = color;
    else
        FragColor = vec4(0.0);
}
)";

const char* BLUR_FS = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D uTexture;
uniform vec2 uDirection;
uniform float uResolution;
uniform float uRadius;
void main() {
    vec4 sum = vec4(0.0);
    float total = 0.0;
    for (float i = -uRadius; i <= uRadius; i += 1.0) {
        float weight = exp(-(i*i) / (2.0 * uRadius * uRadius / 9.0));
        vec2 offset = uDirection * i / uResolution;
        sum += texture(uTexture, TexCoord + offset) * weight;
        total += weight;
    }
    FragColor = sum / total;
}
)";

const char* BLOOM_COMBINE_FS = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D uScene;
uniform sampler2D uBloom;
uniform float uIntensity;
void main() {
    vec4 sceneColor = texture(uScene, TexCoord);
    vec4 bloomColor = texture(uBloom, TexCoord);
    FragColor = sceneColor + bloomColor * uIntensity;
}
)";

const char* CHROMATIC_FS = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D uTexture;
uniform float uIntensity;
uniform float uRedOffset;
uniform float uBlueOffset;
void main() {
    vec2 dir = TexCoord - vec2(0.5);
    float r = texture(uTexture, TexCoord + dir * uRedOffset * uIntensity).r;
    float g = texture(uTexture, TexCoord).g;
    float b = texture(uTexture, TexCoord + dir * uBlueOffset * uIntensity).b;
    float a = texture(uTexture, TexCoord).a;
    FragColor = vec4(r, g, b, a);
}
)";

const char* VIGNETTE_FS = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D uTexture;
uniform float uIntensity;
uniform float uRadius;
uniform float uSmoothness;
uniform vec4 uColor;
void main() {
    vec4 color = texture(uTexture, TexCoord);
    vec2 center = vec2(0.5);
    float dist = distance(TexCoord, center);
    float vignette = smoothstep(uRadius, uRadius - uSmoothness, dist);
    color.rgb = mix(uColor.rgb, color.rgb, vignette * (1.0 - uIntensity) + (1.0 - uIntensity));
    FragColor = color;
}
)";

const char* COLOR_GRADING_FS = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D uTexture;
uniform float uBrightness;
uniform float uContrast;
uniform float uSaturation;
uniform float uGamma;
uniform vec3 uColorBalance;
uniform float uTemperature;
uniform float uTint;
void main() {
    vec4 color = texture(uTexture, TexCoord);
    color.rgb += uBrightness;
    color.rgb = (color.rgb - 0.5) * uContrast + 0.5;
    float gray = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    color.rgb = mix(vec3(gray), color.rgb, uSaturation);
    color.rgb *= uColorBalance;
    color.r += uTemperature * 0.1;
    color.b -= uTemperature * 0.1;
    color.g += uTint * 0.1;
    color.rgb = pow(color.rgb, vec3(1.0 / uGamma));
    FragColor = clamp(color, 0.0, 1.0);
}
)";

const char* PIXELATE_FS = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D uTexture;
uniform float uPixelSize;
uniform vec2 uResolution;
void main() {
    vec2 pixels = uResolution / uPixelSize;
    vec2 coord = floor(TexCoord * pixels) / pixels;
    FragColor = texture(uTexture, coord);
}
)";

const char* CRT_FS = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D uTexture;
uniform float uCurvature;
uniform float uScanlineIntensity;
uniform float uVignette;
uniform float uNoise;
uniform float uFlickering;
uniform float uTime;
uniform vec4 uTint;
uniform vec2 uResolution;

float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    vec2 uv = TexCoord - 0.5;
    float dist = dot(uv, uv);
    uv *= 1.0 + dist * uCurvature;
    uv += 0.5;
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {
        FragColor = vec4(0.0);
        return;
    }
    vec4 color = texture(uTexture, uv);
    float scanline = sin(uv.y * uResolution.y * 3.14159) * 0.5 + 0.5;
    color.rgb *= 1.0 - uScanlineIntensity * (1.0 - scanline);
    float noise = rand(uv + uTime) * uNoise;
    color.rgb += noise - uNoise * 0.5;
    color.rgb *= 1.0 + sin(uTime * 10.0) * uFlickering;
    float vig = 1.0 - dist * uVignette * 2.0;
    color.rgb *= vig;
    color.rgb *= uTint.rgb;
    FragColor = color;
}
)";

const char* FILM_GRAIN_FS = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D uTexture;
uniform float uIntensity;
uniform float uTime;
uniform bool uColored;

float rand(vec2 co, float seed) {
    return fract(sin(dot(co.xy * seed, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    vec4 color = texture(uTexture, TexCoord);
    float seed = uTime;
    if (uColored) {
        color.r += (rand(TexCoord, seed) - 0.5) * uIntensity;
        color.g += (rand(TexCoord, seed + 1.0) - 0.5) * uIntensity;
        color.b += (rand(TexCoord, seed + 2.0) - 0.5) * uIntensity;
    } else {
        float grain = (rand(TexCoord, seed) - 0.5) * uIntensity;
        color.rgb += grain;
    }
    FragColor = clamp(color, 0.0, 1.0);
}
)";

// Helper: compile a shader program from VS + FS source
static ShaderRef MakeShader(const char* vs, const char* fs) {
    auto shader = std::make_shared<SadGraphics::ShaderProgram>(std::string(vs), std::string(fs));
    return shader->IsValid() ? shader : nullptr;
}

} // anonymous namespace

// ==============================================================================
// PostEffect - Base Implementation
// ==============================================================================

PostEffect::PostEffect(PostEffectType type) : m_type(type) {
    switch (type) {
        case PostEffectType::Bloom: m_name = "Bloom"; break;
        case PostEffectType::Blur: m_name = "Blur"; break;
        case PostEffectType::GaussianBlur: m_name = "GaussianBlur"; break;
        case PostEffectType::ChromaticAberration: m_name = "ChromaticAberration"; break;
        case PostEffectType::Vignette: m_name = "Vignette"; break;
        case PostEffectType::ColorGrading: m_name = "ColorGrading"; break;
        case PostEffectType::Pixelate: m_name = "Pixelate"; break;
        case PostEffectType::CRT: m_name = "CRT"; break;
        case PostEffectType::FilmGrain: m_name = "FilmGrain"; break;
        default: m_name = "Effect"; break;
    }
}

// ==============================================================================
// BloomEffect
// ==============================================================================

BloomEffect::BloomEffect() : PostEffect(PostEffectType::Bloom) {}

bool BloomEffect::Initialize(Int32 width, Int32 height) {
    Int32 w = width / 2;
    Int32 h = height / 2;
    
    for (Int32 i = 0; i < m_params.passes; ++i) {
        FramebufferSpec spec;
        spec.width = static_cast<u32>(std::max(1, w));
        spec.height = static_cast<u32>(std::max(1, h));
        m_blurFBOs.push_back(std::make_unique<Framebuffer>(spec));
        w /= 2;
        h /= 2;
    }
    
    m_extractShader = MakeShader(COPY_VS, BLOOM_EXTRACT_FS);
    m_blurShader = MakeShader(COPY_VS, BLUR_FS);
    m_combineShader = MakeShader(COPY_VS, BLOOM_COMBINE_FS);
    
    return m_extractShader && m_blurShader && m_combineShader;
}

void BloomEffect::Cleanup() {
    m_blurFBOs.clear();
    m_extractShader.reset();
    m_blurShader.reset();
    m_combineShader.reset();
}

void BloomEffect::Apply(UInt32 inputTexture, UInt32 outputFBO, Int32 width, Int32 height) {
    if (m_blurFBOs.empty() || !m_extractShader || !m_blurShader || !m_combineShader) return;
    
    // 1. Extract bright parts
    auto& fbo0 = m_blurFBOs[0];
    glBindFramebuffer(GL_FRAMEBUFFER, fbo0->GetFBO());
    glViewport(0, 0, static_cast<GLsizei>(fbo0->GetWidth()), static_cast<GLsizei>(fbo0->GetHeight()));
    glClear(GL_COLOR_BUFFER_BIT);
    m_extractShader->Use();
    m_extractShader->SetInt("uTexture", 0);
    m_extractShader->SetFloat("uThreshold", m_params.threshold);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // 2. Multi-pass blur on each mip level
    for (size_t i = 0; i < m_blurFBOs.size(); ++i) {
        auto& fbo = m_blurFBOs[i];
        glBindFramebuffer(GL_FRAMEBUFFER, fbo->GetFBO());
        glViewport(0, 0, static_cast<GLsizei>(fbo->GetWidth()), static_cast<GLsizei>(fbo->GetHeight()));
        
        m_blurShader->Use();
        m_blurShader->SetInt("uTexture", 0);
        m_blurShader->SetFloat("uRadius", m_params.radius);
        m_blurShader->SetFloat("uResolution", static_cast<Float32>(fbo->GetWidth()));
        
        // Horizontal
        m_blurShader->SetFloat2("uDirection", 1.0f, 0.0f);
        glActiveTexture(GL_TEXTURE0);
        UInt32 srcTex = (i == 0) ? fbo0->GetColorAttachmentID() : m_blurFBOs[i-1]->GetColorAttachmentID();
        glBindTexture(GL_TEXTURE_2D, srcTex);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        // Vertical
        m_blurShader->SetFloat2("uDirection", 0.0f, 1.0f);
        glBindTexture(GL_TEXTURE_2D, fbo->GetColorAttachmentID());
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    // 3. Combine bloom with scene
    glBindFramebuffer(GL_FRAMEBUFFER, outputFBO);
    glViewport(0, 0, width, height);
    m_combineShader->Use();
    m_combineShader->SetInt("uScene", 0);
    m_combineShader->SetInt("uBloom", 1);
    m_combineShader->SetFloat("uIntensity", m_params.intensity * m_intensity);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTexture);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, m_blurFBOs.back()->GetColorAttachmentID());
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glActiveTexture(GL_TEXTURE0);
}

// ==============================================================================
// BlurEffect
// ==============================================================================

BlurEffect::BlurEffect() : PostEffect(PostEffectType::Blur) {}

bool BlurEffect::Initialize(Int32 width, Int32 height) {
    FramebufferSpec spec;
    spec.width = static_cast<u32>(width);
    spec.height = static_cast<u32>(height);
    m_tempFBO = std::make_unique<Framebuffer>(spec);
    m_shader = MakeShader(COPY_VS, BLUR_FS);
    return m_shader != nullptr;
}

void BlurEffect::Apply(UInt32 inputTexture, UInt32 outputFBO, Int32 width, Int32 height) {
    if (!m_shader || !m_tempFBO) return;
    
    m_shader->Use();
    m_shader->SetInt("uTexture", 0);
    m_shader->SetFloat("uRadius", m_params.radius);
    m_shader->SetFloat("uResolution", static_cast<Float32>(width));
    
    // Horizontal pass -> temp FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_tempFBO->GetFBO());
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    m_shader->SetFloat2("uDirection", 1.0f, 0.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // Vertical pass -> output FBO
    glBindFramebuffer(GL_FRAMEBUFFER, outputFBO);
    glViewport(0, 0, width, height);
    m_shader->SetFloat2("uDirection", 0.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, m_tempFBO->GetColorAttachmentID());
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// ==============================================================================
// ChromaticAberrationEffect
// ==============================================================================

ChromaticAberrationEffect::ChromaticAberrationEffect() 
    : PostEffect(PostEffectType::ChromaticAberration) {}

void ChromaticAberrationEffect::Apply(UInt32 inputTexture, UInt32 outputFBO, 
                                       Int32 width, Int32 height) {
    if (!m_shader) {
        m_shader = MakeShader(COPY_VS, CHROMATIC_FS);
        if (!m_shader) return;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, outputFBO);
    glViewport(0, 0, width, height);
    m_shader->Use();
    m_shader->SetInt("uTexture", 0);
    m_shader->SetFloat("uIntensity", m_params.intensity * m_intensity * 0.01f);
    m_shader->SetFloat("uRedOffset", m_params.redOffset);
    m_shader->SetFloat("uBlueOffset", m_params.blueOffset);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// ==============================================================================
// VignetteEffect
// ==============================================================================

VignetteEffect::VignetteEffect() : PostEffect(PostEffectType::Vignette) {}

void VignetteEffect::Apply(UInt32 inputTexture, UInt32 outputFBO, 
                            Int32 width, Int32 height) {
    if (!m_shader) {
        m_shader = MakeShader(COPY_VS, VIGNETTE_FS);
        if (!m_shader) return;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, outputFBO);
    glViewport(0, 0, width, height);
    m_shader->Use();
    m_shader->SetInt("uTexture", 0);
    m_shader->SetFloat("uIntensity", m_params.intensity * m_intensity);
    m_shader->SetFloat("uRadius", m_params.radius);
    m_shader->SetFloat("uSmoothness", m_params.smoothness);
    m_shader->SetFloat4("uColor", m_params.color.r, m_params.color.g, 
                         m_params.color.b, m_params.color.a);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// ==============================================================================
// ColorGradingEffect
// ==============================================================================

ColorGradingEffect::ColorGradingEffect() : PostEffect(PostEffectType::ColorGrading) {}

void ColorGradingEffect::Apply(UInt32 inputTexture, UInt32 outputFBO, 
                                Int32 width, Int32 height) {
    if (!m_shader) {
        m_shader = MakeShader(COPY_VS, COLOR_GRADING_FS);
        if (!m_shader) return;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, outputFBO);
    glViewport(0, 0, width, height);
    m_shader->Use();
    m_shader->SetInt("uTexture", 0);
    m_shader->SetFloat("uBrightness", m_params.brightness);
    m_shader->SetFloat("uContrast", m_params.contrast);
    m_shader->SetFloat("uSaturation", m_params.saturation);
    m_shader->SetFloat("uGamma", m_params.gamma);
    m_shader->SetFloat3("uColorBalance", 
                         m_params.colorBalance.x, m_params.colorBalance.y, m_params.colorBalance.z);
    m_shader->SetFloat("uTemperature", m_params.temperature);
    m_shader->SetFloat("uTint", m_params.tint);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// ==============================================================================
// PixelateEffect
// ==============================================================================

PixelateEffect::PixelateEffect() : PostEffect(PostEffectType::Pixelate) {}

void PixelateEffect::Apply(UInt32 inputTexture, UInt32 outputFBO, 
                            Int32 width, Int32 height) {
    if (!m_shader) {
        m_shader = MakeShader(COPY_VS, PIXELATE_FS);
        if (!m_shader) return;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, outputFBO);
    glViewport(0, 0, width, height);
    m_shader->Use();
    m_shader->SetInt("uTexture", 0);
    m_shader->SetFloat("uPixelSize", m_params.pixelSize);
    m_shader->SetFloat2("uResolution", static_cast<Float32>(width), static_cast<Float32>(height));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// ==============================================================================
// CRTEffect
// ==============================================================================

CRTEffect::CRTEffect() : PostEffect(PostEffectType::CRT) {}

void CRTEffect::Apply(UInt32 inputTexture, UInt32 outputFBO, 
                       Int32 width, Int32 height) {
    if (!m_shader) {
        m_shader = MakeShader(COPY_VS, CRT_FS);
        if (!m_shader) return;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, outputFBO);
    glViewport(0, 0, width, height);
    m_shader->Use();
    m_shader->SetInt("uTexture", 0);
    m_shader->SetFloat("uCurvature", m_params.curvature);
    m_shader->SetFloat("uScanlineIntensity", m_params.scanlineIntensity);
    m_shader->SetFloat("uVignette", m_params.vignette);
    m_shader->SetFloat("uNoise", m_params.noise);
    m_shader->SetFloat("uFlickering", m_params.flickering);
    m_shader->SetFloat("uTime", m_time);
    m_shader->SetFloat4("uTint", m_params.tint.r, m_params.tint.g, 
                         m_params.tint.b, m_params.tint.a);
    m_shader->SetFloat2("uResolution", static_cast<Float32>(width), static_cast<Float32>(height));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// ==============================================================================
// FilmGrainEffect
// ==============================================================================

FilmGrainEffect::FilmGrainEffect() : PostEffect(PostEffectType::FilmGrain) {}

void FilmGrainEffect::Apply(UInt32 inputTexture, UInt32 outputFBO, 
                             Int32 width, Int32 height) {
    if (!m_shader) {
        m_shader = MakeShader(COPY_VS, FILM_GRAIN_FS);
        if (!m_shader) return;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, outputFBO);
    glViewport(0, 0, width, height);
    m_shader->Use();
    m_shader->SetInt("uTexture", 0);
    m_shader->SetFloat("uIntensity", m_params.intensity * m_intensity);
    m_shader->SetFloat("uTime", m_time);
    m_shader->SetInt("uColored", m_params.colored ? 1 : 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// ==============================================================================
// PostProcessingPipeline
// ==============================================================================

PostProcessingPipeline::PostProcessingPipeline() {}

PostProcessingPipeline::~PostProcessingPipeline() {
    Cleanup();
}

bool PostProcessingPipeline::Initialize(Int32 width, Int32 height) {
    if (m_initialized) {
        Cleanup();
    }
    
    m_width = width;
    m_height = height;
    
    // Create ping-pong FBOs
    FramebufferSpec spec;
    spec.width = static_cast<u32>(width);
    spec.height = static_cast<u32>(height);
    
    m_sourceFBO = std::make_unique<Framebuffer>(spec);
    m_pingFBO = std::make_unique<Framebuffer>(spec);
    m_pongFBO = std::make_unique<Framebuffer>(spec);
    
    // Create fullscreen quad
    InitQuad();
    
    // Create copy shader
    m_copyShader = MakeShader(COPY_VS, COPY_FS);
    if (!m_copyShader) return false;
    
    m_initialized = true;
    return true;
}

void PostProcessingPipeline::Resize(Int32 width, Int32 height) {
    m_width = width;
    m_height = height;
    
    // Resize FBOs
    if (m_sourceFBO) m_sourceFBO->Resize(static_cast<u32>(width), static_cast<u32>(height));
    if (m_pingFBO) m_pingFBO->Resize(static_cast<u32>(width), static_cast<u32>(height));
    if (m_pongFBO) m_pongFBO->Resize(static_cast<u32>(width), static_cast<u32>(height));
    
    // Reinitialize effects
    for (auto& effect : m_effects) {
        effect->Cleanup();
        effect->Initialize(width, height);
    }
}

void PostProcessingPipeline::Cleanup() {
    for (auto& effect : m_effects) {
        effect->Cleanup();
    }
    m_effects.clear();
    
    m_sourceFBO.reset();
    m_pingFBO.reset();
    m_pongFBO.reset();
    m_copyShader.reset();
    
    if (m_quadVAO) {
        glDeleteVertexArrays(1, &m_quadVAO);
        m_quadVAO = 0;
    }
    if (m_quadVBO) {
        glDeleteBuffers(1, &m_quadVBO);
        m_quadVBO = 0;
    }
    
    m_initialized = false;
}

PostEffect* PostProcessingPipeline::AddEffect(PostEffectType type) {
    std::unique_ptr<PostEffect> effect;
    
    switch (type) {
        case PostEffectType::Bloom:
            effect = std::make_unique<BloomEffect>();
            break;
        case PostEffectType::Blur:
        case PostEffectType::GaussianBlur:
            effect = std::make_unique<BlurEffect>();
            break;
        case PostEffectType::ChromaticAberration:
            effect = std::make_unique<ChromaticAberrationEffect>();
            break;
        case PostEffectType::Vignette:
            effect = std::make_unique<VignetteEffect>();
            break;
        case PostEffectType::ColorGrading:
            effect = std::make_unique<ColorGradingEffect>();
            break;
        case PostEffectType::Pixelate:
            effect = std::make_unique<PixelateEffect>();
            break;
        case PostEffectType::CRT:
            effect = std::make_unique<CRTEffect>();
            break;
        case PostEffectType::FilmGrain:
            effect = std::make_unique<FilmGrainEffect>();
            break;
        default:
            return nullptr;
    }
    
    effect->Initialize(m_width, m_height);
    PostEffect* ptr = effect.get();
    m_effects.push_back(std::move(effect));
    return ptr;
}

void PostProcessingPipeline::RemoveEffect(PostEffect* effect) {
    auto it = std::find_if(m_effects.begin(), m_effects.end(),
        [effect](const std::unique_ptr<PostEffect>& e) { 
            return e.get() == effect; 
        });
    
    if (it != m_effects.end()) {
        (*it)->Cleanup();
        m_effects.erase(it);
    }
}

void PostProcessingPipeline::ClearEffects() {
    for (auto& effect : m_effects) {
        effect->Cleanup();
    }
    m_effects.clear();
}

PostEffect* PostProcessingPipeline::GetEffect(Int32 index) {
    if (index < 0 || index >= static_cast<Int32>(m_effects.size())) {
        return nullptr;
    }
    return m_effects[index].get();
}

void PostProcessingPipeline::MoveEffect(Int32 from, Int32 to) {
    if (from < 0 || from >= static_cast<Int32>(m_effects.size())) return;
    if (to < 0 || to >= static_cast<Int32>(m_effects.size())) return;
    if (from == to) return;
    
    auto effect = std::move(m_effects[from]);
    m_effects.erase(m_effects.begin() + from);
    m_effects.insert(m_effects.begin() + to, std::move(effect));
}

void PostProcessingPipeline::Begin() {
    if (!m_enabled || !m_initialized || !m_sourceFBO) return;
    
    // Bind source FBO - all scene rendering goes here
    m_sourceFBO->Bind();
    glViewport(0, 0, m_width, m_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PostProcessingPipeline::End() {
    if (!m_enabled || !m_initialized) {
        Framebuffer::Unbind();
        return;
    }
    
    // Bind the quad VAO for all effect rendering
    glBindVertexArray(m_quadVAO);
    glDisable(GL_DEPTH_TEST);
    
    // Apply all enabled effects in ping-pong fashion
    bool usePing = true;
    UInt32 currentTexture = m_sourceFBO->GetColorAttachmentID();
    
    for (auto& effect : m_effects) {
        if (!effect->IsEnabled()) continue;
        
        UInt32 targetFBO = usePing ? m_pingFBO->GetFBO() : m_pongFBO->GetFBO();
        
        effect->Apply(currentTexture, targetFBO, m_width, m_height);
        
        currentTexture = usePing ? m_pingFBO->GetColorAttachmentID() : m_pongFBO->GetColorAttachmentID();
        usePing = !usePing;
    }
    
    // Render final result to screen (FBO 0)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_width, m_height);
    CopyTexture(currentTexture, 0);
    
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}

void PostProcessingPipeline::Update(Float32 deltaTime) {
    for (auto& effect : m_effects) {
        if (effect->IsEnabled()) {
            effect->Update(deltaTime);
        }
    }
}

UInt32 PostProcessingPipeline::GetSourceTexture() const {
    if (m_sourceFBO) return m_sourceFBO->GetColorAttachmentID();
    return 0;
}

void PostProcessingPipeline::InitQuad() {
    // Fullscreen quad vertices: position(2) + texcoord(2)
    Float32 quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    
    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    
    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    // Position attribute (location 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(Float32), (void*)0);
    
    // TexCoord attribute (location 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(Float32), (void*)(2 * sizeof(Float32)));
    
    glBindVertexArray(0);
}

void PostProcessingPipeline::RenderQuad() {
    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void PostProcessingPipeline::CopyTexture(UInt32 source, UInt32 destFBO) {
    if (!m_copyShader) return;
    
    glBindFramebuffer(GL_FRAMEBUFFER, destFBO);
    m_copyShader->Use();
    m_copyShader->SetInt("uTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, source);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// ==============================================================================
// Presets
// ==============================================================================

namespace PostProcessingPresets {

void ApplyCinematic(PostProcessingPipeline& pipeline) {
    pipeline.ClearEffects();
    
    auto* colorGrading = pipeline.AddEffect<ColorGradingEffect>();
    colorGrading->GetParams().contrast = 1.1f;
    colorGrading->GetParams().saturation = 0.9f;
    colorGrading->GetParams().temperature = -0.1f;
    
    auto* vignette = pipeline.AddEffect<VignetteEffect>();
    vignette->GetParams().intensity = 0.3f;
    vignette->GetParams().radius = 0.8f;
    
    auto* grain = pipeline.AddEffect<FilmGrainEffect>();
    grain->GetParams().intensity = 0.03f;
}

void ApplyRetro(PostProcessingPipeline& pipeline) {
    pipeline.ClearEffects();
    
    auto* crt = pipeline.AddEffect<CRTEffect>();
    crt->GetParams().curvature = 0.1f;
    crt->GetParams().scanlineIntensity = 0.4f;
    crt->GetParams().noise = 0.08f;
}

void ApplyDream(PostProcessingPipeline& pipeline) {
    pipeline.ClearEffects();
    
    auto* bloom = pipeline.AddEffect<BloomEffect>();
    bloom->GetParams().threshold = 0.5f;
    bloom->GetParams().intensity = 1.5f;
    
    auto* blur = pipeline.AddEffect<BlurEffect>();
    blur->GetParams().radius = 2.0f;
    
    auto* colorGrading = pipeline.AddEffect<ColorGradingEffect>();
    colorGrading->GetParams().saturation = 1.2f;
    colorGrading->GetParams().temperature = 0.2f;
}

void ApplyHorror(PostProcessingPipeline& pipeline) {
    pipeline.ClearEffects();
    
    auto* chromatic = pipeline.AddEffect<ChromaticAberrationEffect>();
    chromatic->GetParams().intensity = 2.0f;
    
    auto* vignette = pipeline.AddEffect<VignetteEffect>();
    vignette->GetParams().intensity = 0.6f;
    vignette->GetParams().radius = 0.6f;
    
    auto* colorGrading = pipeline.AddEffect<ColorGradingEffect>();
    colorGrading->GetParams().saturation = 0.6f;
    colorGrading->GetParams().temperature = -0.3f;
    colorGrading->GetParams().contrast = 1.2f;
    
    auto* grain = pipeline.AddEffect<FilmGrainEffect>();
    grain->GetParams().intensity = 0.1f;
}

void ApplyBlackAndWhite(PostProcessingPipeline& pipeline) {
    pipeline.ClearEffects();
    
    auto* colorGrading = pipeline.AddEffect<ColorGradingEffect>();
    colorGrading->GetParams().saturation = 0.0f;
    colorGrading->GetParams().contrast = 1.1f;
    
    auto* vignette = pipeline.AddEffect<VignetteEffect>();
    vignette->GetParams().intensity = 0.2f;
}

} // namespace PostProcessingPresets

} // namespace graphics
} // namespace sad
