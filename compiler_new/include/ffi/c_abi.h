// تعطيل تحذير Unicode للنصوص العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file c_abi.h
 * @brief (AR) واجهة ثنائية التطبيق C - تعريفات ABI للتكامل مع C/C++
 *        (EN) C Application Binary Interface - ABI definitions for C/C++ integration
 * 
 * @details
 * (AR) هذا الملف يحتوي على التعريفات الأساسية لنظام واجهة الدوال الخارجية (FFI)
 *      في لغة "ص". يتيح هذا النظام استدعاء دوال C/C++ من كود "ص" بشكل آمن وفعال.
 *      
 *      الميزات الرئيسية:
 *      - دعم جميع اتفاقيات الاستدعاء (cdecl, stdcall, fastcall, إلخ)
 *      - تحويل أنواع البيانات بين "ص" و C
 *      - إدارة الذاكرة الآمنة للموارد الخارجية
 *      - دعم التعليقات التوضيحية للملكية (#[ملك_النتيجة])
 * 
 * (EN) This file contains core definitions for the Foreign Function Interface (FFI)
 *      system in Sad language. This system enables safe and efficient calling of
 *      C/C++ functions from Sad code.
 *      
 *      Key features:
 *      - Support for all calling conventions (cdecl, stdcall, fastcall, etc.)
 *      - Data type conversion between Sad and C
 *      - Safe memory management for external resources
 *      - Support for ownership annotations (#[owns_result])
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2025-01-18
 * @version 1.0.0
 * @license MIT License
 * 
 * @example
 * @code{.cpp}
 * // مثال على استخدام C ABI / Example of using C ABI
 * 
 * // تعريف دالة خارجية / Define external function
 * ExternFunctionDecl func;
 * func.setName("printf");
 * func.setCallingConvention(CallingConvention::CDECL);
 * func.setReturnType(CTypeMapper::sadToCType(intType));
 * func.addParameter("format", CTypeMapper::sadToCType(stringType));
 * func.setVariadic(true);
 * 
 * // توليد كود الاستدعاء / Generate call code
 * CABICodeGenerator generator(llvmContext);
 * auto callValue = generator.generateCall(func, args);
 * @endcode
 */

#ifndef SAD_FFI_C_ABI_H
#define SAD_FFI_C_ABI_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <optional>
#include <functional>

// إعادة التوجيه للأنواع / Forward declarations + LLVM headers
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

namespace Sad {
namespace FFI {

// ============================================================================
//                    (AR) الكلمات المفتاحية العربية للـ FFI
//                    (EN) Arabic FFI Keywords
// ============================================================================

/**
 * @namespace ArabicFFIKeywords
 * @brief (AR) الكلمات المفتاحية العربية لنظام FFI
 *        (EN) Arabic keywords for FFI system
 */
namespace ArabicFFIKeywords {
    // --- (AR) كلمات extern المفتاحية / (EN) Extern keywords ---
    extern const char* const KHARIJI;           // خارجي - extern
    extern const char* const KHARIJI_C;         // خارجي_س - extern "C"
    extern const char* const KHARIJI_CPP;       // خارجي_سي++ - extern "C++"
    
    // --- (AR) اتفاقيات الاستدعاء / (EN) Calling conventions ---
    extern const char* const ISTIDAA_QIYASI;    // استدعاء_قياسي - cdecl
    extern const char* const ISTIDAA_WINDOWS;   // استدعاء_ويندوز - stdcall
    extern const char* const ISTIDAA_SARI;      // استدعاء_سريع - fastcall
    extern const char* const ISTIDAA_MUTAWAQIR; // استدعاء_متوافق - thiscall
    
    // --- (AR) تعليقات الملكية / (EN) Ownership annotations ---
    extern const char* const MALIK_NATIJA;      // #[ملك_النتيجة] - owns result
    extern const char* const YUHARRIR_BI;       // #[يُحرر_بـ:] - freed by
    extern const char* const LA_YUHARRIR;       // #[لا_يُحرر] - no free
    extern const char* const MUSTAAR;           // #[مستعار] - borrowed
    
    // --- (AR) أنواع C الأساسية / (EN) Basic C types ---
    extern const char* const C_VOID;            // فراغ_س - void
    extern const char* const C_CHAR;            // حرف_س - char
    extern const char* const C_SHORT;           // قصير_س - short
    extern const char* const C_INT;             // صحيح_س - int
    extern const char* const C_LONG;            // طويل_س - long
    extern const char* const C_LONGLONG;        // طويل_جداً_س - long long
    extern const char* const C_FLOAT;           // عشري_س - float
    extern const char* const C_DOUBLE;          // مزدوج_س - double
    extern const char* const C_SIZE_T;          // حجم_س - size_t
    extern const char* const C_PTRDIFF_T;       // فرق_مؤشر_س - ptrdiff_t
    
    // --- (AR) مؤهلات الأنواع / (EN) Type qualifiers ---
    extern const char* const BIDOON_ISHARA;     // بدون_إشارة - unsigned
    extern const char* const BI_ISHARA;         // بإشارة - signed
    extern const char* const THABIT;            // ثابت - const
    extern const char* const MUTAQQALIB;        // متقلب - volatile
}

// ============================================================================
//                    (AR) اتفاقيات الاستدعاء
//                    (EN) Calling Conventions
// ============================================================================

/**
 * @enum CallingConvention
 * @brief (AR) اتفاقيات الاستدعاء المدعومة لدوال C/C++
 *        (EN) Supported calling conventions for C/C++ functions
 * 
 * @details
 * (AR) اتفاقية الاستدعاء تحدد كيفية تمرير المعاملات وإرجاع القيم وتنظيف المكدس.
 *      كل منصة لها اتفاقياتها الافتراضية:
 *      - Windows x64: Microsoft x64
 *      - Linux/macOS x64: System V AMD64 ABI
 *      - Windows x86: cdecl (افتراضي) أو stdcall
 * 
 * (EN) Calling convention defines how parameters are passed, values returned,
 *      and stack cleanup. Each platform has its default conventions:
 *      - Windows x64: Microsoft x64
 *      - Linux/macOS x64: System V AMD64 ABI  
 *      - Windows x86: cdecl (default) or stdcall
 */
enum class CallingConvention {
    // --- (AR) الاتفاقيات الأساسية / (EN) Basic conventions ---
    
    /**
     * @brief (AR) cdecl - الاتفاقية القياسية لـ C
     *        (EN) cdecl - Standard C calling convention
     * 
     * (AR) - المستدعي ينظف المكدس
     *      - المعاملات تُمرر من اليمين لليسار
     *      - تدعم الدوال متغيرة المعاملات (variadic)
     * 
     * (EN) - Caller cleans up the stack
     *      - Parameters passed right-to-left
     *      - Supports variadic functions
     */
    CDECL,
    
    /**
     * @brief (AR) stdcall - اتفاقية Windows API
     *        (EN) stdcall - Windows API convention
     * 
     * (AR) - المُستدعَى ينظف المكدس
     *      - المعاملات تُمرر من اليمين لليسار
     *      - لا تدعم variadic
     * 
     * (EN) - Callee cleans up the stack
     *      - Parameters passed right-to-left
     *      - Does not support variadic
     */
    STDCALL,
    
    /**
     * @brief (AR) fastcall - اتفاقية سريعة مع سجلات
     *        (EN) fastcall - Fast convention using registers
     * 
     * (AR) - أول معاملين في ECX و EDX
     *      - الباقي على المكدس
     *      - المُستدعَى ينظف المكدس
     * 
     * (EN) - First two parameters in ECX and EDX
     *      - Rest on stack
     *      - Callee cleans up the stack
     */
    FASTCALL,
    
    /**
     * @brief (AR) thiscall - اتفاقية طرق C++
     *        (EN) thiscall - C++ method convention
     * 
     * (AR) - مؤشر this في ECX
     *      - المعاملات على المكدس
     * 
     * (EN) - this pointer in ECX
     *      - Parameters on stack
     */
    THISCALL,
    
    // --- (AR) اتفاقيات x64 / (EN) x64 conventions ---
    
    /**
     * @brief (AR) Microsoft x64 - اتفاقية Windows 64-bit
     *        (EN) Microsoft x64 - Windows 64-bit convention
     * 
     * (AR) - أول 4 معاملات في RCX, RDX, R8, R9
     *      - القيم العشرية في XMM0-XMM3
     *      - منطقة الظل (shadow space) 32 بايت
     * 
     * (EN) - First 4 parameters in RCX, RDX, R8, R9
     *      - Floating point in XMM0-XMM3
     *      - 32-byte shadow space
     */
    WIN64,
    
    /**
     * @brief (AR) System V AMD64 - اتفاقية Linux/macOS
     *        (EN) System V AMD64 - Linux/macOS convention
     * 
     * (AR) - 6 معاملات صحيحة في RDI, RSI, RDX, RCX, R8, R9
     *      - 8 معاملات عشرية في XMM0-XMM7
     *      - لا توجد منطقة ظل
     * 
     * (EN) - 6 integer parameters in RDI, RSI, RDX, RCX, R8, R9
     *      - 8 floating point parameters in XMM0-XMM7
     *      - No shadow space
     */
    SYSV_AMD64,
    
    // --- (AR) اتفاقيات ARM / (EN) ARM conventions ---
    
    /**
     * @brief (AR) AAPCS - اتفاقية ARM القياسية
     *        (EN) AAPCS - Standard ARM convention
     */
    AAPCS,
    
    /**
     * @brief (AR) AAPCS64 - اتفاقية ARM 64-bit
     *        (EN) AAPCS64 - ARM 64-bit convention
     */
    AAPCS64,
    
    // --- (AR) اتفاقيات خاصة / (EN) Special conventions ---
    
    /**
     * @brief (AR) تلقائي - يُحدد حسب المنصة المستهدفة
     *        (EN) Auto - Determined by target platform
     */
    AUTO,
    
    /**
     * @brief (AR) مخصص - اتفاقية معرّفة من المستخدم
     *        (EN) Custom - User-defined convention
     */
    CUSTOM
};

// ============================================================================
//                    (AR) أنواع C الأساسية
//                    (EN) Basic C Types
// ============================================================================

/**
 * @enum CBasicType
 * @brief (AR) أنواع C الأساسية المدعومة في FFI
 *        (EN) Basic C types supported in FFI
 */
enum class CBasicType {
    // --- (AR) أنواع فارغة / (EN) Void types ---
    VOID,           // فراغ - void
    
    // --- (AR) أنواع صحيحة / (EN) Integer types ---
    CHAR,           // حرف - char (8-bit)
    SCHAR,          // حرف_موقّع - signed char
    UCHAR,          // حرف_غير_موقّع - unsigned char
    SHORT,          // قصير - short (16-bit)
    USHORT,         // قصير_غير_موقّع - unsigned short
    INT,            // صحيح - int (32-bit)
    UINT,           // صحيح_غير_موقّع - unsigned int
    LONG,           // طويل - long (32/64-bit حسب المنصة)
    ULONG,          // طويل_غير_موقّع - unsigned long
    LONGLONG,       // طويل_جداً - long long (64-bit)
    ULONGLONG,      // طويل_جداً_غير_موقّع - unsigned long long
    
    // --- (AR) أنواع عشرية / (EN) Floating point types ---
    FLOAT,          // عشري - float (32-bit)
    DOUBLE,         // مزدوج - double (64-bit)
    LONGDOUBLE,     // مزدوج_طويل - long double (80/128-bit)
    
    // --- (AR) أنواع منطقية / (EN) Boolean types ---
    BOOL,           // منطقي_س - _Bool / bool
    
    // --- (AR) أنواع الحجم / (EN) Size types ---
    SIZE_T,         // حجم_س - size_t
    SSIZE_T,        // حجم_موقّع_س - ssize_t
    PTRDIFF_T,      // فرق_مؤشر - ptrdiff_t
    INTPTR_T,       // صحيح_مؤشر - intptr_t
    UINTPTR_T,      // صحيح_مؤشر_غير_موقّع - uintptr_t
    
    // --- (AR) أنواع ثابتة الحجم / (EN) Fixed-size types ---
    INT8,           // ص8 - int8_t
    UINT8,          // ص_غ8 - uint8_t
    INT16,          // ص16 - int16_t
    UINT16,         // ص_غ16 - uint16_t
    INT32,          // ص32 - int32_t
    UINT32,         // ص_غ32 - uint32_t
    INT64,          // ص64 - int64_t
    UINT64,         // ص_غ64 - uint64_t
    
    // --- (AR) أنواع خاصة / (EN) Special types ---
    WCHAR_T,        // حرف_عريض - wchar_t
    CHAR16_T,       // حرف16 - char16_t
    CHAR32_T        // حرف32 - char32_t
};

// ============================================================================
//                    (AR) إعادة توجيه الأنواع
//                    (EN) Type Forward Declarations
// ============================================================================

class CType;
class CPointerType;
class CArrayType;
class CFunctionType;
class CStructType;
class CUnionType;
class CEnumType;
class ExternFunctionDecl;
class ExternBlockDecl;
class CABICodeGenerator;
class FFIOwnershipTracker;

using CTypePtr = std::shared_ptr<CType>;
using CPointerTypePtr = std::shared_ptr<CPointerType>;
using CArrayTypePtr = std::shared_ptr<CArrayType>;
using CFunctionTypePtr = std::shared_ptr<CFunctionType>;
using CStructTypePtr = std::shared_ptr<CStructType>;
using CUnionTypePtr = std::shared_ptr<CUnionType>;
using CEnumTypePtr = std::shared_ptr<CEnumType>;
using ExternFunctionDeclPtr = std::shared_ptr<ExternFunctionDecl>;
using ExternBlockDeclPtr = std::shared_ptr<ExternBlockDecl>;

// ============================================================================
//                    (AR) نوع C الأساسي
//                    (EN) Base C Type
// ============================================================================

/**
 * @class CType
 * @brief (AR) الصنف الأساسي لجميع أنواع C في نظام FFI
 *        (EN) Base class for all C types in FFI system
 * 
 * @details
 * (AR) يمثل هذا الصنف الأساس لجميع أنواع C التي يمكن استخدامها في FFI.
 *      يوفر واجهة موحدة للتحويل بين أنواع "ص" وأنواع C.
 * 
 * (EN) This class represents the base for all C types usable in FFI.
 *      Provides unified interface for conversion between Sad and C types.
 */
class CType {
public:
    /**
     * @enum Kind
     * @brief (AR) تصنيف النوع
     *        (EN) Type classification
     */
    enum class Kind {
        BASIC,      // نوع أساسي (int, char, etc.)
        POINTER,    // مؤشر (*T)
        ARRAY,      // مصفوفة (T[N])
        FUNCTION,   // دالة (int (*)(int, int))
        STRUCT,     // هيكل (struct { ... })
        UNION,      // اتحاد (union { ... })
        ENUM,       // تعداد (enum { ... })
        TYPEDEF,    // اسم مستعار (typedef)
        QUALIFIED   // نوع مؤهل (const, volatile)
    };
    
    // --- (AR) البناء والهدم / (EN) Construction and destruction ---
    
    /**
     * @brief (AR) هدّام افتراضي
     *        (EN) Virtual destructor
     */
    virtual ~CType() = default;
    
    // --- (AR) الخصائص الأساسية / (EN) Basic properties ---
    
    /**
     * @brief (AR) إرجاع تصنيف النوع
     *        (EN) Returns type classification
     */
    virtual Kind getKind() const = 0;
    
    /**
     * @brief (AR) إرجاع اسم النوع بالعربية
     *        (EN) Returns Arabic type name
     */
    virtual std::string getArabicName() const = 0;
    
    /**
     * @brief (AR) إرجاع اسم النوع بالإنجليزية (C syntax)
     *        (EN) Returns English type name (C syntax)
     */
    virtual std::string getCName() const = 0;
    
    /**
     * @brief (AR) إرجاع حجم النوع بالبايت
     *        (EN) Returns type size in bytes
     * 
     * @param is64Bit (AR) هل المنصة 64 بت؟ (EN) Is platform 64-bit?
     * @return (AR) حجم النوع بالبايت (EN) Type size in bytes
     */
    virtual size_t getSize(bool is64Bit = true) const = 0;
    
    /**
     * @brief (AR) إرجاع محاذاة النوع بالبايت
     *        (EN) Returns type alignment in bytes
     * 
     * @param is64Bit (AR) هل المنصة 64 بت؟ (EN) Is platform 64-bit?
     * @return (AR) محاذاة النوع بالبايت (EN) Type alignment in bytes
     */
    virtual size_t getAlignment(bool is64Bit = true) const = 0;
    
    // --- (AR) مؤهلات النوع / (EN) Type qualifiers ---
    
    /**
     * @brief (AR) هل النوع ثابت (const)؟
     *        (EN) Is type const-qualified?
     */
    bool isConst() const { return isConst_; }
    
    /**
     * @brief (AR) هل النوع متقلب (volatile)؟
     *        (EN) Is type volatile-qualified?
     */
    bool isVolatile() const { return isVolatile_; }
    
    /**
     * @brief (AR) هل النوع مقيد (restrict)؟
     *        (EN) Is type restrict-qualified?
     */
    bool isRestrict() const { return isRestrict_; }
    
    /**
     * @brief (AR) تعيين مؤهل الثبات
     *        (EN) Set const qualifier
     */
    void setConst(bool value) { isConst_ = value; }
    
    /**
     * @brief (AR) تعيين مؤهل التقلب
     *        (EN) Set volatile qualifier
     */
    void setVolatile(bool value) { isVolatile_ = value; }
    
    /**
     * @brief (AR) تعيين مؤهل التقييد
     *        (EN) Set restrict qualifier
     */
    void setRestrict(bool value) { isRestrict_ = value; }
    
    // --- (AR) تحويل LLVM / (EN) LLVM conversion ---
    
    /**
     * @brief (AR) تحويل إلى نوع LLVM
     *        (EN) Convert to LLVM type
     * 
     * @param context (AR) سياق LLVM (EN) LLVM context
     * @return (AR) نوع LLVM المقابل (EN) Corresponding LLVM type
     */
    virtual llvm::Type* toLLVMType(llvm::LLVMContext& context) const = 0;
    
    // --- (AR) فحوصات النوع / (EN) Type checks ---
    
    /**
     * @brief (AR) هل النوع يمثل مؤشراً؟
     *        (EN) Is this a pointer type?
     */
    virtual bool isPointer() const { return false; }
    
    /**
     * @brief (AR) هل النوع يمثل مصفوفة؟
     *        (EN) Is this an array type?
     */
    virtual bool isArray() const { return false; }
    
    /**
     * @brief (AR) هل النوع يمثل دالة؟
     *        (EN) Is this a function type?
     */
    virtual bool isFunction() const { return false; }
    
    /**
     * @brief (AR) هل النوع يمثل هيكلاً؟
     *        (EN) Is this a struct type?
     */
    virtual bool isStruct() const { return false; }
    
    /**
     * @brief (AR) هل النوع يمكن تمريره بالقيمة؟
     *        (EN) Can this type be passed by value?
     */
    virtual bool isPassByValue() const { return true; }
    
    /**
     * @brief (AR) هل النوع يحتاج تحرير يدوي للذاكرة؟
     *        (EN) Does this type require manual memory management?
     */
    virtual bool requiresManualMemoryManagement() const { return false; }

protected:
    bool isConst_ = false;      // ثابت - const
    bool isVolatile_ = false;   // متقلب - volatile
    bool isRestrict_ = false;   // مقيد - restrict
};

// ============================================================================
//                    (AR) نوع C الأساسي
//                    (EN) Basic C Type
// ============================================================================

/**
 * @class CBasicTypeImpl
 * @brief (AR) تنفيذ أنواع C الأساسية (int, char, float, etc.)
 *        (EN) Implementation of basic C types (int, char, float, etc.)
 */
class CBasicTypeImpl : public CType {
public:
    /**
     * @brief (AR) بناء نوع C أساسي
     *        (EN) Construct a basic C type
     * 
     * @param type (AR) نوع البيانات الأساسي (EN) Basic data type
     */
    explicit CBasicTypeImpl(CBasicType type);
    
    // --- (AR) تنفيذ الواجهة / (EN) Interface implementation ---
    
    Kind getKind() const override { return Kind::BASIC; }
    std::string getArabicName() const override;
    std::string getCName() const override;
    size_t getSize(bool is64Bit = true) const override;
    size_t getAlignment(bool is64Bit = true) const override;
    llvm::Type* toLLVMType(llvm::LLVMContext& context) const override;
    
    /**
     * @brief (AR) إرجاع نوع البيانات الأساسي
     *        (EN) Returns basic data type
     */
    CBasicType getBasicType() const { return basicType_; }
    
    /**
     * @brief (AR) هل النوع صحيح (integer)؟
     *        (EN) Is this an integer type?
     */
    bool isInteger() const;
    
    /**
     * @brief (AR) هل النوع عشري (floating point)؟
     *        (EN) Is this a floating point type?
     */
    bool isFloatingPoint() const;
    
    /**
     * @brief (AR) هل النوع موقّع (signed)؟
     *        (EN) Is this a signed type?
     */
    bool isSigned() const;

private:
    CBasicType basicType_;  // نوع البيانات الأساسي
};

// ============================================================================
//                    (AR) نوع المؤشر
//                    (EN) Pointer Type
// ============================================================================

/**
 * @class CPointerType
 * @brief (AR) نوع المؤشر في C (*T)
 *        (EN) C pointer type (*T)
 */
class CPointerType : public CType {
public:
    /**
     * @brief (AR) بناء نوع مؤشر
     *        (EN) Construct a pointer type
     * 
     * @param pointeeType (AR) النوع المُشار إليه (EN) Pointed-to type
     */
    explicit CPointerType(CTypePtr pointeeType);
    
    // --- (AR) تنفيذ الواجهة / (EN) Interface implementation ---
    
    Kind getKind() const override { return Kind::POINTER; }
    std::string getArabicName() const override;
    std::string getCName() const override;
    size_t getSize(bool is64Bit = true) const override;
    size_t getAlignment(bool is64Bit = true) const override;
    llvm::Type* toLLVMType(llvm::LLVMContext& context) const override;
    bool isPointer() const override { return true; }
    bool requiresManualMemoryManagement() const override { return true; }
    
    /**
     * @brief (AR) إرجاع النوع المُشار إليه
     *        (EN) Returns pointed-to type
     */
    CTypePtr getPointeeType() const { return pointeeType_; }
    
    /**
     * @brief (AR) هل هذا مؤشر لدالة؟
     *        (EN) Is this a function pointer?
     */
    bool isFunctionPointer() const;
    
    /**
     * @brief (AR) هل هذا مؤشر لـ void؟
     *        (EN) Is this a void pointer?
     */
    bool isVoidPointer() const;

private:
    CTypePtr pointeeType_;  // النوع المُشار إليه
};

// ============================================================================
//                    (AR) نوع المصفوفة
//                    (EN) Array Type
// ============================================================================

/**
 * @class CArrayType
 * @brief (AR) نوع المصفوفة في C (T[N])
 *        (EN) C array type (T[N])
 */
class CArrayType : public CType {
public:
    /**
     * @brief (AR) بناء نوع مصفوفة
     *        (EN) Construct an array type
     * 
     * @param elementType (AR) نوع العناصر (EN) Element type
     * @param size (AR) حجم المصفوفة (0 = غير محدد) (EN) Array size (0 = unspecified)
     */
    CArrayType(CTypePtr elementType, size_t size = 0);
    
    // --- (AR) تنفيذ الواجهة / (EN) Interface implementation ---
    
    Kind getKind() const override { return Kind::ARRAY; }
    std::string getArabicName() const override;
    std::string getCName() const override;
    size_t getSize(bool is64Bit = true) const override;
    size_t getAlignment(bool is64Bit = true) const override;
    llvm::Type* toLLVMType(llvm::LLVMContext& context) const override;
    bool isArray() const override { return true; }
    bool isPassByValue() const override { return false; }
    
    /**
     * @brief (AR) إرجاع نوع العناصر
     *        (EN) Returns element type
     */
    CTypePtr getElementType() const { return elementType_; }
    
    /**
     * @brief (AR) إرجاع حجم المصفوفة
     *        (EN) Returns array size
     */
    size_t getArraySize() const { return arraySize_; }
    
    /**
     * @brief (AR) هل حجم المصفوفة محدد؟
     *        (EN) Is array size specified?
     */
    bool hasDeterminedSize() const { return arraySize_ > 0; }

private:
    CTypePtr elementType_;  // نوع العناصر
    size_t arraySize_;      // حجم المصفوفة
};

// ============================================================================
//                    (AR) نوع الدالة
//                    (EN) Function Type
// ============================================================================

/**
 * @class CFunctionType
 * @brief (AR) نوع الدالة في C (int (*)(int, int))
 *        (EN) C function type (int (*)(int, int))
 */
class CFunctionType : public CType {
public:
    /**
     * @brief (AR) معامل دالة C
     *        (EN) C function parameter
     */
    struct Parameter {
        std::string name;       // اسم المعامل (اختياري)
        CTypePtr type;          // نوع المعامل
        bool isConst = false;   // هل المعامل ثابت؟
    };
    
    /**
     * @brief (AR) بناء نوع دالة
     *        (EN) Construct a function type
     * 
     * @param returnType (AR) نوع القيمة المُرجعة (EN) Return type
     * @param convention (AR) اتفاقية الاستدعاء (EN) Calling convention
     */
    CFunctionType(CTypePtr returnType, CallingConvention convention = CallingConvention::CDECL);
    
    // --- (AR) تنفيذ الواجهة / (EN) Interface implementation ---
    
    Kind getKind() const override { return Kind::FUNCTION; }
    std::string getArabicName() const override;
    std::string getCName() const override;
    size_t getSize(bool is64Bit = true) const override { return 0; }
    size_t getAlignment(bool is64Bit = true) const override { return 0; }
    llvm::Type* toLLVMType(llvm::LLVMContext& context) const override;
    bool isFunction() const override { return true; }
    bool isPassByValue() const override { return false; }
    
    // --- (AR) معاملات الدالة / (EN) Function parameters ---
    
    /**
     * @brief (AR) إضافة معامل للدالة
     *        (EN) Add parameter to function
     */
    void addParameter(const std::string& name, CTypePtr type, bool isConst = false);
    
    /**
     * @brief (AR) إرجاع قائمة المعاملات
     *        (EN) Returns parameter list
     */
    const std::vector<Parameter>& getParameters() const { return parameters_; }
    
    /**
     * @brief (AR) إرجاع نوع القيمة المُرجعة
     *        (EN) Returns return type
     */
    CTypePtr getReturnType() const { return returnType_; }
    
    /**
     * @brief (AR) إرجاع اتفاقية الاستدعاء
     *        (EN) Returns calling convention
     */
    CallingConvention getCallingConvention() const { return callingConvention_; }
    
    /**
     * @brief (AR) تعيين اتفاقية الاستدعاء
     *        (EN) Set calling convention
     */
    void setCallingConvention(CallingConvention conv) { callingConvention_ = conv; }
    
    /**
     * @brief (AR) هل الدالة متغيرة المعاملات (variadic)؟
     *        (EN) Is function variadic?
     */
    bool isVariadic() const { return isVariadic_; }
    
    /**
     * @brief (AR) تعيين حالة variadic
     *        (EN) Set variadic state
     */
    void setVariadic(bool value) { isVariadic_ = value; }

private:
    CTypePtr returnType_;                       // نوع القيمة المُرجعة
    std::vector<Parameter> parameters_;         // قائمة المعاملات
    CallingConvention callingConvention_;       // اتفاقية الاستدعاء
    bool isVariadic_ = false;                   // هل متغيرة المعاملات؟
};

// ============================================================================
//                    (AR) تصريح دالة خارجية
//                    (EN) External Function Declaration
// ============================================================================

/**
 * @class ExternFunctionDecl
 * @brief (AR) تصريح دالة C/C++ خارجية للاستخدام في كود "ص"
 *        (EN) External C/C++ function declaration for use in Sad code
 * 
 * @details
 * (AR) يمثل هذا الصنف تصريح دالة خارجية من مكتبة C/C++.
 *      يحتوي على جميع المعلومات اللازمة لتوليد كود الاستدعاء الصحيح.
 * 
 * (EN) This class represents an external function declaration from C/C++ library.
 *      Contains all information needed to generate correct call code.
 * 
 * @example
 * @code{.cpp}
 * // تصريح دالة printf / Declare printf function
 * ExternFunctionDecl printfDecl;
 * printfDecl.setOriginalName("printf");
 * printfDecl.setArabicName("اطبع_منسق");
 * printfDecl.setReturnType(makeBasicType(CBasicType::INT));
 * printfDecl.addParameter("format", makePointerType(makeBasicType(CBasicType::CHAR)));
 * printfDecl.setVariadic(true);
 * @endcode
 */
class ExternFunctionDecl {
public:
    // --- (AR) أنواع التعليقات التوضيحية / (EN) Annotation types ---
    
    /**
     * @enum OwnershipAnnotation
     * @brief (AR) تعليقات ملكية الذاكرة للـ FFI
     *        (EN) Memory ownership annotations for FFI
     */
    enum class OwnershipAnnotation {
        NONE,           // لا تعليق - no annotation
        OWNS_RESULT,    // #[ملك_النتيجة] - caller owns returned memory
        BORROWED,       // #[مستعار] - memory is borrowed, don't free
        TRANSFERS,      // #[ينقل] - ownership transfers to callee
        FREES_PARAM     // #[يُحرر_المعامل] - callee frees the parameter
    };
    
    /**
     * @struct ParameterInfo
     * @brief (AR) معلومات معامل الدالة الخارجية
     *        (EN) External function parameter information
     */
    struct ParameterInfo {
        std::string originalName;   // الاسم الأصلي (C)
        std::string arabicName;     // الاسم العربي
        CTypePtr type;              // النوع
        OwnershipAnnotation ownership = OwnershipAnnotation::NONE;
        std::string freeFunction;   // دالة التحرير (إذا كانت مملوكة)
    };
    
    // --- (AR) البناء / (EN) Construction ---
    
    ExternFunctionDecl() = default;
    ~ExternFunctionDecl() = default;
    
    // --- (AR) خصائص الدالة / (EN) Function properties ---
    
    /**
     * @brief (AR) تعيين الاسم الأصلي للدالة (C/C++)
     *        (EN) Set original function name (C/C++)
     */
    void setOriginalName(const std::string& name) { originalName_ = name; }
    std::string getOriginalName() const { return originalName_; }
    
    /**
     * @brief (AR) تعيين الاسم العربي للدالة
     *        (EN) Set Arabic function name
     */
    void setArabicName(const std::string& name) { arabicName_ = name; }
    std::string getArabicName() const { return arabicName_; }
    
    /**
     * @brief (AR) تعيين نوع القيمة المُرجعة
     *        (EN) Set return type
     */
    void setReturnType(CTypePtr type) { returnType_ = type; }
    CTypePtr getReturnType() const { return returnType_; }
    
    /**
     * @brief (AR) تعيين اتفاقية الاستدعاء
     *        (EN) Set calling convention
     */
    void setCallingConvention(CallingConvention conv) { callingConvention_ = conv; }
    CallingConvention getCallingConvention() const { return callingConvention_; }
    
    /**
     * @brief (AR) تعيين حالة variadic
     *        (EN) Set variadic state
     */
    void setVariadic(bool value) { isVariadic_ = value; }
    bool isVariadic() const { return isVariadic_; }
    
    /**
     * @brief (AR) تعيين اسم المكتبة
     *        (EN) Set library name
     */
    void setLibraryName(const std::string& lib) { libraryName_ = lib; }
    std::string getLibraryName() const { return libraryName_; }
    
    // --- (AR) المعاملات / (EN) Parameters ---
    
    /**
     * @brief (AR) إضافة معامل
     *        (EN) Add parameter
     */
    void addParameter(const std::string& originalName, 
                     const std::string& arabicName,
                     CTypePtr type,
                     OwnershipAnnotation ownership = OwnershipAnnotation::NONE,
                     const std::string& freeFunc = "");
    
    /**
     * @brief (AR) إرجاع قائمة المعاملات
     *        (EN) Returns parameter list
     */
    const std::vector<ParameterInfo>& getParameters() const { return parameters_; }
    
    // --- (AR) تعليقات الملكية / (EN) Ownership annotations ---
    
    /**
     * @brief (AR) تعيين تعليق ملكية النتيجة
     *        (EN) Set result ownership annotation
     */
    void setResultOwnership(OwnershipAnnotation ownership, const std::string& freeFunc = "");
    
    /**
     * @brief (AR) إرجاع تعليق ملكية النتيجة
     *        (EN) Returns result ownership annotation
     */
    OwnershipAnnotation getResultOwnership() const { return resultOwnership_; }
    
    /**
     * @brief (AR) إرجاع دالة تحرير النتيجة
     *        (EN) Returns result free function
     */
    std::string getResultFreeFunction() const { return resultFreeFunction_; }
    
    // --- (AR) التحقق / (EN) Validation ---
    
    /**
     * @brief (AR) التحقق من صحة التصريح
     *        (EN) Validate declaration
     */
    bool validate(std::string& error) const;

private:
    std::string originalName_;          // الاسم الأصلي (C)
    std::string arabicName_;            // الاسم العربي
    CTypePtr returnType_;               // نوع القيمة المُرجعة
    std::vector<ParameterInfo> parameters_;  // قائمة المعاملات
    CallingConvention callingConvention_ = CallingConvention::CDECL;
    bool isVariadic_ = false;           // هل متغيرة المعاملات؟
    std::string libraryName_;           // اسم المكتبة
    OwnershipAnnotation resultOwnership_ = OwnershipAnnotation::NONE;
    std::string resultFreeFunction_;    // دالة تحرير النتيجة
};

// ============================================================================
//                    (AR) مولد كود C ABI
//                    (EN) C ABI Code Generator
// ============================================================================

/**
 * @class CABICodeGenerator
 * @brief (AR) مولد كود LLVM IR لاستدعاء دوال C/C++
 *        (EN) LLVM IR code generator for calling C/C++ functions
 * 
 * @details
 * (AR) يقوم هذا الصنف بتوليد كود LLVM IR الصحيح لاستدعاء دوال C/C++
 *      مع مراعاة اتفاقيات الاستدعاء المختلفة ومحاذاة البيانات.
 * 
 * (EN) This class generates correct LLVM IR code for calling C/C++ functions
 *      while respecting different calling conventions and data alignment.
 */
class CABICodeGenerator {
public:
    /**
     * @brief (AR) بناء مولد الكود
     *        (EN) Construct code generator
     * 
     * @param context (AR) سياق LLVM (EN) LLVM context
     * @param module (AR) وحدة LLVM (EN) LLVM module
     */
    CABICodeGenerator(llvm::LLVMContext& context, llvm::Module& module);
    
    ~CABICodeGenerator() = default;
    
    // --- (AR) توليد الكود / (EN) Code generation ---
    
    /**
     * @brief (AR) تصريح دالة خارجية في الوحدة
     *        (EN) Declare external function in module
     * 
     * @param decl (AR) تصريح الدالة (EN) Function declaration
     * @return (AR) دالة LLVM (EN) LLVM function
     */
    llvm::Function* declareExternFunction(const ExternFunctionDecl& decl);
    
    /**
     * @brief (AR) توليد استدعاء دالة خارجية
     *        (EN) Generate external function call
     * 
     * @param decl (AR) تصريح الدالة (EN) Function declaration
     * @param args (AR) قائمة المعاملات (EN) Argument list
     * @param builder (AR) باني IR (EN) IR builder
     * @return (AR) قيمة النتيجة (EN) Result value
     */
    llvm::Value* generateCall(const ExternFunctionDecl& decl,
                             const std::vector<llvm::Value*>& args,
                             llvm::IRBuilder<>& builder);
    
    /**
     * @brief (AR) توليد كود تحويل معامل من نوع "ص" إلى نوع C
     *        (EN) Generate parameter conversion code from Sad type to C type
     */
    llvm::Value* convertSadToC(llvm::Value* sadValue, 
                               CTypePtr cType,
                               llvm::IRBuilder<>& builder);
    
    /**
     * @brief (AR) توليد كود تحويل قيمة مُرجعة من نوع C إلى نوع "ص"
     *        (EN) Generate return value conversion code from C type to Sad type
     */
    llvm::Value* convertCToSad(llvm::Value* cValue,
                               CTypePtr cType,
                               llvm::IRBuilder<>& builder);
    
    // --- (AR) إدارة الذاكرة / (EN) Memory management ---
    
    /**
     * @brief (AR) توليد كود تحرير الذاكرة للنتيجة
     *        (EN) Generate memory cleanup code for result
     */
    void generateResultCleanup(const ExternFunctionDecl& decl,
                              llvm::Value* result,
                              llvm::IRBuilder<>& builder);
    
    // --- (AR) خيارات المنصة / (EN) Platform options ---
    
    /**
     * @brief (AR) تعيين هدف 64-بت
     *        (EN) Set 64-bit target
     */
    void setIs64Bit(bool value) { is64Bit_ = value; }
    
    /**
     * @brief (AR) تعيين منصة Windows
     *        (EN) Set Windows platform
     */
    void setIsWindows(bool value) { isWindows_ = value; }

private:
    llvm::LLVMContext& context_;    // سياق LLVM
    llvm::Module& module_;          // وحدة LLVM
    bool is64Bit_ = true;           // هل 64 بت؟
    bool isWindows_ = true;         // هل Windows؟
    
    // --- (AR) دوال مساعدة / (EN) Helper functions ---
    
    /**
     * @brief (AR) تحويل اتفاقية الاستدعاء إلى LLVM
     *        (EN) Convert calling convention to LLVM
     */
    unsigned toLLVMCallingConv(CallingConvention conv) const;
    
    /**
     * @brief (AR) إنشاء نوع دالة LLVM
     *        (EN) Create LLVM function type
     */
    llvm::FunctionType* createFunctionType(const ExternFunctionDecl& decl) const;
};

// ============================================================================
//                    (AR) دوال مساعدة لإنشاء الأنواع
//                    (EN) Type creation helper functions
// ============================================================================

/**
 * @brief (AR) إنشاء نوع C أساسي
 *        (EN) Create basic C type
 */
CTypePtr makeBasicType(CBasicType type);

/**
 * @brief (AR) إنشاء نوع مؤشر
 *        (EN) Create pointer type
 */
CTypePtr makePointerType(CTypePtr pointeeType);

/**
 * @brief (AR) إنشاء نوع مصفوفة
 *        (EN) Create array type
 */
CTypePtr makeArrayType(CTypePtr elementType, size_t size = 0);

/**
 * @brief (AR) إنشاء نوع دالة
 *        (EN) Create function type
 */
CTypePtr makeFunctionType(CTypePtr returnType, CallingConvention conv = CallingConvention::CDECL);

} // namespace FFI
} // namespace Sad

#endif // SAD_FFI_C_ABI_H
