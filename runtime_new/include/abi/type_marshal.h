/*
 * بسم الله الرحمن الرحيم
 * ================================
 * نظام تحويل الأنواع / Type Marshalling System
 * ================================
 * 
 * يدير التحويل الآمن بين أنواع لغة ص و C
 * Manages safe type conversion between Sad and C types
 * 
 * المكونات / Components:
 * - Basic type conversion
 * - Complex type handling (arrays, structs, strings)
 * - Type validation and verification
 * - Memory ownership management
 * 
 * @file runtime/abi/type_marshal.h
 * @author SadLang Compiler Team
 * @date December 2025
 */

#ifndef SAD_RUNTIME_TYPE_MARSHAL_H
#define SAD_RUNTIME_TYPE_MARSHAL_H

#include "c_abi.h"
#include <memory>
#include <functional>
#include <unordered_map>

namespace Sad {
namespace ABI {

// ============================================================================
// Type Marshaller Class / فئة محول الأنواع
// ============================================================================

/**
 * نظام تحويل الأنواع الرئيسي / Main type marshalling system
 * 
 * يوفر تحويل آمن وفعال بين أنواع Sad و C
 * Provides safe and efficient conversion between Sad and C types
 */
class TypeMarshaller {
public:
    TypeMarshaller();
    ~TypeMarshaller();
    
    // ========================================================================
    // Basic Type Conversion / تحويل الأنواع الأساسية
    // ========================================================================
    
    /**
     * تحويل من Sad إلى C / Convert from Sad to C
     */
    
    // رقم → int64_t / Integer → int64_t
    int64_t sad_integer_to_c(void* sad_int) const;
    
    // عشري → double / Float → double
    double sad_float_to_c(void* sad_float) const;
    
    // منطقي → bool / Boolean → bool
    bool sad_boolean_to_c(void* sad_bool) const;
    
    // نص → const char* / String → const char*
    const char* sad_string_to_c(void* sad_string) const;
    
    // مؤشر → void* / Pointer → void*
    void* sad_pointer_to_c(void* sad_ptr) const;
    
    /**
     * تحويل من C إلى Sad / Convert from C to Sad
     */
    
    // int64_t → رقم / int64_t → Integer
    void* c_integer_to_sad(int64_t c_int) const;
    
    // double → عشري / double → Float
    void* c_float_to_sad(double c_float) const;
    
    // bool → منطقي / bool → Boolean
    void* c_boolean_to_sad(bool c_bool) const;
    
    // const char* → نص / const char* → String
    void* c_string_to_sad(const char* c_string) const;
    
    // void* → مؤشر / void* → Pointer
    void* c_pointer_to_sad(void* c_ptr) const;
    
    // ========================================================================
    // Complex Type Conversion / تحويل الأنواع المعقدة
    // ========================================================================
    
    /**
     * تحويل المصفوفات / Array conversion
     */
    
    // Sad array → C array
    struct CArray {
        void* data;
        size_t length;
        size_t element_size;
    };
    
    CArray sad_array_to_c(void* sad_array, const TypeInfo& element_type) const;
    void* c_array_to_sad(const CArray& c_array, const TypeInfo& element_type) const;
    
    /**
     * تحويل البنى / Struct conversion
     */
    
    void* sad_struct_to_c(void* sad_struct, const StructDescriptor& desc) const;
    void* c_struct_to_sad(void* c_struct, const StructDescriptor& desc) const;
    
    /**
     * تحويل مؤشرات الدوال / Function pointer conversion
     */
    
    void* sad_function_to_c(void* sad_func, const FunctionSignature& sig) const;
    void* c_function_to_sad(void* c_func, const FunctionSignature& sig) const;
    
    // ========================================================================
    // Generic Conversion / التحويل العام
    // ========================================================================
    
    /**
     * تحويل عام من Sad إلى C / Generic Sad → C conversion
     */
    void* sad_to_c(void* sad_value, const TypeInfo& type) const;
    
    /**
     * تحويل عام من C إلى Sad / Generic C → Sad conversion
     */
    void* c_to_sad(void* c_value, const TypeInfo& type) const;
    
    /**
     * تحويل مجموعة قيم / Convert multiple values
     */
    std::vector<void*> sad_values_to_c(
        const std::vector<void*>& sad_values,
        const std::vector<TypeInfo>& types
    ) const;
    
    std::vector<void*> c_values_to_sad(
        const std::vector<void*>& c_values,
        const std::vector<TypeInfo>& types
    ) const;
    
    // ========================================================================
    // Type Validation / التحقق من الأنواع
    // ========================================================================
    
    /**
     * التحقق من صحة النوع / Validate type
     */
    bool validate_type(const TypeInfo& type) const;
    
    /**
     * التحقق من توافق الأنواع / Check type compatibility
     */
    bool are_types_compatible(const TypeInfo& sad_type, const TypeInfo& c_type) const;
    
    /**
     * التحقق من صحة القيمة / Validate value
     */
    bool validate_value(void* value, const TypeInfo& type) const;
    
    /**
     * التحقق من صحة التحويل / Validate conversion
     */
    bool can_convert(const TypeInfo& from, const TypeInfo& to) const;
    
    // ========================================================================
    // Memory Management / إدارة الذاكرة
    // ========================================================================
    
    /**
     * تسجيل قيمة محولة للتنظيف / Register converted value for cleanup
     */
    void register_conversion(void* value, const TypeInfo& type);
    
    /**
     * تنظيف القيم المحولة / Cleanup converted values
     */
    void cleanup_conversions();
    
    /**
     * تنظيف قيمة واحدة / Cleanup single value
     */
    void cleanup_value(void* value, const TypeInfo& type);
    
    /**
     * وضع إدارة الذاكرة / Memory management mode
     */
    enum class MemoryMode {
        Automatic,  // تنظيف تلقائي / Automatic cleanup
        Manual,     // تنظيف يدوي / Manual cleanup
        Shared      // ملكية مشتركة / Shared ownership
    };
    
    void set_memory_mode(MemoryMode mode) { memory_mode_ = mode; }
    MemoryMode get_memory_mode() const { return memory_mode_; }
    
    // ========================================================================
    // Custom Converters / محولات مخصصة
    // ========================================================================
    
    /**
     * نوع دالة التحويل المخصصة / Custom converter function type
     */
    using CustomConverter = std::function<void*(void*, const TypeInfo&)>;
    
    /**
     * تسجيل محول مخصص / Register custom converter
     */
    void register_sad_to_c_converter(
        SadType sad_type,
        CustomConverter converter
    );
    
    void register_c_to_sad_converter(
        CType c_type,
        CustomConverter converter
    );
    
    /**
     * إلغاء تسجيل محول / Unregister converter
     */
    void unregister_sad_to_c_converter(SadType sad_type);
    void unregister_c_to_sad_converter(CType c_type);
    
    // ========================================================================
    // Error Handling / معالجة الأخطاء
    // ========================================================================
    
    enum class ErrorCode {
        Success,
        NullPointer,
        InvalidType,
        TypeMismatch,
        AllocationFailed,
        ConversionFailed,
        ValidationFailed,
        UnknownError
    };
    
    struct Error {
        ErrorCode code;
        std::string message;
        TypeInfo source_type;
        TypeInfo target_type;
        
        Error() : code(ErrorCode::Success) {}
        Error(ErrorCode c, const std::string& msg)
            : code(c), message(msg) {}
        
        bool has_error() const { return code != ErrorCode::Success; }
    };
    
    Error get_last_error() const { return last_error_; }
    void clear_error() { last_error_ = Error(); }
    
    // ========================================================================
    // Statistics & Debugging / الإحصائيات والتنقيح
    // ========================================================================
    
    struct Stats {
        size_t total_conversions;       // مجموع التحويلات
        size_t sad_to_c_conversions;    // Sad → C
        size_t c_to_sad_conversions;    // C → Sad
        size_t failed_conversions;      // فشل
        size_t cached_conversions;      // من الذاكرة المؤقتة
        size_t memory_allocated;        // الذاكرة المخصصة
        size_t active_conversions;      // التحويلات النشطة
    };
    
    Stats get_stats() const { return stats_; }
    void reset_stats();
    
    /**
     * تفعيل وضع التنقيح / Enable debug mode
     */
    void set_debug_mode(bool enabled) { debug_mode_ = enabled; }
    bool is_debug_mode() const { return debug_mode_; }
    
    /**
     * تفعيل التخزين المؤقت / Enable caching
     */
    void set_caching_enabled(bool enabled) { caching_enabled_ = enabled; }
    bool is_caching_enabled() const { return caching_enabled_; }
    
private:
    // ========================================================================
    // Private Helper Methods / طرق مساعدة خاصة
    // ========================================================================
    
    /**
     * تعيين خطأ / Set error
     */
    void set_error(
        ErrorCode code,
        const std::string& message,
        const TypeInfo& source = TypeInfo(),
        const TypeInfo& target = TypeInfo()
    );
    
    /**
     * تسجيل معلومات تنقيح / Log debug info
     */
    void log_debug(const std::string& message);
    
    /**
     * نسخ البيانات / Copy data
     */
    void* copy_data(const void* data, size_t size);
    
    /**
     * تخصيص ذاكرة / Allocate memory
     */
    void* allocate(size_t size, size_t alignment = alignof(std::max_align_t));
    
    /**
     * تحرير ذاكرة / Free memory
     */
    void deallocate(void* ptr);
    
    /**
     * البحث في الذاكرة المؤقتة / Cache lookup
     */
    void* cache_lookup(void* value, const TypeInfo& type) const;
    void cache_store(void* original, void* converted, const TypeInfo& type);
    
    // ========================================================================
    // Private Members / الأعضاء الخاصة
    // ========================================================================
    
    // Custom converters / محولات مخصصة
    std::unordered_map<SadType, CustomConverter> sad_to_c_converters_;
    std::unordered_map<CType, CustomConverter> c_to_sad_converters_;
    
    // Active conversions for cleanup / التحويلات النشطة للتنظيف
    struct ConversionRecord {
        void* value;
        TypeInfo type;
        bool needs_cleanup;
    };
    std::vector<ConversionRecord> active_conversions_;
    
    // Conversion cache / ذاكرة مؤقتة للتحويلات
    struct CacheEntry {
        void* original;
        void* converted;
        TypeInfo type;
    };
    std::vector<CacheEntry> conversion_cache_;
    
    // Configuration / التكوين
    MemoryMode memory_mode_;
    bool debug_mode_;
    bool caching_enabled_;
    size_t max_cache_size_;
    
    // Statistics / الإحصائيات
    mutable Stats stats_;
    
    // Error state / حالة الخطأ
    mutable Error last_error_;
    
    // ABI reference / مرجع ABI
    std::unique_ptr<CABI> abi_;
};

// ============================================================================
// Helper Classes / فئات مساعدة
// ============================================================================

/**
 * مدير التحويل التلقائي / Auto-conversion manager
 * 
 * يدير التحويل التلقائي مع التنظيف عند الانتهاء
 * Manages automatic conversion with cleanup on scope exit
 */
class AutoConversion {
public:
    AutoConversion(TypeMarshaller& marshaller, void* value, const TypeInfo& type)
        : marshaller_(marshaller)
        , value_(value)
        , type_(type)
    {
        marshaller_.register_conversion(value_, type_);
    }
    
    ~AutoConversion() {
        if (value_) {
            marshaller_.cleanup_value(value_, type_);
        }
    }
    
    // منع النسخ / Prevent copying
    AutoConversion(const AutoConversion&) = delete;
    AutoConversion& operator=(const AutoConversion&) = delete;
    
    // السماح بالنقل / Allow moving
    AutoConversion(AutoConversion&& other) noexcept
        : marshaller_(other.marshaller_)
        , value_(other.value_)
        , type_(other.type_)
    {
        other.value_ = nullptr;
    }
    
    void* get() const { return value_; }
    void release() { value_ = nullptr; }
    
private:
    TypeMarshaller& marshaller_;
    void* value_;
    TypeInfo type_;
};

/**
 * مجموعة تحويلات / Conversion batch
 * 
 * تحويل مجموعة من القيم دفعة واحدة
 * Convert multiple values in a batch
 */
class ConversionBatch {
public:
    ConversionBatch(TypeMarshaller& marshaller)
        : marshaller_(marshaller)
    {}
    
    ~ConversionBatch() {
        cleanup();
    }
    
    /**
     * إضافة تحويل / Add conversion
     */
    void add_sad_to_c(void* sad_value, const TypeInfo& type);
    void add_c_to_sad(void* c_value, const TypeInfo& type);
    
    /**
     * تنفيذ جميع التحويلات / Execute all conversions
     */
    bool execute();
    
    /**
     * الحصول على النتائج / Get results
     */
    const std::vector<void*>& get_results() const { return results_; }
    
    /**
     * تنظيف / Cleanup
     */
    void cleanup();
    
private:
    TypeMarshaller& marshaller_;
    
    struct Conversion {
        void* value;
        TypeInfo type;
        bool is_sad_to_c;
    };
    
    std::vector<Conversion> conversions_;
    std::vector<void*> results_;
};

// ============================================================================
// Utility Functions / دوال مساعدة
// ============================================================================

/**
 * تحويل سريع / Quick conversion
 */
template<typename T>
T quick_sad_to_c(void* sad_value, TypeMarshaller& marshaller);

template<typename T>
void* quick_c_to_sad(T c_value, TypeMarshaller& marshaller);

/**
 * التحقق من صحة النوع / Type validation
 */
bool is_primitive_type(const TypeInfo& type);
bool is_complex_type(const TypeInfo& type);
bool requires_deep_copy(const TypeInfo& type);

/**
 * حساب حجم النوع الديناميكي / Calculate dynamic type size
 */
size_t calculate_dynamic_size(void* value, const TypeInfo& type);

} // namespace ABI
} // namespace Sad

#endif // SAD_RUNTIME_TYPE_MARSHAL_H
