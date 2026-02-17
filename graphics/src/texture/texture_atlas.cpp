// ==============================================================================
// texture_atlas.cpp - تنفيذ نظام أطلس النسيج
// Texture Atlas System Implementation
// ==============================================================================

#include "../../include/texture/texture_atlas.h"
#include "../../include/texture/image.h"
#include <algorithm>
#include <sstream>

namespace sad {
namespace graphics {

// ==============================================================================
// المُنشئات والمُدمر / Constructors and Destructor
// ==============================================================================

TextureAtlas::TextureAtlas() : m_texture(nullptr) {}

TextureAtlas::TextureAtlas(const TextureRef& texture) : m_texture(texture) {}

TextureAtlas::~TextureAtlas() = default;

// ==============================================================================
// تحميل الأطلس / Loading Atlas
// ==============================================================================

bool TextureAtlas::LoadFromFile(const std::string& filepath) {
    auto texture = Texture::CreateFromFile(filepath, true, false);
    if (!texture || !texture->IsValid()) {
        return false;
    }
    m_texture = texture;
    return true;
}

void TextureAtlas::SetTexture(const TextureRef& texture) {
    m_texture = texture;
    // إعادة حساب UV لجميع المناطق / Recalculate UV for all regions
    for (auto& pair : m_regions) {
        CalculateUV(pair.second);
    }
}

// ==============================================================================
// إدارة المناطق / Region Management
// ==============================================================================

void TextureAtlas::AddRegion(const std::string& name, Float32 x, Float32 y,
                              Float32 width, Float32 height) {
    AddRegion(name, x, y, width, height, 0.5f, 0.5f);
}

void TextureAtlas::AddRegion(const std::string& name, Float32 x, Float32 y,
                              Float32 width, Float32 height,
                              Float32 pivotX, Float32 pivotY) {
    AtlasRegion region;
    region.name = name;
    region.x = x;
    region.y = y;
    region.width = width;
    region.height = height;
    region.pivotX = pivotX;
    region.pivotY = pivotY;
    region.rotated = false;
    
    CalculateUV(region);
    m_regions[name] = region;
}

void TextureAtlas::RemoveRegion(const std::string& name) {
    m_regions.erase(name);
}

const AtlasRegion* TextureAtlas::GetRegion(const std::string& name) const {
    auto it = m_regions.find(name);
    if (it != m_regions.end()) {
        return &it->second;
    }
    return nullptr;
}

bool TextureAtlas::HasRegion(const std::string& name) const {
    return m_regions.find(name) != m_regions.end();
}

std::vector<std::string> TextureAtlas::GetRegionNames() const {
    std::vector<std::string> names;
    names.reserve(m_regions.size());
    for (const auto& pair : m_regions) {
        names.push_back(pair.first);
    }
    return names;
}

// ==============================================================================
// تقسيم شبكي / Grid Slicing
// ==============================================================================

void TextureAtlas::SliceGrid(const std::string& prefix, Int32 cols, Int32 rows,
                              Float32 padding) {
    if (!m_texture || !m_texture->IsValid() || cols <= 0 || rows <= 0) return;
    
    Float32 texW = static_cast<Float32>(m_texture->GetWidth());
    Float32 texH = static_cast<Float32>(m_texture->GetHeight());
    
    Float32 cellW = (texW - padding * (cols - 1)) / cols;
    Float32 cellH = (texH - padding * (rows - 1)) / rows;
    
    Int32 index = 0;
    for (Int32 row = 0; row < rows; ++row) {
        for (Int32 col = 0; col < cols; ++col) {
            Float32 x = col * (cellW + padding);
            Float32 y = row * (cellH + padding);
            
            // تسمية: prefix_00, prefix_01, ... / Naming: prefix_00, prefix_01, ...
            std::ostringstream oss;
            oss << prefix;
            if (index < 10) oss << "0";
            oss << index;
            
            AddRegion(oss.str(), x, y, cellW, cellH);
            ++index;
        }
    }
}

void TextureAtlas::SliceBySize(const std::string& prefix, Float32 cellWidth, Float32 cellHeight,
                                Float32 padding) {
    if (!m_texture || !m_texture->IsValid() || cellWidth <= 0 || cellHeight <= 0) return;
    
    Float32 texW = static_cast<Float32>(m_texture->GetWidth());
    Float32 texH = static_cast<Float32>(m_texture->GetHeight());
    
    Int32 cols = static_cast<Int32>((texW + padding) / (cellWidth + padding));
    Int32 rows = static_cast<Int32>((texH + padding) / (cellHeight + padding));
    
    Int32 index = 0;
    for (Int32 row = 0; row < rows; ++row) {
        for (Int32 col = 0; col < cols; ++col) {
            Float32 x = col * (cellWidth + padding);
            Float32 y = row * (cellHeight + padding);
            
            // لا نتجاوز حدود الـ texture / Don't exceed texture bounds
            if (x + cellWidth > texW || y + cellHeight > texH) continue;
            
            std::ostringstream oss;
            oss << prefix;
            if (index < 10) oss << "0";
            oss << index;
            
            AddRegion(oss.str(), x, y, cellWidth, cellHeight);
            ++index;
        }
    }
}

// ==============================================================================
// الرسوم المتحركة / Animations
// ==============================================================================

void TextureAtlas::DefineAnimation(const std::string& animName, const std::string& regionPrefix,
                                    Int32 frameCount, Float32 frameDuration, bool loop) {
    AnimationDef anim;
    anim.name = animName;
    anim.loop = loop;
    anim.frames.reserve(frameCount);
    
    for (Int32 i = 0; i < frameCount; ++i) {
        AnimationFrame frame;
        
        std::ostringstream oss;
        oss << regionPrefix;
        if (i < 10) oss << "0";
        oss << i;
        frame.regionName = oss.str();
        frame.duration = frameDuration;
        
        // تحقق من وجود المنطقة / Verify region exists
        if (HasRegion(frame.regionName)) {
            anim.frames.push_back(frame);
        }
    }
    
    if (!anim.frames.empty()) {
        m_animations[animName] = anim;
    }
}

void TextureAtlas::DefineAnimation(const std::string& animName,
                                    const std::vector<AnimationFrame>& frames, bool loop) {
    AnimationDef anim;
    anim.name = animName;
    anim.frames = frames;
    anim.loop = loop;
    m_animations[animName] = anim;
}

const AnimationDef* TextureAtlas::GetAnimation(const std::string& name) const {
    auto it = m_animations.find(name);
    if (it != m_animations.end()) {
        return &it->second;
    }
    return nullptr;
}

// ==============================================================================
// إنشاء ثابت / Static Creation
// ==============================================================================

std::shared_ptr<TextureAtlas> TextureAtlas::CreateFromFile(const std::string& filepath) {
    auto atlas = std::make_shared<TextureAtlas>();
    if (atlas->LoadFromFile(filepath)) {
        return atlas;
    }
    return nullptr;
}

std::shared_ptr<TextureAtlas> TextureAtlas::CreateFromTexture(const TextureRef& texture) {
    return std::make_shared<TextureAtlas>(texture);
}

// ==============================================================================
// دوال مساعدة / Helper Functions
// ==============================================================================

void TextureAtlas::CalculateUV(AtlasRegion& region) const {
    if (!m_texture || !m_texture->IsValid()) {
        region.u0 = region.v0 = 0.0f;
        region.u1 = region.v1 = 1.0f;
        return;
    }
    
    Float32 texW = static_cast<Float32>(m_texture->GetWidth());
    Float32 texH = static_cast<Float32>(m_texture->GetHeight());
    
    region.u0 = region.x / texW;
    region.v0 = region.y / texH;
    region.u1 = (region.x + region.width) / texW;
    region.v1 = (region.y + region.height) / texH;
}

} // namespace graphics
} // namespace sad
