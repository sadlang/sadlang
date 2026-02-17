// ==============================================================================
// sprite.cpp - تنفيذ نظام الرسوم المتحركة
// Sprite & Animation System Implementation
// ==============================================================================

#include "../../include/rendering/sprite.h"
#include <cmath>
#include <sstream>

namespace sad {
namespace graphics {

// ==============================================================================
// المُنشئ / Constructor
// ==============================================================================

Sprite::Sprite()
    : m_position(0.0f, 0.0f)
    , m_size(0.0f, 0.0f)
    , m_scale(1.0f, 1.0f)
    , m_origin(0.5f, 0.5f)
    , m_rotation(0.0f)
    , m_color(Color::White)
    , m_visible(true)
    , m_flipX(false)
    , m_flipY(false)
    , m_texture(nullptr)
    , m_u0(0.0f), m_v0(0.0f)
    , m_u1(1.0f), m_v1(1.0f)
    , m_currentFrame(0)
    , m_frameTimer(0.0f)
    , m_animSpeed(1.0f)
    , m_animState(AnimationState::Stopped)
{
}

// ==============================================================================
// الموقع والتحويل / Position and Transform
// ==============================================================================

void Sprite::SetPosition(Float32 x, Float32 y) {
    m_position.x = x;
    m_position.y = y;
}

void Sprite::SetPosition(const Vec2& pos) {
    m_position = pos;
}

void Sprite::SetSize(Float32 w, Float32 h) {
    m_size.x = w;
    m_size.y = h;
}

void Sprite::SetScale(Float32 sx, Float32 sy) {
    m_scale.x = sx;
    m_scale.y = sy;
}

void Sprite::SetScale(Float32 uniform) {
    m_scale.x = uniform;
    m_scale.y = uniform;
}

void Sprite::SetRotation(Float32 angle) {
    m_rotation = angle;
}

void Sprite::SetOrigin(Float32 x, Float32 y) {
    m_origin.x = x;
    m_origin.y = y;
}

void Sprite::SetColor(const Color& c) {
    m_color = c;
}

void Sprite::SetAlpha(Float32 alpha) {
    m_color.a = alpha;
}

// ==============================================================================
// النسيج / Texture
// ==============================================================================

void Sprite::SetTexture(const TextureRef& texture) {
    m_texture = texture;
    if (texture && texture->IsValid()) {
        // تعيين الأبعاد تلقائياً إذا لم تُعيّن / Auto-set size if not set
        if (m_size.x == 0.0f && m_size.y == 0.0f) {
            m_size.x = static_cast<Float32>(texture->GetWidth());
            m_size.y = static_cast<Float32>(texture->GetHeight());
        }
    }
}

void Sprite::SetTextureRect(Float32 u0, Float32 v0, Float32 u1, Float32 v1) {
    m_u0 = u0;
    m_v0 = v0;
    m_u1 = u1;
    m_v1 = v1;
}

void Sprite::SetAtlasRegion(const TextureAtlasRef& atlas, const std::string& regionName) {
    if (!atlas) return;
    
    m_texture = atlas->GetTexture();
    
    const AtlasRegion* region = atlas->GetRegion(regionName);
    if (region) {
        m_u0 = region->u0;
        m_v0 = region->v0;
        m_u1 = region->u1;
        m_v1 = region->v1;
        m_size.x = region->width;
        m_size.y = region->height;
        m_origin.x = region->pivotX;
        m_origin.y = region->pivotY;
    }
}

// ==============================================================================
// الرسوم المتحركة / Animation
// ==============================================================================

void Sprite::AddAnimation(const std::string& name, const TextureAtlasRef& atlas,
                           const std::string& regionPrefix, Int32 frameCount,
                           Float32 frameDuration, bool loop) {
    if (!atlas) return;
    
    SpriteAnimation anim;
    anim.name = name;
    anim.loop = loop;
    anim.frames.reserve(frameCount);
    
    for (Int32 i = 0; i < frameCount; ++i) {
        std::ostringstream oss;
        oss << regionPrefix;
        if (i < 10) oss << "0";
        oss << i;
        
        const AtlasRegion* region = atlas->GetRegion(oss.str());
        if (region) {
            SpriteFrame frame;
            frame.u0 = region->u0;
            frame.v0 = region->v0;
            frame.u1 = region->u1;
            frame.v1 = region->v1;
            frame.width = region->width;
            frame.height = region->height;
            frame.duration = frameDuration;
            frame.pivotX = region->pivotX;
            frame.pivotY = region->pivotY;
            anim.frames.push_back(frame);
        }
    }
    
    if (!anim.frames.empty()) {
        m_animations[name] = anim;
    }
}

void Sprite::AddAnimation(const std::string& name, const SpriteAnimation& anim) {
    m_animations[name] = anim;
    m_animations[name].name = name;
}

void Sprite::Play(const std::string& name, bool restart) {
    if (m_currentAnimName == name && !restart && m_animState == AnimationState::Playing) {
        return; // بالفعل يعمل / Already playing
    }
    
    auto it = m_animations.find(name);
    if (it == m_animations.end()) return;
    
    m_currentAnimName = name;
    m_currentFrame = 0;
    m_frameTimer = 0.0f;
    m_animState = AnimationState::Playing;
    
    // تطبيق الإطار الأول / Apply first frame
    if (!it->second.frames.empty()) {
        const SpriteFrame& f = it->second.frames[0];
        m_u0 = f.u0;
        m_v0 = f.v0;
        m_u1 = f.u1;
        m_v1 = f.v1;
    }
}

void Sprite::Pause() {
    if (m_animState == AnimationState::Playing) {
        m_animState = AnimationState::Paused;
    }
}

void Sprite::Resume() {
    if (m_animState == AnimationState::Paused) {
        m_animState = AnimationState::Playing;
    }
}

void Sprite::Stop() {
    m_animState = AnimationState::Stopped;
    m_currentFrame = 0;
    m_frameTimer = 0.0f;
}

// ==============================================================================
// التحديث / Update
// ==============================================================================

void Sprite::Update(Float32 deltaTime) {
    if (m_animState != AnimationState::Playing) return;
    
    auto it = m_animations.find(m_currentAnimName);
    if (it == m_animations.end() || it->second.frames.empty()) return;
    
    const SpriteAnimation& anim = it->second;
    
    m_frameTimer += deltaTime * m_animSpeed * anim.speed;
    
    const SpriteFrame& currentFrame = anim.frames[m_currentFrame];
    
    if (m_frameTimer >= currentFrame.duration) {
        m_frameTimer -= currentFrame.duration;
        m_currentFrame++;
        
        if (m_currentFrame >= static_cast<Int32>(anim.frames.size())) {
            if (anim.loop) {
                m_currentFrame = 0;
            } else {
                m_currentFrame = static_cast<Int32>(anim.frames.size()) - 1;
                m_animState = AnimationState::Stopped;
            }
        }
        
        // تطبيق الإطار الجديد / Apply new frame
        const SpriteFrame& f = anim.frames[m_currentFrame];
        m_u0 = f.u0;
        m_v0 = f.v0;
        m_u1 = f.u1;
        m_v1 = f.v1;
    }
}

// ==============================================================================
// معلومات الرسم / Rendering Info
// ==============================================================================

void Sprite::GetCurrentUV(Float32& u0, Float32& v0, Float32& u1, Float32& v1) const {
    if (m_flipX) {
        u0 = m_u1;
        u1 = m_u0;
    } else {
        u0 = m_u0;
        u1 = m_u1;
    }
    
    if (m_flipY) {
        v0 = m_v1;
        v1 = m_v0;
    } else {
        v0 = m_v0;
        v1 = m_v1;
    }
}

Rect Sprite::GetBounds() const {
    Float32 w = m_size.x * m_scale.x;
    Float32 h = m_size.y * m_scale.y;
    Float32 ox = m_origin.x * w;
    Float32 oy = m_origin.y * h;
    
    return Rect(m_position.x - ox, m_position.y - oy, w, h);
}

bool Sprite::ContainsPoint(Float32 x, Float32 y) const {
    Rect bounds = GetBounds();
    return x >= bounds.x && x <= bounds.x + bounds.width &&
           y >= bounds.y && y <= bounds.y + bounds.height;
}

bool Sprite::ContainsPoint(const Vec2& point) const {
    return ContainsPoint(point.x, point.y);
}

// ==============================================================================
// إنشاء ثابت / Static Creation
// ==============================================================================

std::shared_ptr<Sprite> Sprite::Create(const TextureRef& texture) {
    auto sprite = std::make_shared<Sprite>();
    sprite->SetTexture(texture);
    return sprite;
}

std::shared_ptr<Sprite> Sprite::CreateFromAtlas(const TextureAtlasRef& atlas,
                                                  const std::string& regionName) {
    auto sprite = std::make_shared<Sprite>();
    sprite->SetAtlasRegion(atlas, regionName);
    return sprite;
}

} // namespace graphics
} // namespace sad
