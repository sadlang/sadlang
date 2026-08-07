/*
 * بسم الله الرحمن الرحيم
 * ═══════════════════════════════════════════════════════════════════════════
 * ملف: c_abi.h
 * الوصف: نظام تكامل واجهة التطبيقات الثنائية (C ABI) للغة ص
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * الغرض من هذا الملف:
 * ──────────────────
 * يوفر الجسر بين لغة ص ولغة C على مستوى الواجهة الثنائية (ABI).
 * يسمح لبرامج ص باستدعاء دوال C الأصلية والعكس، مع ضمان
 * التوافق الكامل في تمثيل البيانات وأنماط الاستدعاء.
 *
 * المكونات الرئيسية:
 * ─────────────────
 * - SadType: تعداد أنواع لغة ص الأساسية (Void, Integer, Float, Boolean,
 *   String, Array, Pointer, Struct, Function) + أنواع ذات حجم محدد
 *   (Int32, UInt32, Int64, UInt64, Float32, Float64) لـ FFI
 * - CType: تعداد أنواع C المقابلة (int8_t → uint64_t، float، double،
 *   pointer، array، struct، function)
 * - CallingConvention: أنماط الاستدعاء المدعومة:
 *   • SystemV_x64: Linux/macOS/BSD (المعاملات في rdi, rsi, rdx...)
 *   • Microsoft_x64: Windows (المعاملات في rcx, rdx, r8, r9)
 *   • ARM64_AAPCS: معمارية ARM 64-بت
 *   • CDefault, Stdcall, Fastcall, Vectorcall
 * - TypeInfo: معلومات النوع (الحجم، المحاذاة، هل مؤشر/مصفوفة/بنية)
 * - ArrayDescriptor: واصف المصفوفة لنقلها بين Sad و C
 * - StringDescriptor: واصف النص (المؤشر، الطول، الملكية)
 * - StructDescriptor: واصف البنية (الحقول، الحجم، المحاذاة)
 * - FunctionSignature: توقيع الدالة (نوع الإرجاع، المعاملات، نمط الاستدعاء)
 * - CABI: الفئة الرئيسية لتحويل المعاملات والقيم المرجعة
 *
 * الموقع في البنية العامة:
 * ───────────────────────
 *   runtime/
 *   ├── abi/
 *   │   ├── [c_abi.h — هذا الملف] ← واجهة ABI الأساسية
 *   │   └── type_marshal.h ← تحويل الأنواع بين Sad و C
 *   └── ffi/
 *       └── ffi.h ← يبني فوق c_abi.h لتحميل المكتبات واستدعاء الدوال
 *
 * كشف المنصة والمعمارية:
 * ─────────────────────
 * يكشف تلقائياً: Windows/Linux/macOS و x64/ARM64/x86
 * ويعرّف وحدات ماكرو التصدير/الاستيراد المناسبة
 * (SAD_ABI_EXPORT, SAD_ABI_IMPORT, SAD_CALLING_CONV)
 *
 * الاعتماديات:
 * ──────────
 * - <stdarg.h>: لدعم الدوال متغيرة المعاملات (variadic)
 * - <cstdint>, <cstddef>: للأنواع ذات الحجم المحدد
 *
 * @file runtime/abi/c_abi.h
 * @author SadLang Compiler Team
 * @date December 2025
 * ═══════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_RUNTIME_C_ABI_H
#define SAD_RUNTIME_C_ABI_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <memory>
#include <stdarg.h>

// ============================================================================
// Platform Detection / كشف المنصة
// ============================================================================

// (AR) Emscripten أوّلًا: الاستدعاءُ الأجنبيّ لا يعمل في رمل المتصفّح، لكنّ
//      الترويسة تُقحَم في كلّ من يربط `sad_core` — ومنه مفسّرُ المتصفّح. و
//      ‏`#error` هنا كان يمنع بناءَ المفسّر كاملًا للويب، فيُبنى بدلًا منه من
//      قائمةِ ملفّاتٍ يدويّةٍ تُعيد كتابة نصفه — وذلك أصلُ اختلافِ المفسّرَين.
//      فالمنصّة تُعرَّف هنا، وامتناعُ FFI يُفرَض حيث يُنادى لا حيث يُترجَم.
// (EN) Emscripten first: FFI cannot work in the browser sandbox, but this header
//      is pulled in by everything linking `sad_core` — including the wasm build.
#if defined(__EMSCRIPTEN__)
    #define SAD_PLATFORM_WASM 1
    #define SAD_ABI_EXPORT __attribute__((visibility("default")))
    #define SAD_ABI_IMPORT
    #define SAD_CALLING_CONV
    #define SAD_STDCALL
#elif defined(_WIN32) || defined(_WIN64)
    #define SAD_PLATFORM_WINDOWS 1
    #define SAD_ABI_EXPORT __declspec(dllexport)
    #define SAD_ABI_IMPORT __declspec(dllimport)
    #define SAD_CALLING_CONV __cdecl
    #define SAD_STDCALL __stdcall
#elif defined(__linux__)
    #define SAD_PLATFORM_LINUX 1
    #define SAD_ABI_EXPORT __attribute__((visibility("default")))
    #define SAD_ABI_IMPORT
    #define SAD_CALLING_CONV
    #define SAD_STDCALL
#elif defined(__APPLE__)
    #define SAD_PLATFORM_MACOS 1
    #define SAD_ABI_EXPORT __attribute__((visibility("default")))
    #define SAD_ABI_IMPORT
    #define SAD_CALLING_CONV
    #define SAD_STDCALL
#else
    #error "Unsupported platform for C ABI"
#endif

// Architecture detection / كشف المعمارية
#if defined(__x86_64__) || defined(_M_X64)
    #define SAD_ARCH_X64 1
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define SAD_ARCH_ARM64 1
#elif defined(__i386__) || defined(_M_IX86)
    #define SAD_ARCH_X86 1
#elif defined(__wasm32__) || defined(__wasm__)
    // (AR) مؤشّرٌ ٣٢ بتًّا — وهو بعينه ما يجعل خرجَ المتصفّح قد يخالف الأصليّ،
    //      فبوّابةُ التكافؤ في الأكاديمية تقابلهما مثالًا مثالًا.
    #define SAD_ARCH_WASM32 1
#else
    #error "Unsupported architecture for C ABI"
#endif

// ============================================================================
// Type Definitions / تعريفات الأنواع
// ============================================================================

namespace Sad {
namespace ABI {

/**
 * أنواع لغة ص الأساسية / Basic Sad types
 * (AR) تم توسيع التعداد ليشمل الأنواع ذات الحجم المحدد المستخدمة في FFI
 * (EN) Expanded enum to include sized types used in FFI marshalling
 */
enum class SadType : uint8_t {
    Void,       // فارغ
    Integer,    // رقم (64-bit) — اسم مستعار: Int64
    Float,      // عشري (64-bit double) — اسم مستعار: Float64
    Boolean,    // منطقي (bool)
    String,     // نص (pointer to string)
    Array,      // مصفوفة (array descriptor)
    Pointer,    // مؤشر (raw pointer)
    Struct,     // بنية (struct)
    Function,   // دالة (function pointer)
    // أنواع ذات حجم محدد — تُستخدم في التحويل بين Sad و C
    // Sized types — used in Sad ↔ C marshalling
    Int32,      // رقم 32-بت / 32-bit integer
    UInt32,     // رقم طبيعي 32-بت / 32-bit unsigned integer
    Int64,      // رقم 64-بت / 64-bit integer (= Integer)
    UInt64,     // رقم طبيعي 64-بت / 64-bit unsigned integer
    Float32,    // عشري 32-بت / 32-bit float
    Float64,    // عشري 64-بت / 64-bit double (= Float)
    Unknown     // غير معروف
};

/**
 * أنواع C المقابلة / Corresponding C types
 */
enum class CType : uint8_t {
    Void,       // void
    Int8,       // char / int8_t
    Int16,      // short / int16_t
    Int32,      // int / int32_t
    Int64,      // long long / int64_t
    UInt8,      // unsigned char / uint8_t
    UInt16,     // unsigned short / uint16_t
    UInt32,     // unsigned int / uint32_t
    UInt64,     // unsigned long long / uint64_t
    Float,      // float (32-bit)
    Double,     // double (64-bit)
    Pointer,    // void* / T*
    Array,      // T[]
    Struct,     // struct
    Function,   // function pointer
    Unknown
};

/**
 * Calling conventions / أنماط الاستدعاء
 */
enum class CallingConvention : uint8_t {
    // System V AMD64 ABI (Linux, macOS, BSD)
    SystemV_x64,
    
    // Microsoft x64 Calling Convention (Windows)
    Microsoft_x64,
    
    // ARM64 Procedure Call Standard
    ARM64_AAPCS,
    
    // C Default (platform-specific)
    CDefault,
    
    // Stdcall (Windows x86)
    Stdcall,
    
    // Fastcall
    Fastcall,
    
    // Vectorcall (SIMD)
    Vectorcall
};

/**
 * معلومات النوع / Type information
 */
struct TypeInfo {
    SadType sad_type;           // نوع Sad
    CType c_type;               // نوع C المقابل
    size_t size;                // الحجم بالبايتات / Size in bytes
    size_t alignment;           // المحاذاة / Alignment
    bool is_pointer;            // هل هو مؤشر؟
    bool is_array;              // هل هو مصفوفة؟
    bool is_struct;             // هل هو بنية؟
    
    // للمصفوفات والبنى / For arrays and structs
    std::vector<TypeInfo> element_types;
    
    TypeInfo()
        : sad_type(SadType::Unknown)
        , c_type(CType::Unknown)
        , size(0)
        , alignment(0)
        , is_pointer(false)
        , is_array(false)
        , is_struct(false)
    {}
};

/**
 * واصف المصفوفة / Array descriptor
 * Structure used to pass arrays between Sad and C
 */
struct ArrayDescriptor {
    void* data;         // مؤشر البيانات / Data pointer
    size_t length;      // الطول / Length
    size_t capacity;    // السعة / Capacity
    size_t element_size;// حجم العنصر / Element size
    TypeInfo element_type; // نوع العنصر / Element type
};

/**
 * واصف النص / String descriptor
 */
struct StringDescriptor {
    char* data;         // مؤشر النص / String data
    size_t length;      // الطول (بدون null terminator)
    size_t capacity;    // السعة
    bool is_owned;      // هل النص مملوك؟
};

/**
 * واصف البنية / Struct descriptor
 */
struct StructDescriptor {
    void* data;         // مؤشر البيانات / Data pointer
    size_t size;        // حجم البنية / Struct size
    size_t alignment;   // المحاذاة / Alignment
    std::vector<TypeInfo> field_types; // أنواع الحقول
    std::vector<size_t> field_offsets; // إزاحات الحقول
};

/**
 * معلومات الدالة / Function information
 */
struct FunctionSignature {
    std::string name;                   // اسم الدالة
    CallingConvention convention;       // نمط الاستدعاء
    TypeInfo return_type;               // نوع الإرجاع
    std::vector<TypeInfo> param_types;  // أنواع المعاملات
    bool is_variadic;                   // هل متغيرة؟
    bool is_extern;                     // هل خارجية؟
};

// ============================================================================
// C ABI Core Class / فئة نواة C ABI
// ============================================================================

/**
 * نظام C ABI الرئيسي / Main C ABI System
 * 
 * يدير التحويل بين لغة ص و C
 * Manages conversion between Sad and C
 */
class CABI {
public:
    CABI();
    ~CABI();
    
    // ========================================================================
    // Type Conversion / تحويل الأنواع
    // ========================================================================
    
    /**
     * تحويل نوع Sad إلى نوع C / Convert Sad type to C type
     */
    TypeInfo sad_type_to_c_type(SadType sad_type) const;
    
    /**
     * تحويل نوع C إلى نوع Sad / Convert C type to Sad type
     */
    TypeInfo c_type_to_sad_type(CType c_type) const;
    
    /**
     * الحصول على معلومات النوع / Get type information
     */
    TypeInfo get_type_info(SadType type) const;
    TypeInfo get_c_type_info(CType type) const;
    
    /**
     * حساب حجم النوع / Calculate type size
     */
    size_t get_type_size(const TypeInfo& type) const;
    
    /**
     * حساب محاذاة النوع / Calculate type alignment
     */
    size_t get_type_alignment(const TypeInfo& type) const;
    
    // ========================================================================
    // Calling Convention / نمط الاستدعاء
    // ========================================================================
    
    /**
     * الحصول على نمط الاستدعاء الافتراضي للمنصة
     * Get default calling convention for platform
     */
    CallingConvention get_default_convention() const;
    
    /**
     * التحقق من توافق نمط الاستدعاء
     * Check if calling convention is compatible
     */
    bool is_convention_compatible(CallingConvention conv) const;
    
    /**
     * الحصول على اسم نمط الاستدعاء
     * Get calling convention name
     */
    std::string get_convention_name(CallingConvention conv) const;
    
    // ========================================================================
    // Function Call Setup / إعداد استدعاء الدالة
    // ========================================================================
    
    /**
     * إعداد استدعاء دالة C من Sad
     * Prepare to call C function from Sad
     * 
     * @param signature توقيع الدالة
     * @return معلومات الاستدعاء
     */
    struct CallInfo {
        void* function_ptr;             // مؤشر الدالة
        CallingConvention convention;   // نمط الاستدعاء
        std::vector<void*> args;        // المعاملات المحضّرة
        size_t stack_size;              // حجم المكدس المطلوب
        bool needs_cleanup;             // يحتاج تنظيف؟
    };
    
    CallInfo prepare_c_call(
        void* func_ptr,
        const FunctionSignature& signature,
        const std::vector<void*>& sad_args
    );
    
    /**
     * إعداد استدعاء دالة Sad من C
     * Prepare to call Sad function from C
     */
    CallInfo prepare_sad_call(
        void* func_ptr,
        const FunctionSignature& signature,
        const std::vector<void*>& c_args
    );
    
    /**
     * تنفيذ الاستدعاء / Execute the call
     */
    void* execute_call(const CallInfo& call_info);
    
    /**
     * تنظيف بعد الاستدعاء / Cleanup after call
     */
    void cleanup_call(const CallInfo& call_info);
    
    // ========================================================================
    // Struct Layout / تخطيط البنية
    // ========================================================================
    
    /**
     * حساب تخطيط البنية / Calculate struct layout
     */
    StructDescriptor compute_struct_layout(
        const std::vector<TypeInfo>& field_types
    ) const;
    
    /**
     * الحصول على إزاحة حقل / Get field offset
     */
    size_t get_field_offset(
        const StructDescriptor& desc,
        size_t field_index
    ) const;
    
    /**
     * التحقق من توافق تخطيط البنى
     * Verify struct layout compatibility
     */
    bool verify_struct_compatibility(
        const StructDescriptor& sad_struct,
        const StructDescriptor& c_struct
    ) const;
    
    // ========================================================================
    // Variadic Functions / الدوال المتغيرة
    // ========================================================================
    
    /**
     * إعداد استدعاء دالة متغيرة / Prepare variadic call
     */
    CallInfo prepare_variadic_call(
        void* func_ptr,
        const FunctionSignature& signature,
        const std::vector<void*>& sad_args,
        const std::vector<TypeInfo>& variadic_types
    );
    
    /**
     * معالجة va_list / Process va_list
     */
    struct VarArgs {
        va_list args;
        std::vector<TypeInfo> types;
    };
    
    void* get_variadic_arg(VarArgs& va, const TypeInfo& type);
    
    // ========================================================================
    // Error Handling / معالجة الأخطاء
    // ========================================================================
    
    enum class ErrorCode {
        Success,
        InvalidType,
        TypeMismatch,
        UnsupportedConvention,
        StackOverflow,
        InvalidPointer,
        AllocationFailed,
        UnknownError
    };
    
    struct Error {
        ErrorCode code;
        std::string message;
        
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
        size_t total_calls;         // مجموع الاستدعاءات
        size_t sad_to_c_calls;      // Sad → C
        size_t c_to_sad_calls;      // C → Sad
        size_t failed_calls;        // فشل
        size_t total_conversions;   // مجموع التحويلات
    };
    
    Stats get_stats() const { return stats_; }
    void reset_stats() { stats_ = Stats(); }
    
    /**
     * تفعيل وضع التنقيح / Enable debug mode
     */
    void set_debug_mode(bool enabled) { debug_mode_ = enabled; }
    bool is_debug_mode() const { return debug_mode_; }
    
private:
    // ========================================================================
    // Private Implementation / التطبيق الخاص
    // ========================================================================
    
    /**
     * تعيين خطأ / Set error
     */
    void set_error(ErrorCode code, const std::string& message) const;
    
    /**
     * تسجيل معلومات تنقيح / Log debug info
     */
    void log_debug(const std::string& message) const;
    
    // Member variables / متغيرات العضو
    CallingConvention default_convention_;
    mutable Error last_error_;
    mutable Stats stats_;
    bool debug_mode_;
};

// ============================================================================
// Helper Functions / دوال مساعدة
// ============================================================================

/**
 * تحويل سريع من Sad إلى C / Quick Sad → C conversion
 */
template<typename T>
T sad_to_c(void* sad_value, SadType type);

/**
 * تحويل سريع من C إلى Sad / Quick C → Sad conversion
 */
template<typename T>
void* c_to_sad(T c_value, CType type);

/**
 * نسخ آمنة للبيانات / Safe data copy
 */
void safe_memcpy(void* dest, const void* src, size_t size);

/**
 * محاذاة المؤشر / Align pointer
 */
void* align_pointer(void* ptr, size_t alignment);

/**
 * حساب المساحة المطلوبة مع المحاذاة
 * Calculate required space with alignment
 */
size_t align_size(size_t size, size_t alignment);

} // namespace ABI
} // namespace Sad

#endif // SAD_RUNTIME_C_ABI_H
