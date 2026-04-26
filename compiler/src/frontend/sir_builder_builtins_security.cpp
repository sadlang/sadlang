// ============================================================================
// sir_builder_builtins_security.cpp
// (AR) Security builtins: assert, hash, encrypt, sanitize
// (EN) Security Builtin Functions (14 functions)
// ============================================================================

#include "sir_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <optional>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            std::optional<BuildResult> SIRBuilder::buildBuiltinSystem_Security(
                const std::string &funcName,
                bool isUserDefinedFunction,
                std::vector<BuildResult> &argResults,
                std::vector<SIROperand> &argOperands)
            {

                // ========================================================================
                // (AR) دوال الأمان — Security Builtin Functions (14 functions)
                // (EN) Security Builtin Functions (14 functions)
                // ========================================================================
                // (AR) هذا القسم يضيف الدوال المضمنة لنظام الأمان:
                //      - تأكد/تحقق/آمن: فحص الشروط والتحقق من الأمان
                //      - ذعر: إيقاف طارئ مع رسالة
                //      - هاش/شفّر/فك_تشفير: عمليات التشفير
                //      - تأكد_نوع/تأكد_مساواة/تأكد_أكبر: تأكيدات متقدمة
                //      - نظّف: تنظيف المدخلات من HTML
                //      - وقت_الآن/عشوائي_آمن/ترميز_64: أدوات مساعدة
                // ========================================================================

                // 1. تأكد / assert - يتحقق من شرط ويوقف البرنامج إذا كان خاطئاً
                if (funcName == "تأكد" || funcName == "assert" || funcName == "تاكد")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[خطأ] دالة تأكد تتطلب معامل واحد على الأقل (الشرط)" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_ASSERT);
                    inst.operands.push_back(argOperands[0]); // condition
                    if (argOperands.size() > 1)
                    {
                        inst.operands.push_back(argOperands[1]); // optional message
                    }
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // 2. تحقق / verify - يعيد صحيح أو خطأ دون إيقاف البرنامج
                if (funcName == "تحقق" || funcName == "verify")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[خطأ] دالة تحقق تتطلب معامل واحد (الشرط)" << std::endl;
                        return BuildResult("", SadTypeKind::Boolean);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_VERIFY);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // 3. آمن / is_safe - يتحقق من أمان القيمة
                if (funcName == "آمن" || funcName == "is_safe" || funcName == "امن")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[خطأ] دالة آمن تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Boolean);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_IS_SAFE);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // 4. ذعر / panic - إيقاف طارئ مع رسالة خطأ
                if (funcName == "ذعر" || funcName == "panic")
                {
                    SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_PANIC);
                    if (!argOperands.empty())
                    {
                        inst.operands.push_back(argOperands[0]); // message
                    }
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // 5. هاش / hash - حساب هاش FNV-1a للنص
                if (funcName == "هاش" || funcName == "hash")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[خطأ] دالة هاش تتطلب معامل واحد (النص)" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_HASH);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 6. شفّر / encrypt - تشفير XOR
                if (funcName == "شفّر" || funcName == "شفر" || funcName == "encrypt")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] دالة شفّر تتطلب معاملين (النص، المفتاح)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_ENCRYPT);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 7. فك_تشفير / decrypt - فك تشفير XOR
                if (funcName == "فك_تشفير" || funcName == "decrypt")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] دالة فك_تشفير تتطلب معاملين (النص، المفتاح)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_DECRYPT);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 8. تأكد_نوع / assert_type - التحقق من نوع القيمة
                if (funcName == "تأكد_نوع" || funcName == "assert_type" || funcName == "تاكد_نوع")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] دالة تأكد_نوع تتطلب معاملين (القيمة، النوع_المتوقع)" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_ASSERT_TYPE);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // 9. تأكد_مساواة / assert_equal - التحقق من تساوي قيمتين
                if (funcName == "تأكد_مساواة" || funcName == "assert_equal" || funcName == "تاكد_مساواة")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] دالة تأكد_مساواة تتطلب معاملين" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_ASSERT_EQUAL);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // 10. تأكد_أكبر / assert_greater - التحقق من أن القيمة الأولى أكبر
                if (funcName == "تأكد_أكبر" || funcName == "assert_greater" || funcName == "تاكد_اكبر")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] دالة تأكد_أكبر تتطلب معاملين" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_ASSERT_GREATER);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // 11. نظّف / sanitize - تنظيف نص من HTML
                if (funcName == "نظّف" || funcName == "نظف" || funcName == "sanitize")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[خطأ] دالة نظّف تتطلب معامل واحد (النص)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_SANITIZE);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 12. وقت_الآن / الآن / timestamp - الحصول على الوقت الحالي
                // (AR) يدعم: وقت_الآن، الآن، الان، timestamp، now
                // (EN) Supports: وقت_الآن, الآن, الان, timestamp, now
                if (funcName == "وقت_الآن" || funcName == "وقت_الان" ||
                    funcName == "الآن" || funcName == "الان" ||
                    funcName == "timestamp" || funcName == "now")
                {
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_TIMESTAMP);
                    inst.result = resultOp;
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 13. عشوائي_آمن / secure_random - رقم عشوائي آمن
                if (funcName == "عشوائي_آمن" || funcName == "عشوائي_امن" || funcName == "secure_random")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] دالة عشوائي_آمن تتطلب معاملين (الحد_الأدنى، الحد_الأقصى)" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_SECURE_RANDOM);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 14. ترميز_64 / base64_encode - ترميز Base64
                if (funcName == "ترميز_64" || funcName == "base64_encode" || funcName == "base64")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[خطأ] دالة ترميز_64 تتطلب معامل واحد (النص)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_BASE64_ENCODE);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad