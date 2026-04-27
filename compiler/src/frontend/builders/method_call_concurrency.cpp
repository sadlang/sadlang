// ============================================================================
// sir_builder_method_call_concurrency.cpp — طرق التزامن (قنوات/أقفال/مستقبلات/انتظار)
// ============================================================================
// (AR) تم استخراج هذا الملف من sir_builder_method_call.cpp وفق CW-05/CW-01
//      يحتوي على دوال بناء استدعاءات طرق التزامن:
//        - b_.buildChannelMethodCall()   — أرسل/استقبل/أغلق/حجم/سعة/فارغة/...
//        - b_.buildMutexMethodCall()     — اقفل/افتح/مقفل/حاول_قفل
//        - b_.buildFutureMethodCall()    — عيّن/احصل/جاهز
//        - b_.buildWaitGroupMethodCall() — أضف/أنهي/انتظر/العداد
//
// (EN) Extracted from sir_builder_method_call.cpp per CW-05/CW-01
//      Contains concurrency method call builders:
//        - b_.buildChannelMethodCall()   — send/recv/close/size/capacity/empty/...
//        - b_.buildMutexMethodCall()     — lock/unlock/is_locked/try_lock
//        - b_.buildFutureMethodCall()    — set/get/is_ready
//        - b_.buildWaitGroupMethodCall() — add/done/wait/count
// ============================================================================

#include <string>
#include <optional>
#include "sir_builder.h"
#include "builders/method_call_builder.h"
#include "expressions.h"

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // ================================================================
            // (AR) دالة مساعدة: تحويل نتيجة التعبير إلى SIROperand بشكل آمن
            //      تتجنب std::stoll على قيم غير رقمية (مثل النصوص العربية)
            // (EN) Helper: safely convert BuildResult to SIROperand
            //      Avoids std::stoll on non-numeric values (e.g. Arabic strings)
            // ================================================================
            static SIROperand toSafeOperand(const BuildResult &result)
            {
                if (result.isConstant && !result.constantValue.empty() &&
                    (result.type == SadTypeKind::Integer || result.type == SadTypeKind::Boolean))
                {
                    try
                    {
                        return SIROperand::ConstantI64(std::stoll(result.constantValue));
                    }
                    catch (...)
                    {
                        // (AR) إذا فشل التحويل (قيمة غير رقمية)، نستخدم السجل
                        return SIROperand::Register(result.registerName, result.type);
                    }
                }
                return SIROperand::Register(result.registerName, result.type);
            }
            // ================================================================
            // b_.buildChannelMethodCall — طرق القنوات
            // (AR) أرسل/استقبل/حاول_ارسل/حاول_استقبل/أرسل_بمهلة/استقبل_بمهلة
            //      أغلق/مغلقة/فارغة/الحجم/السعة
            // (EN) send/recv/try_send/try_recv/send_timeout/recv_timeout
            //      close/isClosed/empty/size/capacity
            // ================================================================
            std::optional<BuildResult> MethodCallBuilder::buildChannelMethodCall(
                AST::MethodCallExpr *expr, const BuildResult &objResult)
            {
                std::string methodName = expr->methodName;

                // أرسل / send
                if (methodName == "\xD8\xA3\xD8\xB1\xD8\xB3\xD9\x84" || methodName == "\xD8\xA7\xD8\xB1\xD8\xB3\xD9\x84" ||
                    methodName == "send")
                {
                    SIROperand valueOp;
                    if (!expr->arguments.empty())
                    {
                        auto argResult = b_.buildExpression(expr->arguments[0].get());
                        valueOp = toSafeOperand(argResult);
                    }

                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::ASYNC_CHANNEL_SEND);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Void);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    inst.operands.push_back(valueOp);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);

                    // (AR) تسجيل نوع العنصر المُرسل عبر القناة لاستخدامه عند recv
                    //      نستخدم اسم المتغير من AST (إن أمكن) واسم السجل كبديل
                    if (valueOp.dataType != SadTypeKind::Unknown)
                    {
                        b_.channelTypeMap_[objResult.registerName] = valueOp.dataType;
                        // (AR) أيضاً سجّل باسم المتغير الأصلي من AST
                        if (auto *ident = dynamic_cast<AST::VariableExpr *>(expr->object.get()))
                            b_.channelTypeMap_[ident->name] = valueOp.dataType;
                    }

                    return BuildResult(resultReg, SadTypeKind::Void);
                }

                // حاول_ارسل / try_send — غير حاجب
                if (methodName == "\xD8\xAD\xD8\xA7\xD9\x88\xD9\x84_\xD8\xA7\xD8\xB1\xD8\xB3\xD9\x84" || methodName == "try_send")
                {
                    SIROperand valueOp = SIROperand::ConstantI64(0);
                    if (!expr->arguments.empty())
                    {
                        auto argResult = b_.buildExpression(expr->arguments[0].get());
                        valueOp = toSafeOperand(argResult);
                    }

                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::ASYNC_CHANNEL_TRY_SEND);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    inst.operands.push_back(valueOp);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // أرسل_بمهلة / send_timeout
                if (methodName == "\xD8\xA3\xD8\xB1\xD8\xB3\xD9\x84_\xD8\xA8\xD9\x85\xD9\x87\xD9\x84\xD8\xA9" || methodName == "send_timeout")
                {
                    SIROperand valueOp = SIROperand::ConstantI64(0);
                    SIROperand timeoutOp = SIROperand::ConstantI64(0);
                    if (!expr->arguments.empty())
                    {
                        auto arg0 = b_.buildExpression(expr->arguments[0].get());
                        valueOp = toSafeOperand(arg0);
                    }
                    if (expr->arguments.size() >= 2)
                    {
                        auto arg1 = b_.buildExpression(expr->arguments[1].get());
                        timeoutOp = toSafeOperand(arg1);
                    }

                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::ASYNC_CHANNEL_SEND_TIMEOUT);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    inst.operands.push_back(valueOp);
                    inst.operands.push_back(timeoutOp);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // استقبل / receive / recv
                if (methodName == "\xD8\xA7\xD8\xB3\xD8\xAA\xD9\x82\xD8\xA8\xD9\x84" || methodName == "receive" ||
                    methodName == "recv")
                {
                    // (AR) استخدم النوع المسجل من send إذا وُجد، وإلا Integer (الافتراضي)
                    //      السبب: b_.channelTypeMap_ قد لا يجد النوع عند send/recv عبر دوال مختلفة
                    //      (اسم المتغير مختلف بين الدالة المُرسلة والمُستقبلة)
                    //      Integer هو النوع الأكثر شيوعاً في القنوات
                    SadTypeKind recvType = SadTypeKind::Integer;
                    auto it = b_.channelTypeMap_.find(objResult.registerName);
                    if (it != b_.channelTypeMap_.end())
                        recvType = it->second;
                    else if (auto *ident = dynamic_cast<AST::VariableExpr *>(expr->object.get()))
                    {
                        auto it2 = b_.channelTypeMap_.find(ident->name);
                        if (it2 != b_.channelTypeMap_.end())
                            recvType = it2->second;
                    }

                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::ASYNC_CHANNEL_RECV);
                    inst.result = SIROperand::Register(resultReg, recvType);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, recvType);
                }

                // حاول_استقبل / try_recv — غير حاجب
                if (methodName == "\xD8\xAD\xD8\xA7\xD9\x88\xD9\x84_\xD8\xA7\xD8\xB3\xD8\xAA\xD9\x82\xD8\xA8\xD9\x84" || methodName == "try_recv")
                {
                    // (AR) حاول_استقبل قد تُرجع لاشيء (kSadNullSentinel) أو قيمة فعلية
                    //      نستخدم Integer — kSadNullSentinel هو قيمة i64 خاصة
                    //      فحص لاشيء يتم عبر مقارنة مع kSadNullSentinel
                    //      تحويل لنص يتم عبر فحص sentinel في ensureString
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::ASYNC_CHANNEL_TRY_RECV);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // استقبل_بمهلة / recv_timeout
                if (methodName == "\xD8\xA7\xD8\xB3\xD8\xAA\xD9\x82\xD8\xA8\xD9\x84_\xD8\xA8\xD9\x85\xD9\x87\xD9\x84\xD8\xA9" || methodName == "recv_timeout")
                {
                    SIROperand timeoutOp = SIROperand::ConstantI64(0);
                    if (!expr->arguments.empty())
                    {
                        auto arg0 = b_.buildExpression(expr->arguments[0].get());
                        timeoutOp = toSafeOperand(arg0);
                    }

                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::ASYNC_CHANNEL_RECV_TIMEOUT);
                    // (AR) recv_timeout قد تُرجع لاشيء (kSadNullSentinel) — نستخدم Integer
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    inst.operands.push_back(timeoutOp);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // أغلق / close
                if (methodName == "\xD8\xA3\xD8\xBA\xD9\x84\xD9\x82" || methodName == "\xD8\xA7\xD8\xBA\xD9\x84\xD9\x82" ||
                    methodName == "close")
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::ASYNC_CHANNEL_CLOSE);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Void);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Void);
                }

                // مغلقة / isClosed / is_closed
                if (methodName == "\xD9\x85\xD8\xBA\xD9\x84\xD9\x82\xD8\xA9" || methodName == "isClosed" ||
                    methodName == "is_closed")
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::ASYNC_CHANNEL_IS_CLOSED);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // فارغة / empty — عكس has_data
                if (methodName == "\xD9\x81\xD8\xA7\xD8\xB1\xD8\xBA\xD8\xA9" || methodName == "empty")
                {
                    std::string hasDataReg = b_.newTempRegister();
                    SIRInstruction hasDataInst(SIROpcode::ASYNC_CHANNEL_HAS_DATA);
                    hasDataInst.result = SIROperand::Register(hasDataReg, SadTypeKind::Integer);
                    hasDataInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(hasDataInst);

                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction cmpInst(SIROpcode::EQ);
                    cmpInst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    cmpInst.operands.push_back(SIROperand::Register(hasDataReg, SadTypeKind::Integer));
                    cmpInst.operands.push_back(SIROperand::ConstantI64(0));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(cmpInst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // الحجم / size — عدد العناصر الحالية في القناة
                if (methodName == "\xD8\xA7\xD9\x84\xD8\xAD\xD8\xAC\xD9\x85" || methodName == "size" ||
                    methodName == "getSize" || methodName == "get_size")
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::ASYNC_CHANNEL_SIZE);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // السعة / capacity — السعة القصوى للقناة
                if (methodName == "\xD8\xA7\xD9\x84\xD8\xB3\xD8\xB9\xD8\xA9" || methodName == "capacity")
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::ASYNC_CHANNEL_CAPACITY);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) طريقة غير معروفة على القناة — المتصل يتابع البحث
                return std::nullopt;
            }

            // ================================================================
            // b_.buildMutexMethodCall — طرق القفل
            // (AR) اقفل/افتح/مقفل/حاول_قفل
            // (EN) lock/unlock/is_locked/try_lock
            // ================================================================
            std::optional<BuildResult> MethodCallBuilder::buildMutexMethodCall(
                AST::MethodCallExpr *expr, const BuildResult &objResult)
            {
                std::string methodName = expr->methodName;

                // اقفل / lock
                if (methodName == "\xD8\xA7\xD9\x82\xD9\x81\xD9\x84" || methodName == "lock" ||
                    methodName == "\xD8\xA3\xD9\x82\xD9\x81\xD9\x84")
                {
                    SIRInstruction inst(SIROpcode::ASYNC_MUTEX_LOCK);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // افتح / unlock
                if (methodName == "\xD8\xA7\xD9\x81\xD8\xAA\xD8\xAD" || methodName == "unlock" ||
                    methodName == "\xD8\xA3\xD9\x81\xD8\xAA\xD8\xAD")
                {
                    SIRInstruction inst(SIROpcode::ASYNC_MUTEX_UNLOCK);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // مقفل / is_locked — يقرأ حالة القفل من بنية [handle, flag]
                if (methodName == "\xD9\x85\xD9\x82\xD9\x81\xD9\x84" || methodName == "is_locked" ||
                    methodName == "isLocked")
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::ASYNC_MUTEX_IS_LOCKED);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // حاول_قفل / try_lock
                if (methodName == "\xD8\xAD\xD8\xA7\xD9\x88\xD9\x84_\xD9\x82\xD9\x81\xD9\x84" || methodName == "try_lock" ||
                    methodName == "tryLock")
                {
                    SIRInstruction inst(SIROpcode::ASYNC_MUTEX_TRY_LOCK);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    std::string resultReg = b_.newTempRegister();
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    inst.comment = "try_lock";
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                return std::nullopt;
            }

            // ================================================================
            // b_.buildFutureMethodCall — طرق المستقبل
            // (AR) عيّن/احصل/جاهز
            // (EN) set/get/is_ready
            // ================================================================
            std::optional<BuildResult> MethodCallBuilder::buildFutureMethodCall(
                AST::MethodCallExpr *expr, const BuildResult &objResult)
            {
                std::string methodName = expr->methodName;

                // عيّن / set — تعيين النتيجة
                if (methodName == "\xD8\xB9\xD9\x8A\xD9\x91\xD9\x86" || methodName == "set" ||
                    methodName == "\xD8\xB9\xD9\x8A\xD9\x86")
                {
                    SIROperand valueOp;
                    if (!expr->arguments.empty())
                    {
                        auto argResult = b_.buildExpression(expr->arguments[0].get());
                        valueOp = toSafeOperand(argResult);
                    }
                    SIRInstruction inst(SIROpcode::ASYNC_RESOLVE_FUTURE);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    inst.operands.push_back(valueOp);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // احصل / get — الحصول على النتيجة (حجب)
                if (methodName == "\xD8\xA7\xD8\xAD\xD8\xB5\xD9\x84" || methodName == "get")
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::ASYNC_GET_FUTURE);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // جاهز / is_ready — هل النتيجة متوفرة
                if (methodName == "\xD8\xAC\xD8\xA7\xD9\x87\xD8\xB2" || methodName == "is_ready" ||
                    methodName == "isReady")
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::ASYNC_FUTURE_IS_READY);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                return std::nullopt;
            }

            // ================================================================
            // b_.buildWaitGroupMethodCall — طرق مجموعة الانتظار
            // (AR) أضف/أنهي/انتظر/العداد
            // (EN) add/done/wait/count
            // ================================================================
            std::optional<BuildResult> MethodCallBuilder::buildWaitGroupMethodCall(
                AST::MethodCallExpr *expr, const BuildResult &objResult)
            {
                std::string methodName = expr->methodName;

                // أضف / add — إضافة عدد مهام
                if (methodName == "\xD8\xA3\xD8\xB6\xD9\x81" || methodName == "add" ||
                    methodName == "\xD8\xA7\xD8\xB6\xD9\x81")
                {
                    SIROperand countOp;
                    if (!expr->arguments.empty())
                    {
                        auto argResult = b_.buildExpression(expr->arguments[0].get());
                        countOp = toSafeOperand(argResult);
                    }
                    else
                    {
                        countOp = SIROperand::ConstantI64(1);
                    }
                    SIRInstruction inst(SIROpcode::ASYNC_WG_ADD);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    inst.operands.push_back(countOp);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // أنهي / done — إنقاص العداد بواحد
                if (methodName == "\xD8\xA3\xD9\x86\xD9\x87\xD9\x8A" || methodName == "done" ||
                    methodName == "\xD8\xA7\xD9\x86\xD9\x87\xD9\x8A")
                {
                    SIRInstruction inst(SIROpcode::ASYNC_WG_DONE);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // انتظر / wait — حجب حتى العداد = 0
                if (methodName == "\xD8\xA7\xD9\x86\xD8\xAA\xD8\xB8\xD8\xB1" || methodName == "wait")
                {
                    SIRInstruction inst(SIROpcode::ASYNC_WG_WAIT);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // العداد / count — قراءة العداد الحالي
                if (methodName == "\xD8\xA7\xD9\x84\xD8\xB9\xD8\xAF\xD8\xA7\xD8\xAF" || methodName == "count" ||
                    methodName == "getCount")
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::ASYNC_WG_COUNT);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
