/*
 * ============================================================================
 * LLVM IR Code Generator - Branch and Call Operations
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
#include <llvm/IR/Intrinsics.h>
#include <optional>
#include <iostream>
#include <fstream>
#include "builders/arithmetic/controlflow_codegen.h" // (Phase 7 Step 3)
#include "llvm_codegen.h"
#include "sad_dyn_repr.h"
// (AR) منافذُ النصِّ الهابطةُ نداءً — لتردَّ مستقبِلَها العدميَّ إلى بابِ الرفع
#include "string_runtime_ports.h"

using namespace Sad::Compiler::SIR;
using namespace Sad::Compiler; // (AR) للوصول لثوابت sir_constants.h

namespace Sad
{
    namespace LLVM
    {
        llvm::Value *ControlFlowCodeGen::emitBranch(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            // (AR) لا نضيف branch إذا كان البلوك الحالي ينتهي بـ ret
            // (EN) Skip branch if current block already has a terminator (e.g. ret)
            llvm::BasicBlock *currentBB = cg_.builder_->GetInsertBlock();
            if (currentBB && currentBB->getTerminator())
            {
                return nullptr; // Block already terminated
            }

            if (inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "Branch"}});
                return nullptr;
            }

            // Source: SIROperand::name is PUBLIC member at sir_types.h:293
            std::string targetLabel = inst->operands[0].name;

            auto it = cg_.context_info_.basicBlocks.find(targetLabel);
            if (it == cg_.context_info_.basicBlocks.end())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_UNDEFINED_REF, {{"detail", std::string("Target block not found for branch:") + targetLabel}});
                return nullptr;
            }

            return cg_.builder_->CreateBr(it->second);
        }

        /**
         * إصدار تعليمة فرع مشروط
         * Emit conditional branch instruction
         *
         * Source: llvm_codegen.h:454
         */
        llvm::Value *ControlFlowCodeGen::emitCondBranch(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            // (AR) لا نضيف branch إذا كان البلوك الحالي ينتهي بـ ret
            // (EN) Skip conditional branch if current block already has a terminator
            llvm::BasicBlock *currentBB = cg_.builder_->GetInsertBlock();
            if (currentBB && currentBB->getTerminator())
            {
                return nullptr; // Block already terminated
            }

            if (inst->operands.size() < 3)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "Conditional"}});
                return nullptr;
            }

            llvm::Value *condition = cg_.resolveOperand(inst->operands[0]);
            std::string trueLabel = inst->operands[1].name;
            std::string falseLabel = inst->operands[2].name;

#ifndef NDEBUG
            std::cout << "[DEBUG] emitCondBranch: looking for trueLabel='" << trueLabel
                      << "', falseLabel='" << falseLabel << "'" << std::endl;
            std::cout << "[DEBUG] emitCondBranch: registered blocks count=" << cg_.context_info_.basicBlocks.size() << std::endl;
            for (const auto &[name, bb] : cg_.context_info_.basicBlocks)
            {
                std::cout << "[DEBUG] emitCondBranch: registered block '" << name << "'" << std::endl;
            }
#endif

            if (!condition)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_UNDEFINED_REF, {{"detail", "Condition"}});
                return nullptr;
            }

            // (AR) التأكد من أن الشرط من نوع i1 — LLVM CreateCondBr يتطلب i1
            //      إذا كان الشرط ptr، نقارنه بـ null لتحويله إلى i1
            //      إذا كان الشرط i64 أو غيره، نقارنه بالصفر لتحويله إلى i1
            //      [إصلاح BF-04] السبب الجذري: ptr لا يقبل ConstantInt — يجب ConstantPointerNull
            // (EN) Ensure condition is i1 — LLVM CreateCondBr requires i1
            //      If condition is ptr, compare != null to convert to i1
            //      If condition is i64 or other integer, compare != 0 to convert to i1
            //      [Fix BF-04] Root cause: ptr doesn't accept ConstantInt — must use ConstantPointerNull
            if (!condition->getType()->isIntegerTy(1))
            {
                if (condition->getType()->isPointerTy())
                {
                    // (AR) الشرط من نوع مؤشر — نقارنه بـ null
                    // (EN) Condition is pointer type — compare with null
                    condition = cg_.builder_->CreateICmpNE(
                        condition,
                        llvm::ConstantPointerNull::get(
                            llvm::cast<llvm::PointerType>(condition->getType())),
                        "tobool.ptr");
                }
                else
                {
                    // (AR) الشرط من نوع عدد صحيح أو آخر — نقارنه بالصفر
                    //      [إصلاح BF-04 (TE-001)] لاشيء يُمثَّل بـ kSadNullSentinel (≠ 0)،
                    //      لذا فحص `cond != 0` وحده يجعل لاشيء truthy خطأً.
                    //      الدلالة الموحدة مع المفسر: لاشيء و 0 falsy، وما عداهما truthy.
                    //      الحل الجذري: للأعداد الصحيحة بحجم 64-بت نفحص:
                    //      `cond != 0 && cond != kSadNullSentinel`
                    // (EN) Integer/other condition — compare against zero.
                    //      [BF-04 fix (TE-001)] null is represented as kSadNullSentinel (≠ 0),
                    //      so checking `cond != 0` alone makes null truthy by mistake.
                    //      Unified semantics with interpreter: null and 0 are falsy.
                    //      Root fix: for i64, check `cond != 0 && cond != kSadNullSentinel`.
                    auto *zeroConst = llvm::ConstantInt::get(condition->getType(), 0);
                    llvm::Value *neZero = cg_.builder_->CreateICmpNE(condition, zeroConst, "tobool.nz");

                    // (AR) [إصلاح تصادم kSadNullSentinel] لا نطبّق فحص حارس العدم على شرطٍ
                    //      نوعُه الساكن طبيعي64/بايت: الحارس قيمةٌ i64 بعينها (2^63+1 لا-موقَّعًا)
                    //      وطبيعي64 شرعيّ قد يساويها ⇒ كان يُعامَل «عدمًا» (falsy) خطأً. طبيعي64/
                    //      بايت لا يكونان نوعَ العدم (العدمُ Integer)، فصدقُهما «≠ 0» فقط، مطابقةً
                    //      للمفسّر. Integer **مُستثنى** (يتصادم جوهريًّا مع العدم)، والأنواع
                    //      النِّلابليّة (Any/مجهول) يبقى لها الفحص (عدمٌ حقيقيّ قد يصل i64).
                    // (EN) [kSadNullSentinel collision fix] Do not apply the null-sentinel check
                    //      to a condition whose static type is طبيعي64/Byte: the sentinel is one
                    //      specific i64 value (2^63+1 unsigned) a legitimate طبيعي64 can equal ⇒ it
                    //      was wrongly treated as null (falsy). طبيعي64/Byte are never the null type
                    //      (null is Integer), so their truth is just «!= 0», matching the
                    //      interpreter. Integer is EXCLUDED (it collides intrinsically with null);
                    //      nullable types (Any/Unknown) keep the check (a real null may arrive i64).
                    const auto condType = inst->operands[0].dataType;
                    const bool condIsNonNullableNum =
                        condType == SadTypeKind::UInt64 ||
                        condType == SadTypeKind::Byte;

                    if (condition->getType()->isIntegerTy(64) && !condIsNonNullableNum)
                    {
                        auto *nullSentinel = llvm::ConstantInt::get(condition->getType(),
                                                                    static_cast<uint64_t>(kSadNullSentinel),
                                                                    /*isSigned=*/true);
                        llvm::Value *neNull = cg_.builder_->CreateICmpNE(condition, nullSentinel, "tobool.nn");
                        condition = cg_.builder_->CreateAnd(neZero, neNull, "tobool");
                    }
                    else
                    {
                        condition = neZero;
                    }
                }
            }

            auto trueIt = cg_.context_info_.basicBlocks.find(trueLabel);
            auto falseIt = cg_.context_info_.basicBlocks.find(falseLabel);

            if (trueIt == cg_.context_info_.basicBlocks.end() ||
                falseIt == cg_.context_info_.basicBlocks.end())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_UNDEFINED_REF, {{"detail", "Target"}});
                return nullptr;
            }

            return cg_.builder_->CreateCondBr(condition, trueIt->second, falseIt->second);
        }

        /**
         * إصدار تعليمة استدعاء دالة
         * Emit function call instruction
         *
         * Source: llvm_codegen.h:455
         */
        llvm::Value *ControlFlowCodeGen::emitCall(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "Call"}});
                return nullptr;
            }

            std::string funcName = inst->operands[0].name;

            // جمع المعاملات أولاً (نحتاجها لاستنتاج التوقيع إذا كانت الدالة خارجية)
            // Collect arguments first (needed to infer signature for external functions)
            std::vector<llvm::Value *> args;
            for (size_t i = 1; i < inst->operands.size(); ++i)
            {
                llvm::Value *arg = cg_.resolveOperand(inst->operands[i]);
                if (arg)
                {
                    args.push_back(arg);
                }
            }

            // ════════════════════════════════════════════════════════════════════
            // (AR) 🔑 منفذُ **مستقبِلٍ** نصّيٍّ يهبطُ نداءً لا أوپكودًا: «عكس» و«كرر»
            //      و«حرف_عند». وبابُ مؤشّرِ النصِّ يحرسُ بواعثَ الأوپكوداتِ وحدَها، فكان
            //      «س.عكس()» على خانةٍ عدميّةٍ **ينهارُ انهيارَ تجزئة** بينما يرفعُ
            //      المفسّرُ RUN033 — مقيسٌ، كشفه حارسُ تكافؤِ المستقبِلِ العدميّ.
            //      فيمرُّ المعاملُ الأوّلُ من البابِ نفسِه ليتطابقَ الرمزُ والنصُّ معًا.
            //      والاسمُ العربيُّ من الرأسِ المشترك، لا مكتوبًا هنا ثانيةً.
            // (EN) 🔑 A string RECEIVER port that lowers to a CALL, not an opcode (reverse,
            //      repeat, char_at): the door guards opcode emitters only, so a null receiver
            //      segfaulted where the interpreter raises RUN033 (measured; found by the
            //      null-receiver parity guard). Route operand 0 through the same door.
            // ════════════════════════════════════════════════════════════════════
            {
                const std::string_view receiverMethod =
                    ::Sad::Compiler::StringRuntimePorts::receiverMethodName(funcName);
                // (AR) ⚠️ الحكمُ بعددِ **المعاملاتِ** لا بعددِ الوسائطِ المحلولة: الحلقةُ
                //      أعلاه تُسقِطُ معاملًا يفشلُ حلُّه بلا حجزِ مكان، فينزلقُ الفهرس؛
                //      ولو انزلق لصار `args[0]` عددًا فيُقرأ الصفرُ عدمًا ⇒ RUN033 كاذب.
                //      فإن لم يتطابقِ العددان تُركَ النداءُ كما هو: بابٌ لا يُطبَّق أهونُ
                //      من بابٍ يُطبَّق على المعاملِ الخطأ.
                // (EN) ⚠️ Judge by OPERAND count, not resolved-arg count: the loop above drops
                //      an operand that fails to resolve without reserving its slot, so indices
                //      slide and args[0] could be the numeric argument — whose 0 reads as null
                //      ⇒ a false RUN033. On mismatch, leave the call untouched.
                const bool operandsIntact = inst->operands.size() == args.size() + 1;
                if (!receiverMethod.empty() && operandsIntact && !args.empty())
                {
                    args[0] = cg_.emitStringPtrOrRaise(
                        args[0], LLVMCodeGen::stringMethodOperationLabel(receiverMethod),
                        "rt_string.recv");
                }
            }

            // ════════════════════════════════════════════════════════════════════
            // (AR) [RFC #53 F2-ج] نداء دالّة خارجيّة بمعامل/عائد بنية @تمثيل_سي بالقيمة.
            //      خطّةٌ محسوبة سلفًا (emitFunctionPrototype، تصاريح-أوّلًا) تحدّد شكل ABI؛
            //      نولّف الوسائط لتطابق التوقيعَ تمامًا:
            //        Direct  ⇒ حمّل قطع البنية (ثمانيّة/ثمانيّتين) من ذاكرة الكائن ومرّرها؛
            //        Memory  ⇒ مرّر مؤشّر الكائن بسمة byval (LLVM ينسخ ويطبّق ABI الهدف)؛
            //        عائد Direct ⇒ جسّد كائنًا (malloc+memset) واكتب القطعَ فيه؛
            //        عائد Memory ⇒ خصّص كائنًا ومرّره sret أوّلًا، فالعائد هو ذاك الكائن.
            // (EN) [RFC #53 F2-ج] Call an extern with a by-value @تمثيل_سي struct param/return.
            //      A precomputed plan (prototypes-first) fixes the ABI shape; marshal args to
            //      match exactly (Direct ⇒ load struct pieces; Memory ⇒ byval pointer; Direct
            //      return ⇒ materialize object; Memory return ⇒ sret first arg).
            // ════════════════════════════════════════════════════════════════════
            {
                auto planIt = cg_.context_info_.creprCallPlans.find(funcName);
                if (planIt != cg_.context_info_.creprCallPlans.end() && planIt->second.active)
                {
                    const CReprCallPlan &plan = planIt->second;
                    llvm::Function *callee = cg_.module_->getFunction(plan.linkSymbol);
                    if (callee)
                    {
                        llvm::LLVMContext &ctx = *cg_.context_;
                        llvm::IRBuilder<> &b = *cg_.builder_;
                        const llvm::DataLayout &DL = cg_.module_->getDataLayout();
                        llvm::Type *i8Ty = llvm::Type::getInt8Ty(ctx);
                        llvm::Type *i64Ty = llvm::Type::getInt64Ty(ctx);
                        llvm::Type *ptrTy = llvm::PointerType::getUnqual(ctx);

                        std::vector<llvm::Value *> callArgs;
                        std::vector<std::pair<unsigned, llvm::Type *>> callByval; // (فهرس، نوع)
                        llvm::Value *sretObj = nullptr;
                        llvm::Type *sretStructTy = nullptr;
                        llvm::FunctionType *ft = callee->getFunctionType();

                        // (AR) تكييف وسيطٍ عدديّ إلى نوع معامل المُستدعى (مثل الحلقة العامّة:
                        //      ptr↔i64، i1↔i64، عشريّ↔صحيح) كي لا يرفض المدقّقُ المزجَ
                        //      «بنية + عنوان رقم» في توقيعٍ خارجيّ واحد.
                        // (EN) Coerce a scalar arg to the callee param type (like the generic
                        //      loop: ptr↔i64, i1↔i64, float↔int) so the verifier accepts a
                        //      mixed «struct + رقم address» extern signature.
                        auto coerceScalar = [&](llvm::Value *v, llvm::Type *want) -> llvm::Value *
                        {
                            if (!want || v->getType() == want)
                                return v;
                            llvm::Type *have = v->getType();
                            if (want->isPointerTy() && have->isIntegerTy())
                                return b.CreateIntToPtr(v, want, "crepr.s.i2p");
                            if (want->isIntegerTy(64) && have->isPointerTy())
                                return b.CreatePtrToInt(v, want, "crepr.s.p2i");
                            if (want->isIntegerTy(64) && have->isIntegerTy(1))
                                return b.CreateZExt(v, want, "crepr.s.b2i");
                            if (want->isIntegerTy(1) && have->isIntegerTy(64))
                                return b.CreateTrunc(v, want, "crepr.s.i2b");
                            if (want->isDoubleTy() && have->isIntegerTy())
                                return b.CreateSIToFP(v, want, "crepr.s.i2f");
                            if (want->isIntegerTy() && have->isDoubleTy())
                                return b.CreateFPToSI(v, want, "crepr.s.f2i");
                            return v;
                        };

                        // (1) عائد بالذاكرة: خصّص كائن النتيجة ومرّر مؤشّره sret أوّلًا.
                        if (plan.sretReturn)
                        {
                            auto sIt = cg_.context_info_.classStructTypes.find(plan.returnClassName);
                            if (sIt != cg_.context_info_.classStructTypes.end())
                            {
                                sretStructTy = sIt->second;
                                uint64_t sz = DL.getTypeAllocSize(sretStructTy);
                                llvm::Value *szVal = llvm::ConstantInt::get(i64Ty, sz);
                                sretObj = cg_.emitMalloc(szVal, "crepr.sret");
                                b.CreateMemSet(sretObj, llvm::ConstantInt::get(i8Ty, 0), szVal,
                                               llvm::MaybeAlign(8));
                                callArgs.push_back(sretObj);
                            }
                        }

                        // (2) كلّ وسيط ص بالترتيب.
                        for (size_t i = 0; i < args.size(); ++i)
                        {
                            const CReprArgPlan *ap =
                                (i < plan.args.size()) ? &plan.args[i] : nullptr;
                            if (!ap || !ap->isStruct)
                            {
                                unsigned ci2 = static_cast<unsigned>(callArgs.size());
                                llvm::Type *want =
                                    (ci2 < ft->getNumParams()) ? ft->getParamType(ci2) : nullptr;
                                callArgs.push_back(coerceScalar(args[i], want)); // عدديّ مُكيَّف
                                continue;
                            }
                            // مؤشّر كائن ص (ذاكرة البنية بلا ترويسة).
                            llvm::Value *objPtr = args[i];
                            if (objPtr->getType()->isIntegerTy())
                                objPtr = b.CreateIntToPtr(objPtr, ptrTy, "crepr.arg.p");

                            if (ap->abi.kind == CReprAbiKind::Direct)
                            {
                                for (size_t p = 0; p < ap->abi.pieces.size(); ++p)
                                {
                                    llvm::Value *gep = b.CreateGEP(
                                        i8Ty, objPtr,
                                        llvm::ConstantInt::get(i64Ty, static_cast<uint64_t>(p) * 8),
                                        "crepr.arg.piece.p");
                                    llvm::Value *val =
                                        b.CreateLoad(ap->abi.pieces[p], gep, "crepr.arg.piece");
                                    callArgs.push_back(val);
                                }
                            }
                            else // Memory ⇒ مؤشّر بسمة byval (LLVM ينسخ)
                            {
                                auto sIt = cg_.context_info_.classStructTypes.find(ap->className);
                                if (sIt != cg_.context_info_.classStructTypes.end())
                                    callByval.emplace_back(
                                        static_cast<unsigned>(callArgs.size()), sIt->second);
                                callArgs.push_back(objPtr);
                            }
                        }

                        // (3) أصدر النداء وطبّق سمتَي sret/byval عليه (يلزمهما LLVM على موقع النداء).
                        llvm::CallInst *ci =
                            b.CreateCall(callee->getFunctionType(), callee, callArgs);
                        if (plan.sretReturn && sretStructTy)
                            ci->addParamAttr(
                                0, llvm::Attribute::getWithStructRetType(ctx, sretStructTy));
                        for (const auto &bv : callByval)
                            ci->addParamAttr(
                                bv.first, llvm::Attribute::getWithByValType(ctx, bv.second));

                        // (4) جسّد العائد.
                        llvm::Value *result = nullptr;
                        if (plan.sretReturn)
                        {
                            result = sretObj; // الكائن الّذي كتب فيه المُستدعى
                        }
                        else if (plan.directStructReturn)
                        {
                            auto sIt = cg_.context_info_.classStructTypes.find(plan.returnClassName);
                            if (sIt != cg_.context_info_.classStructTypes.end())
                            {
                                llvm::Type *rst = sIt->second;
                                uint64_t sz = DL.getTypeAllocSize(rst);
                                llvm::Value *szVal = llvm::ConstantInt::get(i64Ty, sz);
                                llvm::Value *obj = cg_.emitMalloc(szVal, "crepr.ret");
                                b.CreateMemSet(obj, llvm::ConstantInt::get(i8Ty, 0), szVal,
                                               llvm::MaybeAlign(8));
                                if (plan.returnAbi.pieces.size() == 1)
                                {
                                    b.CreateStore(ci, obj);
                                }
                                else
                                {
                                    for (size_t p = 0; p < plan.returnAbi.pieces.size(); ++p)
                                    {
                                        llvm::Value *elem =
                                            b.CreateExtractValue(ci, static_cast<unsigned>(p),
                                                                 "crepr.ret.elem");
                                        llvm::Value *gep = b.CreateGEP(
                                            i8Ty, obj,
                                            llvm::ConstantInt::get(i64Ty, static_cast<uint64_t>(p) * 8),
                                            "crepr.ret.piece.p");
                                        b.CreateStore(elem, gep);
                                    }
                                }
                                result = obj;
                            }
                        }
                        else
                        {
                            result = ci->getType()->isVoidTy() ? nullptr : ci;
                        }

                        if (inst->result.has_value() && result)
                            cg_.context_info_.namedValues[inst->result->name] = result;
                        return result ? result : static_cast<llvm::Value *>(ci);
                    }
                }
            }

            // ================================================================
            // (AR) تفويض لمعالجات الاستثناءات — __sad_alloc_jmpbuf, __sad_raise, ...
            //      مُستخرجة في llvm_codegen_exceptions.cpp (CW-05)
            // (EN) Delegate to exception handlers — extracted to llvm_codegen_exceptions.cpp
            // ================================================================
            if (auto r = cg_.emitCallException(funcName, args, inst))
                return *r;

            // ================================================================
            // (AR) تفويض لمعالجات الخرائط — __sad_map_create, __sad_map_get, ...
            //      مُستخرجة في llvm_codegen_maps.cpp (CW-05)
            // (EN) Delegate to map handlers — extracted to llvm_codegen_maps.cpp
            // ================================================================
            if (auto r = cg_.emitCallMap(funcName, args, inst))
                return *r;

            // ================================================================
            // (AR) Branch Hints: متوقع(شرط) و غير_متوقع(شرط)
            //      تُترجم إلى llvm.expect.i1 لتوجيه CPU branch prediction
            //      المرجع: LLVM Language Reference - llvm.expect intrinsic
            // (EN) Branch hints: متوقع/غير_متوقع → llvm.expect.i1 intrinsic
            // ================================================================
            if (funcName == "متوقع" || funcName == "غير_متوقع")
            {
                if (args.empty())
                {
                    cg_.reportError(funcName + " requires 1 argument (condition)");
                    return nullptr;
                }
                llvm::Value *cond = args[0];
                // (AR) تحويل القيمة إلى i1 إن لم تكن كذلك
                llvm::Type *i1Ty = llvm::Type::getInt1Ty(*cg_.context_);
                if (cond->getType() != i1Ty)
                {
                    if (cond->getType()->isIntegerTy())
                    {
                        cond = cg_.builder_->CreateICmpNE(
                            cond, llvm::ConstantInt::get(cond->getType(), 0), "branch_hint.cond");
                    }
                    else
                    {
                        // (AR) نوع غير متوقع — لا تطبق التلميح
                        if (inst->result.has_value())
                            cg_.context_info_.namedValues[inst->result->name] = args[0];
                        return args[0];
                    }
                }
                // (AR) القيمة المتوقعة: true لـ متوقع، false لـ غير_متوقع
                llvm::Value *expectedVal = (funcName == "متوقع")
                                               ? llvm::ConstantInt::getTrue(*cg_.context_)
                                               : llvm::ConstantInt::getFalse(*cg_.context_);
                // (AR) استدعاء intrinsic llvm.expect.i1
                llvm::Function *expectFn = llvm::Intrinsic::getDeclaration(
                    cg_.module_.get(), llvm::Intrinsic::expect, {i1Ty});
                llvm::Value *result = cg_.builder_->CreateCall(
                    expectFn, {cond, expectedVal}, "branch_hint");
                if (inst->result.has_value())
                    cg_.context_info_.namedValues[inst->result->name] = result;
                return result;
            }

            llvm::Function *callee = nullptr;
            auto funcIt = cg_.context_info_.functions.find(funcName);
            if (funcIt != cg_.context_info_.functions.end())
            {
                callee = funcIt->second;
            }

            // ═══════════════════════════════════════════════════════════════
            // (AR) بحث سلسلة الوراثة: إذا لم نجد الدالة وكانت استدعاء طريقة (className.method)
            //      نبحث في الأصناف الأب حتى نجد التعريف
            // (EN) Inheritance chain lookup: if method not found and name contains '.'
            //      walk parent classes via classParentMap to find the definition
            // ═══════════════════════════════════════════════════════════════
            if (!callee)
            {
                auto dotPos = funcName.find('.');
                if (dotPos != std::string::npos)
                {
                    std::string className = funcName.substr(0, dotPos);
                    std::string methodName = funcName.substr(dotPos + 1);
                    std::string searchClass = className;
                    while (!searchClass.empty())
                    {
                        auto parentIt = cg_.context_info_.classParentMap.find(searchClass);
                        if (parentIt != cg_.context_info_.classParentMap.end() && !parentIt->second.empty())
                        {
                            searchClass = parentIt->second;
                            std::string parentMethodName = searchClass + "." + methodName;
                            auto parentFuncIt = cg_.context_info_.functions.find(parentMethodName);
                            if (parentFuncIt != cg_.context_info_.functions.end())
                            {
                                callee = parentFuncIt->second;
                                // (AR) حفظ في السياق لتسريع الاستدعاءات اللاحقة
                                // (EN) Cache for future calls
                                cg_.context_info_.functions[funcName] = callee;
                                break;
                            }
                            // (AR) محاولة البحث في LLVM module مباشرة
                            // (EN) Try searching LLVM module directly
                            callee = cg_.module_->getFunction(parentMethodName);
                            if (callee)
                            {
                                cg_.context_info_.functions[funcName] = callee;
                                break;
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                }
            }

            if (!callee)
            {
                // =====================================================================
                // (AR) دعم الربط عبر الملفات: إنشاء تصريح خارجي للدالة
                // (EN) Cross-file linking support: create extern declaration for function
                // عندما تستدعي دالة من ملف آخر، ننشئ تصريحاً (declare) في الوحدة الحالية
                // When calling a function from another file, create a declaration in current module
                // الرابط (linker) سيحل الرمز لاحقاً
                // The linker will resolve the symbol later
                // =====================================================================

                // استنتاج أنواع المعاملات من القيم الفعلية
                // Infer parameter types from actual values
                std::vector<llvm::Type *> paramTypes;
                for (const auto &arg : args)
                {
                    paramTypes.push_back(arg->getType());
                }

                // استنتاج نوع الإرجاع من نتيجة التعليمة
                // Infer return type from instruction result
                llvm::Type *returnType = cg_.getVoidType();
                if (inst->result.has_value())
                {
                    switch (inst->result->dataType)
                    {
                    case SadTypeKind::Integer:
                        returnType = cg_.getInt64Type();
                        break;
                    case SadTypeKind::Float:
                        returnType = cg_.getDoubleType();
                        break;
                    case SadTypeKind::Boolean:
                        returnType = cg_.getInt1Type();
                        break;
                    case SadTypeKind::Pointer:
                    case SadTypeKind::String:
                    case SadTypeKind::Array:
                    case SadTypeKind::Struct:
                    case SadTypeKind::Class:
                        returnType = cg_.getInt8PtrType();
                        break;
                    case SadTypeKind::Void:
                        returnType = cg_.getVoidType();
                        break;
                    default:
                        returnType = cg_.getInt64Type();
                        break;
                    }
                }

                // إنشاء نوع الدالة والتصريح الخارجي
                // Create function type and extern declaration
                llvm::FunctionType *funcType = llvm::FunctionType::get(
                    returnType, paramTypes, false);

                llvm::FunctionCallee fc = cg_.module_->getOrInsertFunction(funcName, funcType);
                callee = llvm::dyn_cast<llvm::Function>(fc.getCallee());

                if (!callee)
                {
                    cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", std::string("Failed to create extern declaration for:") + funcName}});
                    return nullptr;
                }

                // حفظ في السياق لاستخدامات لاحقة
                // Cache in context for future calls
                cg_.context_info_.functions[funcName] = callee;
            }

            // إنشاء تعليمة الاستدعاء
            // Create call instruction

            // ================================================================
            // تحويل أنواع المعاملات إذا لزم الأمر (ptr↔i64)
            // Convert argument types if needed (ptr↔i64)
            // ================================================================
            llvm::FunctionType *funcType = callee->getFunctionType();

            // (AR) مطابقة عدد الوسائط مع عدد بارامترات الدالة
            //      إذا كان عدد الوسائط أكثر — اقتطع الزيادة
            //      إذا كان أقل — أضف قيم صفرية
            // (EN) Match arg count with function parameter count.
            //      If too many args — truncate. If too few — pad with zeros.
            unsigned expectedArgCount = funcType->getNumParams();
            if (args.size() > expectedArgCount)
            {
                args.resize(expectedArgCount);
            }
            else
            {
                while (args.size() < expectedArgCount)
                {
                    llvm::Type *paramType = funcType->getParamType(args.size());
                    args.push_back(llvm::Constant::getNullValue(paramType));
                }
            }

            // ================================================================
            // (AR) ISSUE-076 (حلّ %SadDyn الجذريّ): تمرير القيم لمعاملٍ ديناميّ %SadDyn يُدار
            //      بعدم تطابق النوع (كبقيّة حلقة التحويل) لا بتوسيمٍ يدويّ: وسيطٌ محسوس لمعاملٍ
            //      %SadDyn ⇒ toDyn (غلّف حسب نوع SIR للوسيط)؛ وسيطٌ %SadDyn لمعاملٍ محدَّد ⇒ فُكّ.
            //      يزول قسم التوسيم البتّيّ + تعليب malloc(8) (لا تسريب).
            // (EN) ISSUE-076 (%SadDyn root fix): passing values to a dynamic %SadDyn param is driven
            //      by type mismatch (like the rest of the conversion loop), not manual tagging: a
            //      concrete arg to a %SadDyn param ⇒ toDyn (pack per its SIR type); a %SadDyn arg to
            //      a concrete param ⇒ unpack. The bit-tagging + malloc(8) box (leak) is gone.
            // ================================================================
            // (AR) جدولُ التحويلِ **واحدٌ** الآن: `coerceToParamType` في sad_dyn_repr.cpp.
            //      كان هنا نسخةٌ ثالثةٌ تفكّ الموسومَ إلى i64 بالحمولةِ الخامّ، فقيمةٌ وسمُها
            //      عشريٌّ تعبر هذا الحدَّ بنمطِ بتّاتِها عددًا صحيحًا — جوابٌ خاطئٌ صامت.
            //      ونوعُ SIR للوسيطِ يُمرَّر ليحمل وسمَ التعليبِ الصحيحَ في الاتّجاهِ المقابل.
            for (size_t i = 0; i < args.size() && i < funcType->getNumParams(); ++i)
            {
                const SadTypeKind argSir = ((i + 1) < inst->operands.size())
                                               ? inst->operands[i + 1].dataType
                                               : SadTypeKind::Integer;
                args[i] = coerceToParamType(cg_, args[i], funcType->getParamType(i), argSir);
            }

            llvm::Value *result = nullptr;
            if (callee->getReturnType()->isVoidTy())
            {
                // (AR) إصلاح: إذا كانت الدالة void في LLVM لكن SIR تتوقع قيمة إرجاع (I64 مثلاً)
                //      هذا يحدث عند خارجي("inl") دالة اقرأ_منفذ_32(...) — الدالة الخارجية بدون جسم
                //      يُعيّن نوع إرجاعها VOID لكنها فعلياً تُرجع I64
                //      الحل: استدعاء inline assembly مباشرة بنوع الإرجاع الصحيح
                // (EN) Fix: If LLVM function is void but SIR expects a return value,
                //      create a new declaration with correct return type via getOrInsertFunction
                bool needsReturnValue = inst->result.has_value() &&
                                        inst->result->dataType != SadTypeKind::Void;
                if (needsReturnValue)
                {
                    // (AR) تحديد نوع الإرجاع المطلوب
                    llvm::Type *wantedRetType = cg_.getInt64Type();
                    switch (inst->result->dataType)
                    {
                    case SadTypeKind::Float:
                        wantedRetType = cg_.getDoubleType();
                        break;
                    case SadTypeKind::Boolean:
                        wantedRetType = cg_.getInt1Type();
                        break;
                    case SadTypeKind::Pointer:
                    case SadTypeKind::String:
                        wantedRetType = cg_.getInt8PtrType();
                        break;
                    default:
                        wantedRetType = cg_.getInt64Type();
                        break;
                    }
                    // (AR) إعادة بناء نوع الدالة مع نوع إرجاع صحيح
                    std::vector<llvm::Type *> pTypes;
                    for (unsigned i = 0; i < callee->getFunctionType()->getNumParams(); ++i)
                    {
                        pTypes.push_back(callee->getFunctionType()->getParamType(i));
                    }
                    llvm::FunctionType *newFuncType = llvm::FunctionType::get(wantedRetType, pTypes, false);

                    // (AR) إنشاء استدعاء مباشر بنوع الدالة الجديد عبر FunctionCallee
                    //      لا نحذف الدالة القديمة — نستخدم FunctionCallee مع نوع مختلف
                    // (EN) Create direct call with new function type via FunctionCallee
                    //      Don't delete old function — use FunctionCallee with different type
                    llvm::FunctionCallee fc(newFuncType, callee);
                    result = cg_.builder_->CreateCall(fc, args, "calltmp");
                }
                else
                {
                    cg_.builder_->CreateCall(callee, args);
                    result = llvm::ConstantInt::get(cg_.getInt64Type(), 0);
                }
            }
            else
            {
                result = cg_.builder_->CreateCall(callee, args, "calltmp");
            }

            if (inst->result.has_value())
            {
                llvm::Value *storeVal = result;
                // ================================================================
                // (AR) [Fix #049-B] دوال pass-through: SIR يتوقع نصاً لكن الدالة تُعيد i64
                //      يحدث هذا عندما تُمرر لامدا نصية لدالة عليا (higher-order) مثل طبّق()،
                //      فيُكشف نوع الإرجاع كـ String عبر pass-through detection في SIR،
                //      لكن الدالة نفسها مُعرَّفة بإرجاع i64 (بسبب نمط الإغلاقات: PtrToInt).
                //      الحل: تحويل i64 → ptr بعد الاستدعاء مباشرةً.
                //
                // (EN) [Fix #049-B] pass-through functions: SIR expects String but callee returns i64
                //      Happens when a string lambda is passed to a higher-order function (e.g. طبّق()),
                //      which SIR pass-through detection marks as String, but the actual LLVM function
                //      is compiled as i64 (closure protocol: PtrToInt of string pointer).
                //      Fix: convert i64 → ptr immediately after the call.
                // ================================================================
                if (result &&
                    result->getType()->isIntegerTy(64) &&
                    (inst->result->dataType == SadTypeKind::String ||
                     inst->result->dataType == SadTypeKind::Pointer))
                {
                    storeVal = cg_.builder_->CreateIntToPtr(
                        result, llvm::PointerType::getUnqual(*cg_.context_), "ret.i64toptr");
                }
                cg_.context_info_.namedValues[inst->result->name] = storeVal;
            }

            return result;
        }

    } // namespace LLVM
} // namespace Sad
