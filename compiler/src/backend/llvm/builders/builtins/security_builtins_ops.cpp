/*
 * ============================================================================
 * SecurityBuiltinsCodeGen implementation
 * Phase 7 Step 15
 * ============================================================================
 */
#include "llvm_codegen.h"
#include "builders/builtins/security_builtins_codegen.h"
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <string>
#include <vector>

namespace Sad { namespace LLVM {

        // ====================================================================
        // (AR) توقُّف فشل التأكيد/الهلع (تأكد/ذعر/تأكد_مساواة/تأكد_أكبر):
        //      مستضافًا abort()؛ وحرًّا abort رمز libc غائب على المعدن العاري ⇒
        //      __sad_panic (weak_odr، NoReturn؛ تُبثّ نسخته الافتراضيّة في
        //      emitFreestandingRuntime وللنواة تجاوزها بتعريف قويّ) — النمط نفسه
        //      المتّبع لاستبدال exit في arith_main.cpp/array_ops.cpp. التأكيد مفهوم
        //      أصيل في النوى فمساره الحرّ سليم ولا يُبوَّب في SEM019.
        //      يُصدر unreachable الختاميّ أيضًا.
        // (EN) Assertion-failure/panic halt: hosted → abort(); freestanding →
        //      abort is an absent libc symbol on bare metal, so call __sad_panic
        //      (weak_odr, NoReturn, kernel-overridable; default emitted by
        //      emitFreestandingRuntime) — same pattern as the exit replacement in
        //      arith_main.cpp/array_ops.cpp. Asserts are a native kernel concept,
        //      hence a sound freestanding path instead of an SEM019 gate.
        //      Also emits the trailing unreachable.
        // ====================================================================
        static void emitAbortOrFreestandingPanic(LLVMCodeGen &cg)
        {
            if (cg.freestanding_)
            {
                auto *i64Ty = llvm::Type::getInt64Ty(*cg.context_);
                auto *panicFT = llvm::FunctionType::get(
                    llvm::Type::getVoidTy(*cg.context_), {i64Ty}, false);
                auto panicFn = cg.module_->getOrInsertFunction("__sad_panic", panicFT);
                cg.builder_->CreateCall(panicFn, {llvm::ConstantInt::get(i64Ty, 1)});
            }
            else
            {
                auto *abortFT = llvm::FunctionType::get(
                    llvm::Type::getVoidTy(*cg.context_), {}, false);
                auto abortFn = cg.module_->getOrInsertFunction("abort", abortFT);
                cg.builder_->CreateCall(abortFn, {});
            }
            cg.builder_->CreateUnreachable();
        }

        // ====================================================================
        // (AR) نداء دالّة RUNTIME_CALL تشفيريّة تُرجع NULL عند الخطأ (اصطلاح
        //      سنتينل موحَّد اعتُمد عبر كل دوال تشفير RUNTIME_CALL في
        //      sad_embedded_runtime.c) + رفع استثناء قابل للالتقاط عبر
        //      __sad_raise عند NULL — نفس آليّة عقود «يتطلب» بالضبط
        //      (sir_builder_functions.cpp، [Fix BF-04]، مُعاد استعمالها هنا
        //      عبر LLVMCodeGen::emitCallException العامّة، لا تكرار منطق
        //      longjmp/globals). يوحّد سلوك الفشل بين المحرِّكين: كان المفسّر
        //      وحده يرمي استثناءً قابلًا للالتقاط بـحاول/امسك، والمترجم يفشل
        //      مُغلَقًا بـexit(1) أو يُرجع سلسلة فارغة صامتًا؛ الآن كلاهما يرمي.
        // (EN) Call a crypto RUNTIME_CALL runtime function that returns NULL on
        //      error (unified sentinel convention across all crypto RUNTIME_CALL
        //      functions in sad_embedded_runtime.c) and raise a catchable
        //      exception via __sad_raise on NULL — reusing the exact same
        //      mechanism as `يتطلب` contract preconditions, through the
        //      existing public LLVMCodeGen::emitCallException entry point
        //      (no duplicated longjmp/global-store logic). Unifies failure
        //      behavior across engines.
        // ====================================================================
        static llvm::Value *emitCryptoCallOrRaise(LLVMCodeGen &cg, llvm::FunctionCallee fn,
                                                   const std::vector<llvm::Value *> &args,
                                                   const char *callName, const std::string &errMsg)
        {
            llvm::Value *result = cg.builder_->CreateCall(fn, args, callName);

            llvm::Function *curFunc = cg.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *failBB = llvm::BasicBlock::Create(*cg.context_, "crypto.raise", curFunc);
            llvm::BasicBlock *contBB = llvm::BasicBlock::Create(*cg.context_, "crypto.cont", curFunc);
            llvm::Value *isNull = cg.builder_->CreateIsNull(result, "crypto.isnull");
            cg.builder_->CreateCondBr(isNull, failBB, contBB);

            cg.builder_->SetInsertPoint(failBB);
            llvm::Value *typeStr = cg.getConstantString("خطأ");
            llvm::Value *msgStr = cg.getConstantString(errMsg);
            std::vector<llvm::Value *> raiseArgs = {typeStr, msgStr};
            auto raiseInst = std::make_shared<SIRInstruction>();
            cg.emitCallException("__sad_raise", raiseArgs, raiseInst);
            // (AR) emitCallException("__sad_raise", ...) ينهي failBB بـlongjmp+
            //      unreachable ثمّ يترك نقطة الإدراج داخل كتلته الميتة الخاصّة —
            //      نعيد التوجيه صراحة إلى contBB لمتابعة توليد الشيفرة الطبيعيّة.
            // (EN) emitCallException terminates failBB with longjmp+unreachable
            //      then leaves the insert point inside its own dead block —
            //      explicitly redirect to contBB to continue normal codegen.
            cg.builder_->SetInsertPoint(contBB);
            return result;
        }

        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecurityAssert(std::shared_ptr<SIRInstruction> inst)
        {
            // Security assert - check condition and abort if false
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *cond = cg_.resolveOperand(inst->operands[0]);
            if (!cond)
                return nullptr;
            // (AR) [Fix BF-04] فحص نوع المؤشر قبل المقارنة
            // (EN) [Fix BF-04] Check pointer type before ICmp
            llvm::Value *condBool;
            if (cond->getType()->isIntegerTy(1))
            {
                condBool = cond;
            }
            else if (cond->getType()->isPointerTy())
            {
                condBool = cg_.builder_->CreateICmpNE(
                    cond,
                    llvm::ConstantPointerNull::get(
                        llvm::cast<llvm::PointerType>(cond->getType())),
                    "sec.cmp.ptr");
            }
            else
            {
                condBool = cg_.builder_->CreateICmpNE(cond, llvm::ConstantInt::get(cond->getType(), 0), "sec.cmp");
            }
            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *failBB = llvm::BasicBlock::Create(*cg_.context_, "sec.fail", curFunc);
            llvm::BasicBlock *passBB = llvm::BasicBlock::Create(*cg_.context_, "sec.pass", curFunc);
            cg_.builder_->CreateCondBr(condBool, passBB, failBB);
            cg_.builder_->SetInsertPoint(failBB);
            emitAbortOrFreestandingPanic(cg_);
            cg_.builder_->SetInsertPoint(passBB);
            // (AR) قيمة إشاريّة «عُولجت» — إرجاع nullptr كان يُسقط الموزّع عبر بقيّة
            //      الطبقات فيطبع «Unsupported opcode:200» بائتًا رغم إصدار الكود
            //      (مستضافًا يُبتلَع؛ وحرًّا تُفشِله بوّابة hasErrors خطأً). التعليمة بلا
            //      سجلّ نتيجة فلا مستهلك للقيمة. نفس نمط «اطبع»/«مسح_الشاشة» (#185).
            // (EN) "Handled" sentinel — returning nullptr made the dispatcher fall
            //      through the tiers and print a spurious "Unsupported opcode:200"
            //      (swallowed hosted; wrongly fatal under the freestanding hasErrors
            //      gate). No result register consumes this. Same pattern as #185.
            return llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0);
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecurityVerify(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *val = cg_.resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;
            // (AR) [Fix BF-04] فحص نوع المؤشر قبل المقارنة
            // (EN) [Fix BF-04] Check pointer type before ICmp
            llvm::Value *result;
            if (val->getType()->isPointerTy())
            {
                result = cg_.builder_->CreateICmpNE(
                    val,
                    llvm::ConstantPointerNull::get(
                        llvm::cast<llvm::PointerType>(val->getType())),
                    "sec.verify.ptr");
            }
            else
            {
                result = cg_.builder_->CreateICmpNE(val, llvm::ConstantInt::get(val->getType(), 0));
            }
            llvm::Value *ext = cg_.builder_->CreateZExt(result, llvm::Type::getInt64Ty(*cg_.context_));
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = ext;
            return ext;
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecurityIsSafe(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *ptr = cg_.resolveOperand(inst->operands[0]);
            if (!ptr)
                return nullptr;
            llvm::Value *result = cg_.builder_->CreateICmpNE(ptr, llvm::Constant::getNullValue(ptr->getType()));
            llvm::Value *ext = cg_.builder_->CreateZExt(result, llvm::Type::getInt64Ty(*cg_.context_));
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = ext;
            return ext;
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecurityPanic(std::shared_ptr<SIRInstruction> inst)
        {
            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            emitAbortOrFreestandingPanic(cg_);
            // (AR) كتلة استمرار ميتة بعد unreachable: أيّ تعليمات لاحقة في كتلة SIR
            //      نفسها (كالإرجاع الضمنيّ) كانت ستُلحق بعد unreachable في الكتلة
            //      ذاتها فيفشل تحقّق الوحدة (INT_MODULE_VERIFY). نمط «تأكد» نفسه.
            // (EN) Dead continuation block after unreachable: later instructions of
            //      the same SIR block (e.g., the implicit return) would otherwise be
            //      appended after the terminator, failing module verification.
            llvm::BasicBlock *contBB = llvm::BasicBlock::Create(*cg_.context_, "panic.cont", curFunc);
            cg_.builder_->SetInsertPoint(contBB);
            // (AR) قيمة إشاريّة «عُولجت» — نفس نمط «اطبع»/«مسح_الشاشة» (#185): إرجاع
            //      nullptr يُسقط الموزّع عبر الطبقات ويطبع «Unsupported opcode» بائتًا
            //      (قاتلًا حرًّا عبر بوّابة hasErrors).
            // (EN) "Handled" sentinel — same pattern as print/clear-screen (#185).
            return llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0);
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecurityHash(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *val = cg_.resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;
            // Call runtime sad_security_hash(const char*) -> char* (SHA-256 hex string,
            // matching the interpreter and the documented SoT contract — language-truth/stdlib/functions.yaml)
            llvm::Type *i8Ptr = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8Ptr, {i8Ptr}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sad_security_hash", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {val}, "hash.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecurityEncrypt(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *text = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *key = cg_.resolveOperand(inst->operands[1]);
            if (!text || !key)
                return nullptr;
            // Call runtime sad_security_encrypt(const char*, const char*) -> char*
            llvm::Type *i8Ptr = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8Ptr, {i8Ptr, i8Ptr}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sad_security_encrypt", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {text, key}, "encrypt.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecurityDecrypt(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *text = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *key = cg_.resolveOperand(inst->operands[1]);
            if (!text || !key)
                return nullptr;
            // Call runtime sad_security_decrypt(const char*, const char*) -> char*
            llvm::Type *i8Ptr = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8Ptr, {i8Ptr, i8Ptr}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sad_security_decrypt", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {text, key}, "decrypt.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecurityAssertType(std::shared_ptr<SIRInstruction> inst)
        {
            // Runtime type assertion: check that the value's type tag matches expected
            // Since Sad uses compile-time type checking, emit a runtime no-op but log
            // In debug mode, this could call a runtime check function
            if (!inst || inst->operands.empty())
                return nullptr;

            // Call a runtime helper that prints a warning if type mismatch
            // sad_security_assert_type(const char* expected_type, const char* actual_type)
            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);

            // If we have type metadata in the instruction, emit the check
            if (inst->operands.size() >= 2)
            {
                llvm::Value *expectedType = cg_.resolveOperand(inst->operands[0]);
                llvm::Value *actualVal = cg_.resolveOperand(inst->operands[1]);
                if (expectedType && actualVal)
                {
                    auto *ftType = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {ptrTy, ptrTy}, false);
                    auto fn = cg_.module_->getOrInsertFunction("sad_security_assert_type", ftType);
                    cg_.builder_->CreateCall(fn, {expectedType, actualVal});
                }
            }
            // Compile-time type safety is the primary mechanism
            // (AR) قيمة إشاريّة «عُولجت» — إرجاع nullptr كان يُسقط الموزّع عبر الطبقات
            //      فيبلّغ INT «وصلت بعدد معاملات غير متوقَّع (Unsupported opcode:207)»
            //      زائفًا رغم إصدار النداء (مؤكَّد تجريبيًّا). نمط «اطبع»/«مسح_الشاشة» (#185).
            // (EN) "Handled" sentinel — returning nullptr made the dispatcher fall
            //      through and report a spurious INT arity/opcode error although the
            //      call was emitted (verified live). Print/clear-screen pattern (#185).
            return llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0);
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecurityAssertEqual(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *a = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *b = cg_.resolveOperand(inst->operands[1]);
            if (!a || !b)
                return nullptr;
            // Check types: if both are pointers (strings), use string comparison
            if (a->getType()->isPointerTy() && b->getType()->isPointerTy())
            {
                if (cg_.freestanding_)
                {
                    // (AR) وضع حرّ: sad_security_assert_equal_str يعيش في runtime
                    //      المستضاف (sad_embedded_runtime.c) الذي لا يُربط مع
                    //      ‎-nostdlib‎ ⇒ كان فشل ربط غامضًا. نضمّن المقارنة:
                    //      فشل = (أ==null) أو (ب==null) أو strcmp≠0 ⇒ __sad_panic.
                    //      strcmp آمنة حرًّا — نسختها تُبثّ داخل الوحدة في
                    //      emitFreestandingRuntime (رقم 6).
                    // (EN) Freestanding: sad_security_assert_equal_str lives in the
                    //      hosted embedded runtime (not linked under -nostdlib) —
                    //      an opaque link failure. Inline instead:
                    //      fail = (a==null) || (b==null) || strcmp(a,b)!=0 → panic.
                    //      strcmp is freestanding-safe (in-module, runtime item 6).
                    auto *i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
                    auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
                    llvm::FunctionType *cmpFT = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
                    llvm::FunctionCallee cmpFn = cg_.module_->getOrInsertFunction("strcmp", cmpFT);
                    llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
                    llvm::BasicBlock *cmpBB = llvm::BasicBlock::Create(*cg_.context_, "aeq.str.cmp", curFunc);
                    llvm::BasicBlock *failBB = llvm::BasicBlock::Create(*cg_.context_, "aeq.str.fail", curFunc);
                    llvm::BasicBlock *passBB = llvm::BasicBlock::Create(*cg_.context_, "aeq.str.pass", curFunc);
                    llvm::Value *aNull = cg_.builder_->CreateICmpEQ(
                        a, llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(a->getType())), "aeq.str.anull");
                    llvm::Value *bNull = cg_.builder_->CreateICmpEQ(
                        b, llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(b->getType())), "aeq.str.bnull");
                    llvm::Value *anyNull = cg_.builder_->CreateOr(aNull, bNull, "aeq.str.null");
                    cg_.builder_->CreateCondBr(anyNull, failBB, cmpBB);
                    cg_.builder_->SetInsertPoint(cmpBB);
                    llvm::Value *cmp = cg_.builder_->CreateCall(cmpFn, {a, b}, "aeq.str.res");
                    llvm::Value *ne = cg_.builder_->CreateICmpNE(
                        cmp, llvm::ConstantInt::get(i32Ty, 0), "aeq.str.ne");
                    cg_.builder_->CreateCondBr(ne, failBB, passBB);
                    cg_.builder_->SetInsertPoint(failBB);
                    emitAbortOrFreestandingPanic(cg_);
                    cg_.builder_->SetInsertPoint(passBB);
                }
                else
                {
                    llvm::Type *i8Ptr = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
                    llvm::FunctionType *ft = llvm::FunctionType::get(
                        llvm::Type::getVoidTy(*cg_.context_), {i8Ptr, i8Ptr}, false);
                    llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sad_security_assert_equal_str", ft);
                    cg_.builder_->CreateCall(fn, {a, b});
                }
            }
            else
            {
                // Integer comparison: if a != b, abort
                // Ensure both are same type
                if (a->getType() != b->getType())
                {
                    if (a->getType()->isIntegerTy() && b->getType()->isIntegerTy())
                    {
                        unsigned aBits = a->getType()->getIntegerBitWidth();
                        unsigned bBits = b->getType()->getIntegerBitWidth();
                        if (aBits < bBits)
                            a = cg_.builder_->CreateSExt(a, b->getType());
                        else if (bBits < aBits)
                            b = cg_.builder_->CreateSExt(b, a->getType());
                    }
                }
                llvm::Value *cmp = cg_.builder_->CreateICmpEQ(a, b, "assert.eq");
                llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
                llvm::BasicBlock *failBB = llvm::BasicBlock::Create(*cg_.context_, "aeq.fail", curFunc);
                llvm::BasicBlock *passBB = llvm::BasicBlock::Create(*cg_.context_, "aeq.pass", curFunc);
                cg_.builder_->CreateCondBr(cmp, passBB, failBB);
                cg_.builder_->SetInsertPoint(failBB);
                emitAbortOrFreestandingPanic(cg_);
                cg_.builder_->SetInsertPoint(passBB);
            }
            // (AR) قيمة إشاريّة «عُولجت» — نمط «اطبع»/«مسح_الشاشة» (#185)؛ لا سجلّ نتيجة.
            // (EN) "Handled" sentinel — print/clear-screen pattern (#185); no result register.
            return llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0);
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecurityAssertGreater(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *a = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *b = cg_.resolveOperand(inst->operands[1]);
            if (!a || !b)
                return nullptr;
            // Ensure both are same integer type
            if (a->getType() != b->getType())
            {
                if (a->getType()->isIntegerTy() && b->getType()->isIntegerTy())
                {
                    unsigned aBits = a->getType()->getIntegerBitWidth();
                    unsigned bBits = b->getType()->getIntegerBitWidth();
                    if (aBits < bBits)
                        a = cg_.builder_->CreateSExt(a, b->getType());
                    else if (bBits < aBits)
                        b = cg_.builder_->CreateSExt(b, a->getType());
                }
            }
            llvm::Value *cmp = cg_.builder_->CreateICmpSGT(a, b, "assert.gt");
            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *failBB = llvm::BasicBlock::Create(*cg_.context_, "agt.fail", curFunc);
            llvm::BasicBlock *passBB = llvm::BasicBlock::Create(*cg_.context_, "agt.pass", curFunc);
            cg_.builder_->CreateCondBr(cmp, passBB, failBB);
            cg_.builder_->SetInsertPoint(failBB);
            emitAbortOrFreestandingPanic(cg_);
            cg_.builder_->SetInsertPoint(passBB);
            // (AR) قيمة إشاريّة «عُولجت» — نمط «اطبع»/«مسح_الشاشة» (#185)؛ لا سجلّ نتيجة.
            // (EN) "Handled" sentinel — print/clear-screen pattern (#185); no result register.
            return llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0);
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecuritySanitize(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *val = cg_.resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;
            // Call runtime sad_security_sanitize(const char*) -> char*
            llvm::Type *i8Ptr = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8Ptr, {i8Ptr}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sad_security_sanitize", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {val}, "sanitize.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecurityTimestamp(std::shared_ptr<SIRInstruction> inst)
        {
            // time(NULL) -> i64. The C signature is: time_t time(time_t*)
            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            llvm::FunctionType *ft = llvm::FunctionType::get(i64Ty, {ptrTy}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("time", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy))}, "time.ret");
            if (inst && inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecuritySecureRandom(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) عشوائي_آمن(الحد_الأدنى، الحد_الأقصى) — يستدعي
            //      sad_security_secure_random(i64, i64) -> i64 من وقت التشغيل
            //      المضمَّن (sad_embedded_runtime.c)، بنفس نمط بقيّة عائلة الأمن
            //      المستضافة (هاش/شفّر/فك_تشفير). هذا يستبدل نداءً سابقًا مباشرًا
            //      لِـBCryptGenRandom في LLVM IR كان: (أ) يفشل الربط دومًا — لم
            //      يُربط bcrypt.lib في أيّ من مسارات الربط الثلاثة، و(ب) يتجاهل
            //      معاملَي الحد الأدنى/الأقصى تمامًا (يُرجع عددًا كاملًا شبه غير
            //      محدود بدل مدى مطلوب). الآن: منطق التوليد (وحماية Windows/POSIX
            //      عبر مصدر عشوائيّة النظام) في مكان واحد قابل للاختبار.
            // (EN) Calls sad_security_secure_random(i64,i64)->i64 in the embedded
            //      runtime, matching the rest of the hosted security family. This
            //      replaces a previous direct BCryptGenRandom call in LLVM IR that
            //      (a) never linked (bcrypt.lib was linked nowhere) and (b) ignored
            //      the min/max arguments entirely.
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *minVal = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *maxVal = cg_.resolveOperand(inst->operands[1]);
            if (!minVal || !maxVal)
                return nullptr;
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            llvm::FunctionType *ft = llvm::FunctionType::get(i64Ty, {i64Ty, i64Ty}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sad_security_secure_random", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {minVal, maxVal}, "secure_random.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecurityBase64Encode(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *val = cg_.resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;
            // Call runtime sad_security_base64_encode(const char*) -> char*
            llvm::Type *i8Ptr = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8Ptr, {i8Ptr}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sad_security_base64_encode", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {val}, "base64.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinCryptoBlake3Hash(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *val = cg_.resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;
            // Call runtime sad_blake3_hash(const char*) -> char* (BLAKE3 hex string,
            // matching the interpreter — interpreter/src/builtins/builtin_module_crypto.cpp)
            llvm::Type *i8Ptr = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8Ptr, {i8Ptr}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sad_blake3_hash", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {val}, "blake3.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinCryptoBlake3KeyedHash(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *data = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *key = cg_.resolveOperand(inst->operands[1]);
            if (!data || !key)
                return nullptr;
            // Call runtime sad_blake3_keyed_hash(const char*, const char*) -> char*
            llvm::Type *i8Ptr = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8Ptr, {i8Ptr, i8Ptr}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sad_blake3_keyed_hash", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {data, key}, "blake3_keyed.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinCryptoKdfPbkdf2(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 3)
                return nullptr;
            llvm::Value *password = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *salt = cg_.resolveOperand(inst->operands[1]);
            llvm::Value *iterations = cg_.resolveOperand(inst->operands[2]);
            if (!password || !salt || !iterations)
                return nullptr;
            // Call runtime sad_kdf_pbkdf2(const char*, const char*, long long) -> char*
            // (PBKDF2-HMAC-SHA256, RFC 8018 — hex string, 32-byte fixed output)
            llvm::Type *i8Ptr = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::Type *i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            llvm::FunctionType *ft = llvm::FunctionType::get(i8Ptr, {i8Ptr, i8Ptr, i64Ty}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sad_kdf_pbkdf2", ft);
            llvm::Value *result = emitCryptoCallOrRaise(cg_, fn, {password, salt, iterations}, "pbkdf2.ret",
                "عدد تكرارات PBKDF2 يجب أن يكون أكبر من صفر");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinCryptoKdfHkdf(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 4)
                return nullptr;
            llvm::Value *secret = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *salt = cg_.resolveOperand(inst->operands[1]);
            llvm::Value *info = cg_.resolveOperand(inst->operands[2]);
            llvm::Value *length = cg_.resolveOperand(inst->operands[3]);
            if (!secret || !salt || !info || !length)
                return nullptr;
            // Call runtime sad_kdf_hkdf(const char*, const char*, const char*, long long) -> char*
            // (HKDF-SHA256, RFC 5869 — hex string, caller-chosen output length)
            llvm::Type *i8Ptr = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::Type *i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            llvm::FunctionType *ft = llvm::FunctionType::get(i8Ptr, {i8Ptr, i8Ptr, i8Ptr, i64Ty}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sad_kdf_hkdf", ft);
            llvm::Value *result = emitCryptoCallOrRaise(cg_, fn, {secret, salt, info, length}, "hkdf.ret",
                "طول ناتج HKDF يجب أن يكون بين 1 و8160 بايت");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinCryptoAeadEncrypt(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *text = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *key = cg_.resolveOperand(inst->operands[1]);
            if (!text || !key)
                return nullptr;
            // Call runtime sad_security_aead_encrypt(const char*, const char*) -> char*
            // (ChaCha20-Poly1305 AEAD, RFC 8439 — hex envelope [nonce][ct][tag])
            llvm::Type *i8Ptr = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8Ptr, {i8Ptr, i8Ptr}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sad_security_aead_encrypt", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {text, key}, "aead_enc.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinCryptoAeadDecrypt(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *envelope = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *key = cg_.resolveOperand(inst->operands[1]);
            if (!envelope || !key)
                return nullptr;
            // Call runtime sad_security_aead_decrypt(const char*, const char*) -> char*
            // (ChaCha20-Poly1305 AEAD, RFC 8439). Fail-closed: on tag mismatch or
            // malformed envelope the runtime returns NULL, which raises a catchable
            // exception below (matching the interpreter's حاول/امسك-catchable throw).
            llvm::Type *i8Ptr = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8Ptr, {i8Ptr, i8Ptr}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sad_security_aead_decrypt", ft);
            llvm::Value *result = emitCryptoCallOrRaise(cg_, fn, {envelope, key}, "aead_dec.ret",
                "فشل المصادقة — المغلّف مُحرَّف أو المفتاح خاطئ");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinCryptoKdfArgon2id(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 4)
                return nullptr;
            llvm::Value *password = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *salt = cg_.resolveOperand(inst->operands[1]);
            llvm::Value *memoryCostKib = cg_.resolveOperand(inst->operands[2]);
            llvm::Value *iterations = cg_.resolveOperand(inst->operands[3]);
            if (!password || !salt || !memoryCostKib || !iterations)
                return nullptr;
            // Call runtime sad_kdf_argon2id(const char*, const char*, long long, long long) -> char*
            // (Argon2id, RFC 9106, parallelism fixed at 1 — hex string, 32-byte fixed output)
            llvm::Type *argon2i8Ptr = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::Type *argon2i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            llvm::FunctionType *argon2ft = llvm::FunctionType::get(argon2i8Ptr, {argon2i8Ptr, argon2i8Ptr, argon2i64Ty, argon2i64Ty}, false);
            llvm::FunctionCallee argon2fn = cg_.module_->getOrInsertFunction("sad_kdf_argon2id", argon2ft);
            llvm::Value *argon2result = emitCryptoCallOrRaise(cg_, argon2fn,
                {password, salt, memoryCostKib, iterations}, "argon2id.ret",
                "بارامترات أرجون2 خارج الحدود الآمنة (تكلفة الذاكرة/عدد التكرارات/طول الملح)");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = argon2result;
            return argon2result;
        }

        // ولّد_مفتاح_خاص_x25519 — runtime sad_security_x25519_keygen_priv(void) -> char* (hex)
        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinCryptoX25519KeygenPriv(std::shared_ptr<SIRInstruction> inst)
        {
            llvm::Type *i8Ptr = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8Ptr, {}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sad_security_x25519_keygen_priv", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {}, "x25519_kg.ret");
            if (inst && inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        // اشتق_مفتاح_عام_x25519 — runtime sad_security_x25519_derive_pub(const char*) -> char*
        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinCryptoX25519DerivePub(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 1)
                return nullptr;
            llvm::Value *priv = cg_.resolveOperand(inst->operands[0]);
            if (!priv)
                return nullptr;
            llvm::Type *i8Ptr = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8Ptr, {i8Ptr}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sad_security_x25519_derive_pub", ft);
            llvm::Value *result = emitCryptoCallOrRaise(cg_, fn, {priv}, "x25519_pub.ret",
                "مفتاح x25519 الخاصّ غير صالح (يجب 64 حرفًا ست عشريًّا)");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        // تبادل_مفتاح — runtime sad_security_x25519_exchange(const char*, const char*) -> char*
        // (all-zero shared secret rejected inside the runtime — RFC 7748 §6.1)
        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinCryptoX25519Exchange(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *priv = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *peerPub = cg_.resolveOperand(inst->operands[1]);
            if (!priv || !peerPub)
                return nullptr;
            llvm::Type *i8Ptr = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8Ptr, {i8Ptr, i8Ptr}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sad_security_x25519_exchange", ft);
            llvm::Value *result = emitCryptoCallOrRaise(cg_, fn, {priv, peerPub}, "x25519_dh.ret",
                "تبادل مفتاح x25519 فشل — مدخل غير صالح أو سرّ مشترك صفريّ مرفوض");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        // ولّد_مفتاح_خاص_توقيع — runtime sad_security_ed25519_keygen_priv(void) -> char*
        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinCryptoEd25519KeygenPriv(std::shared_ptr<SIRInstruction> inst)
        {
            llvm::Type *i8Ptr = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8Ptr, {}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sad_security_ed25519_keygen_priv", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {}, "ed25519_kg.ret");
            if (inst && inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        // اشتق_مفتاح_عام_توقيع — runtime sad_security_ed25519_derive_pub(const char*) -> char*
        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinCryptoEd25519DerivePub(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 1)
                return nullptr;
            llvm::Value *seed = cg_.resolveOperand(inst->operands[0]);
            if (!seed)
                return nullptr;
            llvm::Type *i8Ptr = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8Ptr, {i8Ptr}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sad_security_ed25519_derive_pub", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {seed}, "ed25519_pub.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        // وقّع — runtime sad_security_ed25519_sign(const char*, const char*) -> char* (128-hex sig)
        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinCryptoEd25519Sign(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *msg = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *seed = cg_.resolveOperand(inst->operands[1]);
            if (!msg || !seed)
                return nullptr;
            llvm::Type *i8Ptr = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8Ptr, {i8Ptr, i8Ptr}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sad_security_ed25519_sign", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {msg, seed}, "ed25519_sign.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        // تحقق_توقيع — runtime sad_security_ed25519_verify(const char*, const char*, const char*) -> int
        // Query function: returns 0/1 (never throws). SIR result is Boolean; the print
        // path materializes صحيح/خطأ via ICmpNE(v,0) (same convention as اكتب_ملف).
        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinCryptoEd25519Verify(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 3)
                return nullptr;
            llvm::Value *msg = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *sig = cg_.resolveOperand(inst->operands[1]);
            llvm::Value *pub = cg_.resolveOperand(inst->operands[2]);
            if (!msg || !sig || !pub)
                return nullptr;
            llvm::Type *i8Ptr = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::Type *i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            llvm::FunctionType *ft = llvm::FunctionType::get(i32Ty, {i8Ptr, i8Ptr, i8Ptr}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sad_security_ed25519_verify", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {msg, sig, pub}, "ed25519_verify.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

}} // namespace Sad::LLVM
