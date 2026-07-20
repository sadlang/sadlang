// ============================================================================
// builtins_crypto.cpp
// (AR) دوال وحدة تشفير: بلايك3 (BLAKE3 hash)، هاش_مفتاح (BLAKE3 keyed hash)
// (EN) Crypto module builtins: BLAKE3 hash, BLAKE3 keyed hash (MAC)
// ============================================================================

#include "sir_builder.h"
#include "builders/builtin_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <optional>

#include "builtin_registry.h"
namespace Bn = Sad::Builtins::Names;

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            std::optional<BuildResult> BuiltinBuilder::buildBuiltinSystem_Crypto(
                const std::string &funcName,
                bool isUserDefinedFunction,
                std::vector<BuildResult> &argResults,
                std::vector<SIROperand> &argOperands)
            {
                (void)isUserDefinedFunction;

                // 1. بلايك3 / blake3 - هاش BLAKE3 (256 بت، ذاتيّ التنفيذ، مطابق للمفسّر)
                if (funcName == Bn::Crypto::BLAKE3_HASH)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[خطأ] دالة بلايك3 تتطلب معامل واحد (النص)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_CRYPTO_BLAKE3_HASH);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 2. هاش_مفتاح / keyed_hash - مصادقة رسالة عبر نمط BLAKE3 المُفتاح
                if (funcName == Bn::Crypto::BLAKE3_KEYED_HASH)
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] دالة هاش_مفتاح تتطلب معاملين (النص، المفتاح)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_CRYPTO_BLAKE3_KEYED_HASH);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 3. اشتق_مفتاح_مرور / derive_password_key - PBKDF2-HMAC-SHA256
                if (funcName == Bn::Crypto::KDF_PBKDF2)
                {
                    if (argResults.size() < 3)
                    {
                        std::cerr << "[خطأ] دالة اشتق_مفتاح_مرور تتطلب ثلاثة معاملات (كلمة_المرور، ملح، عدد_التكرارات)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_CRYPTO_KDF_PBKDF2);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    inst.operands.push_back(argOperands[2]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 4. اشتق_مفتاح / derive_key - HKDF-SHA256
                if (funcName == Bn::Crypto::KDF_HKDF)
                {
                    if (argResults.size() < 4)
                    {
                        std::cerr << "[خطأ] دالة اشتق_مفتاح تتطلب أربعة معاملات (سرّ، ملح، سياق، الطول)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_CRYPTO_KDF_HKDF);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    inst.operands.push_back(argOperands[2]);
                    inst.operands.push_back(argOperands[3]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 5. أرجون2 / argon2id - Argon2id (RFC 9106)
                if (funcName == Bn::Crypto::KDF_ARGON2ID)
                {
                    if (argResults.size() < 4)
                    {
                        std::cerr << "[خطأ] دالة أرجون2 تتطلب أربعة معاملات (كلمة_المرور، ملح، تكلفة_الذاكرة، عدد_التكرارات)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_CRYPTO_KDF_ARGON2ID);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    inst.operands.push_back(argOperands[2]);
                    inst.operands.push_back(argOperands[3]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
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
