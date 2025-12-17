/*
 * بسم الله الرحمن الرحيم
 * ================================
 * تطبيق نظام تحويل الأنواع / Type Marshalling Implementation
 * ================================
 * 
 * @file runtime/abi/type_marshal.cpp
 * @author SadLang Compiler Team
 * @date December 2025
 */

#include "type_marshal.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <cassert>

namespace Sad {
namespace ABI {

// ============================================================================
// Constructor & Destructor / المُنشئ والمُدمر
// ============================================================================

TypeMarshaller::TypeMarshaller()
    : memory_mode_(MemoryMode::Automatic)
    , debug_mode_(false)
    , caching_enabled_(true)
    , max_cache_size_(1000)
{
    abi_ = std::make_unique<CABI>();
    stats_ = Stats();
    
    log_debug("TypeMarshaller initialized");
}

TypeMarshaller::~TypeMarshaller() {
    cleanup_conversions();
    
    if (debug_mode_) {
        std::cout << "TypeMarshaller Statistics:\n"
                  << "  Total conversions: " << stats_.total_conversions << "\n"
                  << "  Sad→C: " << stats_.sad_to_c_conversions << "\n"
                  << "  C→Sad: " << stats_.c_to_sad_conversions << "\n"
                  << "  Failed: " << stats_.failed_conversions << "\n"
                  << "  Cached: " << stats_.cached_conversions << "\n";
    }
}

// ============================================================================
// Basic Type Conversion - Sad to C / تحويل الأنواع الأساسية - Sad إلى C
// ============================================================================

int64_t TypeMarshaller::sad_integer_to_c(void* sad_int) const {
    if (!sad_int) {
        set_error(ErrorCode::NullPointer, "Null Sad integer pointer");
        return 0;
    }
    
    // في التطبيق الحقيقي، نتعامل مع نوع Sad::Value
    // In real implementation, we deal with Sad::Value type
    int64_t* ptr = static_cast<int64_t*>(sad_int);
    
    const_cast<TypeMarshaller*>(this)->stats_.total_conversions++;
    const_cast<TypeMarshaller*>(this)->stats_.sad_to_c_conversions++;
    
    return *ptr;
}

double TypeMarshaller::sad_float_to_c(void* sad_float) const {
    if (!sad_float) {
        set_error(ErrorCode::NullPointer, "Null Sad float pointer");
        return 0.0;
    }
    
    double* ptr = static_cast<double*>(sad_float);
    
    const_cast<TypeMarshaller*>(this)->stats_.total_conversions++;
    const_cast<TypeMarshaller*>(this)->stats_.sad_to_c_conversions++;
    
    return *ptr;
}

bool TypeMarshaller::sad_boolean_to_c(void* sad_bool) const {
    if (!sad_bool) {
        set_error(ErrorCode::NullPointer, "Null Sad boolean pointer");
        return false;
    }
    
    bool* ptr = static_cast<bool*>(sad_bool);
    
    const_cast<TypeMarshaller*>(this)->stats_.total_conversions++;
    const_cast<TypeMarshaller*>(this)->stats_.sad_to_c_conversions++;
    
    return *ptr;
}

const char* TypeMarshaller::sad_string_to_c(void* sad_string) const {
    if (!sad_string) {
        set_error(ErrorCode::NullPointer, "Null Sad string pointer");
        return nullptr;
    }
    
    // في التطبيق الحقيقي، نتعامل مع StringDescriptor
    // In real implementation, we deal with StringDescriptor
    StringDescriptor* desc = static_cast<StringDescriptor*>(sad_string);
    
    const_cast<TypeMarshaller*>(this)->stats_.total_conversions++;
    const_cast<TypeMarshaller*>(this)->stats_.sad_to_c_conversions++;
    
    return desc->data;
}

void* TypeMarshaller::sad_pointer_to_c(void* sad_ptr) const {
    // المؤشر يمر مباشرة / Pointer passes through directly
    const_cast<TypeMarshaller*>(this)->stats_.total_conversions++;
    const_cast<TypeMarshaller*>(this)->stats_.sad_to_c_conversions++;
    
    return sad_ptr;
}

// ============================================================================
// Basic Type Conversion - C to Sad / تحويل الأنواع الأساسية - C إلى Sad
// ============================================================================

void* TypeMarshaller::c_integer_to_sad(int64_t c_int) const {
    // تخصيص ذاكرة للقيمة / Allocate memory for value
    int64_t* sad_int = new int64_t(c_int);
    
    const_cast<TypeMarshaller*>(this)->stats_.total_conversions++;
    const_cast<TypeMarshaller*>(this)->stats_.c_to_sad_conversions++;
    const_cast<TypeMarshaller*>(this)->stats_.memory_allocated += sizeof(int64_t);
    
    return sad_int;
}

void* TypeMarshaller::c_float_to_sad(double c_float) const {
    double* sad_float = new double(c_float);
    
    const_cast<TypeMarshaller*>(this)->stats_.total_conversions++;
    const_cast<TypeMarshaller*>(this)->stats_.c_to_sad_conversions++;
    const_cast<TypeMarshaller*>(this)->stats_.memory_allocated += sizeof(double);
    
    return sad_float;
}

void* TypeMarshaller::c_boolean_to_sad(bool c_bool) const {
    bool* sad_bool = new bool(c_bool);
    
    const_cast<TypeMarshaller*>(this)->stats_.total_conversions++;
    const_cast<TypeMarshaller*>(this)->stats_.c_to_sad_conversions++;
    const_cast<TypeMarshaller*>(this)->stats_.memory_allocated += sizeof(bool);
    
    return sad_bool;
}

void* TypeMarshaller::c_string_to_sad(const char* c_string) const {
    if (!c_string) {
        set_error(ErrorCode::NullPointer, "Null C string pointer");
        return nullptr;
    }
    
    // إنشاء StringDescriptor / Create StringDescriptor
    StringDescriptor* desc = new StringDescriptor();
    desc->length = std::strlen(c_string);
    desc->capacity = desc->length + 1;
    desc->data = new char[desc->capacity];
    desc->is_owned = true;
    
    std::strcpy(desc->data, c_string);
    
    const_cast<TypeMarshaller*>(this)->stats_.total_conversions++;
    const_cast<TypeMarshaller*>(this)->stats_.c_to_sad_conversions++;
    const_cast<TypeMarshaller*>(this)->stats_.memory_allocated += 
        sizeof(StringDescriptor) + desc->capacity;
    
    return desc;
}

void* TypeMarshaller::c_pointer_to_sad(void* c_ptr) const {
    const_cast<TypeMarshaller*>(this)->stats_.total_conversions++;
    const_cast<TypeMarshaller*>(this)->stats_.c_to_sad_conversions++;
    
    return c_ptr;
}

// ============================================================================
// Complex Type Conversion / تحويل الأنواع المعقدة
// ============================================================================

TypeMarshaller::CArray TypeMarshaller::sad_array_to_c(
    void* sad_array,
    const TypeInfo& element_type) const
{
    CArray result = {nullptr, 0, 0};
    
    if (!sad_array) {
        set_error(ErrorCode::NullPointer, "Null Sad array pointer");
        return result;
    }
    
    // في التطبيق الحقيقي، نتعامل مع ArrayDescriptor
    // In real implementation, we deal with ArrayDescriptor
    ArrayDescriptor* desc = static_cast<ArrayDescriptor*>(sad_array);
    
    result.data = desc->data;
    result.length = desc->length;
    result.element_size = desc->element_size;
    
    const_cast<TypeMarshaller*>(this)->stats_.total_conversions++;
    const_cast<TypeMarshaller*>(this)->stats_.sad_to_c_conversions++;
    
    log_debug("Converted Sad array of length " + std::to_string(result.length));
    
    return result;
}

void* TypeMarshaller::c_array_to_sad(
    const CArray& c_array,
    const TypeInfo& element_type) const
{
    if (!c_array.data) {
        set_error(ErrorCode::NullPointer, "Null C array data");
        return nullptr;
    }
    
    // إنشاء ArrayDescriptor / Create ArrayDescriptor
    ArrayDescriptor* desc = new ArrayDescriptor();
    desc->length = c_array.length;
    desc->capacity = c_array.length;
    desc->element_size = c_array.element_size;
    desc->element_type = element_type;
    
    // نسخ البيانات / Copy data
    size_t total_size = c_array.length * c_array.element_size;
    desc->data = allocate(total_size);
    std::memcpy(desc->data, c_array.data, total_size);
    
    const_cast<TypeMarshaller*>(this)->stats_.total_conversions++;
    const_cast<TypeMarshaller*>(this)->stats_.c_to_sad_conversions++;
    const_cast<TypeMarshaller*>(this)->stats_.memory_allocated += 
        sizeof(ArrayDescriptor) + total_size;
    
    return desc;
}

void* TypeMarshaller::sad_struct_to_c(
    void* sad_struct,
    const StructDescriptor& desc) const
{
    if (!sad_struct) {
        set_error(ErrorCode::NullPointer, "Null Sad struct pointer");
        return nullptr;
    }
    
    // في التطبيق الحقيقي، نحول كل حقل حسب نوعه
    // In real implementation, we convert each field by its type
    
    // نسخ البيانات مباشرة إذا كان التخطيط متوافقاً
    // Direct copy if layout is compatible
    void* c_struct = allocate(desc.size, desc.alignment);
    std::memcpy(c_struct, sad_struct, desc.size);
    
    const_cast<TypeMarshaller*>(this)->stats_.total_conversions++;
    const_cast<TypeMarshaller*>(this)->stats_.sad_to_c_conversions++;
    const_cast<TypeMarshaller*>(this)->stats_.memory_allocated += desc.size;
    
    return c_struct;
}

void* TypeMarshaller::c_struct_to_sad(
    void* c_struct,
    const StructDescriptor& desc) const
{
    if (!c_struct) {
        set_error(ErrorCode::NullPointer, "Null C struct pointer");
        return nullptr;
    }
    
    // نسخ البيانات / Copy data
    void* sad_struct = allocate(desc.size, desc.alignment);
    std::memcpy(sad_struct, c_struct, desc.size);
    
    const_cast<TypeMarshaller*>(this)->stats_.total_conversions++;
    const_cast<TypeMarshaller*>(this)->stats_.c_to_sad_conversions++;
    const_cast<TypeMarshaller*>(this)->stats_.memory_allocated += desc.size;
    
    return sad_struct;
}

void* TypeMarshaller::sad_function_to_c(
    void* sad_func,
    const FunctionSignature& sig) const
{
    if (!sad_func) {
        set_error(ErrorCode::NullPointer, "Null Sad function pointer");
        return nullptr;
    }
    
    // في التطبيق الحقيقي، نحتاج إنشاء wrapper function
    // In real implementation, we need to create a wrapper function
    
    const_cast<TypeMarshaller*>(this)->stats_.total_conversions++;
    const_cast<TypeMarshaller*>(this)->stats_.sad_to_c_conversions++;
    
    return sad_func; // Simplified
}

void* TypeMarshaller::c_function_to_sad(
    void* c_func,
    const FunctionSignature& sig) const
{
    if (!c_func) {
        set_error(ErrorCode::NullPointer, "Null C function pointer");
        return nullptr;
    }
    
    const_cast<TypeMarshaller*>(this)->stats_.total_conversions++;
    const_cast<TypeMarshaller*>(this)->stats_.c_to_sad_conversions++;
    
    return c_func; // Simplified
}

// ============================================================================
// Generic Conversion / التحويل العام
// ============================================================================

void* TypeMarshaller::sad_to_c(void* sad_value, const TypeInfo& type) const {
    if (!sad_value && type.sad_type != SadType::Void) {
        set_error(ErrorCode::NullPointer, "Null Sad value pointer");
        return nullptr;
    }
    
    // التحقق من الذاكرة المؤقتة / Check cache
    if (caching_enabled_) {
        void* cached = cache_lookup(sad_value, type);
        if (cached) {
            const_cast<TypeMarshaller*>(this)->stats_.cached_conversions++;
            return cached;
        }
    }
    
    // التحقق من المحول المخصص / Check custom converter
    auto it = sad_to_c_converters_.find(type.sad_type);
    if (it != sad_to_c_converters_.end()) {
        return it->second(sad_value, type);
    }
    
    // تحويل حسب النوع / Convert by type
    void* result = nullptr;
    
    switch (type.sad_type) {
        case SadType::Void:
            result = nullptr;
            break;
            
        case SadType::Integer:
            result = reinterpret_cast<void*>(
                static_cast<intptr_t>(sad_integer_to_c(sad_value))
            );
            break;
            
        case SadType::Float: {
            double val = sad_float_to_c(sad_value);
            double* ptr = new double(val);
            result = ptr;
            break;
        }
            
        case SadType::Boolean:
            result = reinterpret_cast<void*>(
                static_cast<intptr_t>(sad_boolean_to_c(sad_value))
            );
            break;
            
        case SadType::String:
            result = const_cast<char*>(sad_string_to_c(sad_value));
            break;
            
        case SadType::Pointer:
            result = sad_pointer_to_c(sad_value);
            break;
            
        case SadType::Array: {
            TypeInfo elem_type;
            if (!type.element_types.empty()) {
                elem_type = type.element_types[0];
            }
            CArray arr = sad_array_to_c(sad_value, elem_type);
            result = arr.data;
            break;
        }
            
        default:
            set_error(ErrorCode::InvalidType, 
                     "Unsupported Sad type for conversion",
                     type, TypeInfo());
            return nullptr;
    }
    
    // تخزين في الذاكرة المؤقتة / Store in cache
    if (caching_enabled_ && result) {
        const_cast<TypeMarshaller*>(this)->cache_store(sad_value, result, type);
    }
    
    return result;
}

void* TypeMarshaller::c_to_sad(void* c_value, const TypeInfo& type) const {
    // التحقق من المحول المخصص / Check custom converter
    auto it = c_to_sad_converters_.find(type.c_type);
    if (it != c_to_sad_converters_.end()) {
        return it->second(c_value, type);
    }
    
    // تحويل حسب النوع / Convert by type
    void* result = nullptr;
    
    switch (type.c_type) {
        case CType::Void:
            result = nullptr;
            break;
            
        case CType::Int64: {
            int64_t val = reinterpret_cast<int64_t>(c_value);
            result = c_integer_to_sad(val);
            break;
        }
            
        case CType::Double: {
            double* ptr = static_cast<double*>(c_value);
            result = c_float_to_sad(*ptr);
            break;
        }
            
        case CType::Int32: {
            int32_t val = static_cast<int32_t>(reinterpret_cast<intptr_t>(c_value));
            result = c_boolean_to_sad(val != 0);
            break;
        }
            
        case CType::Pointer:
            if (type.sad_type == SadType::String) {
                result = c_string_to_sad(static_cast<const char*>(c_value));
            } else {
                result = c_pointer_to_sad(c_value);
            }
            break;
            
        default:
            set_error(ErrorCode::InvalidType,
                     "Unsupported C type for conversion",
                     TypeInfo(), type);
            return nullptr;
    }
    
    return result;
}

std::vector<void*> TypeMarshaller::sad_values_to_c(
    const std::vector<void*>& sad_values,
    const std::vector<TypeInfo>& types) const
{
    std::vector<void*> results;
    results.reserve(sad_values.size());
    
    for (size_t i = 0; i < sad_values.size(); ++i) {
        const TypeInfo& type = (i < types.size()) ? types[i] : TypeInfo();
        void* converted = sad_to_c(sad_values[i], type);
        results.push_back(converted);
    }
    
    return results;
}

std::vector<void*> TypeMarshaller::c_values_to_sad(
    const std::vector<void*>& c_values,
    const std::vector<TypeInfo>& types) const
{
    std::vector<void*> results;
    results.reserve(c_values.size());
    
    for (size_t i = 0; i < c_values.size(); ++i) {
        const TypeInfo& type = (i < types.size()) ? types[i] : TypeInfo();
        void* converted = c_to_sad(c_values[i], type);
        results.push_back(converted);
    }
    
    return results;
}

// ============================================================================
// Type Validation / التحقق من الأنواع
// ============================================================================

bool TypeMarshaller::validate_type(const TypeInfo& type) const {
    // التحقق من صحة النوع / Validate type
    
    if (type.size == 0 && type.sad_type != SadType::Void) {
        return false;
    }
    
    if (type.alignment == 0) {
        return false;
    }
    
    if (type.sad_type == SadType::Unknown || type.c_type == CType::Unknown) {
        return false;
    }
    
    return true;
}

bool TypeMarshaller::are_types_compatible(
    const TypeInfo& sad_type,
    const TypeInfo& c_type) const
{
    // التحقق من توافق الأنواع / Check type compatibility
    
    TypeInfo converted = abi_->sad_type_to_c_type(sad_type.sad_type);
    
    return converted.c_type == c_type.c_type;
}

bool TypeMarshaller::validate_value(void* value, const TypeInfo& type) const {
    if (!value && type.sad_type != SadType::Void) {
        return false;
    }
    
    // يمكن إضافة فحوصات إضافية / Can add additional checks
    // - Range checks for integers
    // - Null termination for strings
    // - Array bounds
    
    return true;
}

bool TypeMarshaller::can_convert(const TypeInfo& from, const TypeInfo& to) const {
    // التحقق من إمكانية التحويل / Check if conversion is possible
    
    if (!validate_type(from) || !validate_type(to)) {
        return false;
    }
    
    // نفس النوع / Same type
    if (from.sad_type == to.sad_type && from.c_type == to.c_type) {
        return true;
    }
    
    // تحويلات مدعومة / Supported conversions
    // Integer ↔ Float
    if ((from.sad_type == SadType::Integer && to.sad_type == SadType::Float) ||
        (from.sad_type == SadType::Float && to.sad_type == SadType::Integer)) {
        return true;
    }
    
    // Pointer ↔ Integer
    if ((from.sad_type == SadType::Pointer && to.sad_type == SadType::Integer) ||
        (from.sad_type == SadType::Integer && to.sad_type == SadType::Pointer)) {
        return true;
    }
    
    return false;
}

// ============================================================================
// Memory Management / إدارة الذاكرة
// ============================================================================

void TypeMarshaller::register_conversion(void* value, const TypeInfo& type) {
    if (!value) return;
    
    ConversionRecord record;
    record.value = value;
    record.type = type;
    record.needs_cleanup = requires_deep_copy(type);
    
    active_conversions_.push_back(record);
    stats_.active_conversions++;
}

void TypeMarshaller::cleanup_conversions() {
    for (const auto& record : active_conversions_) {
        if (record.needs_cleanup) {
            cleanup_value(record.value, record.type);
        }
    }
    
    active_conversions_.clear();
    stats_.active_conversions = 0;
    
    log_debug("Cleaned up all conversions");
}

void TypeMarshaller::cleanup_value(void* value, const TypeInfo& type) {
    if (!value) return;
    
    // تنظيف حسب النوع / Cleanup by type
    switch (type.sad_type) {
        case SadType::String: {
            StringDescriptor* desc = static_cast<StringDescriptor*>(value);
            if (desc->is_owned && desc->data) {
                delete[] desc->data;
            }
            delete desc;
            break;
        }
            
        case SadType::Array: {
            ArrayDescriptor* desc = static_cast<ArrayDescriptor*>(value);
            if (desc->data) {
                deallocate(desc->data);
            }
            delete desc;
            break;
        }
            
        default:
            deallocate(value);
            break;
    }
}

// ============================================================================
// Custom Converters / محولات مخصصة
// ============================================================================

void TypeMarshaller::register_sad_to_c_converter(
    SadType sad_type,
    CustomConverter converter)
{
    sad_to_c_converters_[sad_type] = converter;
    log_debug("Registered custom Sad→C converter");
}

void TypeMarshaller::register_c_to_sad_converter(
    CType c_type,
    CustomConverter converter)
{
    c_to_sad_converters_[c_type] = converter;
    log_debug("Registered custom C→Sad converter");
}

void TypeMarshaller::unregister_sad_to_c_converter(SadType sad_type) {
    sad_to_c_converters_.erase(sad_type);
}

void TypeMarshaller::unregister_c_to_sad_converter(CType c_type) {
    c_to_sad_converters_.erase(c_type);
}

// ============================================================================
// Statistics / الإحصائيات
// ============================================================================

void TypeMarshaller::reset_stats() {
    stats_ = Stats();
    log_debug("Statistics reset");
}

// ============================================================================
// Private Helper Methods / طرق مساعدة خاصة
// ============================================================================

void TypeMarshaller::set_error(
    ErrorCode code,
    const std::string& message,
    const TypeInfo& source,
    const TypeInfo& target) const
{
    const_cast<TypeMarshaller*>(this)->last_error_ = Error(code, message);
    last_error_.source_type = source;
    last_error_.target_type = target;
    
    const_cast<TypeMarshaller*>(this)->stats_.failed_conversions++;
    
    if (debug_mode_) {
        std::cerr << "TypeMarshaller Error [" << static_cast<int>(code) << "]: "
                  << message << "\n";
    }
}

void TypeMarshaller::log_debug(const std::string& message) {
    if (debug_mode_) {
        std::cout << "[TypeMarshaller] " << message << "\n";
    }
}

void* TypeMarshaller::copy_data(const void* data, size_t size) {
    if (!data || size == 0) return nullptr;
    
    void* copy = allocate(size);
    std::memcpy(copy, data, size);
    
    return copy;
}

void* TypeMarshaller::allocate(size_t size, size_t alignment) {
    // في التطبيق الحقيقي، نستخدم memory allocator مخصص
    // In real implementation, we use custom memory allocator
    
    void* ptr = ::operator new(size, std::align_val_t(alignment));
    stats_.memory_allocated += size;
    
    return ptr;
}

void TypeMarshaller::deallocate(void* ptr) {
    if (ptr) {
        ::operator delete(ptr);
    }
}

void* TypeMarshaller::cache_lookup(void* value, const TypeInfo& type) const {
    if (!caching_enabled_) return nullptr;
    
    for (const auto& entry : conversion_cache_) {
        if (entry.original == value &&
            entry.type.sad_type == type.sad_type &&
            entry.type.c_type == type.c_type) {
            return entry.converted;
        }
    }
    
    return nullptr;
}

void TypeMarshaller::cache_store(void* original, void* converted, const TypeInfo& type) {
    if (!caching_enabled_) return;
    
    // تحديد حجم الذاكرة المؤقتة / Limit cache size
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
// Utility Functions Implementation / تطبيق الدوال المساعدة
// ============================================================================

bool is_primitive_type(const TypeInfo& type) {
    return type.sad_type == SadType::Integer ||
           type.sad_type == SadType::Float ||
           type.sad_type == SadType::Boolean ||
           type.sad_type == SadType::Pointer;
}

bool is_complex_type(const TypeInfo& type) {
    return type.sad_type == SadType::String ||
           type.sad_type == SadType::Array ||
           type.sad_type == SadType::Struct;
}

bool requires_deep_copy(const TypeInfo& type) {
    return is_complex_type(type);
}

size_t calculate_dynamic_size(void* value, const TypeInfo& type) {
    if (!value) return 0;
    
    switch (type.sad_type) {
        case SadType::String: {
            StringDescriptor* desc = static_cast<StringDescriptor*>(value);
            return sizeof(StringDescriptor) + desc->capacity;
        }
            
        case SadType::Array: {
            ArrayDescriptor* desc = static_cast<ArrayDescriptor*>(value);
            return sizeof(ArrayDescriptor) + 
                   (desc->length * desc->element_size);
        }
            
        default:
            return type.size;
    }
}

// ============================================================================
// ConversionBatch Implementation / تطبيق دفعة التحويلات
// ============================================================================

void ConversionBatch::add_sad_to_c(void* sad_value, const TypeInfo& type) {
    Conversion conv;
    conv.value = sad_value;
    conv.type = type;
    conv.is_sad_to_c = true;
    conversions_.push_back(conv);
}

void ConversionBatch::add_c_to_sad(void* c_value, const TypeInfo& type) {
    Conversion conv;
    conv.value = c_value;
    conv.type = type;
    conv.is_sad_to_c = false;
    conversions_.push_back(conv);
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
        
        if (!result && conv.type.sad_type != SadType::Void) {
            return false;
        }
        
        results_.push_back(result);
    }
    
    return true;
}

void ConversionBatch::cleanup() {
    for (size_t i = 0; i < results_.size(); ++i) {
        if (results_[i]) {
            marshaller_.cleanup_value(results_[i], conversions_[i].type);
        }
    }
    
    results_.clear();
    conversions_.clear();
}

} // namespace ABI
} // namespace Sad
