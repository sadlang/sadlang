/*
 * ============================================================================
 * LLVM IR Code Generator - Builtin Functions
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "builders/builtins/builtin_funcs_codegen.h"
#include "sad_dyn_repr.h" // (AR) spike ISSUE-076: getSadDynType/isSadDyn/toDyn لـ%SadDyn PHI
#include "sir_constants.h" // (AR) kSadPanicCheckViolation (رمز سبب الهلع)
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

using namespace Sad::Compiler::SIR;

namespace Sad
{
    namespace LLVM
    {

        // (AR) مسار كامن/احتياطيّ: الواجهة الأماميّة تُخفِض «تأكد» إلى
        //      BUILTIN_SECURITY_ASSERT (⇒ emitAbortOrFreestandingPanic، كتلة sec.fail)
        //      لا BUILTIN_ASSERT، فلا مُنتِج يصل هذه الدالّة اليوم. الفصل حرّ/مستضاف
        //      أدناه محفوظ دفاعيًّا: لو أُعيد توجيه مُنتِج إليها لاحقًا، لا يتسرّب سطر
        //      «Assertion failed» الإنجليزيّ في الوضع الحرّ (اللافتة السياديّة وحدها).
        // (EN) Latent/fallback path: the frontend lowers «تأكد» to
        //      BUILTIN_SECURITY_ASSERT (⇒ emitAbortOrFreestandingPanic, sec.fail block),
        //      not BUILTIN_ASSERT, so no producer reaches this today. The hosted/
        //      freestanding split below is kept defensively: if a producer is ever
        //      routed here, no English «Assertion failed» leaks in freestanding.
        llvm::Value *BuiltinFuncsCodeGen::emitBuiltinAssert(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "ASSERT"}});
                // (AR) خطأ حقيقيّ أُبلغ — إشارة مميّزة توقف الموزّع بلا «Unsupported opcode» زائف.
                // (EN) Real error reported — distinct sentinel stops the dispatcher fall-through.
                return cg_.builtinErrorSentinel(inst);
            }

            llvm::Value *cond = cg_.resolveOperand(inst->operands[0]);
            if (!cond)
                return nullptr;

            // Convert to i1 if not already
            if (!cond->getType()->isIntegerTy(1))
            {
                cond = cg_.builder_->CreateICmpNE(cond,
                                              llvm::ConstantInt::get(cond->getType(), 0), "assert.cond");
            }

            // Create basic blocks
            llvm::Function *fn = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *failBB = llvm::BasicBlock::Create(*cg_.context_, "assert.fail", fn);
            llvm::BasicBlock *contBB = llvm::BasicBlock::Create(*cg_.context_, "assert.cont", fn);

            cg_.builder_->CreateCondBr(cond, contBB, failBB);

            // Fail block: freestanding → sovereign panic banner only; hosted → English
            // developer diagnostic + abort.
            cg_.builder_->SetInsertPoint(failBB);

            if (cg_.freestanding_)
            {
                // (AR) وضع حرّ: لا سطر إنجليزيّ سابق — اللافتة السياديّة هي التشخيص
                //      الوحيد (النمط نفسه المتّبع في na.fail/emitBoundsCheck). abort
                //      رمز libc غائب على المعدن ⇒ __sad_panic(kSadPanicCheckViolation)
                //      (weak_odr، NoReturn، للنواة تجاوزه).
                // (EN) Freestanding: no preceding English line — the sovereign banner
                //      is the sole diagnostic (same pattern as na.fail/emitBoundsCheck).
                //      abort is absent on bare metal ⇒ __sad_panic(kSadPanicCheckViolation).
                cg_.emitFreestandingPanicCall(Sad::Compiler::kSadPanicCheckViolation);
            }
            else
            {
                // (AR) مستضاف: تشخيص إنجليزيّ للمطوّر (منفذ libc) ثم abort().
                // (EN) Hosted: English developer diagnostic (libc) then abort().
                auto *printfType = llvm::FunctionType::get(
                    llvm::Type::getInt32Ty(*cg_.context_),
                    {llvm::PointerType::getUnqual(*cg_.context_)}, true);
                auto printfFunc = cg_.module_->getOrInsertFunction("printf", printfType);

                std::string msg = "Assertion failed";
                if (inst->operands.size() >= 2)
                {
                    msg = "Assertion failed: " + inst->operands[1].name;
                }
                llvm::Value *msgStr = cg_.builder_->CreateGlobalStringPtr(msg + "\n", "assert.msg");
                cg_.builder_->CreateCall(printfFunc, {msgStr});

                auto *abortType = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {}, false);
                auto abortFunc = cg_.module_->getOrInsertFunction("abort", abortType);
                cg_.builder_->CreateCall(abortFunc, {});
            }
            cg_.builder_->CreateUnreachable();

            // Continue block
            cg_.builder_->SetInsertPoint(contBB);

            return llvm::ConstantInt::get(cg_.getInt64Type(), 0);
        }

        // (AR) تنقيح: يستدعي printf فقط — آمن حرًّا بلا تبويب SEM019 لأنّ نسخة printf
        //      تسلسليّة تُحقَن داخل الوحدة في --freestanding (emitFreestandingPrintf
        //      تدعم المحدِّدات المستعملة هنا: ‎%lld/%s/%g‎). ملاحظة وصوليّة: الواجهة
        //      الأماميّة لا تُنتج BUILTIN_DEBUG اليوم (لا اسم SoT له).
        // (EN) debug-print: printf-only — freestanding-safe without an SEM019 gate
        //      because an in-module serial printf is injected under --freestanding
        //      (emitFreestandingPrintf supports the specifiers used here:
        //      %lld/%s/%g). Reachability note: the frontend does not currently
        //      produce BUILTIN_DEBUG (it has no SoT name).
        llvm::Value *BuiltinFuncsCodeGen::emitBuiltinDebug(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                return llvm::ConstantInt::get(cg_.getInt64Type(), 0);
            }

            llvm::Value *val = cg_.resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            auto *printfType = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(*cg_.context_),
                {llvm::PointerType::getUnqual(*cg_.context_)}, true);
            auto printfFunc = cg_.module_->getOrInsertFunction("printf", printfType);

            if (val->getType()->isDoubleTy())
            {
                llvm::Value *fmt = cg_.builder_->CreateGlobalStringPtr("[DEBUG] %g\n", "debug_fmt_f64");
                cg_.builder_->CreateCall(printfFunc, {fmt, val});
            }
            else if (val->getType()->isPointerTy())
            {
                llvm::Value *fmt = cg_.builder_->CreateGlobalStringPtr("[DEBUG] %s\n", "debug_fmt_str");
                cg_.builder_->CreateCall(printfFunc, {fmt, val});
            }
            else
            {
                llvm::Value *fmt = cg_.builder_->CreateGlobalStringPtr("[DEBUG] %lld\n", "debug_fmt_i64");
                cg_.builder_->CreateCall(printfFunc, {fmt, val});
            }

            return val;
        }

        // ============================================================================
        // (AR) هل يُخفَّض هذا النوع إلى مؤشّر (i8*) في الخلفية؟
        //      يوازي mapSIRType: كلّ الأنواع المركّبة/الكائنيّة/الوظيفيّة/المرجعيّة
        //      تُمثَّل مؤشّرًا لبنية وقت التشغيل. لا يشمل Any (يُخفَّض إلى %SadDyn بنيةً).
        // (EN) Does this kind lower to a pointer (i8*) in the backend?
        //      Mirrors mapSIRType: every composite/OOP/callable/reference kind is a
        //      pointer to a runtime struct. Excludes Any (lowers to the %SadDyn struct).
        // ============================================================================
        static bool isPointerLoweredKind(SadTypeKind kind)
        {
            switch (kind)
            {
            case SadTypeKind::String:
            case SadTypeKind::Pointer:
            case SadTypeKind::Array:
            case SadTypeKind::Map:
            case SadTypeKind::Tuple:
            case SadTypeKind::Slice:
            case SadTypeKind::Class:
            case SadTypeKind::Struct:
            case SadTypeKind::Enum:
            case SadTypeKind::Trait:
            case SadTypeKind::Function:
            case SadTypeKind::Closure:
            case SadTypeKind::Reference:
            case SadTypeKind::MutableRef:
            case SadTypeKind::Error:
            case SadTypeKind::Widget:
            case SadTypeKind::Window:
            case SadTypeKind::Event:
            case SadTypeKind::Future:
            case SadTypeKind::Generator:
                return true;
            default:
                return false;
            }
        }

        // ============================================================================
        // emitPhi - عقدة فاي / Phi node (SSA form)
        // ============================================================================
        llvm::Value *BuiltinFuncsCodeGen::emitPhi(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2 || inst->operands.size() % 2 != 0)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "PHI"}});
                // (AR) خطأ حقيقيّ أُبلغ — إشارة مميّزة بدل nullptr المُلبس (#185/#188).
                // (EN) Real error reported — distinct sentinel instead of ambiguous nullptr.
                return cg_.builtinErrorSentinel(inst);
            }

            // Determine phi type from result
            llvm::Type *phiType = cg_.getInt64Type(); // default
            if (inst->result.has_value())
            {
                // (AR) spike ISSUE-076: نتيجة Any ⇒ PHI بنوع %SadDyn (قيمة موسومة ذاتيّة الوصف).
                //      تُمكّن «؟. النصّية» من دمج فرع النصّ (Str) مع فرع العدم (Null) بلا تثبيتٍ
                //      على i64. لا مستهلك حاليّ يُصدر PHI بنوع Any ⇒ خامدة لسائر المسارات (؟؟/القنوات).
                // (EN) spike ISSUE-076: an Any result ⇒ a %SadDyn PHI (self-describing tagged value).
                //      Lets «؟. string» merge the string (Str) branch with the null (Null) branch
                //      without pinning to i64. No current consumer emits an Any-typed PHI ⇒ inert for
                //      all other paths (?? / channels).
                if (inst->result->dataType == SadTypeKind::Any)
                    phiType = getSadDynType(*cg_.context_);
                else if (inst->result->dataType == SadTypeKind::Float)
                    phiType = llvm::Type::getDoubleTy(*cg_.context_);
                else if (inst->result->dataType == SadTypeKind::Boolean)
                    phiType = llvm::Type::getInt1Ty(*cg_.context_);
                // (AR) الأنواع المُخفَّضة إلى مؤشّر (i8*): نصّ/مؤشّر إضافةً إلى كلّ
                //      الأنواع المركّبة والكائنيّة والوظيفيّة (مصفوفة/خريطة/صف/شريحة/
                //      صنف/بنية/تعداد/دالة/إغلاق/مرجع/خطأ/…). كانت تسقط سابقًا إلى i64
                //      الافتراضيّ، فينشأ PHI بنوع i64 بينما وارده الحيّ مؤشّرٌ حقيقيّ ⇒
                //      «PHI node operands are not the same type» في ‏?? مثل «لاشيء ؟؟ [1،2]».
                // (EN) Pointer-lowered kinds (i8*): string/pointer plus every composite,
                //      OOP and callable kind (array/map/tuple/slice/class/struct/enum/
                //      function/closure/reference/error/…). These previously fell to the
                //      i64 default, yielding an i64 PHI whose live incoming is a real
                //      pointer ⇒ «PHI node operands are not the same type» for `?`
                //      forms such as «لاشيء ؟؟ [1،2]».
                else if (isPointerLoweredKind(inst->result->dataType))
                    phiType = llvm::PointerType::getUnqual(*cg_.context_);
            }

            unsigned numIncoming = inst->operands.size() / 2;
            llvm::PHINode *phi = cg_.builder_->CreatePHI(phiType, numIncoming, "phi");

            llvm::Function *func = cg_.builder_->GetInsertBlock()->getParent();
            for (unsigned i = 0; i < inst->operands.size(); i += 2)
            {
                const std::string &bbName = inst->operands[i + 1].name;

                // (AR) السلف الفعليّ قد يكون كتلةً منقسمة عن bbName (فحص حدود المصفوفة
                //      يحوّل نهاية الكتلة إلى set.bc.ok). نُفضّل «كتلة الخروج الفعليّة»
                //      المسجَّلة أثناء الخفض؛ وإلّا نبحث بالاسم كسابق عهده.
                // (EN) The real predecessor may be a block split off from bbName (array
                //      bounds check moves the block's tail into set.bc.ok). Prefer the
                //      recorded «effective exit block»; else fall back to name lookup.
                llvm::BasicBlock *bb = nullptr;
                auto exitIt = cg_.context_info_.basicBlockExits.find(bbName);
                if (exitIt != cg_.context_info_.basicBlockExits.end())
                {
                    bb = exitIt->second;
                }
                else
                {
                    // Find the basic block by name
                    for (auto &block : *func)
                    {
                        if (block.getName() == bbName)
                        {
                            bb = &block;
                            break;
                        }
                    }
                }
                if (bb)
                {
                    // (AR) مهم: حل قيمة الـ incoming داخل predecessor نفسه
                    //      لتفادي إنشاء load في merge block مما يكسر قواعد dominance في LLVM.
                    // (EN) Important: resolve incoming value inside its predecessor block
                    //      to avoid generating loads in merge block, which breaks dominance.
                    llvm::Value *val = nullptr;
                    {
                        llvm::IRBuilderBase::InsertPointGuard guard(*cg_.builder_);
                        if (bb->getTerminator())
                        {
                            cg_.builder_->SetInsertPoint(bb->getTerminator());
                        }
                        else
                        {
                            cg_.builder_->SetInsertPoint(bb);
                        }
                        val = cg_.resolveOperand(inst->operands[i]);

                        // (AR) توحيد نوع الوارد مع نوع PHI داخل السَّلَف نفسه. الفرع
                        //      الميت في ‏?? يحمل حارس العدم (i64) بينما نوع PHI مؤشّر،
                        //      والعكس قد يَرِد؛ نُدرِج inttoptr/ptrtoint هنا (لا في كتلة
                        //      الدمج) حفاظًا على قاعدة الهيمنة. تحويل مؤشّر↔مؤشّر لاغٍ مع
                        //      المؤشّرات الشفّافة لكنّه يصون الصلاحية إن اختلفت الأنواع.
                        // (EN) Reconcile the incoming's type with the PHI type inside the
                        //      predecessor itself. A dead `?` branch carries the i64 null
                        //      sentinel while the PHI type is a pointer (and vice versa);
                        //      emit inttoptr/ptrtoint here (not in the merge block) to keep
                        //      dominance intact. ptr↔ptr is a no-op with opaque pointers but
                        //      preserves validity should the types differ.
                        // (AR) spike ISSUE-076: PHI بنوع %SadDyn ⇒ علّب كلّ وارد بوسمه عبر toDyn
                        //      (نصّ⇒Str، عدم⇒Null) داخل السَّلَف نفسه — فيصير نوعه مطابقًا لنوع الـPHI.
                        // (EN) spike ISSUE-076: a %SadDyn PHI ⇒ box each incoming per its kind via
                        //      toDyn (String⇒Str, Null⇒Null) inside its predecessor to match the PHI.
                        if (val && !isSadDyn(val) && phiType == getSadDynType(*cg_.context_))
                            val = toDyn(cg_, val, inst->operands[i].dataType);
                        // (AR) والاتّجاهُ المعاكس: واردٌ %SadDyn وPHI محسوس. يقع حين
                        //      يُخفَّض «أي» إلى %SadDyn فيصير أحدُ فرعَي «؟؟» موسومًا
                        //      والآخرُ رقمًا — وكان يُجهض المترجمَ بتأكيدِ LLVM
                        //      «All operands to PHI node must be the same type»، أي
                        //      إخفاقًا داخليًّا لا تشخيصًا. يُفكّ التعليبُ إلى نوعِ الـPHI
                        //      داخلَ السَّلَفِ نفسِه (حفاظًا على الهيمنة).
                        // (EN) The mirror direction: a %SadDyn incoming into a concrete
                        //      PHI. It appears once `أي` lowers to %SadDyn, making one
                        //      `??` arm tagged and the other an int — which used to abort
                        //      the compiler on an LLVM assertion rather than diagnose.
                        //      Unbox to the PHI type inside the predecessor (dominance).
                        else if (val && isSadDyn(val) && phiType != getSadDynType(*cg_.context_))
                        {
                            if (phiType->isDoubleTy())
                                val = unpackDouble(cg_, val);
                            else if (phiType->isPointerTy())
                                val = unpackPtr(cg_, val);
                            else
                            {
                                val = unpackI64(cg_, val);
                                if (phiType->isIntegerTy() && !phiType->isIntegerTy(64))
                                    val = cg_.builder_->CreateTrunc(val, phiType, "phi.dyn.trunc");
                            }
                        }

                        if (val && val->getType() != phiType)
                        {
                            if (phiType->isPointerTy() && val->getType()->isIntegerTy())
                                val = cg_.builder_->CreateIntToPtr(val, phiType, "phi.i2p");
                            else if (phiType->isIntegerTy() && val->getType()->isPointerTy())
                                val = cg_.builder_->CreatePtrToInt(val, phiType, "phi.p2i");
                            else if (phiType->isPointerTy() && val->getType()->isPointerTy())
                                val = cg_.builder_->CreateBitCast(val, phiType, "phi.p2p");
                        }
                    }

                    if (!val)
                    {
                        // (AR) قيمة افتراضية عند فشل cg_.resolveOperand — يمنع "PHINode must have entry for each predecessor"
                        // (EN) Fallback when cg_.resolveOperand fails — prevents PHI predecessor mismatch error
                        val = llvm::Constant::getNullValue(phiType);
                    }
                    phi->addIncoming(val, bb);
                }
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = phi;
            }
            return phi;
        }

        // ============================================================================
        // emitBuiltinTypeOf - نوع_المتغير / typeof
        // ============================================================================

        // ============================================================================
        // New stdlib builtins - دوال المكتبة القياسية الجديدة
        // ============================================================================

        llvm::Value *BuiltinFuncsCodeGen::emitBuiltinIsType(std::shared_ptr<SIRInstruction> inst, const std::string &typeName)
        {
            // (AR) في الترجمة الساكنة، نوع المتغير معروف وقت الترجمة
            // (EN) In static compilation, variable type is known at compile time
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "is_type"}});
                // (AR) خطأ حقيقيّ أُبلغ — إشارة مميّزة بدل nullptr المُلبس (#185/#188).
                // (EN) Real error reported — distinct sentinel instead of ambiguous nullptr.
                return cg_.builtinErrorSentinel(inst);
            }

            llvm::Value *val = cg_.resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            llvm::Value *result;
            if (typeName == "integer")
            {
                // Check if the operand type is integer
                result = llvm::ConstantInt::get(cg_.getInt64Type(), val->getType()->isIntegerTy() ? 1 : 0);
            }
            else if (typeName == "float")
            {
                result = llvm::ConstantInt::get(cg_.getInt64Type(), val->getType()->isDoubleTy() ? 1 : 0);
            }
            else if (typeName == "string")
            {
                result = llvm::ConstantInt::get(cg_.getInt64Type(), val->getType()->isPointerTy() ? 1 : 0);
            }
            else if (typeName == "array")
            {
                result = llvm::ConstantInt::get(cg_.getInt64Type(), val->getType()->isPointerTy() ? 1 : 0);
            }
            else
            {
                result = llvm::ConstantInt::get(cg_.getInt64Type(), 0);
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *BuiltinFuncsCodeGen::emitBuiltinToBool(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "to_bool"}});
                // (AR) خطأ حقيقيّ أُبلغ — إشارة مميّزة بدل nullptr المُلبس (#185/#188).
                // (EN) Real error reported — distinct sentinel instead of ambiguous nullptr.
                return cg_.builtinErrorSentinel(inst);
            }

            llvm::Value *val = cg_.resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            llvm::Value *result;
            if (val->getType()->isIntegerTy())
            {
                // Non-zero = true
                llvm::Value *cmp = cg_.builder_->CreateICmpNE(val,
                                                          llvm::ConstantInt::get(val->getType(), 0), "tobool.cmp");
                result = cg_.builder_->CreateZExt(cmp, cg_.getInt64Type(), "tobool");
            }
            else if (val->getType()->isDoubleTy())
            {
                // Non-zero float = true
                llvm::Value *cmp = cg_.builder_->CreateFCmpONE(val,
                                                           llvm::ConstantFP::get(cg_.getDoubleType(), 0.0), "tobool.cmp");
                result = cg_.builder_->CreateZExt(cmp, cg_.getInt64Type(), "tobool");
            }
            else if (val->getType()->isPointerTy())
            {
                // Non-null pointer = true
                llvm::Value *cmp = cg_.builder_->CreateICmpNE(
                    cg_.builder_->CreatePtrToInt(val, cg_.getInt64Type()),
                    llvm::ConstantInt::get(cg_.getInt64Type(), 0), "tobool.cmp");
                result = cg_.builder_->CreateZExt(cmp, cg_.getInt64Type(), "tobool");
            }
            else
            {
                result = llvm::ConstantInt::get(cg_.getInt64Type(), 0);
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *BuiltinFuncsCodeGen::emitBuiltinReadLine(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) قراءة سطر كامل من stdin باستخدام fgets
            // (EN) Read a full line from stdin using fgets

            // char buf[1024]; scanf("%1023[^\n]", buf); getchar(); return strdup(buf);
            auto *charTy = llvm::Type::getInt8Ty(*cg_.context_);
            auto *bufTy = llvm::ArrayType::get(charTy, 1024);
            llvm::Value *buf = cg_.builder_->CreateAlloca(bufTy, nullptr, "readline.buf");
            llvm::Value *bufPtr = cg_.builder_->CreateBitCast(buf, llvm::PointerType::getUnqual(*cg_.context_), "readline.ptr");

            // Use scanf("%1023[^\n]", buf) - read until newline
            auto *scanfTy = llvm::FunctionType::get(cg_.getInt32Type(), {llvm::PointerType::getUnqual(*cg_.context_)}, true);
            auto scanfFn = cg_.module_->getOrInsertFunction("scanf", scanfTy);

            llvm::Value *fmtStr = cg_.builder_->CreateGlobalStringPtr("%1023[^\n]", "readline.fmt");
            cg_.builder_->CreateCall(scanfFn, {fmtStr, bufPtr});

            // Consume the newline
            auto getcTy = llvm::FunctionType::get(cg_.getInt32Type(), {}, false);
            auto getcFn = cg_.module_->getOrInsertFunction("getchar", getcTy);
            cg_.builder_->CreateCall(getcFn, {});

            // Duplicate the string with strdup
            auto *strdupTy = llvm::FunctionType::get(
                llvm::PointerType::getUnqual(*cg_.context_),
                {llvm::PointerType::getUnqual(*cg_.context_)}, false);
            auto strdupFn = cg_.module_->getOrInsertFunction("strdup", strdupTy);
            llvm::Value *result = cg_.builder_->CreateCall(strdupFn, {bufPtr}, "readline.dup");

            if (inst && inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *BuiltinFuncsCodeGen::emitBuiltinClearScreen(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) مسح الشاشة باستخدام system("cls") على ويندوز
            // (EN) Clear screen using system("cls") on Windows
            auto *systemTy = llvm::FunctionType::get(cg_.getInt32Type(),
                                                     {llvm::PointerType::getUnqual(*cg_.context_)}, false);
            auto systemFn = cg_.module_->getOrInsertFunction("system", systemTy);

#ifdef _WIN32
            llvm::Value *cmd = cg_.builder_->CreateGlobalStringPtr("cls", "cls.str");
#else
            llvm::Value *cmd = cg_.builder_->CreateGlobalStringPtr("clear", "clear.str");
#endif

            cg_.builder_->CreateCall(systemFn, {cmd});
            // (AR) قيمة إشاريّة «عُولجت» — إرجاع nullptr كان يُسقط الموزّع عبر بقيّة
            //      الطبقات فيطبع «Unsupported opcode» بائتًا رغم إصدار النداء (تعليمة
            //      SIR هنا بلا سجلّ نتيجة فلا مستهلك للقيمة). نفس نمط «اطبع»
            //      (io_builtins_ops.cpp).
            // (EN) "Handled" sentinel — returning nullptr made the dispatcher fall
            //      through the remaining tiers and print a spurious "Unsupported
            //      opcode" despite the call being emitted (the SIR instruction has no
            //      result register, so nothing consumes this). Same pattern as print.
            return llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0);
        }

        llvm::Value *BuiltinFuncsCodeGen::emitBuiltinSum(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) حساب مجموع عناصر مصفوفة
            // (EN) Calculate sum of array elements
            // For static compilation, we implement a loop over the array
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "SUM"}});
                // (AR) خطأ حقيقيّ أُبلغ — إشارة مميّزة بدل nullptr المُلبس (النمط المثبَت هنا).
                // (EN) Real error reported — distinct sentinel instead of the ambiguous
                //      nullptr (the very pattern documented for this handler).
                return cg_.builtinErrorSentinel(inst);
            }

            llvm::Value *arr = cg_.resolveOperand(inst->operands[0]);
            if (!arr)
                return nullptr;

            // Get array size (stored at offset 0 of array struct)
            llvm::Value *sizePtr = cg_.builder_->CreateStructGEP(
                llvm::StructType::get(*cg_.context_, {cg_.getInt64Type(), llvm::PointerType::getUnqual(*cg_.context_)}),
                arr, 0, "sum.sizeptr");
            llvm::Value *size = cg_.builder_->CreateLoad(cg_.getInt64Type(), sizePtr, "sum.size");

            // Get data pointer (offset 1)
            llvm::Value *dataPtr = cg_.builder_->CreateStructGEP(
                llvm::StructType::get(*cg_.context_, {cg_.getInt64Type(), llvm::PointerType::getUnqual(*cg_.context_)}),
                arr, 1, "sum.dataptr");
            llvm::Value *data = cg_.builder_->CreateLoad(llvm::PointerType::getUnqual(*cg_.context_), dataPtr, "sum.data");

            // Loop to sum all elements
            llvm::Function *fn = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *preheader = cg_.builder_->GetInsertBlock();
            llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*cg_.context_, "sum.loop", fn);
            llvm::BasicBlock *exitBB = llvm::BasicBlock::Create(*cg_.context_, "sum.exit", fn);

            cg_.builder_->CreateBr(loopBB);
            cg_.builder_->SetInsertPoint(loopBB);

            llvm::PHINode *idx = cg_.builder_->CreatePHI(cg_.getInt64Type(), 2, "sum.idx");
            llvm::PHINode *acc = cg_.builder_->CreatePHI(cg_.getInt64Type(), 2, "sum.acc");
            idx->addIncoming(llvm::ConstantInt::get(cg_.getInt64Type(), 0), preheader);
            acc->addIncoming(llvm::ConstantInt::get(cg_.getInt64Type(), 0), preheader);

            // Load element at idx
            llvm::Value *elemPtr = cg_.builder_->CreateGEP(cg_.getInt64Type(), data, {idx}, "sum.elemptr");
            llvm::Value *elem = cg_.builder_->CreateLoad(cg_.getInt64Type(), elemPtr, "sum.elem");
            llvm::Value *newAcc = cg_.builder_->CreateAdd(acc, elem, "sum.newacc");

            // Increment idx
            llvm::Value *nextIdx = cg_.builder_->CreateAdd(idx, llvm::ConstantInt::get(cg_.getInt64Type(), 1), "sum.nextidx");
            llvm::Value *done = cg_.builder_->CreateICmpSGE(nextIdx, size, "sum.done");
            cg_.builder_->CreateCondBr(done, exitBB, loopBB);

            idx->addIncoming(nextIdx, loopBB);
            acc->addIncoming(newAcc, loopBB);

            cg_.builder_->SetInsertPoint(exitBB);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = newAcc;
            }
            return newAcc;
        }

    } // namespace LLVM
} // namespace Sad
