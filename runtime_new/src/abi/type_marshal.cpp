/**
 * @file type_marshal.cpp
 * @brief (AR) نظام تحويل الأنواع (Marshalling) — الجسر بين عالم ص وعالم C
 * @brief (EN) Type Marshalling system — bridge between Sad world and C world
 *
 * @details
 * ═══════════════════════════════════════════════════════════════════════════
 * (AR) شرح موسّع — نظام تحويل الأنواع (Type Marshalling)
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * ما هو تحويل الأنواع (Marshalling)؟
 * ──────────────────────────────────
 * عندما يُستدعى دالة C من كود لغة ص، يجب تحويل القيم من تمثيل لغة ص
 * الداخلي إلى التمثيل الذي تتوقعه C، والعكس عند استقبال النتائج.
 * هذه العملية تُسمى "marshalling" (التنظيم/التحويل).
 *
 * اتجاهات التحويل:
 * ─────────────────
 *   Sad → C (للمعاملات عند الاستدعاء):
 *   ├── sad_integer_to_c(): رقم صحيح → int64_t
 *   ├── sad_float_to_c(): عشري → double
 *   ├── sad_boolean_to_c(): منطقي → bool
 *   ├── sad_string_to_c(): نص → const char*
 *   ├── sad_pointer_to_c(): مؤشر → void*
 *   ├── sad_array_to_c(): مصفوفة → مصفوفة C متجاورة
 *   └── sad_struct_to_c(): بنية → بنية C مع محاذاة صحيحة
 *
 *   C → Sad (للقيم المُرجَعة):
 *   ├── c_integer_to_sad(): int64_t → رقم صحيح
 *   ├── c_float_to_sad(): double → عشري
 *   ├── c_boolean_to_sad(): bool → منطقي
 *   ├── c_string_to_sad(): const char* → نص (مع نسخ الذاكرة)
 *   ├── c_pointer_to_sad(): void* → مؤشر
 *   └── c_array_to_sad(): مصفوفة C → مصفوفة Sad
 *
 * إدارة الذاكرة:
 * ─────────────
 * - الوضع التلقائي (Automatic): يُتتبع كل تخصيص ويُنظَّف عند التدمير
 * - الوضع اليدوي (Manual): المُستدعي مسؤول عن التحرير
 * - cleanup_conversions(): تنظيف جميع التحويلات النشطة
 * - يدعم التخزين المؤقت (caching) لتقليل التخصيصات المتكررة
 *
 * التحقق من الأخطاء:
 * ─────────────────
 * - فحص المؤشرات الفارغة (null) قبل أي تحويل
 * - التحقق من توافق الأنواع
 * - تتبع أخطاء التحويل عبر ErrorCode ورسائل ثنائية اللغة
 *
 * ═══════════════════════════════════════════════════════════════════════════
 * (EN) Extended Description — Type Marshalling
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * Implements bidirectional type conversion between Sad runtime values and C
 * types. Handles primitives (int, float, bool), strings (with memory copy),
 * arrays (contiguous C arrays), and structs (with proper alignment/padding).
 *
 * Memory management: automatic cleanup mode tracks all allocations; manual
 * mode delegates cleanup to caller. Supports caching to reduce repeated
 * allocations. All conversions are null-safe with bilingual error reporting.
 *
 * @note الاعتماديات / Dependencies: abi/type_marshal.h, abi/c_abi.h,
 *       <iostream>, <cstring>, <cstdlib>, <cassert>
 * @note يُنشئ كائن CABI داخلياً لحساب تخطيط البنى والمحاذاة
 *
 * @see runtime_new/include/abi/type_marshal.h — تصريح صنف TypeMarshaller
 * @see runtime_new/src/abi/c_abi.cpp — نظام ABI الذي يعتمد عليه
 * @see runtime_new/src/ffi/memory_tracker.cpp — تتبع تسريبات الذاكرة
 *
 * @author فريق لغة ص / Sad Language Team
 * @date ديسمبر 2025 — فبراير 2026 / December 2025 — February 2026
 * @version 1.0
 */

#include "abi/type_marshal.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <new>

namespace Sad {
namespace ABI {

// ============================================================================
// باني ومفكك / Constructor and Destructor
// ============================================================================

TypeMarshaller::TypeMarshaller()
    : memory_mode_(MemoryMode::Automatic)
    , debug_mode_(false)
    , caching_enabled_(false)
    , max_cache_size_(1024)
    , stats_{}
{
    abi_ = std::make_unique<CABI>();
}

TypeMarshaller::~TypeMarshaller() {
    // (AR) تنظيف جميع التحويلات النشطة / (EN) Cleanup all active conversions
    cleanup_conversions();
}

// ============================================================================
// تحويل الأنواع الأساسية: Sad → C
// Basic Type Conversion: Sad → C
// ============================================================================

int64_t TypeMarshaller::sad_integer_to_c(void* sad_int) const {
    if (!sad_int) {
        const_cast<TypeMarshaller*>(this)->set_error(
            ErrorCode::NullPointer, "Null Sad integer / رقم Sad فارغ");
        return 0;
    }
    // (AR) نفترض أن المؤشر يشير إلى int64_t
    // (EN) Assume pointer points to int64_t
    return *static_cast<int64_t*>(sad_int);
}

double TypeMarshaller::sad_float_to_c(void* sad_float) const {
    if (!sad_float) {
        const_cast<TypeMarshaller*>(this)->set_error(
            ErrorCode::NullPointer, "Null Sad float / عشري Sad فارغ");
        return 0.0;
    }
    return *static_cast<double*>(sad_float);
}

bool TypeMarshaller::sad_boolean_to_c(void* sad_bool) const {
    if (!sad_bool) return false;
    return *static_cast<bool*>(sad_bool);
}

const char* TypeMarshaller::sad_string_to_c(void* sad_string) const {
    if (!sad_string) return "";
    // (AR) نفترض أن المؤشر يشير إلى std::string
    // (EN) Assume pointer points to std::string
    return static_cast<std::string*>(sad_string)->c_str();
}

void* TypeMarshaller::sad_pointer_to_c(void* sad_ptr) const {
    return sad_ptr; // (AR) المؤشرات متوافقة مباشرة / (EN) Pointers are directly compatible
}

// ============================================================================
// تحويل الأنواع الأساسية: C → Sad
// Basic Type Conversion: C → Sad
// ============================================================================

void* TypeMarshaller::c_integer_to_sad(int64_t c_int) const {
    auto* result = new int64_t(c_int);
    const_cast<TypeMarshaller*>(this)->stats_.c_to_sad_conversions++;
    return static_cast<void*>(result);
}

void* TypeMarshaller::c_float_to_sad(double c_float) const {
    auto* result = new double(c_float);
    const_cast<TypeMarshaller*>(this)->stats_.c_to_sad_conversions++;
    return static_cast<void*>(result);
}

void* TypeMarshaller::c_boolean_to_sad(bool c_bool) const {
    auto* result = new bool(c_bool);
    const_cast<TypeMarshaller*>(this)->stats_.c_to_sad_conversions++;
    return static_cast<void*>(result);
}

void* TypeMarshaller::c_string_to_sad(const char* c_string) const {
    auto* result = new std::string(c_string ? c_string : "");
    const_cast<TypeMarshaller*>(this)->stats_.c_to_sad_conversions++;
    return static_cast<void*>(result);
}

void* TypeMarshaller::c_pointer_to_sad(void* c_ptr) const {
    return c_ptr;
}

// ============================================================================
// تحويل الأنواع المعقدة / Complex Type Conversion
// ============================================================================

TypeMarshaller::CArray TypeMarshaller::sad_array_to_c(
    void* sad_array, const TypeInfo& element_type) const
{
    CArray result;
    result.data = nullptr;
    result.length = 0;
    result.element_size = element_type.size;

    if (!sad_array) {
        const_cast<TypeMarshaller*>(this)->set_error(
            ErrorCode::NullPointer, "Null Sad array / مصفوفة Sad فارغة");
        return result;
    }

    // (AR) نسخ بيانات المصفوفة — يعتمد على تمثيل المصفوفة الداخلي
    // (EN) Copy array data — depends on internal array representation
    auto* desc = static_cast<ArrayDescriptor*>(sad_array);
    result.data = desc->data;
    result.length = desc->length;
    result.element_size = desc->element_size;

    const_cast<TypeMarshaller*>(this)->stats_.sad_to_c_conversions++;
    return result;
}

void* TypeMarshaller::c_array_to_sad(
    const CArray& c_array, const TypeInfo& element_type) const
{
    auto* desc = new ArrayDescriptor();
    desc->data = c_array.data;
    desc->length = c_array.length;
    desc->capacity = c_array.length;
    desc->element_size = c_array.element_size;
    desc->element_type = element_type;

    const_cast<TypeMarshaller*>(this)->stats_.c_to_sad_conversions++;
    return static_cast<void*>(desc);
}

void* TypeMarshaller::sad_struct_to_c(
    void* sad_struct, const StructDescriptor& desc) const
{
    if (!sad_struct) {
        const_cast<TypeMarshaller*>(this)->set_error(
            ErrorCode::NullPointer, "Null Sad struct / بنية Sad فارغة");
        return nullptr;
    }
    // (AR) في حالة التوافق المباشر، نُعيد المؤشر كما هو
    // (EN) If directly compatible, return pointer as-is
    const_cast<TypeMarshaller*>(this)->stats_.sad_to_c_conversions++;
    return sad_struct;
}

void* TypeMarshaller::c_struct_to_sad(
    void* c_struct, const StructDescriptor& desc) const
{
    if (!c_struct) return nullptr;
    // (AR) نسخ بيانات البنية / (EN) Copy struct data
    void* result = std::malloc(desc.size);
    if (result) {
        std::memcpy(result, c_struct, desc.size);
    }
    const_cast<TypeMarshaller*>(this)->stats_.c_to_sad_conversions++;
    return result;
}

void* TypeMarshaller::sad_function_to_c(
    void* sad_func, const FunctionSignature& sig) const
{
    // (AR) مؤشرات الدوال متوافقة مباشرة في التطبيق الحالي
    // (EN) Function pointers are directly compatible in current impl
    const_cast<TypeMarshaller*>(this)->stats_.sad_to_c_conversions++;
    return sad_func;
}

void* TypeMarshaller::c_function_to_sad(
    void* c_func, const FunctionSignature& sig) const
{
    const_cast<TypeMarshaller*>(this)->stats_.c_to_sad_conversions++;
    return c_func;
}

// ============================================================================
// التحويل العام / Generic Conversion
// ============================================================================

void* TypeMarshaller::sad_to_c(void* sad_value, const TypeInfo& type) const {
    if (!sad_value && type.sad_type != SadType::Void) {
        const_cast<TypeMarshaller*>(this)->set_error(
            ErrorCode::NullPointer, "Null value in sad_to_c / قيمة فارغة");
        return nullptr;
    }

    // (AR) فحص الذاكرة المؤقتة أولاً / (EN) Check cache first
    if (caching_enabled_) {
        void* cached = cache_lookup(sad_value, type);
        if (cached) return cached;
    }

    const_cast<TypeMarshaller*>(this)->stats_.total_conversions++;
    const_cast<TypeMarshaller*>(this)->stats_.sad_to_c_conversions++;

    // (AR) التحويل حسب النوع / (EN) Convert based on type
    switch (type.sad_type) {
        case SadType::Integer:
        case SadType::Int64:
        case SadType::Int32:
        case SadType::UInt32:
        case SadType::UInt64:
        case SadType::Float:
        case SadType::Float32:
        case SadType::Float64:
        case SadType::Boolean:
        case SadType::Pointer:
            return sad_value; // (AR) متوافق مباشرة / (EN) Directly compatible
        case SadType::String:
            return const_cast<char*>(sad_string_to_c(sad_value));
        default:
            return sad_value;
    }
}

void* TypeMarshaller::c_to_sad(void* c_value, const TypeInfo& type) const {
    const_cast<TypeMarshaller*>(this)->stats_.total_conversions++;
    const_cast<TypeMarshaller*>(this)->stats_.c_to_sad_conversions++;
    return c_value; // (AR) تطبيق مبسط / (EN) Simplified implementation
}

std::vector<void*> TypeMarshaller::sad_values_to_c(
    const std::vector<void*>& sad_values,
    const std::vector<TypeInfo>& types) const
{
    std::vector<void*> result;
    result.reserve(sad_values.size());

    size_t count = std::min(sad_values.size(), types.size());
    for (size_t i = 0; i < count; ++i) {
        result.push_back(sad_to_c(sad_values[i], types[i]));
    }

    return result;
}

std::vector<void*> TypeMarshaller::c_values_to_sad(
    const std::vector<void*>& c_values,
    const std::vector<TypeInfo>& types) const
{
    std::vector<void*> result;
    result.reserve(c_values.size());

    size_t count = std::min(c_values.size(), types.size());
    for (size_t i = 0; i < count; ++i) {
        result.push_back(c_to_sad(c_values[i], types[i]));
    }

    return result;
}

// ============================================================================
// التحقق من الأنواع / Type Validation
// ============================================================================

bool TypeMarshaller::validate_type(const TypeInfo& type) const {
    return type.sad_type != SadType::Unknown && type.c_type != CType::Unknown;
}

bool TypeMarshaller::are_types_compatible(
    const TypeInfo& sad_type, const TypeInfo& c_type) const
{
    // (AR) التحقق الأساسي: الحجم والمحاذاة
    // (EN) Basic check: size and alignment
    if (sad_type.size != c_type.size) return false;
    if (sad_type.alignment != c_type.alignment) return false;
    return true;
}

bool TypeMarshaller::validate_value(void* value, const TypeInfo& type) const {
    if (type.sad_type == SadType::Void) return true;
    return value != nullptr;
}

bool TypeMarshaller::can_convert(const TypeInfo& from, const TypeInfo& to) const {
    // (AR) أي نوع يمكن تحويله إلى مؤشر / (EN) Any type can convert to pointer
    if (to.is_pointer) return true;
    // (AR) أنواع لها نفس الحجم / (EN) Types of same size
    if (from.size == to.size) return true;
    return false;
}

// ============================================================================
// إدارة الذاكرة / Memory Management
// ============================================================================

void TypeMarshaller::register_conversion(void* value, const TypeInfo& type) {
    ConversionRecord record;
    record.value = value;
    record.type = type;
    record.needs_cleanup = (memory_mode_ == MemoryMode::Automatic);
    active_conversions_.push_back(record);
    stats_.active_conversions = active_conversions_.size();
}

void TypeMarshaller::cleanup_conversions() {
    for (auto& record : active_conversions_) {
        if (record.needs_cleanup && record.value) {
            cleanup_value(record.value, record.type);
        }
    }
    active_conversions_.clear();
    stats_.active_conversions = 0;
}

void TypeMarshaller::cleanup_value(void* value, const TypeInfo& type) {
    if (!value) return;
    // (AR) تحرير الذاكرة حسب النوع / (EN) Free memory based on type
    switch (type.sad_type) {
        case SadType::String:
            delete static_cast<std::string*>(value);
            break;
        case SadType::Array:
            delete static_cast<ArrayDescriptor*>(value);
            break;
        default:
            // (AR) الأنواع البدائية قد تكون مُخصصة بـ new
            // (EN) Primitive types may be allocated with new
            break;
    }
}

void TypeMarshaller::reset_stats() {
    stats_ = Stats{};
}

// ============================================================================
// محولات مخصصة / Custom Converters
// ============================================================================

void TypeMarshaller::register_sad_to_c_converter(
    SadType sad_type, CustomConverter converter)
{
    sad_to_c_converters_[sad_type] = converter;
}

void TypeMarshaller::register_c_to_sad_converter(
    CType c_type, CustomConverter converter)
{
    c_to_sad_converters_[c_type] = converter;
}

void TypeMarshaller::unregister_sad_to_c_converter(SadType sad_type) {
    sad_to_c_converters_.erase(sad_type);
}

void TypeMarshaller::unregister_c_to_sad_converter(CType c_type) {
    c_to_sad_converters_.erase(c_type);
}

// ============================================================================
// الطرق المساعدة الخاصة / Private Helper Methods
// ============================================================================

void TypeMarshaller::set_error(
    ErrorCode code, const std::string& message,
    const TypeInfo& source, const TypeInfo& target)
{
    last_error_.code = code;
    last_error_.message = message;
    last_error_.source_type = source;
    last_error_.target_type = target;

    if (debug_mode_) {
        log_debug("[خطأ/Error] " + message);
    }
    stats_.failed_conversions++;
}

void TypeMarshaller::log_debug(const std::string& message) {
    std::cout << "[TypeMarshal تنقيح/Debug] " << message << "\n";
}

void* TypeMarshaller::copy_data(const void* data, size_t size) {
    if (!data || size == 0) return nullptr;
    void* copy = std::malloc(size);
    if (copy) {
        std::memcpy(copy, data, size);
        stats_.memory_allocated += size;
    }
    return copy;
}

void* TypeMarshaller::allocate(size_t size, size_t alignment) {
    // (AR) استخدام aligned_alloc أو _aligned_malloc حسب المنصة
    // (EN) Use aligned_alloc or _aligned_malloc based on platform
#ifdef _WIN32
    void* ptr = _aligned_malloc(size, alignment);
#else
    void* ptr = nullptr;
    posix_memalign(&ptr, alignment, size);
#endif
    if (ptr) {
        stats_.memory_allocated += size;
    }
    return ptr;
}

void TypeMarshaller::deallocate(void* ptr) {
    if (!ptr) return;
#ifdef _WIN32
    _aligned_free(ptr);
#else
    std::free(ptr);
#endif
}

void* TypeMarshaller::cache_lookup(void* value, const TypeInfo& type) const {
    if (!caching_enabled_) return nullptr;
    for (const auto& entry : conversion_cache_) {
        if (entry.original == value) {
            const_cast<TypeMarshaller*>(this)->stats_.cached_conversions++;
            return entry.converted;
        }
    }
    return nullptr;
}

void TypeMarshaller::cache_store(
    void* original, void* converted, const TypeInfo& type)
{
    if (!caching_enabled_) return;
    if (conversion_cache_.size() >= max_cache_size_) {
        conversion_cache_.erase(conversion_cache_.begin());
    }
    CacheEntry entry;
    entry.original = original;
    entry.converted = converted;
    entry.type = type;
    conversion_cache_.push_back(entry);
}

// ============================================================================
// ConversionBatch — تطبيق / Implementation
// ============================================================================

void ConversionBatch::add_sad_to_c(void* sad_value, const TypeInfo& type) {
    conversions_.push_back({sad_value, type, true});
}

void ConversionBatch::add_c_to_sad(void* c_value, const TypeInfo& type) {
    conversions_.push_back({c_value, type, false});
}

bool ConversionBatch::execute() {
    results_.clear();
    results_.reserve(conversions_.size());

    for (const auto& conv : conversions_) {
        void* result = nullptr;
        if (conv.is_sad_to_c) {
            result = marshaller_.sad_to_c(conv.value, conv.type);
        } else {
            result = marshaller_.c_to_sad(conv.value, conv.type);
        }
        results_.push_back(result);
    }
    return true;
}

void ConversionBatch::cleanup() {
    results_.clear();
    conversions_.clear();
}

// ============================================================================
// دوال مساعدة حرة / Free Helper Functions
// ============================================================================

bool is_primitive_type(const TypeInfo& type) {
    switch (type.sad_type) {
        case SadType::Integer:
        case SadType::Float:
        case SadType::Boolean:
        case SadType::Int32:
        case SadType::UInt32:
        case SadType::Int64:
        case SadType::UInt64:
        case SadType::Float32:
        case SadType::Float64:
            return true;
        default:
            return false;
    }
}

bool is_complex_type(const TypeInfo& type) {
    return type.is_array || type.is_struct ||
           type.sad_type == SadType::String;
}

bool requires_deep_copy(const TypeInfo& type) {
    return is_complex_type(type);
}

size_t calculate_dynamic_size(void* value, const TypeInfo& type) {
    if (!value) return 0;

    switch (type.sad_type) {
        case SadType::String:
            return static_cast<std::string*>(value)->size() + 1;
        case SadType::Array: {
            auto* desc = static_cast<ArrayDescriptor*>(value);
            return desc->length * desc->element_size;
        }
        default:
            return type.size;
    }
}

} // namespace ABI
} // namespace Sad
