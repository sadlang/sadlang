/*
 * ============================================================================
 * مولد كود LLVM IR - ملف التنفيذ
 * LLVM IR Code Generator - Implementation File
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "llvm_optimizer.h"
#include "llvm_volatile_ops.h"
#include <llvm/Support/TargetSelect.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/InlineAsm.h>
#include <iostream>
#include <fstream>
#ifdef _WIN32
#include <filesystem>
#include <windows.h>
#else
#include <cstdio>
#endif

using namespace Sad::Compiler::SIR;

namespace Sad
{
    namespace LLVM
    {
// ============================================================================
// (AR) عمليات النصوص: emitStringConcat, emitStringCharAt, emitStringCmp,
//      emitInlineAsm, ensureArrayToStringHelper
// (EN) String operations: concat, charAt, compare, inline asm, array-to-string
// (AR) تم فصله من llvm_codegen_output.cpp وفق CW-05
// ============================================================================
        llvm::Value *LLVMCodeGen::emitStringConcat(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                reportError("STRING_CONCAT requires 2 operands");
                return nullptr;
            }

            llvm::Value *left = resolveOperand(inst->operands[0]);
            llvm::Value *right = resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                reportError("STRING_CONCAT: failed to resolve operands");
                return nullptr;
            }

            // Convert integer/float to string if needed
            llvm::Type *leftTy = left->getType();
            llvm::Type *rightTy = right->getType();

            // (AR) توليد دالة __sad_array_to_string إذا كان أحد المعاملين مصفوفة
            // (EN) Generate __sad_array_to_string helper if one operand is an array
            if (inst->operands[0].dataType == SadTypeKind::Array ||
                inst->operands[1].dataType == SadTypeKind::Array)
            {
                ensureArrayToStringHelper();
            }

            // Ensure both operands are string pointers
            // If one is an integer, convert it to string using sprintf (or __sad_itoa in freestanding)
            auto ensureString = [&](llvm::Value *val, llvm::Type *ty, const SIROperand &op) -> llvm::Value *
            {
                // (AR) تحويل المصفوفة إلى نص: "[عنصر1، عنصر2، ...]"
                // (EN) Convert array to string representation
                if (op.dataType == SadTypeKind::Array)
                {
                    // (AR) إذا كانت القيمة i64 (مؤشر مُخزّن كعدد صحيح)، نحوّلها إلى مؤشر أولاً
                    // (EN) If value is i64 (pointer stored as integer), convert to pointer first
                    if (ty->isIntegerTy(64))
                    {
                        val = builder_->CreateIntToPtr(val,
                                                       llvm::PointerType::getUnqual(*context_), "arr.i2p");
                        ty = val->getType();
                    }
                    if (ty->isPointerTy())
                    {
                        // (AR) قراءة طول المصفوفة من هيكل SadArray {i64 length, i64 capacity, ptr data}
                        // (EN) Read array length from SadArray struct {i64 length, i64 capacity, ptr data}
                        llvm::StructType *arrTy = llvm::StructType::getTypeByName(*context_, "SadArray");
                        if (!arrTy)
                        {
                            arrTy = llvm::StructType::create(*context_, {
                                                                            llvm::Type::getInt64Ty(*context_),      // length
                                                                            llvm::Type::getInt64Ty(*context_),      // capacity
                                                                            llvm::PointerType::getUnqual(*context_) // data
                                                                        },
                                                             "SadArray");
                        }

                        auto i64Ty = llvm::Type::getInt64Ty(*context_);
                        auto ptrTy = llvm::PointerType::getUnqual(*context_);

                        // Load length
                        llvm::Value *lenGep = builder_->CreateStructGEP(arrTy, val, 0, "arr.str.len.gep");
                        llvm::Value *arrLen = builder_->CreateLoad(i64Ty, lenGep, "arr.str.len");

                        // Load data pointer
                        llvm::Value *dataGep = builder_->CreateStructGEP(arrTy, val, 2, "arr.str.data.gep");
                        llvm::Value *dataPtr = builder_->CreateLoad(ptrTy, dataGep, "arr.str.data");

                        // (AR) تخصيص مخزن كبير كافٍ: "[" + (كل عنصر حتى 32 حرف + ", ") * الطول + "]" + '\0'
                        // (EN) Allocate sufficient buffer: "[" + (each elem up to 32 chars + ", ") * len + "]" + '\0'
                        llvm::Value *bufLen = builder_->CreateAdd(
                            builder_->CreateMul(arrLen, llvm::ConstantInt::get(i64Ty, 34), "arr.str.elmsz"),
                            llvm::ConstantInt::get(i64Ty, 4), "arr.str.bufsz");

                        llvm::FunctionType *mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
                        llvm::FunctionCallee mallocFn = module_->getOrInsertFunction("malloc", mallocType);
                        llvm::Value *buf = builder_->CreateCall(mallocFn, {bufLen}, "arr.str.buf");

                        // (AR) استدعاء __sad_array_to_string_impl(buf, arrLen, dataPtr) -> i32 (عدد الأحرف المكتوبة)
                        // (EN) Call __sad_array_to_string_impl(buf, arrLen, dataPtr) -> i32 (chars written)
                        // We generate this inline: write "[", then loop over elements, write "]"

                        // For simplicity, use sprintf to format: "[%lld, %lld, ...]"
                        // Start with "["
                        llvm::FunctionType *sprintfType = llvm::FunctionType::get(
                            llvm::Type::getInt32Ty(*context_), {ptrTy, ptrTy}, true);
                        llvm::FunctionCallee sprintfFn = module_->getOrInsertFunction("sprintf", sprintfType);

                        // (AR) بناء النص التمثيلي عبر دالة مساعدة مُولَّدة في LLVM IR
                        // (EN) Build string representation via helper function generated in LLVM IR
                        llvm::FunctionType *helperType = llvm::FunctionType::get(ptrTy, {ptrTy, i64Ty, ptrTy}, false);
                        llvm::FunctionCallee helperFn = module_->getOrInsertFunction("__sad_array_to_string", helperType);
                        llvm::Value *result = builder_->CreateCall(helperFn, {buf, arrLen, dataPtr}, "arr.str.result");
                        return result;
                    } // end if (ty->isPointerTy())
                } // end if (op.dataType == SadTypeKind::Array)

                // (AR) معالجة لاشيء (null) — إذا كان النوع Pointer والقيمة null pointer
                //      نُرجع نص "void" كما يفعل المفسر
                //      بدون هذا الفحص، تمرير null pointer إلى strlen يسبب crash
                // (EN) Handle null (لاشيء) — if type is Pointer and value is null pointer
                //      return "void" string as the interpreter does
                //      Without this check, passing null pointer to strlen causes crash
                if (op.dataType == SadTypeKind::Pointer)
                {
                    // (AR) لاشيء/null → نص "void"
                    return builder_->CreateGlobalStringPtr("void", "null.str");
                }

                if (ty->isPointerTy())
                {
                    // (AR) إذا كان مؤشر null فعلي (ConstantPointerNull أو i64(0) مُحوّل)
                    //      نفحص وقت التشغيل ونعيد "void" إذا كان null
                    // (EN) If it's actually an LLVM null pointer, do a runtime check
                    //      and return "void" if null
                    llvm::Value *isNull = builder_->CreateICmpEQ(
                        val,
                        llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(val->getType())),
                        "ptr.isnull");
                    llvm::Value *voidStr = builder_->CreateGlobalStringPtr("void", "null.fallback");
                    return builder_->CreateSelect(isNull, voidStr, val, "ptr.safe");
                }
                // (AR) إذا كان نوع المعامل نصاً لكن القيمة i64 (مؤشر مُخزّن كعدد صحيح)
                //      نحوّل من i64 إلى مؤشر — يحدث مع النصوص المنسقة (f-strings) والمتغيرات النصية
                // (EN) If operand type is String but LLVM value is i64 (boxed string pointer),
                //      convert from i64 to pointer — happens with f-strings and string variables
                if (op.dataType == SadTypeKind::String && ty->isIntegerTy(64))
                {
                    return builder_->CreateIntToPtr(val,
                                                    llvm::PointerType::getUnqual(*context_), "str.unbox");
                }
                // (AR) القيم المنطقية: ارجع "صحيح"/"خطأ" مباشرة بدلاً من تحويل الرقم
                // (EN) Boolean values: return "صحيح"/"خطأ" directly instead of converting the number
                if (op.dataType == SadTypeKind::Boolean)
                {
                    llvm::Value *trueStr = builder_->CreateGlobalStringPtr("\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "bool.true");
                    llvm::Value *falseStr = builder_->CreateGlobalStringPtr("\xd8\xae\xd8\xb7\xd8\xa3", "bool.false");
                    llvm::Value *cond;
                    if (val->getType()->isIntegerTy(1))
                    {
                        cond = val;
                    }
                    else if (val->getType()->isPointerTy())
                    {
                        // (AR) [Fix BF-04] المؤشرات تُقارن بـ null وليس بـ 0
                        // (EN) [Fix BF-04] Pointers must be compared to null, not integer 0
                        cond = builder_->CreateICmpNE(
                            val,
                            llvm::ConstantPointerNull::get(
                                llvm::cast<llvm::PointerType>(val->getType())),
                            "bool.cmp.ptr");
                    }
                    else
                    {
                        cond = builder_->CreateICmpNE(val, llvm::ConstantInt::get(val->getType(), 0), "bool.cmp");
                    }
                    return builder_->CreateSelect(cond, trueStr, falseStr, "bool.str");
                }
                // Need to convert integer/float to string
                // Allocate a small buffer on stack
                llvm::Value *buf = builder_->CreateAlloca(
                    llvm::Type::getInt8Ty(*context_),
                    llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 32),
                    "strbuf");

                if (freestanding_)
                {
                    // (AR) في الوضع الحر: استخدم __sad_itoa بدلاً من sprintf المتغيرة الوسائط
                    // (EN) Freestanding: use __sad_itoa instead of variadic sprintf
                    llvm::FunctionType *itoaType = llvm::FunctionType::get(
                        llvm::Type::getInt32Ty(*context_),
                        {llvm::PointerType::getUnqual(*context_),
                         llvm::Type::getInt64Ty(*context_)},
                        false);
                    llvm::FunctionCallee itoaFn = module_->getOrInsertFunction("__sad_itoa", itoaType);

                    if (ty->isIntegerTy())
                    {
                        llvm::Value *val64 = builder_->CreateIntCast(val, llvm::Type::getInt64Ty(*context_), true);
                        builder_->CreateCall(itoaFn, {buf, val64});
                    }
                    else if (ty->isDoubleTy())
                    {
                        // (AR) تحويل العشري إلى نص عبر __sad_ftoa (دقة كاملة)
                        // (EN) Convert double to string via __sad_ftoa (full precision)
                        llvm::FunctionType *ftoaType = llvm::FunctionType::get(
                            llvm::Type::getInt32Ty(*context_),
                            {llvm::PointerType::getUnqual(*context_),
                             llvm::Type::getDoubleTy(*context_)},
                            false);
                        llvm::FunctionCallee ftoaFn = module_->getOrInsertFunction("__sad_ftoa", ftoaType);
                        builder_->CreateCall(ftoaFn, {buf, val});
                    }
                }
                else
                {
                    // Declare sprintf if not already declared
                    llvm::FunctionType *sprintfType = llvm::FunctionType::get(
                        llvm::Type::getInt32Ty(*context_),
                        {llvm::PointerType::getUnqual(*context_),
                         llvm::PointerType::getUnqual(*context_)},
                        true);
                    llvm::FunctionCallee sprintfFn = module_->getOrInsertFunction("sprintf", sprintfType);

                    if (ty->isIntegerTy())
                    {
                        llvm::Value *fmt = builder_->CreateGlobalStringPtr("%lld", "int.fmt");
                        llvm::Value *val64 = builder_->CreateIntCast(val, llvm::Type::getInt64Ty(*context_), true);
                        builder_->CreateCall(sprintfFn, {buf, fmt, val64});
                    }
                    else if (ty->isDoubleTy())
                    {
                        llvm::Value *fmt = builder_->CreateGlobalStringPtr("%g", "float.fmt");
                        builder_->CreateCall(sprintfFn, {buf, fmt, val});
                    }
                }
                return buf;
            };

            left = ensureString(left, leftTy, inst->operands[0]);
            right = ensureString(right, rightTy, inst->operands[1]);

            // Get lengths using strlen
            llvm::FunctionType *strlenType = llvm::FunctionType::get(
                llvm::Type::getInt64Ty(*context_),
                {llvm::PointerType::getUnqual(*context_)},
                false);
            llvm::FunctionCallee strlenFn = module_->getOrInsertFunction("strlen", strlenType);

            llvm::Value *len1 = builder_->CreateCall(strlenFn, {left}, "len1");
            llvm::Value *len2 = builder_->CreateCall(strlenFn, {right}, "len2");
            llvm::Value *totalLen = builder_->CreateAdd(len1, len2, "totallen");
            llvm::Value *bufSize = builder_->CreateAdd(totalLen,
                                                       llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 1), "bufsize");

            // Allocate result buffer on HEAP (using malloc) so it's safe to return
            // (AR) تخصيص على الـ Heap بدلاً من Stack لتجنب مشاكل الرجوع من الدوال
            // (EN) Allocate on heap instead of stack to avoid returning dangling pointers
            llvm::FunctionType *mallocType = llvm::FunctionType::get(
                llvm::PointerType::getUnqual(*context_),
                {llvm::Type::getInt64Ty(*context_)},
                false);
            llvm::FunctionCallee mallocFn = module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *result = builder_->CreateCall(mallocFn, {bufSize}, "concat.buf");

            // Copy first string using memcpy
            llvm::FunctionType *memcpyType = llvm::FunctionType::get(
                llvm::PointerType::getUnqual(*context_),
                {llvm::PointerType::getUnqual(*context_),
                 llvm::PointerType::getUnqual(*context_),
                 llvm::Type::getInt64Ty(*context_)},
                false);
            llvm::FunctionCallee memcpyFn = module_->getOrInsertFunction("memcpy", memcpyType);

            builder_->CreateCall(memcpyFn, {result, left, len1});

            // Copy second string after first
            llvm::Value *dest2 = builder_->CreateGEP(
                llvm::Type::getInt8Ty(*context_), result, len1, "dest2");
            llvm::Value *copyLen2 = builder_->CreateAdd(len2,
                                                        llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 1), "copylen2");
            builder_->CreateCall(memcpyFn, {dest2, right, copyLen2});

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        llvm::Value *LLVMCodeGen::emitStringCharAt(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                reportError("BUILTIN_STRING_CHAR_AT requires 2 operands (string, index)");
                return nullptr;
            }

            llvm::Value *str = resolveOperand(inst->operands[0]);
            llvm::Value *index = resolveOperand(inst->operands[1]);
            if (!str || !index)
                return nullptr;

            // Ensure str is a pointer
            llvm::Type *i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
            if (!str->getType()->isPointerTy())
            {
                str = builder_->CreateIntToPtr(str, i8p, "str.ptr");
            }

            // Ensure index is i64
            if (index->getType() != llvm::Type::getInt64Ty(*context_))
            {
                index = builder_->CreateIntCast(index, llvm::Type::getInt64Ty(*context_), true, "idx.i64");
            }

            // GEP to get pointer to character at index
            llvm::Value *charPtr = builder_->CreateGEP(
                llvm::Type::getInt8Ty(*context_), str, index, "char.ptr");

            // Load the character (i8)
            llvm::Value *charVal = builder_->CreateLoad(
                llvm::Type::getInt8Ty(*context_), charPtr, "char.val");

            // Zero-extend to i64
            llvm::Value *result = builder_->CreateZExt(charVal, llvm::Type::getInt64Ty(*context_), "char.i64");

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        llvm::Value *LLVMCodeGen::emitStringCmp(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                reportError("STRING_CMP requires 2 operands");
                return nullptr;
            }

            llvm::Value *left = resolveOperand(inst->operands[0]);
            llvm::Value *right = resolveOperand(inst->operands[1]);
            if (!left || !right)
            {
                reportError("STRING_CMP: operands not found");
                return nullptr;
            }

            // تأكد أن المعاملين من نوع مؤشر
            llvm::Type *i8p = llvm::PointerType::getUnqual(*context_);
            if (!left->getType()->isPointerTy())
            {
                left = builder_->CreateIntToPtr(left, i8p, "str.l");
            }
            if (!right->getType()->isPointerTy())
            {
                right = builder_->CreateIntToPtr(right, i8p, "str.r");
            }

            // strcmp(left, right) → i32
            llvm::FunctionType *strcmpType = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(*context_), {i8p, i8p}, false);
            llvm::FunctionCallee strcmpFn = module_->getOrInsertFunction("strcmp", strcmpType);
            llvm::Value *cmpResult = builder_->CreateCall(strcmpFn, {left, right}, "strcmp.ret");

            // strcmp == 0 → متساويان → النتيجة 1 (صحيحة)
            llvm::Value *result = builder_->CreateICmpEQ(cmpResult,
                                                         llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0), "streq");

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitInlineAsm(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            // (AR) تنفيذ الأسمبلي المدمج — طبقة التراب
            // (EN) Inline assembly implementation — Turab layer
            //
            // الصيغة في SIR:
            //   operands[0] = نص الأسمبلي (STRING constant)
            //   operands[1] = قيود الأسمبلي (constraints, STRING constant) — اختياري
            //   operands[2..N] = المعاملات المدخلة — اختياري
            //   result = المعامل المُخرج — اختياري
            //
            // SIR format:
            //   operands[0] = assembly text (STRING constant)
            //   operands[1] = constraints (STRING constant) — optional
            //   operands[2..N] = input operands — optional
            //   result = output operand — optional

            if (inst->operands.empty())
            {
                reportError("INLINE_ASM: requires at least 1 operand (assembly text)");
                return nullptr;
            }

            // (AR) الحصول على نص الأسمبلي
            // (EN) Get assembly text
            std::string asmText;
            if (inst->operands[0].type == SIROperandType::CONSTANT &&
                inst->operands[0].dataType == SadTypeKind::String)
            {
                asmText = inst->operands[0].name;
            }
            else
            {
                reportError("INLINE_ASM: first operand must be a string constant (assembly text)");
                return nullptr;
            }

            // (AR) تحويل صيغة GCC إلى صيغة LLVM للأسمبلي المضمن
            // (EN) Convert GCC-style inline asm syntax to LLVM-style:
            //   GCC: %%reg → LLVM: %reg  (double-percent = literal register name)
            //   GCC: %N   → LLVM: $N    (percent + digit = operand reference)
            {
                std::string converted;
                converted.reserve(asmText.size());
                for (size_t i = 0; i < asmText.size(); i++)
                {
                    if (asmText[i] == '%')
                    {
                        if (i + 1 < asmText.size() && asmText[i + 1] == '%')
                        {
                            // GCC %% → LLVM % (literal percent / register prefix)
                            converted += '%';
                            i++; // skip second %
                        }
                        else if (i + 1 < asmText.size() && asmText[i + 1] >= '0' && asmText[i + 1] <= '9')
                        {
                            // GCC %0, %1, ... → LLVM $0, $1, ...
                            converted += '$';
                            // don't skip digit — it will be added in next iteration
                        }
                        else
                        {
                            // standalone % — keep as-is (already LLVM-style register name)
                            converted += '%';
                        }
                    }
                    else
                    {
                        converted += asmText[i];
                    }
                }
                asmText = converted;
            }

            // (AR) الحصول على قيود الأسمبلي (اختياري)
            // (EN) Get constraints (optional)
            std::string constraints;
            if (inst->operands.size() > 1 &&
                inst->operands[1].type == SIROperandType::CONSTANT &&
                inst->operands[1].dataType == SadTypeKind::String)
            {
                constraints = inst->operands[1].name;
            }

            // (AR) جمع المعاملات المدخلة
            // (EN) Collect input operands
            std::vector<llvm::Value *> inputValues;
            std::vector<llvm::Type *> inputTypes;
            for (size_t i = 2; i < inst->operands.size(); i++)
            {
                llvm::Value *val = resolveOperand(inst->operands[i]);
                if (val)
                {
                    inputValues.push_back(val);
                    inputTypes.push_back(val->getType());
                }
            }

            // (AR) تحديد نوع الرجوع
            // (EN) Determine return type
            llvm::Type *retType = llvm::Type::getVoidTy(*context_);
            bool hasResult = inst->result.has_value();
            if (hasResult)
            {
                // (AR) إذا كان هناك نتيجة، نستخدم i64 كنوع افتراضي
                // (EN) If there's a result, use i64 as default type
                retType = llvm::Type::getInt64Ty(*context_);
            }

            // (AR) بناء نوع الدالة
            // (EN) Build function type
            llvm::FunctionType *asmFuncType = llvm::FunctionType::get(retType, inputTypes, false);

            // (AR) إنشاء الأسمبلي المدمج
            // (EN) Create inline assembly
            bool hasSideEffects = true;
            bool isAlignStack = true;
            llvm::InlineAsm *inlineAsm = llvm::InlineAsm::get(
                asmFuncType, asmText, constraints, hasSideEffects, isAlignStack);

            // (AR) استدعاء الأسمبلي
            // (EN) Call inline assembly
            llvm::Value *result = builder_->CreateCall(asmFuncType, inlineAsm, inputValues);

            if (hasResult)
            {
                context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        // ============================================================================
        // (AR) توليد دالة __sad_array_to_string في الموديول
        //      تحوّل مصفوفة SadArray إلى نص مقروء: "[عنصر1, عنصر2, ...]"
        // (EN) Generate __sad_array_to_string function in the module
        //      Converts SadArray to readable string: "[elem1, elem2, ...]"
        // ============================================================================
        void LLVMCodeGen::ensureArrayToStringHelper()
        {
            // (AR) إذا الدالة موجودة ولها جسم، لا تُنشئها مرة أخرى
            // (EN) If function exists and has a body, don't recreate
            llvm::Function *existing = module_->getFunction("__sad_array_to_string");
            if (existing && !existing->empty())
            {
                return;
            }

            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            auto i32Ty = llvm::Type::getInt32Ty(*context_);
            auto i8Ty = llvm::Type::getInt8Ty(*context_);
            auto ptrTy = llvm::PointerType::getUnqual(*context_);

            // Function signature: i8* __sad_array_to_string(i8* buf, i64 len, i8* data)
            llvm::FunctionType *fnTy = llvm::FunctionType::get(ptrTy, {ptrTy, i64Ty, ptrTy}, false);
            llvm::Function *fn = llvm::Function::Create(
                fnTy, llvm::Function::InternalLinkage, "__sad_array_to_string", module_.get());

            llvm::Argument *bufArg = fn->getArg(0);
            llvm::Argument *lenArg = fn->getArg(1);
            llvm::Argument *dataArg = fn->getArg(2);
            bufArg->setName("buf");
            lenArg->setName("len");
            dataArg->setName("data");

            // Save current insertion point
            llvm::BasicBlock *savedBB = builder_->GetInsertBlock();
            llvm::BasicBlock::iterator savedPoint = builder_->GetInsertPoint();

            // Create basic blocks
            llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(*context_, "entry", fn);
            llvm::BasicBlock *loopCheckBB = llvm::BasicBlock::Create(*context_, "loop.check", fn);
            llvm::BasicBlock *loopBodyBB = llvm::BasicBlock::Create(*context_, "loop.body", fn);
            llvm::BasicBlock *commaWriteBB = llvm::BasicBlock::Create(*context_, "comma.write", fn);
            llvm::BasicBlock *elemWriteBB = llvm::BasicBlock::Create(*context_, "elem.write", fn);
            llvm::BasicBlock *loopEndBB = llvm::BasicBlock::Create(*context_, "loop.end", fn);

            // Declare sprintf
            llvm::FunctionType *sprintfTy = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, true);
            llvm::FunctionCallee sprintfFn = module_->getOrInsertFunction("sprintf", sprintfTy);

            // entry: write '[' at buf[0], pos = 1
            builder_->SetInsertPoint(entryBB);
            builder_->CreateStore(llvm::ConstantInt::get(i8Ty, '['), bufArg);
            llvm::Value *initPos = llvm::ConstantInt::get(i64Ty, 1);
            builder_->CreateBr(loopCheckBB);

            // loop.check: i = phi, pos = phi; if i < len goto body else goto end
            builder_->SetInsertPoint(loopCheckBB);
            llvm::PHINode *iPhi = builder_->CreatePHI(i64Ty, 2, "i");
            llvm::PHINode *posPhi = builder_->CreatePHI(i64Ty, 2, "pos");
            iPhi->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entryBB);
            posPhi->addIncoming(initPos, entryBB);
            llvm::Value *cmp = builder_->CreateICmpSLT(iPhi, lenArg, "i.lt.len");
            builder_->CreateCondBr(cmp, loopBodyBB, loopEndBB);

            // loop.body: if i > 0, write ", "
            builder_->SetInsertPoint(loopBodyBB);
            llvm::Value *needComma = builder_->CreateICmpSGT(iPhi, llvm::ConstantInt::get(i64Ty, 0), "need.comma");
            builder_->CreateCondBr(needComma, commaWriteBB, elemWriteBB);

            // comma.write: write ", " at buf+pos
            builder_->SetInsertPoint(commaWriteBB);
            llvm::Value *commaFmt = builder_->CreateGlobalStringPtr(", ", "comma.fmt");
            llvm::Value *commaPos = builder_->CreateGEP(i8Ty, bufArg, posPhi, "comma.ptr");
            llvm::Value *commaLen = builder_->CreateCall(sprintfFn, {commaPos, commaFmt}, "comma.len");
            llvm::Value *commaLen64 = builder_->CreateSExt(commaLen, i64Ty, "comma.len64");
            llvm::Value *posAfterComma = builder_->CreateAdd(posPhi, commaLen64, "pos.after.comma");
            builder_->CreateBr(elemWriteBB);

            // elem.write: load element, sprintf it, advance pos
            builder_->SetInsertPoint(elemWriteBB);
            llvm::PHINode *elemPosPhi = builder_->CreatePHI(i64Ty, 2, "elem.pos");
            elemPosPhi->addIncoming(posPhi, loopBodyBB);
            elemPosPhi->addIncoming(posAfterComma, commaWriteBB);

            // Load element as i64 (all array elements stored as i64 or ptr-sized values)
            llvm::Value *elemGep = builder_->CreateGEP(i64Ty, dataArg, iPhi, "elem.gep");
            llvm::Value *elemVal = builder_->CreateLoad(i64Ty, elemGep, "elem.val");

            // sprintf(buf+pos, "%lld", elem)
            llvm::Value *elemFmt = builder_->CreateGlobalStringPtr("%lld", "elem.fmt");
            llvm::Value *elemDst = builder_->CreateGEP(i8Ty, bufArg, elemPosPhi, "elem.dst");
            llvm::Value *elemLen = builder_->CreateCall(sprintfFn, {elemDst, elemFmt, elemVal}, "elem.len");
            llvm::Value *elemLen64 = builder_->CreateSExt(elemLen, i64Ty, "elem.len64");
            llvm::Value *newPos = builder_->CreateAdd(elemPosPhi, elemLen64, "new.pos");

            // i++
            llvm::Value *nextI = builder_->CreateAdd(iPhi, llvm::ConstantInt::get(i64Ty, 1), "next.i");

            // Back to loop check
            iPhi->addIncoming(nextI, elemWriteBB);
            posPhi->addIncoming(newPos, elemWriteBB);
            builder_->CreateBr(loopCheckBB);

            // loop.end: write ']' and '\0'
            builder_->SetInsertPoint(loopEndBB);
            llvm::Value *closeBracketPtr = builder_->CreateGEP(i8Ty, bufArg, posPhi, "close.ptr");
            builder_->CreateStore(llvm::ConstantInt::get(i8Ty, ']'), closeBracketPtr);
            llvm::Value *endPos = builder_->CreateAdd(posPhi, llvm::ConstantInt::get(i64Ty, 1), "end.pos");
            llvm::Value *nullPtr = builder_->CreateGEP(i8Ty, bufArg, endPos, "null.ptr");
            builder_->CreateStore(llvm::ConstantInt::get(i8Ty, 0), nullPtr);
            builder_->CreateRet(bufArg);

            // Restore insertion point
            if (savedBB)
            {
                builder_->SetInsertPoint(savedBB, savedPoint);
            }
        }
    } // namespace LLVM
} // namespace Sad