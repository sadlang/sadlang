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

                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
