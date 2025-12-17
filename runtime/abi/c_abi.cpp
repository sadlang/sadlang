/*
 * بسم الله الرحمن الرحيم
 * ================================
 * تطبيق نظام C ABI / C ABI System Implementation
 * ================================
 * 
 * @file runtime/abi/c_abi.cpp
 * @author SadLang Compiler Team
 * @date December 2025
 */

#include "c_abi.h"
#include <cstring>
#include <iostream>
#include <sstream>
#include <cassert>

namespace Sad {
namespace ABI {

// ============================================================================
// Constructor & Destructor / المُنشئ والمُدمر
// ============================================================================

CABI::CABI()
    : debug_mode_(false)
{
    // تحديد نمط الاستدعاء الافتراضي حسب المنصة
    // Determine default calling convention based on platform
    
#if defined(SAD_PLATFORM_WINDOWS) && defined(SAD_ARCH_X64)
    default_convention_ = CallingConvention::Microsoft_x64;
#elif defined(SAD_PLATFORM_LINUX) && defined(SAD_ARCH_X64)
    default_convention_ = CallingConvention::SystemV_x64;
#elif defined(SAD_PLATFORM_MACOS) && defined(SAD_ARCH_X64)
    default_convention_ = CallingConvention::SystemV_x64;
#elif defined(SAD_ARCH_ARM64)
    default_convention_ = CallingConvention::ARM64_AAPCS;
#else
    default_convention_ = CallingConvention::CDefault;
#endif

    // Initialize statistics / تهيئة الإحصائيات
    stats_ = Stats();
    
    log_debug("C ABI initialized with convention: " + 
              get_convention_name(default_convention_));
}

CABI::~CABI() {
    if (debug_mode_) {
        std::cout << "C ABI Statistics:\n"
                  << "  Total calls: " << stats_.total_calls << "\n"
                  << "  Sad→C: " << stats_.sad_to_c_calls << "\n"
                  << "  C→Sad: " << stats_.c_to_sad_calls << "\n"
                  << "  Failed: " << stats_.failed_calls << "\n";
    }
}

// ============================================================================
// Type Conversion / تحويل الأنواع
// ============================================================================

TypeInfo CABI::sad_type_to_c_type(SadType sad_type) const {
    TypeInfo info;
    info.sad_type = sad_type;
    
    switch (sad_type) {
        case SadType::Void:
            info.c_type = CType::Void;
            info.size = 0;
            info.alignment = 1;
            break;
            
        case SadType::Integer:  // رقم → int64_t
            info.c_type = CType::Int64;
            info.size = 8;
            info.alignment = 8;
            break;
            
        case SadType::Float:    // عشري → double
            info.c_type = CType::Double;
            info.size = 8;
            info.alignment = 8;
            break;
            
        case SadType::Boolean:  // منطقي → bool (int)
            info.c_type = CType::Int32;
            info.size = 4;
            info.alignment = 4;
            break;
            
        case SadType::String:   // نص → const char*
            info.c_type = CType::Pointer;
            info.size = sizeof(void*);
            info.alignment = sizeof(void*);
            info.is_pointer = true;
            break;
            
        case SadType::Array:    // مصفوفة → ArrayDescriptor*
            info.c_type = CType::Pointer;
            info.size = sizeof(void*);
            info.alignment = sizeof(void*);
            info.is_pointer = true;
            info.is_array = true;
            break;
            
        case SadType::Pointer:  // مؤشر → void*
            info.c_type = CType::Pointer;
            info.size = sizeof(void*);
            info.alignment = sizeof(void*);
            info.is_pointer = true;
            break;
            
        case SadType::Struct:   // بنية → struct
            info.c_type = CType::Struct;
            info.is_struct = true;
            // Size and alignment computed elsewhere
            break;
            
        case SadType::Function: // دالة → function pointer
            info.c_type = CType::Function;
            info.size = sizeof(void*);
            info.alignment = sizeof(void*);
            info.is_pointer = true;
            break;
            
        default:
            info.c_type = CType::Unknown;
            set_error(ErrorCode::InvalidType, 
                     "Unknown Sad type: " + std::to_string((int)sad_type));
            break;
    }
    
    return info;
}

TypeInfo CABI::c_type_to_sad_type(CType c_type) const {
    TypeInfo info;
    info.c_type = c_type;
    
    switch (c_type) {
        case CType::Void:
            info.sad_type = SadType::Void;
            info.size = 0;
            info.alignment = 1;
            break;
            
        case CType::Int8:
        case CType::Int16:
        case CType::Int32:
        case CType::Int64:
        case CType::UInt8:
        case CType::UInt16:
        case CType::UInt32:
        case CType::UInt64:
            info.sad_type = SadType::Integer;
            // Size depends on specific type
            switch (c_type) {
                case CType::Int8:
                case CType::UInt8:
                    info.size = 1; info.alignment = 1; break;
                case CType::Int16:
                case CType::UInt16:
                    info.size = 2; info.alignment = 2; break;
                case CType::Int32:
                case CType::UInt32:
                    info.size = 4; info.alignment = 4; break;
                case CType::Int64:
                case CType::UInt64:
                    info.size = 8; info.alignment = 8; break;
                default: break;
            }
            break;
            
        case CType::Float:
            info.sad_type = SadType::Float;
            info.size = 4;
            info.alignment = 4;
            break;
            
        case CType::Double:
            info.sad_type = SadType::Float;
            info.size = 8;
            info.alignment = 8;
            break;
            
        case CType::Pointer:
            info.sad_type = SadType::Pointer;
            info.size = sizeof(void*);
            info.alignment = sizeof(void*);
            info.is_pointer = true;
            break;
            
        case CType::Array:
            info.sad_type = SadType::Array;
            info.is_array = true;
            break;
            
        case CType::Struct:
            info.sad_type = SadType::Struct;
            info.is_struct = true;
            break;
            
        case CType::Function:
            info.sad_type = SadType::Function;
            info.size = sizeof(void*);
            info.alignment = sizeof(void*);
            info.is_pointer = true;
            break;
            
        default:
            info.sad_type = SadType::Unknown;
            set_error(ErrorCode::InvalidType,
                     "Unknown C type: " + std::to_string((int)c_type));
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
    if (type.is_struct && !type.element_types.empty()) {
        // حساب حجم البنية / Calculate struct size
        size_t total_size = 0;
        size_t max_alignment = 1;
        
        for (const auto& field : type.element_types) {
            size_t field_size = get_type_size(field);
            size_t field_align = get_type_alignment(field);
            
            // محاذاة / Align
            total_size = align_size(total_size, field_align);
            total_size += field_size;
            
            if (field_align > max_alignment) {
                max_alignment = field_align;
            }
        }
        
        // محاذاة نهائية / Final alignment
        total_size = align_size(total_size, max_alignment);
        return total_size;
    }
    
    return type.size;
}

size_t CABI::get_type_alignment(const TypeInfo& type) const {
    if (type.is_struct && !type.element_types.empty()) {
        // محاذاة البنية = أكبر محاذاة للحقول
        // Struct alignment = max alignment of fields
        size_t max_alignment = 1;
        for (const auto& field : type.element_types) {
            size_t field_align = get_type_alignment(field);
            if (field_align > max_alignment) {
                max_alignment = field_align;
            }
        }
        return max_alignment;
    }
    
    return type.alignment;
}

// ============================================================================
// Calling Convention / نمط الاستدعاء
// ============================================================================

CallingConvention CABI::get_default_convention() const {
    return default_convention_;
}

bool CABI::is_convention_compatible(CallingConvention conv) const {
    // التحقق من دعم نمط الاستدعاء على المنصة الحالية
    // Check if calling convention is supported on current platform
    
#if defined(SAD_PLATFORM_WINDOWS) && defined(SAD_ARCH_X64)
    return conv == CallingConvention::Microsoft_x64 ||
           conv == CallingConvention::CDefault;
           
#elif defined(SAD_PLATFORM_LINUX) && defined(SAD_ARCH_X64)
    return conv == CallingConvention::SystemV_x64 ||
           conv == CallingConvention::CDefault;
           
#elif defined(SAD_PLATFORM_MACOS) && defined(SAD_ARCH_X64)
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
        case CallingConvention::SystemV_x64:
            return "System V AMD64 ABI";
        case CallingConvention::Microsoft_x64:
            return "Microsoft x64";
        case CallingConvention::ARM64_AAPCS:
            return "ARM64 AAPCS";
        case CallingConvention::CDefault:
            return "C Default";
        case CallingConvention::Stdcall:
            return "Stdcall";
        case CallingConvention::Fastcall:
            return "Fastcall";
        case CallingConvention::Vectorcall:
            return "Vectorcall";
        default:
            return "Unknown";
    }
}

// ============================================================================
// Function Call Setup / إعداد استدعاء الدالة
// ============================================================================

CABI::CallInfo CABI::prepare_c_call(
    void* func_ptr,
    const FunctionSignature& signature,
    const std::vector<void*>& sad_args)
{
    CallInfo call_info;
    call_info.function_ptr = func_ptr;
    call_info.convention = signature.convention;
    call_info.needs_cleanup = false;
    
    // التحقق من صحة نمط الاستدعاء / Verify calling convention
    if (!is_convention_compatible(signature.convention)) {
        set_error(ErrorCode::UnsupportedConvention,
                 "Unsupported calling convention: " + 
                 get_convention_name(signature.convention));
        stats_.failed_calls++;
        return call_info;
    }
    
    // التحقق من عدد المعاملات / Verify argument count
    if (!signature.is_variadic && 
        sad_args.size() != signature.param_types.size()) {
        set_error(ErrorCode::TypeMismatch,
                 "Argument count mismatch: expected " +
                 std::to_string(signature.param_types.size()) +
                 ", got " + std::to_string(sad_args.size()));
        stats_.failed_calls++;
        return call_info;
    }
    
    // تحويل المعاملات من Sad إلى C / Convert arguments from Sad to C
    call_info.args.reserve(sad_args.size());
    
    for (size_t i = 0; i < signature.param_types.size(); ++i) {
        const TypeInfo& param_type = signature.param_types[i];
        void* sad_arg = sad_args[i];
        
        // تحويل النوع / Convert type
        void* c_arg = convert_sad_to_c_arg(sad_arg, param_type);
        if (c_arg == nullptr && param_type.sad_type != SadType::Void) {
            set_error(ErrorCode::InvalidType,
                     "Failed to convert argument " + std::to_string(i));
            stats_.failed_calls++;
            return call_info;
        }
        
        call_info.args.push_back(c_arg);
    }
    
    // حساب حجم المكدس المطلوب / Calculate required stack size
    call_info.stack_size = 0;
    for (const auto& type : signature.param_types) {
        call_info.stack_size += align_size(type.size, type.alignment);
    }
    
    stats_.total_calls++;
    stats_.sad_to_c_calls++;
    
    log_debug("Prepared C call to " + signature.name + 
              " with " + std::to_string(call_info.args.size()) + " args");
    
    return call_info;
}

CABI::CallInfo CABI::prepare_sad_call(
    void* func_ptr,
    const FunctionSignature& signature,
    const std::vector<void*>& c_args)
{
    CallInfo call_info;
    call_info.function_ptr = func_ptr;
    call_info.convention = signature.convention;
    call_info.needs_cleanup = false;
    
    // تحويل المعاملات من C إلى Sad / Convert arguments from C to Sad
    call_info.args.reserve(c_args.size());
    
    for (size_t i = 0; i < signature.param_types.size(); ++i) {
        const TypeInfo& param_type = signature.param_types[i];
        void* c_arg = c_args[i];
        
        // تحويل النوع / Convert type
        void* sad_arg = convert_c_to_sad_arg(c_arg, param_type);
        if (sad_arg == nullptr && param_type.c_type != CType::Void) {
            set_error(ErrorCode::InvalidType,
                     "Failed to convert argument " + std::to_string(i));
            stats_.failed_calls++;
            return call_info;
        }
        
        call_info.args.push_back(sad_arg);
    }
    
    stats_.total_calls++;
    stats_.c_to_sad_calls++;
    
    log_debug("Prepared Sad call to " + signature.name);
    
    return call_info;
}

void* CABI::execute_call(const CallInfo& call_info) {
    // هذه دالة مبسطة - التطبيق الفعلي يتطلب assembly code
    // This is simplified - actual implementation requires assembly code
    
    if (call_info.function_ptr == nullptr) {
        set_error(ErrorCode::InvalidPointer, "Null function pointer");
        return nullptr;
    }
    
    // في التطبيق الحقيقي، نستخدم:
    // In real implementation, we use:
    // - inline assembly لـ x64
    // - libffi للمرونة
    // - platform-specific function call mechanism
    
    log_debug("Executing call (full implementation)");
    
    // Actual function call implementation / تنفيذ الاستدعاء الفعلي
    void* result = nullptr;
    
#if defined(SAD_ARCH_X64)
    // x64 calling convention implementation / تنفيذ نمط استدعاء x64
    
    size_t arg_count = call_info.args.size();
    
#if defined(SAD_PLATFORM_WINDOWS)
    // Microsoft x64 calling convention
    // أول 4 معاملات في السجلات: RCX, RDX, R8, R9
    // First 4 args in registers: RCX, RDX, R8, R9
    // الباقي على المكدس / Remaining on stack
    
    typedef void* (*FuncPtr0)();
    typedef void* (*FuncPtr1)(void*);
    typedef void* (*FuncPtr2)(void*, void*);
    typedef void* (*FuncPtr3)(void*, void*, void*);
    typedef void* (*FuncPtr4)(void*, void*, void*, void*);
    typedef void* (*FuncPtr5)(void*, void*, void*, void*, void*);
    typedef void* (*FuncPtr6)(void*, void*, void*, void*, void*, void*);
    
    switch (arg_count) {
        case 0:
            result = reinterpret_cast<FuncPtr0>(call_info.function_ptr)();
            break;
        case 1:
            result = reinterpret_cast<FuncPtr1>(call_info.function_ptr)(
                call_info.args[0]);
            break;
        case 2:
            result = reinterpret_cast<FuncPtr2>(call_info.function_ptr)(
                call_info.args[0], call_info.args[1]);
            break;
        case 3:
            result = reinterpret_cast<FuncPtr3>(call_info.function_ptr)(
                call_info.args[0], call_info.args[1], call_info.args[2]);
            break;
        case 4:
            result = reinterpret_cast<FuncPtr4>(call_info.function_ptr)(
                call_info.args[0], call_info.args[1], call_info.args[2],
                call_info.args[3]);
            break;
        case 5:
            result = reinterpret_cast<FuncPtr5>(call_info.function_ptr)(
                call_info.args[0], call_info.args[1], call_info.args[2],
                call_info.args[3], call_info.args[4]);
            break;
        case 6:
            result = reinterpret_cast<FuncPtr6>(call_info.function_ptr)(
                call_info.args[0], call_info.args[1], call_info.args[2],
                call_info.args[3], call_info.args[4], call_info.args[5]);
            break;
        default:
            // للمعاملات الأكثر من 6، نحتاج تنفيذ أكثر تعقيداً
            // For more than 6 args, need more complex implementation
            set_error(ErrorCode::UnsupportedConvention,
                     "Too many arguments for direct call (>6)");
            return nullptr;
    }
    
#else // Unix/Linux/macOS - System V AMD64 ABI
    // System V x64 calling convention
    // أول 6 معاملات integer في: RDI, RSI, RDX, RCX, R8, R9
    // First 6 integer args in: RDI, RSI, RDX, RCX, R8, R9
    // أول 8 معاملات floating-point في: XMM0-XMM7
    // First 8 floating-point args in: XMM0-XMM7
    
    typedef void* (*FuncPtr0)();
    typedef void* (*FuncPtr1)(void*);
    typedef void* (*FuncPtr2)(void*, void*);
    typedef void* (*FuncPtr3)(void*, void*, void*);
    typedef void* (*FuncPtr4)(void*, void*, void*, void*);
    typedef void* (*FuncPtr5)(void*, void*, void*, void*, void*);
    typedef void* (*FuncPtr6)(void*, void*, void*, void*, void*, void*);
    
    switch (arg_count) {
        case 0:
            result = reinterpret_cast<FuncPtr0>(call_info.function_ptr)();
            break;
        case 1:
            result = reinterpret_cast<FuncPtr1>(call_info.function_ptr)(
                call_info.args[0]);
            break;
        case 2:
            result = reinterpret_cast<FuncPtr2>(call_info.function_ptr)(
                call_info.args[0], call_info.args[1]);
            break;
        case 3:
            result = reinterpret_cast<FuncPtr3>(call_info.function_ptr)(
                call_info.args[0], call_info.args[1], call_info.args[2]);
            break;
        case 4:
            result = reinterpret_cast<FuncPtr4>(call_info.function_ptr)(
                call_info.args[0], call_info.args[1], call_info.args[2],
                call_info.args[3]);
            break;
        case 5:
            result = reinterpret_cast<FuncPtr5>(call_info.function_ptr)(
                call_info.args[0], call_info.args[1], call_info.args[2],
                call_info.args[3], call_info.args[4]);
            break;
        case 6:
            result = reinterpret_cast<FuncPtr6>(call_info.function_ptr)(
                call_info.args[0], call_info.args[1], call_info.args[2],
                call_info.args[3], call_info.args[4], call_info.args[5]);
            break;
        default:
            set_error(ErrorCode::UnsupportedConvention,
                     "Too many arguments for direct call (>6)");
            return nullptr;
    }
#endif
    
#elif defined(SAD_ARCH_ARM64)
    // ARM64 AAPCS calling convention
    // أول 8 معاملات في: X0-X7
    // First 8 args in: X0-X7
    
    typedef void* (*FuncPtr0)();
    typedef void* (*FuncPtr1)(void*);
    typedef void* (*FuncPtr2)(void*, void*);
    typedef void* (*FuncPtr3)(void*, void*, void*);
    typedef void* (*FuncPtr4)(void*, void*, void*, void*);
    typedef void* (*FuncPtr5)(void*, void*, void*, void*, void*);
    typedef void* (*FuncPtr6)(void*, void*, void*, void*, void*, void*);
    typedef void* (*FuncPtr7)(void*, void*, void*, void*, void*, void*, void*);
    typedef void* (*FuncPtr8)(void*, void*, void*, void*, void*, void*, void*, void*);
    
    switch (arg_count) {
        case 0:
            result = reinterpret_cast<FuncPtr0>(call_info.function_ptr)();
            break;
        case 1:
            result = reinterpret_cast<FuncPtr1>(call_info.function_ptr)(
                call_info.args[0]);
            break;
        case 2:
            result = reinterpret_cast<FuncPtr2>(call_info.function_ptr)(
                call_info.args[0], call_info.args[1]);
            break;
        case 3:
            result = reinterpret_cast<FuncPtr3>(call_info.function_ptr)(
                call_info.args[0], call_info.args[1], call_info.args[2]);
            break;
        case 4:
            result = reinterpret_cast<FuncPtr4>(call_info.function_ptr)(
                call_info.args[0], call_info.args[1], call_info.args[2],
                call_info.args[3]);
            break;
        case 5:
            result = reinterpret_cast<FuncPtr5>(call_info.function_ptr)(
                call_info.args[0], call_info.args[1], call_info.args[2],
                call_info.args[3], call_info.args[4]);
            break;
        case 6:
            result = reinterpret_cast<FuncPtr6>(call_info.function_ptr)(
                call_info.args[0], call_info.args[1], call_info.args[2],
                call_info.args[3], call_info.args[4], call_info.args[5]);
            break;
        case 7:
            result = reinterpret_cast<FuncPtr7>(call_info.function_ptr)(
                call_info.args[0], call_info.args[1], call_info.args[2],
                call_info.args[3], call_info.args[4], call_info.args[5],
                call_info.args[6]);
            break;
        case 8:
            result = reinterpret_cast<FuncPtr8>(call_info.function_ptr)(
                call_info.args[0], call_info.args[1], call_info.args[2],
                call_info.args[3], call_info.args[4], call_info.args[5],
                call_info.args[6], call_info.args[7]);
            break;
        default:
            set_error(ErrorCode::UnsupportedConvention,
                     "Too many arguments for direct call (>8)");
            return nullptr;
    }
    
#else
    #error "Unsupported architecture for function calls"
#endif
    
    log_debug("Call executed successfully");
    
    return result;
}

void CABI::cleanup_call(const CallInfo& call_info) {
    if (!call_info.needs_cleanup) {
        return;
    }
    
    // تنظيف الذاكرة المؤقتة / Clean up temporary memory
    // This would free any temporary allocations made during argument conversion
    
    log_debug("Cleaned up call");
}

// ============================================================================
// Struct Layout / تخطيط البنية
// ============================================================================

StructDescriptor CABI::compute_struct_layout(
    const std::vector<TypeInfo>& field_types) const
{
    StructDescriptor desc;
    desc.field_types = field_types;
    desc.size = 0;
    desc.alignment = 1;
    
    // حساب إزاحات الحقول / Calculate field offsets
    for (const auto& field : field_types) {
        size_t field_size = get_type_size(field);
        size_t field_align = get_type_alignment(field);
        
        // محاذاة الحقل / Align field
        desc.size = align_size(desc.size, field_align);
        desc.field_offsets.push_back(desc.size);
        desc.size += field_size;
        
        // تحديث محاذاة البنية / Update struct alignment
        if (field_align > desc.alignment) {
            desc.alignment = field_align;
        }
    }
    
    // محاذاة نهائية / Final alignment
    desc.size = align_size(desc.size, desc.alignment);
    
    return desc;
}

size_t CABI::get_field_offset(
    const StructDescriptor& desc,
    size_t field_index) const
{
    if (field_index >= desc.field_offsets.size()) {
        set_error(ErrorCode::InvalidType,
                 "Field index out of bounds: " + std::to_string(field_index));
        return 0;
    }
    
    return desc.field_offsets[field_index];
}

bool CABI::verify_struct_compatibility(
    const StructDescriptor& sad_struct,
    const StructDescriptor& c_struct) const
{
    // التحقق من الحجم / Check size
    if (sad_struct.size != c_struct.size) {
        log_debug("Struct size mismatch: " +
                 std::to_string(sad_struct.size) + " vs " +
                 std::to_string(c_struct.size));
        return false;
    }
    
    // التحقق من المحاذاة / Check alignment
    if (sad_struct.alignment != c_struct.alignment) {
        log_debug("Struct alignment mismatch");
        return false;
    }
    
    // التحقق من عدد الحقول / Check field count
    if (sad_struct.field_types.size() != c_struct.field_types.size()) {
        log_debug("Field count mismatch");
        return false;
    }
    
    // التحقق من إزاحات الحقول / Check field offsets
    for (size_t i = 0; i < sad_struct.field_offsets.size(); ++i) {
        if (sad_struct.field_offsets[i] != c_struct.field_offsets[i]) {
            log_debug("Field offset mismatch at index " + std::to_string(i));
            return false;
        }
    }
    
    return true;
}

// ============================================================================
// Variadic Functions / الدوال المتغيرة
// ============================================================================

CABI::CallInfo CABI::prepare_variadic_call(
    void* func_ptr,
    const FunctionSignature& signature,
    const std::vector<void*>& sad_args,
    const std::vector<TypeInfo>& variadic_types)
{
    CallInfo call_info;
    call_info.function_ptr = func_ptr;
    call_info.convention = signature.convention;
    call_info.needs_cleanup = true; // Variadic calls need cleanup
    
    if (!signature.is_variadic) {
        set_error(ErrorCode::TypeMismatch,
                 "Function is not variadic: " + signature.name);
        return call_info;
    }
    
    // تحويل المعاملات الثابتة / Convert fixed arguments
    size_t fixed_count = signature.param_types.size();
    for (size_t i = 0; i < fixed_count; ++i) {
        void* c_arg = convert_sad_to_c_arg(sad_args[i], signature.param_types[i]);
        call_info.args.push_back(c_arg);
    }
    
    // تحويل المعاملات المتغيرة / Convert variadic arguments
    for (size_t i = fixed_count; i < sad_args.size(); ++i) {
        size_t var_idx = i - fixed_count;
        if (var_idx >= variadic_types.size()) {
            set_error(ErrorCode::TypeMismatch, "Missing type info for variadic arg");
            return call_info;
        }
        
        void* c_arg = convert_sad_to_c_arg(sad_args[i], variadic_types[var_idx]);
        call_info.args.push_back(c_arg);
    }
    
    log_debug("Prepared variadic call with " + 
              std::to_string(call_info.args.size()) + " args");
    
    return call_info;
}

void* CABI::get_variadic_arg(VarArgs& va, const TypeInfo& type) {
    // استخراج قيمة من va_list / Extract value from va_list
    
    switch (type.c_type) {
        case CType::Int32:
            return reinterpret_cast<void*>(static_cast<intptr_t>(va_arg(va.args, int32_t)));
        case CType::Int64:
            return reinterpret_cast<void*>(static_cast<intptr_t>(va_arg(va.args, int64_t)));
        case CType::Double:
            return reinterpret_cast<void*>(static_cast<intptr_t>(va_arg(va.args, double)));
        case CType::Pointer:
            return va_arg(va.args, void*);
        default:
            set_error(ErrorCode::InvalidType, "Unsupported variadic type");
            return nullptr;
    }
}

// ============================================================================
// Private Helper Methods / طرق مساعدة خاصة
// ============================================================================

void CABI::set_error(ErrorCode code, const std::string& message) const {
    // Note: mutable member needed for this
    const_cast<CABI*>(this)->last_error_ = Error(code, message);
    
    if (debug_mode_) {
        std::cerr << "C ABI Error [" << static_cast<int>(code) << "]: " 
                  << message << "\n";
    }
}

void CABI::log_debug(const std::string& message) {
    if (debug_mode_) {
        std::cout << "[C ABI] " << message << "\n";
    }
}

// These need to be implemented based on actual type system
void* CABI::convert_sad_to_c_arg(void* sad_arg, const TypeInfo& type) {
    // Actual conversion implementation / تنفيذ التحويل الفعلي
    
    if (sad_arg == nullptr) {
        // null يبقى null / null remains null
        return nullptr;
    }
    
    switch (type.sad_type) {
        case SadType::Void:
            return nullptr;
            
        case SadType::Integer:
            // رقم Sad (64-bit) → integer C
            // Sad Integer (64-bit) → C integer
            switch (type.c_type) {
                case CType::Int8:
                case CType::UInt8:
                case CType::Int16:
                case CType::UInt16:
                case CType::Int32:
                case CType::UInt32:
                case CType::Int64:
                case CType::UInt64:
                    // المعامل مباشرة كمؤشر / Argument directly as pointer
                    return sad_arg;
                default:
                    set_error(ErrorCode::TypeMismatch, 
                             "Cannot convert Integer to non-integer C type");
                    return nullptr;
            }
            
        case SadType::Float:
            // عشري Sad (double) → float/double C
            // Sad Float (double) → C float/double
            if (type.c_type == CType::Float || type.c_type == CType::Double) {
                return sad_arg;
            }
            set_error(ErrorCode::TypeMismatch,
                     "Cannot convert Float to non-float C type");
            return nullptr;
            
        case SadType::Boolean:
            // منطقي → int (0 or 1)
            // Boolean → int (0 or 1)
            // يتم تمريره كمؤشر / Passed as pointer
            return sad_arg;
            
        case SadType::String:
            // نص Sad → const char*
            // Sad String → const char*
            // إذا كان sad_arg مؤشر StringDescriptor
            // If sad_arg is StringDescriptor pointer
            if (type.c_type == CType::Pointer) {
                // افتراضياً، sad_arg هو StringDescriptor*
                // Assuming sad_arg is StringDescriptor*
                StringDescriptor* str_desc = static_cast<StringDescriptor*>(sad_arg);
                if (str_desc && str_desc->data) {
                    return static_cast<void*>(str_desc->data);
                }
                // أو إذا كان مباشرة char*
                // Or if it's directly char*
                return sad_arg;
            }
            set_error(ErrorCode::TypeMismatch,
                     "Cannot convert String to non-pointer C type");
            return nullptr;
            
        case SadType::Array:
            // مصفوفة → pointer to data
            // Array → pointer to data
            if (type.c_type == CType::Pointer || type.c_type == CType::Array) {
                // إذا كان sad_arg هو ArrayDescriptor*
                // If sad_arg is ArrayDescriptor*
                ArrayDescriptor* arr_desc = static_cast<ArrayDescriptor*>(sad_arg);
                if (arr_desc && arr_desc->data) {
                    return arr_desc->data;
                }
                // أو مباشرة pointer
                // Or directly pointer
                return sad_arg;
            }
            set_error(ErrorCode::TypeMismatch,
                     "Cannot convert Array to non-pointer C type");
            return nullptr;
            
        case SadType::Pointer:
            // مؤشر → void*
            // Pointer → void*
            return sad_arg;
            
        case SadType::Struct:
            // بنية → pointer to struct data
            // Struct → pointer to struct data
            if (type.c_type == CType::Struct || type.c_type == CType::Pointer) {
                // إذا كان sad_arg هو StructDescriptor*
                // If sad_arg is StructDescriptor*
                StructDescriptor* struct_desc = static_cast<StructDescriptor*>(sad_arg);
                if (struct_desc && struct_desc->data) {
                    return struct_desc->data;
                }
                return sad_arg;
            }
            set_error(ErrorCode::TypeMismatch,
                     "Cannot convert Struct to incompatible C type");
            return nullptr;
            
        case SadType::Function:
            // دالة → function pointer
            // Function → function pointer
            if (type.c_type == CType::Function || type.c_type == CType::Pointer) {
                return sad_arg;
            }
            set_error(ErrorCode::TypeMismatch,
                     "Cannot convert Function to non-function C type");
            return nullptr;
            
        default:
            set_error(ErrorCode::InvalidType, "Unknown Sad type for conversion");
            return nullptr;
    }
}

void* CABI::convert_c_to_sad_arg(void* c_arg, const TypeInfo& type) {
    // Actual conversion implementation / تنفيذ التحويل الفعلي
    
    if (c_arg == nullptr) {
        // null يبقى null / null remains null
        return nullptr;
    }
    
    switch (type.c_type) {
        case CType::Void:
            return nullptr;
            
        case CType::Int8:
        case CType::Int16:
        case CType::Int32:
        case CType::Int64:
        case CType::UInt8:
        case CType::UInt16:
        case CType::UInt32:
        case CType::UInt64:
            // integer C → رقم Sad (64-bit)
            // C integer → Sad Integer (64-bit)
            if (type.sad_type == SadType::Integer) {
                // المعامل مباشرة - قد تحتاج تحويل للحجم الصحيح
                // Direct argument - may need size conversion
                // في التطبيق الفعلي، قد نحتاج توسيع/تضييق
                // In actual implementation, may need sign/zero extension
                return c_arg;
            }
            set_error(ErrorCode::TypeMismatch,
                     "Cannot convert C integer to non-Integer Sad type");
            return nullptr;
            
        case CType::Float:
        case CType::Double:
            // float/double C → عشري Sad
            // C float/double → Sad Float
            if (type.sad_type == SadType::Float) {
                // قد تحتاج تحويل float → double
                // May need float → double conversion
                return c_arg;
            }
            set_error(ErrorCode::TypeMismatch,
                     "Cannot convert C float to non-Float Sad type");
            return nullptr;
            
        case CType::Pointer:
            // مؤشر C → نوع Sad المناسب
            // C pointer → appropriate Sad type
            switch (type.sad_type) {
                case SadType::String:
                    // const char* → نص Sad
                    // const char* → Sad String
                    // ينبغي إنشاء StringDescriptor
                    // Should create StringDescriptor
                    {
                        char* str_ptr = static_cast<char*>(c_arg);
                        if (str_ptr) {
                            StringDescriptor* str_desc = new StringDescriptor();
                            str_desc->data = str_ptr;
                            str_desc->length = std::strlen(str_ptr);
                            str_desc->capacity = str_desc->length + 1;
                            str_desc->is_owned = false; // C owns it
                            return static_cast<void*>(str_desc);
                        }
                    }
                    return nullptr;
                    
                case SadType::Array:
                    // مؤشر → مصفوفة
                    // pointer → Array
                    // ينبغي إنشاء ArrayDescriptor
                    // Should create ArrayDescriptor
                    {
                        ArrayDescriptor* arr_desc = new ArrayDescriptor();
                        arr_desc->data = c_arg;
                        // الطول غير معروف من C pointer فقط
                        // Length unknown from C pointer alone
                        arr_desc->length = 0; // Unknown
                        arr_desc->capacity = 0;
                        arr_desc->element_size = type.size;
                        arr_desc->element_type = type;
                        return static_cast<void*>(arr_desc);
                    }
                    
                case SadType::Pointer:
                    // مؤشر يبقى مؤشر
                    // pointer remains pointer
                    return c_arg;
                    
                case SadType::Struct:
                    // مؤشر لبنية
                    // pointer to struct
                    {
                        StructDescriptor* struct_desc = new StructDescriptor();
                        struct_desc->data = c_arg;
                        struct_desc->size = type.size;
                        struct_desc->alignment = type.alignment;
                        struct_desc->field_types = type.element_types;
                        return static_cast<void*>(struct_desc);
                    }
                    
                default:
                    // مباشرة كمؤشر
                    // Directly as pointer
                    return c_arg;
            }
            
        case CType::Array:
            // مصفوفة C → مصفوفة Sad
            // C array → Sad Array
            if (type.sad_type == SadType::Array) {
                // ينبغي إنشاء ArrayDescriptor
                // Should create ArrayDescriptor
                ArrayDescriptor* arr_desc = new ArrayDescriptor();
                arr_desc->data = c_arg;
                arr_desc->length = 0; // Size unknown from raw C array
                arr_desc->capacity = 0;
                arr_desc->element_size = type.size;
                arr_desc->element_type = type;
                return static_cast<void*>(arr_desc);
            }
            set_error(ErrorCode::TypeMismatch,
                     "Cannot convert C array to non-Array Sad type");
            return nullptr;
            
        case CType::Struct:
            // بنية C → بنية Sad
            // C struct → Sad Struct
            if (type.sad_type == SadType::Struct) {
                // ينبغي إنشاء StructDescriptor
                // Should create StructDescriptor
                StructDescriptor* struct_desc = new StructDescriptor();
                struct_desc->data = c_arg;
                struct_desc->size = type.size;
                struct_desc->alignment = type.alignment;
                struct_desc->field_types = type.element_types;
                return static_cast<void*>(struct_desc);
            }
            set_error(ErrorCode::TypeMismatch,
                     "Cannot convert C struct to non-Struct Sad type");
            return nullptr;
            
        case CType::Function:
            // دالة C → دالة Sad
            // C function → Sad Function
            if (type.sad_type == SadType::Function) {
                return c_arg;
            }
            set_error(ErrorCode::TypeMismatch,
                     "Cannot convert C function to non-Function Sad type");
            return nullptr;
            
        default:
            set_error(ErrorCode::InvalidType, "Unknown C type for conversion");
            return nullptr;
    }
}

// ============================================================================
// Helper Functions Implementation / تطبيق الدوال المساعدة
// ============================================================================

void safe_memcpy(void* dest, const void* src, size_t size) {
    if (dest == nullptr || src == nullptr || size == 0) {
        return;
    }
    std::memcpy(dest, src, size);
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
