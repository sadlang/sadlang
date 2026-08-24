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
#include "builders/builtin_arity_check.h"
namespace Bn = Sad::Builtins::Names;
// (AR) رتبةُ المدمجِ من حقلِ `arity` في مصدرِ الحقيقةِ — ثابتٌ مُولَّدٌ لا رقمٌ
//      يُكتَب عند الفحص.
namespace Ar = Sad::Builtins::Arity;

// (AR) كانت ههنا اثنتا عشرةَ رسالةَ خطأٍ لعددِ الوسائطِ، كلُّ واحدةٍ نصٌّ عربيٌّ
//      مكتوبٌ باليدِ يصفُ الرتبةَ وصفًا («تتطلب ثلاثة معاملات …») — أي **نسخةٌ
//      ثانيةٌ من العددِ منثورةً**، لا يقيسُها أحدٌ ولا تحمرُّ إن خالفت العقد.
//      صار العددُ يُقرأ من `Ar::Crypto::*`، والرسالةُ تُبنى من كتالوجِ الأخطاء،
//      فماتت الثوابتُ كلُّها ولم يبقَ لها موضعُ نداءٍ واحد.

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
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::Crypto::BLAKE3_HASH, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
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
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::Crypto::BLAKE3_KEYED_HASH, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
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
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::Crypto::KDF_PBKDF2, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
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
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::Crypto::KDF_HKDF, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
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
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::Crypto::AEAD_ENCRYPT, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
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
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::Crypto::AEAD_DECRYPT, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
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
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::Crypto::KDF_ARGON2ID, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
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
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::Crypto::X25519_DERIVE_PUB, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
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
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::Crypto::X25519_EXCHANGE, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
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
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::Crypto::ED25519_DERIVE_PUB, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
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
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::Crypto::ED25519_SIGN, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
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
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::Crypto::ED25519_VERIFY, argResults.size()))
                        return BuildResult("", SadTypeKind::Boolean);
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
