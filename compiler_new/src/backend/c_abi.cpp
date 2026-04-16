// تعطيل تحذير Unicode للنصوص العربية
#ifdef _MSC_VER
#pragma warning(disable : 4819)
#endif

/**
 * @file c_abi.cpp
 * @brief (AR) تنفيذ واجهة ثنائية التطبيق C - اتفاقيات الاستدعاء وتوليد الكود
 *        (EN) C Application Binary Interface Implementation - Calling conventions and code generation
 *
 * @details
 * (AR) هذا الملف يحتوي على تنفيذ نظام FFI للغة "ص".
 *      يتضمن:
 *      - تنفيذ أنواع C الأساسية والمركبة
 *      - تحويل أنواع "ص" إلى أنواع C والعكس
 *      - توليد كود LLVM IR لاستدعاء دوال C/C++
 *      - إدارة اتفاقيات الاستدعاء المختلفة
 *
 * (EN) This file contains the FFI system implementation for Sad language.
 *      Includes:
 *      - Implementation of basic and composite C types
 *      - Conversion between Sad types and C types
 *      - LLVM IR code generation for C/C++ function calls
 *      - Management of different calling conventions
 *
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2025-01-18
 * @version 1.0.0
 * @license MIT License
 */

#include "ffi/c_abi.h"

// تضمينات LLVM / LLVM includes
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/Attributes.h>

#include <sstream>
#include <cassert>
#include <stdexcept>

namespace Sad
{
    namespace FFI
    {

        // ============================================================================
        //                    (AR) تعريف الكلمات المفتاحية العربية
        //                    (EN) Arabic Keywords Definition
        // ============================================================================

        namespace ArabicFFIKeywords
        {
            // --- (AR) كلمات extern المفتاحية / (EN) Extern keywords ---
            // خارجي - extern
            const char *const KHARIJI = "\xd8\xae\xd8\xa7\xd8\xb1\xd8\xac\xd9\x8a";
            // خارجي_س - extern "C"
            const char *const KHARIJI_C = "\xd8\xae\xd8\xa7\xd8\xb1\xd8\xac\xd9\x8a_\xd8\xb3";
            // خارجي_سي++ - extern "C++"
            const char *const KHARIJI_CPP = "\xd8\xae\xd8\xa7\xd8\xb1\xd8\xac\xd9\x8a_\xd8\xb3\xd9\x8a++";

            // --- (AR) اتفاقيات الاستدعاء / (EN) Calling conventions ---
            // استدعاء_قياسي - cdecl
            const char *const ISTIDAA_QIYASI = "\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xaf\xd8\xb9\xd8\xa7\xd8\xa1_\xd9\x82\xd9\x8a\xd8\xa7\xd8\xb3\xd9\x8a";
            // استدعاء_ويندوز - stdcall
            const char *const ISTIDAA_WINDOWS = "\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xaf\xd8\xb9\xd8\xa7\xd8\xa1_\xd9\x88\xd9\x8a\xd9\x86\xd8\xaf\xd9\x88\xd8\xb2";
            // استدعاء_سريع - fastcall
            const char *const ISTIDAA_SARI = "\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xaf\xd8\xb9\xd8\xa7\xd8\xa1_\xd8\xb3\xd8\xb1\xd9\x8a\xd8\xb9";
            // استدعاء_متوافق - thiscall
            const char *const ISTIDAA_MUTAWAQIR = "\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xaf\xd8\xb9\xd8\xa7\xd8\xa1_\xd9\x85\xd8\xaa\xd9\x88\xd8\xa7\xd9\x81\xd9\x82";

            // --- (AR) تعليقات الملكية / (EN) Ownership annotations ---
            // #[ملك_النتيجة] - owns result
            const char *const MALIK_NATIJA = "#[\xd9\x85\xd9\x84\xd9\x83_\xd8\xa7\xd9\x84\xd9\x86\xd8\xaa\xd9\x8a\xd8\xac\xd8\xa9]";
            // #[يُحرر_بـ:] - freed by
            const char *const YUHARRIR_BI = "#[\xd9\x8a\xd9\x8f\xd8\xad\xd8\xb1\xd8\xb1_\xd8\xa8\xd9\x80:]";
            // #[لا_يُحرر] - no free
            const char *const LA_YUHARRIR = "#[\xd9\x84\xd8\xa7_\xd9\x8a\xd9\x8f\xd8\xad\xd8\xb1\xd8\xb1]";
            // #[مستعار] - borrowed
            const char *const MUSTAAR = "#[\xd9\x85\xd8\xb3\xd8\xaa\xd8\xb9\xd8\xa7\xd8\xb1]";

            // --- (AR) أنواع C الأساسية / (EN) Basic C types ---
            // فراغ_س - void
            const char *const C_VOID = "\xd9\x81\xd8\xb1\xd8\xa7\xd8\xba_\xd8\xb3";
            // حرف_س - char
            const char *const C_CHAR = "\xd8\xad\xd8\xb1\xd9\x81_\xd8\xb3";
            // قصير_س - short
            const char *const C_SHORT = "\xd9\x82\xd8\xb5\xd9\x8a\xd8\xb1_\xd8\xb3";
            // صحيح_س - int
            const char *const C_INT = "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad_\xd8\xb3";
            // طويل_س - long
            const char *const C_LONG = "\xd8\xb7\xd9\x88\xd9\x8a\xd9\x84_\xd8\xb3";
            // طويل_جداً_س - long long
            const char *const C_LONGLONG = "\xd8\xb7\xd9\x88\xd9\x8a\xd9\x84_\xd8\xac\xd8\xaf\xd8\xa7\xd9\x8b_\xd8\xb3";
            // عشري_س - float
            const char *const C_FLOAT = "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a_\xd8\xb3";
            // مزدوج_س - double
            const char *const C_DOUBLE = "\xd9\x85\xd8\xb2\xd8\xaf\xd9\x88\xd8\xac_\xd8\xb3";
            // حجم_س - size_t
            const char *const C_SIZE_T = "\xd8\xad\xd8\xac\xd9\x85_\xd8\xb3";
            // فرق_مؤشر_س - ptrdiff_t
            const char *const C_PTRDIFF_T = "\xd9\x81\xd8\xb1\xd9\x82_\xd9\x85\xd8\xa4\xd8\xb4\xd8\xb1_\xd8\xb3";

            // --- (AR) مؤهلات الأنواع / (EN) Type qualifiers ---
            // بدون_إشارة - unsigned
            const char *const BIDOON_ISHARA = "\xd8\xa8\xd8\xaf\xd9\x88\xd9\x86_\xd8\xa5\xd8\xb4\xd8\xa7\xd8\xb1\xd8\xa9";
            // بإشارة - signed
            const char *const BI_ISHARA = "\xd8\xa8\xd8\xa5\xd8\xb4\xd8\xa7\xd8\xb1\xd8\xa9";
            // ثابت - const
            const char *const THABIT = "\xd8\xab\xd8\xa7\xd8\xa8\xd8\xaa";
            // متقلب - volatile
            const char *const MUTAQQALIB = "\xd9\x85\xd8\xaa\xd9\x82\xd9\x84\xd8\xa8";
        }

        // ============================================================================
        //                    (AR) تنفيذ CBasicTypeImpl
        //                    (EN) CBasicTypeImpl Implementation
        // ============================================================================

        /**
         * (AR) بناء نوع C أساسي
         * (EN) Construct a basic C type
         */
        CBasicTypeImpl::CBasicTypeImpl(CBasicType type)
            : basicType_(type)
        {
        }

        /**
         * (AR) إرجاع الاسم العربي للنوع الأساسي
         * (EN) Returns Arabic name for basic type
         */
        std::string CBasicTypeImpl::getArabicName() const
        {
            switch (basicType_)
            {
            // --- (AR) أنواع فارغة / (EN) Void types ---
            case CBasicType::VOID:
                return "\xd9\x81\xd8\xb1\xd8\xa7\xd8\xba"; // فراغ

            // --- (AR) أنواع صحيحة / (EN) Integer types ---
            case CBasicType::CHAR:
                return "\xd8\xad\xd8\xb1\xd9\x81"; // حرف
            case CBasicType::SCHAR:
                return "\xd8\xad\xd8\xb1\xd9\x81_\xd9\x85\xd9\x88\xd9\x82\xd9\x91\xd8\xb9"; // حرف_موقّع
            case CBasicType::UCHAR:
                return "\xd8\xad\xd8\xb1\xd9\x81_\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd9\x88\xd9\x82\xd9\x91\xd8\xb9"; // حرف_غير_موقّع
            case CBasicType::SHORT:
                return "\xd9\x82\xd8\xb5\xd9\x8a\xd8\xb1"; // قصير
            case CBasicType::USHORT:
                return "\xd9\x82\xd8\xb5\xd9\x8a\xd8\xb1_\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd9\x88\xd9\x82\xd9\x91\xd8\xb9"; // قصير_غير_موقّع
            case CBasicType::INT:
                return "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad"; // صحيح
            case CBasicType::UINT:
                return "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad_\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd9\x88\xd9\x82\xd9\x91\xd8\xb9"; // صحيح_غير_موقّع
            case CBasicType::LONG:
                return "\xd8\xb7\xd9\x88\xd9\x8a\xd9\x84"; // طويل
            case CBasicType::ULONG:
                return "\xd8\xb7\xd9\x88\xd9\x8a\xd9\x84_\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd9\x88\xd9\x82\xd9\x91\xd8\xb9"; // طويل_غير_موقّع
            case CBasicType::LONGLONG:
                return "\xd8\xb7\xd9\x88\xd9\x8a\xd9\x84_\xd8\xac\xd8\xaf\xd8\xa7\xd9\x8b"; // طويل_جداً
            case CBasicType::ULONGLONG:
                return "\xd8\xb7\xd9\x88\xd9\x8a\xd9\x84_\xd8\xac\xd8\xaf\xd8\xa7\xd9\x8b_\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd9\x88\xd9\x82\xd9\x91\xd8\xb9"; // طويل_جداً_غير_موقّع

            // --- (AR) أنواع عشرية / (EN) Floating point types ---
            case CBasicType::FLOAT:
                return "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a"; // عشري
            case CBasicType::DOUBLE:
                return "\xd9\x85\xd8\xb2\xd8\xaf\xd9\x88\xd8\xac"; // مزدوج
            case CBasicType::LONGDOUBLE:
                return "\xd9\x85\xd8\xb2\xd8\xaf\xd9\x88\xd8\xac_\xd8\xb7\xd9\x88\xd9\x8a\xd9\x84"; // مزدوج_طويل

            // --- (AR) أنواع منطقية / (EN) Boolean types ---
            case CBasicType::BOOL:
                return "\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a"; // منطقي

            // --- (AR) أنواع الحجم / (EN) Size types ---
            case CBasicType::SIZE_T:
                return "\xd8\xad\xd8\xac\xd9\x85"; // حجم
            case CBasicType::SSIZE_T:
                return "\xd8\xad\xd8\xac\xd9\x85_\xd9\x85\xd9\x88\xd9\x82\xd9\x91\xd8\xb9"; // حجم_موقّع
            case CBasicType::PTRDIFF_T:
                return "\xd9\x81\xd8\xb1\xd9\x82_\xd9\x85\xd8\xa4\xd8\xb4\xd8\xb1"; // فرق_مؤشر
            case CBasicType::INTPTR_T:
                return "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad_\xd9\x85\xd8\xa4\xd8\xb4\xd8\xb1"; // صحيح_مؤشر
            case CBasicType::UINTPTR_T:
                return "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad_\xd9\x85\xd8\xa4\xd8\xb4\xd8\xb1_\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd9\x88\xd9\x82\xd9\x91\xd8\xb9"; // صحيح_مؤشر_غير_موقّع

            // --- (AR) أنواع ثابتة الحجم / (EN) Fixed-size types ---
            case CBasicType::INT8:
                return "\xd8\xb5"
                       "8"; // ص8
            case CBasicType::UINT8:
                return "\xd8\xb5_\xd8\xba"
                       "8"; // ص_غ8
            case CBasicType::INT16:
                return "\xd8\xb5"
                       "16"; // ص16
            case CBasicType::UINT16:
                return "\xd8\xb5_\xd8\xba"
                       "16"; // ص_غ16
            case CBasicType::INT32:
                return "\xd8\xb5"
                       "32"; // ص32
            case CBasicType::UINT32:
                return "\xd8\xb5_\xd8\xba"
                       "32"; // ص_غ32
            case CBasicType::INT64:
                return "\xd8\xb5"
                       "64"; // ص64
            case CBasicType::UINT64:
                return "\xd8\xb5_\xd8\xba"
                       "64"; // ص_غ64

            // --- (AR) أنواع خاصة / (EN) Special types ---
            case CBasicType::WCHAR_T:
                return "\xd8\xad\xd8\xb1\xd9\x81_\xd8\xb9\xd8\xb1\xd9\x8a\xd8\xb6"; // حرف_عريض
            case CBasicType::CHAR16_T:
                return "\xd8\xad\xd8\xb1\xd9\x81"
                       "16"; // حرف16
            case CBasicType::CHAR32_T:
                return "\xd8\xad\xd8\xb1\xd9\x81"
                       "32"; // حرف32

            default:
                return "\xd9\x85\xd8\xac\xd9\x87\xd9\x88\xd9\x84"; // مجهول
            }
        }

        /**
         * (AR) إرجاع اسم النوع بصيغة C
         * (EN) Returns C-style type name
         */
        std::string CBasicTypeImpl::getCName() const
        {
            switch (basicType_)
            {
            case CBasicType::VOID:
                return "void";
            case CBasicType::CHAR:
                return "char";
            case CBasicType::SCHAR:
                return "signed char";
            case CBasicType::UCHAR:
                return "unsigned char";
            case CBasicType::SHORT:
                return "short";
            case CBasicType::USHORT:
                return "unsigned short";
            case CBasicType::INT:
                return "int";
            case CBasicType::UINT:
                return "unsigned int";
            case CBasicType::LONG:
                return "long";
            case CBasicType::ULONG:
                return "unsigned long";
            case CBasicType::LONGLONG:
                return "long long";
            case CBasicType::ULONGLONG:
                return "unsigned long long";
            case CBasicType::FLOAT:
                return "float";
            case CBasicType::DOUBLE:
                return "double";
            case CBasicType::LONGDOUBLE:
                return "long double";
            case CBasicType::BOOL:
                return "_Bool";
            case CBasicType::SIZE_T:
                return "size_t";
            case CBasicType::SSIZE_T:
                return "ssize_t";
            case CBasicType::PTRDIFF_T:
                return "ptrdiff_t";
            case CBasicType::INTPTR_T:
                return "intptr_t";
            case CBasicType::UINTPTR_T:
                return "uintptr_t";
            case CBasicType::INT8:
                return "int8_t";
            case CBasicType::UINT8:
                return "uint8_t";
            case CBasicType::INT16:
                return "int16_t";
            case CBasicType::UINT16:
                return "uint16_t";
            case CBasicType::INT32:
                return "int32_t";
            case CBasicType::UINT32:
                return "uint32_t";
            case CBasicType::INT64:
                return "int64_t";
            case CBasicType::UINT64:
                return "uint64_t";
            case CBasicType::WCHAR_T:
                return "wchar_t";
            case CBasicType::CHAR16_T:
                return "char16_t";
            case CBasicType::CHAR32_T:
                return "char32_t";
            default:
                return "unknown";
            }
        }

        /**
         * (AR) إرجاع حجم النوع بالبايت
         * (EN) Returns type size in bytes
         */
        size_t CBasicTypeImpl::getSize(bool is64Bit) const
        {
            switch (basicType_)
            {
            case CBasicType::VOID:
                return 0;
            case CBasicType::CHAR:
            case CBasicType::SCHAR:
            case CBasicType::UCHAR:
            case CBasicType::INT8:
            case CBasicType::UINT8:
            case CBasicType::BOOL:
                return 1;

            case CBasicType::SHORT:
            case CBasicType::USHORT:
            case CBasicType::INT16:
            case CBasicType::UINT16:
            case CBasicType::CHAR16_T:
                return 2;

            case CBasicType::INT:
            case CBasicType::UINT:
            case CBasicType::INT32:
            case CBasicType::UINT32:
            case CBasicType::CHAR32_T:
            case CBasicType::FLOAT:
            case CBasicType::WCHAR_T:
                return 4;

            case CBasicType::LONG:
            case CBasicType::ULONG:
// (AR) على Windows: long دائماً 4 بايت
// (EN) On Windows: long is always 4 bytes
#ifdef _WIN32
                return 4;
#else
                return is64Bit ? 8 : 4;
#endif

            case CBasicType::LONGLONG:
            case CBasicType::ULONGLONG:
            case CBasicType::INT64:
            case CBasicType::UINT64:
            case CBasicType::DOUBLE:
                return 8;

            case CBasicType::SIZE_T:
            case CBasicType::SSIZE_T:
            case CBasicType::PTRDIFF_T:
            case CBasicType::INTPTR_T:
            case CBasicType::UINTPTR_T:
                return is64Bit ? 8 : 4;

            case CBasicType::LONGDOUBLE:
// (AR) حجم long double يختلف حسب المنصة والمترجم
// (EN) long double size varies by platform and compiler
#ifdef _WIN32
                return 8; // MSVC: same as double
#else
                return 16; // GCC/Clang: 80-bit extended
#endif

            default:
                return 0;
            }
        }

        /**
         * (AR) إرجاع محاذاة النوع بالبايت
         * (EN) Returns type alignment in bytes
         */
        size_t CBasicTypeImpl::getAlignment(bool is64Bit) const
        {
            // (AR) معظم الأنواع محاذاتها تساوي حجمها
            // (EN) Most types have alignment equal to their size
            return getSize(is64Bit);
        }

        /**
         * (AR) تحويل النوع الأساسي إلى نوع LLVM
         * (EN) Convert basic type to LLVM type
         */
        llvm::Type *CBasicTypeImpl::toLLVMType(llvm::LLVMContext &context) const
        {
            switch (basicType_)
            {
            case CBasicType::VOID:
                return llvm::Type::getVoidTy(context);

            case CBasicType::CHAR:
            case CBasicType::SCHAR:
            case CBasicType::UCHAR:
            case CBasicType::INT8:
            case CBasicType::UINT8:
            case CBasicType::BOOL:
                return llvm::Type::getInt8Ty(context);

            case CBasicType::SHORT:
            case CBasicType::USHORT:
            case CBasicType::INT16:
            case CBasicType::UINT16:
            case CBasicType::CHAR16_T:
                return llvm::Type::getInt16Ty(context);

            case CBasicType::INT:
            case CBasicType::UINT:
            case CBasicType::INT32:
            case CBasicType::UINT32:
            case CBasicType::CHAR32_T:
            case CBasicType::WCHAR_T:
                return llvm::Type::getInt32Ty(context);

            case CBasicType::LONG:
            case CBasicType::ULONG:
#ifdef _WIN32
                return llvm::Type::getInt32Ty(context);
#else
                return llvm::Type::getInt64Ty(context);
#endif

            case CBasicType::LONGLONG:
            case CBasicType::ULONGLONG:
            case CBasicType::INT64:
            case CBasicType::UINT64:
                return llvm::Type::getInt64Ty(context);

            case CBasicType::SIZE_T:
            case CBasicType::SSIZE_T:
            case CBasicType::PTRDIFF_T:
            case CBasicType::INTPTR_T:
            case CBasicType::UINTPTR_T:
                // (AR) يعتمد على حجم المؤشر
                // (EN) Depends on pointer size
                return llvm::Type::getInt64Ty(context); // Assuming 64-bit

            case CBasicType::FLOAT:
                return llvm::Type::getFloatTy(context);

            case CBasicType::DOUBLE:
                return llvm::Type::getDoubleTy(context);

            case CBasicType::LONGDOUBLE:
#ifdef _WIN32
                return llvm::Type::getDoubleTy(context);
#else
                return llvm::Type::getFP128Ty(context);
#endif

            default:
                return llvm::Type::getVoidTy(context);
            }
        }

        /**
         * (AR) هل النوع صحيح (integer)؟
         * (EN) Is this an integer type?
         */
        bool CBasicTypeImpl::isInteger() const
        {
            switch (basicType_)
            {
            case CBasicType::CHAR:
            case CBasicType::SCHAR:
            case CBasicType::UCHAR:
            case CBasicType::SHORT:
            case CBasicType::USHORT:
            case CBasicType::INT:
            case CBasicType::UINT:
            case CBasicType::LONG:
            case CBasicType::ULONG:
            case CBasicType::LONGLONG:
            case CBasicType::ULONGLONG:
            case CBasicType::INT8:
            case CBasicType::UINT8:
            case CBasicType::INT16:
            case CBasicType::UINT16:
            case CBasicType::INT32:
            case CBasicType::UINT32:
            case CBasicType::INT64:
            case CBasicType::UINT64:
            case CBasicType::BOOL:
            case CBasicType::SIZE_T:
            case CBasicType::SSIZE_T:
            case CBasicType::PTRDIFF_T:
            case CBasicType::INTPTR_T:
            case CBasicType::UINTPTR_T:
            case CBasicType::WCHAR_T:
            case CBasicType::CHAR16_T:
            case CBasicType::CHAR32_T:
                return true;
            default:
                return false;
            }
        }

        /**
         * (AR) هل النوع عشري (floating point)؟
         * (EN) Is this a floating point type?
         */
        bool CBasicTypeImpl::isFloatingPoint() const
        {
            return basicType_ == CBasicType::FLOAT ||
                   basicType_ == CBasicType::DOUBLE ||
                   basicType_ == CBasicType::LONGDOUBLE;
        }

        /**
         * (AR) هل النوع موقّع (signed)؟
         * (EN) Is this a signed type?
         */
        bool CBasicTypeImpl::isSigned() const
        {
            switch (basicType_)
            {
            case CBasicType::CHAR: // (AR) يعتمد على المنصة، نفترض signed
            case CBasicType::SCHAR:
            case CBasicType::SHORT:
            case CBasicType::INT:
            case CBasicType::LONG:
            case CBasicType::LONGLONG:
            case CBasicType::INT8:
            case CBasicType::INT16:
            case CBasicType::INT32:
            case CBasicType::INT64:
            case CBasicType::SSIZE_T:
            case CBasicType::PTRDIFF_T:
            case CBasicType::INTPTR_T:
            case CBasicType::FLOAT:
            case CBasicType::DOUBLE:
            case CBasicType::LONGDOUBLE:
                return true;
            default:
                return false;
            }
        }

        // ============================================================================
        //                    (AR) تنفيذ CPointerType
        //                    (EN) CPointerType Implementation
        // ============================================================================

        CPointerType::CPointerType(CTypePtr pointeeType)
            : pointeeType_(pointeeType)
        {
        }

        std::string CPointerType::getArabicName() const
        {
            std::string baseName = pointeeType_->getArabicName();
            // (AR) مؤشر لـ X = *X
            // (EN) Pointer to X = *X
            return "\xd9\x85\xd8\xa4\xd8\xb4\xd8\xb1<" + baseName + ">"; // مؤشر<X>
        }

        std::string CPointerType::getCName() const
        {
            std::string baseName = pointeeType_->getCName();
            return baseName + "*";
        }

        size_t CPointerType::getSize(bool is64Bit) const
        {
            return is64Bit ? 8 : 4;
        }

        size_t CPointerType::getAlignment(bool is64Bit) const
        {
            return getSize(is64Bit);
        }

        llvm::Type *CPointerType::toLLVMType(llvm::LLVMContext &context) const
        {
            llvm::Type *pointeeType = pointeeType_->toLLVMType(context);
            return llvm::PointerType::get(pointeeType, 0);
        }

        bool CPointerType::isFunctionPointer() const
        {
            return pointeeType_->isFunction();
        }

        bool CPointerType::isVoidPointer() const
        {
            if (auto *basic = dynamic_cast<const CBasicTypeImpl *>(pointeeType_.get()))
            {
                return basic->getBasicType() == CBasicType::VOID;
            }
            return false;
        }

        // ============================================================================
        //                    (AR) تنفيذ CArrayType
        //                    (EN) CArrayType Implementation
        // ============================================================================

        CArrayType::CArrayType(CTypePtr elementType, size_t size)
            : elementType_(elementType), arraySize_(size)
        {
        }

        std::string CArrayType::getArabicName() const
        {
            std::string baseName = elementType_->getArabicName();
            if (arraySize_ > 0)
            {
                return "\xd9\x85\xd8\xb5\xd9\x81\xd9\x88\xd9\x81\xd8\xa9<" + baseName + ", " + std::to_string(arraySize_) + ">"; // مصفوفة<X, N>
            }
            return "\xd9\x85\xd8\xb5\xd9\x81\xd9\x88\xd9\x81\xd8\xa9<" + baseName + ">"; // مصفوفة<X>
        }

        std::string CArrayType::getCName() const
        {
            std::string baseName = elementType_->getCName();
            if (arraySize_ > 0)
            {
                return baseName + "[" + std::to_string(arraySize_) + "]";
            }
            return baseName + "[]";
        }

        size_t CArrayType::getSize(bool is64Bit) const
        {
            if (arraySize_ == 0)
                return 0;
            return elementType_->getSize(is64Bit) * arraySize_;
        }

        size_t CArrayType::getAlignment(bool is64Bit) const
        {
            return elementType_->getAlignment(is64Bit);
        }

        llvm::Type *CArrayType::toLLVMType(llvm::LLVMContext &context) const
        {
            llvm::Type *elemType = elementType_->toLLVMType(context);
            if (arraySize_ > 0)
            {
                return llvm::ArrayType::get(elemType, arraySize_);
            }
            // (AR) مصفوفة غير محددة الحجم = مؤشر
            // (EN) Unsized array = pointer
            return llvm::PointerType::get(elemType, 0);
        }

        // ============================================================================
        //                    (AR) تنفيذ CFunctionType
        //                    (EN) CFunctionType Implementation
        // ============================================================================

        CFunctionType::CFunctionType(CTypePtr returnType, CallingConvention convention)
            : returnType_(returnType), callingConvention_(convention)
        {
        }

        void CFunctionType::addParameter(const std::string &name, CTypePtr type, bool isConst)
        {
            parameters_.push_back({name, type, isConst});
        }

        std::string CFunctionType::getArabicName() const
        {
            std::stringstream ss;
            ss << "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9("; // دالة(

            bool first = true;
            for (const auto &param : parameters_)
            {
                if (!first)
                    ss << ", ";
                ss << param.type->getArabicName();
                first = false;
            }

            if (isVariadic_)
            {
                if (!parameters_.empty())
                    ss << ", ";
                ss << "...";
            }

            ss << ") -> " << returnType_->getArabicName();
            return ss.str();
        }

        std::string CFunctionType::getCName() const
        {
            std::stringstream ss;
            ss << returnType_->getCName() << " (*)(";

            bool first = true;
            for (const auto &param : parameters_)
            {
                if (!first)
                    ss << ", ";
                if (param.isConst)
                    ss << "const ";
                ss << param.type->getCName();
                first = false;
            }

            if (isVariadic_)
            {
                if (!parameters_.empty())
                    ss << ", ";
                ss << "...";
            }

            ss << ")";
            return ss.str();
        }

        llvm::Type *CFunctionType::toLLVMType(llvm::LLVMContext &context) const
        {
            llvm::Type *retType = returnType_->toLLVMType(context);

            std::vector<llvm::Type *> paramTypes;
            for (const auto &param : parameters_)
            {
                paramTypes.push_back(param.type->toLLVMType(context));
            }

            return llvm::FunctionType::get(retType, paramTypes, isVariadic_);
        }

        // ============================================================================
        //                    (AR) تنفيذ ExternFunctionDecl
        //                    (EN) ExternFunctionDecl Implementation
        // ============================================================================

        void ExternFunctionDecl::addParameter(
            const std::string &originalName,
            const std::string &arabicName,
            CTypePtr type,
            OwnershipAnnotation ownership,
            const std::string &freeFunc)
        {
            // (AR) إنشاء معلومات المعامل — حقل 'name' يُعيّن كنسخة من الاسم الأصلي
            // (EN) Create parameter info — 'name' field is set as copy of originalName
            parameters_.push_back({originalName, arabicName, originalName, type, ownership, freeFunc});
        }

        void ExternFunctionDecl::setResultOwnership(OwnershipAnnotation ownership, const std::string &freeFunc)
        {
            resultOwnership_ = ownership;
            resultFreeFunction_ = freeFunc;
        }

        bool ExternFunctionDecl::validate(std::string &error) const
        {
            // (AR) التحقق من وجود الاسم الأصلي
            // (EN) Verify original name exists
            if (originalName_.empty())
            {
                error = "\xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd9\x85 \xd8\xa7\xd9\x84\xd8\xa3\xd8\xb5\xd9\x84\xd9\x8a \xd9\x84\xd9\x84\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 \xd9\x85\xd8\xb7\xd9\x84\xd9\x88\xd8\xa8"; // الاسم الأصلي للدالة مطلوب
                return false;
            }

            // (AR) التحقق من وجود نوع القيمة المُرجعة
            // (EN) Verify return type exists
            if (!returnType_)
            {
                error = "\xd9\x86\xd9\x88\xd8\xb9 \xd8\xa7\xd9\x84\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9 \xd8\xa7\xd9\x84\xd9\x85\xd9\x8f\xd8\xb1\xd8\xac\xd8\xb9\xd8\xa9 \xd9\x85\xd8\xb7\xd9\x84\xd9\x88\xd8\xa8"; // نوع القيمة المُرجعة مطلوب
                return false;
            }

            // (AR) التحقق من أنواع المعاملات
            // (EN) Verify parameter types
            for (size_t i = 0; i < parameters_.size(); ++i)
            {
                if (!parameters_[i].type)
                {
                    error = "\xd9\x86\xd9\x88\xd8\xb9 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb9\xd8\xa7\xd9\x85\xd9\x84 " + std::to_string(i + 1) + " \xd9\x85\xd8\xb7\xd9\x84\xd9\x88\xd8\xa8"; // نوع المعامل X مطلوب
                    return false;
                }
            }

            // (AR) التحقق من تعليقات الملكية
            // (EN) Verify ownership annotations
            if (resultOwnership_ == OwnershipAnnotation::OWNS_RESULT && resultFreeFunction_.empty())
            {
                // (AR) يجب تحديد دالة التحرير إذا كانت النتيجة مملوكة
                // (EN) Free function must be specified if result is owned
                // (AR) تحذير: لم تُحدد دالة التحرير للنتيجة المملوكة
                // (EN) Warning: No free function specified for owned result
                // (AR) هذا تحذير وليس خطأ
            }

            return true;
        }

        // ============================================================================
        //                    (AR) دوال مساعدة لإنشاء الأنواع
        //                    (EN) Type creation helper functions
        // ============================================================================

        CTypePtr makeBasicType(CBasicType type)
        {
            return std::make_shared<CBasicTypeImpl>(type);
        }

        CTypePtr makePointerType(CTypePtr pointeeType)
        {
            return std::make_shared<CPointerType>(pointeeType);
        }

        CTypePtr makeArrayType(CTypePtr elementType, size_t size)
        {
            return std::make_shared<CArrayType>(elementType, size);
        }

        CTypePtr makeFunctionType(CTypePtr returnType, CallingConvention conv)
        {
            return std::make_shared<CFunctionType>(returnType, conv);
        }

    } // namespace FFI
} // namespace Sad
