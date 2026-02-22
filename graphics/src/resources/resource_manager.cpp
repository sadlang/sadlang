// ==============================================================================
// resource_manager.cpp - تطبيق نظام إدارة الموارد / Resource Manager Implementation
// ==============================================================================
// الوصف: تطبيق كامل لنظام إدارة وتخزين الموارد
// Description: Complete implementation of resource management and caching system
// ==============================================================================

#include "../../include/resources/resource_manager.h"  // ملف الرأس / Header file

#include <iostream>                      // للإدخال/الإخراج / For I/O
#include <fstream>                       // لقراءة الملفات / For file reading
#include <algorithm>                     // للخوارزميات / For algorithms
#include <chrono>                        // للوقت / For timing
#include <filesystem>                    // لنظام الملفات / For filesystem

namespace fs = std::filesystem;          // اختصار / Shortcut

namespace sad {
namespace graphics {

// ==============================================================================
// الوصول للـ Singleton / Singleton Access
// ==============================================================================

/// الحصول على instance الوحيد / Get singleton instance
ResourceManager& ResourceManager::GetInstance() {
    static ResourceManager instance;     // instance ساكن / Static instance
    return instance;                     // إرجاع المرجع / Return reference
}

// ==============================================================================
// المُنشئ والمُدمر / Constructor and Destructor
// ==============================================================================

/// المُنشئ الخاص / Private constructor
ResourceManager::ResourceManager()
    : m_memoryLimit(DEFAULT_MAX_MEMORY)  // حد الذاكرة الافتراضي / Default memory limit
    , m_resourceLimit(DEFAULT_MAX_RESOURCES)  // حد الموارد الافتراضي / Default resource limit
    , m_autoUnload(true)                 // تفعيل التفريغ التلقائي / Enable auto unload
{
    m_stats.memoryLimit = m_memoryLimit; // حفظ حد الذاكرة / Store memory limit
    m_stats.Reset();                     // إعادة تعيين الإحصائيات / Reset statistics
    
    std::cout << "(AR) تم تهيئة مدير الموارد / (EN) Resource Manager initialized" << std::endl;
}

/// المُدمر / Destructor
ResourceManager::~ResourceManager() {
    UnloadAll();                         // تفريغ جميع الموارد / Unload all resources
    std::cout << "(AR) تم تدمير مدير الموارد / (EN) Resource Manager destroyed" << std::endl;
}

// ==============================================================================
// إدارة Textures / Texture Management
// ==============================================================================

/// تحميل texture من ملف / Load texture from file
TextureRef ResourceManager::LoadTexture(const std::string& path,
                                       TextureFilter filter,
                                       TextureWrap wrap) {
    std::lock_guard<std::mutex> lock(m_mutex);  // قفل للـ thread safety / Lock for thread safety
    
    // التحقق من التخزين المؤقت / Check cache
    auto it = m_textureCache.find(path);
    if (it != m_textureCache.end()) {    // إذا وُجد / If found
        m_stats.cacheHits++;             // زيادة cache hits / Increment cache hits
        
        // تحديث معلومات المورد / Update resource info
        auto& info = m_resourceInfo[path];
        info.refCount++;                 // زيادة عداد المراجع / Increment ref count
        info.lastAccessTime = GetCurrentTime();  // تحديث وقت الوصول / Update access time
        
        return it->second;               // إرجاع من Cache / Return from cache
    }
    
    m_stats.cacheMisses++;               // زيادة cache misses / Increment cache misses
    
    // البحث عن الملف / Search for file
    std::string fullPath = FindFile(path);
    if (fullPath.empty()) {              // إذا لم يُوجد / If not found
        std::cerr << "(AR) فشل إيجاد ملف: " << path 
                 << " / (EN) Failed to find file: " << path << std::endl;
        return nullptr;                  // فشل / Failure
    }
    
    // تحميل الـ texture / Load texture
    TextureRef texture = Texture::CreateFromFile(fullPath);
    if (texture && texture->IsValid()) {
        // تطبيق الفلتر والتغليف / Apply filter and wrap
        texture->SetFilter(filter, filter);
        texture->SetWrap(wrap, wrap);
    }
    
    if (!texture || !texture->IsValid()) {  // إذا فشل التحميل / If loading failed
        std::cerr << "(AR) فشل تحميل texture: " << path 
                 << " / (EN) Failed to load texture: " << path << std::endl;
        return nullptr;                  // فشل / Failure
    }
    
    // حساب حجم الذاكرة / Calculate memory size
    size_t size = CalculateTextureSize(texture);
    
    // التحقق من الذاكرة / Check memory
    if (!CheckMemory(size)) {            // إذا لم تكن هناك ذاكرة كافية / If not enough memory
        std::cerr << "(AR) لا توجد ذاكرة كافية لتحميل texture / "
                 << "(EN) Not enough memory to load texture" << std::endl;
        
        // محاولة تحرير ذاكرة / Try to free memory
        if (!FreeMemory(size)) {         // إذا فشل التحرير / If freeing failed
            return nullptr;              // فشل / Failure
        }
    }
    
    // تخزين في Cache / Store in cache
    m_textureCache[path] = texture;      // حفظ الـ texture / Store texture
    
    // حفظ معلومات المورد / Store resource info
    ResourceInfo info;
    info.path = path;                    // المسار / Path
    info.name = fs::path(path).filename().string();  // الاسم / Name
    info.type = ResourceType::Texture;   // النوع / Type
    info.size = size;                    // الحجم / Size
    info.refCount = 1;                   // عداد مراجع / Ref count
    info.loaded = true;                  // محمّل / Loaded
    info.lastAccessTime = GetCurrentTime();  // وقت الوصول / Access time
    m_resourceInfo[path] = info;         // حفظ المعلومات / Store info
    
    // تحديث الإحصائيات / Update statistics
    m_stats.loadedResources++;           // زيادة عدد الموارد / Increment loaded count
    m_stats.cachedResources++;           // زيادة عدد المخزنة / Increment cached count
    m_stats.textureCount++;              // زيادة عدد textures / Increment texture count
    m_stats.memoryUsed += size;          // زيادة الذاكرة المستخدمة / Increment memory used
    m_stats.loadCount++;                 // زيادة عدد التحميلات / Increment load count
    
    std::cout << "(AR) تم تحميل texture: " << path 
             << " / (EN) Loaded texture: " << path << std::endl;
    
    return texture;                      // إرجاع الـ texture / Return texture
}

/// الحصول على texture محمّل / Get loaded texture
TextureRef ResourceManager::GetTexture(const std::string& path) const {
    std::lock_guard<std::mutex> lock(m_mutex);  // قفل / Lock
    
    auto it = m_textureCache.find(path); // البحث / Search
    if (it != m_textureCache.end()) {    // إذا وُجد / If found
        return it->second;               // إرجاع / Return
    }
    
    return nullptr;                      // لم يُوجد / Not found
}

/// تفريغ texture / Unload texture
void ResourceManager::UnloadTexture(const std::string& path) {
    std::lock_guard<std::mutex> lock(m_mutex);  // قفل / Lock
    
    auto it = m_textureCache.find(path); // البحث / Search
    if (it == m_textureCache.end()) {    // إذا لم يُوجد / If not found
        return;                          // خروج / Exit
    }
    
    // الحصول على معلومات المورد / Get resource info
    auto infoIt = m_resourceInfo.find(path);
    if (infoIt != m_resourceInfo.end()) {
        // تحديث الإحصائيات / Update statistics
        m_stats.memoryUsed -= infoIt->second.size;  // تقليل الذاكرة / Decrease memory
        m_stats.cachedResources--;       // تقليل عدد المخزنة / Decrease cached count
        m_stats.textureCount--;          // تقليل عدد textures / Decrease texture count
        
        m_resourceInfo.erase(infoIt);    // حذف المعلومات / Delete info
    }
    
    m_textureCache.erase(it);            // حذف من Cache / Delete from cache
    
    std::cout << "(AR) تم تفريغ texture: " << path 
             << " / (EN) Unloaded texture: " << path << std::endl;
}

/// تحميل texture مسبقاً / Preload texture
void ResourceManager::PreloadTexture(const std::string& path,
                                    std::function<void(TextureRef)> callback) {
    // ملاحظة: في التطبيق الحقيقي، يجب استخدام thread منفصل
    // Note: In real implementation, should use separate thread
    
    // تحميل عادي حالياً / Normal loading for now
    TextureRef texture = LoadTexture(path);
    
    if (callback) {                      // إذا كان هناك callback / If callback exists
        callback(texture);               // استدعاء / Call
    }
}

// ==============================================================================
// إدارة الخطوط / Font Management
// ==============================================================================

/// تحميل خط من ملف / Load font from file
FontRef ResourceManager::LoadFont(const std::string& path, float fontSize) {
    std::lock_guard<std::mutex> lock(m_mutex);  // قفل / Lock
    
    // توليد مفتاح / Generate key
    std::string key = GenerateFontKey(path, fontSize);
    
    // التحقق من Cache / Check cache
    auto it = m_fontCache.find(key);
    if (it != m_fontCache.end()) {       // إذا وُجد / If found
        m_stats.cacheHits++;             // زيادة hits / Increment hits
        
        // تحديث معلومات / Update info
        auto& info = m_resourceInfo[key];
        info.refCount++;                 // زيادة عداد / Increment count
        info.lastAccessTime = GetCurrentTime();  // تحديث وقت / Update time
        
        return it->second;               // إرجاع / Return
    }
    
    m_stats.cacheMisses++;               // زيادة misses / Increment misses
    
    // البحث عن الملف / Search for file
    std::string fullPath = FindFile(path);
    if (fullPath.empty()) {              // إذا لم يُوجد / If not found
        std::cerr << "(AR) فشل إيجاد ملف الخط: " << path 
                 << " / (EN) Failed to find font file: " << path << std::endl;
        return nullptr;                  // فشل / Failure
    }
    
    // تحميل الخط / Load font
    FontRef font = Font::CreateFromFile(fullPath, fontSize);
    
    if (!font || !font->IsValid()) {     // إذا فشل / If failed
        std::cerr << "(AR) فشل تحميل خط: " << path 
                 << " / (EN) Failed to load font: " << path << std::endl;
        return nullptr;                  // فشل / Failure
    }
    
    // حساب الحجم / Calculate size
    size_t size = CalculateFontSize(font);
    
    // التحقق من الذاكرة / Check memory
    if (!CheckMemory(size)) {            // إذا لم تكن كافية / If not enough
        if (!FreeMemory(size)) {         // محاولة تحرير / Try to free
            return nullptr;              // فشل / Failure
        }
    }
    
    // تخزين / Store
    m_fontCache[key] = font;             // حفظ / Save
    
    // معلومات المورد / Resource info
    ResourceInfo info;
    info.path = path;
    info.name = fs::path(path).filename().string();
    info.type = ResourceType::Font;
    info.size = size;
    info.refCount = 1;
    info.loaded = true;
    info.lastAccessTime = GetCurrentTime();
    m_resourceInfo[key] = info;
    
    // تحديث الإحصائيات / Update statistics
    m_stats.loadedResources++;
    m_stats.cachedResources++;
    m_stats.fontCount++;
    m_stats.memoryUsed += size;
    m_stats.loadCount++;
    
    std::cout << "(AR) تم تحميل خط: " << path << " (حجم: " << fontSize << ")"
             << " / (EN) Loaded font: " << path << " (size: " << fontSize << ")" << std::endl;
    
    return font;                         // إرجاع / Return
}

/// الحصول على خط محمّل / Get loaded font
FontRef ResourceManager::GetFont(const std::string& path, float fontSize) const {
    std::lock_guard<std::mutex> lock(m_mutex);  // قفل / Lock
    
    std::string key = GenerateFontKey(path, fontSize);  // المفتاح / Key
    auto it = m_fontCache.find(key);     // البحث / Search
    
    if (it != m_fontCache.end()) {       // إذا وُجد / If found
        return it->second;               // إرجاع / Return
    }
    
    return nullptr;                      // لم يُوجد / Not found
}

/// تفريغ خط / Unload font
void ResourceManager::UnloadFont(const std::string& path, float fontSize) {
    std::lock_guard<std::mutex> lock(m_mutex);  // قفل / Lock
    
    std::string key = GenerateFontKey(path, fontSize);  // المفتاح / Key
    auto it = m_fontCache.find(key);     // البحث / Search
    
    if (it == m_fontCache.end()) {       // إذا لم يُوجد / If not found
        return;                          // خروج / Exit
    }
    
    // تحديث الإحصائيات / Update statistics
    auto infoIt = m_resourceInfo.find(key);
    if (infoIt != m_resourceInfo.end()) {
        m_stats.memoryUsed -= infoIt->second.size;
        m_stats.cachedResources--;
        m_stats.fontCount--;
        m_resourceInfo.erase(infoIt);
    }
    
    m_fontCache.erase(it);               // حذف / Delete
    
    std::cout << "(AR) تم تفريغ خط: " << path 
             << " / (EN) Unloaded font: " << path << std::endl;
}

// ==============================================================================
// إدارة Shaders / Shader Management
// ==============================================================================

/// تحميل shader من ملفات / Load shader from files
std::shared_ptr<SadGraphics::ShaderProgram> ResourceManager::LoadShader(
    const std::string& name,
    const std::string& vertexPath,
    const std::string& fragmentPath) {
    
    std::lock_guard<std::mutex> lock(m_mutex);  // قفل / Lock
    
    // التحقق من Cache / Check cache
    auto it = m_shaderCache.find(name);
    if (it != m_shaderCache.end()) {     // إذا وُجد / If found
        m_stats.cacheHits++;             // زيادة hits / Increment hits
        return it->second;               // إرجاع / Return
    }
    
    m_stats.cacheMisses++;               // زيادة misses / Increment misses
    
    // قراءة الملفات / Read files
    std::string vertexFullPath = FindFile(vertexPath);
    std::string fragmentFullPath = FindFile(fragmentPath);
    
    if (vertexFullPath.empty() || fragmentFullPath.empty()) {
        std::cerr << "(AR) فشل إيجاد ملفات shader / (EN) Failed to find shader files" << std::endl;
        return nullptr;
    }
    
    // قراءة محتوى الملفات / Read file contents
    std::ifstream vShaderFile(vertexFullPath);
    std::ifstream fShaderFile(fragmentFullPath);
    
    if (!vShaderFile.is_open() || !fShaderFile.is_open()) {
        std::cerr << "(AR) فشل فتح ملفات shader / (EN) Failed to open shader files" << std::endl;
        return nullptr;
    }
    
    std::string vertexCode((std::istreambuf_iterator<char>(vShaderFile)),
                           std::istreambuf_iterator<char>());
    std::string fragmentCode((std::istreambuf_iterator<char>(fShaderFile)),
                             std::istreambuf_iterator<char>());
    
    // إنشاء shader / Create shader
    auto shader = std::make_shared<SadGraphics::ShaderProgram>();
    if (shader->Compile(vertexCode, fragmentCode) != sad::graphics::Result::Success) {
        std::cerr << "(AR) فشل ترجمة shader / (EN) Failed to compile shader" << std::endl;
        return nullptr;
    }
    
    // تخزين / Store
    m_shaderCache[name] = shader;
    
    // معلومات / Info
    ResourceInfo info;
    info.path = name;
    info.name = name;
    info.type = ResourceType::Shader;
    info.size = vertexCode.size() + fragmentCode.size();
    info.refCount = 1;
    info.loaded = true;
    info.lastAccessTime = GetCurrentTime();
    m_resourceInfo[name] = info;
    
    // إحصائيات / Statistics
    m_stats.loadedResources++;
    m_stats.cachedResources++;
    m_stats.shaderCount++;
    m_stats.memoryUsed += info.size;
    m_stats.loadCount++;
    
    std::cout << "(AR) تم تحميل shader: " << name 
             << " / (EN) Loaded shader: " << name << std::endl;
    
    return shader;
}

/// الحصول على shader محمّل / Get loaded shader
std::shared_ptr<SadGraphics::ShaderProgram> ResourceManager::GetShader(const std::string& name) const {
    std::lock_guard<std::mutex> lock(m_mutex);  // قفل / Lock
    
    auto it = m_shaderCache.find(name);  // البحث / Search
    if (it != m_shaderCache.end()) {     // إذا وُجد / If found
        return it->second;               // إرجاع / Return
    }
    
    return nullptr;                      // لم يُوجد / Not found
}

/// تفريغ shader / Unload shader
void ResourceManager::UnloadShader(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);  // قفل / Lock
    
    auto it = m_shaderCache.find(name);  // البحث / Search
    if (it == m_shaderCache.end()) {     // إذا لم يُوجد / If not found
        return;                          // خروج / Exit
    }
    
    // تحديث الإحصائيات / Update statistics
    auto infoIt = m_resourceInfo.find(name);
    if (infoIt != m_resourceInfo.end()) {
        m_stats.memoryUsed -= infoIt->second.size;
        m_stats.cachedResources--;
        m_stats.shaderCount--;
        m_resourceInfo.erase(infoIt);
    }
    
    m_shaderCache.erase(it);             // حذف / Delete
    
    std::cout << "(AR) تم تفريغ shader: " << name 
             << " / (EN) Unloaded shader: " << name << std::endl;
}

// ==============================================================================
// إدارة عامة / General Management
// ==============================================================================

/// تفريغ جميع الموارد / Unload all resources
void ResourceManager::UnloadAll() {
    std::lock_guard<std::mutex> lock(m_mutex);  // قفل / Lock
    
    std::cout << "(AR) تفريغ جميع الموارد... / (EN) Unloading all resources..." << std::endl;
    
    // مسح جميع Caches / Clear all caches
    m_textureCache.clear();              // مسح textures / Clear textures
    m_fontCache.clear();                 // مسح fonts / Clear fonts
    m_shaderCache.clear();               // مسح shaders / Clear shaders
    m_resourceInfo.clear();              // مسح المعلومات / Clear info
    
    // إعادة تعيين الإحصائيات / Reset statistics
    m_stats.Reset();
    m_stats.memoryLimit = m_memoryLimit;
    
    std::cout << "(AR) تم تفريغ جميع الموارد / (EN) All resources unloaded" << std::endl;
}

/// تفريغ الموارد غير المستخدمة / Unload unused resources
size_t ResourceManager::UnloadUnused() {
    std::lock_guard<std::mutex> lock(m_mutex);  // قفل / Lock
    
    size_t freedCount = 0;               // عداد المحررة / Freed counter
    double currentTime = GetCurrentTime();  // الوقت الحالي / Current time
    const double UNUSED_THRESHOLD = 60.0;   // عتبة: 60 ثانية / Threshold: 60 seconds
    
    // قائمة للحذف / List for deletion
    std::vector<std::string> toUnload;
    
    // البحث عن موارد غير مستخدمة / Search for unused resources
    for (const auto& [path, info] : m_resourceInfo) {
        // إذا لم يُستخدم لفترة طويلة / If not used for long time
        if (info.refCount == 0 && (currentTime - info.lastAccessTime) > UNUSED_THRESHOLD) {
            toUnload.push_back(path);    // إضافة للقائمة / Add to list
        }
    }
    
    // تفريغ الموارد / Unload resources
    for (const auto& path : toUnload) {
        auto& info = m_resourceInfo[path];
        
        switch (info.type) {
            case ResourceType::Texture:
                UnloadTexture(path);     // تفريغ texture / Unload texture
                break;
            case ResourceType::Font:
                // ملاحظة: يحتاج فك تشفير المفتاح
                // Note: needs key decoding
                break;
            case ResourceType::Shader:
                UnloadShader(path);      // تفريغ shader / Unload shader
                break;
            default:
                break;
        }
        
        freedCount++;                    // زيادة العداد / Increment counter
    }
    
    if (freedCount > 0) {
        std::cout << "(AR) تم تفريغ " << freedCount << " مورد غير مستخدم"
                 << " / (EN) Unloaded " << freedCount << " unused resources" << std::endl;
    }
    
    return freedCount;                   // إرجاع العدد / Return count
}

/// تفريغ الموارد حسب النوع / Unload resources by type
void ResourceManager::UnloadByType(ResourceType type) {
    std::lock_guard<std::mutex> lock(m_mutex);  // قفل / Lock
    
    std::vector<std::string> toUnload;   // قائمة للحذف / List for deletion
    
    // جمع الموارد من النوع المحدد / Collect resources of specified type
    for (const auto& [path, info] : m_resourceInfo) {
        if (info.type == type) {         // إذا كان النوع مطابق / If type matches
            toUnload.push_back(path);    // إضافة / Add
        }
    }
    
    // تفريغ / Unload
    for (const auto& path : toUnload) {
        switch (type) {
            case ResourceType::Texture:
                UnloadTexture(path);
                break;
            case ResourceType::Font:
                // يحتاج معالجة خاصة / Needs special handling
                break;
            case ResourceType::Shader:
                UnloadShader(path);
                break;
            default:
                break;
        }
    }
    
    std::cout << "(AR) تم تفريغ " << toUnload.size() << " مورد من نوع محدد"
             << " / (EN) Unloaded " << toUnload.size() << " resources of type" << std::endl;
}

/// تحديث / Update
void ResourceManager::Update() {
    // إذا كان التفريغ التلقائي مفعّل / If auto unload enabled
    if (m_autoUnload) {
        // تفريغ غير المستخدم كل فترة / Unload unused periodically
        static double lastCheck = 0.0;   // آخر فحص / Last check
        double currentTime = GetCurrentTime();  // الوقت الحالي / Current time
        
        if (currentTime - lastCheck > 10.0) {  // كل 10 ثواني / Every 10 seconds
            UnloadUnused();              // تفريغ / Unload
            lastCheck = currentTime;     // تحديث / Update
        }
    }
}

// ==============================================================================
// الإحصائيات / Statistics
// ==============================================================================

/// طباعة الإحصائيات / Print statistics
void ResourceManager::PrintStats() const {
    std::lock_guard<std::mutex> lock(m_mutex);  // قفل / Lock
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "(AR) إحصائيات مدير الموارد / (EN) Resource Manager Statistics" << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::cout << "(AR) الموارد المحملة / (EN) Loaded Resources: " 
             << m_stats.loadedResources << std::endl;
    std::cout << "(AR) الموارد المخزنة / (EN) Cached Resources: " 
             << m_stats.cachedResources << std::endl;
    
    std::cout << "\n(AR) التوزيع حسب النوع / (EN) Distribution by Type:" << std::endl;
    std::cout << "  Textures: " << m_stats.textureCount << std::endl;
    std::cout << "  Fonts: " << m_stats.fontCount << std::endl;
    std::cout << "  Shaders: " << m_stats.shaderCount << std::endl;
    
    std::cout << "\n(AR) استخدام الذاكرة / (EN) Memory Usage:" << std::endl;
    std::cout << "  (AR) المستخدم / (EN) Used: " 
             << (m_stats.memoryUsed / 1024.0 / 1024.0) << " MB" << std::endl;
    std::cout << "  (AR) الحد / (EN) Limit: " 
             << (m_stats.memoryLimit / 1024.0 / 1024.0) << " MB" << std::endl;
    std::cout << "  (AR) النسبة / (EN) Percentage: " 
             << (m_stats.memoryUsed * 100.0 / m_stats.memoryLimit) << "%" << std::endl;
    
    std::cout << "\n(AR) الأداء / (EN) Performance:" << std::endl;
    std::cout << "  (AR) عدد التحميلات / (EN) Load Count: " 
             << m_stats.loadCount << std::endl;
    std::cout << "  (AR) نجاح Cache / (EN) Cache Hits: " 
             << m_stats.cacheHits << std::endl;
    std::cout << "  (AR) فشل Cache / (EN) Cache Misses: " 
             << m_stats.cacheMisses << std::endl;
    std::cout << "  (AR) نسبة النجاح / (EN) Hit Ratio: " 
             << (m_stats.GetCacheHitRatio() * 100.0f) << "%" << std::endl;
    
    std::cout << "========================================\n" << std::endl;
}

/// الحصول على قائمة الموارد المحملة / Get loaded resources list
std::vector<ResourceInfo> ResourceManager::GetLoadedResources() const {
    std::lock_guard<std::mutex> lock(m_mutex);  // قفل / Lock
    
    std::vector<ResourceInfo> resources;  // قائمة الموارد / Resources list
    
    for (const auto& [path, info] : m_resourceInfo) {
        resources.push_back(info);       // إضافة / Add
    }
    
    return resources;                    // إرجاع / Return
}

/// الحصول على معلومات مورد / Get resource info
ResourceInfo ResourceManager::GetResourceInfo(const std::string& path) const {
    std::lock_guard<std::mutex> lock(m_mutex);  // قفل / Lock
    
    auto it = m_resourceInfo.find(path);  // البحث / Search
    if (it != m_resourceInfo.end()) {    // إذا وُجد / If found
        return it->second;               // إرجاع / Return
    }
    
    return ResourceInfo();               // إرجاع فارغ / Return empty
}

// ==============================================================================
// مسارات البحث / Search Paths
// ==============================================================================

/// إضافة مسار بحث / Add search path
void ResourceManager::AddSearchPath(const std::string& path) {
    std::lock_guard<std::mutex> lock(m_mutex);  // قفل / Lock
    
    // التحقق من عدم وجوده / Check if not exists
    if (std::find(m_searchPaths.begin(), m_searchPaths.end(), path) == m_searchPaths.end()) {
        m_searchPaths.push_back(path);   // إضافة / Add
        std::cout << "(AR) تمت إضافة مسار بحث: " << path 
                 << " / (EN) Added search path: " << path << std::endl;
    }
}

/// إزالة مسار بحث / Remove search path
void ResourceManager::RemoveSearchPath(const std::string& path) {
    std::lock_guard<std::mutex> lock(m_mutex);  // قفل / Lock
    
    auto it = std::find(m_searchPaths.begin(), m_searchPaths.end(), path);
    if (it != m_searchPaths.end()) {     // إذا وُجد / If found
        m_searchPaths.erase(it);         // حذف / Delete
        std::cout << "(AR) تمت إزالة مسار بحث: " << path 
                 << " / (EN) Removed search path: " << path << std::endl;
    }
}

/// مسح جميع مسارات البحث / Clear all search paths
void ResourceManager::ClearSearchPaths() {
    std::lock_guard<std::mutex> lock(m_mutex);  // قفل / Lock
    
    m_searchPaths.clear();               // مسح / Clear
    std::cout << "(AR) تم مسح جميع مسارات البحث / (EN) Cleared all search paths" << std::endl;
}

/// البحث عن ملف / Search for file
std::string ResourceManager::FindFile(const std::string& filename) const {
    // التحقق من المسار المباشر / Check direct path
    if (fs::exists(filename)) {          // إذا كان موجوداً / If exists
        return filename;                 // إرجاع / Return
    }
    
    // البحث في المسارات / Search in paths
    for (const auto& searchPath : m_searchPaths) {
        fs::path fullPath = fs::path(searchPath) / filename;
        if (fs::exists(fullPath)) {      // إذا وُجد / If found
            return fullPath.string();    // إرجاع / Return
        }
    }
    
    return "";                           // لم يُوجد / Not found
}

// ==============================================================================
// الدوال المساعدة / Helper Functions
// ==============================================================================

/// توليد مفتاح خط / Generate font key
std::string ResourceManager::GenerateFontKey(const std::string& path, float fontSize) const {
    return path + "_" + std::to_string(static_cast<int>(fontSize));  // المسار + الحجم / Path + size
}

/// حساب حجم texture / Calculate texture size
size_t ResourceManager::CalculateTextureSize(const TextureRef& texture) const {
    if (!texture) return 0;              // إذا كان null / If null
    
    // حساب تقريبي / Approximate calculation
    u32 width = texture->GetWidth();     // العرض / Width
    u32 height = texture->GetHeight();   // الارتفاع / Height
    u32 channels = 4;                    // افتراض RGBA / Assume RGBA
    
    return width * height * channels;    // الحجم / Size
}

/// حساب حجم font / Calculate font size
size_t ResourceManager::CalculateFontSize(const FontRef& font) const {
    if (!font) return 0;                 // إذا كان null / If null
    
    // حساب تقريبي / Approximate calculation
    // حجم atlas + بيانات الخط / Atlas size + font data
    return 512 * 512 + 50000;            // تقريبي / Approximate
}

/// التحقق من الذاكرة / Check memory
bool ResourceManager::CheckMemory(size_t size) {
    // التحقق من الحد / Check limit
    return (m_stats.memoryUsed + size) <= m_memoryLimit;
}

/// تحرير ذاكرة / Free memory
bool ResourceManager::FreeMemory(size_t size) {
    // محاولة تفريغ غير المستخدم / Try to unload unused
    size_t freed = UnloadUnused() * 1000;  // تقريبي / Approximate
    
    return freed >= size;                // نجح؟ / Success?
}

/// الحصول على الوقت الحالي / Get current time
double ResourceManager::GetCurrentTime() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration<double>(duration).count();
}

} // namespace graphics
} // namespace sad
