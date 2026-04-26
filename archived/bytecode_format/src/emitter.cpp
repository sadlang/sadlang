/**
 * @file emitter.cpp
 * @brief ״×״·״¨‚ …״µ״¯״± ״§„״¨״§״× ƒˆ״¯ / Bytecode Emitter Implementation
 * @brief Complete SIR to bytecode translation logic
 *
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#include "bytecode/emitter.h"
#include <iostream>
#include <stdexcept>

namespace Sad
{
    namespace Compiler
    {
        namespace Bytecode
        {

            // ========================================
            // Constructor
            // ========================================

            BytecodeEmitter::BytecodeEmitter()
                : currentFunction_(nullptr),
                  localCount_(0),
                  tempCounter_(0),
                  optimize_(false),
                  debugMode_(false),
                  errorCount_(0)
            {
            }

            // ========================================
            // Main Emission
            // ========================================

            std::unique_ptr<BytecodeModule> BytecodeEmitter::emit(std::shared_ptr<SIR::SIRModule> module)
            {
                module_ = std::make_unique<BytecodeModule>(module->getName());

                debug("Starting bytecode emission for module: " + module->getName());

                // (AR) ״¥״µ״¯״§״± ״§„…״×״÷״±״§״× ״§„״¹״§…״©
                // (EN) Emit global variables
                for (const auto &global : module->getGlobals())
                {
                    uint32_t index = registerGlobal(global.name);

                    GlobalInfo info;
                    info.name = global.name;
                    info.index = index;
                    info.initialValue = VM::Value::Null();

                    module_->addGlobal(info);
                }

                // (AR) ״¥״µ״¯״§״± ״§„״¯ˆ״§„
                // (EN) Emit functions
                for (auto *func : module->getFunctions())
                {
                    emitFunction(func);
                }

                // (AR) ״­„ ״§„״×״³…״§״×
                // (EN) Resolve labels
                resolveLabels();

                // (AR) ״×״­״³† ״¥״°״§ …״·„ˆ״¨
                // (EN) Optimize if requested
                if (optimize_)
                {
                    optimize();
                }

                debug("Bytecode emission complete");

                if (errorCount_ > 0)
                {
                    error("Emission completed with " + std::to_string(errorCount_) + " errors");
                }

                return std::move(module_);
            }

            void BytecodeEmitter::emitFunction(SIR::SIRFunction *function)
            {
                currentFunction_ = function;
                localCount_ = 0;
                tempCounter_ = 0;
                labels_.clear();
                variables_.clear();
                registerMap_.clear();

                debug("Emitting function: " + function->getName());

                // (AR) ״×״³״¬„ ״§„…״¹״§…„״§״× ƒ…״×״÷״±״§״× …״­„״©
                // (EN) Register parameters as locals
                for (const auto &param : function->getParameters())
                {
                    registerLocal(param.name);
                }

                // (AR) ״­״¸ …ˆ״¶״¹ ״¨״¯״§״© ״§„״¯״§„״©
                // (EN) Save function start position
                uint32_t functionStart = module_->getCurrentOffset();

                // (AR) ״¥״µ״¯״§״± ״§„ƒ״×„
                // (EN) Emit blocks
                for (auto *block : function->getBasicBlocks())
                {
                    emitBlock(block);
                }

                // (AR) ״­״¸ …״¹„ˆ…״§״× ״§„״¯״§„״©
                // (EN) Save function info
                FunctionInfo info;
                info.name = function->getName();
                info.arity = static_cast<uint32_t>(function->getParameters().size());
                info.localCount = localCount_;
                info.codeOffset = functionStart;
                info.codeSize = module_->getCurrentOffset() - functionStart;

                module_->addFunction(info);

                debug("Function complete: " + std::to_string(info.codeSize) + " bytes");
            }

            void BytecodeEmitter::emitBlock(SIR::SIRBasicBlock *block)
            {
                // (AR) ״×״¹״± ״×״³…״© ״§„ƒ״×„״©
                // (EN) Define block label
                // Source: SIRBasicBlock::name is PUBLIC member at sir_instruction.h:355
                defineLabel(block->name);

                // (AR) ״¥״µ״¯״§״± ״§„״×״¹„…״§״×
                // (EN) Emit instructions
                // Source: SIRBasicBlock::instructions is PUBLIC member at sir_instruction.h:356
                for (const auto &inst : block->instructions)
                {
                    // Source: SIRInstruction::lineNumber is PUBLIC member at sir_instruction.h:67
                    if (inst.lineNumber > 0)
                    {
                        module_->addLineInfo(inst.lineNumber);
                    }

                    emitInstruction(&inst);
                }
            }

            void BytecodeEmitter::emitInstruction(SIR::SIRInstruction *inst)
            {
                // Source: SIRInstruction::opcode is PUBLIC member at sir_instruction.h:60
                SIR::SIROpcode op = inst->opcode;

                if (isArithmeticOp(op))
                {
                    emitArithmetic(inst);
                }
                else if (isBitwiseOp(op))
                {
                    emitBitwise(inst);
                }
                else if (isComparisonOp(op))
                {
                    emitComparison(inst);
                }
                else if (isJumpOp(op))
                {
                    emitJump(inst);
                }
                else if (isMemoryOp(op))
                {
                    emitMemory(inst);
                }
                else
                {
                    switch (op)
                    {
                    case SIR::SIR_CALL:
                        emitCall(inst);
                        break;
                    case SIR::SIR_RET:
                        emitReturn(inst);
                        break;
                    case SIR::SIR_LOAD:
                        emitLoad(inst);
                        break;
                    case SIR::SIR_STORE:
                        emitStore(inst);
                        break;
                    case SIR::SIR_ARRAY_NEW:
                    case SIR::SIR_ARRAY_GET:
                    case SIR::SIR_ARRAY_SET:
                        emitArray(inst);
                        break;
                    case SIR::SIR_BUILTIN:
                        emitBuiltin(inst);
                        break;
                    default:
                        error("Unknown SIR opcode: " + std::to_string(static_cast<int>(op)));
                        break;
                    }
                }
            }

            // ========================================
            // Arithmetic Operations
            // ========================================

            void BytecodeEmitter::emitArithmetic(SIR::SIRInstruction *inst)
            {
                auto operands = inst->getOperands();

                if (operands.size() < 2)
                {
                    error("Arithmetic operation requires at least 2 operands");
                    return;
                }

                // (AR) ״§״¯״¹ ״§„…״¹״§…„״§״×
                // (EN) Push operands
                emitOperand(operands[0]);
                emitOperand(operands[1]);

                // (AR) ״¥״µ״¯״§״± ״§„״¹…„״©
                // (EN) Emit operation
                // Source: SIRInstruction::opcode is PUBLIC member at sir_instruction.h:60
                VM::Opcode bytecodeOp = sirToBytecodeOp(inst->opcode);
                module_->emitByte(bytecodeOp);

                // (AR) ״®״²‘† ״§„†״×״¬״©
                // (EN) Store result
                if (inst->hasResult())
                {
                    emitResult(inst->getResult());
                }
            }

            void BytecodeEmitter::emitBitwise(SIR::SIRInstruction *inst)
            {
                // Source: SIRInstruction::operands is PUBLIC member at sir_instruction.h:62
                auto &operands = inst->operands;

                if (operands.size() < 2)
                {
                    error("Bitwise operation requires at least 2 operands");
                    return;
                }

                emitOperand(operands[0]);
                emitOperand(operands[1]);

                // Source: SIRInstruction::opcode is PUBLIC member at sir_instruction.h:60
                VM::Opcode bytecodeOp = sirToBytecodeOp(inst->opcode);
                module_->emitByte(bytecodeOp);

                if (inst->hasResult())
                {
                    emitResult(inst->getResult());
                }
            }

            void BytecodeEmitter::emitComparison(SIR::SIRInstruction *inst)
            {
                // Source: SIRInstruction::operands is PUBLIC member at sir_instruction.h:62
                auto &operands = inst->operands;

                if (operands.size() < 2)
                {
                    error("Comparison requires 2 operands");
                    return;
                }

                emitOperand(operands[0]);
                emitOperand(operands[1]);

                // Source: SIRInstruction::opcode is PUBLIC member at sir_instruction.h:60
                VM::Opcode bytecodeOp = sirToBytecodeOp(inst->opcode);
                module_->emitByte(bytecodeOp);

                if (inst->hasResult())
                {
                    emitResult(inst->getResult());
                }
            }

            // ========================================
            // Control Flow
            // ========================================

            void BytecodeEmitter::emitJump(SIR::SIRInstruction *inst)
            {
                auto operands = inst->getOperands();

                if (operands.empty())
                {
                    error("Jump requires target label");
                    return;
                }

                std::string targetLabel = operands[0].getName();

                // Source: SIRInstruction::opcode is PUBLIC member at sir_instruction.h:60
                switch (inst->opcode)
                {
                case SIR::SIR_JMP:
                    emitJumpToLabel(VM::OP_JMP, targetLabel);
                    break;

                case SIR::SIR_BR:
                {
                    // (AR) ‚״² ״´״±״·
                    // (EN) Conditional branch
                    if (operands.size() < 2)
                    {
                        error("Branch requires condition and target");
                        return;
                    }

                    emitOperand(operands[0]); // ״§„״´״±״· / condition
                    emitJumpToLabel(VM::OP_JMP_IF, operands[1].getName());
                    break;
                }

                default:
                    error("Unknown jump opcode");
                    break;
                }
            }

            void BytecodeEmitter::emitCall(SIR::SIRInstruction *inst)
            {
                auto operands = inst->getOperands();

                if (operands.empty())
                {
                    error("Call requires function name");
                    return;
                }

                // (AR) ״§״¯״¹ ״§„ˆ״³״§״¦״·
                // (EN) Push arguments
                for (size_t i = 1; i < operands.size(); ++i)
                {
                    emitOperand(operands[i]);
                }

                // (AR) ״§״­״µ„ ״¹„‰ ‡״±״³ ״§„״¯״§„״©
                // (EN) Get function index
                std::string funcName = operands[0].getName();
                int funcIndex = module_->getFunctionIndex(funcName);

                // (AR) ״¥״°״§ „… ״×… ״§„״¹״«ˆ״± ״¹„‰ ״§„״¯״§„״©״ ״§״³״×״®״¯… 0 ƒ״§״×״±״§״¶ / (EN) If not found, use 0 as default
                uint32_t finalIndex = (funcIndex >= 0) ? static_cast<uint32_t>(funcIndex) : 0;

                if (funcIndex < 0)
                {
                    error("Function not found: " + funcName);
                }

                // (AR) ״¥״µ״¯״§״± ״§״³״×״¯״¹״§״¡
                // (EN) Emit call
                module_->emitByte(VM::OP_CALL);
                module_->emitU32(finalIndex);
                module_->emitByte(static_cast<VM::Byte>(operands.size() - 1)); // argc

                // (AR) ״®״²‘† ״§„‚…״© ״§„…״¹״§״¯״©
                // (EN) Store return value
                if (inst->hasResult())
                {
                    emitResult(inst->getResult());
                }
                else
                {
                    module_->emitByte(VM::OP_POP); // ״×״¬״§‡„ ״§„‚…״© / discard value
                }
            }

            void BytecodeEmitter::emitReturn(SIR::SIRInstruction *inst)
            {
                auto operands = inst->getOperands();

                if (operands.empty())
                {
                    // (AR) ״¹ˆ״¯״© ״¨״¯ˆ† ‚…״©
                    // (EN) Return without value
                    module_->emitByte(VM::OP_RET);
                }
                else
                {
                    // (AR) ״¹ˆ״¯״© …״¹ ‚…״©
                    // (EN) Return with value
                    emitOperand(operands[0]);
                    module_->emitByte(VM::OP_RET_VAL);
                }
            }

            // ========================================
            // Memory Operations
            // ========================================

            void BytecodeEmitter::emitLoad(SIR::SIRInstruction *inst)
            {
                auto operands = inst->getOperands();

                if (operands.empty())
                {
                    error("Load requires source");
                    return;
                }

                const auto &src = operands[0];

                if (src.isRegister())
                {
                    // (AR) ״×״­…„ …† …״×״÷״±
                    // (EN) Load from variable
                    uint32_t index = getVariableIndex(src.getName());
                    bool isGlobal = this->isGlobal(src.getName());

                    if (isGlobal)
                    {
                        module_->emitByte(VM::OP_GET_GLOBAL);
                        module_->emitU32(index);
                    }
                    else
                    {
                        module_->emitByte(VM::OP_GET_LOCAL);
                        module_->emitU32(index);
                    }
                }
                else if (src.isImmediate())
                {
                    // (AR) ״×״­…„ ‚…״© …״¨״§״´״±״©
                    // (EN) Load immediate value
                    emitOperand(src);
                }

                // (AR) ״®״²‘†  ״§„†״×״¬״©
                // (EN) Store to result
                if (inst->hasResult())
                {
                    emitResult(inst->getResult());
                }
            }

            void BytecodeEmitter::emitStore(SIR::SIRInstruction *inst)
            {
                auto operands = inst->getOperands();

                if (operands.size() < 2)
                {
                    error("Store requires source and destination");
                    return;
                }

                const auto &value = operands[0];
                const auto &dest = operands[1];

                // (AR) ״§״¯״¹ ״§„‚…״©
                // (EN) Push value
                emitOperand(value);

                // (AR) ״®״²‘†  ״§„ˆ״¬‡״©
                // (EN) Store to destination
                if (dest.isRegister())
                {
                    uint32_t index = getVariableIndex(dest.getName());
                    bool isGlobal = this->isGlobal(dest.getName());

                    if (isGlobal)
                    {
                        module_->emitByte(VM::OP_SET_GLOBAL);
                        module_->emitU32(index);
                    }
                    else
                    {
                        module_->emitByte(VM::OP_SET_LOCAL);
                        module_->emitU32(index);
                    }
                }
            }

            void BytecodeEmitter::emitMemory(SIR::SIRInstruction *inst)
            {
                // Source: SIRInstruction::operands is PUBLIC member at sir_instruction.h:62
                auto &operands = inst->operands;

                // Source: SIRInstruction::opcode is PUBLIC member at sir_instruction.h:60
                switch (inst->opcode)
                {
                case SIR::SIR_ALLOCA:
                {
                    // (AR) ״×״®״µ״µ ״°״§ƒ״±״© …״­„״©
                    // (EN) Allocate local memory
                    if (operands.empty())
                    {
                        error("Alloca requires size");
                        return;
                    }

                    emitOperand(operands[0]); // ״§„״­״¬… / size
                    module_->emitByte(VM::OP_MALLOC);

                    if (inst->hasResult())
                    {
                        emitResult(inst->getResult());
                    }
                    break;
                }

                case SIR::SIR_MALLOC:
                {
                    // (AR) ״×״®״µ״µ ״°״§ƒ״±״© ״¯†״§…ƒ״©
                    // (EN) Allocate dynamic memory
                    if (operands.empty())
                    {
                        error("Malloc requires size");
                        return;
                    }

                    emitOperand(operands[0]);
                    module_->emitByte(VM::OP_MALLOC);

                    if (inst->hasResult())
                    {
                        emitResult(inst->getResult());
                    }
                    break;
                }

                default:
                    error("Unknown memory operation");
                    break;
                }
            }

            void BytecodeEmitter::emitArray(SIR::SIRInstruction *inst)
            {
                // Source: SIRInstruction::operands is PUBLIC member at sir_instruction.h:62
                auto &operands = inst->operands;

                // Source: SIRInstruction::opcode is PUBLIC member at sir_instruction.h:60
                switch (inst->opcode)
                {
                case SIR::SIR_ARRAY_NEW:
                {
                    // (AR) ״¥†״´״§״¡ …״µˆ״© ״¬״¯״¯״©
                    // (EN) Create new array
                    if (operands.empty())
                    {
                        error("Array creation requires size");
                        return;
                    }

                    emitOperand(operands[0]); // ״§„״­״¬… / size
                    module_->emitByte(VM::OP_ARRAY_NEW);

                    if (inst->hasResult())
                    {
                        emitResult(inst->getResult());
                    }
                    break;
                }

                case SIR::SIR_ARRAY_GET:
                {
                    // (AR) ״¬„״¨ ״¹†״µ״± …† …״µˆ״©
                    // (EN) Get array element
                    if (operands.size() < 2)
                    {
                        error("Array get requires array and index");
                        return;
                    }

                    emitOperand(operands[0]); // ״§„…״µˆ״© / array
                    emitOperand(operands[1]); // ״§„‡״±״³ / index
                    module_->emitByte(VM::OP_ARRAY_GET);

                    if (inst->hasResult())
                    {
                        emitResult(inst->getResult());
                    }
                    break;
                }

                case SIR::SIR_ARRAY_SET:
                {
                    // (AR) ״×״¹† ״¹†״µ״±  …״µˆ״©
                    // (EN) Set array element
                    if (operands.size() < 3)
                    {
                        error("Array set requires array, index, and value");
                        return;
                    }

                    emitOperand(operands[0]); // ״§„…״µˆ״© / array
                    emitOperand(operands[1]); // ״§„‡״±״³ / index
                    emitOperand(operands[2]); // ״§„‚…״© / value
                    module_->emitByte(VM::OP_ARRAY_SET);
                    break;
                }

                default:
                    error("Unknown array operation");
                    break;
                }
            }

            void BytecodeEmitter::emitBuiltin(SIR::SIRInstruction *inst)
            {
                auto operands = inst->getOperands();

                if (operands.empty())
                {
                    error("Builtin requires function name");
                    return;
                }

                std::string builtinName = operands[0].getName();

                // (AR) ״§״¯״¹ ״§„ˆ״³״§״¦״·
                // (EN) Push arguments
                for (size_t i = 1; i < operands.size(); ++i)
                {
                    emitOperand(operands[i]);
                }

                // (AR) ״§״­״µ„ ״¹„‰ ״±‚… ״§„״¯״§„״© ״§„…״¯…״¬״©
                // (EN) Get builtin function ID
                std::string builtinName = operands[0].getName();
                int builtinId = module_->getBuiltinId(builtinName);

                // (AR) ״¥״°״§ „… ״×… ״§„״¹״«ˆ״±״ ״­״§ˆ„ ״¥״¬״§״¯ ״§„״±‚… …† ״§„״§״³… / (EN) If not found, try to derive ID from name
                uint32_t finalId = 0;
                if (builtinId >= 0)
                {
                    finalId = static_cast<uint32_t>(builtinId);
                }
                else
                {
                    // (AR) ״×״¹† ״±‚… ״§״×״±״§״¶ ״­״³״¨ ״§„״§״³… / (EN) Assign default ID based on name
                    if (builtinName == "print" || builtinName == "״§״·״¨״¹")
                    {
                        finalId = 1;
                    }
                    else if (builtinName == "input" || builtinName == "״¥״¯״®״§„")
                    {
                        finalId = 2;
                    }
                    else if (builtinName == "len" || builtinName == "״·ˆ„")
                    {
                        finalId = 3;
                    }
                    else if (builtinName == "type" || builtinName == "†ˆ״¹")
                    {
                        finalId = 4;
                    }
                    else
                    {
                        error("Unknown builtin function: " + builtinName);
                    }
                }

                module_->emitByte(VM::OP_CALL_NATIVE);
                module_->emitU32(finalId);
                module_->emitByte(static_cast<VM::Byte>(operands.size() - 1));

                if (inst->hasResult())
                {
                    emitResult(inst->getResult());
                }
            }

            // ========================================
            // Operand Handling
            // ========================================


        } // namespace Bytecode
    } // namespace Compiler
} // namespace Sad

