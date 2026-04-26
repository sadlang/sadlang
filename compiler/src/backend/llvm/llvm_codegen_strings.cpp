/*
 * ============================================================================
 * مولد كود LLVM IR - ملف التنفيذ
 * LLVM IR Code Generator - Implementation File
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "llvm_optimizer.h"
#include "llvm_volatile_ops.h"
#include "sir_constants.h"
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
                // (AR) === فحص ADT pointer tagging ===
                //      إذا كانت القيمة i64 من ENUM_GET_PAYLOAD وتحمل flag __is_ptr
                //      فنفحص وقت التشغيل: إذا كانت مؤشر (نص) → inttoptr → strlen
                //      وإلا → sprintf كرقم عادي
                // (EN) === ADT pointer tagging check ===
                //      If value is i64 from ENUM_GET_PAYLOAD with __is_ptr flag
                //      runtime check: if pointer (string) → inttoptr → strlen
                //      else → sprintf as regular number
                if (ty->isIntegerTy(64) && !op.name.empty())
                {
                    // ================================================================
                    // (AR) [إصلاح قنوات] فك وسم MSB 2-bit للقيم المستقبَلة من القنوات
                    //      نوع Any يعني أن القيمة موسومة:
                    //      bit63=0 → مؤشر (نص)
                    //      bit63=1, bit62=0 → رقم — مسح bit63 ثم sprintf
                    //      bit63=1, bit62=1 → منطقي — مسح bit63+62 ثم صحيح/خطأ
                    //      kSadNullSentinel → "لاشيء"
                    // (EN) [Channel fix] Decode MSB 2-bit for channel-received values in concat
                    // ================================================================
                    if (op.dataType == SadTypeKind::Any)
                    {
                        auto *i64Ty_l = llvm::Type::getInt64Ty(*context_);
                        auto *ptrTy_l = llvm::PointerType::getUnqual(*context_);

                        // (AR) فحص sentinel (لاشيء) أولاً
                        llvm::Value *isNullSentinel = builder_->CreateICmpEQ(
                            val, llvm::ConstantInt::get(i64Ty_l, Sad::Compiler::kSadNullSentinel),
                            "any.concat.null");

                        auto *parentFunc = builder_->GetInsertBlock()->getParent();
                        auto *nullBB = llvm::BasicBlock::Create(*context_, "any.c.null", parentFunc);
                        auto *checkBB = llvm::BasicBlock::Create(*context_, "any.c.check", parentFunc);
                        auto *ptrBB_l = llvm::BasicBlock::Create(*context_, "any.c.ptr", parentFunc);
                        auto *intOrBoolBB = llvm::BasicBlock::Create(*context_, "any.c.iob", parentFunc);
                        auto *boolBB_l = llvm::BasicBlock::Create(*context_, "any.c.bool", parentFunc);
                        auto *intBB_l = llvm::BasicBlock::Create(*context_, "any.c.int", parentFunc);
                        auto *mergeBB = llvm::BasicBlock::Create(*context_, "any.c.merge", parentFunc);

                        builder_->CreateCondBr(isNullSentinel, nullBB, checkBB);

                        // (AR) لاشيء
                        builder_->SetInsertPoint(nullBB);
                        llvm::Value *nullStr = builder_->CreateGlobalStringPtr(
                            "\xd9\x84\xd8\xa7\xd8\xb4\xd9\x8a\xd8\xa1", "any.c.nullstr"); // لاشيء
                        builder_->CreateBr(mergeBB);

                        // (AR) فحص bit63
                        builder_->SetInsertPoint(checkBB);
                        llvm::Value *bit63Mask = llvm::ConstantInt::get(i64Ty_l, 1ULL << 63);
                        llvm::Value *bit63 = builder_->CreateAnd(val, bit63Mask, "any.c.bit63");
                        llvm::Value *isPtr = builder_->CreateICmpEQ(
                            bit63, llvm::ConstantInt::get(i64Ty_l, 0), "any.c.isptr");
                        builder_->CreateCondBr(isPtr, ptrBB_l, intOrBoolBB);

                        // (AR) مؤشر (نص)
                        builder_->SetInsertPoint(ptrBB_l);
                        llvm::Value *strPtr = builder_->CreateIntToPtr(val, ptrTy_l, "any.c.str");
                        llvm::Value *ptrIsNull = builder_->CreateICmpEQ(
                            strPtr, llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy_l)), "any.c.ptrnull");
                        llvm::Value *voidStr = builder_->CreateGlobalStringPtr("void", "any.c.void");
                        llvm::Value *safeStr = builder_->CreateSelect(ptrIsNull, voidStr, strPtr, "any.c.safe");
                        builder_->CreateBr(mergeBB);

                        // (AR) فحص bit62 — منطقي أم رقم
                        builder_->SetInsertPoint(intOrBoolBB);
                        llvm::Value *bit62Mask = llvm::ConstantInt::get(i64Ty_l, 1ULL << 62);
                        llvm::Value *bit62 = builder_->CreateAnd(val, bit62Mask, "any.c.bit62");
                        llvm::Value *isBool = builder_->CreateICmpNE(
                            bit62, llvm::ConstantInt::get(i64Ty_l, 0), "any.c.isbool");
                        builder_->CreateCondBr(isBool, boolBB_l, intBB_l);

                        // (AR) منطقي
                        builder_->SetInsertPoint(boolBB_l);
                        llvm::Value *clearMaskB = llvm::ConstantInt::get(i64Ty_l, ~(3ULL << 62));
                        llvm::Value *cleanBool = builder_->CreateAnd(val, clearMaskB, "any.c.bclean");
                        llvm::Value *trueStr = builder_->CreateGlobalStringPtr(
                            "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "any.c.true");
                        llvm::Value *falseStr = builder_->CreateGlobalStringPtr(
                            "\xd8\xae\xd8\xb7\xd8\xa3", "any.c.false");
                        llvm::Value *boolCond = builder_->CreateICmpNE(
                            cleanBool, llvm::ConstantInt::get(i64Ty_l, 0), "any.c.bcond");
                        llvm::Value *boolStr = builder_->CreateSelect(boolCond, trueStr, falseStr, "any.c.bstr");
                        builder_->CreateBr(mergeBB);

                        // (AR) رقم — مسح bit63 ثم sprintf
                        builder_->SetInsertPoint(intBB_l);
                        llvm::Value *clearBit63 = llvm::ConstantInt::get(i64Ty_l, ~(1ULL << 63));
                        llvm::Value *cleanInt = builder_->CreateAnd(val, clearBit63, "any.c.iclean");
                        llvm::Value *numBuf = builder_->CreateAlloca(
                            llvm::Type::getInt8Ty(*context_),
                            llvm::ConstantInt::get(i64Ty_l, 32), "any.c.nbuf");
                        llvm::FunctionType *sprintfTy = llvm::FunctionType::get(
                            llvm::Type::getInt32Ty(*context_),
                            {ptrTy_l, ptrTy_l}, true);
                        llvm::FunctionCallee sprintfFunc = module_->getOrInsertFunction("sprintf", sprintfTy);
                        llvm::Value *intFmt = builder_->CreateGlobalStringPtr("%lld", "any.c.ifmt");
                        builder_->CreateCall(sprintfFunc, {numBuf, intFmt, cleanInt});
                        builder_->CreateBr(mergeBB);

                        // (AR) دمج النتائج بـ PHI
                        builder_->SetInsertPoint(mergeBB);
                        auto *phi = builder_->CreatePHI(ptrTy_l, 4, "any.c.result");
                        phi->addIncoming(nullStr, nullBB);
                        phi->addIncoming(safeStr, ptrBB_l);
                        phi->addIncoming(boolStr, boolBB_l);
                        phi->addIncoming(numBuf, intBB_l);
                        return phi;
                    }

                    auto isPtrIt = context_info_.namedValues.find(op.name + ".__is_ptr");
                    if (isPtrIt != context_info_.namedValues.end())
                    {
                        llvm::Value *isPtr = isPtrIt->second;

                        // (AR) فك وسم 2-bit: bit63 + bit62
                        //      bit63=0 → مؤشر (نص)
                        //      bit63=1, bit62=0 → رقم
                        //      bit63=1, bit62=1 → منطقي (boolean)
                        // (EN) 2-bit untagging: bit63 + bit62
                        llvm::Value *clearMask = llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), ~(3ULL << 62));
                        llvm::Value *cleanVal = builder_->CreateAnd(val, clearMask, "adt.clean");

                        // (AR) فحص bit62 للتمييز بين رقم ومنطقي
                        llvm::Value *bit62Mask = llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 1ULL << 62);
                        llvm::Value *bit62 = builder_->CreateAnd(val, bit62Mask, "adt.bit62");
                        llvm::Value *isBool = builder_->CreateICmpNE(bit62, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 0), "adt.isbool");

                        // (AR) تحويل i64 إلى ptr لحالة النص
                        llvm::Value *asPtr = builder_->CreateIntToPtr(
                            cleanVal, llvm::PointerType::getUnqual(*context_), "adt.str.ptr");
                        llvm::Value *ptrIsNull = builder_->CreateICmpEQ(
                            asPtr,
                            llvm::ConstantPointerNull::get(
                                llvm::cast<llvm::PointerType>(asPtr->getType())),
                            "adt.ptr.isnull");
                        llvm::Value *voidStr = builder_->CreateGlobalStringPtr("void", "adt.null.str");
                        llvm::Value *safePtr = builder_->CreateSelect(
                            ptrIsNull, voidStr, asPtr, "adt.ptr.safe");

                        // (AR) تحويل الرقم إلى نص عبر sprintf
                        llvm::Value *numBuf = builder_->CreateAlloca(
                            llvm::Type::getInt8Ty(*context_),
                            llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 32),
                            "adt.num.buf");
                        llvm::FunctionType *sprintfTy = llvm::FunctionType::get(
                            llvm::Type::getInt32Ty(*context_),
                            {llvm::PointerType::getUnqual(*context_),
                             llvm::PointerType::getUnqual(*context_)},
                            true);
                        llvm::FunctionCallee sprintfFunc = module_->getOrInsertFunction("sprintf", sprintfTy);
                        llvm::Value *intFmt = builder_->CreateGlobalStringPtr("%lld", "adt.int.fmt");
                        builder_->CreateCall(sprintfFunc, {numBuf, intFmt, cleanVal});

                        // (AR) نصوص المنطقي
                        llvm::Value *trueStr = builder_->CreateGlobalStringPtr("\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "adt.bool.true");
                        llvm::Value *falseStr = builder_->CreateGlobalStringPtr("\xd8\xae\xd8\xb7\xd8\xa3", "adt.bool.false");
                        llvm::Value *boolCond = builder_->CreateICmpNE(cleanVal, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 0), "adt.bool.cond");
                        llvm::Value *boolStr = builder_->CreateSelect(boolCond, trueStr, falseStr, "adt.bool.str");

                        // (AR) اختيار النتيجة: مؤشر / منطقي / رقم
                        //      isPtr → safePtr
                        //      !isPtr && isBool → boolStr
                        //      !isPtr && !isBool → numBuf
                        llvm::Value *nonPtrResult = builder_->CreateSelect(isBool, boolStr, numBuf, "adt.nonptr");
                        return builder_->CreateSelect(isPtr, safePtr, nonPtrResult, "adt.str.result");
                    }
                }

                // ================================================================
                // (AR) القيم المنطقية بنوع LLVM i1 — تحويل مباشر إلى "صحيح"/"خطأ"
                //      يحدث عند تحميل حقل bool من كائن وربطه بنص عبر string.concat
                // (EN) Boolean values with LLVM type i1 — direct conversion to "صحيح"/"خطأ"
                //      Occurs when loading a bool field from object and concatenating with string
                // ================================================================
                if (ty->isIntegerTy(1))
                {
                    llvm::Value *trueStr = builder_->CreateGlobalStringPtr(
                        "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "bool.true"); // صحيح
                    llvm::Value *falseStr = builder_->CreateGlobalStringPtr(
                        "\xd8\xae\xd8\xb7\xd8\xa3", "bool.false"); // خطأ
                    return builder_->CreateSelect(val, trueStr, falseStr, "bool.str");
                }

                // ================================================================
                // (AR) فحص kSadNullSentinel — القيمة الخاصة بـ لاشيء
                //      إذا كان الرقم يساوي kSadNullSentinel نُرجع "لاشيء"
                //      يحدث مع: حاول_استقبل من قناة فارغة، استقبل_بمهلة بعد انتهاء المهلة
                // (EN) Check kSadNullSentinel — special null value
                //      If integer equals kSadNullSentinel, return "لاشيء"
                //      Occurs with: try_recv from empty channel, recv_timeout after timeout
                // ================================================================
                if (ty->isIntegerTy(64))
                {
                    auto *i64Ty_s = llvm::Type::getInt64Ty(*context_);
                    auto *sentinelVal = llvm::ConstantInt::get(i64Ty_s, Sad::Compiler::kSadNullSentinel);
                    auto *isSentinel = builder_->CreateICmpEQ(val, sentinelVal, "int.is.null");
                    auto *nullStr = builder_->CreateGlobalStringPtr(
                        "\xd9\x84\xd8\xa7\xd8\xb4\xd9\x8a\xd8\xa1", "int.null.str"); // لاشيء

                    auto *parentFunc3 = builder_->GetInsertBlock()->getParent();
                    auto *sentinelBB = llvm::BasicBlock::Create(*context_, "int.sentinel", parentFunc3);
                    auto *normalBB = llvm::BasicBlock::Create(*context_, "int.normal", parentFunc3);
                    auto *intMergeBB = llvm::BasicBlock::Create(*context_, "int.merge", parentFunc3);

                    builder_->CreateCondBr(isSentinel, sentinelBB, normalBB);

                    // (AR) كتلة sentinel: إرجاع "لاشيء"
                    builder_->SetInsertPoint(sentinelBB);
                    builder_->CreateBr(intMergeBB);

                    // (AR) كتلة عادية: تحويل الرقم إلى نص
                    builder_->SetInsertPoint(normalBB);

                    // Allocate a small buffer on stack
                    llvm::Value *buf = builder_->CreateAlloca(
                        llvm::Type::getInt8Ty(*context_),
                        llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 32),
                        "strbuf");

                    if (freestanding_)
                    {
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
                            // (AR) إصلاح: استخدام __sad_format_double لمطابقة دقة المفسر
                            // (EN) Fix: use __sad_format_double to match interpreter precision
                            auto *fmtDblType = llvm::FunctionType::get(
                                llvm::Type::getVoidTy(*context_),
                                {llvm::PointerType::getUnqual(*context_),
                                 llvm::Type::getDoubleTy(*context_)},
                                false);
                            auto fmtDblFn = module_->getOrInsertFunction("__sad_format_double", fmtDblType);
                            builder_->CreateCall(fmtDblFn, {buf, val});
                        }
                    }

                    builder_->CreateBr(intMergeBB);

                    // (AR) دمج النتائج
                    builder_->SetInsertPoint(intMergeBB);
                    auto *ptrTy3 = llvm::PointerType::getUnqual(*context_);
                    auto *intPhi = builder_->CreatePHI(ptrTy3, 2, "int.str.result");
                    intPhi->addIncoming(nullStr, sentinelBB);
                    intPhi->addIncoming(buf, normalBB);
                    return intPhi;
                }

                // (AR) للأنواع الأخرى (float/double): المسار العادي بدون فحص sentinel
                // Allocate a small buffer on stack
                llvm::Value *buf = builder_->CreateAlloca(
                    llvm::Type::getInt8Ty(*context_),
                    llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 32),
                    "strbuf");
                if (ty->isDoubleTy())
                {
                    if (freestanding_)
                    {
                        llvm::FunctionType *ftoaType = llvm::FunctionType::get(
                            llvm::Type::getInt32Ty(*context_),
                            {llvm::PointerType::getUnqual(*context_),
                             llvm::Type::getDoubleTy(*context_)},
                            false);
                        llvm::FunctionCallee ftoaFn = module_->getOrInsertFunction("__sad_ftoa", ftoaType);
                        builder_->CreateCall(ftoaFn, {buf, val});
                    }
                    else
                    {
                        // (AR) إصلاح: استخدام __sad_format_double لمطابقة دقة المفسر
                        // (EN) Fix: use __sad_format_double to match interpreter precision
                        auto *fmtDblType = llvm::FunctionType::get(
                            llvm::Type::getVoidTy(*context_),
                            {llvm::PointerType::getUnqual(*context_),
                             llvm::Type::getDoubleTy(*context_)},
                            false);
                        auto fmtDblFn = module_->getOrInsertFunction("__sad_format_double", fmtDblType);
                        builder_->CreateCall(fmtDblFn, {buf, val});
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

            // (AR) حماية ضد null sentinel وقيم tagged:
            //      القيم المستقبلة من القنوات قد تحمل null sentinel أو أرقام مُعلّمة
            //      (bit63=1). استدعاء strcmp على هذه القيم يسبب ACCESS_VIOLATION.
            //      الحل: فحص bit63 لكلا المعاملين قبل strcmp.
            //      إذا كان أي منهما يحمل bit63=1 أو يساوي null → النتيجة = خطأ (غير متساويين).
            // (EN) Guard against null sentinel and tagged values from channel recv:
            //      If bit63=1, value is a tagged int/bool, not a string pointer.
            //      Calling strcmp on it would crash. Check both operands before strcmp.
            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            auto i8p = llvm::PointerType::getUnqual(*context_);
            auto zero64 = llvm::ConstantInt::get(i64Ty, 0);
            auto bit63Mask = llvm::ConstantInt::get(i64Ty, 1ULL << 63);

            // (AR) الحصول على تمثيل i64 لفحص العلامات
            llvm::Value *leftI64 = left->getType()->isPointerTy()
                                       ? builder_->CreatePtrToInt(left, i64Ty, "str.cmp.l.i64")
                                       : left;
            llvm::Value *rightI64 = right->getType()->isPointerTy()
                                        ? builder_->CreatePtrToInt(right, i64Ty, "str.cmp.r.i64")
                                        : right;

            // (AR) فحص: bit63=0 (مؤشر صالح) و ≠ null
            auto leftTag = builder_->CreateAnd(leftI64, bit63Mask, "str.cmp.l.tag");
            auto rightTag = builder_->CreateAnd(rightI64, bit63Mask, "str.cmp.r.tag");
            auto leftIsStr = builder_->CreateICmpEQ(leftTag, zero64, "str.cmp.l.isstr");
            auto rightIsStr = builder_->CreateICmpEQ(rightTag, zero64, "str.cmp.r.isstr");
            auto leftNotNull = builder_->CreateICmpNE(leftI64, zero64, "str.cmp.l.nn");
            auto rightNotNull = builder_->CreateICmpNE(rightI64, zero64, "str.cmp.r.nn");

            auto leftOk = builder_->CreateAnd(leftIsStr, leftNotNull, "str.cmp.l.ok");
            auto rightOk = builder_->CreateAnd(rightIsStr, rightNotNull, "str.cmp.r.ok");
            auto canStrcmp = builder_->CreateAnd(leftOk, rightOk, "str.cmp.can");

            // (AR) تفريع: إذا كلاهما نص صالح → strcmp، وإلا → خطأ
            auto *entryBB = builder_->GetInsertBlock();
            auto *parentFunc = entryBB->getParent();
            auto *strcmpBB = llvm::BasicBlock::Create(*context_, "str.cmp.do", parentFunc);
            auto *doneBB = llvm::BasicBlock::Create(*context_, "str.cmp.done", parentFunc);

            builder_->CreateCondBr(canStrcmp, strcmpBB, doneBB);

            // ─── كتلة strcmp: مقارنة فعلية ───
            builder_->SetInsertPoint(strcmpBB);
            llvm::Value *leftPtr = left->getType()->isPointerTy()
                                       ? left
                                       : builder_->CreateIntToPtr(left, i8p, "str.l");
            llvm::Value *rightPtr = right->getType()->isPointerTy()
                                        ? right
                                        : builder_->CreateIntToPtr(right, i8p, "str.r");

            llvm::FunctionType *strcmpType = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(*context_), {i8p, i8p}, false);
            llvm::FunctionCallee strcmpFn = module_->getOrInsertFunction("strcmp", strcmpType);
            llvm::Value *cmpResult = builder_->CreateCall(strcmpFn, {leftPtr, rightPtr}, "strcmp.ret");

            // strcmp == 0 → متساويان → النتيجة 1 (صحيحة)
            llvm::Value *streq = builder_->CreateICmpEQ(cmpResult,
                                                        llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0), "streq");
            builder_->CreateBr(doneBB);

            // ─── كتلة النتيجة: دمج ───
            builder_->SetInsertPoint(doneBB);
            auto *result = builder_->CreatePHI(llvm::Type::getInt1Ty(*context_), 2, "streq.phi");
            result->addIncoming(streq, strcmpBB);
            result->addIncoming(llvm::ConstantInt::getFalse(*context_), entryBB);

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