// ==============================================================================
// resource_manager.h - نظام إدارة الموارد / Resource Management System
// ==============================================================================
// الوصف: نظام مركزي لتحميل وإدارة وتخزين الموارد (textures, fonts, shaders)
// Description: Central system for loading, managing, and caching resources
// ==============================================================================

#ifndef SAD_GRAPHICS_RESOURCES_RESOURCE_MANAGER_H
#define SAD_GRAPHICS_RESOURCES_RESOURCE_MANAGER_H

#include "../core/types.h"               // الأنواع الأساسية / Basic types
#include "../texture/texture.h"          // نظام الـ textures / Texture system
#include "../text/font.h"                // نظام الخطوط / Font system
#include "../rendering/shader.h"         // نظام التظليل / Shader system

#include <memory>                        // للمؤشرات الذكية / For smart pointers
#include <string>                        // للنصوص / For strings
#include <unordered_map>                 // للخرائط / For maps
#include <vector>                        // للمصفوفات / For vectors
#include <mutex>                         // للقفل / For locking
#include <functional>                    // للدوال / For functions

namespace sad {
namespace graphics {

// ==============================================================================
// الثوابت / Constants
// ==============================================================================

/// حد أقصى لحجم الذاكرة المستخدمة (بالبايت) / Maximum memory usage (bytes)
constexpr size_t DEFAULT_MAX_MEMORY = 512 * 1024 * 1024;  // 512 MB

/// حد أقصى لعدد الموارد / Maximum number of resources
constexpr size_t DEFAULT_MAX_RESOURCES = 1000;

// ==============================================================================
// إحصائيات الموارد / Resource Statistics
// ==============================================================================

/// إحصائيات استخدام الموارد / Resource usage statistics
struct ResourceStats {
    size_t totalResources;               // إجمالي الموارد / Total resources
    size_t loadedResources;              // الموارد المحملة / Loaded resources
    size_t cachedResources;              // الموارد المخزنة / Cached resources
    size_t memoryUsed;                   // الذاكرة المستخدمة (بايت) / Memory used (bytes)
    size_t memoryLimit;                  // حد الذاكرة (بايت) / Memory limit (bytes)
    
    // إحصائيات حسب النوع / Statistics by type
    size_t textureCount;                 // عدد textures / Texture count
    size_t fontCount;                    // عدد الخطوط / Font count
    size_t shaderCount;                  // عدد shaders / Shader count
    
    // إحصائيات الأداء / Performance stats
    size_t loadCount;                    // عدد التحميلات / Load count
    size_t cacheHits;                    // عدد نجاح Cache / Cache hits
    size_t cacheMisses;                  // عدد فشل Cache / Cache misses
    
    /// إعادة تعيين الإحصائيات / Reset statistics
    void Reset() {
        totalResources = 0;
        loadedResources = 0;
        cachedResources = 0;
        memoryUsed = 0;
        textureCount = 0;
        fontCount = 0;
        shaderCount = 0;
        loadCount = 0;
        cacheHits = 0;
        cacheMisses = 0;
    }
    
    /// حساب نسبة نجاح Cache / Calculate cache hit ratio
    float GetCacheHitRatio() const {
        size_t total = cacheHits + cacheMisses;
        return total > 0 ? (float)cacheHits / total : 0.0f;
    }
};

// ==============================================================================
// معلومات المورد / Resource Info
// ==============================================================================

/// نوع المورد / Resource type
enum class ResourceType {
    Texture,                             // صورة / Texture
    Font,                                // خط / Font
    Shader,                              // shader
    Unknown                              // غير معروف / Unknown
};

/// معلومات المورد الأساسية / Basic resource information
struct ResourceInfo {
    std::string path;                    // مسار الملف / File path
    std::string name;                    // الاسم / Name
    ResourceType type;                   // النوع / Type
    size_t size;                         // الحجم بالبايت / Size in bytes
    u32 refCount;                        // عداد المراجع / Reference count
    bool loaded;                         // محمّل؟ / Loaded?
    double lastAccessTime;               // آخر وقت استخدام / Last access time
    
    /// المُنشئ / Constructor
    ResourceInfo()
        : type(ResourceType::Unknown)
        , size(0)
        , refCount(0)
        , loaded(false)
        , lastAccessTime(0.0)
    {}
};

// ==============================================================================
// مدير الموارد / Resource Manager
// ==============================================================================

/// مدير الموارد - Singleton pattern
class ResourceManager {
public:
    // ==============================================================================
    // الوصول للـ Singleton / Singleton Access
    // ==============================================================================
    
    /// الحصول على instance الوحيد / Get singleton instance
    static ResourceManager& GetInstance();
    
    // منع النسخ والنقل / Prevent copy and move
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;
    
    // ==============================================================================
    // إدارة Textures / Texture Management
    // ==============================================================================
    
    /// تحميل texture من ملف / Load texture from file
    /// path: مسار الملف / File path
    /// filter: نوع الفلتر / Filter type
    /// wrap: نوع الـ wrapping / Wrap type
    /// Returns: مؤشر للـ texture / Pointer to texture
    TextureRef LoadTexture(const std::string& path,
                          TextureFilter filter = TextureFilter::Linear,
                          TextureWrap wrap = TextureWrap::Repeat);
    
    /// الحصول على texture محمّل / Get loaded texture
    /// path: مسار الملف / File path
    /// Returns: مؤشر للـ texture أو nullptr / Pointer to texture or nullptr
    TextureRef GetTexture(const std::string& path) const;
    
    /// تفريغ texture / Unload texture
    /// path: مسار الملف / File path
    void UnloadTexture(const std::string& path);
    
    /// تحميل texture مسبقاً (async) / Preload texture (async)
    /// path: مسار الملف / File path
    /// callback: دالة عند الانتهاء / Callback on completion
    void PreloadTexture(const std::string& path,
                       std::function<void(TextureRef)> callback = nullptr);
    
    // ==============================================================================
    // إدارة الخطوط / Font Management
    // ==============================================================================
    
    /// تحميل خط من ملف / Load font from file
    /// path: مسار الملف / File path
    /// fontSize: حجم الخط / Font size
    /// Returns: مؤشر للخط / Pointer to font
    FontRef LoadFont(const std::string& path, float fontSize = 24.0f);
    
    /// الحصول على خط محمّل / Get loaded font
    /// path: مسار الملف / File path
    /// fontSize: حجم الخط / Font size
    /// Returns: مؤشر للخط أو nullptr / Pointer to font or nullptr
    FontRef GetFont(const std::string& path, float fontSize = 24.0f) const;
    
    /// تفريغ خط / Unload font
    /// path: مسار الملف / File path
    void UnloadFont(const std::string& path, float fontSize = 24.0f);
    
    // ==============================================================================
    // إدارة Shaders / Shader Management
    // ==============================================================================
    
    /// تحميل shader من ملفات / Load shader from files
    /// name: اسم الـ shader / Shader name
    /// vertexPath: مسار vertex shader / Vertex shader path
    /// fragmentPath: مسار fragment shader / Fragment shader path
    /// Returns: مؤشر للـ shader / Pointer to shader
    std::shared_ptr<SadGraphics::ShaderProgram> LoadShader(
        const std::string& name,
        const std::string& vertexPath,
        const std::string& fragmentPath
    );
    
    /// الحصول على shader محمّل / Get loaded shader
    /// name: اسم الـ shader / Shader name
    /// Returns: مؤشر للـ shader أو nullptr / Pointer to shader or nullptr
    std::shared_ptr<SadGraphics::ShaderProgram> GetShader(const std::string& name) const;
    
    /// تفريغ shader / Unload shader
    /// name: اسم الـ shader / Shader name
    void UnloadShader(const std::string& name);
    
    // ==============================================================================
    // إدارة عامة / General Management
    // ==============================================================================
    
    /// تفريغ جميع الموارد / Unload all resources
    void UnloadAll();
    
    /// تفريغ الموارد غير المستخدمة / Unload unused resources
    /// Returns: عدد الموارد المحررة / Number of resources freed
    size_t UnloadUnused();
    
    /// تفريغ الموارد حسب النوع / Unload resources by type
    /// type: نوع المورد / Resource type
    void UnloadByType(ResourceType type);
    
    /// تحديث (يُستدعى كل إطار) / Update (called every frame)
    void Update();
    
    // ==============================================================================
    // الإعدادات / Settings
    // ==============================================================================
    
    /// ضبط حد الذاكرة / Set memory limit
    /// limit: الحد بالبايت / Limit in bytes
    void SetMemoryLimit(size_t limit) { m_memoryLimit = limit; }
    
    /// الحصول على حد الذاكرة / Get memory limit
    /// Returns: الحد بالبايت / Limit in bytes
    size_t GetMemoryLimit() const { return m_memoryLimit; }
    
    /// ضبط حد عدد الموارد / Set resource count limit
    /// limit: الحد / Limit
    void SetResourceLimit(size_t limit) { m_resourceLimit = limit; }
    
    /// الحصول على حد عدد الموارد / Get resource count limit
    /// Returns: الحد / Limit
    size_t GetResourceLimit() const { return m_resourceLimit; }
    
    /// تفعيل/تعطيل التحميل التلقائي / Enable/disable auto-unload
    /// enable: تفعيل؟ / Enable?
    void SetAutoUnload(bool enable) { m_autoUnload = enable; }
    
    /// هل التحميل التلقائي مفعّل؟ / Is auto-unload enabled?
    /// Returns: مفعّل؟ / Enabled?
    bool IsAutoUnloadEnabled() const { return m_autoUnload; }
    
    // ==============================================================================
    // الإحصائيات والتشخيص / Statistics and Diagnostics
    // ==============================================================================
    
    /// الحصول على الإحصائيات / Get statistics
    /// Returns: الإحصائيات / Statistics
    const ResourceStats& GetStats() const { return m_stats; }
    
    /// إعادة تعيين الإحصائيات / Reset statistics
    void ResetStats() { m_stats.Reset(); }
    
    /// طباعة الإحصائيات / Print statistics
    void PrintStats() const;
    
    /// الحصول على قائمة الموارد المحملة / Get list of loaded resources
    /// Returns: قائمة المعلومات / List of info
    std::vector<ResourceInfo> GetLoadedResources() const;
    
    /// الحصول على معلومات مورد / Get resource info
    /// path: مسار المورد / Resource path
    /// Returns: المعلومات / Info
    ResourceInfo GetResourceInfo(const std::string& path) const;
    
    // ==============================================================================
    // مسارات البحث / Search Paths
    // ==============================================================================
    
    /// إضافة مسار بحث / Add search path
    /// path: المسار / Path
    void AddSearchPath(const std::string& path);
    
    /// إزالة مسار بحث / Remove search path
    /// path: المسار / Path
    void RemoveSearchPath(const std::string& path);
    
    /// مسح جميع مسارات البحث / Clear all search paths
    void ClearSearchPaths();
    
    /// الحصول على قائمة مسارات البحث / Get search paths list
    /// Returns: قائمة المسارات / List of paths
    const std::vector<std::string>& GetSearchPaths() const { return m_searchPaths; }
    
    /// البحث عن ملف في المسارات / Search for file in paths
    /// filename: اسم الملف / Filename
    /// Returns: المسار الكامل أو فارغ / Full path or empty
    std::string FindFile(const std::string& filename) const;

private:
    // ==============================================================================
    // المُنشئ والمُدمر الخاص / Private Constructor and Destructor
    // ==============================================================================
    
    /// المُنشئ الخاص / Private constructor
    ResourceManager();
    
    /// المُدمر / Destructor
    ~ResourceManager();
    
    // ==============================================================================
    // الدوال المساعدة / Helper Functions
    // ==============================================================================
    
    /// توليد مفتاح لخط / Generate font key
    /// path: مسار الخط / Font path
    /// fontSize: حجم الخط / Font size
    /// Returns: المفتاح / Key
    std::string GenerateFontKey(const std::string& path, float fontSize) const;
    
    /// حساب حجم texture / Calculate texture size
    /// texture: الـ texture / Texture
    /// Returns: الحجم بالبايت / Size in bytes
    size_t CalculateTextureSize(const TextureRef& texture) const;
    
    /// حساب حجم font / Calculate font size
    /// font: الخط / Font
    /// Returns: الحجم بالبايت / Size in bytes
    size_t CalculateFontSize(const FontRef& font) const;
    
    /// التحقق من الذاكرة / Check memory
    /// size: الحجم المطلوب / Required size
    /// Returns: هل يمكن التخصيص؟ / Can allocate?
    bool CheckMemory(size_t size);
    
    /// تحرير ذاكرة / Free memory
    /// size: الحجم المطلوب / Required size
    /// Returns: تم التحرير؟ / Freed?
    bool FreeMemory(size_t size);
    
    /// الحصول على الوقت الحالي / Get current time
    /// Returns: الوقت بالثواني / Time in seconds
    double GetCurrentTime() const;
    
    // ==============================================================================
    // البيانات الخاصة / Private Data
    // ==============================================================================
    
    // Caches (التخزين المؤقت)
    std::unordered_map<std::string, TextureRef> m_textureCache;     // textures
    std::unordered_map<std::string, FontRef> m_fontCache;           // fonts
    std::unordered_map<std::string, std::shared_ptr<SadGraphics::ShaderProgram>> m_shaderCache;  // shaders
    
    // معلومات الموارد / Resource info
    std::unordered_map<std::string, ResourceInfo> m_resourceInfo;   // معلومات / Info
    
    // الإعدادات / Settings
    size_t m_memoryLimit;                // حد الذاكرة / Memory limit
    size_t m_resourceLimit;              // حد عدد الموارد / Resource limit
    bool m_autoUnload;                   // تفريغ تلقائي / Auto unload
    
    // الإحصائيات / Statistics
    ResourceStats m_stats;               // الإحصائيات / Statistics
    
    // مسارات البحث / Search paths
    std::vector<std::string> m_searchPaths;  // مسارات البحث / Search paths
    
    // القفل للـ thread safety / Lock for thread safety
    mutable std::mutex m_mutex;          // قفل / Mutex
};

// ==============================================================================
// دوال مساعدة عامة / Global Helper Functions
// ==============================================================================

/// تحميل texture بسرعة / Quick load texture
/// path: مسار الملف / File path
/// Returns: مؤشر للـ texture / Pointer to texture
inline TextureRef LoadTexture(const std::string& path) {
    return ResourceManager::GetInstance().LoadTexture(path);
}

/// تحميل خط بسرعة / Quick load font
/// path: مسار الملف / File path
/// fontSize: حجم الخط / Font size
/// Returns: مؤشر للخط / Pointer to font
inline FontRef LoadFont(const std::string& path, float fontSize = 24.0f) {
    return ResourceManager::GetInstance().LoadFont(path, fontSize);
}

} // namespace graphics
} // namespace sad

#endif // SAD_GRAPHICS_RESOURCES_RESOURCE_MANAGER_H
