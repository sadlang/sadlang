// ============================================================================
// (AR) ملف: llvm_codegen_builtins_network.cpp
// (EN) File: llvm_codegen_builtins_network.cpp
// المكون: Sad::LLVM
// المسار: compiler/src/backend/llvm/llvm_codegen_builtins_network.cpp
// ============================================================================
// (AR) توليد كود LLVM IR لدوال وحدة الشبكة المضمنة.
//      كل SIROpcode من القسم 21 (BUILTIN_NET_*) يُترجم إلى استدعاء
//      دالة C API مقابلة من مكتبة stdlib/network (sad_tcp_*, sad_http_*, ...).
//      النمط المتبع: getOrInsertFunction للحصول على/إنشاء تعريف الدالة الخارجية،
//      ثم CreateCall لإصدار استدعاء LLVM IR.
//      المقابض (handles) تُمرر كـ i64 (مؤشرات مخزنة كأعداد صحيحة).
//      النصوص تُمرر كـ i8* (مؤشرات لسلاسل C).
//      القيم المنطقية تُرجع كـ i64 (0 = خطأ، 1 = صحيح).
//
// (EN) Generates LLVM IR for network module builtin functions.
//      Each Section 21 SIROpcode (BUILTIN_NET_*) is translated to a call
//      to the corresponding C API function from stdlib/network (sad_tcp_*, sad_http_*, ...).
//      Pattern: getOrInsertFunction to get/create the extern function declaration,
//      then CreateCall to emit the LLVM IR call.
//      Handles are passed as i64 (pointers stored as integers).
//      Strings are passed as i8* (C string pointers).
//      Booleans are returned as i64 (0 = false, 1 = true).
// ============================================================================

#include "llvm_codegen.h"
#include "builders/builtins/network_builtins_codegen.h"
#include <iostream>
#include "bounds_checker.h" // (AR) فحص حدود موحَّد / (EN) unified bounds checking

using namespace Sad::Compiler::SIR;

namespace Sad
{
    namespace LLVM
    {

        // ============================================================================
        // (AR) دالة مساعدة: الحصول على نوع i8* (مؤشر نص C)
        // (EN) Helper: get i8* type (C string pointer)
        // ============================================================================
        static llvm::Type *getI8PtrTy(llvm::LLVMContext &ctx)
        {
            return llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(ctx));
        }

        // ============================================================================
        // (AR) دالة مساعدة: تطبيع نوع معامل الشبكة قبل الاستدعاء الخارجي.
        //      السبب الجذري هنا أن الـ SIR يحتفظ غالباً بالأعداد الصحيحة على هيئة i64
        //      بينما واجهات C الفعلية تستخدم مزيجاً من i16 و i32 و bool.
        //      لذلك نُطبّق التحويل في طبقة codegen بدلاً من ترك LLVM verifier
        //      يصطدم بتواقيع غير متطابقة.
        // (EN) Helper: normalize network argument type before an extern call.
        //      SIR values are commonly carried as i64, while the actual C APIs use
        //      a mix of i16, i32, and bool. We normalize here so LLVM sees the exact
        //      ABI-compatible signature.
        // ============================================================================
        static llvm::Value *adaptNetworkArgument(
            llvm::IRBuilder<> *builder,
            llvm::LLVMContext &ctx,
            llvm::Value *value,
            llvm::Type *expectedType)
        {
            if (!value || !expectedType || value->getType() == expectedType)
            {
                return value;
            }

            if (expectedType->isPointerTy())
            {
                if (value->getType()->isPointerTy())
                {
                    return builder->CreateBitCast(value, expectedType, "net.arg.ptrcast");
                }

                if (value->getType()->isIntegerTy())
                {
                    llvm::Type *i64Type = llvm::Type::getInt64Ty(ctx);
                    if (!value->getType()->isIntegerTy(64))
                    {
                        value = value->getType()->isIntegerTy(1)
                                    ? builder->CreateZExt(value, i64Type, "net.arg.zext64")
                                    : builder->CreateSExtOrTrunc(value, i64Type, "net.arg.sext64");
                    }
                    // (AR) فحص handle غير صالح: القيم السالبة (مثل -1) أو 0
                    //      تعني handle فاشل من emitHandleCall. نُحوّلها إلى nullptr
                    //      حتى تتمكن دوال C من فحصها بـ ensure_socket_handle.
                    //      هذا يصلح Bug 2: هل_متصلة(-1) كان يسبب crash لأن
                    //      IntToPtr(-1) ينتج 0xFFFF... وليس nullptr.
                    // (EN) Check for invalid handle: negative values (like -1) or 0
                    //      indicate a failed handle from emitHandleCall. We replace
                    //      them with nullptr so C functions can detect via
                    //      ensure_socket_handle. This fixes Bug 2: هل_متصلة(-1)
                    //      crashed because IntToPtr(-1) yields 0xFFFF... not nullptr.
                    llvm::Value *isInvalid = builder->CreateICmpSLE(
                        value,
                        llvm::ConstantInt::get(i64Type, 0),
                        "net.arg.handle_invalid");
                    llvm::Value *rawPtr = builder->CreateIntToPtr(value, expectedType, "net.h2p");
                    llvm::Value *nullPtr = llvm::ConstantPointerNull::get(
                        llvm::cast<llvm::PointerType>(expectedType));
                    return builder->CreateSelect(isInvalid, nullPtr, rawPtr, "net.arg.safe_handle");
                }

                return value;
            }

            if (!expectedType->isIntegerTy())
            {
                return value;
            }

            if (value->getType()->isPointerTy())
            {
                return builder->CreatePtrToInt(value, expectedType, "net.p2h");
            }

            if (!value->getType()->isIntegerTy())
            {
                return value;
            }

            auto *expectedInt = llvm::cast<llvm::IntegerType>(expectedType);
            unsigned expectedBits = expectedInt->getBitWidth();

            if (expectedBits == 1)
            {
                if (value->getType()->isIntegerTy(1))
                {
                    return value;
                }
                return builder->CreateICmpNE(
                    value,
                    llvm::ConstantInt::get(value->getType(), 0),
                    "net.arg.bool");
            }

            auto *actualInt = llvm::cast<llvm::IntegerType>(value->getType());
            unsigned actualBits = actualInt->getBitWidth();

            if (actualBits == expectedBits)
            {
                return value;
            }

            if (actualBits == 1)
            {
                return builder->CreateZExt(value, expectedType, "net.arg.zext");
            }

            if (actualBits < expectedBits)
            {
                return builder->CreateSExt(value, expectedType, "net.arg.sext");
            }

            return builder->CreateTrunc(value, expectedType, "net.arg.trunc");
        }

        // ============================================================================
        // (AR) دالة مساعدة: إصدار استدعاء دالة C خارجية بسيطة
        //      تُنشئ تعريف الدالة إذا لم يكن موجوداً، ثم تستدعيها.
        //      تدعم إرجاع i64 أو i8* أو void.
        //      المعاملات تُحوّل تلقائياً من SIR operands إلى LLVM values.
        // (EN) Helper: emit call to a simple C extern function.
        //      Creates function declaration if not present, then calls it.
        //      Supports returning i64, i8*, or void.
        //      Arguments are auto-resolved from SIR operands to LLVM values.
        // ============================================================================
        llvm::Value *NetworkBuiltinsCodeGen::emitNetworkCall(
            std::shared_ptr<SIRInstruction> inst,
            const char *cFuncName,
            llvm::Type *returnType,
            const std::vector<llvm::Type *> &paramTypes)
        {
            if (!inst)
                return nullptr;

            // (AR) تحويل المعاملات من SIR إلى LLVM
            // (EN) Resolve SIR operands to LLVM values
            std::vector<llvm::Value *> args;
            for (size_t i = 0; i < inst->operands.size(); i++)
            {
                llvm::Value *val = cg_.resolveOperand(inst->operands[i]);
                if (!val)
                {
                    cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_NULL_IR,
                                    {{"detail", cFuncName}});
                    return nullptr;
                }

                // (AR) تحويل النوع إذا لزم الأمر:
                //      i64 → ptr: IntToPtr (للمقابض المخزنة كأعداد)
                //      ptr → i64: PtrToInt (للمقابض المعادة من دوال C)
                // (EN) Type cast if needed:
                //      i64 → ptr: IntToPtr (for handles stored as integers)
                //      ptr → i64: PtrToInt (for handles returned from C functions)
                if (Sad::Security::BoundsChecker::checkArrayIndex(i, paramTypes.size()))
                {
                    val = adaptNetworkArgument(cg_.builder_.get(), *cg_.context_, val, paramTypes[i]);
                }

                args.push_back(val);
            }

            // (AR) تطبيع عدد المعاملات ليطابق توقيع C: إسقاط الزائد، ملء الناقص بـ null/zero.
            //      هذا يُعالج الفروقات بين توقيع stdlib (.ص) وتوقيع FFI (مثل user_data
            //      الاختياري في http callbacks)، ويمنع «Incorrect number of arguments».
            // (EN) Normalize arg count to match C signature: drop extras, pad missing with null/zero.
            //      Handles mismatches between .ص stdlib signatures and FFI signatures
            //      (e.g., optional user_data in http callbacks), preventing verifier errors.
            if (args.size() > paramTypes.size())
            {
                args.resize(paramTypes.size());
            }
            while (args.size() < paramTypes.size())
            {
                llvm::Type *missingTy = paramTypes[args.size()];
                if (missingTy->isPointerTy())
                    args.push_back(llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(missingTy)));
                else if (missingTy->isIntegerTy())
                    args.push_back(llvm::ConstantInt::get(missingTy, 0));
                else if (missingTy->isFloatingPointTy())
                    args.push_back(llvm::ConstantFP::get(missingTy, 0.0));
                else
                    args.push_back(llvm::Constant::getNullValue(missingTy));
            }

            // (AR) الحصول على أو إنشاء تعريف الدالة الخارجية
            // (EN) Get or create the extern function declaration
            llvm::FunctionType *ft = llvm::FunctionType::get(returnType, paramTypes, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction(cFuncName, ft);

            // (AR) إصدار استدعاء LLVM IR
            // (EN) Emit LLVM IR call
            llvm::Value *result = nullptr;
            if (returnType->isVoidTy())
            {
                cg_.builder_->CreateCall(fn, args);
                result = llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0);
            }
            else
            {
                std::string label = std::string(cFuncName) + ".ret";
                result = cg_.builder_->CreateCall(fn, args, label);

                // (AR) الإرجاعات الصحيحة تُطبَّع إلى i64 بينما تُترك المؤشرات كما هي.
                //      هذا يسمح لنا بتمييز مؤشرات النصوص عن مقابض الكائنات؛
                //      المقابض تُحوَّل لاحقاً في الطبقة المستدعية إلى i64 مع -1 عند الفشل.
                // (EN) Integer returns are normalized to i64 while pointers are kept raw.
                //      This lets the caller distinguish string pointers from opaque handles.
                //      Handle-returning builtins are converted later to i64 with -1 on failure.
                if (result->getType()->isPointerTy())
                {
                    // اترك المؤشر كما هو
                }
                // (AR) توسيع i32/i16 → i64 إذا لزم الأمر
                //      نستخدم ZExt دائماً لأن جميع دوال FFI للشبكة تُرجع
                //      قيماً غير مُوقَّعة: bool, uint16_t (المنفذ 0..65535),
                //      int32_t (أكواد الخطأ والحالة) — السالب هنا يعني فشل
                //      (-1 من handle calls) لكن تلك تُعالج عبر ptr وليس int.
                //      SExt كان يُحوّل uint16_t=65535 إلى -1 خطأً (Bug 1).
                // (EN) Extend i32/i16 → i64 if needed.
                //      We use ZExt always because all FFI network functions
                //      return unsigned values: bool, uint16_t (port 0..65535),
                //      int32_t (error/status codes). Negative values would only
                //      come from handle returns which use ptr path, not int.
                //      SExt was incorrectly converting uint16_t=65535 to -1 (Bug 1).
                else if (result->getType()->isIntegerTy() && !result->getType()->isIntegerTy(64))
                {
                    result = cg_.builder_->CreateZExt(
                        result,
                        llvm::Type::getInt64Ty(*cg_.context_),
                        result->getType()->isIntegerTy(1) ? "net.ret.bool" : "net.ret.zext");
                }
            }

            // (AR) تسجيل النتيجة في جدول الأسماء
            // (EN) Register result in named values table
            if (inst->result.has_value() && result)
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        // ============================================================================
        // (AR) الدالة الرئيسية: توليد كود LLVM IR لجميع تعليمات الشبكة
        //      تُستدعى من emitInstructionCore عبر switch على الـ opcode.
        //      تُرجع nullptr إذا لم يُطابق أي opcode شبكة.
        // (EN) Main function: generate LLVM IR for all network instructions.
        //      Called from emitInstructionCore via switch on opcode.
        //      Returns nullptr if opcode doesn't match any network opcode.
        // ============================================================================
        llvm::Value *NetworkBuiltinsCodeGen::emitNetworkBuiltin(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            llvm::Type *i64 = llvm::Type::getInt64Ty(*cg_.context_);
            llvm::Type *i32 = llvm::Type::getInt32Ty(*cg_.context_);
            llvm::Type *i16 = llvm::Type::getInt16Ty(*cg_.context_);
            llvm::Type *i1 = llvm::Type::getInt1Ty(*cg_.context_);
            llvm::Type *i8p = getI8PtrTy(*cg_.context_);
            llvm::Type *voidTy = llvm::Type::getVoidTy(*cg_.context_);
            llvm::Type *ptr = llvm::PointerType::getUnqual(*cg_.context_);

            auto emitHandleCall = [&](const char *cFuncName, const std::vector<llvm::Type *> &paramTypes) -> llvm::Value *
            {
                llvm::Value *raw = emitNetworkCall(inst, cFuncName, ptr, paramTypes);
                if (!raw)
                {
                    return nullptr;
                }

                llvm::Value *handleValue = cg_.builder_->CreatePtrToInt(raw, i64, "net.ret.handle");
                llvm::Value *isNull = cg_.builder_->CreateICmpEQ(
                    raw,
                    llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(raw->getType())),
                    "net.ret.is_null");
                llvm::Value *result = cg_.builder_->CreateSelect(
                    isNull,
                    llvm::ConstantInt::getSigned(i64, -1),
                    handleValue,
                    "net.ret.handle_or_neg1");

                if (inst->result.has_value())
                {
                    cg_.context_info_.namedValues[inst->result->name] = result;
                }
                return result;
            };

            auto emitCStringCall = [&](const char *cFuncName, const std::vector<llvm::Type *> &paramTypes) -> llvm::Value *
            {
                llvm::Value *raw = emitNetworkCall(inst, cFuncName, i8p, paramTypes);
                if (!raw)
                {
                    return nullptr;
                }

                llvm::Value *empty = cg_.builder_->CreateGlobalStringPtr("", "net.empty");
                llvm::Value *isNull = cg_.builder_->CreateICmpEQ(
                    raw,
                    llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(raw->getType())),
                    "net.str.is_null");
                llvm::Value *result = cg_.builder_->CreateSelect(isNull, empty, raw, "net.str.safe");

                if (inst->result.has_value())
                {
                    cg_.context_info_.namedValues[inst->result->name] = result;
                }
                return result;
            };

            auto emitVoidSuccess = [&](const char *cFuncName, const std::vector<llvm::Type *> &paramTypes) -> llvm::Value *
            {
                llvm::Value *ignored = emitNetworkCall(inst, cFuncName, voidTy, paramTypes);
                if (!ignored)
                {
                    return nullptr;
                }

                llvm::Value *result = llvm::ConstantInt::get(i64, 1);
                if (inst->result.has_value())
                {
                    cg_.context_info_.namedValues[inst->result->name] = result;
                }
                return result;
            };

            auto emitPreparedCall = [&](const char *cFuncName,
                                        llvm::Type *returnType,
                                        const std::vector<llvm::Value *> &preparedArgs,
                                        const std::vector<llvm::Type *> &paramTypes) -> llvm::Value *
            {
                if (preparedArgs.size() != paramTypes.size())
                {
                    cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS,
                                    {{"detail", cFuncName}});
                    return nullptr;
                }

                std::vector<llvm::Value *> args;
                args.reserve(preparedArgs.size());
                for (size_t i = 0; i < preparedArgs.size(); ++i)
                {
                    llvm::Value *arg = adaptNetworkArgument(cg_.builder_.get(), *cg_.context_, preparedArgs[i], paramTypes[i]);
                    if (!arg)
                    {
                        cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_NULL_IR,
                                        {{"detail", cFuncName}});
                        return nullptr;
                    }
                    args.push_back(arg);
                }

                llvm::FunctionType *ft = llvm::FunctionType::get(returnType, paramTypes, false);
                llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction(cFuncName, ft);

                llvm::Value *result = nullptr;
                if (returnType->isVoidTy())
                {
                    cg_.builder_->CreateCall(fn, args);
                    result = llvm::ConstantInt::get(i64, 0);
                }
                else
                {
                    std::string label = std::string(cFuncName) + ".ret";
                    result = cg_.builder_->CreateCall(fn, args, label);

                    if (result->getType()->isIntegerTy() && !result->getType()->isIntegerTy(64))
                    {
                        result = result->getType()->isIntegerTy(1)
                                     ? cg_.builder_->CreateZExt(result, i64, "net.ret.bool")
                                     : cg_.builder_->CreateSExtOrTrunc(result, i64, "net.ret.ext");
                    }
                }

                if (inst->result.has_value() && result)
                {
                    cg_.context_info_.namedValues[inst->result->name] = result;
                }

                return result;
            };

            auto emitPreparedHandleCall = [&](const char *cFuncName,
                                              const std::vector<llvm::Value *> &preparedArgs,
                                              const std::vector<llvm::Type *> &paramTypes) -> llvm::Value *
            {
                llvm::Value *raw = emitPreparedCall(cFuncName, ptr, preparedArgs, paramTypes);
                if (!raw)
                {
                    return nullptr;
                }

                llvm::Value *handleValue = cg_.builder_->CreatePtrToInt(raw, i64, "net.ret.handle");
                llvm::Value *isNull = cg_.builder_->CreateICmpEQ(
                    raw,
                    llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(raw->getType())),
                    "net.ret.is_null");
                llvm::Value *result = cg_.builder_->CreateSelect(
                    isNull,
                    llvm::ConstantInt::getSigned(i64, -1),
                    handleValue,
                    "net.ret.handle_or_neg1");

                if (inst->result.has_value())
                {
                    cg_.context_info_.namedValues[inst->result->name] = result;
                }
                return result;
            };

            auto emitPreparedCStringCall = [&](const char *cFuncName,
                                               const std::vector<llvm::Value *> &preparedArgs,
                                               const std::vector<llvm::Type *> &paramTypes) -> llvm::Value *
            {
                llvm::Value *raw = emitPreparedCall(cFuncName, i8p, preparedArgs, paramTypes);
                if (!raw)
                {
                    return nullptr;
                }

                llvm::Value *empty = cg_.builder_->CreateGlobalStringPtr("", "net.empty");
                llvm::Value *isNull = cg_.builder_->CreateICmpEQ(
                    raw,
                    llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(raw->getType())),
                    "net.str.is_null");
                llvm::Value *result = cg_.builder_->CreateSelect(isNull, empty, raw, "net.str.safe");

                if (inst->result.has_value())
                {
                    cg_.context_info_.namedValues[inst->result->name] = result;
                }
                return result;
            };

            auto getPreparedCStringLength = [&](llvm::Value *rawString) -> llvm::Value *
            {
                llvm::Value *stringValue = adaptNetworkArgument(cg_.builder_.get(), *cg_.context_, rawString, i8p);
                if (!stringValue)
                {
                    return llvm::ConstantInt::get(i64, 0);
                }

                llvm::FunctionType *strlenType = llvm::FunctionType::get(i64, {i8p}, false);
                llvm::FunctionCallee strlenFn = cg_.module_->getOrInsertFunction("strlen", strlenType);
                llvm::Value *isNull = cg_.builder_->CreateICmpEQ(
                    stringValue,
                    llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(stringValue->getType())),
                    "net.strlen.is_null");
                llvm::Value *measured = cg_.builder_->CreateCall(strlenFn, {stringValue}, "net.strlen");
                return cg_.builder_->CreateSelect(isNull, llvm::ConstantInt::get(i64, 0), measured, "net.strlen.safe");
            };

            switch (inst->opcode)
            {
                // ================================================================
                // --- (AR) مقابس TCP / (EN) TCP Sockets ---
                // ================================================================

            case SIROpcode::BUILTIN_NET_TCP_NEW:
                // (AR) sad_tcp_socket_create() → void* ثم يُطبَّع إلى handle أو -1
                return emitHandleCall("sad_tcp_socket_create", {});

            case SIROpcode::BUILTIN_NET_TCP_CONNECT:
                return emitNetworkCall(inst, "sad_tcp_socket_connect", i1, {ptr, i8p, i16});

            case SIROpcode::BUILTIN_NET_TCP_SEND:
                if (inst->operands.size() >= 2)
                {
                    llvm::Value *socket = cg_.resolveOperand(inst->operands[0]);
                    llvm::Value *data = cg_.resolveOperand(inst->operands[1]);
                    llvm::Value *size = (inst->operands.size() >= 3)
                                            ? cg_.resolveOperand(inst->operands[2])
                                            : getPreparedCStringLength(data);
                    return emitPreparedCall("sad_tcp_socket_send", i32, {socket, data, size}, {ptr, i8p, i64});
                }
                return emitNetworkCall(inst, "sad_tcp_socket_send", i32, {ptr, i8p, i64});

            case SIROpcode::BUILTIN_NET_TCP_RECV:
                if (!inst->operands.empty())
                {
                    llvm::Value *socket = cg_.resolveOperand(inst->operands[0]);
                    llvm::Value *size = (inst->operands.size() >= 2)
                                            ? cg_.resolveOperand(inst->operands[1])
                                            : llvm::ConstantInt::get(i64, 4096);
                    return emitPreparedCStringCall("sad_tcp_socket_receive_string", {socket, size}, {ptr, i64});
                }
                return emitCStringCall("sad_tcp_socket_receive_string", {ptr, i64});

            case SIROpcode::BUILTIN_NET_TCP_CLOSE:
                return emitVoidSuccess("sad_tcp_socket_close", {ptr});

            case SIROpcode::BUILTIN_NET_TCP_BIND:
                if (inst->operands.size() >= 3)
                {
                    return emitNetworkCall(inst, "sad_tcp_socket_bind_addr", i1, {ptr, i8p, i16});
                }
                return emitNetworkCall(inst, "sad_tcp_socket_bind", i1, {ptr, i16});

            case SIROpcode::BUILTIN_NET_TCP_LISTEN:
                if (!inst->operands.empty())
                {
                    llvm::Value *socket = cg_.resolveOperand(inst->operands[0]);
                    llvm::Value *backlog = (inst->operands.size() >= 2)
                                               ? cg_.resolveOperand(inst->operands[1])
                                               : llvm::ConstantInt::get(i32, 10);
                    return emitPreparedCall("sad_tcp_socket_listen", i1, {socket, backlog}, {ptr, i32});
                }
                return emitNetworkCall(inst, "sad_tcp_socket_listen", i1, {ptr, i32});

            case SIROpcode::BUILTIN_NET_TCP_ACCEPT:
                return emitHandleCall("sad_tcp_socket_accept", {ptr});

            case SIROpcode::BUILTIN_NET_TCP_CONNECTED:
                return emitNetworkCall(inst, "sad_tcp_socket_is_connected", i1, {ptr});

            case SIROpcode::BUILTIN_NET_TCP_REMOTE_ADDR:
                return emitCStringCall("sad_tcp_socket_remote_addr", {ptr});

                // ================================================================
                // --- (AR) مقابس UDP / (EN) UDP Sockets ---
                // ================================================================

            case SIROpcode::BUILTIN_NET_UDP_NEW:
                return emitHandleCall("sad_udp_socket_create", {});

            case SIROpcode::BUILTIN_NET_UDP_BIND:
                if (inst->operands.size() >= 3)
                {
                    return emitNetworkCall(inst, "sad_udp_socket_bind_addr", i1, {ptr, i8p, i16});
                }
                return emitNetworkCall(inst, "sad_udp_socket_bind", i1, {ptr, i16});

            case SIROpcode::BUILTIN_NET_UDP_SEND:
                if (inst->operands.size() >= 4)
                {
                    llvm::Value *socket = cg_.resolveOperand(inst->operands[0]);
                    llvm::Value *data = cg_.resolveOperand(inst->operands[1]);
                    llvm::Value *size = (inst->operands.size() >= 5)
                                            ? cg_.resolveOperand(inst->operands[2])
                                            : getPreparedCStringLength(data);
                    llvm::Value *host = cg_.resolveOperand(inst->operands[inst->operands.size() >= 5 ? 3 : 2]);
                    llvm::Value *port = cg_.resolveOperand(inst->operands[inst->operands.size() >= 5 ? 4 : 3]);
                    return emitPreparedCall("sad_udp_socket_send_to", i32, {socket, data, size, host, port}, {ptr, i8p, i64, i8p, i16});
                }
                return emitNetworkCall(inst, "sad_udp_socket_send_to", i32, {ptr, i8p, i64, i8p, i16});

            case SIROpcode::BUILTIN_NET_UDP_RECV:
                if (!inst->operands.empty())
                {
                    llvm::Value *socket = cg_.resolveOperand(inst->operands[0]);
                    llvm::Value *size = (inst->operands.size() >= 2)
                                            ? cg_.resolveOperand(inst->operands[1])
                                            : llvm::ConstantInt::get(i64, 4096);
                    return emitPreparedCStringCall("sad_udp_socket_receive_string", {socket, size}, {ptr, i64});
                }
                return emitCStringCall("sad_udp_socket_receive_string", {ptr, i64});

            case SIROpcode::BUILTIN_NET_UDP_CLOSE:
                return emitVoidSuccess("sad_udp_socket_close", {ptr});

                // ================================================================
                // --- (AR) إعدادات المقابس / (EN) Socket Settings ---
                // ================================================================

            case SIROpcode::BUILTIN_NET_RECV_TIMEOUT:
                return emitNetworkCall(inst, "sad_socket_set_recv_timeout", i1, {ptr, i32});

            case SIROpcode::BUILTIN_NET_SEND_TIMEOUT:
                return emitNetworkCall(inst, "sad_socket_set_send_timeout", i1, {ptr, i32});

            case SIROpcode::BUILTIN_NET_NO_DELAY:
                return emitVoidSuccess("sad_tcp_socket_set_no_delay", {ptr, i1});

            case SIROpcode::BUILTIN_NET_LOCAL_PORT:
                return emitNetworkCall(inst, "sad_socket_local_port", i32, {ptr});

                // ================================================================
                // --- (AR) عميل HTTP / (EN) HTTP Client ---
                // ================================================================

            case SIROpcode::BUILTIN_NET_HTTP_NEW_CLIENT:
                return emitHandleCall("sad_http_client_new", {});

            case SIROpcode::BUILTIN_NET_HTTP_FREE_CLIENT:
                return emitVoidSuccess("sad_http_client_free", {ptr});

            case SIROpcode::BUILTIN_NET_HTTP_GET:
                return emitHandleCall("sad_http_client_get", {ptr, i8p});

            case SIROpcode::BUILTIN_NET_HTTP_POST:
                return emitHandleCall("sad_http_client_post", {ptr, i8p, i8p});

            case SIROpcode::BUILTIN_NET_HTTP_PUT:
                return emitHandleCall("sad_http_client_put", {ptr, i8p, i8p});

            case SIROpcode::BUILTIN_NET_HTTP_DELETE:
                return emitHandleCall("sad_http_client_delete", {ptr, i8p});

            case SIROpcode::BUILTIN_NET_HTTP_PATCH:
                return emitHandleCall("sad_http_client_patch", {ptr, i8p, i8p});

            case SIROpcode::BUILTIN_NET_HTTP_SET_BASE:
                return emitVoidSuccess("sad_http_client_set_base_url", {ptr, i8p});

            case SIROpcode::BUILTIN_NET_HTTP_SET_HEADER:
                return emitVoidSuccess("sad_http_client_set_header", {ptr, i8p, i8p});

            case SIROpcode::BUILTIN_NET_HTTP_SET_TIMEOUT:
                return emitVoidSuccess("sad_http_client_set_timeout", {ptr, i32});

            case SIROpcode::BUILTIN_NET_HTTP_SET_BEARER:
                return emitVoidSuccess("sad_http_client_set_bearer_token", {ptr, i8p});

            case SIROpcode::BUILTIN_NET_HTTP_LAST_ERROR:
                return emitCStringCall("sad_http_client_last_error", {ptr});

            case SIROpcode::BUILTIN_NET_HTTP_IS_OK:
                return emitNetworkCall(inst, "sad_http_client_is_ok", i32, {ptr});

                // ================================================================
                // --- (AR) استجابة HTTP / (EN) HTTP Response ---
                // ================================================================

            case SIROpcode::BUILTIN_NET_RESP_FREE:
                return emitVoidSuccess("sad_http_response_free", {ptr});

            case SIROpcode::BUILTIN_NET_RESP_STATUS:
                return emitNetworkCall(inst, "sad_http_response_status", i32, {ptr});

            case SIROpcode::BUILTIN_NET_RESP_BODY:
                return emitCStringCall("sad_http_response_body", {ptr});

            case SIROpcode::BUILTIN_NET_RESP_HEADER:
                return emitCStringCall("sad_http_response_header", {ptr, i8p});

            case SIROpcode::BUILTIN_NET_RESP_SUCCESS:
                return emitNetworkCall(inst, "sad_http_response_is_success", i32, {ptr});

                // ================================================================
                // --- (AR) خادم HTTP / (EN) HTTP Server ---
                // ================================================================

            case SIROpcode::BUILTIN_NET_SRV_NEW:
                return emitHandleCall("sad_http_server_new", {i32});

            case SIROpcode::BUILTIN_NET_SRV_FREE:
                return emitVoidSuccess("sad_http_server_free", {ptr});

            case SIROpcode::BUILTIN_NET_SRV_ON_GET:
                return emitVoidSuccess("sad_http_server_get_cb", {ptr, i8p, ptr, ptr});

            case SIROpcode::BUILTIN_NET_SRV_ON_POST:
                return emitVoidSuccess("sad_http_server_post_cb", {ptr, i8p, ptr, ptr});

            case SIROpcode::BUILTIN_NET_SRV_ON_PUT:
                return emitVoidSuccess("sad_http_server_put_cb", {ptr, i8p, ptr, ptr});

            case SIROpcode::BUILTIN_NET_SRV_ON_DELETE:
                return emitVoidSuccess("sad_http_server_delete_cb", {ptr, i8p, ptr, ptr});

            case SIROpcode::BUILTIN_NET_SRV_LISTEN:
                return emitVoidSuccess("sad_http_server_listen", {ptr});

            case SIROpcode::BUILTIN_NET_SRV_STOP:
                return emitVoidSuccess("sad_http_server_stop", {ptr});

            case SIROpcode::BUILTIN_NET_SRV_CORS:
                return emitVoidSuccess("sad_http_server_enable_cors", {ptr, i8p});

                // ================================================================
                // --- (AR) بيانات الطلب / (EN) Request Data ---
                // ================================================================

            case SIROpcode::BUILTIN_NET_REQ_METHOD:
                return emitCStringCall("sad_http_request_method", {ptr});

            case SIROpcode::BUILTIN_NET_REQ_PATH:
                return emitCStringCall("sad_http_request_path", {ptr});

            case SIROpcode::BUILTIN_NET_REQ_BODY:
                return emitCStringCall("sad_http_request_body", {ptr});

            case SIROpcode::BUILTIN_NET_REQ_HEADER:
                return emitCStringCall("sad_http_request_header", {ptr, i8p});

            case SIROpcode::BUILTIN_NET_REQ_QUERY:
                return emitCStringCall("sad_http_request_query_param", {ptr, i8p});

                // ================================================================
                // --- (AR) بناء الاستجابة / (EN) Response Building ---
                // ================================================================

            case SIROpcode::BUILTIN_NET_RESP_SET_STATUS:
                return emitVoidSuccess("sad_http_response_set_status", {ptr, i32});

            case SIROpcode::BUILTIN_NET_RESP_SET_BODY:
                return emitVoidSuccess("sad_http_response_set_body", {ptr, i8p});

            case SIROpcode::BUILTIN_NET_RESP_SET_JSON:
                return emitVoidSuccess("sad_http_response_set_json", {ptr, i8p});

            case SIROpcode::BUILTIN_NET_RESP_SET_HTML:
                return emitVoidSuccess("sad_http_response_set_html", {ptr, i8p});

            case SIROpcode::BUILTIN_NET_RESP_SET_HEADER:
                return emitVoidSuccess("sad_http_response_set_header", {ptr, i8p, i8p});

                // ================================================================
                // --- (AR) أدوات الشبكة / (EN) Network Utilities ---
                // ================================================================

            case SIROpcode::BUILTIN_NET_INIT:
                return emitNetworkCall(inst, "sad_network_init", i1, {});

            case SIROpcode::BUILTIN_NET_CLEANUP:
                return emitVoidSuccess("sad_network_cleanup", {});

            case SIROpcode::BUILTIN_NET_ERROR_CODE:
                return emitNetworkCall(inst, "sad_network_get_last_error", i32, {});

            case SIROpcode::BUILTIN_NET_ERROR_MSG:
                return emitCStringCall("sad_network_get_last_error_message", {});

                // ================================================================
                // --- (AR) العناوين / (EN) Addresses ---
                // ================================================================

            case SIROpcode::BUILTIN_NET_ADDR_NEW:
                return emitHandleCall("sad_socket_address_create", {i8p, i16});

            case SIROpcode::BUILTIN_NET_ADDR_NEW_V6:
                return emitHandleCall("sad_socket_address_create_v6", {i8p, i16});

            case SIROpcode::BUILTIN_NET_ADDR_FREE:
                return emitVoidSuccess("sad_socket_address_delete", {ptr});

            case SIROpcode::BUILTIN_NET_ADDR_IP:
                return emitCStringCall("sad_socket_address_get_ip", {ptr});

            case SIROpcode::BUILTIN_NET_ADDR_PORT:
                return emitNetworkCall(inst, "sad_socket_address_get_port", i16, {ptr});

            case SIROpcode::BUILTIN_NET_ADDR_IS_V4:
                return emitNetworkCall(inst, "sad_socket_address_is_ipv4", i1, {ptr});

            case SIROpcode::BUILTIN_NET_ADDR_IS_V6:
                return emitNetworkCall(inst, "sad_socket_address_is_ipv6", i1, {ptr});

            case SIROpcode::BUILTIN_NET_ADDR_STR:
                return emitCStringCall("sad_socket_address_to_string", {ptr});

                // ================================================================
                // --- (AR) عميل WebSocket / (EN) WebSocket Client ---
                // ================================================================

            case SIROpcode::BUILTIN_NET_WS_CLIENT_NEW:
                return emitHandleCall("sad_ws_client_new", {});

            case SIROpcode::BUILTIN_NET_WS_CLIENT_FREE:
                return emitVoidSuccess("sad_ws_client_free", {ptr});

            case SIROpcode::BUILTIN_NET_WS_CLIENT_CONNECT:
                return emitNetworkCall(inst, "sad_ws_client_connect", i1, {ptr, i8p});

            case SIROpcode::BUILTIN_NET_WS_CLIENT_CLOSE:
            {
                // (AR) sad_ws_client_close يُرجع void — نُرجع true دائماً للتوافق
                // (EN) sad_ws_client_close returns void — return true always for compat
                emitVoidSuccess("sad_ws_client_close", {ptr, i16, i8p});
                return llvm::ConstantInt::get(i1, 1);
            }

            case SIROpcode::BUILTIN_NET_WS_CLIENT_IS_CONNECTED:
                return emitNetworkCall(inst, "sad_ws_client_is_connected", i1, {ptr});

            case SIROpcode::BUILTIN_NET_WS_CLIENT_SEND:
                return emitNetworkCall(inst, "sad_ws_client_send", i1, {ptr, i8p});

            case SIROpcode::BUILTIN_NET_WS_CLIENT_PING:
                return emitNetworkCall(inst, "sad_ws_client_ping", i1, {ptr});

            case SIROpcode::BUILTIN_NET_WS_CLIENT_RECEIVE:
                return emitCStringCall("sad_ws_client_receive", {ptr, i32});

            case SIROpcode::BUILTIN_NET_WS_CLIENT_HAS_MESSAGE:
                return emitNetworkCall(inst, "sad_ws_client_has_message", i1, {ptr});

            case SIROpcode::BUILTIN_NET_WS_CLIENT_GET_URL:
                return emitCStringCall("sad_ws_client_get_url", {ptr});

            case SIROpcode::BUILTIN_NET_WS_CLIENT_LAST_ERROR:
                return emitCStringCall("sad_ws_client_last_error", {ptr});

            case SIROpcode::BUILTIN_NET_WS_CLIENT_RECV_TIMEOUT:
                return emitVoidSuccess("sad_ws_client_set_recv_timeout", {ptr, i32});

            case SIROpcode::BUILTIN_NET_WS_CLIENT_SEND_TIMEOUT:
                return emitVoidSuccess("sad_ws_client_set_send_timeout", {ptr, i32});

                // ================================================================
                // --- (AR) خادم WebSocket / (EN) WebSocket Server ---
                // ================================================================

            case SIROpcode::BUILTIN_NET_WS_SERVER_NEW:
                return emitHandleCall("sad_ws_server_new", {});

            case SIROpcode::BUILTIN_NET_WS_SERVER_FREE:
                return emitVoidSuccess("sad_ws_server_free", {ptr});

            case SIROpcode::BUILTIN_NET_WS_SERVER_START:
                return emitNetworkCall(inst, "sad_ws_server_start", i1, {ptr, i16, i8p});

            case SIROpcode::BUILTIN_NET_WS_SERVER_STOP:
            {
                emitVoidSuccess("sad_ws_server_stop", {ptr});
                return llvm::ConstantInt::get(i1, 1);
            }

            case SIROpcode::BUILTIN_NET_WS_SERVER_IS_RUNNING:
                return emitNetworkCall(inst, "sad_ws_server_is_running", i1, {ptr});

            case SIROpcode::BUILTIN_NET_WS_SERVER_CONN_COUNT:
                return emitNetworkCall(inst, "sad_ws_server_connection_count", i32, {ptr});

            case SIROpcode::BUILTIN_NET_WS_SERVER_GET_PORT:
                return emitNetworkCall(inst, "sad_ws_server_get_port", i16, {ptr});

            case SIROpcode::BUILTIN_NET_WS_SERVER_BROADCAST:
                return emitNetworkCall(inst, "sad_ws_server_broadcast", i32, {ptr, i8p});

            case SIROpcode::BUILTIN_NET_WS_SERVER_BROADCAST_ROOM:
                return emitNetworkCall(inst, "sad_ws_server_broadcast_to_room", i32, {ptr, i8p, i8p});

            case SIROpcode::BUILTIN_NET_WS_SERVER_ROOM_COUNT:
                return emitNetworkCall(inst, "sad_ws_server_room_count", i32, {ptr});

            case SIROpcode::BUILTIN_NET_WS_SERVER_ROOM_SIZE:
                return emitNetworkCall(inst, "sad_ws_server_room_size", i32, {ptr, i8p});

            case SIROpcode::BUILTIN_NET_WS_SERVER_CLOSE_ALL:
            {
                emitVoidSuccess("sad_ws_server_close_all", {ptr});
                return llvm::ConstantInt::get(i1, 1);
            }

            default:
                return nullptr;
            }
        }

    } // namespace LLVM
} // namespace Sad
