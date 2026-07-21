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

                // 5. شفّر_موثّق / aead_encrypt - ChaCha20-Poly1305 AEAD (RFC 8439)
                if (funcName == Bn::Crypto::AEAD_ENCRYPT)
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] دالة شفّر_موثّق تتطلب معاملين (النص، المفتاح)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_CRYPTO_AEAD_ENCRYPT);
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

                // 6. فك_تشفير_موثّق / aead_decrypt - ChaCha20-Poly1305 AEAD (fail-closed)
                if (funcName == Bn::Crypto::AEAD_DECRYPT)
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] دالة فك_تشفير_موثّق تتطلب معاملين (المغلّف، المفتاح)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_CRYPTO_AEAD_DECRYPT);
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

                // 7. أرجون2 / argon2id - Argon2id (RFC 9106)
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

                // 6. ولّد_مفتاح_خاص_x25519 / X25519 private key gen (0 args)
                if (funcName == Bn::Crypto::X25519_KEYGEN_PRIV)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_CRYPTO_X25519_KEYGEN_PRIV);
                    inst.result = resultOp;
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 7. اشتق_مفتاح_عام_x25519 / X25519 derive public (1 arg)
                if (funcName == Bn::Crypto::X25519_DERIVE_PUB)
                {
                    if (argResults.size() < 1)
                    {
                        std::cerr << "[خطأ] دالة اشتق_مفتاح_عام_x25519 تتطلب معاملاً واحداً (المفتاح الخاصّ)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_CRYPTO_X25519_DERIVE_PUB);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 7. تبادل_مفتاح / X25519 exchange (2 args)
                if (funcName == Bn::Crypto::X25519_EXCHANGE)
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] دالة تبادل_مفتاح تتطلب معاملين (المفتاح الخاصّ، المفتاح العامّ للطرف الآخر)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_CRYPTO_X25519_EXCHANGE);
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

                // 8. ولّد_مفتاح_خاص_توقيع / Ed25519 seed gen (0 args)
                if (funcName == Bn::Crypto::ED25519_KEYGEN_PRIV)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_CRYPTO_ED25519_KEYGEN_PRIV);
                    inst.result = resultOp;
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 9. اشتق_مفتاح_عام_توقيع / Ed25519 derive public (1 arg)
                if (funcName == Bn::Crypto::ED25519_DERIVE_PUB)
                {
                    if (argResults.size() < 1)
                    {
                        std::cerr << "[خطأ] دالة اشتق_مفتاح_عام_توقيع تتطلب معاملاً واحداً (المفتاح الخاصّ)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_CRYPTO_ED25519_DERIVE_PUB);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 10. وقّع / Ed25519 sign (2 args)
                if (funcName == Bn::Crypto::ED25519_SIGN)
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] دالة وقّع تتطلب معاملين (الرسالة، المفتاح الخاصّ)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_CRYPTO_ED25519_SIGN);
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

                // 11. تحقق_توقيع / Ed25519 verify (3 args) — returns Boolean
                if (funcName == Bn::Crypto::ED25519_VERIFY)
                {
                    if (argResults.size() < 3)
                    {
                        std::cerr << "[خطأ] دالة تحقق_توقيع تتطلب ثلاثة معاملات (الرسالة، التوقيع، المفتاح العامّ)" << std::endl;
                        return BuildResult("", SadTypeKind::Boolean);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    SIRInstruction inst(SIROpcode::BUILTIN_CRYPTO_ED25519_VERIFY);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    inst.operands.push_back(argOperands[2]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
