/**
 * @file vm_gc.h
 * @brief نظام جمع القمامة المتقدم / Advanced Garbage Collector
 * @brief Tri-color marking with generational collection
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 2.0
 * 
 * @details
 * (AR) نظام GC متقدم مع:
 *      - خوارزمية التلوين الثلاثي (أبيض، رمادي، أسود)
 *      - جمع الأجيال (جيل صغير وجيل كبير)
 *      - حواجز الكتابة لتتبع المراجع
 *      - جمع تدريجي لتقليل التوقف
 *      - ضبط ديناميكي للعتبات
 * 
 * (EN) Advanced GC system with:
 *      - Tri-color marking algorithm (white, gray, black)
 *      - Generational collection (young/old generations)
 *      - Write barriers for reference tracking
 *      - Incremental collection for reduced pause times
 *      - Dynamic threshold adjustment
 */

#pragma once

#include "vm.h"
#include <vector>
#include <unordered_set>
#include <chrono>

namespace Sad {
namespace VM {

// ========================================
// GC Configuration - إعدادات GC
// ========================================

/**
 * @brief إعدادات جامع القمامة / GC Configuration
 */
struct GCConfig {
    // (AR) حجم الجيل الصغير بالبايت / (EN) Young generation size in bytes
    size_t youngGenSize = 1024 * 1024; // 1 MB
    
    // (AR) حجم الجيل الكبير بالبايت / (EN) Old generation size in bytes
    size_t oldGenSize = 8 * 1024 * 1024; // 8 MB
    
    // (AR) عتبة بدء GC للجيل الصغير / (EN) Young gen GC threshold
    size_t youngGenThreshold = 512 * 1024; // 512 KB
    
    // (AR) عتبة بدء GC للجيل الكبير / (EN) Old gen GC threshold
    size_t oldGenThreshold = 4 * 1024 * 1024; // 4 MB
    
    // (AR) عدد دورات الجيل الصغير قبل الترقية / (EN) Young gen cycles before promotion
    int promotionAge = 3;
    
    // (AR) عدد الكائنات في كل خطوة تدريجية / (EN) Objects per incremental step
    size_t incrementalStepSize = 100;
    
    // (AR) تفعيل الجمع التدريجي / (EN) Enable incremental collection
    bool enableIncremental = true;
    
    // (AR) تفعيل إحصائيات GC / (EN) Enable GC statistics
    bool enableStats = true;
    
    // (AR) طباعة معلومات التصحيح / (EN) Print debug info
    bool verbose = false;
};

// ========================================
// Object Colors - ألوان الكائنات
// ========================================

/**
 * @brief ألوان الكائنات في خوارزمية التلوين الثلاثي
 * @brief Object colors in tri-color marking
 */
enum class ObjectColor : uint8_t {
    WHITE = 0,  // (AR) أبيض: لم يُزَر بعد / (EN) White: not yet visited
    GRAY = 1,   // (AR) رمادي: تم زيارته لكن لم تُفحَص مراجعه / (EN) Gray: visited but children not scanned
    BLACK = 2   // (AR) أسود: تم زيارته وفحص جميع مراجعه / (EN) Black: visited and all children scanned
};

// ========================================
// Object Metadata - بيانات الكائن
// ========================================

/**
 * @brief بيانات إضافية لكل كائن لإدارة GC
 * @brief Additional metadata for each object for GC management
 */
struct ObjectMetadata {
    ObjectColor color = ObjectColor::WHITE;
    uint8_t generation = 0;  // (AR) 0 = صغير، 1 = كبير / (EN) 0 = young, 1 = old
    uint8_t age = 0;         // (AR) عدد دورات GC التي نجا منها / (EN) Number of GC cycles survived
    bool pinned = false;     // (AR) مثبّت (لا يُحرَّك) / (EN) Pinned (cannot be moved)
    size_t size = 0;         // (AR) حجم الكائن بالبايت / (EN) Object size in bytes
};

// ========================================
// GC Statistics - إحصائيات GC
// ========================================

/**
 * @brief إحصائيات أداء جامع القمامة
 * @brief Garbage collector performance statistics
 */
struct GCStats {
    // (AR) عدد دورات GC / (EN) Number of GC cycles
    uint64_t youngGenCollections = 0;
    uint64_t oldGenCollections = 0;
    uint64_t fullCollections = 0;
    
    // (AR) الوقت المستغرق في GC (ميلي ثانية) / (EN) Time spent in GC (milliseconds)
    uint64_t totalGCTime = 0;
    uint64_t youngGenTime = 0;
    uint64_t oldGenTime = 0;
    
    // (AR) عدد الكائنات / (EN) Object counts
    size_t objectsAllocated = 0;
    size_t objectsFreed = 0;
    size_t objectsPromoted = 0;
    
    // (AR) استخدام الذاكرة / (EN) Memory usage
    size_t bytesAllocated = 0;
    size_t bytesFreed = 0;
    size_t peakMemoryUsage = 0;
    
    // (AR) الكائنات الحية / (EN) Live objects
    size_t youngGenObjects = 0;
    size_t oldGenObjects = 0;
    
    // (AR) أطول وقف / (EN) Longest pause
    uint64_t maxPauseTime = 0;
    
    /**
     * @brief طباعة الإحصائيات / Print statistics
     */
    void print() const;
    
    /**
     * @brief إعادة تعيين الإحصائيات / Reset statistics
     */
    void reset();
};

// ========================================
// Garbage Collector - جامع القمامة
// ========================================

/**
 * @brief جامع القمامة المتقدم مع الأجيال والتلوين الثلاثي
 * @brief Advanced garbage collector with generations and tri-color marking
 */
class GarbageCollector {
public:
    /**
     * @brief المُنشئ / Constructor
     * @param vm مؤشر للآلة الافتراضية / Pointer to virtual machine
     * @param config إعدادات GC / GC configuration
     */
    explicit GarbageCollector(VirtualMachine* vm, const GCConfig& config = GCConfig{});
    
    /**
     * @brief المُدمِّر / Destructor
     */
    ~GarbageCollector();
    
    // ========================================
    // Allocation - التخصيص
    // ========================================
    
    /**
     * @brief تخصيص كائن جديد / Allocate new object
     * @param type نوع الكائن / Object type
     * @param size حجم الكائن / Object size
     * @return مؤشر للكائن المخصص / Pointer to allocated object
     */
    Object* allocate(ObjectType type, size_t size);
    
    /**
     * @brief تثبيت كائن (منع تحريكه) / Pin object (prevent moving)
     * @param obj الكائن المراد تثبيته / Object to pin
     */
    void pinObject(Object* obj);
    
    /**
     * @brief إلغاء تثبيت كائن / Unpin object
     * @param obj الكائن المراد إلغاء تثبيته / Object to unpin
     */
    void unpinObject(Object* obj);
    
    // ========================================
    // Collection - الجمع
    // ========================================
    
    /**
     * @brief جمع الجيل الصغير / Collect young generation
     */
    void collectYoung();
    
    /**
     * @brief جمع الجيل الكبير / Collect old generation
     */
    void collectOld();
    
    /**
     * @brief جمع كامل / Full collection
     */
    void collectFull();
    
    /**
     * @brief خطوة واحدة من الجمع التدريجي / One step of incremental collection
     * @return true إذا اكتملت الدورة / true if cycle completed
     */
    bool incrementalStep();
    
    /**
     * @brief تشغيل GC تلقائيًا إذا لزم الأمر / Run GC automatically if needed
     */
    void maybeCollect();
    
    // ========================================
    // Write Barriers - حواجز الكتابة
    // ========================================
    
    /**
     * @brief حاجز الكتابة: تسجيل مرجع من كائن إلى آخر
     * @brief Write barrier: record reference from one object to another
     * @param from الكائن المصدر / Source object
     * @param to الكائن الهدف / Target object
     */
    void writeBarrier(Object* from, Object* to);
    
    // ========================================
    // Roots Management - إدارة الجذور
    // ========================================
    
    /**
     * @brief إضافة جذر / Add root
     * @param obj كائن الجذر / Root object
     */
    void addRoot(Object* obj);
    
    /**
     * @brief إزالة جذر / Remove root
     * @param obj كائن الجذر / Root object
     */
    void removeRoot(Object* obj);
    
    // ========================================
    // Statistics & Configuration
    // ========================================
    
    /**
     * @brief الحصول على الإحصائيات / Get statistics
     * @return الإحصائيات الحالية / Current statistics
     */
    const GCStats& getStats() const { return stats_; }
    
    /**
     * @brief الحصول على الإعدادات / Get configuration
     * @return الإعدادات الحالية / Current configuration
     */
    const GCConfig& getConfig() const { return config_; }
    
    /**
     * @brief تحديث الإعدادات / Update configuration
     * @param config الإعدادات الجديدة / New configuration
     */
    void setConfig(const GCConfig& config);
    
    /**
     * @brief الحصول على استخدام الذاكرة الحالي / Get current memory usage
     * @return حجم الذاكرة المستخدمة بالبايت / Memory usage in bytes
     */
    size_t getMemoryUsage() const;
    
private:
    // ========================================
    // Internal Methods
    // ========================================
    
    /**
     * @brief تلوين جميع الكائنات بالأبيض / Mark all objects as white
     */
    void markWhite();
    
    /**
     * @brief تلوين الجذور بالرمادي / Mark roots as gray
     */
    void markRoots();
    
    /**
     * @brief معالجة جميع الكائنات الرمادية / Process all gray objects
     */
    void processGray();
    
    /**
     * @brief معالجة كائن واحد رمادي / Process one gray object
     * @param obj الكائن المراد معالجته / Object to process
     */
    void processObject(Object* obj);
    
    /**
     * @brief كنس الكائنات البيضاء / Sweep white objects
     * @return عدد الكائنات المُحرَّرة / Number of objects freed
     */
    size_t sweep();
    
    /**
     * @brief ترقية الكائنات الناجية إلى الجيل الكبير / Promote surviving objects to old generation
     */
    void promoteObjects();
    
    /**
     * @brief تحرير كائن / Free object
     * @param obj الكائن المراد تحريره / Object to free
     */
    void freeObject(Object* obj);
    
    /**
     * @brief الحصول على البيانات الإضافية لكائن / Get metadata for object
     * @param obj الكائن / Object
     * @return مؤشر للبيانات / Pointer to metadata
     */
    ObjectMetadata* getMetadata(Object* obj);
    
    /**
     * @brief تحديث العتبات ديناميكيًا / Update thresholds dynamically
     */
    void adjustThresholds();
    
    // ========================================
    // Member Variables
    // ========================================
    
    VirtualMachine* vm_;               // (AR) الآلة الافتراضية / (EN) Virtual machine
    GCConfig config_;                  // (AR) الإعدادات / (EN) Configuration
    GCStats stats_;                    // (AR) الإحصائيات / (EN) Statistics
    
    // (AR) قوائم الكائنات / (EN) Object lists
    std::vector<Object*> youngGen_;    // (AR) الجيل الصغير / (EN) Young generation
    std::vector<Object*> oldGen_;      // (AR) الجيل الكبير / (EN) Old generation
    std::vector<Object*> grayObjects_; // (AR) الكائنات الرمادية / (EN) Gray objects
    std::unordered_set<Object*> roots_;// (AR) الجذور / (EN) Roots
    
    // (AR) البيانات الإضافية / (EN) Metadata
    std::unordered_map<Object*, ObjectMetadata> metadata_;
    
    // (AR) حالة الجمع التدريجي / (EN) Incremental collection state
    bool incrementalInProgress_ = false;
    size_t incrementalPosition_ = 0;
    
    // (AR) أحجام الأجيال / (EN) Generation sizes
    size_t youngGenSize_ = 0;
    size_t oldGenSize_ = 0;
    
    // (AR) مؤقت الأداء / (EN) Performance timer
    std::chrono::high_resolution_clock::time_point gcStartTime_;
};

// ========================================
// Helper Functions - دوال مساعدة
// ========================================

/**
 * @brief تحويل لون الكائن إلى نص / Convert object color to string
 * @param color اللون / Color
 * @return اسم اللون / Color name
 */
const char* colorToString(ObjectColor color);

/**
 * @brief حساب حجم الكائن / Calculate object size
 * @param obj الكائن / Object
 * @return الحجم بالبايت / Size in bytes
 */
size_t calculateObjectSize(Object* obj);

} // namespace VM
} // namespace Sad
