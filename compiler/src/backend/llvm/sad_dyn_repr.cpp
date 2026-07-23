/*
 * ============================================================================
 * (AR) التمثيل الديناميّ المميّز `%SadDyn` — تنفيذ حلّ ISSUE-076 الجذريّ.
 * (EN) The distinct dynamic representation `%SadDyn` — ISSUE-076 root fix impl.
 * ============================================================================
 */

#include "sad_dyn_repr.h"
#include "llvm_codegen.h"
#include "sir_constants.h" // (AR) kDivZeroRun001Msg + kSadPanicCheckViolation (د-1) / (EN) D-1 div-zero guard constants
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

        // ====================================================================
        // (AR) ISSUE-063: المسح المسبق لديناميّة الخانات — انظر توثيق llvm_codegen.h
        // (EN) ISSUE-063: the dyn-slot pre-scan — see the llvm_codegen.h docs
        // ====================================================================
        namespace
        {
            // (AR) تطبيع اسم الخانة: السجلّات تبدأ بـ% والعامّة لا / (EN) strip the % register prefix
            std::string cleanSlotName(const std::string &n)
            {
                return (!n.empty() && n[0] == '%') ? n.substr(1) : n;
            }

            // (AR) تجريد علامات التنصيص عن اسم الحقل / (EN) strip quotes off a field name
            std::string cleanFieldName(std::string f)
            {
                if (!f.empty() && f.front() == '"')
                    f = f.substr(1);
                if (!f.empty() && f.back() == '"')
                    f = f.substr(0, f.size() - 1);
                return f;
            }

            // (AR) حدّ التقارب الأعلى للمسح (تلوّثٌ عبر الدوال متسلسل) — عمليًّا يتقارب في ≤3
            // (EN) fixpoint upper bound (cross-function taint chains) — converges in ≤3 in practice
            constexpr int kDynScanMaxIterations = 8;
        } // namespace

        bool LLVMCodeGen::isDynSlot(const std::string &funcName, const std::string &slotName) const
        {
            std::string n = cleanSlotName(slotName);
            if (dynGlobalSlots_.count(n))
                return true;
            auto it = dynLocalSlots_.find(funcName);
            return it != dynLocalSlots_.end() && it->second.count(n) != 0;
        }

        void LLVMCodeGen::collectDynSlots(std::shared_ptr<SIRModule> sirModule)
        {
            dynGlobalSlots_.clear();
            dynLocalSlots_.clear();
            if (!sirModule)
                return;

            // (AR) أسماء المتغيّرات العامّة + العامّ المصرَّح Any أصلًا (المستوى الأعلى)
            // (EN) global names + globals the frontend already declared Any (top level)
            std::set<std::string> globalNames;
            for (const auto &g : sirModule->getGlobalVariables())
            {
                if (!g)
                    continue;
                globalNames.insert(g->name);
                if (g->type == SadTypeKind::Any)
                    dynGlobalSlots_.insert(g->name);
            }

            // (AR) الدوال التي تُرجع ديناميًّا (تتقارب عبر الدورات) / (EN) dyn-returning functions
            std::map<std::string, bool> dynReturnFuncs;
            for (const auto &fn : sirModule->getFunctions())
                if (fn)
                    dynReturnFuncs[fn->getName()] = (fn->returnType == SadTypeKind::Any);

            bool changed = true;
            for (int iter = 0; changed && iter < kDynScanMaxIterations; ++iter)
            {
                changed = false;
                for (const auto &fn : sirModule->getFunctions())
                {
                    if (!fn)
                        continue;
                    std::set<std::string> &localDyn = dynLocalSlots_[fn->getName()];

                    // (AR) سجلّاتٌ قيمتها ديناميّة زمنَ التشغيل (مسح أماميّ داخل الدالّة)
                    // (EN) registers whose runtime value is dynamic (forward scan within the function)
                    std::set<std::string> dynRegs;
                    // (AR) أنواع القيم المخزَّنة لكلّ خانة — لقاعدة مزيج نصّ/عدد
                    // (EN) stored value kinds per slot — for the text/number mix rule
                    std::map<std::string, std::set<SadTypeKind>> slotStoredKinds;

                    auto isDynSlotName = [&](const std::string &raw)
                    {
                        std::string n = cleanSlotName(raw);
                        return localDyn.count(n) != 0 || dynGlobalSlots_.count(n) != 0;
                    };
                    // (AR) قيمةُ معامل ديناميّة؟ Any ساكنًا، أو سجلّ ملوَّث، أو خانةٌ-كقيمة
                    //      ديناميّة (resolveOperand يحمّل الخانة تلقائيًّا)
                    // (EN) dynamic operand value? statically Any, a tainted register, or a
                    //      dyn slot used as a value (resolveOperand auto-loads slots)
                    auto valueIsDyn = [&](const SIROperand &op)
                    {
                        if (op.dataType == SadTypeKind::Any)
                            return true;
                        if (op.type != SIROperandType::REGISTER)
                            return false;
                        return dynRegs.count(op.name) != 0 || isDynSlotName(op.name);
                    };
                    auto markSlotDyn = [&](const std::string &raw)
                    {
                        std::string n = cleanSlotName(raw);
                        if (globalNames.count(n))
                            changed = dynGlobalSlots_.insert(n).second || changed;
                        changed = localDyn.insert(n).second || changed;
                    };

                    for (const auto &bb : fn->getBasicBlocks())
                    {
                        if (!bb)
                            continue;
                        for (const auto &inst : bb->instructions)
                        {
                            // (AR) 1) تلويث السجلّات / (EN) 1) register tainting
                            if (inst.result.has_value())
                            {
                                bool resDyn = (inst.result->dataType == SadTypeKind::Any);
                                if (!resDyn && !inst.operands.empty())
                                {
                                    switch (inst.opcode)
                                    {
                                    case SIROpcode::LOAD:
                                        resDyn = isDynSlotName(inst.operands[0].name);
                                        break;
                                    case SIROpcode::MOVE:
                                        resDyn = valueIsDyn(inst.operands[0]);
                                        break;
                                    case SIROpcode::CALL:
                                    {
                                        auto cit = dynReturnFuncs.find(inst.operands[0].name);
                                        resDyn = (cit != dynReturnFuncs.end() && cit->second);
                                        break;
                                    }
                                    default:
                                        break;
                                    }
                                }
                                if (resDyn)
                                    dynRegs.insert(inst.result->name);
                            }

                            // (AR) 2) تخزينٌ بخانة (معاملان): ديناميّ ⇒ خانة %SadDyn؛
                            //         وجمعُ أنواع المخزون لقاعدة مزيج نصّ/عشريّ (دَين 5)
                            // (EN) 2) 2-operand STORE: dynamic ⇒ %SadDyn slot; also collect
                            //         stored kinds for the string/float mix rule (debt 5)
                            if (inst.opcode == SIROpcode::STORE && inst.operands.size() == 2)
                            {
                                const std::string slot = cleanSlotName(inst.operands[1].name);
                                if (valueIsDyn(inst.operands[0]))
                                {
                                    markSlotDyn(slot);
                                }
                                else
                                {
                                    // (AR) قاعدة المزيج: نجمع النصّيّ والعدديّ فقط (isNumericKind من SoT الأنواع)
                                    // (EN) mix rule: track only string and numeric kinds (SoT isNumericKind)
                                    SadTypeKind k = inst.operands[0].dataType;
                                    if (k == SadTypeKind::String || ::Sad::Types::isNumericKind(k))
                                    {
                                        auto &kinds = slotStoredKinds[slot];
                                        kinds.insert(k);
                                        // (AR) مزيجُ نصٍّ وعشريّ بخانةٍ واحدة: ترقية double
                                        //      القديمة تُنتج inttoptr(double) ⇒ فشل verifyModule
                                        // (EN) string+float mixed in one slot: the legacy double
                                        //      promotion yields inttoptr(double) ⇒ verifyModule failure
                                        if (kinds.count(SadTypeKind::String) &&
                                            kinds.count(SadTypeKind::Float))
                                            markSlotDyn(slot);
                                    }
                                }
                            }

                            // (AR) 3) تخزينٌ بحقل: OBJECT_SET (القيمة [2]) أو STORE ثلاثيّ
                            //         (القيمة [0] والحقل [2]) ⇒ رفعُ نوع الحقل إلى Any عند
                            //         الديناميّ أو المخالف للمعلَن (عشريّ/نصّ بحقل صحيح)
                            // (EN) 3) field stores: OBJECT_SET (value [2]) or 3-operand STORE
                            //         (value [0], field [2]) ⇒ raise the SIR field type to Any
                            //         for dynamic or declared-mismatching values
                            const bool isObjSet = (inst.opcode == SIROpcode::OBJECT_SET &&
                                                   inst.operands.size() >= 3);
                            const bool isMemberStore = (inst.opcode == SIROpcode::STORE &&
                                                        inst.operands.size() >= 3);
                            if (isObjSet || isMemberStore)
                            {
                                const SIROperand &valOp = isObjSet ? inst.operands[2] : inst.operands[0];
                                const std::string fieldName = cleanFieldName(
                                    isObjSet ? inst.operands[1].name : inst.operands[2].name);
                                const SadTypeKind v = valOp.dataType;
                                const bool dynVal = valueIsDyn(valOp);
                                for (const auto &cls : sirModule->getClasses())
                                {
                                    if (!cls)
                                        continue;
                                    auto fit = cls->fields_.find(fieldName);
                                    if (fit == cls->fields_.end() || fit->second == SadTypeKind::Any)
                                        continue;
                                    const SadTypeKind d = fit->second;
                                    const bool mismatch =
                                        (v == SadTypeKind::Float && d == SadTypeKind::Integer) ||
                                        (v == SadTypeKind::String &&
                                         (d == SadTypeKind::Integer || d == SadTypeKind::Float));
                                    if (dynVal || mismatch)
                                    {
                                        fit->second = SadTypeKind::Any;
                                        changed = true;
                                    }
                                }
                            }

                            // (AR) 4) إرجاعُ قيمةٍ ديناميّة ⇒ نوع إرجاع الدالّة Any (%SadDyn)
                            //         — كان dynPayloadI64 يقتطع الوسم عند حدود الدالّة
                            // (EN) 4) returning a dynamic value ⇒ Any (%SadDyn) return type —
                            //         dynPayloadI64 used to strip the kind at the boundary
                            if (inst.opcode == SIROpcode::RET && !inst.operands.empty() &&
                                fn->returnType != SadTypeKind::Any &&
                                fn->returnType != SadTypeKind::Void &&
                                valueIsDyn(inst.operands[0]))
                            {
                                fn->returnType = SadTypeKind::Any;
                                dynReturnFuncs[fn->getName()] = true;
                                changed = true;
                            }
                        }
                    }
                }
            }
        }

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

        llvm::Value *unpackI64(LLVMCodeGen &cg, llvm::Value *dyn)
        {
            auto &b = *cg.builder_;
            auto &ctx = *cg.context_;
            auto *i8 = llvm::Type::getInt8Ty(ctx);
            auto *i64 = llvm::Type::getInt64Ty(ctx);
            auto *dbl = llvm::Type::getDoubleTy(ctx);
            llvm::Value *payload = dynPayloadI64(cg, dyn);
            llvm::Value *isF = b.CreateICmpEQ(
                dynKindByte(cg, dyn), llvm::ConstantInt::get(i8, DynKind::Float), "dyn.i.isf");
            // (AR) دلالة الإشباع الموحَّدة منصّيًّا (fptosi.sat) — كمواضع البتّيّات في
            //      arith_main/arith_cmp، وإلّا يتباعد مسار SadDyn عن الدلالة المعلَنة.
            // (EN) Platform-independent saturation (fptosi.sat) — matches the bitwise
            //      sites in arith_main/arith_cmp; a raw fptosi here would make the
            //      SadDyn path diverge from the declared semantics.
            llvm::Function *satFn = llvm::Intrinsic::getDeclaration(
                cg.module_.get(), llvm::Intrinsic::fptosi_sat, {i64, dbl});
            llvm::Value *fromF = b.CreateCall(
                satFn, {b.CreateBitCast(payload, dbl, "dyn.i.fbc")}, "dyn.i.f2i.sat");
            return b.CreateSelect(isF, fromF, payload, "dyn.i64");
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

        // (AR) حارس القاسم العشريّ الصفريّ للمسار الديناميّ (د-1 + إغلاق بوّابتَي
        //      NaN المتبقّيتين RUN009/RUN010 — مرآة emitFloatDivZeroGuard في المسار
        //      الساكن): failCond جاهز عند النداء (عادةً eitherF ∧ القاسم == 0.0 —
        //      أو intDivZero للفرع الصحيح — رسالة %g أو %lld بحسب المقسوم). مستضاف ⇒ تشخيص عربيّ
        //      بالكتالوج (hostedMsg بموضع %g للمقسوم) + exit(1)؛ حرّ ⇒ __sad_panic.
        // (EN) Dynamic-path float zero-divisor guard (D-1 + closing the remaining
        //      NaN gates RUN009/RUN010 — mirror of the static-path
        //      emitFloatDivZeroGuard): failCond is precomputed by the caller
        //      (typically eitherF ∧ divisor == 0.0 — the integer branch keeps its
        //      or intDivZero for the int branch — %g or %lld message to match the
        //      dividend). Hosted ⇒ Arabic catalog diagnostic
        //      (hostedMsg with %g for the dividend) + exit(1); freestanding ⇒
        //      __sad_panic(check-violation).
        static void emitDynDivZeroGuard(LLVMCodeGen &cg, llvm::Value *failCond,
                                             llvm::Value *dividendD,
                                             const char *hostedMsg, const char *tag)
        {
            auto &b = *cg.builder_;
            auto &ctx = *cg.context_;
            llvm::Function *curFunc = b.GetInsertBlock()->getParent();
            llvm::BasicBlock *failBB =
                llvm::BasicBlock::Create(ctx, std::string(tag) + ".fail", curFunc);
            llvm::BasicBlock *contBB =
                llvm::BasicBlock::Create(ctx, std::string(tag) + ".ok", curFunc);
            b.CreateCondBr(failCond, failBB, contBB);

            b.SetInsertPoint(failBB);
            if (cg.freestanding_)
            {
                cg.emitFreestandingPanicCall(Sad::Compiler::kSadPanicCheckViolation);
            }
            else
            {
                auto *ptrTy = llvm::PointerType::getUnqual(ctx);
                auto *printfType = llvm::FunctionType::get(
                    llvm::Type::getInt32Ty(ctx), {ptrTy}, true);
                auto printfFunc = cg.module_->getOrInsertFunction("printf", printfType);
                llvm::Value *msg = b.CreateGlobalStringPtr(
                    hostedMsg, std::string(tag) + ".fmt");
                b.CreateCall(printfFunc, {msg, dividendD});
                auto *exitType = llvm::FunctionType::get(
                    llvm::Type::getVoidTy(ctx), {llvm::Type::getInt32Ty(ctx)}, false);
                auto exitFunc = cg.module_->getOrInsertFunction("exit", exitType);
                b.CreateCall(exitFunc,
                             {llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 1)});
            }
            b.CreateUnreachable();

            b.SetInsertPoint(contBB);
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
            // (AR) Amelia (ISSUE-063): توسيع الحارس — إلى جانب الفرع العشريّ، القاسمُ
            //      الصحيح صفر (sdiv/srem ⇒ #DE انهيار) وINT64_MIN/-1 (فيض sdiv ⇒ #DE أيضًا)
            //      يستبدلان القاسمَ 1؛ ثمّ تُفرَض النتيجة 0 لحالة القسمة على صفر (سياسة
            //      المسار الساكن المترجم نفسها — المفسّر يرمي RUN001/RUN009، تباعدٌ موثَّق).
            // (EN) Amelia (ISSUE-063): widen the guard — besides the float branch, an integer
            //      zero divisor (sdiv/srem ⇒ #DE crash) and INT64_MIN/-1 (sdiv overflow ⇒ #DE
            //      too) substitute divisor 1. The int zero-divisor case is no longer a
            //      silent result-0 policy: it is rejected by the guards below with
            //      RUN001/RUN009/RUN010, matching the interpreter (divergence closed).
            llvm::Value *dynZero64 = llvm::ConstantInt::get(i64, 0);
            llvm::Value *intDivZero = b.CreateAnd(
                b.CreateNot(eitherF, "dyn.not.f"),
                b.CreateICmpEQ(rI, dynZero64, "dyn.rz"), "dyn.idivz");
            llvm::Value *minOverflow = b.CreateAnd(
                b.CreateICmpEQ(lI, llvm::ConstantInt::get(
                                       i64, llvm::APInt::getSignedMinValue(64)), "dyn.lmin"),
                b.CreateICmpEQ(rI, llvm::ConstantInt::getSigned(i64, -1), "dyn.rneg1"),
                "dyn.minov");
            llvm::Value *unsafeDivisor = b.CreateOr(
                eitherF, b.CreateOr(intDivZero, minOverflow, "dyn.iunsafe"), "dyn.unsafe");
            llvm::Value *safeRI = b.CreateSelect(
                unsafeDivisor, llvm::ConstantInt::get(i64, 1), rI, "dyn.safe.divisor");

            // (AR) نحسب النتيجتين العشريّة والصحيحة ثمّ نختار زمنَ التشغيل حسب الوسم؛ يطابق
            //      المفسّر: عشريّ ⇒ fadd/… (و% ⇒ frem أي fmod، // ⇒ floor(fdiv))؛ صحيح ⇒ add/…
            //      (و% ⇒ srem، // ⇒ sdiv). المعاملات الصحيحة تُقرأ من الحمولة مباشرة (نظيفة).
            // (EN) Compute both the float and integer results, then runtime-select by the tag,
            //      matching the interpreter: float ⇒ fadd/… (% ⇒ frem i.e. fmod, // ⇒ floor(fdiv));
            //      int ⇒ add/… (% ⇒ srem, // ⇒ sdiv). Int operands read the payload directly (clean).
            llvm::Value *fRes = nullptr;
            llvm::Value *iRes = nullptr;
            // (AR) ISSUE-063: وسمُ النتيجة قد يتجاوز eitherF لبعض العمليّات (القسمة `/`
            //      على صحيحين بباقٍ ⇒ عشريّ) — يبدأ بـeitherF وتعدّله الحالة عند الحاجة.
            // (EN) ISSUE-063: the result kind may exceed eitherF for some ops (`/` on two
            //      ints with a remainder ⇒ float) — starts as eitherF, cases may extend it.
            llvm::Value *isFloatRes = eitherF;

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
            {
                // (AR) ISSUE-063: دلالة المفسّر للقسمة `/` على صحيحين: صحيح عند انعدام
                //      الباقي (6/3=2) وعشريّ عند وجوده (7/2=3.5). نحسب srem؛ على الفرع
                //      العشريّ safeRI=1 ⇒ الباقي 0 ولا أثر (يحسم eitherF). fdiv(lD,rD)
                //      صحيحٌ في الحالتين لأنّ unpackDouble يرقّي الحمولة الصحيحة sitofp.
                // (EN) ISSUE-063: interpreter semantics for `/` on two ints: int when the
                //      remainder is zero (6/3=2), float otherwise (7/2=3.5). Compute srem;
                //      on the float branch safeRI=1 ⇒ remainder 0, no effect (eitherF wins).
                //      fdiv(lD,rD) is correct either way since unpackDouble sitofp-promotes
                //      integer payloads.
                // (AR) حارس القسمة العشريّة على صفر (د-1، توافق المحرّكين): فرعٌ
                //      عشريّ (eitherF) بمقسومٍ عليه 0.0 كان يُنتج nan/inf بصمت —
                //      نفس بوّابة NaN التي سُدَّت في المسار الساكن (emitDiv). المفسّر
                //      يرمي RUN001؛ هنا نرفض زمنيًّا قبل fdiv: مستضاف ⇒ تشخيص RUN001
                //      العربيّ + exit(1)؛ حرّ ⇒ __sad_panic برمز انتهاك الفحص.
                //      والفرع الصحيح (قاسم صفر صحيح) يُرفض بـRUN001 كذلك — سياسة
                //      النتيجة 0 الصامتة أُلغيت (سدّ تباعد الصحيحين).
                // (EN) Float division-by-zero guard (D-1, engine parity): a float
                //      branch (eitherF) with divisor 0.0 silently produced nan/inf —
                //      the same NaN gate closed on the static path (emitDiv). The
                //      interpreter throws RUN001; reject at runtime before fdiv:
                //      hosted ⇒ Arabic RUN001 diagnostic + exit(1); freestanding ⇒
                //      __sad_panic(check-violation). The integer branch (int zero
                //      divisor) is rejected with RUN001 too — the silent result-0
                //      policy is gone (int divergence closed).
                emitDynDivZeroGuard(
                    cg,
                    b.CreateAnd(eitherF,
                                b.CreateFCmpOEQ(rD, llvm::ConstantFP::get(dbl, 0.0),
                                                "dyn.fdiv.rz"),
                                "dyn.fdivz"),
                    lD, Sad::Compiler::kDivZeroRun001Msg, "dyn.fdiv.dz");
                // (AR) والفرع الصحيح: قاسم صفر ⇒ RUN001 كالمفسّر (كانت سياسة 0 صامتة)
                // (EN) Int branch: zero divisor ⇒ RUN001 like the interpreter
                //      (was a silent result-0 policy)
                emitDynDivZeroGuard(cg, intDivZero, lI,
                                    Sad::Compiler::kDivZeroRun001IntMsg, "dyn.idiv.dz");
                fRes = b.CreateFDiv(lD, rD, "dyn.fdiv");
                iRes = b.CreateSDiv(lI, safeRI, "dyn.idiv");
                llvm::Value *rem = b.CreateSRem(lI, safeRI, "dyn.div.rem");
                llvm::Value *inexact = b.CreateICmpNE(
                    rem, llvm::ConstantInt::get(i64, 0), "dyn.div.inexact");
                // (AR) INT64_MIN / -1 يفيض i64 ⇒ المفسّر (المرجع) يرقّيه إلى عشريّ
                //      (-(double)INT64_MIN = 9223372036854775808.0). fRes = fdiv(lD,rD) يحسبه
                //      أصلًا؛ نضمّ minOverflow لوسم النتيجة عشريّةً بدل إرجاع INT64_MIN صحيحًا.
                // (EN) INT64_MIN / -1 overflows i64 ⇒ the interpreter (reference) promotes it
                //      to a float (-(double)INT64_MIN = 9223372036854775808.0). fRes = fdiv(lD,rD)
                //      already yields it; OR in minOverflow to tag the result float instead of
                //      returning the wrapped integer INT64_MIN.
                isFloatRes = b.CreateOr(b.CreateOr(eitherF, inexact, "dyn.div.isf0"),
                                        minOverflow, "dyn.div.isf");
                break;
            }
            case SIROpcode::MOD_I64:
                // (AR) % : عشريّ ⇒ frem (fmod، مثل المفسّر 7.5%2=1.5)؛ صحيح ⇒ srem.
                //      حارس القاسم الصفريّ العشريّ ⇒ RUN010 (المفسّر يرمي؛ كان frem
                //      يُنتج NaN بصمت)، والفرع الصحيح ⇒ RUN010 كذلك (كانت سياسة 0 صامتة).
                // (EN) % : float ⇒ frem (fmod, like the interpreter 7.5%2=1.5); int ⇒ srem.
                //      Float zero-divisor guard ⇒ RUN010 (interpreter throws; frem
                //      silently produced NaN); int branch ⇒ RUN010 too (was silent 0).
                emitDynDivZeroGuard(
                    cg,
                    b.CreateAnd(eitherF,
                                b.CreateFCmpOEQ(rD, llvm::ConstantFP::get(dbl, 0.0),
                                                "dyn.frem.rz"),
                                "dyn.fremz"),
                    lD, Sad::Compiler::kModZeroRun010Msg, "dyn.frem.dz");
                emitDynDivZeroGuard(cg, intDivZero, lI,
                                    Sad::Compiler::kModZeroRun010IntMsg, "dyn.srem.dz");
                fRes = b.CreateFRem(lD, rD, "dyn.frem");
                iRes = b.CreateSRem(lI, safeRI, "dyn.srem");
                break;
            case SIROpcode::FLOOR_DIV_I64:
            {
                // (AR) // : عشريّ ⇒ floor(fdiv) (مثل المفسّر 7.5//2=3.0)؛ صحيح ⇒ sdiv.
                // (EN) // : float ⇒ floor(fdiv) (like the interpreter 7.5//2=3.0); int ⇒ sdiv.
                // (AR) حارس القاسم الصفريّ العشريّ ⇒ RUN009 (المفسّر يرمي؛ كان
                //      fdiv/floor يُنتج nan/inf بصمت)، والفرع الصحيح ⇒ RUN009 كذلك.
                // (EN) Float zero-divisor guard ⇒ RUN009 (interpreter throws;
                //      fdiv/floor silently produced nan/inf); int branch ⇒ RUN009 too.
                emitDynDivZeroGuard(
                    cg,
                    b.CreateAnd(eitherF,
                                b.CreateFCmpOEQ(rD, llvm::ConstantFP::get(dbl, 0.0),
                                                "dyn.fd.rz"),
                                "dyn.fdz"),
                    lD, Sad::Compiler::kFloorDivZeroRun009Msg, "dyn.ffd.dz");
                emitDynDivZeroGuard(cg, intDivZero, lI,
                                    Sad::Compiler::kFloorDivZeroRun009IntMsg,
                                    "dyn.ifd.dz");
                llvm::Value *q = b.CreateFDiv(lD, rD, "dyn.fdiv.q");
                llvm::Function *floorFn = llvm::Intrinsic::getDeclaration(
                    cg.module_.get(), llvm::Intrinsic::floor, {dbl});
                fRes = b.CreateCall(floorFn, {q}, "dyn.floor");
                // (AR) Amelia (ISSUE-063): تسويةٌ أرضيّة للفرع الصحيح (-7//2=-4 كالمفسّر)
                //      بدل اقتطاع sdiv نحو الصفر (-3).
                // (EN) Amelia (ISSUE-063): floor adjustment on the integer branch
                //      (-7//2=-4 like the interpreter) instead of sdiv truncation (-3).
                llvm::Value *iq = b.CreateSDiv(lI, safeRI, "dyn.sdiv");
                llvm::Value *irem = b.CreateSRem(lI, safeRI, "dyn.sdiv.rem");
                llvm::Value *signsDiffer = b.CreateICmpSLT(
                    b.CreateXor(lI, safeRI, "dyn.fd.sx"), dynZero64, "dyn.fd.sd");
                llvm::Value *inexactI = b.CreateICmpNE(irem, dynZero64, "dyn.fd.ix");
                llvm::Value *needAdj = b.CreateAnd(signsDiffer, inexactI, "dyn.fd.na");
                iRes = b.CreateSub(iq, b.CreateZExt(needAdj, i64, "dyn.fd.adj"), "dyn.fd.q");
                // (AR) INT64_MIN // -1 يفيض ⇒ المفسّر يرقّيه إلى عشريّ (9223372036854775808.0)
                //      كالقسمة `/`. fRes = floor(fdiv(lD,rD)) يحسبه؛ نضمّ minOverflow لوسمه عشريًّا.
                // (EN) INT64_MIN // -1 overflows ⇒ the interpreter promotes to float
                //      (9223372036854775808.0) like `/`. fRes = floor(fdiv(lD,rD)) yields it;
                //      OR in minOverflow to tag the result float.
                isFloatRes = b.CreateOr(eitherF, minOverflow, "dyn.fd.isf");
                break;
            }
            default:
                fRes = b.CreateFAdd(lD, rD, "dyn.fadd.def");
                iRes = b.CreateAdd(lI, rI, "dyn.iadd.def");
                break;
            }

            llvm::Value *resKind = b.CreateSelect(
                isFloatRes, llvm::ConstantInt::get(i8, DynKind::Float),
                llvm::ConstantInt::get(i8, DynKind::Int), "dyn.res.kind");
            llvm::Value *fBits = b.CreateBitCast(fRes, i64, "dyn.res.fbits");
            llvm::Value *resPayload = b.CreateSelect(isFloatRes, fBits, iRes, "dyn.res.payload");
            return makeDyn(cg, resKind, resPayload);
        }

        llvm::Value *dynCompare(LLVMCodeGen &cg, DynCmp cmp, llvm::Value *l, llvm::Value *r)
        {
            auto &b = *cg.builder_;
            auto &ctx = *cg.context_;
            auto *i8 = llvm::Type::getInt8Ty(ctx);
            auto *i1 = llvm::Type::getInt1Ty(ctx);
            auto *i32 = llvm::Type::getInt32Ty(ctx);
            auto *ptrTy = llvm::PointerType::getUnqual(ctx);

            // (AR) م1ب ISSUE-076: كلا المعامِلين نصّ (وسم Str) ⇒ قارِن بالمحتوى عبر strcmp لا
            //      الحمولةَ (المؤشّرات — عناوين malloc تختلف لنصّين متطابقين). فرعٌ زمنَ التشغيل
            //      لأنّ strcmp غير آمن على حمولةٍ غير مؤشّريّة (صحيح/عشريّ). يخدم كامل عائلة %SadDyn:
            //      EQ/NE (عبر emitDynamicEqNe) وLT/LE/GT/GE (عبر emitCmpLt…) يوحّدان الطرفين هنا.
            // (EN) م1ب ISSUE-076: both operands are strings (Str tag) ⇒ compare by content via strcmp,
            //      not the payload (pointers — distinct malloc addresses for equal strings). A runtime
            //      branch since strcmp is unsafe on a non-pointer payload (int/float). Serves the whole
            //      %SadDyn family: EQ/NE (via emitDynamicEqNe) and LT/LE/GT/GE (via emitCmpLt…) unify here.
            llvm::Value *strK = llvm::ConstantInt::get(i8, DynKind::Str);
            llvm::Value *bothStr = b.CreateAnd(
                b.CreateICmpEQ(dynKindByte(cg, l), strK, "dyn.cmp.lstr"),
                b.CreateICmpEQ(dynKindByte(cg, r), strK, "dyn.cmp.rstr"),
                "dyn.cmp.bothstr");

            auto *parent = b.GetInsertBlock()->getParent();
            auto *strBB = llvm::BasicBlock::Create(ctx, "dyn.cmp.str", parent);
            auto *numBB = llvm::BasicBlock::Create(ctx, "dyn.cmp.num", parent);
            auto *mergeBB = llvm::BasicBlock::Create(ctx, "dyn.cmp.merge", parent);
            b.CreateCondBr(bothStr, strBB, numBB);

            // (AR) فرع النصّ: strcmp(المحتوى، المحتوى) ثمّ قارِن الناتج بالصفر حسب المُقارِن
            //      (نفس اصطلاح strcmp المستعمل في emitCmpEq للنصوص العاديّة). / (EN) String branch:
            //      strcmp(content, content), then compare the result to zero per the operator.
            b.SetInsertPoint(strBB);
            llvm::Value *lp = unpackPtr(cg, l);
            llvm::Value *rp = unpackPtr(cg, r);
            auto *strcmpTy = llvm::FunctionType::get(i32, {ptrTy, ptrTy}, false);
            auto strcmpFn = cg.module_->getOrInsertFunction("strcmp", strcmpTy);
            llvm::Value *sc = b.CreateCall(strcmpFn, {lp, rp}, "dyn.cmp.strcmp");
            llvm::Value *z32 = llvm::ConstantInt::get(i32, 0);
            llvm::Value *strRes = nullptr;
            switch (cmp)
            {
            case DynCmp::EQ: strRes = b.CreateICmpEQ(sc, z32, "dyn.cmp.seq"); break;
            case DynCmp::NE: strRes = b.CreateICmpNE(sc, z32, "dyn.cmp.sne"); break;
            case DynCmp::LT: strRes = b.CreateICmpSLT(sc, z32, "dyn.cmp.slt"); break;
            case DynCmp::LE: strRes = b.CreateICmpSLE(sc, z32, "dyn.cmp.sle"); break;
            case DynCmp::GT: strRes = b.CreateICmpSGT(sc, z32, "dyn.cmp.sgt"); break;
            case DynCmp::GE: strRes = b.CreateICmpSGE(sc, z32, "dyn.cmp.sge"); break;
            }
            b.CreateBr(mergeBB);
            strBB = b.GetInsertBlock();

            // (AR) فرع العدد (المنطق الأصليّ محفوظًا حرفيًّا): عشريّ⇒fcmp، صحيح⇒icmp موقَّع على الحمولة.
            // (EN) Numeric branch (original logic preserved verbatim): float⇒fcmp, int⇒signed icmp on payload.
            b.SetInsertPoint(numBB);
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
            llvm::Value *numRes = b.CreateSelect(eitherF, fRes, iRes, "dyn.cmp.res");
            b.CreateBr(mergeBB);
            numBB = b.GetInsertBlock();

            // (AR) الدمج: النتيجة من فرع النصّ أو العدد. / (EN) Merge: result from the string or numeric branch.
            b.SetInsertPoint(mergeBB);
            auto *phi = b.CreatePHI(i1, 2, "dyn.cmp.merge.res");
            phi->addIncoming(strRes, strBB);
            phi->addIncoming(numRes, numBB);
            return phi;
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
            // (AR) بانٍ محلّيّ (b) ⇒ تصريح مباشر بنوع size_t الهدف.
            llvm::Type *szTy = cg.getSizeType();
            auto *mallocTy = llvm::FunctionType::get(ptrTy, {szTy}, false);
            auto mallocFn = cg.module_->getOrInsertFunction("malloc", mallocTy);
            llvm::Value *buf = b.CreateCall(mallocFn, {llvm::ConstantInt::get(szTy, 512)}, "dyn.ts.buf");

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
