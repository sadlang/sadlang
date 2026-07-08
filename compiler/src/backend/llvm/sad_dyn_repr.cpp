/*
 * ============================================================================
 * (AR) التمثيل الديناميّ المميّز `%SadDyn` — تنفيذ حلّ ISSUE-076 الجذريّ.
 * (EN) The distinct dynamic representation `%SadDyn` — ISSUE-076 root fix impl.
 * ============================================================================
 */

#include "sad_dyn_repr.h"
#include "llvm_codegen.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Intrinsics.h>

using namespace Sad::Compiler::SIR;

namespace Sad
{
    namespace LLVM
    {
        // (AR) اسم النوع المميّز — مصدرٌ واحد / (EN) the distinct type name — single source
        static const char *kSadDynTypeName = "SadDyn";

        llvm::StructType *getSadDynType(llvm::LLVMContext &ctx)
        {
            // (AR) أعِد النوع المسمّى إن وُجد (يضمن هويّة واحدة عبر الوحدة كلّها)
            // (EN) reuse the named type if present (guarantees a single identity module-wide)
            if (llvm::StructType *existing = llvm::StructType::getTypeByName(ctx, kSadDynTypeName))
                return existing;
            return llvm::StructType::create(
                ctx,
                {llvm::Type::getInt8Ty(ctx), llvm::Type::getInt64Ty(ctx)},
                kSadDynTypeName);
        }

        bool isSadDyn(const llvm::Value *v)
        {
            if (!v)
                return false;
            llvm::Type *t = v->getType();
            if (!t->isStructTy())
                return false;
            auto *st = llvm::cast<llvm::StructType>(t);
            return st->hasName() && st->getName() == kSadDynTypeName;
        }

        // ====================================================================
        // البناء / construction
        // ====================================================================
        llvm::Value *makeDyn(LLVMCodeGen &cg, llvm::Value *kindI8, llvm::Value *payloadI64)
        {
            auto &b = *cg.builder_;
            auto *ty = getSadDynType(*cg.context_);
            llvm::Value *agg = llvm::UndefValue::get(ty);
            agg = b.CreateInsertValue(agg, kindI8, {kDynKindFieldIdx}, "dyn.k");
            agg = b.CreateInsertValue(agg, payloadI64, {kDynPayloadFieldIdx}, "dyn.v");
            return agg;
        }

        llvm::Value *packDyn(LLVMCodeGen &cg, llvm::Value *concrete, uint8_t kind)
        {
            auto &b = *cg.builder_;
            auto &ctx = *cg.context_;
            auto *i64 = llvm::Type::getInt64Ty(ctx);
            auto *i8 = llvm::Type::getInt8Ty(ctx);
            auto *dbl = llvm::Type::getDoubleTy(ctx);
            llvm::Value *payload = nullptr;
            llvm::Type *ct = concrete ? concrete->getType() : nullptr;

            if (kind == DynKind::Float)
            {
                if (ct && ct->isDoubleTy())
                    payload = b.CreateBitCast(concrete, i64, "pack.f.bits");
                else if (ct && ct->isIntegerTy(64))
                    payload = concrete; // (AR) بِتّات double خام / (EN) raw double bits
                else if (ct && ct->isIntegerTy())
                    payload = b.CreateBitCast(b.CreateSIToFP(concrete, dbl, "pack.i2f"), i64, "pack.f.bits2");
                else
                    payload = llvm::ConstantInt::get(i64, 0);
            }
            else if (kind == DynKind::Int)
            {
                if (ct && ct->isIntegerTy(64))
                    payload = concrete;
                else if (ct && ct->isIntegerTy(1))
                    payload = b.CreateZExt(concrete, i64, "pack.b2i");
                else if (ct && ct->isIntegerTy())
                    payload = b.CreateSExt(concrete, i64, "pack.i.ext");
                else if (ct && ct->isDoubleTy())
                    payload = b.CreateFPToSI(concrete, i64, "pack.d2i");
                else if (ct && ct->isPointerTy())
                    payload = b.CreatePtrToInt(concrete, i64, "pack.p2i");
                else
                    payload = llvm::ConstantInt::get(i64, 0);
            }
            else if (kind == DynKind::Bool)
            {
                if (ct && ct->isIntegerTy(1))
                    payload = b.CreateZExt(concrete, i64, "pack.b2i");
                else if (ct && ct->isIntegerTy())
                    payload = b.CreateZExtOrTrunc(concrete, i64, "pack.b.ext");
                else
                    payload = llvm::ConstantInt::get(i64, 0);
            }
            else // Str / Array / Map / Obj / Adt / Null → مؤشّر أو صفر
            {
                if (ct && ct->isPointerTy())
                    payload = b.CreatePtrToInt(concrete, i64, "pack.p2i");
                else if (ct && ct->isIntegerTy(64))
                    payload = concrete;
                else if (ct && ct->isIntegerTy())
                    payload = b.CreateZExtOrTrunc(concrete, i64, "pack.x.ext");
                else
                    payload = llvm::ConstantInt::get(i64, 0);
            }

            return makeDyn(cg, llvm::ConstantInt::get(i8, kind), payload);
        }

        llvm::Value *toDyn(LLVMCodeGen &cg, llvm::Value *v, SadTypeKind sirType)
        {
            if (isSadDyn(v))
                return v;
            uint8_t kind;
            switch (sirType)
            {
            case SadTypeKind::Integer:
                kind = DynKind::Int;
                break;
            case SadTypeKind::Float:
                kind = DynKind::Float;
                break;
            case SadTypeKind::Boolean:
                kind = DynKind::Bool;
                break;
            case SadTypeKind::String:
                kind = DynKind::Str;
                break;
            case SadTypeKind::Pointer:
                kind = DynKind::Str;
                break;
            case SadTypeKind::Null:
                kind = DynKind::Null;
                break;
            case SadTypeKind::Array:
                kind = DynKind::Array;
                break;
            case SadTypeKind::Map:
                kind = DynKind::Map;
                break;
            default:
            {
                // (AR) استدلالٌ من نوع LLVM حين يكون SIR مبهمًا (Any/Void/…)
                // (EN) infer from the LLVM type when the SIR type is opaque (Any/Void/…)
                llvm::Type *t = v ? v->getType() : nullptr;
                if (t && t->isDoubleTy())
                    kind = DynKind::Float;
                else if (t && t->isIntegerTy(1))
                    kind = DynKind::Bool;
                else if (t && t->isPointerTy())
                    kind = DynKind::Str;
                else
                    kind = DynKind::Int;
                break;
            }
            }
            return packDyn(cg, v, kind);
        }

        // ====================================================================
        // الاستخراج / extraction
        // ====================================================================
        llvm::Value *dynKindByte(LLVMCodeGen &cg, llvm::Value *dyn)
        {
            return cg.builder_->CreateExtractValue(dyn, {kDynKindFieldIdx}, "dyn.kind");
        }

        llvm::Value *dynPayloadI64(LLVMCodeGen &cg, llvm::Value *dyn)
        {
            return cg.builder_->CreateExtractValue(dyn, {kDynPayloadFieldIdx}, "dyn.payload");
        }

        llvm::Value *unpackDouble(LLVMCodeGen &cg, llvm::Value *dyn)
        {
            auto &b = *cg.builder_;
            auto &ctx = *cg.context_;
            auto *dbl = llvm::Type::getDoubleTy(ctx);
            auto *i8 = llvm::Type::getInt8Ty(ctx);
            llvm::Value *payload = dynPayloadI64(cg, dyn);
            llvm::Value *kind = dynKindByte(cg, dyn);
            llvm::Value *isF = b.CreateICmpEQ(kind, llvm::ConstantInt::get(i8, DynKind::Float), "dyn.isf");
            llvm::Value *asBits = b.CreateBitCast(payload, dbl, "dyn.f.bc");
            llvm::Value *asInt = b.CreateSIToFP(payload, dbl, "dyn.i.f");
            return b.CreateSelect(isF, asBits, asInt, "dyn.d");
        }

        llvm::Value *unpackPtr(LLVMCodeGen &cg, llvm::Value *dyn)
        {
            auto *ptrTy = llvm::PointerType::getUnqual(*cg.context_);
            return cg.builder_->CreateIntToPtr(dynPayloadI64(cg, dyn), ptrTy, "dyn.ptr");
        }

        // ====================================================================
        // الموزِّعات / dispatchers
        // ====================================================================
        static llvm::Value *isFloatKind(LLVMCodeGen &cg, llvm::Value *dyn)
        {
            auto *i8 = llvm::Type::getInt8Ty(*cg.context_);
            return cg.builder_->CreateICmpEQ(
                dynKindByte(cg, dyn), llvm::ConstantInt::get(i8, DynKind::Float), "dyn.k.isf");
        }

        llvm::Value *dynBinOp(LLVMCodeGen &cg, SIROpcode op, llvm::Value *l, llvm::Value *r)
        {
            auto &b = *cg.builder_;
            auto &ctx = *cg.context_;
            auto *i64 = llvm::Type::getInt64Ty(ctx);
            auto *i8 = llvm::Type::getInt8Ty(ctx);
            auto *dbl = llvm::Type::getDoubleTy(ctx);

            llvm::Value *eitherF = b.CreateOr(isFloatKind(cg, l), isFloatKind(cg, r), "dyn.either.f");
            llvm::Value *lD = unpackDouble(cg, l);
            llvm::Value *rD = unpackDouble(cg, r);
            llvm::Value *lI = dynPayloadI64(cg, l);
            llvm::Value *rI = dynPayloadI64(cg, r);

            // (AR) === Amelia (ISSUE-076): قاسمٌ آمنٌ للفرع الصحيح المُهمَل ===
            //      الموزِّع بلا فروع: يحسب iRes (صحيح) و fRes (عشريّ) دائمًا ثمّ يختار بالوسم.
            //      لكنّ sdiv/srem على صفرٍ (أو INT_MIN/-1) سلوكٌ غير معرَّف يُطلق #DE (SIGFPE)
            //      على x86 **وإن أُهمِلت النتيجة**. عند عمليّةٍ عشريّة (eitherF) تكون rI بِتّات
            //      double خام (قد تساوي 0 للقاسم 0.0) ⇒ الـsdiv المُهمَل ينهار. نستبدل القاسمَ 1
            //      على الفرع العشريّ فقط؛ الفرع الصحيح الحقيقيّ يُبقي rI (دلالةُ القسمة محفوظة).
            // (EN) === Amelia (ISSUE-076): safe divisor for the discarded integer branch ===
            //      The dispatcher is branchless: it always computes iRes (int) and fRes (float)
            //      then selects by the tag. But sdiv/srem by zero (or INT_MIN/-1) is UB that
            //      raises #DE (SIGFPE) on x86 **even when the result is unused**. For a float op
            //      (eitherF) rI holds raw double bits (0 for divisor 0.0) ⇒ the discarded sdiv
            //      traps. Substitute divisor 1 on the float branch only; the true integer path
            //      keeps rI intact (integer-division semantics preserved).
            llvm::Value *safeRI = b.CreateSelect(
                eitherF, llvm::ConstantInt::get(i64, 1), rI, "dyn.safe.divisor");

            // (AR) نحسب النتيجتين العشريّة والصحيحة ثمّ نختار زمنَ التشغيل حسب الوسم؛ يطابق
            //      المفسّر: عشريّ ⇒ fadd/… (و% ⇒ frem أي fmod، // ⇒ floor(fdiv))؛ صحيح ⇒ add/…
            //      (و% ⇒ srem، // ⇒ sdiv). المعاملات الصحيحة تُقرأ من الحمولة مباشرة (نظيفة).
            // (EN) Compute both the float and integer results, then runtime-select by the tag,
            //      matching the interpreter: float ⇒ fadd/… (% ⇒ frem i.e. fmod, // ⇒ floor(fdiv));
            //      int ⇒ add/… (% ⇒ srem, // ⇒ sdiv). Int operands read the payload directly (clean).
            llvm::Value *fRes = nullptr;
            llvm::Value *iRes = nullptr;

            switch (op)
            {
            case SIROpcode::ADD_I64:
            case SIROpcode::ADD_F64:
                fRes = b.CreateFAdd(lD, rD, "dyn.fadd");
                iRes = b.CreateAdd(lI, rI, "dyn.iadd");
                break;
            case SIROpcode::SUB_I64:
            case SIROpcode::SUB_F64:
                fRes = b.CreateFSub(lD, rD, "dyn.fsub");
                iRes = b.CreateSub(lI, rI, "dyn.isub");
                break;
            case SIROpcode::MUL_I64:
            case SIROpcode::MUL_F64:
                fRes = b.CreateFMul(lD, rD, "dyn.fmul");
                iRes = b.CreateMul(lI, rI, "dyn.imul");
                break;
            case SIROpcode::DIV_I64:
            case SIROpcode::DIV_F64:
                fRes = b.CreateFDiv(lD, rD, "dyn.fdiv");
                iRes = b.CreateSDiv(lI, safeRI, "dyn.idiv");
                break;
            case SIROpcode::MOD_I64:
                // (AR) % : عشريّ ⇒ frem (fmod، مثل المفسّر 7.5%2=1.5)؛ صحيح ⇒ srem.
                // (EN) % : float ⇒ frem (fmod, like the interpreter 7.5%2=1.5); int ⇒ srem.
                fRes = b.CreateFRem(lD, rD, "dyn.frem");
                iRes = b.CreateSRem(lI, safeRI, "dyn.srem");
                break;
            case SIROpcode::FLOOR_DIV_I64:
            {
                // (AR) // : عشريّ ⇒ floor(fdiv) (مثل المفسّر 7.5//2=3.0)؛ صحيح ⇒ sdiv.
                // (EN) // : float ⇒ floor(fdiv) (like the interpreter 7.5//2=3.0); int ⇒ sdiv.
                llvm::Value *q = b.CreateFDiv(lD, rD, "dyn.fdiv.q");
                llvm::Function *floorFn = llvm::Intrinsic::getDeclaration(
                    cg.module_.get(), llvm::Intrinsic::floor, {dbl});
                fRes = b.CreateCall(floorFn, {q}, "dyn.floor");
                iRes = b.CreateSDiv(lI, safeRI, "dyn.sdiv");
                break;
            }
            default:
                fRes = b.CreateFAdd(lD, rD, "dyn.fadd.def");
                iRes = b.CreateAdd(lI, rI, "dyn.iadd.def");
                break;
            }

            llvm::Value *resKind = b.CreateSelect(
                eitherF, llvm::ConstantInt::get(i8, DynKind::Float),
                llvm::ConstantInt::get(i8, DynKind::Int), "dyn.res.kind");
            llvm::Value *fBits = b.CreateBitCast(fRes, i64, "dyn.res.fbits");
            llvm::Value *resPayload = b.CreateSelect(eitherF, fBits, iRes, "dyn.res.payload");
            return makeDyn(cg, resKind, resPayload);
        }

        llvm::Value *dynCompare(LLVMCodeGen &cg, DynCmp cmp, llvm::Value *l, llvm::Value *r)
        {
            auto &b = *cg.builder_;
            llvm::Value *eitherF = b.CreateOr(isFloatKind(cg, l), isFloatKind(cg, r), "dyn.cmp.either.f");
            llvm::Value *lD = unpackDouble(cg, l);
            llvm::Value *rD = unpackDouble(cg, r);
            llvm::Value *lI = dynPayloadI64(cg, l);
            llvm::Value *rI = dynPayloadI64(cg, r);

            llvm::Value *fRes = nullptr;
            llvm::Value *iRes = nullptr;
            switch (cmp)
            {
            case DynCmp::EQ:
                fRes = b.CreateFCmpOEQ(lD, rD, "dyn.feq");
                iRes = b.CreateICmpEQ(lI, rI, "dyn.ieq");
                break;
            case DynCmp::NE:
                fRes = b.CreateFCmpONE(lD, rD, "dyn.fne");
                iRes = b.CreateICmpNE(lI, rI, "dyn.ine");
                break;
            case DynCmp::LT:
                fRes = b.CreateFCmpOLT(lD, rD, "dyn.flt");
                iRes = b.CreateICmpSLT(lI, rI, "dyn.ilt");
                break;
            case DynCmp::LE:
                fRes = b.CreateFCmpOLE(lD, rD, "dyn.fle");
                iRes = b.CreateICmpSLE(lI, rI, "dyn.ile");
                break;
            case DynCmp::GT:
                fRes = b.CreateFCmpOGT(lD, rD, "dyn.fgt");
                iRes = b.CreateICmpSGT(lI, rI, "dyn.igt");
                break;
            case DynCmp::GE:
                fRes = b.CreateFCmpOGE(lD, rD, "dyn.fge");
                iRes = b.CreateICmpSGE(lI, rI, "dyn.ige");
                break;
            }
            return b.CreateSelect(eitherF, fRes, iRes, "dyn.cmp.res");
        }

        llvm::Value *dynToString(LLVMCodeGen &cg, llvm::Value *dyn)
        {
            auto &b = *cg.builder_;
            auto &ctx = *cg.context_;
            auto *i64 = llvm::Type::getInt64Ty(ctx);
            auto *i8 = llvm::Type::getInt8Ty(ctx);
            auto *dbl = llvm::Type::getDoubleTy(ctx);
            auto *ptrTy = llvm::PointerType::getUnqual(ctx);

            llvm::Value *kind = dynKindByte(cg, dyn);
            llvm::Value *payload = dynPayloadI64(cg, dyn);

            // (AR) مخزّن مشترك للصحيح/العشريّ (512 لـ%.6f لـDBL_MAX) / (EN) shared buffer int/float
            auto *mallocTy = llvm::FunctionType::get(ptrTy, {i64}, false);
            auto mallocFn = cg.module_->getOrInsertFunction("malloc", mallocTy);
            llvm::Value *buf = b.CreateCall(mallocFn, {llvm::ConstantInt::get(i64, 512)}, "dyn.ts.buf");

            auto *parent = b.GetInsertBlock()->getParent();
            auto *intBB = llvm::BasicBlock::Create(ctx, "dyn.ts.int", parent);
            auto *floatBB = llvm::BasicBlock::Create(ctx, "dyn.ts.float", parent);
            auto *boolBB = llvm::BasicBlock::Create(ctx, "dyn.ts.bool", parent);
            auto *strBB = llvm::BasicBlock::Create(ctx, "dyn.ts.str", parent);
            auto *nullBB = llvm::BasicBlock::Create(ctx, "dyn.ts.null", parent);
            auto *mergeBB = llvm::BasicBlock::Create(ctx, "dyn.ts.merge", parent);

            llvm::SwitchInst *sw = b.CreateSwitch(kind, nullBB, 4);
            sw->addCase(llvm::ConstantInt::get(i8, DynKind::Int), intBB);
            sw->addCase(llvm::ConstantInt::get(i8, DynKind::Float), floatBB);
            sw->addCase(llvm::ConstantInt::get(i8, DynKind::Bool), boolBB);
            sw->addCase(llvm::ConstantInt::get(i8, DynKind::Str), strBB);

            // (AR) صحيح: %lld / (EN) int: %lld
            b.SetInsertPoint(intBB);
            if (cg.freestanding_)
            {
                auto *itoaTy = llvm::FunctionType::get(llvm::Type::getInt32Ty(ctx), {ptrTy, i64}, false);
                auto itoaFn = cg.module_->getOrInsertFunction("__sad_itoa", itoaTy);
                b.CreateCall(itoaFn, {buf, payload});
            }
            else
            {
                auto *sprintfTy = llvm::FunctionType::get(llvm::Type::getInt32Ty(ctx), {ptrTy, ptrTy}, true);
                auto sprintfFn = cg.module_->getOrInsertFunction("sprintf", sprintfTy);
                llvm::Value *ifmt = b.CreateGlobalStringPtr("%lld", "dyn.ts.ifmt");
                b.CreateCall(sprintfFn, {buf, ifmt, payload});
            }
            llvm::Value *intRes = buf;
            b.CreateBr(mergeBB);
            intBB = b.GetInsertBlock();

            // (AR) عشريّ: __sad_format_double(bitcast(payload)) / (EN) float
            b.SetInsertPoint(floatBB);
            llvm::Value *fdbl = b.CreateBitCast(payload, dbl, "dyn.ts.fbc");
            if (cg.freestanding_)
            {
                auto *ftoaTy = llvm::FunctionType::get(llvm::Type::getInt32Ty(ctx), {ptrTy, dbl}, false);
                auto ftoaFn = cg.module_->getOrInsertFunction("__sad_ftoa", ftoaTy);
                b.CreateCall(ftoaFn, {buf, fdbl});
            }
            else
            {
                auto *fmtDblTy = llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), {ptrTy, dbl}, false);
                auto fmtDblFn = cg.module_->getOrInsertFunction("__sad_format_double", fmtDblTy);
                b.CreateCall(fmtDblFn, {buf, fdbl});
            }
            llvm::Value *floatRes = buf;
            b.CreateBr(mergeBB);
            floatBB = b.GetInsertBlock();

            // (AR) منطقيّ: صحيح/خطأ / (EN) bool: صحيح/خطأ
            b.SetInsertPoint(boolBB);
            llvm::Value *bcond = b.CreateICmpNE(payload, llvm::ConstantInt::get(i64, 0), "dyn.ts.bc");
            llvm::Value *trueStr = b.CreateGlobalStringPtr("\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "dyn.ts.true");
            llvm::Value *falseStr = b.CreateGlobalStringPtr("\xd8\xae\xd8\xb7\xd8\xa3", "dyn.ts.false");
            llvm::Value *boolRes = b.CreateSelect(bcond, trueStr, falseStr, "dyn.ts.bstr");
            b.CreateBr(mergeBB);
            boolBB = b.GetInsertBlock();

            // (AR) نصّ: inttoptr(payload) مع حماية null / (EN) str: inttoptr with null guard
            b.SetInsertPoint(strBB);
            llvm::Value *strPtr = b.CreateIntToPtr(payload, ptrTy, "dyn.ts.strp");
            llvm::Value *strNull = b.CreateICmpEQ(
                strPtr, llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)), "dyn.ts.strnull");
            llvm::Value *voidStr = b.CreateGlobalStringPtr("void", "dyn.ts.void");
            llvm::Value *strRes = b.CreateSelect(strNull, voidStr, strPtr, "dyn.ts.safestr");
            b.CreateBr(mergeBB);
            strBB = b.GetInsertBlock();

            // (AR) عدم/غيره: لاشيء / (EN) null/other: لاشيء
            b.SetInsertPoint(nullBB);
            llvm::Value *nullRes = b.CreateGlobalStringPtr(
                "\xd9\x84\xd8\xa7\xd8\xb4\xd9\x8a\xd8\xa1", "dyn.ts.nullstr"); // لاشيء
            b.CreateBr(mergeBB);
            nullBB = b.GetInsertBlock();

            b.SetInsertPoint(mergeBB);
            auto *phi = b.CreatePHI(ptrTy, 5, "dyn.ts.result");
            phi->addIncoming(intRes, intBB);
            phi->addIncoming(floatRes, floatBB);
            phi->addIncoming(boolRes, boolBB);
            phi->addIncoming(strRes, strBB);
            phi->addIncoming(nullRes, nullBB);
            return phi;
        }

        llvm::Value *dynTypeName(LLVMCodeGen &cg, llvm::Value *dyn)
        {
            auto &b = *cg.builder_;
            auto &ctx = *cg.context_;
            auto *i8 = llvm::Type::getInt8Ty(ctx);
            auto *ptrTy = llvm::PointerType::getUnqual(ctx);
            llvm::Value *kind = dynKindByte(cg, dyn);

            // (AR) اسم النوع من نفس مصدر sadTypeKindArabicName (types.yaml) لكلّ وسمٍ زمنَ التشغيل
            //      ⇒ يطابق نوع() المفسّر بايتيًّا (رقم/عشري/منطقي/نص). لا سلاسل حرفيّة مكرّرة.
            // (EN) The type name from the same source as sadTypeKindArabicName (types.yaml) for each
            //      runtime kind ⇒ matches نوع() byte-for-byte (رقم/عشري/منطقي/نص). No duplicated literals.
            auto nameFor = [&](SadTypeKind k)
            { return b.CreateGlobalStringPtr(::Sad::Types::sadTypeKindArabicName(k), "dyn.tn"); };

            auto *parent = b.GetInsertBlock()->getParent();
            auto *intBB = llvm::BasicBlock::Create(ctx, "dyn.tn.int", parent);
            auto *floatBB = llvm::BasicBlock::Create(ctx, "dyn.tn.float", parent);
            auto *boolBB = llvm::BasicBlock::Create(ctx, "dyn.tn.bool", parent);
            auto *strBB = llvm::BasicBlock::Create(ctx, "dyn.tn.str", parent);
            auto *defBB = llvm::BasicBlock::Create(ctx, "dyn.tn.def", parent);
            // (AR) Amelia (ISSUE-076): وسمُ العدم (Null) ⇒ اسم النوع «عدم» من SoT لا «مجهول».
            //      نوع(حمولةٍ ديناميّةٍ = لاشيء) يطابق المفسّرَ («عدم») بدل تراجُع default («مجهول»).
            // (EN) Amelia (ISSUE-076): the Null kind ⇒ the SoT name «عدم», not «مجهول». نوع() of a
            //      null dynamic payload matches the interpreter («عدم») instead of the default.
            auto *nullBB = llvm::BasicBlock::Create(ctx, "dyn.tn.null", parent);
            auto *mergeBB = llvm::BasicBlock::Create(ctx, "dyn.tn.merge", parent);

            llvm::SwitchInst *sw = b.CreateSwitch(kind, defBB, 5);
            sw->addCase(llvm::ConstantInt::get(i8, DynKind::Int), intBB);
            sw->addCase(llvm::ConstantInt::get(i8, DynKind::Float), floatBB);
            sw->addCase(llvm::ConstantInt::get(i8, DynKind::Bool), boolBB);
            sw->addCase(llvm::ConstantInt::get(i8, DynKind::Str), strBB);
            sw->addCase(llvm::ConstantInt::get(i8, DynKind::Null), nullBB);

            b.SetInsertPoint(intBB);
            llvm::Value *ri = nameFor(SadTypeKind::Integer);
            b.CreateBr(mergeBB);
            intBB = b.GetInsertBlock();
            b.SetInsertPoint(floatBB);
            llvm::Value *rf = nameFor(SadTypeKind::Float);
            b.CreateBr(mergeBB);
            floatBB = b.GetInsertBlock();
            b.SetInsertPoint(boolBB);
            llvm::Value *rb = nameFor(SadTypeKind::Boolean);
            b.CreateBr(mergeBB);
            boolBB = b.GetInsertBlock();
            b.SetInsertPoint(strBB);
            llvm::Value *rs = nameFor(SadTypeKind::String);
            b.CreateBr(mergeBB);
            strBB = b.GetInsertBlock();
            b.SetInsertPoint(defBB);
            llvm::Value *rd = nameFor(SadTypeKind::Unknown);
            b.CreateBr(mergeBB);
            defBB = b.GetInsertBlock();
            b.SetInsertPoint(nullBB);
            llvm::Value *rnull = nameFor(SadTypeKind::Null);
            b.CreateBr(mergeBB);
            nullBB = b.GetInsertBlock();

            b.SetInsertPoint(mergeBB);
            auto *phi = b.CreatePHI(ptrTy, 6, "dyn.tn.result");
            phi->addIncoming(ri, intBB);
            phi->addIncoming(rf, floatBB);
            phi->addIncoming(rb, boolBB);
            phi->addIncoming(rs, strBB);
            phi->addIncoming(rd, defBB);
            phi->addIncoming(rnull, nullBB);
            return phi;
        }

    } // namespace LLVM
} // namespace Sad
