/**
 * @file c_abi.cpp
 * @brief (AR) تطبيق واجهة التطبيق الثنائية لـ C — نظام FFI الأساسي
 * @brief (EN) C Application Binary Interface implementation — core FFI system
 *
 * @details
 * ═══════════════════════════════════════════════════════════════════════════
 * (AR) شرح موسّع — واجهة التطبيق الثنائية (C ABI)
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * ما هو C ABI؟
 * ────────────
 * واجهة التطبيق الثنائية (ABI) هي العقد بين لغة ص ولغة C على مستوى
 * الآلة. تُحدد كيفية تمرير المعاملات واستقبال القيم المُرجَعة وتخطيط
 * البنى في الذاكرة عند استدعاء دوال C من كود لغة ص.
 *
 * أنماط الاستدعاء المدعومة:
 * ─────────────────────────
 * - Microsoft x64 (Windows): أول 4 معاملات في RCX,RDX,R8,R9 ثم المكدس
 * - System V x64 (Linux/Mac): أول 6 معاملات في RDI,RSI,RDX,RCX,R8,R9
 * - ARM64 AAPCS: أول 8 معاملات في X0-X7
 * - CDefault: النمط الافتراضي حسب المنصة
 *
 * الوظائف الرئيسية:
 * ────────────────
 * - sad_type_to_c_type(): تحويل أنواع Sad إلى أنواع C (مع الحجم والمحاذاة)
 * - compute_struct_layout(): حساب تخطيط البنى مع المحاذاة والحشو (padding)
 * - prepare_call(): تحضير استدعاء دالة C (ترتيب المعاملات حسب ABI)
 * - execute_call(): تنفيذ الاستدعاء الفعلي عبر مؤشر الدالة
 * - handle_return(): معالجة القيمة المُرجَعة وتحويلها لنوع Sad
 * - setup_variadic(): دعم الدوال متغيرة المعاملات (مثل printf)
 *
 * تحويل الأنواع:
 * ─────────────
 *   Sad::Integer/Int64 ←→ int64_t    Sad::Float/Float64 ←→ double
 *   Sad::Int32 ←→ int32_t            Sad::Float32 ←→ float
 *   Sad::Boolean ←→ bool             Sad::String ←→ const char*
 *   Sad::Pointer ←→ void*            Sad::Void ←→ void
 *
 * إحصاءات الأداء:
 * ───────────────
 * الصنف CABI يحتفظ بإحصاءات عن عدد الاستدعاءات، التحويلات،
 * والأخطاء لأغراض التنقيح والتحسين.
 *
 * ═══════════════════════════════════════════════════════════════════════════
 * (EN) Extended Description — C ABI Implementation
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * Implements the C ABI for FFI (Foreign Function Interface) calls from Sad.
 * Handles calling conventions (Microsoft x64, System V x64, ARM64 AAPCS),
 * type conversion between Sad and C types, struct layout with alignment/padding,
 * variadic function support, and return value handling.
 *
 * Key functions: sad_type_to_c_type(), compute_struct_layout(), prepare_call(),
 * execute_call(), handle_return(), setup_variadic().
 *
 * @note الاعتماديات / Dependencies: abi/c_abi.h (التصريح), <iostream>,
 *       <cstring>, <cassert>
 * @note يُحدَّد نمط الاستدعاء الافتراضي تلقائياً حسب المنصة عند الإنشاء
 *
 * @see runtime_new/include/abi/c_abi.h — تصريح صنف CABI
 * @see runtime_new/src/abi/type_marshal.cpp — نظام تحويل الأنواع
 * @see runtime_new/src/ffi/memory_tracker.cpp — تتبع ذاكرة FFI
 *
 * @author فريق لغة ص / Sad Language Team
 * @date ديسمبر 2025 — فبراير 2026 / December 2025 — February 2026
 * @version 1.0
 */

#include "abi/c_abi.h"
#include <iostream>
#include <cstring>
#include <cassert>

namespace Sad {
namespace ABI {

// ============================================================================
// CABI — باني ومفكك / Constructor and Destructor
// ============================================================================

CABI::CABI()
    : debug_mode_(false)
    , stats_{}
{
    // (AR) تحديد نمط الاستدعاء الافتراضي حسب المنصة والمعمارية
    // (EN) Determine default calling convention based on platform and architecture
#if defined(SAD_PLATFORM_WINDOWS) && defined(SAD_ARCH_X64)
    default_convention_ = CallingConvention::Microsoft_x64;
#elif defined(SAD_ARCH_X64)
    default_convention_ = CallingConvention::SystemV_x64;
#elif defined(SAD_ARCH_ARM64)
    default_convention_ = CallingConvention::ARM64_AAPCS;
#else
    default_convention_ = CallingConvention::CDefault;
#endif

    if (debug_mode_) {
        log_debug("CABI initialized with convention: " + get_convention_name(default_convention_));
    }
}

CABI::~CABI() = default;

// ============================================================================
// تحويل الأنواع / Type Conversion
// ============================================================================

TypeInfo CABI::sad_type_to_c_type(SadType sad_type) const {
    TypeInfo info;
    info.sad_type = sad_type;
    info.is_pointer = false;
    info.is_array = false;
    info.is_struct = false;

    // (AR) تعيين نوع C والحجم والمحاذاة حسب نوع Sad
    // (EN) Set C type, size and alignment based on Sad type
    switch (sad_type) {
        case SadType::Void:
            info.c_type = CType::Void;
            info.size = 0;
            info.alignment = 0;
            break;
        case SadType::Integer:
        case SadType::Int64:
            info.c_type = CType::Int64;
            info.size = sizeof(int64_t);
            info.alignment = alignof(int64_t);
            break;
        case SadType::Int32:
            info.c_type = CType::Int32;
            info.size = sizeof(int32_t);
            info.alignment = alignof(int32_t);
            break;
        case SadType::UInt32:
            info.c_type = CType::UInt32;
            info.size = sizeof(uint32_t);
            info.alignment = alignof(uint32_t);
            break;
        case SadType::UInt64:
            info.c_type = CType::UInt64;
            info.size = sizeof(uint64_t);
            info.alignment = alignof(uint64_t);
            break;
        case SadType::Float:
        case SadType::Float64:
            info.c_type = CType::Double;
            info.size = sizeof(double);
            info.alignment = alignof(double);
            break;
        case SadType::Float32:
            info.c_type = CType::Float;
            info.size = sizeof(float);
            info.alignment = alignof(float);
            break;
        case SadType::Boolean:
            info.c_type = CType::Int8;
            info.size = sizeof(bool);
            info.alignment = alignof(bool);
            break;
        case SadType::String:
            info.c_type = CType::Pointer;
            info.size = sizeof(void*);
            info.alignment = alignof(void*);
            info.is_pointer = true;
            break;
        case SadType::Pointer:
            info.c_type = CType::Pointer;
            info.size = sizeof(void*);
            info.alignment = alignof(void*);
            info.is_pointer = true;
            break;
        case SadType::Array:
            info.c_type = CType::Pointer;
            info.size = sizeof(void*);
            info.alignment = alignof(void*);
            info.is_array = true;
            break;
        case SadType::Struct:
            info.c_type = CType::Struct;
            info.is_struct = true;
            break;
        case SadType::Function:
            info.c_type = CType::Function;
            info.size = sizeof(void*);
            info.alignment = alignof(void*);
            break;
        default:
            info.c_type = CType::Unknown;
            info.size = 0;
            info.alignment = 0;
            break;
    }

    return info;
}

TypeInfo CABI::c_type_to_sad_type(CType c_type) const {
    TypeInfo info;
    info.c_type = c_type;
    info.is_pointer = false;
    info.is_array = false;
    info.is_struct = false;

    switch (c_type) {
        case CType::Void:
            info.sad_type = SadType::Void;
            info.size = 0;
            info.alignment = 0;
            break;
        case CType::Int8:
        case CType::Int16:
        case CType::Int32:
            info.sad_type = SadType::Int32;
            info.size = sizeof(int32_t);
            info.alignment = alignof(int32_t);
            break;
        case CType::Int64:
            info.sad_type = SadType::Integer;
            info.size = sizeof(int64_t);
            info.alignment = alignof(int64_t);
            break;
        case CType::UInt8:
        case CType::UInt16:
        case CType::UInt32:
            info.sad_type = SadType::UInt32;
            info.size = sizeof(uint32_t);
            info.alignment = alignof(uint32_t);
            break;
        case CType::UInt64:
            info.sad_type = SadType::UInt64;
            info.size = sizeof(uint64_t);
            info.alignment = alignof(uint64_t);
            break;
        case CType::Float:
            info.sad_type = SadType::Float32;
            info.size = sizeof(float);
            info.alignment = alignof(float);
            break;
        case CType::Double:
            info.sad_type = SadType::Float;
            info.size = sizeof(double);
            info.alignment = alignof(double);
            break;
        case CType::Pointer:
            info.sad_type = SadType::Pointer;
            info.size = sizeof(void*);
            info.alignment = alignof(void*);
            info.is_pointer = true;
            break;
        case CType::Array:
            info.sad_type = SadType::Array;
            info.size = sizeof(void*);
            info.alignment = alignof(void*);
            info.is_array = true;
            break;
        case CType::Struct:
            info.sad_type = SadType::Struct;
            info.is_struct = true;
            break;
        case CType::Function:
            info.sad_type = SadType::Function;
            info.size = sizeof(void*);
            info.alignment = alignof(void*);
            break;
        default:
            info.sad_type = SadType::Unknown;
            info.size = 0;
            info.alignment = 0;
            break;
    }

    return info;
}

TypeInfo CABI::get_type_info(SadType type) const {
    return sad_type_to_c_type(type);
}

TypeInfo CABI::get_c_type_info(CType type) const {
    return c_type_to_sad_type(type);
}

size_t CABI::get_type_size(const TypeInfo& type) const {
    return type.size;
}

size_t CABI::get_type_alignment(const TypeInfo& type) const {
    return type.alignment;
}

// ============================================================================
// نمط الاستدعاء / Calling Convention
// ============================================================================

CallingConvention CABI::get_default_convention() const {
    return default_convention_;
}

bool CABI::is_convention_compatible(CallingConvention conv) const {
    // (AR) على Windows x64 نقبل Microsoft_x64 و CDefault
    // (EN) On Windows x64 we accept Microsoft_x64 and CDefault
#if defined(SAD_PLATFORM_WINDOWS) && defined(SAD_ARCH_X64)
    return conv == CallingConvention::Microsoft_x64 ||
           conv == CallingConvention::CDefault;
#elif defined(SAD_ARCH_X64)
    return conv == CallingConvention::SystemV_x64 ||
           conv == CallingConvention::CDefault;
#elif defined(SAD_ARCH_ARM64)
    return conv == CallingConvention::ARM64_AAPCS ||
           conv == CallingConvention::CDefault;
#else
    return conv == CallingConvention::CDefault;
#endif
}

std::string CABI::get_convention_name(CallingConvention conv) const {
    switch (conv) {
        case CallingConvention::SystemV_x64:    return "SystemV x64";
        case CallingConvention::Microsoft_x64:  return "Microsoft x64";
        case CallingConvention::ARM64_AAPCS:    return "ARM64 AAPCS";
        case CallingConvention::CDefault:       return "C Default";
        case CallingConvention::Stdcall:        return "Stdcall";
        case CallingConvention::Fastcall:       return "Fastcall";
        case CallingConvention::Vectorcall:     return "Vectorcall";
        default:                                return "Unknown/مجهول";
    }
}

// ============================================================================
// إعداد الاستدعاء / Call Setup
// ============================================================================

CABI::CallInfo CABI::prepare_c_call(
    void* func_ptr,
    const FunctionSignature& signature,
    const std::vector<void*>& sad_args)
{
    CallInfo info;
    info.function_ptr = func_ptr;
    info.convention = signature.convention;
    info.args = sad_args;  // (AR) في التطبيق البسيط نمرر كما هي
    info.stack_size = sad_args.size() * sizeof(void*);
    info.needs_cleanup = false;

    stats_.total_calls++;
    stats_.sad_to_c_calls++;

    if (debug_mode_) {
        log_debug("Prepared C call: " + signature.name +
                  " with " + std::to_string(sad_args.size()) + " args");
    }

    return info;
}

CABI::CallInfo CABI::prepare_sad_call(
    void* func_ptr,
    const FunctionSignature& signature,
    const std::vector<void*>& c_args)
{
    CallInfo info;
    info.function_ptr = func_ptr;
    info.convention = signature.convention;
    info.args = c_args;
    info.stack_size = c_args.size() * sizeof(void*);
    info.needs_cleanup = false;

    stats_.total_calls++;
    stats_.c_to_sad_calls++;

    if (debug_mode_) {
        log_debug("Prepared Sad call: " + signature.name +
                  " with " + std::to_string(c_args.size()) + " args");
    }

    return info;
}

void* CABI::execute_call(const CallInfo& call_info) {
    // (AR) تنفيذ الاستدعاء عبر مؤشر الدالة
    //      هذا تطبيق مبسط — التطبيق الكامل يتطلب تجميع ASM
    //      لكل نمط استدعاء على حدة.
    // (EN) Execute call via function pointer
    //      This is a simplified implementation — full implementation
    //      requires ASM for each calling convention.
    using FuncType = void*(*)();
    auto func = reinterpret_cast<FuncType>(call_info.function_ptr);

    if (!func) {
        set_error(ErrorCode::InvalidPointer, "Null function pointer / مؤشر دالة فارغ");
        return nullptr;
    }

    // (AR) استدعاء بدون معاملات — للاستدعاءات الأكثر تعقيداً
    //      نحتاج libffi أو تجميع محلي
    // (EN) Call without args — for more complex calls we need libffi or inline asm
    return func();
}

void CABI::cleanup_call(const CallInfo& call_info) {
    // (AR) تنظيف الموارد بعد الاستدعاء
    // (EN) Cleanup resources after call
    if (call_info.needs_cleanup) {
        // (AR) في التطبيق الحالي لا يوجد تنظيف مطلوب
        // (EN) In current implementation no cleanup needed
    }
}

// ============================================================================
// تخطيط البنية / Struct Layout
// ============================================================================

StructDescriptor CABI::compute_struct_layout(
    const std::vector<TypeInfo>& field_types) const
{
    StructDescriptor desc;
    desc.data = nullptr;
    desc.field_types = field_types;

    size_t offset = 0;
    size_t max_alignment = 1;

    // (AR) حساب الإزاحة لكل حقل مع مراعاة المحاذاة
    // (EN) Calculate offset for each field respecting alignment
    for (const auto& field : field_types) {
        size_t alignment = field.alignment > 0 ? field.alignment : 1;
        max_alignment = std::max(max_alignment, alignment);

        // (AR) محاذاة الإزاحة / (EN) Align offset
        offset = (offset + alignment - 1) & ~(alignment - 1);

        desc.field_offsets.push_back(offset);
        offset += field.size;
    }

    // (AR) محاذاة الحجم الكلي / (EN) Align total size
    desc.size = (offset + max_alignment - 1) & ~(max_alignment - 1);
    desc.alignment = max_alignment;

    return desc;
}

size_t CABI::get_field_offset(
    const StructDescriptor& desc,
    size_t field_index) const
{
    if (field_index >= desc.field_offsets.size()) {
        set_error(ErrorCode::InvalidType,
                  "Field index out of range / فهرس الحقل خارج النطاق");
        return 0;
    }
    return desc.field_offsets[field_index];
}

bool CABI::verify_struct_compatibility(
    const StructDescriptor& sad_struct,
    const StructDescriptor& c_struct) const
{
    // (AR) تحقق من تطابق الحجم والمحاذاة وعدد الحقول
    // (EN) Verify size, alignment and field count match
    if (sad_struct.field_types.size() != c_struct.field_types.size()) {
        return false;
    }
    if (sad_struct.size != c_struct.size) {
        return false;
    }
    if (sad_struct.alignment != c_struct.alignment) {
        return false;
    }
    return true;
}

// ============================================================================
// الدوال المتغيرة / Variadic Functions
// ============================================================================

CABI::CallInfo CABI::prepare_variadic_call(
    void* func_ptr,
    const FunctionSignature& signature,
    const std::vector<void*>& sad_args,
    const std::vector<TypeInfo>& variadic_types)
{
    CallInfo info;
    info.function_ptr = func_ptr;
    info.convention = signature.convention;
    info.args = sad_args;
    info.stack_size = sad_args.size() * sizeof(void*);
    info.needs_cleanup = false;

    stats_.total_calls++;

    if (debug_mode_) {
        log_debug("Prepared variadic call: " + signature.name +
                  " fixed=" + std::to_string(signature.param_types.size()) +
                  " variadic=" + std::to_string(variadic_types.size()));
    }

    return info;
}

void* CABI::get_variadic_arg(VarArgs& va, const TypeInfo& type) {
    // (AR) استخراج معامل واحد من va_list حسب النوع
    // (EN) Extract one argument from va_list based on type
    switch (type.c_type) {
        case CType::Int32:
        case CType::Int16:
        case CType::Int8:
            return reinterpret_cast<void*>(static_cast<intptr_t>(va_arg(va.args, int)));
        case CType::Int64:
            return reinterpret_cast<void*>(va_arg(va.args, int64_t));
        case CType::UInt32:
        case CType::UInt16:
        case CType::UInt8:
            return reinterpret_cast<void*>(static_cast<uintptr_t>(va_arg(va.args, unsigned int)));
        case CType::UInt64:
            return reinterpret_cast<void*>(static_cast<uintptr_t>(va_arg(va.args, uint64_t)));
        case CType::Double:
        case CType::Float: {
            double val = va_arg(va.args, double);
            void* result = nullptr;
            std::memcpy(&result, &val, std::min(sizeof(double), sizeof(void*)));
            return result;
        }
        case CType::Pointer:
        case CType::Function:
            return va_arg(va.args, void*);
        default:
            return va_arg(va.args, void*);
    }
}

// ============================================================================
// معالجة الأخطاء / Error Handling
// ============================================================================

void CABI::set_error(ErrorCode code, const std::string& message) const {
    last_error_ = Error(code, message);
    if (debug_mode_) {
        log_debug("[خطأ/Error] " + message);
    }
    stats_.failed_calls++;
}

void CABI::log_debug(const std::string& message) const {
    std::cout << "[C-ABI تنقيح/Debug] " << message << "\n";
}

// ============================================================================
// دوال مساعدة / Helper Functions
// ============================================================================

void safe_memcpy(void* dest, const void* src, size_t size) {
    if (dest && src && size > 0) {
        std::memcpy(dest, src, size);
    }
}

void* align_pointer(void* ptr, size_t alignment) {
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
    uintptr_t aligned = (addr + alignment - 1) & ~(alignment - 1);
    return reinterpret_cast<void*>(aligned);
}

size_t align_size(size_t size, size_t alignment) {
    return (size + alignment - 1) & ~(alignment - 1);
}

} // namespace ABI
} // namespace Sad
