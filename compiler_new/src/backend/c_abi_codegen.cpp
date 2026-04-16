// تعطيل تحذير Unicode للنصوص العربية
#ifdef _MSC_VER
#pragma warning(disable : 4819)
#endif

/**
 * @file c_abi_codegen.cpp
 * @brief (AR) تنفيذ مولد كود C ABI
 *        (EN) C ABI code generator implementation
 *
 * @details
 * (AR) هذا الملف يحتوي على تنفيذ CABICodeGenerator فقط،
 *      وتم فصله لتقليل حجم الملف الأصلي c_abi.cpp وتحسين قابلية الصيانة.
 *
 * (EN) This file contains CABICodeGenerator implementation only,
 *      extracted from c_abi.cpp to reduce file size and improve maintainability.
 */

#include "ffi/c_abi.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/Attributes.h>

#include <sstream>
#include <stdexcept>

namespace Sad
{
    namespace FFI
    {

        CABICodeGenerator::CABICodeGenerator(llvm::LLVMContext &context, llvm::Module &module)
            : context_(context), module_(module)
        {
            // (AR) تحديد المنصة تلقائياً
            // (EN) Auto-detect platform
#ifdef _WIN32
            isWindows_ = true;
#else
            isWindows_ = false;
#endif

            // (AR) افتراض 64-بت
            // (EN) Assume 64-bit
            is64Bit_ = true;
        }

        unsigned CABICodeGenerator::toLLVMCallingConv(CallingConvention conv) const
        {
            switch (conv)
            {
            case CallingConvention::CDECL:
                return llvm::CallingConv::C;
            case CallingConvention::STDCALL:
                return llvm::CallingConv::X86_StdCall;
            case CallingConvention::FASTCALL:
                return llvm::CallingConv::X86_FastCall;
            case CallingConvention::THISCALL:
                return llvm::CallingConv::X86_ThisCall;
            case CallingConvention::WIN64:
                return llvm::CallingConv::Win64;
            case CallingConvention::SYSV_AMD64:
                return llvm::CallingConv::X86_64_SysV;
            case CallingConvention::AAPCS:
                return llvm::CallingConv::ARM_AAPCS;
            case CallingConvention::AAPCS64:
                return llvm::CallingConv::AArch64_VectorCall;
            case CallingConvention::AUTO:
                // (AR) تحديد تلقائي حسب المنصة
                // (EN) Auto-detect based on platform
                if (is64Bit_)
                {
                    return isWindows_ ? llvm::CallingConv::Win64 : llvm::CallingConv::X86_64_SysV;
                }
                return llvm::CallingConv::C;
            case CallingConvention::CUSTOM:
            default:
                return llvm::CallingConv::C;
            }
        }

        llvm::FunctionType *CABICodeGenerator::createFunctionType(const ExternFunctionDecl &decl) const
        {
            llvm::Type *retType = decl.getReturnType()->toLLVMType(context_);

            std::vector<llvm::Type *> paramTypes;
            for (const auto &param : decl.getParameters())
            {
                paramTypes.push_back(param.type->toLLVMType(context_));
            }

            return llvm::FunctionType::get(retType, paramTypes, decl.isVariadic());
        }

        llvm::Function *CABICodeGenerator::declareExternFunction(const ExternFunctionDecl &decl)
        {
            // (AR) التحقق من صحة التصريح
            // (EN) Validate declaration
            std::string error;
            if (!decl.validate(error))
            {
                throw std::runtime_error(error);
            }

            // (AR) إنشاء نوع الدالة
            // (EN) Create function type
            llvm::FunctionType *funcType = createFunctionType(decl);

            // (AR) البحث عن دالة موجودة بنفس الاسم
            // (EN) Look for existing function with same name
            llvm::Function *func = module_.getFunction(decl.getOriginalName());
            if (func)
            {
                // (AR) التحقق من تطابق النوع
                // (EN) Verify type match
                if (func->getFunctionType() != funcType)
                {
                    throw std::runtime_error(
                        "\xd8\xaa\xd8\xb9\xd8\xa7\xd8\xb1\xd8\xb6 \xd9\x81\xd9\x8a \xd8\xaa\xd8\xb5\xd8\xb1\xd9\x8a\xd8\xad \xd8\xa7\xd9\x84\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9: " + decl.getOriginalName() // تعارض في تصريح الدالة:
                    );
                }
                return func;
            }

            // (AR) إنشاء دالة جديدة
            // (EN) Create new function
            func = llvm::Function::Create(
                funcType,
                llvm::Function::ExternalLinkage,
                decl.getOriginalName(),
                module_);

            // (AR) تعيين اتفاقية الاستدعاء
            // (EN) Set calling convention
            func->setCallingConv(toLLVMCallingConv(decl.getCallingConvention()));

            // (AR) تعيين أسماء المعاملات
            // (EN) Set parameter names
            const auto &params = decl.getParameters();
            unsigned idx = 0;
            for (auto &arg : func->args())
            {
                if (idx < params.size())
                {
                    arg.setName(params[idx].originalName);
                }
                ++idx;
            }

            return func;
        }

        llvm::Value *CABICodeGenerator::generateCall(
            const ExternFunctionDecl &decl,
            const std::vector<llvm::Value *> &args,
            llvm::IRBuilder<> &builder)
        {
            // (AR) الحصول على الدالة أو التصريح عنها
            // (EN) Get function or declare it
            llvm::Function *func = module_.getFunction(decl.getOriginalName());
            if (!func)
            {
                func = declareExternFunction(decl);
            }

            // (AR) التحقق من عدد المعاملات
            // (EN) Verify argument count
            const auto &params = decl.getParameters();
            if (!decl.isVariadic() && args.size() != params.size())
            {
                throw std::runtime_error(
                    "\xd8\xb9\xd8\xaf\xd8\xaf \xd8\xa7\xd9\x84\xd9\x85\xd8\xb9\xd8\xa7\xd9\x85\xd9\x84\xd8\xa7\xd8\xaa \xd8\xba\xd9\x8a\xd8\xb1 \xd8\xb5\xd8\xad\xd9\x8a\xd8\xad \xd9\x84\xd9\x84\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9: " + decl.getOriginalName() // عدد المعاملات غير صحيح للدالة:
                );
            }

            // (AR) توليد استدعاء الدالة
            // (EN) Generate function call
            llvm::CallInst *call = builder.CreateCall(func, args);

            // (AR) تعيين اتفاقية الاستدعاء
            // (EN) Set calling convention
            call->setCallingConv(toLLVMCallingConv(decl.getCallingConvention()));

            return call;
        }

        llvm::Value *CABICodeGenerator::convertSadToC(
            llvm::Value *sadValue,
            CTypePtr cType,
            llvm::IRBuilder<> &builder)
        {
            // (AR) تحويل القيم بين أنواع "ص" وأنواع C
            // (EN) Convert values between Sad types and C types

            llvm::Type *targetType = cType->toLLVMType(context_);
            llvm::Type *sourceType = sadValue->getType();

            // (AR) إذا كانت الأنواع متطابقة، لا حاجة للتحويل
            // (EN) If types match, no conversion needed
            if (sourceType == targetType)
            {
                return sadValue;
            }

            // (AR) تحويل بين أنواع صحيحة
            // (EN) Integer type conversion
            if (sourceType->isIntegerTy() && targetType->isIntegerTy())
            {
                unsigned srcBits = sourceType->getIntegerBitWidth();
                unsigned dstBits = targetType->getIntegerBitWidth();

                if (srcBits < dstBits)
                {
                    // (AR) توسيع النوع
                    // (EN) Type extension
                    auto *basicType = dynamic_cast<const CBasicTypeImpl *>(cType.get());
                    if (basicType && basicType->isSigned())
                    {
                        return builder.CreateSExt(sadValue, targetType, "sext");
                    }
                    return builder.CreateZExt(sadValue, targetType, "zext");
                }
                else if (srcBits > dstBits)
                {
                    // (AR) اقتطاع النوع
                    // (EN) Type truncation
                    return builder.CreateTrunc(sadValue, targetType, "trunc");
                }
            }

            // (AR) تحويل بين أنواع عشرية
            // (EN) Floating point type conversion
            if (sourceType->isFloatingPointTy() && targetType->isFloatingPointTy())
            {
                if (sourceType->isFloatTy() && targetType->isDoubleTy())
                {
                    return builder.CreateFPExt(sadValue, targetType, "fpext");
                }
                else if (sourceType->isDoubleTy() && targetType->isFloatTy())
                {
                    return builder.CreateFPTrunc(sadValue, targetType, "fptrunc");
                }
            }

            // (AR) تحويل مؤشرات
            // (EN) Pointer conversion
            if (sourceType->isPointerTy() && targetType->isPointerTy())
            {
                return builder.CreateBitCast(sadValue, targetType, "ptrcast");
            }

            // (AR) تحويل عام (bitcast)
            // (EN) General conversion (bitcast)
            return builder.CreateBitCast(sadValue, targetType, "bitcast");
        }

        llvm::Value *CABICodeGenerator::convertCToSad(
            llvm::Value *cValue,
            CTypePtr cType,
            llvm::IRBuilder<> &builder)
        {
            // (AR) التحويل العكسي من C إلى "ص"
            // (EN) Reverse conversion from C to Sad

            // (AR) في معظم الحالات، الأنواع متوافقة
            // (EN) In most cases, types are compatible
            return cValue;
        }

        void CABICodeGenerator::generateResultCleanup(
            const ExternFunctionDecl &decl,
            llvm::Value *result,
            llvm::IRBuilder<> &builder)
        {
            // (AR) إذا كانت النتيجة مملوكة، يجب تحريرها لاحقاً
            // (EN) If result is owned, it must be freed later

            if (decl.getResultOwnership() != ExternFunctionDecl::OwnershipAnnotation::OWNS_RESULT)
            {
                return;
            }

            // (AR) هذا placeholder - سيتم التكامل مع نظام الملكية
            // (EN) This is a placeholder - will integrate with ownership system

            // (AR) لا نولد كود التحرير هنا مباشرة
            //      نظام الملكية سيتولى ذلك عند نهاية النطاق
            // (EN) We don't generate cleanup code here directly
            //      Ownership system will handle this at scope end
        }

    } // namespace FFI
} // namespace Sad
