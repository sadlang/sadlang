// ============================================================================
// jit_cache.h - نظام الذاكرة المؤقتة لمحرك JIT
// JIT Cache System for Compiled Code
// ============================================================================
// الغرض: إدارة ذاكرة مؤقتة ذكية للكود المُجمّع
// Purpose: Manage intelligent cache for compiled code
// ============================================================================

#ifndef SAD_JIT_CACHE_H
#define SAD_JIT_CACHE_H

#include <memory>        // لإدارة الذاكرة الذكية / For smart pointers
#include <string>        // للنصوص / For strings
#include <unordered_map> // للخرائط السريعة / For fast maps
#include <vector>        // للقوائم الديناميكية / For dynamic arrays
#include <chrono>        // للوقت / For time
#include <mutex>         // للتزامن / For thread safety
#include <fstream>       // للملفات / For file I/O

namespace Sad {
namespace JIT {

// ============================================================================
// CacheEntry - عنصر في الذاكرة المؤقتة / Cache Entry
// ============================================================================

struct CacheEntry {
    // ========================================
    // البيانات الأساسية / Basic Data
    // ========================================
    
    std::string function_name;       // اسم الدالة / Function name
    void* compiled_code;             // مؤشر للكود المُجمّع / Pointer to compiled code
    size_t code_size_bytes;          // حجم الكود (bytes) / Code size (bytes)
    
    // ========================================
    // معلومات التجميع / Compilation Info
    // ========================================
    
    std::string source_hash;         // hash للكود المصدري / Hash of source code
    int optimization_level;          // مستوى التحسين المستخدم / Optimization level used
    std::string target_triple;       // معمارية الهدف / Target architecture
    
    // ========================================
    // إحصائيات الاستخدام / Usage Statistics
    // ========================================
    
    size_t hit_count;                // عدد مرات الاستخدام / Usage count
    size_t execution_count;          // عدد مرات التنفيذ / Execution count
    
    std::chrono::system_clock::time_point created_at;    // وقت الإنشاء / Creation time
    std::chrono::system_clock::time_point last_accessed; // آخر وصول / Last access
    
    double total_execution_time_ms;  // الوقت الكلي للتنفيذ (ms) / Total execution time (ms)
    double avg_execution_time_ms;    // متوسط وقت التنفيذ (ms) / Average execution time (ms)
    
    // ========================================
    // معلومات إضافية / Additional Info
    // ========================================
    
    bool is_hot;                     // هل الدالة ساخنة؟ / Is function hot?
    int tier_level;                  // مستوى التجميع المُدرّج (0-3) / Tiering level (0-3)
    
    // ========================================
    // المُنشئ / Constructor
    // ========================================
    
    CacheEntry()
        : compiled_code(nullptr)
        , code_size_bytes(0)
        , optimization_level(0)
        , hit_count(0)
        , execution_count(0)
        , created_at(std::chrono::system_clock::now())
        , last_accessed(std::chrono::system_clock::now())
        , total_execution_time_ms(0.0)
        , avg_execution_time_ms(0.0)
        , is_hot(false)
        , tier_level(0)
    {}
    
    // ========================================
    // تسجيل وصول / Record Access
    // ========================================
    
    void recordAccess() {
        hit_count++;
        last_accessed = std::chrono::system_clock::now();
    }
    
    // ========================================
    // تسجيل تنفيذ / Record Execution
    // ========================================
    
    void recordExecution(double execution_time_ms) {
        execution_count++;
        total_execution_time_ms += execution_time_ms;
        avg_execution_time_ms = total_execution_time_ms / execution_count;
        last_accessed = std::chrono::system_clock::now();
    }
    
    // ========================================
    // حساب الأولوية / Calculate Priority
    // ========================================
    
    double getPriority() const {
        // الأولوية تعتمد على: عدد الاستخدامات، حداثة الوصول، حجم الكود
        // Priority depends on: usage count, access recency, code size
        
        auto now = std::chrono::system_clock::now();
        auto time_since_access = std::chrono::duration_cast<std::chrono::seconds>(
            now - last_accessed
        ).count();
        
        // معامل الحداثة (أقل = أحدث = أفضل) / Recency factor (lower = newer = better)
        double recency_factor = 1.0 / (1.0 + time_since_access);
        
        // معامل التردد / Frequency factor
        double frequency_factor = static_cast<double>(hit_count);
        
        // معامل الحجم (أصغر = أفضل) / Size factor (smaller = better)
        double size_factor = 1.0 / (1.0 + code_size_bytes / 1024.0);
        
        // الأولوية الكلية / Total priority
        return (frequency_factor * 0.5) + (recency_factor * 0.3) + (size_factor * 0.2);
    }
};

// ============================================================================
// CacheEvictionPolicy - سياسة الإخلاء / Eviction Policy
// ============================================================================

enum class CacheEvictionPolicy {
    LRU,        // الأقل استخداماً حديثاً / Least Recently Used
    LFU,        // الأقل استخداماً / Least Frequently Used
    PRIORITY,   // بناءً على الأولوية / Based on priority
    SIZE,       // الأكبر حجماً / Largest size
    FIFO        // الأول دخولاً الأول خروجاً / First In First Out
};

// ============================================================================
// JITCache - نظام الذاكرة المؤقتة / Cache System
// ============================================================================

class JITCache {
public:
    // ========================================
    // المُنشئ والمُدمر / Constructor & Destructor
    // ========================================
    
    // المُنشئ / Constructor
    // max_size_mb: الحد الأقصى لحجم الذاكرة المؤقتة (MB) / Max cache size (MB)
    // eviction_policy: سياسة الإخلاء / Eviction policy
    explicit JITCache(
        size_t max_size_mb = 256,
        CacheEvictionPolicy eviction_policy = CacheEvictionPolicy::LRU
    );
    
    // المُدمر / Destructor
    ~JITCache();
    
    // منع النسخ / Prevent copying
    JITCache(const JITCache&) = delete;
    JITCache& operator=(const JITCache&) = delete;
    
    // ========================================
    // العمليات الأساسية / Basic Operations
    // ========================================
    
    // إضافة عنصر إلى الذاكرة المؤقتة / Add entry to cache
    // entry: العنصر المراد إضافته / Entry to add
    // returns: true إذا نجحت الإضافة / true if addition succeeded
    bool put(const CacheEntry& entry);
    
    // البحث عن عنصر / Lookup entry
    // function_name: اسم الدالة / Function name
    // returns: مؤشر للعنصر (أو nullptr) / Pointer to entry (or nullptr)
    CacheEntry* get(const std::string& function_name);
    
    // البحث عن عنصر (ثابت) / Lookup entry (const)
    const CacheEntry* get(const std::string& function_name) const;
    
    // هل العنصر موجود؟ / Does entry exist?
    bool contains(const std::string& function_name) const;
    
    // حذف عنصر / Remove entry
    // function_name: اسم الدالة / Function name
    // returns: true إذا تم الحذف / true if removed
    bool remove(const std::string& function_name);
    
    // مسح الذاكرة المؤقتة بالكامل / Clear entire cache
    void clear();
    
    // ========================================
    // إدارة الحجم / Size Management
    // ========================================
    
    // الحصول على الحجم الحالي (bytes) / Get current size (bytes)
    size_t getCurrentSize() const;
    
    // الحصول على عدد العناصر / Get number of entries
    size_t getCount() const;
    
    // هل الذاكرة المؤقتة ممتلئة؟ / Is cache full?
    bool isFull() const;
    
    // الحصول على نسبة الاستخدام (0-100) / Get usage percentage (0-100)
    double getUsagePercentage() const;
    
    // ========================================
    // سياسة الإخلاء / Eviction Policy
    // ========================================
    
    // تطبيق سياسة الإخلاء / Apply eviction policy
    // required_space: المساحة المطلوبة (bytes) / Required space (bytes)
    // returns: true إذا تم توفير المساحة / true if space was freed
    bool evict(size_t required_space);
    
    // إخلاء عنصر واحد / Evict single entry
    // returns: true إذا تم الإخلاء / true if evicted
    bool evictOne();
    
    // تغيير سياسة الإخلاء / Change eviction policy
    void setEvictionPolicy(CacheEvictionPolicy policy);
    
    // الحصول على سياسة الإخلاء الحالية / Get current eviction policy
    CacheEvictionPolicy getEvictionPolicy() const;
    
    // ========================================
    // الإحصائيات / Statistics
    // ========================================
    
    // عدد الإصابات / Hit count
    size_t getHitCount() const { return hit_count_; }
    
    // عدد الإخفاقات / Miss count
    size_t getMissCount() const { return miss_count_; }
    
    // نسبة الإصابة (0-100) / Hit rate (0-100)
    double getHitRate() const;
    
    // عدد الإخلاءات / Eviction count
    size_t getEvictionCount() const { return eviction_count_; }
    
    // إعادة تعيين الإحصائيات / Reset statistics
    void resetStatistics();
    
    // ========================================
    // الذاكرة المؤقتة الدائمة / Persistent Cache
    // ========================================
    
    // حفظ الذاكرة المؤقتة إلى ملف / Save cache to file
    // file_path: مسار الملف / File path
    // returns: true إذا نجح الحفظ / true if save succeeded
    bool saveToDisk(const std::string& file_path) const;
    
    // تحميل الذاكرة المؤقتة من ملف / Load cache from file
    // file_path: مسار الملف / File path
    // returns: true إذا نجح التحميل / true if load succeeded
    bool loadFromDisk(const std::string& file_path);
    
    // ========================================
    // التصدير والتحليل / Export & Analysis
    // ========================================
    
    // الحصول على جميع العناصر / Get all entries
    std::vector<CacheEntry> getAllEntries() const;
    
    // الحصول على العناصر الساخنة / Get hot entries
    std::vector<CacheEntry> getHotEntries() const;
    
    // طباعة معلومات الذاكرة المؤقتة / Print cache information
    void printInfo() const;
    
    // تصدير إلى JSON / Export to JSON
    std::string toJSON() const;

private:
    // ========================================
    // البيانات الداخلية / Internal Data
    // ========================================
    
    std::unordered_map<std::string, CacheEntry> cache_;  // الذاكرة المؤقتة / Cache storage
    
    size_t max_size_bytes_;          // الحد الأقصى للحجم (bytes) / Max size (bytes)
    size_t current_size_bytes_;      // الحجم الحالي (bytes) / Current size (bytes)
    
    CacheEvictionPolicy eviction_policy_; // سياسة الإخلاء / Eviction policy
    
    // ========================================
    // الإحصائيات / Statistics
    // ========================================
    
    size_t hit_count_;               // عدد الإصابات / Hit count
    size_t miss_count_;              // عدد الإخفاقات / Miss count
    size_t eviction_count_;          // عدد الإخلاءات / Eviction count
    
    mutable std::mutex mutex_;       // قفل للتزامن / Mutex for thread safety
    
    // ========================================
    // الدوال المساعدة الداخلية / Internal Helper Functions
    // ========================================
    
    // اختيار عنصر للإخلاء بناءً على السياسة / Select entry to evict based on policy
    std::string selectVictim();
    
    // اختيار عنصر LRU / Select LRU entry
    std::string selectLRU();
    
    // اختيار عنصر LFU / Select LFU entry
    std::string selectLFU();
    
    // اختيار عنصر بأقل أولوية / Select lowest priority entry
    std::string selectLowestPriority();
    
    // اختيار أكبر عنصر / Select largest entry
    std::string selectLargest();
    
    // اختيار عنصر FIFO / Select FIFO entry
    std::string selectFIFO();
};

} // namespace JIT
} // namespace Sad

#endif // SAD_JIT_CACHE_H

// ============================================================================
// نهاية الملف / End of File
// ============================================================================
// ملاحظات التنفيذ / Implementation Notes:
// - يدعم عدة سياسات للإخلاء (LRU, LFU, PRIORITY, SIZE, FIFO)
// - Supports multiple eviction policies (LRU, LFU, PRIORITY, SIZE, FIFO)
// - Thread-safe باستخدام mutex
// - Thread-safe using mutex
// - يدعم الحفظ والتحميل من القرص
// - Supports save/load from disk
// - إحصائيات مفصلة للأداء
// - Detailed performance statistics
// ============================================================================
