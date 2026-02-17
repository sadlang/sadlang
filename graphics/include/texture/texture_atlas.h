// ==============================================================================
// texture_atlas.h - نظام أطلس النسيج وصفحات الرسوم المتحركة
// Texture Atlas & Spritesheet System
// ==============================================================================
// الوصف: إدارة مناطق متعددة داخل texture واحد لأداء أفضل
// Description: Manage multiple regions within a single texture for better performance
// ==============================================================================

#pragma once

#include "../core/types.h"
#include "texture.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

namespace sad {
namespace graphics {

// ==============================================================================
// منطقة داخل أطلس / Region within an atlas
// ==============================================================================
struct AtlasRegion {
    std::string name;      // اسم المنطقة / Region name
    Float32 x, y;          // موقع البداية بالبكسل / Start position in pixels
    Float32 width, height; // الأبعاد بالبكسل / Dimensions in pixels
    Float32 u0, v0;        // إحداثيات UV العلوية اليسرى / Top-left UV coordinates
    Float32 u1, v1;        // إحداثيات UV السفلية اليمنى / Bottom-right UV coordinates
    
    // هل المنطقة مقلوبة (لبعض صيغ التعبئة) / Is region rotated (for some packing formats)
    bool rotated = false;
    
    // بيانات المحور المركزي (للرسوم المتحركة) / Pivot data (for sprites)
    Float32 pivotX = 0.5f;
    Float32 pivotY = 0.5f;
};

// ==============================================================================
// إطار رسم متحرك / Animation Frame
// ==============================================================================
struct AnimationFrame {
    std::string regionName;   // اسم المنطقة / Region name
    Float32 duration;         // مدة الإطار بالثواني / Frame duration in seconds
};

// ==============================================================================
// تعريف الرسم المتحرك / Animation Definition
// ==============================================================================
struct AnimationDef {
    std::string name;                       // اسم الرسم المتحرك / Animation name
    std::vector<AnimationFrame> frames;     // إطارات الرسم المتحرك / Animation frames
    bool loop = true;                       // هل يتكرر / Should loop
};

// ==============================================================================
// فئة أطلس النسيج / Texture Atlas Class
// ==============================================================================
class TextureAtlas {
public:
    // المُنشئ الافتراضي / Default constructor
    TextureAtlas();
    
    // المُنشئ من texture موجود / Constructor from existing texture
    // texture: الـ texture الأساسي / Base texture
    explicit TextureAtlas(const TextureRef& texture);
    
    // المدمر / Destructor
    ~TextureAtlas();
    
    // ==============================================================================
    // تحميل الأطلس / Loading Atlas
    // ==============================================================================
    
    /// تحميل أطلس من ملف صورة
    /// Load atlas from image file
    /// filepath: مسار ملف الصورة / Image file path
    /// returns: نجاح التحميل / Load success
    bool LoadFromFile(const std::string& filepath);
    
    /// تعيين texture الأطلس
    /// Set atlas texture
    /// texture: الـ texture الجديد / New texture
    void SetTexture(const TextureRef& texture);
    
    /// الحصول على texture الأطلس
    /// Get atlas texture
    TextureRef GetTexture() const { return m_texture; }
    
    // ==============================================================================
    // إدارة المناطق / Region Management
    // ==============================================================================
    
    /// إضافة منطقة للأطلس
    /// Add region to atlas
    /// name: اسم المنطقة / Region name
    /// x, y: موقع البداية / Start position  
    /// width, height: أبعاد المنطقة / Region dimensions
    void AddRegion(const std::string& name, Float32 x, Float32 y,
                   Float32 width, Float32 height);
    
    /// إضافة منطقة مع محور مركزي مخصص
    /// Add region with custom pivot
    void AddRegion(const std::string& name, Float32 x, Float32 y,
                   Float32 width, Float32 height,
                   Float32 pivotX, Float32 pivotY);
    
    /// إزالة منطقة / Remove region
    /// name: اسم المنطقة / Region name
    void RemoveRegion(const std::string& name);
    
    /// الحصول على منطقة بالاسم / Get region by name
    /// name: اسم المنطقة / Region name
    /// returns: مؤشر للمنطقة (nullptr إذا لم توجد) / Pointer to region (nullptr if not found)
    const AtlasRegion* GetRegion(const std::string& name) const;
    
    /// التحقق من وجود منطقة / Check if region exists
    bool HasRegion(const std::string& name) const;
    
    /// الحصول على جميع أسماء المناطق / Get all region names
    std::vector<std::string> GetRegionNames() const;
    
    /// عدد المناطق / Number of regions
    size_t GetRegionCount() const { return m_regions.size(); }
    
    // ==============================================================================
    // تقسيم شبكي (Spritesheet) / Grid Slicing (Spritesheet)
    // ==============================================================================
    
    /// تقسيم الأطلس إلى شبكة منتظمة
    /// Slice atlas into uniform grid
    /// prefix: بادئة أسماء المناطق / Region name prefix
    /// cols: عدد الأعمدة / Number of columns
    /// rows: عدد الصفوف / Number of rows
    /// padding: المسافة بين المناطق / Padding between regions
    void SliceGrid(const std::string& prefix, Int32 cols, Int32 rows,
                   Float32 padding = 0.0f);
    
    /// تقسيم بأبعاد محددة للخلية
    /// Slice with specific cell dimensions
    /// prefix: بادئة أسماء المناطق / Region name prefix
    /// cellWidth, cellHeight: أبعاد الخلية الواحدة / Cell dimensions
    /// padding: المسافة بين المناطق / Padding between regions
    void SliceBySize(const std::string& prefix, Float32 cellWidth, Float32 cellHeight,
                     Float32 padding = 0.0f);
    
    // ==============================================================================
    // الرسوم المتحركة / Animations
    // ==============================================================================
    
    /// تعريف رسم متحرك من مناطق متتالية
    /// Define animation from sequential regions
    /// animName: اسم الرسم المتحرك / Animation name
    /// regionPrefix: بادئة المناطق / Region prefix (e.g., "walk_")
    /// frameCount: عدد الإطارات / Frame count
    /// frameDuration: مدة كل إطار بالثواني / Duration per frame in seconds
    /// loop: هل يتكرر / Should loop
    void DefineAnimation(const std::string& animName, const std::string& regionPrefix,
                         Int32 frameCount, Float32 frameDuration = 0.1f, bool loop = true);
    
    /// تعريف رسم متحرك من قائمة مناطق
    /// Define animation from region list
    void DefineAnimation(const std::string& animName,
                         const std::vector<AnimationFrame>& frames, bool loop = true);
    
    /// الحصول على رسم متحرك / Get animation definition
    const AnimationDef* GetAnimation(const std::string& name) const;
    
    /// عدد الرسوم المتحركة / Number of animations
    size_t GetAnimationCount() const { return m_animations.size(); }
    
    // ==============================================================================
    // إنشاء ثابت / Static Creation
    // ==============================================================================
    
    /// إنشاء أطلس من ملف صورة
    /// Create atlas from image file
    static std::shared_ptr<TextureAtlas> CreateFromFile(const std::string& filepath);
    
    /// إنشاء أطلس من texture
    /// Create atlas from texture
    static std::shared_ptr<TextureAtlas> CreateFromTexture(const TextureRef& texture);

private:
    TextureRef m_texture;                                              // نسيج الأطلس / Atlas texture
    std::unordered_map<std::string, AtlasRegion> m_regions;            // المناطق / Regions
    std::unordered_map<std::string, AnimationDef> m_animations;        // الرسوم المتحركة / Animations
    
    // حساب إحداثيات UV من إحداثيات البكسل / Calculate UV from pixel coordinates
    void CalculateUV(AtlasRegion& region) const;
};

// أنواع مستعارة / Type aliases
using TextureAtlasRef = std::shared_ptr<TextureAtlas>;

} // namespace graphics
} // namespace sad

// اختصار لسهولة الوصول / Shortcut for convenience
namespace SadGraphics {
    using TextureAtlas = sad::graphics::TextureAtlas;
    using TextureAtlasRef = sad::graphics::TextureAtlasRef;
    using AtlasRegion = sad::graphics::AtlasRegion;
    using AnimationDef = sad::graphics::AnimationDef;
    using AnimationFrame = sad::graphics::AnimationFrame;
}
