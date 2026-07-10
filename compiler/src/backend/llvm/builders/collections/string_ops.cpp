/*
 * ============================================================================
 * StringOpsCodeGen — توليد LLVM IR لعمليات النصوص
 * Phase 7 Step 6 — 17 methods استُخرِجت من 3 ملفات نصوص
 * ============================================================================
 */
#include "builders/collections/string_ops_codegen.h"
#include "llvm_codegen.h"

#include <llvm/IR/Function.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/DerivedTypes.h>

#include "sir_constants.h"
#include "adt_payload_tags.h"

using namespace Sad::Compiler::SIR;

namespace Sad {
namespace LLVM {
static llvm::StructType *getArrayStructType(llvm::LLVMContext &ctx)
{
    llvm::StructType *st = llvm::StructType::getTypeByName(ctx, "SadArray");
    if (!st) {
        st = llvm::StructType::create(ctx, "SadArray");
        st->setBody({llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0), llvm::Type::getInt64Ty(ctx), llvm::Type::getInt64Ty(ctx), llvm::Type::getInt32Ty(ctx)});
    }
    return st;
}


        // ============================================================================
        // (AR) عمليات النصوص: emitStringConcat, emitStringCharAt, emitStringCmp,
        //      cg_.emitInlineAsm, cg_.ensureArrayToStringHelper
        // (EN) String operations: concat, charAt, compare, inline asm, array-to-string
        // (AR) تم فصله من llvm_codegen_output.cpp وفق CW-05
        // ============================================================================
        llvm::Value *StringOpsCodeGen::emitStringConcat(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "STRING_CONCAT"}});
                return nullptr;
            }

            llvm::Value *left = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *right = cg_.resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "STRING_CONCAT"}});
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
                cg_.ensureArrayToStringHelper();
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
                        val = cg_.builder_->CreateIntToPtr(val,
                                                       llvm::PointerType::getUnqual(*cg_.context_), "arr.i2p");
                        ty = val->getType();
                    }
                    if (ty->isPointerTy())
                    {
                        // (AR) قراءة طول المصفوفة من هيكل SadArray {i64 length, i64 capacity, ptr data}
                        // (EN) Read array length from SadArray struct {i64 length, i64 capacity, ptr data}
                        llvm::StructType *arrTy = llvm::StructType::getTypeByName(*cg_.context_, "SadArray");
                        if (!arrTy)
                        {
                            arrTy = llvm::StructType::create(*cg_.context_, {
                                                                            llvm::Type::getInt64Ty(*cg_.context_),      // length
                                                                            llvm::Type::getInt64Ty(*cg_.context_),      // capacity
                                                                            llvm::PointerType::getUnqual(*cg_.context_) // data
                                                                        },
                                                             "SadArray");
                        }

                        auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
                        auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

                        // Load length
                        llvm::Value *lenGep = cg_.builder_->CreateStructGEP(arrTy, val, 0, "arr.str.len.gep");
                        llvm::Value *arrLen = cg_.builder_->CreateLoad(i64Ty, lenGep, "arr.str.len");

                        // Load data pointer
                        llvm::Value *dataGep = cg_.builder_->CreateStructGEP(arrTy, val, 2, "arr.str.data.gep");
                        llvm::Value *dataPtr = cg_.builder_->CreateLoad(ptrTy, dataGep, "arr.str.data");

                        // (AR) تخصيص مخزن كبير كافٍ: "[" + (كل عنصر حتى 32 حرف + ", ") * الطول + "]" + '\0'
                        // (EN) Allocate sufficient buffer: "[" + (each elem up to 32 chars + ", ") * len + "]" + '\0'
                        llvm::Value *bufLen = cg_.builder_->CreateAdd(
                            cg_.builder_->CreateMul(arrLen, llvm::ConstantInt::get(i64Ty, 34), "arr.str.elmsz"),
                            llvm::ConstantInt::get(i64Ty, 4), "arr.str.bufsz");

                        llvm::FunctionType *mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
                        llvm::FunctionCallee mallocFn = cg_.module_->getOrInsertFunction("malloc", mallocType);
                        llvm::Value *buf = cg_.builder_->CreateCall(mallocFn, {bufLen}, "arr.str.buf");

                        // (AR) استدعاء __sad_array_to_string_impl(buf, arrLen, dataPtr) -> i32 (عدد الأحرف المكتوبة)
                        // (EN) Call __sad_array_to_string_impl(buf, arrLen, dataPtr) -> i32 (chars written)
                        // We generate this inline: write "[", then loop over elements, write "]"

                        // For simplicity, use sprintf to format: "[%lld, %lld, ...]"
                        // Start with "["
                        llvm::FunctionType *sprintfType = llvm::FunctionType::get(
                            llvm::Type::getInt32Ty(*cg_.context_), {ptrTy, ptrTy}, true);
                        llvm::FunctionCallee sprintfFn = cg_.module_->getOrInsertFunction("sprintf", sprintfType);

                        // (AR) بناء النص التمثيلي عبر دالة مساعدة مُولَّدة في LLVM IR
                        // (EN) Build string representation via helper function generated in LLVM IR
                        llvm::FunctionType *helperType = llvm::FunctionType::get(ptrTy, {ptrTy, i64Ty, ptrTy}, false);
                        llvm::FunctionCallee helperFn = cg_.module_->getOrInsertFunction("__sad_array_to_string", helperType);
                        llvm::Value *result = cg_.builder_->CreateCall(helperFn, {buf, arrLen, dataPtr}, "arr.str.result");
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
                    return cg_.builder_->CreateGlobalStringPtr("void", "null.str");
                }

                if (ty->isPointerTy())
                {
                    // (AR) إذا كان مؤشر null فعلي (ConstantPointerNull أو i64(0) مُحوّل)
                    //      نفحص وقت التشغيل ونعيد "void" إذا كان null
                    // (EN) If it's actually an LLVM null pointer, do a runtime check
                    //      and return "void" if null
                    llvm::Value *isNull = cg_.builder_->CreateICmpEQ(
                        val,
                        llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(val->getType())),
                        "ptr.isnull");
                    llvm::Value *voidStr = cg_.builder_->CreateGlobalStringPtr("void", "null.fallback");
                    return cg_.builder_->CreateSelect(isNull, voidStr, val, "ptr.safe");
                }
                // (AR) إذا كان نوع المعامل نصاً لكن القيمة i64 (مؤشر مُخزّن كعدد صحيح)
                //      نحوّل من i64 إلى مؤشر — يحدث مع النصوص المنسقة (f-strings) والمتغيرات النصية
                // (EN) If operand type is String but LLVM value is i64 (boxed string pointer),
                //      convert from i64 to pointer — happens with f-strings and string variables
                if (op.dataType == SadTypeKind::String && ty->isIntegerTy(64))
                {
                    return cg_.builder_->CreateIntToPtr(val,
                                                    llvm::PointerType::getUnqual(*cg_.context_), "str.unbox");
                }
                // (AR) القيم المنطقية: ارجع "صحيح"/"خطأ" مباشرة بدلاً من تحويل الرقم
                // (EN) Boolean values: return "صحيح"/"خطأ" directly instead of converting the number
                if (op.dataType == SadTypeKind::Boolean)
                {
                    llvm::Value *trueStr = cg_.builder_->CreateGlobalStringPtr("\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "bool.true");
                    llvm::Value *falseStr = cg_.builder_->CreateGlobalStringPtr("\xd8\xae\xd8\xb7\xd8\xa3", "bool.false");
                    llvm::Value *cond;
                    if (val->getType()->isIntegerTy(1))
                    {
                        cond = val;
                    }
                    else if (val->getType()->isPointerTy())
                    {
                        // (AR) [Fix BF-04] المؤشرات تُقارن بـ null وليس بـ 0
                        // (EN) [Fix BF-04] Pointers must be compared to null, not integer 0
                        cond = cg_.builder_->CreateICmpNE(
                            val,
                            llvm::ConstantPointerNull::get(
                                llvm::cast<llvm::PointerType>(val->getType())),
                            "bool.cmp.ptr");
                    }
                    else
                    {
                        cond = cg_.builder_->CreateICmpNE(val, llvm::ConstantInt::get(val->getType(), 0), "bool.cmp");
                    }
                    return cg_.builder_->CreateSelect(cond, trueStr, falseStr, "bool.str");
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
                        auto *i64Ty_l = llvm::Type::getInt64Ty(*cg_.context_);
                        auto *ptrTy_l = llvm::PointerType::getUnqual(*cg_.context_);

                        // (AR) فحص sentinel (لاشيء) أولاً
                        llvm::Value *isNullSentinel = cg_.builder_->CreateICmpEQ(
                            val, llvm::ConstantInt::get(i64Ty_l, Sad::Compiler::kSadNullSentinel),
                            "any.concat.null");

                        auto *parentFunc = cg_.builder_->GetInsertBlock()->getParent();
                        auto *nullBB = llvm::BasicBlock::Create(*cg_.context_, "any.c.null", parentFunc);
                        auto *checkBB = llvm::BasicBlock::Create(*cg_.context_, "any.c.check", parentFunc);
                        auto *ptrOrFloatBB = llvm::BasicBlock::Create(*cg_.context_, "any.c.pof", parentFunc);
                        auto *ptrBB_l = llvm::BasicBlock::Create(*cg_.context_, "any.c.ptr", parentFunc);
                        auto *floatBB_l = llvm::BasicBlock::Create(*cg_.context_, "any.c.float", parentFunc);
                        auto *intOrBoolBB = llvm::BasicBlock::Create(*cg_.context_, "any.c.iob", parentFunc);
                        auto *boolBB_l = llvm::BasicBlock::Create(*cg_.context_, "any.c.bool", parentFunc);
                        auto *intBB_l = llvm::BasicBlock::Create(*cg_.context_, "any.c.int", parentFunc);
                        auto *mergeBB = llvm::BasicBlock::Create(*cg_.context_, "any.c.merge", parentFunc);

                        cg_.builder_->CreateCondBr(isNullSentinel, nullBB, checkBB);

                        // (AR) لاشيء
                        cg_.builder_->SetInsertPoint(nullBB);
                        llvm::Value *nullStr = cg_.builder_->CreateGlobalStringPtr(
                            "\xd9\x84\xd8\xa7\xd8\xb4\xd9\x8a\xd8\xa1", "any.c.nullstr"); // لاشيء
                        cg_.builder_->CreateBr(mergeBB);

                        // (AR) فحص bit63: مصفّر ⇒ مؤشّر/عشريّ (00/01)؛ مضبوط ⇒ صحيح/منطقيّ (10/11)
                        // (EN) Check bit63: clear ⇒ pointer/float (00/01); set ⇒ int/bool (10/11)
                        cg_.builder_->SetInsertPoint(checkBB);
                        llvm::Value *bit63Mask = llvm::ConstantInt::get(i64Ty_l, kAdtPayloadBit63);
                        llvm::Value *bit63 = cg_.builder_->CreateAnd(val, bit63Mask, "any.c.bit63");
                        llvm::Value *hiClear = cg_.builder_->CreateICmpEQ(
                            bit63, llvm::ConstantInt::get(i64Ty_l, 0), "any.c.hiclear");
                        cg_.builder_->CreateCondBr(hiClear, ptrOrFloatBB, intOrBoolBB);

                        // (AR) ISSUE-076/084: تمييز النصّ (00) عن الصندوق العشريّ (01) عبر bit62
                        // (EN) ISSUE-076/084: distinguish string (00) from boxed float (01) via bit62
                        cg_.builder_->SetInsertPoint(ptrOrFloatBB);
                        llvm::Value *bit62pf = cg_.builder_->CreateAnd(
                            val, llvm::ConstantInt::get(i64Ty_l, kAdtPayloadBit62), "any.c.bit62pf");
                        llvm::Value *isFloatPf = cg_.builder_->CreateICmpNE(
                            bit62pf, llvm::ConstantInt::get(i64Ty_l, 0), "any.c.isfloat");
                        cg_.builder_->CreateCondBr(isFloatPf, floatBB_l, ptrBB_l);

                        // (AR) مؤشر (نص)
                        cg_.builder_->SetInsertPoint(ptrBB_l);
                        llvm::Value *strPtr = cg_.builder_->CreateIntToPtr(val, ptrTy_l, "any.c.str");
                        llvm::Value *ptrIsNull = cg_.builder_->CreateICmpEQ(
                            strPtr, llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy_l)), "any.c.ptrnull");
                        llvm::Value *voidStr = cg_.builder_->CreateGlobalStringPtr("void", "any.c.void");
                        llvm::Value *safeStr = cg_.builder_->CreateSelect(ptrIsNull, voidStr, strPtr, "any.c.safe");
                        cg_.builder_->CreateBr(mergeBB);

                        // (AR) صندوق عشريّ (01): امسح bit62 ⇒ مؤشّر ⇒ حمّل double ⇒ نُنسّقه
                        //      بـ__sad_format_double (نفس دقّة المفسّر) في مخزن مكدّس. ISSUE-076/084.
                        // (EN) Boxed float (01): clear bit62 ⇒ pointer ⇒ load double ⇒ format it via
                        //      __sad_format_double (interpreter precision) into a stack buffer.
                        cg_.builder_->SetInsertPoint(floatBB_l);
                        llvm::Value *fboxI64 = cg_.builder_->CreateAnd(
                            val, llvm::ConstantInt::get(i64Ty_l, ~kAdtPayloadBit62), "any.c.fclear");
                        llvm::Value *fboxPtr = cg_.builder_->CreateIntToPtr(fboxI64, ptrTy_l, "any.c.fptr");
                        llvm::Value *fdbl = cg_.builder_->CreateLoad(
                            llvm::Type::getDoubleTy(*cg_.context_), fboxPtr, "any.c.fload");
                        // (AR) ISSUE-076 (Amelia #8): 512 لا 32 — %.6f لـDBL_MAX ~316 حرفًا يفيض.
                        // (EN) ISSUE-076 (Amelia #8): 512 not 32 — %.6f for DBL_MAX ~316 chars overflows.
                        llvm::Value *fbuf = cg_.builder_->CreateAlloca(
                            llvm::Type::getInt8Ty(*cg_.context_),
                            llvm::ConstantInt::get(i64Ty_l, 512), "any.c.fbuf");
                        auto *fmtDblType = llvm::FunctionType::get(
                            llvm::Type::getVoidTy(*cg_.context_),
                            {ptrTy_l, llvm::Type::getDoubleTy(*cg_.context_)}, false);
                        auto fmtDblFn = cg_.module_->getOrInsertFunction("__sad_format_double", fmtDblType);
                        cg_.builder_->CreateCall(fmtDblFn, {fbuf, fdbl});
                        cg_.builder_->CreateBr(mergeBB);

                        // (AR) فحص bit62 — منطقي أم رقم
                        cg_.builder_->SetInsertPoint(intOrBoolBB);
                        llvm::Value *bit62Mask = llvm::ConstantInt::get(i64Ty_l, 1ULL << 62);
                        llvm::Value *bit62 = cg_.builder_->CreateAnd(val, bit62Mask, "any.c.bit62");
                        llvm::Value *isBool = cg_.builder_->CreateICmpNE(
                            bit62, llvm::ConstantInt::get(i64Ty_l, 0), "any.c.isbool");
                        cg_.builder_->CreateCondBr(isBool, boolBB_l, intBB_l);

                        // (AR) منطقي
                        cg_.builder_->SetInsertPoint(boolBB_l);
                        llvm::Value *clearMaskB = llvm::ConstantInt::get(i64Ty_l, ~(3ULL << 62));
                        llvm::Value *cleanBool = cg_.builder_->CreateAnd(val, clearMaskB, "any.c.bclean");
                        llvm::Value *trueStr = cg_.builder_->CreateGlobalStringPtr(
                            "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "any.c.true");
                        llvm::Value *falseStr = cg_.builder_->CreateGlobalStringPtr(
                            "\xd8\xae\xd8\xb7\xd8\xa3", "any.c.false");
                        llvm::Value *boolCond = cg_.builder_->CreateICmpNE(
                            cleanBool, llvm::ConstantInt::get(i64Ty_l, 0), "any.c.bcond");
                        llvm::Value *boolStr = cg_.builder_->CreateSelect(boolCond, trueStr, falseStr, "any.c.bstr");
                        cg_.builder_->CreateBr(mergeBB);

                        // (AR) رقم — مسح bit63 ثم sprintf
                        cg_.builder_->SetInsertPoint(intBB_l);
                        llvm::Value *clearBit63 = llvm::ConstantInt::get(i64Ty_l, ~(1ULL << 63));
                        llvm::Value *cleanInt = cg_.builder_->CreateAnd(val, clearBit63, "any.c.iclean");
                        llvm::Value *numBuf = cg_.builder_->CreateAlloca(
                            llvm::Type::getInt8Ty(*cg_.context_),
                            llvm::ConstantInt::get(i64Ty_l, 32), "any.c.nbuf");
                        llvm::FunctionType *sprintfTy = llvm::FunctionType::get(
                            llvm::Type::getInt32Ty(*cg_.context_),
                            {ptrTy_l, ptrTy_l}, true);
                        llvm::FunctionCallee sprintfFunc = cg_.module_->getOrInsertFunction("sprintf", sprintfTy);
                        llvm::Value *intFmt = cg_.builder_->CreateGlobalStringPtr("%lld", "any.c.ifmt");
                        cg_.builder_->CreateCall(sprintfFunc, {numBuf, intFmt, cleanInt});
                        cg_.builder_->CreateBr(mergeBB);

                        // (AR) دمج النتائج بـ PHI (5 مصادر: لاشيء/نصّ/عشريّ/منطقيّ/صحيح)
                        // (EN) Merge results via PHI (5 sources: null/string/float/bool/int)
                        cg_.builder_->SetInsertPoint(mergeBB);
                        auto *phi = cg_.builder_->CreatePHI(ptrTy_l, 5, "any.c.result");
                        phi->addIncoming(nullStr, nullBB);
                        phi->addIncoming(safeStr, ptrBB_l);
                        phi->addIncoming(fbuf, floatBB_l);
                        phi->addIncoming(boolStr, boolBB_l);
                        phi->addIncoming(numBuf, intBB_l);
                        return phi;
                    }

                    auto isPtrIt = cg_.context_info_.namedValues.find(op.name + ".__is_ptr");
                    if (isPtrIt != cg_.context_info_.namedValues.end())
                    {
                        llvm::Value *isPtr = isPtrIt->second;

                        // (AR) فك وسم 2-bit: bit63 + bit62
                        //      bit63=0 → مؤشر (نص)
                        //      bit63=1, bit62=0 → رقم
                        //      bit63=1, bit62=1 → منطقي (boolean)
                        // (EN) 2-bit untagging: bit63 + bit62
                        llvm::Value *clearMask = llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), ~(3ULL << 62));
                        llvm::Value *cleanVal = cg_.builder_->CreateAnd(val, clearMask, "adt.clean");

                        // (AR) فحص bit62 للتمييز بين رقم ومنطقي
                        llvm::Value *bit62Mask = llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 1ULL << 62);
                        llvm::Value *bit62 = cg_.builder_->CreateAnd(val, bit62Mask, "adt.bit62");
                        llvm::Value *isBool = cg_.builder_->CreateICmpNE(bit62, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0), "adt.isbool");

                        // (AR) تحويل i64 إلى ptr لحالة النص
                        llvm::Value *asPtr = cg_.builder_->CreateIntToPtr(
                            cleanVal, llvm::PointerType::getUnqual(*cg_.context_), "adt.str.ptr");
                        llvm::Value *ptrIsNull = cg_.builder_->CreateICmpEQ(
                            asPtr,
                            llvm::ConstantPointerNull::get(
                                llvm::cast<llvm::PointerType>(asPtr->getType())),
                            "adt.ptr.isnull");
                        llvm::Value *voidStr = cg_.builder_->CreateGlobalStringPtr("void", "adt.null.str");
                        llvm::Value *safePtr = cg_.builder_->CreateSelect(
                            ptrIsNull, voidStr, asPtr, "adt.ptr.safe");

                        // (AR) تحويل الرقم إلى نص عبر sprintf
                        llvm::Value *numBuf = cg_.builder_->CreateAlloca(
                            llvm::Type::getInt8Ty(*cg_.context_),
                            llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 32),
                            "adt.num.buf");
                        llvm::FunctionType *sprintfTy = llvm::FunctionType::get(
                            llvm::Type::getInt32Ty(*cg_.context_),
                            {llvm::PointerType::getUnqual(*cg_.context_),
                             llvm::PointerType::getUnqual(*cg_.context_)},
                            true);
                        llvm::FunctionCallee sprintfFunc = cg_.module_->getOrInsertFunction("sprintf", sprintfTy);
                        llvm::Value *intFmt = cg_.builder_->CreateGlobalStringPtr("%lld", "adt.int.fmt");
                        cg_.builder_->CreateCall(sprintfFunc, {numBuf, intFmt, cleanVal});

                        // (AR) نصوص المنطقي
                        llvm::Value *trueStr = cg_.builder_->CreateGlobalStringPtr("\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "adt.bool.true");
                        llvm::Value *falseStr = cg_.builder_->CreateGlobalStringPtr("\xd8\xae\xd8\xb7\xd8\xa3", "adt.bool.false");
                        llvm::Value *boolCond = cg_.builder_->CreateICmpNE(cleanVal, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0), "adt.bool.cond");
                        llvm::Value *boolStr = cg_.builder_->CreateSelect(boolCond, trueStr, falseStr, "adt.bool.str");

                        // (AR) اختيار النتيجة: مؤشر / منطقي / رقم
                        //      isPtr → safePtr
                        //      !isPtr && isBool → boolStr
                        //      !isPtr && !isBool → numBuf
                        llvm::Value *nonPtrResult = cg_.builder_->CreateSelect(isBool, boolStr, numBuf, "adt.nonptr");
                        return cg_.builder_->CreateSelect(isPtr, safePtr, nonPtrResult, "adt.str.result");
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
                    llvm::Value *trueStr = cg_.builder_->CreateGlobalStringPtr(
                        "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "bool.true"); // صحيح
                    llvm::Value *falseStr = cg_.builder_->CreateGlobalStringPtr(
                        "\xd8\xae\xd8\xb7\xd8\xa3", "bool.false"); // خطأ
                    return cg_.builder_->CreateSelect(val, trueStr, falseStr, "bool.str");
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
                    auto *i64Ty_s = llvm::Type::getInt64Ty(*cg_.context_);
                    auto *sentinelVal = llvm::ConstantInt::get(i64Ty_s, Sad::Compiler::kSadNullSentinel);
                    auto *isSentinel = cg_.builder_->CreateICmpEQ(val, sentinelVal, "int.is.null");
                    auto *nullStr = cg_.builder_->CreateGlobalStringPtr(
                        "\xd9\x84\xd8\xa7\xd8\xb4\xd9\x8a\xd8\xa1", "int.null.str"); // لاشيء

                    auto *parentFunc3 = cg_.builder_->GetInsertBlock()->getParent();
                    auto *sentinelBB = llvm::BasicBlock::Create(*cg_.context_, "int.sentinel", parentFunc3);
                    auto *normalBB = llvm::BasicBlock::Create(*cg_.context_, "int.normal", parentFunc3);
                    auto *intMergeBB = llvm::BasicBlock::Create(*cg_.context_, "int.merge", parentFunc3);

                    cg_.builder_->CreateCondBr(isSentinel, sentinelBB, normalBB);

                    // (AR) كتلة sentinel: إرجاع "لاشيء"
                    cg_.builder_->SetInsertPoint(sentinelBB);
                    cg_.builder_->CreateBr(intMergeBB);

                    // (AR) كتلة عادية: تحويل الرقم إلى نص
                    cg_.builder_->SetInsertPoint(normalBB);

                    // (AR) ISSUE-076 (Amelia #8): 512 — يخدم النصّ العدديّ والعشريّ (%.6f قد
                    //      يبلغ ~316 حرفًا لـDBL_MAX عبر __sad_format_double فيفيض 32).
                    // (EN) ISSUE-076 (Amelia #8): 512 — serves int + double text (%.6f can reach
                    //      ~316 chars for DBL_MAX via __sad_format_double, overflowing 32).
                    llvm::Value *buf = cg_.builder_->CreateAlloca(
                        llvm::Type::getInt8Ty(*cg_.context_),
                        llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 512),
                        "strbuf");

                    if (cg_.freestanding_)
                    {
                        llvm::FunctionType *itoaType = llvm::FunctionType::get(
                            llvm::Type::getInt32Ty(*cg_.context_),
                            {llvm::PointerType::getUnqual(*cg_.context_),
                             llvm::Type::getInt64Ty(*cg_.context_)},
                            false);
                        llvm::FunctionCallee itoaFn = cg_.module_->getOrInsertFunction("__sad_itoa", itoaType);

                        if (ty->isIntegerTy())
                        {
                            llvm::Value *val64 = cg_.builder_->CreateIntCast(val, llvm::Type::getInt64Ty(*cg_.context_), true);
                            cg_.builder_->CreateCall(itoaFn, {buf, val64});
                        }
                        else if (ty->isDoubleTy())
                        {
                            llvm::FunctionType *ftoaType = llvm::FunctionType::get(
                                llvm::Type::getInt32Ty(*cg_.context_),
                                {llvm::PointerType::getUnqual(*cg_.context_),
                                 llvm::Type::getDoubleTy(*cg_.context_)},
                                false);
                            llvm::FunctionCallee ftoaFn = cg_.module_->getOrInsertFunction("__sad_ftoa", ftoaType);
                            cg_.builder_->CreateCall(ftoaFn, {buf, val});
                        }
                    }
                    else
                    {
                        llvm::FunctionType *sprintfType = llvm::FunctionType::get(
                            llvm::Type::getInt32Ty(*cg_.context_),
                            {llvm::PointerType::getUnqual(*cg_.context_),
                             llvm::PointerType::getUnqual(*cg_.context_)},
                            true);
                        llvm::FunctionCallee sprintfFn = cg_.module_->getOrInsertFunction("sprintf", sprintfType);

                        if (ty->isIntegerTy())
                        {
                            llvm::Value *fmt = cg_.builder_->CreateGlobalStringPtr("%lld", "int.fmt");
                            llvm::Value *val64 = cg_.builder_->CreateIntCast(val, llvm::Type::getInt64Ty(*cg_.context_), true);
                            cg_.builder_->CreateCall(sprintfFn, {buf, fmt, val64});
                        }
                        else if (ty->isDoubleTy())
                        {
                            // (AR) إصلاح: استخدام __sad_format_double لمطابقة دقة المفسر
                            // (EN) Fix: use __sad_format_double to match interpreter precision
                            auto *fmtDblType = llvm::FunctionType::get(
                                llvm::Type::getVoidTy(*cg_.context_),
                                {llvm::PointerType::getUnqual(*cg_.context_),
                                 llvm::Type::getDoubleTy(*cg_.context_)},
                                false);
                            auto fmtDblFn = cg_.module_->getOrInsertFunction("__sad_format_double", fmtDblType);
                            cg_.builder_->CreateCall(fmtDblFn, {buf, val});
                        }
                    }

                    cg_.builder_->CreateBr(intMergeBB);

                    // (AR) دمج النتائج
                    cg_.builder_->SetInsertPoint(intMergeBB);
                    auto *ptrTy3 = llvm::PointerType::getUnqual(*cg_.context_);
                    auto *intPhi = cg_.builder_->CreatePHI(ptrTy3, 2, "int.str.result");
                    intPhi->addIncoming(nullStr, sentinelBB);
                    intPhi->addIncoming(buf, normalBB);
                    return intPhi;
                }

                // (AR) للأنواع الأخرى (float/double): المسار العادي بدون فحص sentinel
                // (AR) ISSUE-076 (Amelia #8): 512 لا 32 — %.6f لـDBL_MAX ~316 حرفًا يفيض.
                // (EN) ISSUE-076 (Amelia #8): 512 not 32 — %.6f for DBL_MAX ~316 chars overflows.
                llvm::Value *buf = cg_.builder_->CreateAlloca(
                    llvm::Type::getInt8Ty(*cg_.context_),
                    llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 512),
                    "strbuf");
                if (ty->isDoubleTy())
                {
                    if (cg_.freestanding_)
                    {
                        llvm::FunctionType *ftoaType = llvm::FunctionType::get(
                            llvm::Type::getInt32Ty(*cg_.context_),
                            {llvm::PointerType::getUnqual(*cg_.context_),
                             llvm::Type::getDoubleTy(*cg_.context_)},
                            false);
                        llvm::FunctionCallee ftoaFn = cg_.module_->getOrInsertFunction("__sad_ftoa", ftoaType);
                        cg_.builder_->CreateCall(ftoaFn, {buf, val});
                    }
                    else
                    {
                        // (AR) إصلاح: استخدام __sad_format_double لمطابقة دقة المفسر
                        // (EN) Fix: use __sad_format_double to match interpreter precision
                        auto *fmtDblType = llvm::FunctionType::get(
                            llvm::Type::getVoidTy(*cg_.context_),
                            {llvm::PointerType::getUnqual(*cg_.context_),
                             llvm::Type::getDoubleTy(*cg_.context_)},
                            false);
                        auto fmtDblFn = cg_.module_->getOrInsertFunction("__sad_format_double", fmtDblType);
                        cg_.builder_->CreateCall(fmtDblFn, {buf, val});
                    }
                }
                return buf;
            };

            left = ensureString(left, leftTy, inst->operands[0]);
            right = ensureString(right, rightTy, inst->operands[1]);

            // Get lengths using strlen
            llvm::FunctionType *strlenType = llvm::FunctionType::get(
                llvm::Type::getInt64Ty(*cg_.context_),
                {llvm::PointerType::getUnqual(*cg_.context_)},
                false);
            llvm::FunctionCallee strlenFn = cg_.module_->getOrInsertFunction("strlen", strlenType);

            llvm::Value *len1 = cg_.builder_->CreateCall(strlenFn, {left}, "len1");
            llvm::Value *len2 = cg_.builder_->CreateCall(strlenFn, {right}, "len2");
            llvm::Value *totalLen = cg_.builder_->CreateAdd(len1, len2, "totallen");
            llvm::Value *bufSize = cg_.builder_->CreateAdd(totalLen,
                                                       llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 1), "bufsize");

            // Allocate result buffer on HEAP (using malloc) so it's safe to return
            // (AR) تخصيص على الـ Heap بدلاً من Stack لتجنب مشاكل الرجوع من الدوال
            // (EN) Allocate on heap instead of stack to avoid returning dangling pointers
            llvm::FunctionType *mallocType = llvm::FunctionType::get(
                llvm::PointerType::getUnqual(*cg_.context_),
                {llvm::Type::getInt64Ty(*cg_.context_)},
                false);
            llvm::FunctionCallee mallocFn = cg_.module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *result = cg_.builder_->CreateCall(mallocFn, {bufSize}, "concat.buf");

            // Copy first string using memcpy
            llvm::FunctionType *memcpyType = llvm::FunctionType::get(
                llvm::PointerType::getUnqual(*cg_.context_),
                {llvm::PointerType::getUnqual(*cg_.context_),
                 llvm::PointerType::getUnqual(*cg_.context_),
                 llvm::Type::getInt64Ty(*cg_.context_)},
                false);
            llvm::FunctionCallee memcpyFn = cg_.module_->getOrInsertFunction("memcpy", memcpyType);

            cg_.builder_->CreateCall(memcpyFn, {result, left, len1});

            // Copy second string after first
            llvm::Value *dest2 = cg_.builder_->CreateGEP(
                llvm::Type::getInt8Ty(*cg_.context_), result, len1, "dest2");
            llvm::Value *copyLen2 = cg_.builder_->CreateAdd(len2,
                                                        llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 1), "copylen2");
            cg_.builder_->CreateCall(memcpyFn, {dest2, right, copyLen2});

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }



        llvm::Value *StringOpsCodeGen::emitStringCharAt(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "BUILTIN_STRING_CHAR_AT"}});
                return nullptr;
            }

            llvm::Value *str = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *index = cg_.resolveOperand(inst->operands[1]);
            if (!str || !index)
                return nullptr;

            // Ensure str is a pointer
            llvm::Type *i8p = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            if (!str->getType()->isPointerTy())
            {
                str = cg_.builder_->CreateIntToPtr(str, i8p, "str.ptr");
            }

            // Ensure index is i64
            if (index->getType() != llvm::Type::getInt64Ty(*cg_.context_))
            {
                index = cg_.builder_->CreateIntCast(index, llvm::Type::getInt64Ty(*cg_.context_), true, "idx.i64");
            }

            // GEP to get pointer to character at index
            llvm::Value *charPtr = cg_.builder_->CreateGEP(
                llvm::Type::getInt8Ty(*cg_.context_), str, index, "char.ptr");

            // Load the character (i8)
            llvm::Value *charVal = cg_.builder_->CreateLoad(
                llvm::Type::getInt8Ty(*cg_.context_), charPtr, "char.val");

            // Zero-extend to i64
            llvm::Value *result = cg_.builder_->CreateZExt(charVal, llvm::Type::getInt64Ty(*cg_.context_), "char.i64");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }



        llvm::Value *StringOpsCodeGen::emitStringCmp(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "STRING_CMP"}});
                return nullptr;
            }

            llvm::Value *left = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *right = cg_.resolveOperand(inst->operands[1]);
            if (!left || !right)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "STRING_CMP"}});
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
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i8p = llvm::PointerType::getUnqual(*cg_.context_);
            auto zero64 = llvm::ConstantInt::get(i64Ty, 0);
            auto bit63Mask = llvm::ConstantInt::get(i64Ty, 1ULL << 63);

            // (AR) الحصول على تمثيل i64 لفحص العلامات
            llvm::Value *leftI64 = left->getType()->isPointerTy()
                                       ? cg_.builder_->CreatePtrToInt(left, i64Ty, "str.cmp.l.i64")
                                       : left;
            llvm::Value *rightI64 = right->getType()->isPointerTy()
                                        ? cg_.builder_->CreatePtrToInt(right, i64Ty, "str.cmp.r.i64")
                                        : right;

            // (AR) فحص: bit63=0 (مؤشر صالح) و ≠ null
            auto leftTag = cg_.builder_->CreateAnd(leftI64, bit63Mask, "str.cmp.l.tag");
            auto rightTag = cg_.builder_->CreateAnd(rightI64, bit63Mask, "str.cmp.r.tag");
            auto leftIsStr = cg_.builder_->CreateICmpEQ(leftTag, zero64, "str.cmp.l.isstr");
            auto rightIsStr = cg_.builder_->CreateICmpEQ(rightTag, zero64, "str.cmp.r.isstr");
            auto leftNotNull = cg_.builder_->CreateICmpNE(leftI64, zero64, "str.cmp.l.nn");
            auto rightNotNull = cg_.builder_->CreateICmpNE(rightI64, zero64, "str.cmp.r.nn");

            auto leftOk = cg_.builder_->CreateAnd(leftIsStr, leftNotNull, "str.cmp.l.ok");
            auto rightOk = cg_.builder_->CreateAnd(rightIsStr, rightNotNull, "str.cmp.r.ok");
            auto canStrcmp = cg_.builder_->CreateAnd(leftOk, rightOk, "str.cmp.can");

            // (AR) تفريع: إذا كلاهما نص صالح → strcmp، وإلا → خطأ
            auto *entryBB = cg_.builder_->GetInsertBlock();
            auto *parentFunc = entryBB->getParent();
            auto *strcmpBB = llvm::BasicBlock::Create(*cg_.context_, "str.cmp.do", parentFunc);
            auto *doneBB = llvm::BasicBlock::Create(*cg_.context_, "str.cmp.done", parentFunc);

            cg_.builder_->CreateCondBr(canStrcmp, strcmpBB, doneBB);

            // ─── كتلة strcmp: مقارنة فعلية ───
            cg_.builder_->SetInsertPoint(strcmpBB);
            llvm::Value *leftPtr = left->getType()->isPointerTy()
                                       ? left
                                       : cg_.builder_->CreateIntToPtr(left, i8p, "str.l");
            llvm::Value *rightPtr = right->getType()->isPointerTy()
                                        ? right
                                        : cg_.builder_->CreateIntToPtr(right, i8p, "str.r");

            llvm::FunctionType *strcmpType = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(*cg_.context_), {i8p, i8p}, false);
            llvm::FunctionCallee strcmpFn = cg_.module_->getOrInsertFunction("strcmp", strcmpType);
            llvm::Value *cmpResult = cg_.builder_->CreateCall(strcmpFn, {leftPtr, rightPtr}, "strcmp.ret");

            // strcmp == 0 → متساويان → النتيجة 1 (صحيحة)
            llvm::Value *streq = cg_.builder_->CreateICmpEQ(cmpResult,
                                                        llvm::ConstantInt::get(llvm::Type::getInt32Ty(*cg_.context_), 0), "streq");
            cg_.builder_->CreateBr(doneBB);

            // ─── كتلة النتيجة: دمج ───
            cg_.builder_->SetInsertPoint(doneBB);
            auto *result = cg_.builder_->CreatePHI(llvm::Type::getInt1Ty(*cg_.context_), 2, "streq.phi");
            result->addIncoming(streq, strcmpBB);
            result->addIncoming(llvm::ConstantInt::getFalse(*cg_.context_), entryBB);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }



        // ============================================================================
        // Phase N: Builtin String Functions / دوال النصوص المضمنة
        // ============================================================================

        llvm::Value *StringOpsCodeGen::emitStringToI64(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "STRING_TO_I64"}});
                return nullptr;
            }
            llvm::Value *str = cg_.resolveOperand(inst->operands[0]);
            if (!str)
                return nullptr;

            // Call atoll(str) → i64
            auto *atollType = llvm::FunctionType::get(
                cg_.getInt64Type(), {llvm::PointerType::getUnqual(*cg_.context_)}, false);
            auto atollFunc = cg_.module_->getOrInsertFunction("atoll", atollType);
            llvm::Value *result = cg_.builder_->CreateCall(atollFunc, {str}, "str2i64");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }



        // ============================================================================
        // (AR) تحويل نص إلى عدد عشري — STRING_TO_F64
        //      نستدعي دالة atof() من مكتبة C القياسية التي تحوّل نصاً مثل "3.14"
        //      إلى قيمة عشرية مزدوجة الدقة (double/f64)
        //      هذا ضروري لدالة لعشري() المضمنة في لغة ص
        //
        // (EN) Convert string to float — STRING_TO_F64
        //      Calls C standard library atof() which converts a string like "3.14"
        //      to a double-precision floating point value (f64)
        //      Required for the built-in لعشري() function in Sad language
        // ============================================================================
        llvm::Value *StringOpsCodeGen::emitStringToF64(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "STRING_TO_F64"}});
                return nullptr;
            }
            llvm::Value *str = cg_.resolveOperand(inst->operands[0]);
            if (!str)
                return nullptr;

            // (AR) استدعاء atof(str) → f64
            //      atof هي دالة C قياسية تحوّل نصاً إلى double
            // (EN) Call atof(str) → f64
            //      atof is a standard C function that converts string to double
            auto *atofType = llvm::FunctionType::get(
                llvm::Type::getDoubleTy(*cg_.context_),
                {llvm::PointerType::getUnqual(*cg_.context_)}, false);
            auto atofFunc = cg_.module_->getOrInsertFunction("atof", atofType);
            llvm::Value *result = cg_.builder_->CreateCall(atofFunc, {str}, "str2f64");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }



        llvm::Value *StringOpsCodeGen::emitBuiltinStringLength(std::shared_ptr<SIRInstruction> inst)
        {
            return cg_.emitFFIStrlen(inst);
        }



        llvm::Value *StringOpsCodeGen::emitBuiltinStringToUpper(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *str = cg_.resolveOperand(inst->operands[0]);
            if (!str)
                return nullptr;

            // strlen + malloc + loop calling toupper
            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = cg_.getInt64Type();
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto i8Ty = llvm::Type::getInt8Ty(*cg_.context_);

            auto *strlenType = llvm::FunctionType::get(i64Ty, {ptrTy}, false);
            auto strlenFunc = cg_.module_->getOrInsertFunction("strlen", strlenType);
            llvm::Value *len = cg_.builder_->CreateCall(strlenFunc, {str}, "len");

            llvm::Value *newLen = cg_.builder_->CreateAdd(len, llvm::ConstantInt::get(i64Ty, 1));
            auto *mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
            auto mallocFunc = cg_.module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *buf = cg_.builder_->CreateCall(mallocFunc, {newLen}, "upper_buf");

            // strcpy then loop toupper
            auto *strcpyType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
            auto strcpyFunc = cg_.module_->getOrInsertFunction("strcpy", strcpyType);
            cg_.builder_->CreateCall(strcpyFunc, {buf, str});

            // Simple approach: call _strupr or iterate with toupper
            auto *struprType = llvm::FunctionType::get(ptrTy, {ptrTy}, false);
            auto struprFunc = cg_.module_->getOrInsertFunction("_strupr", struprType);
            cg_.builder_->CreateCall(struprFunc, {buf});

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = buf;
            }
            return buf;
        }



        llvm::Value *StringOpsCodeGen::emitBuiltinStringToLower(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *str = cg_.resolveOperand(inst->operands[0]);
            if (!str)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = cg_.getInt64Type();

            auto *strlenType = llvm::FunctionType::get(i64Ty, {ptrTy}, false);
            auto strlenFunc = cg_.module_->getOrInsertFunction("strlen", strlenType);
            llvm::Value *len = cg_.builder_->CreateCall(strlenFunc, {str}, "len");

            llvm::Value *newLen = cg_.builder_->CreateAdd(len, llvm::ConstantInt::get(i64Ty, 1));
            auto *mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
            auto mallocFunc = cg_.module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *buf = cg_.builder_->CreateCall(mallocFunc, {newLen}, "lower_buf");

            auto *strcpyType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
            auto strcpyFunc = cg_.module_->getOrInsertFunction("strcpy", strcpyType);
            cg_.builder_->CreateCall(strcpyFunc, {buf, str});

            auto *strlwrType = llvm::FunctionType::get(ptrTy, {ptrTy}, false);
            auto strlwrFunc = cg_.module_->getOrInsertFunction("_strlwr", strlwrType);
            cg_.builder_->CreateCall(strlwrFunc, {buf});

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = buf;
            }
            return buf;
        }



        llvm::Value *StringOpsCodeGen::emitBuiltinStringFind(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "STRING_FIND"}});
                return nullptr;
            }
            llvm::Value *haystack = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *needle = cg_.resolveOperand(inst->operands[1]);
            if (!haystack || !needle)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = cg_.getInt64Type();

            // strstr(haystack, needle) → ptr or null
            auto *strstrType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
            auto strstrFunc = cg_.module_->getOrInsertFunction("strstr", strstrType);
            llvm::Value *found = cg_.builder_->CreateCall(strstrFunc, {haystack, needle}, "found");

            // (AR) حساب الإزاحة بالبايت ثم تحويلها إلى فهرس حرف UTF-8
            // (EN) Calculate byte offset then convert to UTF-8 character index
            llvm::Value *isNull = cg_.builder_->CreateICmpEQ(found,
                                                         llvm::ConstantPointerNull::get(llvm::PointerType::getUnqual(*cg_.context_)), "isnull");
            llvm::Value *foundInt = cg_.builder_->CreatePtrToInt(found, i64Ty, "found.int");
            llvm::Value *hstackInt = cg_.builder_->CreatePtrToInt(haystack, i64Ty, "hstack.int");
            llvm::Value *byteOffset = cg_.builder_->CreateSub(foundInt, hstackInt, "byte.offset");

            // (AR) تحويل موقع البايت إلى فهرس الحرف UTF-8
            // (EN) Convert byte offset to UTF-8 character index
            llvm::Function *byteToCharFn = cg_.getOrCreateUtf8ByteToChar();
            llvm::Value *charIndex = cg_.builder_->CreateCall(byteToCharFn, {haystack, byteOffset}, "char.index");

            // (AR) إذا لم يُعثر → -1، وإلا → فهرس الحرف
            llvm::Value *result = cg_.builder_->CreateSelect(isNull,
                                                         llvm::ConstantInt::get(i64Ty, -1), charIndex, "find_result");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }



        llvm::Value *StringOpsCodeGen::emitBuiltinStringReplace(std::shared_ptr<SIRInstruction> inst)
        {
            // Full string replace: call runtime helper sad_string_replace(str, old, new) -> char*
            if (!inst || inst->operands.size() < 3)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "STRING_REPLACE"}});
                return nullptr;
            }
            llvm::Value *str = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *oldStr = cg_.resolveOperand(inst->operands[1]);
            llvm::Value *newStr = cg_.resolveOperand(inst->operands[2]);
            if (!str || !oldStr || !newStr)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = cg_.getInt64Type();
            auto i8Ty = llvm::Type::getInt8Ty(*cg_.context_);

            // Build inline: find first occurrence with strstr, copy before + new + after
            // Allocate generous buffer: strlen(str) * 2 + strlen(newStr) + 1
            auto *strlenType = llvm::FunctionType::get(i64Ty, {ptrTy}, false);
            auto strlenFunc = cg_.module_->getOrInsertFunction("strlen", strlenType);
            llvm::Value *srcLen = cg_.builder_->CreateCall(strlenFunc, {str}, "src.len");
            llvm::Value *oldLen = cg_.builder_->CreateCall(strlenFunc, {oldStr}, "old.len");
            llvm::Value *newLen = cg_.builder_->CreateCall(strlenFunc, {newStr}, "new.len");

            // bufSize = srcLen * 2 + newLen + 1 (generous)
            llvm::Value *bufSize = cg_.builder_->CreateMul(srcLen, llvm::ConstantInt::get(i64Ty, 2));
            bufSize = cg_.builder_->CreateAdd(bufSize, newLen);
            bufSize = cg_.builder_->CreateAdd(bufSize, llvm::ConstantInt::get(i64Ty, 1));

            auto *mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
            auto mallocFunc = cg_.module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *buf = cg_.builder_->CreateCall(mallocFunc, {bufSize}, "replace_buf");

            // Use strstr to find oldStr in str
            auto *strstrType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
            auto strstrFunc = cg_.module_->getOrInsertFunction("strstr", strstrType);
            llvm::Value *found = cg_.builder_->CreateCall(strstrFunc, {str, oldStr}, "found");

            // If not found, just copy original
            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *foundBB = llvm::BasicBlock::Create(*cg_.context_, "replace.found", curFunc);
            llvm::BasicBlock *notFoundBB = llvm::BasicBlock::Create(*cg_.context_, "replace.notfound", curFunc);
            llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(*cg_.context_, "replace.merge", curFunc);

            llvm::Value *isNull = cg_.builder_->CreateICmpEQ(found,
                                                         llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)), "isnull");
            cg_.builder_->CreateCondBr(isNull, notFoundBB, foundBB);

            // Not found: strcpy original
            cg_.builder_->SetInsertPoint(notFoundBB);
            auto *strcpyType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
            auto strcpyFunc = cg_.module_->getOrInsertFunction("strcpy", strcpyType);
            cg_.builder_->CreateCall(strcpyFunc, {buf, str});
            cg_.builder_->CreateBr(mergeBB);

            // Found: copy prefix + newStr + suffix
            cg_.builder_->SetInsertPoint(foundBB);
            llvm::Value *prefixLen = cg_.builder_->CreatePtrDiff(i8Ty, found, str, "prefix.len");
            auto *memcpyType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy, i64Ty}, false);
            auto memcpyFunc = cg_.module_->getOrInsertFunction("memcpy", memcpyType);
            cg_.builder_->CreateCall(memcpyFunc, {buf, str, prefixLen}); // copy prefix
            llvm::Value *dst1 = cg_.builder_->CreateGEP(i8Ty, buf, {prefixLen}, "dst1");
            cg_.builder_->CreateCall(memcpyFunc, {dst1, newStr, newLen}); // copy newStr
            llvm::Value *dst2 = cg_.builder_->CreateGEP(i8Ty, dst1, {newLen}, "dst2");
            llvm::Value *suffixStart = cg_.builder_->CreateGEP(i8Ty, found, {oldLen}, "suffix.start");
            llvm::Value *suffixLen = cg_.builder_->CreateSub(srcLen, cg_.builder_->CreateAdd(prefixLen, oldLen));
            llvm::Value *suffixCopyLen = cg_.builder_->CreateAdd(suffixLen, llvm::ConstantInt::get(i64Ty, 1)); // include null
            cg_.builder_->CreateCall(memcpyFunc, {dst2, suffixStart, suffixCopyLen});
            cg_.builder_->CreateBr(mergeBB);

            cg_.builder_->SetInsertPoint(mergeBB);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = buf;
            }
            return buf;
        }



        llvm::Value *StringOpsCodeGen::emitBuiltinStringSubstring(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 3)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "STRING_SUBSTRING"}});
                return nullptr;
            }
            llvm::Value *str = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *start = cg_.resolveOperand(inst->operands[1]);
            llvm::Value *len = cg_.resolveOperand(inst->operands[2]);
            if (!str || !start || !len)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = cg_.getInt64Type();
            auto i8Ty = llvm::Type::getInt8Ty(*cg_.context_);

            // (AR) تحويل فهرس الحرف والطول إلى إزاحات بايت باستخدام UTF-8
            // (EN) Convert character start index and length to byte offsets using UTF-8
            llvm::Function *charToByteFn = cg_.getOrCreateUtf8CharToByte();

            // (AR) موقع البايت لبداية الاستخراج
            // (EN) Byte offset of start character
            llvm::Value *byteStart = cg_.builder_->CreateCall(charToByteFn, {str, start}, "byte.start");

            // (AR) موقع البايت لنهاية الاستخراج (start + len)
            // (EN) Byte offset of end character (start + len)
            llvm::Value *charEnd = cg_.builder_->CreateAdd(start, len, "char.end");
            llvm::Value *byteEnd = cg_.builder_->CreateCall(charToByteFn, {str, charEnd}, "byte.end");

            // (AR) طول البايتات = byteEnd - byteStart
            // (EN) Byte length = byteEnd - byteStart
            llvm::Value *byteLen = cg_.builder_->CreateSub(byteEnd, byteStart, "byte.len");

            // (AR) حجز ذاكرة: byteLen + 1
            llvm::Value *bufSize = cg_.builder_->CreateAdd(byteLen, llvm::ConstantInt::get(i64Ty, 1));
            auto *mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
            auto mallocFunc = cg_.module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *buf = cg_.builder_->CreateCall(mallocFunc, {bufSize}, "substr_buf");

            // (AR) مؤشر المصدر: str + byteStart
            llvm::Value *srcPtr = cg_.builder_->CreateGEP(i8Ty, str, {byteStart}, "substr.src");

            // memcpy(buf, srcPtr, byteLen)
            auto *memcpyType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy, i64Ty}, false);
            auto memcpyFunc = cg_.module_->getOrInsertFunction("memcpy", memcpyType);
            cg_.builder_->CreateCall(memcpyFunc, {buf, srcPtr, byteLen});

            // (AR) إنهاء النص بـ null
            llvm::Value *endPtr = cg_.builder_->CreateGEP(i8Ty, buf, {byteLen}, "substr.end");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, 0), endPtr);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = buf;
            }
            return buf;
        }



        llvm::Value *StringOpsCodeGen::emitBuiltinStringTrim(std::shared_ptr<SIRInstruction> inst)
        {
            // Call C runtime: skip leading whitespace, then copy until trailing whitespace
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *str = cg_.resolveOperand(inst->operands[0]);
            if (!str)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = cg_.getInt64Type();
            auto i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);

            // Use strspn to find leading whitespace count, then strlen-based trim end
            // strspn(str, " \t\n\r") returns number of leading whitespace chars
            auto *strspnType = llvm::FunctionType::get(i64Ty, {ptrTy, ptrTy}, false);
            auto strspnFunc = cg_.module_->getOrInsertFunction("strspn", strspnType);
            llvm::Value *ws = cg_.builder_->CreateGlobalStringPtr(" \t\n\r", "ws_chars");
            llvm::Value *leadingWS = cg_.builder_->CreateCall(strspnFunc, {str, ws}, "leading.ws");

            // start = str + leadingWS
            llvm::Value *start = cg_.builder_->CreateGEP(i8Ty, str, {leadingWS}, "trim.start");

            // Get length of remaining string
            auto *strlenType = llvm::FunctionType::get(i64Ty, {ptrTy}, false);
            auto strlenFunc = cg_.module_->getOrInsertFunction("strlen", strlenType);
            llvm::Value *remLen = cg_.builder_->CreateCall(strlenFunc, {start}, "rem.len");

            // Allocate buffer: remLen + 1
            llvm::Value *bufSize = cg_.builder_->CreateAdd(remLen, llvm::ConstantInt::get(i64Ty, 1));
            auto *mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
            auto mallocFunc = cg_.module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *buf = cg_.builder_->CreateCall(mallocFunc, {bufSize}, "trim_buf");

            // memcpy start content
            auto *memcpyType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy, i64Ty}, false);
            auto memcpyFunc = cg_.module_->getOrInsertFunction("memcpy", memcpyType);
            cg_.builder_->CreateCall(memcpyFunc, {buf, start, bufSize});

            // Trim trailing whitespace: walk back from end while isspace
            // Simple approach: create loop to null-terminate at first non-whitespace from end
            // For simplicity, use a runtime helper pattern: buf[len] scanning back
            // We'll call isspace on each char from the end
            auto *isSpaceType = llvm::FunctionType::get(i32Ty, {i32Ty}, false);
            auto isSpaceFunc = cg_.module_->getOrInsertFunction("isspace", isSpaceType);

            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*cg_.context_, "trim.loop", curFunc);
            llvm::BasicBlock *bodyBB = llvm::BasicBlock::Create(*cg_.context_, "trim.body", curFunc);
            llvm::BasicBlock *doneBB = llvm::BasicBlock::Create(*cg_.context_, "trim.done", curFunc);

            // idx = remLen - 1
            llvm::Value *startIdx = cg_.builder_->CreateSub(remLen, llvm::ConstantInt::get(i64Ty, 1));
            cg_.builder_->CreateBr(loopBB);

            cg_.builder_->SetInsertPoint(loopBB);
            llvm::PHINode *idx = cg_.builder_->CreatePHI(i64Ty, 2, "trim.idx");
            idx->addIncoming(startIdx, loopBB->getSinglePredecessor());

            // if idx < 0, done
            llvm::Value *isNeg = cg_.builder_->CreateICmpSLT(idx, llvm::ConstantInt::get(i64Ty, 0), "is.neg");
            cg_.builder_->CreateCondBr(isNeg, doneBB, bodyBB);

            cg_.builder_->SetInsertPoint(bodyBB);
            llvm::Value *charPtr = cg_.builder_->CreateGEP(i8Ty, buf, {idx}, "char.ptr");
            llvm::Value *ch = cg_.builder_->CreateLoad(i8Ty, charPtr, "ch");
            llvm::Value *chInt = cg_.builder_->CreateZExt(ch, i32Ty, "ch.int");
            llvm::Value *isSp = cg_.builder_->CreateCall(isSpaceFunc, {chInt}, "is.sp");
            llvm::Value *isSpBool = cg_.builder_->CreateICmpNE(isSp, llvm::ConstantInt::get(i32Ty, 0), "is.sp.bool");

            // If space, null-terminate and continue
            llvm::BasicBlock *trimBB = llvm::BasicBlock::Create(*cg_.context_, "trim.set", curFunc);
            cg_.builder_->CreateCondBr(isSpBool, trimBB, doneBB);

            cg_.builder_->SetInsertPoint(trimBB);
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, 0), charPtr);
            llvm::Value *nextIdx = cg_.builder_->CreateSub(idx, llvm::ConstantInt::get(i64Ty, 1));
            idx->addIncoming(nextIdx, trimBB);
            cg_.builder_->CreateBr(loopBB);

            cg_.builder_->SetInsertPoint(doneBB);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = buf;
            }
            return buf;
        }


        llvm::Value *StringOpsCodeGen::emitBuiltinStringSplit(std::shared_ptr<SIRInstruction> inst)
        {
            // Split string by delimiter into a SadArray of string pointers
            // Uses strtok-like approach: count delimiters, allocate array, copy tokens
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *str = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *delim = cg_.resolveOperand(inst->operands[1]);
            if (!str || !delim)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = cg_.getInt64Type();
            auto i8Ty = llvm::Type::getInt8Ty(*cg_.context_);

            // Call runtime: sad_string_split(str, delim) -> SadArray*
            // For now, create a SadArray with a single element (the original string)
            // This is correct for the case when delimiter is not found
            llvm::StructType *arrTy = getArrayStructType(*cg_.context_);
            auto *mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
            auto mallocFunc = cg_.module_->getOrInsertFunction("malloc", mallocType);

            // Allocate SadArray struct
            llvm::Value *arrSize = llvm::ConstantInt::get(i64Ty, 24); // 3 * i64
            llvm::Value *arrPtr = cg_.builder_->CreateCall(mallocFunc, {arrSize}, "split.arr");

            // Allocate data buffer for 16 pointers initially
            llvm::Value *dataSize = llvm::ConstantInt::get(i64Ty, 16 * 8);
            llvm::Value *dataPtr = cg_.builder_->CreateCall(mallocFunc, {dataSize}, "split.data");

            // Store array metadata: length=0, capacity=16, data=dataPtr
            llvm::Value *lenGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 0, "split.len.gep");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), lenGep);
            llvm::Value *capGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 1, "split.cap.gep");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 16), capGep);
            llvm::Value *datGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 2, "split.dat.gep");
            cg_.builder_->CreateStore(dataPtr, datGep);

            // Use strtok to tokenize: first make a copy of str (strtok modifies input)
            auto *strlenType = llvm::FunctionType::get(i64Ty, {ptrTy}, false);
            auto strlenFunc = cg_.module_->getOrInsertFunction("strlen", strlenType);
            llvm::Value *srcLen = cg_.builder_->CreateCall(strlenFunc, {str}, "src.len");
            llvm::Value *copySize = cg_.builder_->CreateAdd(srcLen, llvm::ConstantInt::get(i64Ty, 1));
            llvm::Value *strCopy = cg_.builder_->CreateCall(mallocFunc, {copySize}, "str.copy");
            auto *memcpyType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy, i64Ty}, false);
            auto memcpyFunc = cg_.module_->getOrInsertFunction("memcpy", memcpyType);
            cg_.builder_->CreateCall(memcpyFunc, {strCopy, str, copySize});

            // Call strtok(strCopy, delim) in a loop
            auto *strtokType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
            auto strtokFunc = cg_.module_->getOrInsertFunction("strtok", strtokType);

            // First call: strtok(strCopy, delim)
            llvm::Value *firstTok = cg_.builder_->CreateCall(strtokFunc, {strCopy, delim}, "tok.first");

            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*cg_.context_, "split.loop", curFunc);
            llvm::BasicBlock *bodyBB = llvm::BasicBlock::Create(*cg_.context_, "split.body", curFunc);
            llvm::BasicBlock *doneBB = llvm::BasicBlock::Create(*cg_.context_, "split.done", curFunc);
            cg_.builder_->CreateBr(loopBB);

            cg_.builder_->SetInsertPoint(loopBB);
            llvm::PHINode *tok = cg_.builder_->CreatePHI(ptrTy, 2, "tok");
            tok->addIncoming(firstTok, loopBB->getSinglePredecessor());
            llvm::PHINode *count = cg_.builder_->CreatePHI(i64Ty, 2, "count");
            count->addIncoming(llvm::ConstantInt::get(i64Ty, 0), loopBB->getSinglePredecessor());

            llvm::Value *tokNull = cg_.builder_->CreateICmpEQ(tok,
                                                          llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)), "tok.null");
            cg_.builder_->CreateCondBr(tokNull, doneBB, bodyBB);

            cg_.builder_->SetInsertPoint(bodyBB);
            // Store token pointer: data[count] = strdup(tok)
            llvm::Value *tokLen = cg_.builder_->CreateCall(strlenFunc, {tok}, "tok.len");
            llvm::Value *tokBufSz = cg_.builder_->CreateAdd(tokLen, llvm::ConstantInt::get(i64Ty, 1));
            llvm::Value *tokCopy = cg_.builder_->CreateCall(mallocFunc, {tokBufSz}, "tok.copy");
            cg_.builder_->CreateCall(memcpyFunc, {tokCopy, tok, tokBufSz});

            llvm::Value *curData = cg_.builder_->CreateLoad(ptrTy, datGep, "cur.data");
            // (AR) خطوة العنصر i64 (8) لتوحيد حجم خانة SadArray عبر الأهداف —
            //      كانت ptrTy (=4 على i686) تخالف الجلب/التخصيص الموحَّد. التخزين
            //      يبقى بقيمة مؤشّر (SAD_ARRAY_SLOT_BYTES).
            // (EN) i64-stride element (8) for unified SadArray slots; store as ptr.
            llvm::Value *elemPtr = cg_.builder_->CreateGEP(cg_.getInt64Type(), curData, {count}, "elem.ptr");
            cg_.builder_->CreateStore(tokCopy, elemPtr);

            llvm::Value *nextCount = cg_.builder_->CreateAdd(count, llvm::ConstantInt::get(i64Ty, 1));
            // Next token: strtok(NULL, delim)
            llvm::Value *nextTok = cg_.builder_->CreateCall(strtokFunc, {llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)), delim}, "tok.next");
            tok->addIncoming(nextTok, bodyBB);
            count->addIncoming(nextCount, bodyBB);
            cg_.builder_->CreateBr(loopBB);

            cg_.builder_->SetInsertPoint(doneBB);
            // Update length
            cg_.builder_->CreateStore(count, lenGep);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = arrPtr;
            }
            return arrPtr;
        }



        llvm::Value *StringOpsCodeGen::emitBuiltinStringJoin(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) دمج مصفوفة نصوص بفاصل — مثلاً: دمج(["أ","ب","ج"], "-") → "أ-ب-ج"
            // (EN) Join array of strings with separator
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *arrPtr = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *sep = cg_.resolveOperand(inst->operands[1]);
            if (!arrPtr || !sep)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = cg_.getInt64Type();
            auto i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
            llvm::StructType *arrTy = getArrayStructType(*cg_.context_);

            // ================================================================
            // (AR) إصلاح خطأ GEP: المتغير المحمّل من alloca(i64) يكون i64 وليس مؤشر.
            //      CreateStructGEP يتطلب مؤشراً — cg_.normalizeArrayPtr يحوّل i64→ptr.
            //      هذا يحدث عندما: تقسيم() ترجع ptr → STORE يحوّله لـ i64 (PtrToInt)
            //      → cg_.resolveOperand يحمّل i64 → GEP يفشل بدون هذا التحويل.
            // (EN) Fix GEP error: variable loaded from alloca(i64) is i64 not pointer.
            //      CreateStructGEP requires pointer — cg_.normalizeArrayPtr converts i64→ptr.
            // ================================================================
            arrPtr = cg_.normalizeArrayPtr(arrPtr, "join");

            // Load array length and data
            llvm::Value *lenGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 0, "join.len.gep");
            llvm::Value *arrLen = cg_.builder_->CreateLoad(i64Ty, lenGep, "join.len");
            llvm::Value *datGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 2, "join.dat.gep");
            llvm::Value *dataPtr = cg_.builder_->CreateLoad(ptrTy, datGep, "join.data");

            // Get separator length
            auto *strlenType = llvm::FunctionType::get(i64Ty, {ptrTy}, false);
            auto strlenFunc = cg_.module_->getOrInsertFunction("strlen", strlenType);
            llvm::Value *sepLen = cg_.builder_->CreateCall(strlenFunc, {sep}, "sep.len");

            // Allocate generous buffer: arrLen * 256 (rough estimate)
            llvm::Value *bufSize = cg_.builder_->CreateMul(arrLen, llvm::ConstantInt::get(i64Ty, 256));
            bufSize = cg_.builder_->CreateAdd(bufSize, llvm::ConstantInt::get(i64Ty, 1));
            auto *mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
            auto mallocFunc = cg_.module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *buf = cg_.builder_->CreateCall(mallocFunc, {bufSize}, "join.buf");

            // Start with empty string
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, 0), buf);

            // Loop: strcat each element + separator
            auto *strcatType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
            auto strcatFunc = cg_.module_->getOrInsertFunction("strcat", strcatType);

            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*cg_.context_, "join.loop", curFunc);
            llvm::BasicBlock *bodyBB = llvm::BasicBlock::Create(*cg_.context_, "join.body", curFunc);
            llvm::BasicBlock *doneBB = llvm::BasicBlock::Create(*cg_.context_, "join.done", curFunc);
            cg_.builder_->CreateBr(loopBB);

            cg_.builder_->SetInsertPoint(loopBB);
            llvm::PHINode *idx = cg_.builder_->CreatePHI(i64Ty, 2, "join.idx");
            idx->addIncoming(llvm::ConstantInt::get(i64Ty, 0), loopBB->getSinglePredecessor());
            llvm::Value *isDone = cg_.builder_->CreateICmpUGE(idx, arrLen, "join.done.check");
            cg_.builder_->CreateCondBr(isDone, doneBB, bodyBB);

            cg_.builder_->SetInsertPoint(bodyBB);
            // If not first element, append separator
            llvm::Value *isFirst = cg_.builder_->CreateICmpEQ(idx, llvm::ConstantInt::get(i64Ty, 0));
            llvm::BasicBlock *sepBB = llvm::BasicBlock::Create(*cg_.context_, "join.sep", curFunc);
            llvm::BasicBlock *concatBB = llvm::BasicBlock::Create(*cg_.context_, "join.concat", curFunc);
            cg_.builder_->CreateCondBr(isFirst, concatBB, sepBB);

            cg_.builder_->SetInsertPoint(sepBB);
            cg_.builder_->CreateCall(strcatFunc, {buf, sep});
            cg_.builder_->CreateBr(concatBB);

            cg_.builder_->SetInsertPoint(concatBB);
            // (AR) خطوة العنصر i64 (8) لتوحيد حجم خانة SadArray — كانت ptrTy
            //      (=4 على i686) تقرأ مؤشّرًا فاسدًا من مصفوفة مخزَّنة بخطوة 8.
            //      التحميل يبقى بنوع مؤشّر.
            // (EN) i64-stride element (8) for unified SadArray slots; load as ptr.
            llvm::Value *elemGep = cg_.builder_->CreateGEP(cg_.getInt64Type(), dataPtr, {idx}, "join.elem.gep");
            llvm::Value *elem = cg_.builder_->CreateLoad(ptrTy, elemGep, "join.elem");
            cg_.builder_->CreateCall(strcatFunc, {buf, elem});

            llvm::Value *nextIdx = cg_.builder_->CreateAdd(idx, llvm::ConstantInt::get(i64Ty, 1));
            idx->addIncoming(nextIdx, concatBB);
            cg_.builder_->CreateBr(loopBB);

            cg_.builder_->SetInsertPoint(doneBB);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = buf;
            }
            return buf;
        }



        llvm::Value *StringOpsCodeGen::emitBuiltinStringStartsWith(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *str = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *prefix = cg_.resolveOperand(inst->operands[1]);
            if (!str || !prefix)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = cg_.getInt64Type();

            // Get prefix length
            auto *strlenType = llvm::FunctionType::get(i64Ty, {ptrTy}, false);
            auto strlenFunc = cg_.module_->getOrInsertFunction("strlen", strlenType);
            llvm::Value *prefLen = cg_.builder_->CreateCall(strlenFunc, {prefix}, "pref.len");

            // strncmp(str, prefix, prefLen)
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto *strncmpType = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy, i64Ty}, false);
            auto strncmpFunc = cg_.module_->getOrInsertFunction("strncmp", strncmpType);
            llvm::Value *cmp = cg_.builder_->CreateCall(strncmpFunc, {str, prefix, prefLen}, "starts.cmp");
            llvm::Value *cmpBool = cg_.builder_->CreateICmpEQ(cmp,
                                                          llvm::ConstantInt::get(i32Ty, 0), "starts_with");
            llvm::Value *result = cg_.builder_->CreateZExt(cmpBool, i64Ty, "starts_with.i64");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }



        llvm::Value *StringOpsCodeGen::emitBuiltinStringEndsWith(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *str = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *suffix = cg_.resolveOperand(inst->operands[1]);
            if (!str || !suffix)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = cg_.getInt64Type();
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);

            auto *strlenType = llvm::FunctionType::get(i64Ty, {ptrTy}, false);
            auto strlenFunc = cg_.module_->getOrInsertFunction("strlen", strlenType);
            llvm::Value *strLen = cg_.builder_->CreateCall(strlenFunc, {str}, "str.len");
            llvm::Value *sufLen = cg_.builder_->CreateCall(strlenFunc, {suffix}, "suf.len");

            // Compare last sufLen chars: str + (strLen - sufLen)
            llvm::Value *offset = cg_.builder_->CreateSub(strLen, sufLen, "offset");
            llvm::Value *endPtr = cg_.builder_->CreateGEP(
                llvm::Type::getInt8Ty(*cg_.context_), str, {offset}, "end.ptr");

            auto *strcmpType = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
            auto strcmpFunc = cg_.module_->getOrInsertFunction("strcmp", strcmpType);
            llvm::Value *cmp = cg_.builder_->CreateCall(strcmpFunc, {endPtr, suffix}, "ends.cmp");
            llvm::Value *cmpBool = cg_.builder_->CreateICmpEQ(cmp,
                                                          llvm::ConstantInt::get(i32Ty, 0), "ends_with");
            llvm::Value *result = cg_.builder_->CreateZExt(cmpBool, i64Ty, "ends_with.i64");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }



        llvm::Value *StringOpsCodeGen::emitBuiltinStringContains(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *str = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *substr = cg_.resolveOperand(inst->operands[1]);
            if (!str || !substr)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

            // strstr(str, substr) != null
            auto *strstrType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
            auto strstrFunc = cg_.module_->getOrInsertFunction("strstr", strstrType);
            llvm::Value *found = cg_.builder_->CreateCall(strstrFunc, {str, substr}, "found");
            llvm::Value *cmpBool = cg_.builder_->CreateICmpNE(found,
                                                          llvm::ConstantPointerNull::get(llvm::PointerType::getUnqual(*cg_.context_)), "contains");
            llvm::Value *result = cg_.builder_->CreateZExt(cmpBool, cg_.getInt64Type(), "contains.i64");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }


} // namespace LLVM
} // namespace Sad
