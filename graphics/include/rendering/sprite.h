// ==============================================================================
// sprite.h - نظام الرسوم المتحركة / Sprite & Animation System
// ==============================================================================
// الوصف: نظام رسوم متحركة متقدم مع دعم أطلس النسيج والإطارات
// Description: Advanced sprite animation system with atlas and frame support
// ==============================================================================

#pragma once

#include "../core/types.h"
#include "../texture/texture.h"
#include "../texture/texture_atlas.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

namespace sad {
namespace graphics {

// ==============================================================================
// حالة الرسم المتحرك / Animation State
// ==============================================================================
enum class AnimationState {
    Stopped,     // متوقف / Stopped
    Playing,     // يعمل / Playing
    Paused       // متوقف مؤقتاً / Paused
};

// ==============================================================================
// معلومات الإطار / Frame Info
// ==============================================================================
struct SpriteFrame {
    Float32 u0, v0, u1, v1;      // إحداثيات UV / UV coordinates
    Float32 width, height;        // أبعاد الإطار الأصلي / Original frame dimensions
    Float32 duration;             // مدة الإطار بالثواني / Frame duration in seconds
    Float32 pivotX = 0.5f;        // المحور X (0-1) / Pivot X (0-1)
    Float32 pivotY = 0.5f;        // المحور Y (0-1) / Pivot Y (0-1)
};

// ==============================================================================
// رسم متحرك مسمى / Named Animation
// ==============================================================================
struct SpriteAnimation {
    std::string name;                       // الاسم / Name
    std::vector<SpriteFrame> frames;        // الإطارات / Frames
    bool loop = true;                       // تكرار / Loop
    Float32 speed = 1.0f;                   // عامل السرعة / Speed multiplier
};

// ==============================================================================
// فئة الرسم المتحرك / Sprite Class
// ==============================================================================
class Sprite {
public:
    // الإنشاء / Construction
    Sprite();
    ~Sprite() = default;
    
    // ==============================================================================
    // الموقع والتحويل / Position and Transform
    // ==============================================================================
    
    /// تعيين الموقع / Set position
    void SetPosition(Float32 x, Float32 y);
    void SetPosition(const Vec2& pos);
    Vec2 GetPosition() const { return m_position; }
    
    /// تعيين الأبعاد / Set size
    void SetSize(Float32 w, Float32 h);
    Vec2 GetSize() const { return m_size; }
    
    /// تعيين القياس / Set scale
    void SetScale(Float32 sx, Float32 sy);
    void SetScale(Float32 uniform);
    Vec2 GetScale() const { return m_scale; }
    
    /// تعيين الدوران (بالراديان) / Set rotation (radians)
    void SetRotation(Float32 angle);
    Float32 GetRotation() const { return m_rotation; }
    
    /// تعيين نقطة المحور (0-1) / Set origin/pivot (0-1)
    void SetOrigin(Float32 x, Float32 y);
    Vec2 GetOrigin() const { return m_origin; }
    
    /// تعيين لون التلوين / Set tint color
    void SetColor(const Color& c);
    Color GetColor() const { return m_color; }
    
    /// تعيين الشفافية / Set opacity
    void SetAlpha(Float32 alpha);
    Float32 GetAlpha() const { return m_color.a; }
    
    /// إظهار/إخفاء / Show/Hide
    void SetVisible(bool vis) { m_visible = vis; }
    bool IsVisible() const { return m_visible; }
    
    /// قلب أفقي/رأسي / Flip horizontal/vertical
    void SetFlipX(bool flip) { m_flipX = flip; }
    void SetFlipY(bool flip) { m_flipY = flip; }
    bool IsFlippedX() const { return m_flipX; }
    bool IsFlippedY() const { return m_flipY; }
    
    // ==============================================================================
    // النسيج / Texture
    // ==============================================================================
    
    /// تعيين النسيج / Set texture
    void SetTexture(const TextureRef& texture);
    TextureRef GetTexture() const { return m_texture; }
    
    /// تعيين منطقة النسيج (UV يدوي) / Set texture region (manual UV)
    void SetTextureRect(Float32 u0, Float32 v0, Float32 u1, Float32 v1);
    
    /// تعيين من أطلس / Set from atlas region
    void SetAtlasRegion(const TextureAtlasRef& atlas, const std::string& regionName);
    
    // ==============================================================================
    // الرسوم المتحركة / Animation
    // ==============================================================================
    
    /// إضافة رسم متحرك من أطلس
    /// Add animation from atlas
    /// name: اسم الرسم المتحرك / Animation name
    /// atlas: أطلس النسيج / Texture atlas
    /// regionPrefix: بادئة المناطق / Region prefix
    /// frameCount: عدد الإطارات / Frame count
    /// frameDuration: مدة كل إطار / Duration per frame
    /// loop: تكرار / Loop
    void AddAnimation(const std::string& name, const TextureAtlasRef& atlas,
                      const std::string& regionPrefix, Int32 frameCount,
                      Float32 frameDuration = 0.1f, bool loop = true);
    
    /// إضافة رسم متحرك من إطارات يدوية
    /// Add animation from manual frames
    void AddAnimation(const std::string& name, const SpriteAnimation& anim);
    
    /// تشغيل رسم متحرك / Play animation
    /// name: اسم الرسم المتحرك / Animation name
    /// restart: إعادة البدء إذا كان نفسه / Restart if same
    void Play(const std::string& name, bool restart = false);
    
    /// إيقاف مؤقت / Pause
    void Pause();
    
    /// استئناف / Resume
    void Resume();
    
    /// إيقاف / Stop
    void Stop();
    
    /// الحصول على الحالة / Get state
    AnimationState GetAnimationState() const { return m_animState; }
    
    /// الحصول على اسم الرسم المتحرك الحالي / Get current animation name
    const std::string& GetCurrentAnimation() const { return m_currentAnimName; }
    
    /// الحصول على رقم الإطار الحالي / Get current frame index
    Int32 GetCurrentFrame() const { return m_currentFrame; }
    
    /// تعيين سرعة الرسم المتحرك / Set animation speed
    void SetAnimationSpeed(Float32 speed) { m_animSpeed = speed; }
    Float32 GetAnimationSpeed() const { return m_animSpeed; }
    
    // ==============================================================================
    // التحديث / Update
    // ==============================================================================
    
    /// تحديث الرسم المتحرك / Update animation
    /// deltaTime: الوقت المنقضي / Elapsed time
    void Update(Float32 deltaTime);
    
    // ==============================================================================
    // معلومات الرسم / Rendering Info
    // ==============================================================================
    
    /// الحصول على إحداثيات UV الحالية / Get current UV coordinates
    void GetCurrentUV(Float32& u0, Float32& v0, Float32& u1, Float32& v1) const;
    
    /// الحصول على مستطيل الحدود / Get bounding rect
    Rect GetBounds() const;
    
    /// التحقق من التقاطع مع نقطة / Check point intersection
    bool ContainsPoint(Float32 x, Float32 y) const;
    bool ContainsPoint(const Vec2& point) const;
    
    // ==============================================================================
    // إنشاء ثابت / Static Creation
    // ==============================================================================
    
    static std::shared_ptr<Sprite> Create(const TextureRef& texture);
    static std::shared_ptr<Sprite> CreateFromAtlas(const TextureAtlasRef& atlas,
                                                    const std::string& regionName);

private:
    // التحويل / Transform
    Vec2 m_position;
    Vec2 m_size;
    Vec2 m_scale;
    Vec2 m_origin;
    Float32 m_rotation;
    Color m_color;
    bool m_visible;
    bool m_flipX, m_flipY;
    
    // النسيج / Texture
    TextureRef m_texture;
    Float32 m_u0, m_v0, m_u1, m_v1;
    
    // الرسوم المتحركة / Animations
    std::unordered_map<std::string, SpriteAnimation> m_animations;
    std::string m_currentAnimName;
    Int32 m_currentFrame;
    Float32 m_frameTimer;
    Float32 m_animSpeed;
    AnimationState m_animState;
};

// أنواع مستعارة / Type aliases
using SpriteRef = std::shared_ptr<Sprite>;

} // namespace graphics
} // namespace sad

// اختصار / Shortcut
namespace SadGraphics {
    using Sprite = sad::graphics::Sprite;
    using SpriteRef = sad::graphics::SpriteRef;
    using SpriteAnimation = sad::graphics::SpriteAnimation;
    using SpriteFrame = sad::graphics::SpriteFrame;
    using AnimationState = sad::graphics::AnimationState;
}
