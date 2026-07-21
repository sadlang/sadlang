/*
 * ============================================================================
 * LLVM IR Code Generator - Exception Call Handlers
 * (AR) معالجة دوال runtime الاستثناءات المستدعاة عبر cg_.emitCall
 *      مستخرجة من llvm_codegen_branch_call.cpp بواسطة نمط Strangler Fig (CW-05)
 * (EN) Exception runtime call handlers extracted from llvm_codegen_branch_call.cpp
 *      via Strangler Fig pattern to comply with CW-05 (max 800 lines per file)
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "builders/arithmetic/exception_codegen.h"
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
#include <optional>
#include <iostream>
#include <fstream>

using namespace Sad::Compiler::SIR;
using namespace Sad::Compiler; // (AR) للوصول لثوابت sir_constants.h

namespace Sad
{
    namespace LLVM
    {
        /**
         * @brief (AR) معالجة دوال runtime الاستثناءات المضمنة في cg_.emitCall
         *             __sad_alloc_jmpbuf, __sad_push_handler, __sad_setjmp,
         *             __sad_pop_handler, __sad_raise, __sad_raise_current,
         *             __sad_get_exception, __sad_get_exception_type, __sad_str_equals
         *        (EN) Handle built-in exception runtime functions dispatched from cg_.emitCall
         *
         * @return std::nullopt اذا لم يكن funcName دالة استثناء (تابع البحث)
         *         std::optional(value) اذا جرى التعامل معها (قد تكون nullptr عند الخطأ)
         * @return std::nullopt if funcName is not an exception function (keep looking)
         *         std::optional(value) if handled (value may be nullptr on error)
         */
        std::optional<llvm::Value *> ExceptionCodeGen::emitCallException(
            const std::string &funcName,
            std::vector<llvm::Value *> &args,
            std::shared_ptr<SIRInstruction> inst)
        {
            // ================================================================
            // (AR) معالجة دوال runtime الاستثناءات (setjmp/longjmp)
            // (EN) Handle exception runtime functions (setjmp/longjmp based)
            // ================================================================

            if (funcName == "__sad_alloc_jmpbuf")
            {
                // (AR) تخصيص jmpbuf في كتلة الدخول (256 بايت لـ Windows x64)
                // (EN) Allocate jmpbuf in entry block (256 bytes for Windows x64)
                auto *currentBB = cg_.builder_->GetInsertBlock();
                auto *currentFunc = currentBB->getParent();
                auto &entryBB = currentFunc->getEntryBlock();

                llvm::IRBuilder<> entryBuilder(&entryBB, entryBB.begin());
                auto *jmpbufType = llvm::ArrayType::get(llvm::Type::getInt8Ty(*cg_.context_), 256);
                auto *jmpbuf = entryBuilder.CreateAlloca(jmpbufType, nullptr, "jmpbuf");
                jmpbuf->setAlignment(llvm::Align(16));

                llvm::Value *result = jmpbuf;
                if (inst->result.has_value())
                {
                    cg_.context_info_.namedValues[inst->result->name] = result;
                }
                return result;
            }

            // ================================================================
            // (AR) مدير السياق «باستخدام»: __دخول__ / __خروج__
            //      للقيم غير الكائنيّة لا عمل (مطابِق لسلوك المفسّر الذي يتخطّى
            //      enter/exit ما لم يكن المورد كائنًا يملك الطريقتين). توزيع
            //      __دخول__/__خروج__ على الكائنات غير مدعوم بعد في المترجم
            //      (يُتخطّى بأمان لا يُكسَر الربط) — راجع ISSUE في تغطية القواعد.
            // (EN) Context manager enter/exit — no-op for non-object resources,
            //      matching the interpreter which only dispatches __enter__/__exit__
            //      on objects that define them. Object dispatch is not yet emitted
            //      by the compiler; treated as a safe no-op (avoids undefined symbol).
            // ================================================================
            if (funcName == "__sad_context_enter" || funcName == "__sad_context_exit")
            {
                llvm::Value *dummy = llvm::ConstantInt::get(cg_.getInt64Type(), 0);
                if (inst->result.has_value())
                {
                    cg_.context_info_.namedValues[inst->result->name] = dummy;
                }
                return dummy;
            }

            if (funcName == "__sad_push_handler")
            {
                // (AR) دفع jmpbuf إلى مكدس المعالجات — نأخذ المؤشر مباشرة بدون تحميل
                // (EN) Push jmpbuf onto handler stack — get pointer directly without loading
                llvm::Value *jmpbufPtr = nullptr;
                if (inst->operands.size() > 1)
                {
                    auto it = cg_.context_info_.namedValues.find(inst->operands[1].name);
                    if (it != cg_.context_info_.namedValues.end())
                        jmpbufPtr = it->second;
                }
                if (!jmpbufPtr)
                    return nullptr;

                auto *ptrType = llvm::PointerType::getUnqual(*cg_.context_);
                auto *i32Type = llvm::Type::getInt32Ty(*cg_.context_);

                auto *handlerStack = cg_.module_->getNamedGlobal(kRuntimeHandlerStack);
                if (!handlerStack)
                {
                    auto *arrType = llvm::ArrayType::get(ptrType, 64);
                    handlerStack = new llvm::GlobalVariable(
                        *cg_.module_, arrType, false, llvm::GlobalValue::InternalLinkage,
                        llvm::ConstantAggregateZero::get(arrType), kRuntimeHandlerStack);
                }

                auto *handlerCount = cg_.module_->getNamedGlobal(kRuntimeHandlerCount);
                if (!handlerCount)
                {
                    handlerCount = new llvm::GlobalVariable(
                        *cg_.module_, i32Type, false, llvm::GlobalValue::InternalLinkage,
                        llvm::ConstantInt::get(i32Type, 0), kRuntimeHandlerCount);
                }

                llvm::Value *count = cg_.builder_->CreateLoad(i32Type, handlerCount, "handler_count");
                auto *arrType = llvm::ArrayType::get(ptrType, 64);
                llvm::Value *slot = cg_.builder_->CreateGEP(arrType, handlerStack,
                                                        {cg_.builder_->getInt32(0), count}, "handler_slot");
                cg_.builder_->CreateStore(jmpbufPtr, slot);
                llvm::Value *newCount = cg_.builder_->CreateAdd(count, cg_.builder_->getInt32(1), "new_count");
                cg_.builder_->CreateStore(newCount, handlerCount);

                llvm::Value *dummy = llvm::ConstantInt::get(cg_.getInt64Type(), 0);
                if (inst->result.has_value())
                {
                    cg_.context_info_.namedValues[inst->result->name] = dummy;
                }
                return dummy;
            }

            if (funcName == "__sad_setjmp")
            {
                // (AR) استدعاء _setjmp — نأخذ المؤشر مباشرة بدون تحميل
                // (EN) Call _setjmp — get pointer directly without loading
                llvm::Value *jmpbufPtr = nullptr;
                if (inst->operands.size() > 1)
                {
                    auto it = cg_.context_info_.namedValues.find(inst->operands[1].name);
                    if (it != cg_.context_info_.namedValues.end())
                        jmpbufPtr = it->second;
                }
                if (!jmpbufPtr)
                    return nullptr;

                auto *ptrType = llvm::PointerType::getUnqual(*cg_.context_);
                auto *i32Type = llvm::Type::getInt32Ty(*cg_.context_);

                auto *setjmpFuncType = llvm::FunctionType::get(i32Type, {ptrType, ptrType}, false);
                auto setjmpCallee = cg_.module_->getOrInsertFunction("_setjmp", setjmpFuncType);
                if (auto *setjmpFunc = llvm::dyn_cast<llvm::Function>(setjmpCallee.getCallee()))
                {
                    setjmpFunc->addFnAttr(llvm::Attribute::ReturnsTwice);
                }

                llvm::Value *nullPtr = llvm::ConstantPointerNull::get(ptrType);
                llvm::Value *result32 = cg_.builder_->CreateCall(setjmpCallee, {jmpbufPtr, nullPtr}, "setjmp_result");
                llvm::Value *result = cg_.builder_->CreateSExt(result32, cg_.getInt64Type(), "setjmp_result64");

                if (inst->result.has_value())
                {
                    cg_.context_info_.namedValues[inst->result->name] = result;
                }
                return result;
            }

            if (funcName == "__sad_pop_handler")
            {
                // (AR) إزالة آخر معالج من المكدس
                // (EN) Pop last handler from handler stack
                auto *i32Type = llvm::Type::getInt32Ty(*cg_.context_);

                auto *handlerCount = cg_.module_->getNamedGlobal(kRuntimeHandlerCount);
                if (!handlerCount)
                {
                    handlerCount = new llvm::GlobalVariable(
                        *cg_.module_, i32Type, false, llvm::GlobalValue::InternalLinkage,
                        llvm::ConstantInt::get(i32Type, 0), kRuntimeHandlerCount);
                }

                llvm::Value *count = cg_.builder_->CreateLoad(i32Type, handlerCount, "handler_count");
                llvm::Value *newCount = cg_.builder_->CreateSub(count, cg_.builder_->getInt32(1), "new_count");
                cg_.builder_->CreateStore(newCount, handlerCount);

                llvm::Value *dummy = llvm::ConstantInt::get(cg_.getInt64Type(), 0);
                if (inst->result.has_value())
                {
                    cg_.context_info_.namedValues[inst->result->name] = dummy;
                }
                return dummy;
            }

            if (funcName == "__sad_raise")
            {
                // (AR) رفع استثناء: تخزين النوع والرسالة + longjmp إلى آخر معالج
                // (EN) Raise exception: store type+message + longjmp to last handler
                // (AR) الصيغة: __sad_raise(type, msg) — إذا وسيط واحد فقط: type = "خطأ"
                // (EN) Format: __sad_raise(type, msg) — if single arg: type defaults to "خطأ"
                llvm::Value *excType = nullptr;
                llvm::Value *msg = nullptr;

                if (args.size() >= 2)
                {
                    excType = args[0];
                    msg = args[1];
                }
                else if (args.size() == 1)
                {
                    excType = cg_.getConstantString("\xd8\xae\xd8\xb7\xd8\xa3"); // "خطأ"
                    msg = args[0];
                }

                auto *ptrType = llvm::PointerType::getUnqual(*cg_.context_);
                auto *i32Type = llvm::Type::getInt32Ty(*cg_.context_);

                // (AR) تخزين نوع الاستثناء في متغير عام
                // (EN) Store exception type in global
                auto *exceptionType = cg_.module_->getNamedGlobal(kRuntimeExceptionType);
                if (!exceptionType)
                {
                    exceptionType = new llvm::GlobalVariable(
                        *cg_.module_, ptrType, false, llvm::GlobalValue::InternalLinkage,
                        llvm::ConstantPointerNull::get(ptrType), kRuntimeExceptionType);
                }
                if (excType)
                {
                    cg_.builder_->CreateStore(excType, exceptionType);
                }

                // (AR) تخزين رسالة الاستثناء في متغير عام
                // (EN) Store exception message in global
                auto *exceptionMsg = cg_.module_->getNamedGlobal(kRuntimeExceptionMsg);
                if (!exceptionMsg)
                {
                    exceptionMsg = new llvm::GlobalVariable(
                        *cg_.module_, ptrType, false, llvm::GlobalValue::InternalLinkage,
                        llvm::ConstantPointerNull::get(ptrType), kRuntimeExceptionMsg);
                }
                if (msg)
                {
                    cg_.builder_->CreateStore(msg, exceptionMsg);
                }

                // (AR) تحميل jmpbuf من مكدس المعالجات
                // (EN) Load jmpbuf from handler stack
                auto *handlerStack = cg_.module_->getNamedGlobal(kRuntimeHandlerStack);
                if (!handlerStack)
                {
                    auto *arrType = llvm::ArrayType::get(ptrType, 64);
                    handlerStack = new llvm::GlobalVariable(
                        *cg_.module_, arrType, false, llvm::GlobalValue::InternalLinkage,
                        llvm::ConstantAggregateZero::get(arrType), kRuntimeHandlerStack);
                }

                auto *handlerCount = cg_.module_->getNamedGlobal(kRuntimeHandlerCount);
                if (!handlerCount)
                {
                    handlerCount = new llvm::GlobalVariable(
                        *cg_.module_, i32Type, false, llvm::GlobalValue::InternalLinkage,
                        llvm::ConstantInt::get(i32Type, 0), kRuntimeHandlerCount);
                }

                llvm::Value *count = cg_.builder_->CreateLoad(i32Type, handlerCount, "handler_count");

                // (AR) لا معالج حاول/امسك مسجَّل (count == 0): longjmp إلى فهرس -1 قراءة
                //      خارج الحدود ⇒ UB/تعطّل. نتفرّع إلى مسار آمن يطبع الاستثناء ويخرج
                //      نظيفًا بدل تخمين jmpbuf غير موجود.
                // (EN) No حاول/امسك handler registered (count == 0): longjmp to index -1
                //      is an out-of-bounds read ⇒ UB/crash. Branch to a safe path that
                //      prints the exception and exits cleanly instead of guessing a
                //      nonexistent jmpbuf.
                llvm::Function *raiseCurFunc = cg_.builder_->GetInsertBlock()->getParent();
                llvm::BasicBlock *noHandlerBB =
                    llvm::BasicBlock::Create(*cg_.context_, "raise.nohandler", raiseCurFunc);
                llvm::BasicBlock *hasHandlerBB =
                    llvm::BasicBlock::Create(*cg_.context_, "raise.hashandler", raiseCurFunc);
                llvm::Value *hasHandler = cg_.builder_->CreateICmpSGT(
                    count, cg_.builder_->getInt32(0), "has_handler");
                cg_.builder_->CreateCondBr(hasHandler, hasHandlerBB, noHandlerBB);

                cg_.builder_->SetInsertPoint(noHandlerBB);
                auto *reportFuncType = llvm::FunctionType::get(
                    llvm::Type::getVoidTy(*cg_.context_), {ptrType, ptrType}, false);
                auto reportCallee = cg_.module_->getOrInsertFunction(
                    "sad_report_unhandled_exception", reportFuncType);
                // (AR) ارمِ يقبل أيّ قيمة (عدد/منطقي/كائن...) لا سلاسل فقط، فقد يصل type/msg
                //      هنا بنوع LLVM غير ptr (مثلاً i64 لـ«ارمي 42»). استدعاء دالّة الطباعة
                //      بتوقيع (ptr,ptr) يتطلّب مطابقة صارمة، فنستبدل القيمة غير-ptr بـ null
                //      بدل توليد IR غير سليم (verifyModule) أو inttoptr قد يقرأ عنوانًا وهميًّا.
                // (EN) ارمِ accepts any value (number/bool/object...), not only strings, so
                //      type/msg may arrive here with a non-ptr LLVM type (e.g. i64 for
                //      "ارمي 42"). The (ptr,ptr) print call requires an exact match, so a
                //      non-ptr value is replaced with null instead of emitting invalid IR
                //      or inttoptr'ing into a bogus address.
                llvm::Value *excTypeForReport =
                    (excType && excType->getType() == ptrType) ? excType : llvm::ConstantPointerNull::get(ptrType);
                llvm::Value *msgForReport =
                    (msg && msg->getType() == ptrType) ? msg : llvm::ConstantPointerNull::get(ptrType);
                cg_.builder_->CreateCall(reportCallee, {excTypeForReport, msgForReport});
                cg_.builder_->CreateUnreachable();

                cg_.builder_->SetInsertPoint(hasHandlerBB);
                llvm::Value *idx = cg_.builder_->CreateSub(count, cg_.builder_->getInt32(1), "handler_idx");

                auto *arrType = llvm::ArrayType::get(ptrType, 64);
                llvm::Value *slot = cg_.builder_->CreateGEP(arrType, handlerStack,
                                                        {cg_.builder_->getInt32(0), idx}, "handler_slot");
                llvm::Value *jmpbuf = cg_.builder_->CreateLoad(ptrType, slot, "jmpbuf");

                // (AR) استدعاء longjmp — لا يعود أبداً
                // (EN) Call longjmp — never returns
                auto *longjmpFuncType = llvm::FunctionType::get(
                    llvm::Type::getVoidTy(*cg_.context_), {ptrType, i32Type}, false);
                auto longjmpCallee = cg_.module_->getOrInsertFunction("longjmp", longjmpFuncType);
                if (auto *longjmpFunc = llvm::dyn_cast<llvm::Function>(longjmpCallee.getCallee()))
                {
                    longjmpFunc->addFnAttr(llvm::Attribute::NoReturn);
                }

                cg_.builder_->CreateCall(longjmpCallee, {jmpbuf, cg_.builder_->getInt32(1)});
                cg_.builder_->CreateUnreachable();

                // (AR) كتلة ميتة مع unreachable كـ terminator
                // (EN) Dead block with unreachable as terminator
                auto *deadBlock = llvm::BasicBlock::Create(
                    *cg_.context_, "dead.after_raise", cg_.builder_->GetInsertBlock()->getParent());
                cg_.builder_->SetInsertPoint(deadBlock);
                cg_.builder_->CreateUnreachable();

                llvm::Value *dummy = llvm::ConstantInt::get(cg_.getInt64Type(), 0);
                if (inst->result.has_value())
                {
                    cg_.context_info_.namedValues[inst->result->name] = dummy;
                }
                return dummy;
            }

            if (funcName == "__sad_raise_current")
            {
                // (AR) إعادة رمي الاستثناء الحالي — يقرأ النوع والرسالة من المتغيرات العامة
                // (EN) Re-throw current exception — reads type/msg from globals, calls longjmp
                auto *ptrType = llvm::PointerType::getUnqual(*cg_.context_);
                auto *i32Type = llvm::Type::getInt32Ty(*cg_.context_);

                // (AR) لا نغيّر المتغيرات العامة — نستخدم ما هو مخزن فعلاً
                // (EN) Don't modify globals — use what's already stored

                auto *handlerStack = cg_.module_->getNamedGlobal(kRuntimeHandlerStack);
                if (!handlerStack)
                {
                    auto *arrType = llvm::ArrayType::get(ptrType, 64);
                    handlerStack = new llvm::GlobalVariable(
                        *cg_.module_, arrType, false, llvm::GlobalValue::InternalLinkage,
                        llvm::ConstantAggregateZero::get(arrType), kRuntimeHandlerStack);
                }

                auto *handlerCount = cg_.module_->getNamedGlobal(kRuntimeHandlerCount);
                if (!handlerCount)
                {
                    handlerCount = new llvm::GlobalVariable(
                        *cg_.module_, i32Type, false, llvm::GlobalValue::InternalLinkage,
                        llvm::ConstantInt::get(i32Type, 0), kRuntimeHandlerCount);
                }

                llvm::Value *count = cg_.builder_->CreateLoad(i32Type, handlerCount, "handler_count");
                llvm::Value *idx = cg_.builder_->CreateSub(count, cg_.builder_->getInt32(1), "handler_idx");

                auto *arrType = llvm::ArrayType::get(ptrType, 64);
                llvm::Value *slot = cg_.builder_->CreateGEP(arrType, handlerStack,
                                                        {cg_.builder_->getInt32(0), idx}, "handler_slot");
                llvm::Value *jmpbuf = cg_.builder_->CreateLoad(ptrType, slot, "jmpbuf");

                auto *longjmpFuncType = llvm::FunctionType::get(
                    llvm::Type::getVoidTy(*cg_.context_), {ptrType, i32Type}, false);
                auto longjmpCallee = cg_.module_->getOrInsertFunction("longjmp", longjmpFuncType);
                if (auto *longjmpFunc = llvm::dyn_cast<llvm::Function>(longjmpCallee.getCallee()))
                {
                    longjmpFunc->addFnAttr(llvm::Attribute::NoReturn);
                }

                cg_.builder_->CreateCall(longjmpCallee, {jmpbuf, cg_.builder_->getInt32(1)});
                cg_.builder_->CreateUnreachable();

                auto *deadBlock = llvm::BasicBlock::Create(
                    *cg_.context_, "dead.after_rethrow", cg_.builder_->GetInsertBlock()->getParent());
                cg_.builder_->SetInsertPoint(deadBlock);
                cg_.builder_->CreateUnreachable();

                llvm::Value *dummy = llvm::ConstantInt::get(cg_.getInt64Type(), 0);
                if (inst->result.has_value())
                {
                    cg_.context_info_.namedValues[inst->result->name] = dummy;
                }
                return dummy;
            }

            if (funcName == "__sad_get_exception")
            {
                // (AR) تحميل رسالة الاستثناء المحفوظة
                // (EN) Load stored exception message
                auto *ptrType = llvm::PointerType::getUnqual(*cg_.context_);

                auto *exceptionMsg = cg_.module_->getNamedGlobal(kRuntimeExceptionMsg);
                if (!exceptionMsg)
                {
                    exceptionMsg = new llvm::GlobalVariable(
                        *cg_.module_, ptrType, false, llvm::GlobalValue::InternalLinkage,
                        llvm::ConstantPointerNull::get(ptrType), kRuntimeExceptionMsg);
                }

                llvm::Value *result = cg_.builder_->CreateLoad(ptrType, exceptionMsg, "exception_msg");
                if (inst->result.has_value())
                {
                    cg_.context_info_.namedValues[inst->result->name] = result;
                }
                return result;
            }

            if (funcName == "__sad_get_exception_type")
            {
                // (AR) تحميل نوع الاستثناء المحفوظ
                // (EN) Load stored exception type
                auto *ptrType = llvm::PointerType::getUnqual(*cg_.context_);

                auto *exceptionType = cg_.module_->getNamedGlobal(kRuntimeExceptionType);
                if (!exceptionType)
                {
                    exceptionType = new llvm::GlobalVariable(
                        *cg_.module_, ptrType, false, llvm::GlobalValue::InternalLinkage,
                        llvm::ConstantPointerNull::get(ptrType), kRuntimeExceptionType);
                }

                llvm::Value *result = cg_.builder_->CreateLoad(ptrType, exceptionType, "exception_type");
                if (inst->result.has_value())
                {
                    cg_.context_info_.namedValues[inst->result->name] = result;
                }
                return result;
            }

            if (funcName == "__sad_str_equals")
            {
                // (AR) مقارنة نصين — يستدعي strcmp ويعيد 1 إذا متساويين، 0 إذا لا
                // (EN) Compare two strings — calls strcmp, returns 1 if equal, 0 if not
                if (args.size() < 2)
                    return nullptr;

                auto *ptrType = llvm::PointerType::getUnqual(*cg_.context_);
                auto *i32Type = llvm::Type::getInt32Ty(*cg_.context_);

                auto *strcmpFuncType = llvm::FunctionType::get(i32Type, {ptrType, ptrType}, false);
                auto strcmpCallee = cg_.module_->getOrInsertFunction("strcmp", strcmpFuncType);

                llvm::Value *cmpResult = cg_.builder_->CreateCall(strcmpCallee, {args[0], args[1]}, "strcmp_result");
                llvm::Value *isEqual = cg_.builder_->CreateICmpEQ(cmpResult, cg_.builder_->getInt32(0), "str_eq");
                llvm::Value *result = cg_.builder_->CreateZExt(isEqual, cg_.getInt64Type(), "str_eq_i64");

                if (inst->result.has_value())
                {
                    cg_.context_info_.namedValues[inst->result->name] = result;
                }
                return result;
            }

            return std::nullopt;
        }

        // ============================================================================
        // markSetjmpGlobalsVolatile
        // ============================================================================
        // (AR) يمشي على كل دوال الوحدة. أي دالة تستدعي `_setjmp` تعني أنها تستخدم
        //      آلية try/catch المبنية على setjmp/longjmp. في هذه الدوال، المتغيرات
        //      العامة (internal globals) قد يُحوّلها LLVM GlobalOpt إلى SSA form
        //      تُخزَّن في callee-saved registers — وهذه الـ registers تُعاد لقيمها
        //      القديمة عند longjmp فتفسد النتائج (مثال: مجموع=50 بدلاً من 90).
        //
        //      الحل: جعل كل load/store يستهدف GlobalVariable في هذه الدوال volatile.
        //      Volatile يخبر LLVM بعدم cache القيمة في register أبداً، ويُجبر
        //      على القراءة/الكتابة المباشرة من/إلى الذاكرة في كل مرة.
        //
        //      يُستثنى من هذا التعديل:
        //      - __sad_handler_stack, __sad_handler_count: هي globals خاصة بالـ runtime
        //        وليست متغيرات مستخدم — تُعامَل بشكل صحيح أصلاً لأن LLVM يعرف أن
        //        longjmp لا يُعيد قيمها (هي تُعدَّل قبل longjmp وبعده وهذا مقصود).
        //        لكن نتركها volatile أيضاً لأمان إضافي.
        //
        // (EN) Walks all module functions. Any function calling `_setjmp` uses the
        //      setjmp/longjmp-based try/catch mechanism. In those functions, internal
        //      global variables may be promoted by LLVM GlobalOpt to SSA form stored
        //      in callee-saved registers — those registers are restored to old values
        //      by longjmp corrupting results (e.g. sum=50 instead of 90).
        //
        //      Fix: mark all load/store instructions targeting GlobalVariables in
        //      those functions as volatile. Volatile tells LLVM to never cache the
        //      value in a register and always read/write directly from/to memory.
        // ============================================================================
        void ExceptionCodeGen::markSetjmpGlobalsVolatile()
        {
            if (!cg_.module_)
                return;

            // (AR) مجموعة الدوال التي تستدعي _setjmp مباشرة
            // (EN) Set of functions directly calling _setjmp
            std::unordered_set<llvm::Function *> setjmpCallers;

            // (AR) مسح الوحدة للبحث عن مستدعي _setjmp
            // (EN) Scan module for _setjmp callers
            for (auto &F : *cg_.module_)
            {
                if (F.isDeclaration())
                    continue;

                for (auto &BB : F)
                {
                    for (auto &I : BB)
                    {
                        if (auto *callInst = llvm::dyn_cast<llvm::CallInst>(&I))
                        {
                            if (auto *callee = callInst->getCalledFunction())
                            {
                                // (AR) نبحث عن _setjmp (Windows) أو setjmp (Unix)
                                // (EN) Look for _setjmp (Windows) or setjmp (Unix)
                                llvm::StringRef name = callee->getName();
                                if (name == "_setjmp" || name == "setjmp" ||
                                    name == "_setjmpex" || name == "__sigsetjmp")
                                {
                                    setjmpCallers.insert(&F);
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            if (setjmpCallers.empty())
                return;

            // (AR) في كل دالة تستدعي setjmp: جعل جميع loads/stores على globals volatile
            // (EN) For each function calling setjmp: make all GlobalVariable loads/stores volatile
            for (auto *F : setjmpCallers)
            {
                for (auto &BB : *F)
                {
                    for (auto &I : BB)
                    {
                        if (auto *loadInst = llvm::dyn_cast<llvm::LoadInst>(&I))
                        {
                            // (AR) إذا كان المصدر GlobalVariable → volatile
                            // (EN) If source is a GlobalVariable → volatile
                            if (llvm::isa<llvm::GlobalVariable>(loadInst->getPointerOperand()))
                            {
                                loadInst->setVolatile(true);
                            }
                        }
                        else if (auto *storeInst = llvm::dyn_cast<llvm::StoreInst>(&I))
                        {
                            // (AR) إذا كان الهدف GlobalVariable → volatile
                            // (EN) If target is a GlobalVariable → volatile
                            if (llvm::isa<llvm::GlobalVariable>(storeInst->getPointerOperand()))
                            {
                                storeInst->setVolatile(true);
                            }
                        }
                    }
                }
            }
        }

    } // namespace LLVM
} // namespace Sad
