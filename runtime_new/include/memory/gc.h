/*
 * بسم الله الرحمن الرحيم
 * ═══════════════════════════════════════════════════════════════════════════
 * ملف: gc.h
 * الوصف: جامع القمامة — نظام إدارة الذاكرة التلقائي للغة ص
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * الغرض من هذا الملف:
 * ──────────────────
 * يوفر نظام جمع قمامة تلقائي يتتبع الكائنات المخصصة ديناميكياً
 * ويحرر الذاكرة غير المستخدمة دون تدخل المبرمج. يستخدم خوارزمية
 * Mark-and-Sweep ثلاثية الألوان مع دعم اختياري لضغط الذاكرة.
 *
 * المكونات الرئيسية:
 * ─────────────────
 * - GCConfig: إعدادات جامع القمامة (حجم الكومة، عتبة التشغيل،
 *   معامل النمو، تفعيل التدريجي/المتزامن/الضغط)
 * - GCObject: الكائن القابل للجمع — يحمل النوع والحالة وحجم الكائن
 *   ودالة التتبع (trace_fn) ودالة التدمير (finalize_fn)
 * - GCObjectState: ألوان الكائن الثلاثة:
 *   • أبيض (White): لم يُزَر بعد — سيُحرَّر إذا بقي أبيض
 *   • رمادي (Gray): معلّم لكن مراجعه لم تُفحص بعد
 *   • أسود (Black): تم فحصه بالكامل — لن يُحرَّر
 * - GCRoot: جذر في مجموعة الجذور (Stack, Global, Register, Handle)
 * - GCStatistics: إحصائيات الأداء (عدد عمليات الجمع، الذاكرة المحررة،
 *   أقصى وقت توقف)
 * - GarbageCollector: النظام الرئيسي (Singleton) الذي يدير دورة الجمع
 *
 * خوارزمية الجمع (Mark-and-Sweep):
 * ─────────────────────────────────
 * 1. مرحلة التعليم (Mark): بدءاً من الجذور، اتبع جميع المراجع
 *    وعلّم الكائنات القابلة للوصول (أبيض → رمادي → أسود)
 * 2. مرحلة الكنس (Sweep): احذف جميع الكائنات البيضاء (غير القابلة للوصول)
 * 3. مرحلة الضغط (Compact — اختياري): أعد ترتيب الكائنات الحية
 *    في الذاكرة لتقليل التجزئة
 *
 * الموقع في البنية العامة:
 * ───────────────────────
 *   runtime_new/memory/
 *   ├── allocator.h ← نظام التخصيص (الطبقة الأدنى)
 *   └── [gc.h — هذا الملف] ← جامع القمامة (يبني فوق المخصص)
 *         ↑
 *   vm/vm.h يستدعي GC عند الحاجة أثناء التنفيذ
 *
 * الاعتماديات:
 * ──────────
 * - allocator.h: لتخصيص وتحرير الذاكرة الفعلية
 * - <atomic>: لعدادات المراجع والعلامات الذرية
 * - <functional>: لدوال التتبع والتدمير
 *
 * @file runtime/memory/gc.h
 * @author SadLang Compiler Team
 * @date December 2025
 * ═══════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_RUNTIME_GC_H
#define SAD_RUNTIME_GC_H

#include "allocator.h"
#include <vector>
#include <unordered_set>
#include <functional>
#include <atomic>

namespace Sad {
namespace Memory {

// ============================================================================
// Forward Declarations / تصريحات مسبقة
// ============================================================================

class GarbageCollector;
struct GCObject;

// ============================================================================
// GC Configuration / إعدادات جامع القمامة
// ============================================================================

/**
 * إعدادات جامع القمامة / GC configuration
 */
struct GCConfig {
    size_t heap_start_size;         // حجم الكومة الابتدائي
    size_t heap_max_size;           // الحد الأقصى للكومة
    size_t gc_threshold;            // عتبة تشغيل GC
    float growth_factor;            // معامل النمو
    bool enable_incremental;        // تفعيل GC التدريجي
    bool enable_concurrent;         // تفعيل GC المتزامن
    bool enable_compaction;         // تفعيل الضغط
    
    /**
     * الإعدادات الافتراضية / Default configuration
     */
    static GCConfig default_config() {
        return GCConfig{
            1024 * 1024,        // heap_start_size: 1 MB
            64 * 1024 * 1024,   // heap_max_size: 64 MB
            512 * 1024,         // gc_threshold: 512 KB
            2.0f,               // growth_factor: 2x
            false,              // enable_incremental
            false,              // enable_concurrent
            false               // enable_compaction
        };
    }
};

// ============================================================================
// GC Object / كائن GC
// ============================================================================

/**
 * نوع الكائن / Object type
 */
enum class GCObjectType {
    Integer,        // عدد صحيح
    Float,          // عدد عشري
    String,         // نص
    Array,          // مصفوفة
    Object,         // كائن
    Function,       // دالة
    Closure,        // إغلاق
    Native          // كائن أصلي
};

/**
 * حالة الكائن / Object state
 */
enum class GCObjectState {
    White,          // أبيض (غير معلم)
    Gray,           // رمادي (معلم، لكن الأبناء لم يتم فحصهم)
    Black           // أسود (معلم والأبناء تم فحصهم)
};

/**
 * كائن قابل لجمع القمامة / Garbage collectable object
 */
struct GCObject {
    GCObjectType type;              // نوع الكائن
    GCObjectState state;            // حالة الكائن
    size_t size;                    // حجم الكائن
    uint32_t ref_count;             // عداد المراجع
    bool pinned;                    // مثبت؟
    GCObject* next;                 // التالي في القائمة
    
    /**
     * عنوان الإعادة التوجيه / Forwarding address
     * يُستخدم أثناء الضغط لتحديث المراجع
     * Used during compaction to update references
     */
    void* forwarding_address;
    
    /**
     * دالة التتبع / Trace function
     * تُستدعى للعثور على جميع المراجع
     */
    std::function<void(GarbageCollector*)> trace_fn;
    
    /**
     * دالة التدمير / Finalizer function
     * تُستدعى قبل تحرير الكائن
     */
    std::function<void()> finalize_fn;
    
    /**
     * بيانات الكائن / Object data
     * يتبع هذا الهيكل مباشرة في الذاكرة
     */
    uint8_t data[1];
};

// ============================================================================
// Root Set / مجموعة الجذور
// ============================================================================

/**
 * نوع الجذر / Root type
 */
enum class RootType {
    Stack,          // متغير في الـ stack
    Global,         // متغير عام
    Register,       // سجل
    Handle          // مقبض
};

/**
 * جذر GC / GC root
 */
struct GCRoot {
    RootType type;          // نوع الجذر
    void* location;         // موقع الجذر
    const char* name;       // اسم (للتصحيح)
};

// ============================================================================
// GC Statistics / إحصائيات جامع القمامة
// ============================================================================

/**
 * إحصائيات GC / GC statistics
 */
struct GCStatistics {
    size_t total_collections;       // إجمالي عمليات الجمع
    size_t compactions;             // عدد عمليات الضغط
    size_t total_objects;           // إجمالي الكائنات
    size_t live_objects;            // الكائنات الحية
    size_t dead_objects;            // الكائنات الميتة
    size_t total_memory;            // إجمالي الذاكرة
    size_t used_memory;             // الذاكرة المستخدمة
    size_t freed_memory;            // الذاكرة المحررة
    size_t bytes_freed;             // البايتات المحررة
    uint64_t last_collection_time;  // وقت آخر جمع (ميكروثانية)
    uint64_t total_collection_time; // إجمالي وقت الجمع
    uint64_t max_pause_time;        // أقصى وقت توقف
};

// ============================================================================
// Garbage Collector / جامع القمامة
// ============================================================================

/**
 * جامع القمامة الرئيسي
 * Main garbage collector
 */
class GarbageCollector {
public:
    /**
     * الحصول على النسخة الوحيدة / Get singleton instance
     */
    static GarbageCollector& instance();
    
    /**
     * تهيئة جامع القمامة / Initialize GC
     */
    bool initialize(const GCConfig* config = nullptr);
    
    /**
     * إيقاف جامع القمامة / Shutdown GC
     */
    void shutdown();
    
    /**
     * تخصيص كائن / Allocate object
     * 
     * @param type نوع الكائن
     * @param size حجم البيانات
     * @return مؤشر الكائن أو nullptr
     */
    GCObject* allocate(GCObjectType type, size_t size);
    
    /**
     * تسجيل جذر / Register root
     */
    void register_root(void* location, RootType type, const char* name = nullptr);
    
    /**
     * إلغاء تسجيل جذر / Unregister root
     */
    void unregister_root(void* location);
    
    /**
     * تشغيل جمع القمامة / Run garbage collection
     * 
     * @return عدد البايتات المحررة
     */
    size_t collect();
    
    /**
     * فحص ما إذا كان يجب تشغيل GC / Check if GC should run
     */
    bool should_collect() const;
    
    /**
     * زيادة عداد المراجع / Increment reference count
     */
    void retain(GCObject* obj);
    
    /**
     * إنقاص عداد المراجع / Decrement reference count
     */
    void release(GCObject* obj);
    
    /**
     * تثبيت كائن / Pin object
     */
    void pin(GCObject* obj);
    
    /**
     * إلغاء تثبيت كائن / Unpin object
     */
    void unpin(GCObject* obj);
    
    /**
     * كتابة حاجز / Write barrier
     * يُستدعى عند تحديث مرجع
     */
    void write_barrier(GCObject* parent, GCObject* child);
    
    /**
     * الحصول على الإحصائيات / Get statistics
     */
    GCStatistics get_statistics() const;
    
    /**
     * إعادة تعيين الإحصائيات / Reset statistics
     */
    void reset_statistics();
    
    /**
     * طباعة الإحصائيات / Print statistics
     */
    void print_statistics() const;
    
    /**
     * تفعيل/تعطيل GC / Enable/disable GC
     */
    void enable(bool enabled) { enabled_ = enabled; }
    bool is_enabled() const { return enabled_; }
    
    /**
     * تعيين عتبة GC / Set GC threshold
     */
    void set_threshold(size_t threshold) { config_.gc_threshold = threshold; }
    
private:
    /**
     * مُنشئ خاص (Singleton) / Private constructor
     */
    GarbageCollector();
    
    /**
     * مُدمر / Destructor
     */
    ~GarbageCollector();
    
    // منع النسخ / Prevent copying
    GarbageCollector(const GarbageCollector&) = delete;
    GarbageCollector& operator=(const GarbageCollector&) = delete;
    
    /**
     * مرحلة التعليم / Mark phase
     */
    void mark();
    
    /**
     * تعليم كائن / Mark object
     */
    void mark_object(GCObject* obj);
    
    /**
     * مرحلة المسح / Sweep phase
     */
    size_t sweep();
    
    /**
     * مرحلة الضغط / Compact phase
     */
    void compact();
    
    /**
     * تتبع الجذور / Trace roots
     */
    void trace_roots();
    
    /**
     * معالجة القائمة الرمادية / Process gray list
     */
    void process_gray_list();
    
    /**
     * إعادة تعيين الحالات / Reset states
     */
    void reset_states();
    
    bool initialized_;                          // حالة التهيئة
    bool enabled_;                              // حالة التفعيل
    std::atomic<bool> collecting_;              // قيد الجمع؟
    
    GCConfig config_;                           // الإعدادات
    
    GCObject* objects_list_;                    // قائمة جميع الكائنات
    std::vector<GCObject*> gray_list_;          // القائمة الرمادية
    std::vector<GCRoot> roots_;                 // الجذور
    
    size_t allocated_bytes_;                    // البايتات المخصصة
    size_t threshold_;                          // العتبة الحالية
    
    mutable std::mutex gc_mutex_;               // قفل GC
    mutable GCStatistics stats_;                        // الإحصائيات
};

// ============================================================================
// Smart Pointers / مؤشرات ذكية
// ============================================================================

/**
 * مؤشر ذكي لكائن GC
 * Smart pointer for GC object
 */
template<typename T>
class GCPtr {
public:
    GCPtr() : obj_(nullptr) {}
    
    explicit GCPtr(GCObject* obj) : obj_(obj) {
        if (obj_) {
            GarbageCollector::instance().retain(obj_);
        }
    }
    
    GCPtr(const GCPtr& other) : obj_(other.obj_) {
        if (obj_) {
            GarbageCollector::instance().retain(obj_);
        }
    }
    
    GCPtr(GCPtr&& other) noexcept : obj_(other.obj_) {
        other.obj_ = nullptr;
    }
    
    ~GCPtr() {
        if (obj_) {
            GarbageCollector::instance().release(obj_);
        }
    }
    
    GCPtr& operator=(const GCPtr& other) {
        if (this != &other) {
            if (obj_) {
                GarbageCollector::instance().release(obj_);
            }
            obj_ = other.obj_;
            if (obj_) {
                GarbageCollector::instance().retain(obj_);
            }
        }
        return *this;
    }
    
    GCPtr& operator=(GCPtr&& other) noexcept {
        if (this != &other) {
            if (obj_) {
                GarbageCollector::instance().release(obj_);
            }
            obj_ = other.obj_;
            other.obj_ = nullptr;
        }
        return *this;
    }
    
    T* get() const {
        return obj_ ? reinterpret_cast<T*>(obj_->data) : nullptr;
    }
    
    T* operator->() const { return get(); }
    T& operator*() const { return *get(); }
    
    explicit operator bool() const { return obj_ != nullptr; }
    
    GCObject* raw() const { return obj_; }
    
private:
    GCObject* obj_;
};

// ============================================================================
// Helper Macros / ماكروهات مساعدة
// ============================================================================

/**
 * تخصيص كائن GC / Allocate GC object
 */
#define GC_ALLOC(type, size) \
    Sad::Memory::GarbageCollector::instance().allocate(type, size)

/**
 * تسجيل جذر / Register root
 */
#define GC_ROOT(var) \
    Sad::Memory::GarbageCollector::instance().register_root( \
        &(var), Sad::Memory::RootType::Stack, #var)

/**
 * إلغاء تسجيل جذر / Unregister root
 */
#define GC_UNROOT(var) \
    Sad::Memory::GarbageCollector::instance().unregister_root(&(var))

/**
 * تشغيل GC / Run GC
 */
#define GC_COLLECT() \
    Sad::Memory::GarbageCollector::instance().collect()

/**
 * حاجز الكتابة / Write barrier
 */
#define GC_WRITE_BARRIER(parent, child) \
    Sad::Memory::GarbageCollector::instance().write_barrier(parent, child)

} // namespace Memory
} // namespace Sad

#endif // SAD_RUNTIME_GC_H
