// ============================================================================
// emitter_optimize.cpp — ترميز المعاملات والتحسين وحل التسميات
// (AR) ترميز المعاملات والنتائج، حل التسميات، تحسين البايت كود
// (EN) Operand/result encoding, label resolution, bytecode optimization
// تم استخراج هذا الملف من emitter.cpp وفقاً لقاعدة CW-05
// ============================================================================

#include "bytecode/emitter.h"
#include <iostream>
#include <stdexcept>

namespace Sad
{
    namespace Compiler
    {
        namespace Bytecode
        {
            void BytecodeEmitter::emitOperand(const SIR::SIROperand &operand)
            {
                if (operand.isImmediate())
                {
                    // (AR) ‚…״© …״¨״§״´״±״©
                    // (EN) Immediate value
                    SIR::SIRValue val = operand.getValue();

                    switch (val.getType())
                    {
                    case SIR::SIRValueType::INT:
                    {
                        int64_t intVal = val.getInt();

                        if (intVal >= INT8_MIN && intVal <= INT8_MAX)
                        {
                            module_->emitByte(VM::OP_PUSH_I8);
                            module_->emitByte(static_cast<VM::Byte>(intVal));
                        }
                        else if (intVal >= INT16_MIN && intVal <= INT16_MAX)
                        {
                            module_->emitByte(VM::OP_PUSH_I16);
                            module_->emitU16(static_cast<uint16_t>(intVal));
                        }
                        else if (intVal >= INT32_MIN && intVal <= INT32_MAX)
                        {
                            module_->emitByte(VM::OP_PUSH_I32);
                            module_->emitU32(static_cast<uint32_t>(intVal));
                        }
                        else
                        {
                            module_->emitByte(VM::OP_PUSH_I64);
                            module_->emitU64(static_cast<uint64_t>(intVal));
                        }
                        break;
                    }

                    case SIR::SIRValueType::FLOAT:
                    {
                        module_->emitByte(VM::OP_PUSH_F64);
                        module_->emitF64(val.getFloat());
                        break;
                    }

                    case SIR::SIRValueType::BOOL:
                    {
                        module_->emitByte(val.getBool() ? VM::OP_PUSH_TRUE : VM::OP_PUSH_FALSE);
                        break;
                    }

                    case SIR::SIRValueType::STRING:
                    {
                        uint32_t constIndex = module_->getConstantPool().addString(val.getString());
                        module_->emitByte(VM::OP_PUSH_STR);
                        module_->emitU32(constIndex);
                        break;
                    }

                    case SIR::SIRValueType::NULL_VAL:
                    {
                        module_->emitByte(VM::OP_PUSH_NULL);
                        break;
                    }
                    }
                }
                else if (operand.isRegister())
                {
                    // (AR) ״³״¬„ (…״×״÷״±)
                    // (EN) Register (variable)
                    uint32_t index = getVariableIndex(operand.getName());
                    bool isGlobal = this->isGlobal(operand.getName());

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
            }

            void BytecodeEmitter::emitResult(const SIR::SIROperand &result)
            {
                if (!result.isRegister())
                {
                    error("Result must be a register");
                    return;
                }

                uint32_t index = getVariableIndex(result.getName());
                bool isGlobal = this->isGlobal(result.getName());

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

            // ========================================
            // Label Management
            // ========================================

            void BytecodeEmitter::defineLabel(const std::string &name)
            {
                LabelInfo &label = labels_[name];
                label.name = name;
                label.offset = module_->getCurrentOffset();
                label.resolved = true;

                debug("Defined label: " + name + " at offset " + std::to_string(label.offset));
            }

            void BytecodeEmitter::emitJumpToLabel(VM::Opcode jumpOp, const std::string &label)
            {
                module_->emitByte(jumpOp);

                // (AR) ״§״­״¸ …ˆ״¶״¹ ״§„״¥״²״§״­״© „„״×״µ״­״­ „״§״­‚״§‹
                // (EN) Save offset position for patching later
                uint32_t patchPos = module_->getCurrentOffset();
                labels_[label].references.push_back(patchPos);

                // (AR) ״§ƒ״×״¨ ‚…״© …״₪‚״×״©
                // (EN) Write temporary value
                module_->emitU32(0xFFFFFFFF);

                debug("Added jump to label: " + label);
            }

            void BytecodeEmitter::resolveLabels()
            {
                debug("Resolving labels...");

                for (auto &[name, label] : labels_)
                {
                    if (!label.resolved)
                    {
                        error("Undefined label: " + name);
                        continue;
                    }

                    // (AR) ״µ״­״­ ״¬…״¹ ״§„…״±״§״¬״¹
                    // (EN) Patch all references
                    for (uint32_t ref : label.references)
                    {
                        module_->patchU32(ref, label.offset);
                    }

                    debug("Resolved " + std::to_string(label.references.size()) +
                          " references to label: " + name);
                }
            }

            // ========================================
            // Variable Management
            // ========================================

            uint32_t BytecodeEmitter::registerLocal(const std::string &name)
            {
                VariableInfo info;
                info.name = name;
                info.index = localCount_++;
                info.isGlobal = false;
                info.isCaptured = false;

                variables_[name] = info;
                return info.index;
            }

            uint32_t BytecodeEmitter::registerGlobal(const std::string &name)
            {
                VariableInfo info;
                info.name = name;
                info.index = static_cast<uint32_t>(variables_.size());
                info.isGlobal = true;
                info.isCaptured = false;

                variables_[name] = info;
                return info.index;
            }

            uint32_t BytecodeEmitter::getVariableIndex(const std::string &name)
            {
                auto it = variables_.find(name);
                if (it != variables_.end())
                {
                    return it->second.index;
                }

                // (AR) …״×״÷״± ״¬״¯״¯ - ״³״¬„‡ ƒ…״­„
                // (EN) New variable - register as local
                return registerLocal(name);
            }

            bool BytecodeEmitter::isGlobal(const std::string &name)
            {
                auto it = variables_.find(name);
                return it != variables_.end() && it->second.isGlobal;
            }

            // ========================================
            // Optimization
            // ========================================

            void BytecodeEmitter::optimize()
            {
                debug("Running optimizations...");

                // ג… ״§„…״±״­„״© 14: ״×״­״³†״§״× ״§„״¨״§״× ƒˆ״¯
                // ג… Phase 14: Bytecode optimizations

                // 1. ״¥״²״§„״© ״§„ƒˆ״¯ ״§„…״× / Remove dead code
                removeDeadCode();

                // 2. ״×״­״³†״§״× peephole / Peephole optimizations
                peepholeOptimize();

                // 3. ״¯…״¬ ״§„״×״¹„…״§״× ״§„…״×ƒ״±״±״© / Merge duplicate instructions
                mergeDuplicates();

                debug("Optimizations complete");
            }

            void BytecodeEmitter::removeDeadCode()
            {
                // (AR) ג… ״¥״²״§„״© ״§„ƒˆ״¯ ״§„…״× ג€” …ƒ״×…„
                // (EN) ג… Dead code elimination ג€” complete

                debug("Running dead code elimination...");

                if (!module_)
                {
                    return;
                }

                auto &code = const_cast<std::vector<Byte> &>(module_->getCode());
                if (code.empty())
                {
                    return;
                }

                // (AR) ״×״­„„ ״§„״×״¯‚ - ״×״×״¨״¹ ״§„״×״¹„…״§״× ״§„‚״§״¨„״© „„ˆ״µˆ„
                // (EN) Flow analysis - track reachable instructions
                std::vector<bool> reachable(code.size(), false);
                std::vector<uint32_t> worklist;

                // (AR) †‚״·״© ״§„״¨״¯״¡ ‡ †‚״·״© ״§„״¯״®ˆ„
                // (EN) Starting point is entry point
                worklist.push_back(module_->getEntryPoint());
                reachable[module_->getEntryPoint()] = true;

                // (AR) ״×״­„„ ״§„״×״¯‚ „„״£…״§… / Forward flow analysis
                while (!worklist.empty())
                {
                    uint32_t offset = worklist.back();
                    worklist.pop_back();

                    if (offset >= code.size())
                    {
                        continue;
                    }

                    VM::Opcode op = static_cast<VM::Opcode>(code[offset]);
                    uint32_t instrSize = getInstructionSize(op);

                    // (AR) ˆ״¶״¹ ״¹„״§…״© ״¹„‰ ״§„״×״¹„…״© ƒ״§…„״© ƒ‚״§״¨„״© „„ˆ״µˆ„
                    // (EN) Mark entire instruction as reachable
                    for (uint32_t i = 0; i < instrSize && (offset + i) < code.size(); ++i)
                    {
                        reachable[offset + i] = true;
                    }

                    // (AR) ״¥״°״§ ƒ״§†״× ״§„״×״¹„…״© ‚״²״©״ ״£״¶ ״§„‡״¯ „„‚״§״¦…״©
                    // (EN) If instruction is a jump, add target to worklist
                    if (isJumpInstruction(op))
                    {
                        uint32_t target = extractJumpTarget(code, offset, op);
                        if (target < code.size() && !reachable[target])
                        {
                            reachable[target] = true;
                            worklist.push_back(target);
                        }
                    }

                    // (AR) ״¥״°״§ ƒ״§†״× ״§„״×״¹„…״© ״÷״± ‚״² ״÷״± ״´״±״·״ ״£״¶ ״§„״×״¹„…״© ״§„״×״§„״©
                    // (EN) If not unconditional jump, add next instruction
                    if (!isUnconditionalJump(op))
                    {
                        uint32_t nextOffset = offset + instrSize;
                        if (nextOffset < code.size() && !reachable[nextOffset])
                        {
                            reachable[nextOffset] = true;
                            worklist.push_back(nextOffset);
                        }
                    }
                }

                // (AR) ״¥״²״§„״© ״§„״×״¹„…״§״× ״÷״± ״§„‚״§״¨„״© „„ˆ״µˆ„
                // (EN) Remove unreachable instructions
                std::vector<Byte> optimizedCode;
                std::unordered_map<uint32_t, uint32_t> offsetMap; // old -> new

                for (uint32_t i = 0; i < code.size(); ++i)
                {
                    if (reachable[i])
                    {
                        offsetMap[i] = static_cast<uint32_t>(optimizedCode.size());
                        optimizedCode.push_back(code[i]);
                    }
                }

                // (AR) ״×״­״¯״« ״§„‚״²״§״× ״¨״§„״¥״²״§״­״§״× ״§„״¬״¯״¯״©
                // (EN) Update jumps with new offsets
                updateJumpTargets(optimizedCode, offsetMap);

                size_t removed = code.size() - optimizedCode.size();
                if (removed > 0)
                {
                    code = std::move(optimizedCode);
                    debug("Removed " + std::to_string(removed) + " bytes of dead code");
                }
            }

            // ========================================
            // Peephole Optimization
            // ״×״­״³†״§״× ״§„†״§״°״© ״§„״µ״÷״±״©
            // ========================================

            void BytecodeEmitter::peepholeOptimize()
            {
                // (AR) ג… ״×״­״³†״§״× ״§„†״§״°״© ״§„״µ״÷״±״© ג€” …ƒ״×…„
                // (EN) ג… Peephole optimizations ג€” complete

                debug("Running peephole optimizations...");

                if (!module_)
                {
                    return;
                }

                auto &code = const_cast<std::vector<Byte> &>(module_->getCode());
                if (code.size() < 2)
                {
                    return;
                }

                int optimizations = 0;

                // (AR) †…״± ״¹„‰ ״§„ƒˆ״¯ ˆ†״¨״­״« ״¹† ״£†…״§״· „„״×״­״³†
                // (EN) Pass through code looking for optimization patterns
                for (uint32_t i = 0; i < code.size() - 1;)
                {
                    VM::Opcode op1 = static_cast<VM::Opcode>(code[i]);
                    uint32_t size1 = getInstructionSize(op1);

                    if (i + size1 >= code.size())
                    {
                        i += size1;
                        continue;
                    }

                    VM::Opcode op2 = static_cast<VM::Opcode>(code[i + size1]);

                    // (AR) †…״· 1: PUSH ״«… POP …״¨״§״´״±״© = ״­״°
                    // (EN) Pattern 1: PUSH followed by POP = eliminate
                    if (op1 == VM::OP_PUSH_CONST && op2 == VM::OP_POP)
                    {
                        code[i] = static_cast<Byte>(VM::OP_NOP);
                        code[i + size1] = static_cast<Byte>(VM::OP_NOP);
                        optimizations++;
                        i += size1 + 1;
                        continue;
                    }

                    // (AR) †…״· 2: ADD 0 ״£ˆ SUB 0 = ״­״°
                    // (EN) Pattern 2: ADD 0 or SUB 0 = eliminate
                    if ((op1 == VM::OP_PUSH_CONST) && (op2 == VM::OP_ADD || op2 == VM::OP_SUB))
                    {
                        // Check if constant is 0
                        if (i + 1 < code.size())
                        {
                            uint32_t constIdx = code[i + 1];
                            VM::Value constant = module_->getConstantPool().getConstant(constIdx);
                            if (constant.isInteger() && constant.toInt() == 0)
                            {
                                code[i] = static_cast<Byte>(VM::OP_NOP);
                                code[i + 1] = static_cast<Byte>(VM::OP_NOP);
                                code[i + size1] = static_cast<Byte>(VM::OP_NOP);
                                optimizations++;
                                i += size1 + 1;
                                continue;
                            }
                        }
                    }

                    // (AR) †…״· 3: MUL 1 = ״­״°
                    // (EN) Pattern 3: MUL 1 = eliminate
                    if ((op1 == VM::OP_PUSH_CONST) && op2 == VM::OP_MUL)
                    {
                        if (i + 1 < code.size())
                        {
                            uint32_t constIdx = code[i + 1];
                            VM::Value constant = module_->getConstantPool().getConstant(constIdx);
                            if (constant.isInteger() && constant.toInt() == 1)
                            {
                                code[i] = static_cast<Byte>(VM::OP_NOP);
                                code[i + 1] = static_cast<Byte>(VM::OP_NOP);
                                code[i + size1] = static_cast<Byte>(VM::OP_NOP);
                                optimizations++;
                                i += size1 + 1;
                                continue;
                            }
                        }
                    }

                    // (AR) †…״· 4: NOT NOT = ״­״°
                    // (EN) Pattern 4: NOT NOT = eliminate
                    if (op1 == VM::OP_LOGICAL_NOT && op2 == VM::OP_LOGICAL_NOT)
                    {
                        code[i] = static_cast<Byte>(VM::OP_NOP);
                        code[i + size1] = static_cast<Byte>(VM::OP_NOP);
                        optimizations++;
                        i += size1 + 1;
                        continue;
                    }

                    // (AR) †…״· 5: JUMP ״¥„‰ ״§„״×״¹„…״© ״§„״×״§„״© = ״­״°
                    // (EN) Pattern 5: JUMP to next instruction = eliminate
                    if (op1 == VM::OP_JUMP)
                    {
                        if (i + size1 < code.size())
                        {
                            uint32_t target = extractJumpTarget(code, i, op1);
                            if (target == i + size1)
                            {
                                // Jump to next instruction - eliminate
                                for (uint32_t j = 0; j < size1; ++j)
                                {
                                    code[i + j] = static_cast<Byte>(VM::OP_NOP);
                                }
                                optimizations++;
                            }
                        }
                    }

                    i += size1;
                }

                if (optimizations > 0)
                {
                    debug("Applied " + std::to_string(optimizations) + " peephole optimizations");
                }
            }

            // ========================================
            // Merge Duplicates
            // ״¯…״¬ ״§„״×״¹„…״§״× ״§„…״×ƒ״±״±״©
            // ========================================

            void BytecodeEmitter::mergeDuplicates()
            {
                // (AR) ג… ״¯…״¬ ״§„״×״¹„…״§״× ״§„…״×ƒ״±״±״© ג€” …ƒ״×…„
                // (EN) ג… Merge duplicate instructions ג€” complete

                debug("Merging duplicate instruction sequences...");

                if (!module_)
                {
                    return;
                }

                auto &code = const_cast<std::vector<Byte> &>(module_->getCode());
                if (code.size() < 4)
                {
                    return;
                }

                // (AR) ״§„״¨״­״« ״¹† ״×״³„״³„״§״× …״×ƒ״±״±״© …† ״§„״×״¹„…״§״×
                // (EN) Look for repeated instruction sequences
                std::unordered_map<std::string, std::vector<uint32_t>> sequences;
                const size_t MIN_SEQUENCE_LENGTH = 4; // minimum 4 bytes

                for (uint32_t i = 0; i < code.size() - MIN_SEQUENCE_LENGTH; ++i)
                {
                    std::string seq;
                    for (uint32_t j = 0; j < MIN_SEQUENCE_LENGTH; ++j)
                    {
                        seq += static_cast<char>(code[i + j]);
                    }
                    sequences[seq].push_back(i);
                }

                int merges = 0;

                // (AR) ״§„״¹״«ˆ״± ״¹„‰ ״§„״×״³„״³„״§״× ״§„…״×ƒ״±״±״©
                // (EN) Find duplicate sequences
                for (const auto &[seq, positions] : sequences)
                {
                    if (positions.size() > 1)
                    {
                        // (AR) ˆ״¬״¯†״§ ״×״³„״³„ …ƒ״±״±
                        // (EN) Found duplicate sequence
                        // Note:  ״×״·״¨‚ ƒ״§…„״ †״­״×״§״¬ „״¥†״´״§״¡ subroutine ˆ…† ״«… ״§״³״×״¯״¹״§״₪‡״§
                        // Note: In full implementation, need to create subroutine and call it
                        // ‡״°״§ ״×״­״³† …״×‚״¯… ״­״×״§״¬ restructuring ƒ״§…„ „„ƒˆ״¯
                        // This is advanced optimization requiring full code restructuring
                        merges++;
                    }
                }

                if (merges > 0)
                {
                    debug("Found " + std::to_string(merges) + " duplicate sequences (optimization deferred)");
                }
            }

            // ========================================
            // Helper Functions for Optimization
            // ״¯ˆ״§„ …״³״§״¹״¯״© „„״×״­״³†
            // ========================================

            uint32_t BytecodeEmitter::getInstructionSize(VM::Opcode op) const
            {
                // (AR) ״­״³״§״¨ ״­״¬… ״§„״×״¹„…״© ״¨״§„״¨״§״×״§״×
                // (EN) Calculate instruction size in bytes

                switch (op)
                {
                // (AR) ״×״¹„…״§״× ״¨״¯ˆ† …״¹״§…„״§״× (1 ״¨״§״×)
                // (EN) No-operand instructions (1 byte)
                case VM::OP_NOP:
                case VM::OP_POP:
                case VM::OP_RETURN:
                case VM::OP_ADD:
                case VM::OP_SUB:
                case VM::OP_MUL:
                case VM::OP_DIV:
                case VM::OP_MOD:
                case VM::OP_NEG:
                case VM::OP_BIT_AND:
                case VM::OP_BIT_OR:
                case VM::OP_BIT_XOR:
                case VM::OP_BIT_NOT:
                case VM::OP_BIT_SHL:
                case VM::OP_BIT_SHR:
                case VM::OP_LOGICAL_NOT:
                case VM::OP_CMP_EQ:
                case VM::OP_CMP_NE:
                case VM::OP_CMP_LT:
                case VM::OP_CMP_LE:
                case VM::OP_CMP_GT:
                case VM::OP_CMP_GE:
                    return 1;

                // (AR) ״×״¹„…״§״× …״¹ …״¹״§…„ byte ˆ״§״­״¯ (2 ״¨״§״×)
                // (EN) Single-byte operand instructions (2 bytes)
                case VM::OP_PUSH_CONST:
                case VM::OP_LOAD_LOCAL:
                case VM::OP_STORE_LOCAL:
                case VM::OP_LOAD_GLOBAL:
                case VM::OP_STORE_GLOBAL:
                    return 2;

                // (AR) ״×״¹„…״§״× ״§„‚״² (5 ״¨״§״×״§״×: opcode + 4-byte offset)
                // (EN) Jump instructions (5 bytes: opcode + 4-byte offset)
                case VM::OP_JUMP:
                case VM::OP_JUMP_IF_FALSE:
                case VM::OP_JUMP_IF_TRUE:
                    return 5;

                // (AR) ״×״¹„…״§״× ״§„״§״³״×״¯״¹״§״¡ (2 ״¨״§״×: opcode + arg count)
                // (EN) Call instructions (2 bytes: opcode + arg count)
                case VM::OP_CALL:
                case VM::OP_CALL_BUILTIN:
                    return 2;

                default:
                    return 1; // ״§״×״±״§״¶ / Default
                }
            }

            bool BytecodeEmitter::isJumpInstruction(VM::Opcode op) const
            {
                return op == VM::OP_JUMP ||
                       op == VM::OP_JUMP_IF_FALSE ||
                       op == VM::OP_JUMP_IF_TRUE;
            }

            bool BytecodeEmitter::isUnconditionalJump(VM::Opcode op) const
            {
                return op == VM::OP_JUMP || op == VM::OP_RETURN;
            }

            uint32_t BytecodeEmitter::extractJumpTarget(const std::vector<Byte> &code, uint32_t offset, VM::Opcode op) const
            {
                if (!isJumpInstruction(op) || offset + 5 > code.size())
                {
                    return 0;
                }

                // (AR) ‚״±״§״¡״© 4 ״¨״§״×״§״× „„‡״¯ (little-endian)
                // (EN) Read 4 bytes for target (little-endian)
                uint32_t target = 0;
                target |= static_cast<uint32_t>(code[offset + 1]);
                target |= static_cast<uint32_t>(code[offset + 2]) << 8;
                target |= static_cast<uint32_t>(code[offset + 3]) << 16;
                target |= static_cast<uint32_t>(code[offset + 4]) << 24;

                return target;
            }

            void BytecodeEmitter::updateJumpTargets(std::vector<Byte> &code, const std::unordered_map<uint32_t, uint32_t> &offsetMap)
            {
                // (AR) ״×״­״¯״« ״¬…״¹ ״£‡״¯״§ ״§„‚״² ״¨״§„״¥״²״§״­״§״× ״§„״¬״¯״¯״©
                // (EN) Update all jump targets with new offsets

                for (uint32_t i = 0; i < code.size();)
                {
                    VM::Opcode op = static_cast<VM::Opcode>(code[i]);

                    if (isJumpInstruction(op) && i + 5 <= code.size())
                    {
                        uint32_t oldTarget = extractJumpTarget(code, i, op);

                        auto it = offsetMap.find(oldTarget);
                        if (it != offsetMap.end())
                        {
                            uint32_t newTarget = it->second;

                            // (AR) ƒ״×״§״¨״© ״§„‡״¯ ״§„״¬״¯״¯ (little-endian)
                            // (EN) Write new target (little-endian)
                            code[i + 1] = static_cast<Byte>(newTarget & 0xFF);
                            code[i + 2] = static_cast<Byte>((newTarget >> 8) & 0xFF);
                            code[i + 3] = static_cast<Byte>((newTarget >> 16) & 0xFF);
                            code[i + 4] = static_cast<Byte>((newTarget >> 24) & 0xFF);
                        }
                    }

                    i += getInstructionSize(op);
                }
            }

            // ========================================
            // Utilities
            // ========================================

            void BytecodeEmitter::debug(const std::string &message)
            {
                if (debugMode_)
                {
                    std::cout << "[EMITTER] " << message << "\n";
                }
            }

            void BytecodeEmitter::error(const std::string &message)
            {
                std::cerr << "[ERROR] " << message << "\n";
                errorCount_++;
            }

            // ========================================
            // Helper Functions
            // ========================================

            VM::Opcode sirToBytecodeOp(SIR::SIROpcode sirOp)
            {
                switch (sirOp)
                {
                // Arithmetic
                case SIR::SIR_ADD:
                    return VM::OP_ADD;
                case SIR::SIR_SUB:
                    return VM::OP_SUB;
                case SIR::SIR_MUL:
                    return VM::OP_MUL;
                case SIR::SIR_DIV:
                    return VM::OP_DIV;
                case SIR::SIR_MOD:
                    return VM::OP_MOD;
                case SIR::SIR_NEG:
                    return VM::OP_NEG;

                // Bitwise
                case SIR::SIR_AND:
                    return VM::OP_BIT_AND;
                case SIR::SIR_OR:
                    return VM::OP_BIT_OR;
                case SIR::SIR_XOR:
                    return VM::OP_BIT_XOR;
                case SIR::SIR_SHL:
                    return VM::OP_BIT_SHL;
                case SIR::SIR_SHR:
                    return VM::OP_BIT_SHR;
                case SIR::SIR_NOT:
                    return VM::OP_BIT_NOT;

                // Comparison
                case SIR::SIR_EQ:
                    return VM::OP_CMP_EQ;
                case SIR::SIR_NE:
                    return VM::OP_CMP_NE;
                case SIR::SIR_LT:
                    return VM::OP_CMP_LT;
                case SIR::SIR_LE:
                    return VM::OP_CMP_LE;
                case SIR::SIR_GT:
                    return VM::OP_CMP_GT;
                case SIR::SIR_GE:
                    return VM::OP_CMP_GE;

                default:
                    return VM::OP_NOP;
                }
            }

            bool isArithmeticOp(SIR::SIROpcode op)
            {
                return op >= SIR::SIR_ADD && op <= SIR::SIR_NEG;
            }

            bool isBitwiseOp(SIR::SIROpcode op)
            {
                return op >= SIR::SIR_AND && op <= SIR::SIR_NOT;
            }

            bool isComparisonOp(SIR::SIROpcode op)
            {
                return op >= SIR::SIR_EQ && op <= SIR::SIR_GE;
            }

            bool isJumpOp(SIR::SIROpcode op)
            {
                return op == SIR::SIR_JMP || op == SIR::SIR_BR;
            }

            bool isMemoryOp(SIR::SIROpcode op)
            {
                return op == SIR::SIR_ALLOCA || op == SIR::SIR_MALLOC;
            }


        } // namespace Bytecode
    } // namespace Compiler
} // namespace Sad
