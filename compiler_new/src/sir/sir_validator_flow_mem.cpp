/**
 * =============================================================================
 * @file sir_validator_flow_mem.cpp
 * @brief (AR) التحقق من تدفق التحكم وإدارة الذاكرة في SIR
 * @brief (EN) Control flow and memory validation for SIR
 * تم استخراج هذا الملف من sir_validator.cpp وفقاً لقاعدة CW-05
 * =============================================================================
 */

#include <string>
#include <vector>
#include <set>
#include <map>
#include <optional>
#include <variant>
#include <sstream>
#include <stack>
#include <algorithm>
#include "sad_type_system.h"

namespace sad
{
    namespace compiler
    {
        namespace sir
        {

            using Sad::Types::SadTypeKind;

            class ControlFlowValidator
            {
            public:
                struct ValidationResult
                {
                    bool isValid = true;
                    std::vector<std::string> errors;
                    std::vector<std::string> warnings;
                    std::set<std::string> unreachableBlocks;
                    std::set<std::string> deadCodeBlocks;
                };

                /**
                 * @brief التحقق من تدفق التحكم للدالة
                 */
                ValidationResult validate(const SirFunction &function)
                {
                    ValidationResult result;

                    if (function.blocks.empty())
                    {
                        result.errors.push_back("Function has no blocks");
                        result.isValid = false;
                        return result;
                    }

                    // 1. بناء رسم بياني للتحكم
                    std::map<std::string, std::set<std::string>> cfg;
                    std::set<std::string> allBlocks;

                    for (const auto &block : function.blocks)
                    {
                        allBlocks.insert(block.name);
                        cfg[block.name] = std::set<std::string>(
                            block.successors.begin(), block.successors.end());
                    }

                    // 2. البحث عن الكتل الغير قابلة للوصول
                    std::set<std::string> reachable;
                    std::stack<std::string> worklist;
                    worklist.push(function.blocks[0].name);

                    while (!worklist.empty())
                    {
                        std::string current = worklist.top();
                        worklist.pop();

                        if (reachable.count(current))
                            continue;
                        reachable.insert(current);

                        for (const auto &succ : cfg[current])
                        {
                            if (!reachable.count(succ))
                            {
                                worklist.push(succ);
                            }
                        }
                    }

                    for (const auto &block : allBlocks)
                    {
                        if (!reachable.count(block))
                        {
                            result.unreachableBlocks.insert(block);
                            result.warnings.push_back("Unreachable block: " + block);
                        }
                    }

                    // 3. البحث عن الحلقات اللانهائية
                    for (const auto &block : function.blocks)
                    {
                        if (cfg[block.name].count(block.name))
                        {
                            // حلقة ذاتية - قد تكون لانهائية
                            result.warnings.push_back(
                                "Potential infinite loop in block: " + block.name);
                        }
                    }

                    // 4. التحقق من أن جميع المسارات تنتهي
                    for (const auto &block : function.blocks)
                    {
                        if (cfg[block.name].empty())
                        {
                            // كتلة بدون خلفاء - يجب أن تنتهي بـ ret أو unreachable
                            if (!block.instructions.empty())
                            {
                                const auto &lastInst = block.instructions.back();
                                if (lastInst.opcode != SirOpcode::Ret &&
                                    lastInst.opcode != SirOpcode::Unreachable)
                                {
                                    result.errors.push_back(
                                        "Block '" + block.name + "' has no successors but doesn't return");
                                    result.isValid = false;
                                }
                            }
                        }
                    }

                    // 5. التحقق من تناسق PHI
                    for (const auto &block : function.blocks)
                    {
                        for (const auto &inst : block.instructions)
                        {
                            if (inst.opcode == SirOpcode::Phi)
                            {
                                // يجب أن يكون لدى PHI قيمة لكل سلف
                                std::set<std::string> preds(
                                    block.predecessors.begin(), block.predecessors.end());

                                // افتراض أن الـ PHI يحتوي على أزواج (قيمة، كتلة)
                                std::set<std::string> phiBlocks;
                                for (size_t i = 1; i < inst.operands.size(); i += 2)
                                {
                                    phiBlocks.insert(inst.operands[i]);
                                }

                                for (const auto &pred : preds)
                                {
                                    if (!phiBlocks.count(pred))
                                    {
                                        result.errors.push_back(
                                            "PHI instruction missing value for predecessor '" +
                                            pred + "'");
                                        result.isValid = false;
                                    }
                                }
                            }
                        }
                    }

                    return result;
                }
            };

            // =============================================================================
            // القسم 4: التحقق من إدارة الذاكرة (25 إصلاح)
            // =============================================================================

            /**
             * @brief محقق الذاكرة
             */
            class MemoryValidator
            {
            public:
                struct ValidationResult
                {
                    bool isValid = true;
                    std::vector<std::string> errors;
                    std::vector<std::string> warnings;
                    std::vector<std::string> potentialLeaks;
                    std::vector<std::string> useAfterFree;
                    std::vector<std::string> doubleFree;
                };

                /**
                 * @brief التحقق من استخدام الذاكرة في الدالة
                 */
                ValidationResult validate(const SirFunction &function)
                {
                    ValidationResult result;

                    // تتبع حالة الذاكرة
                    std::map<std::string, MemoryState> memoryStates;

                    for (const auto &block : function.blocks)
                    {
                        for (const auto &inst : block.instructions)
                        {
                            validateInstruction(inst, memoryStates, result);
                        }
                    }

                    // البحث عن التسربات المحتملة
                    for (const auto &[var, state] : memoryStates)
                    {
                        if (state == MemoryState::Allocated)
                        {
                            result.potentialLeaks.push_back(var);
                            result.warnings.push_back("Potential memory leak: " + var);
                        }
                    }

                    return result;
                }

            private:
                enum class MemoryState
                {
                    Unknown,
                    Allocated,
                    Borrowed,
                    BorrowedMut,
                    Moved,
                    Freed
                };

                void validateInstruction(const SirInstruction &inst,
                                         std::map<std::string, MemoryState> &states,
                                         ValidationResult &result)
                {
                    switch (inst.opcode)
                    {
                    case SirOpcode::Alloca:
                        if (inst.result.has_value())
                        {
                            states[inst.result.value()] = MemoryState::Allocated;
                        }
                        break;

                    case SirOpcode::Move:
                        if (!inst.operands.empty())
                        {
                            MemoryState state = states[inst.operands[0]];
                            if (state == MemoryState::Moved)
                            {
                                result.errors.push_back(
                                    "Use after move: " + inst.operands[0]);
                                result.isValid = false;
                            }
                            else if (state == MemoryState::Freed)
                            {
                                result.useAfterFree.push_back(inst.operands[0]);
                                result.errors.push_back(
                                    "Use after free: " + inst.operands[0]);
                                result.isValid = false;
                            }
                            states[inst.operands[0]] = MemoryState::Moved;
                            if (inst.result.has_value())
                            {
                                states[inst.result.value()] = MemoryState::Allocated;
                            }
                        }
                        break;

                    case SirOpcode::Borrow:
                        if (!inst.operands.empty())
                        {
                            MemoryState state = states[inst.operands[0]];
                            if (state == MemoryState::Moved || state == MemoryState::Freed)
                            {
                                result.errors.push_back(
                                    "Borrowing moved/freed value: " + inst.operands[0]);
                                result.isValid = false;
                            }
                            if (inst.result.has_value())
                            {
                                states[inst.result.value()] = MemoryState::Borrowed;
                            }
                        }
                        break;

                    case SirOpcode::BorrowMut:
                        if (!inst.operands.empty())
                        {
                            MemoryState state = states[inst.operands[0]];
                            if (state == MemoryState::BorrowedMut)
                            {
                                result.errors.push_back(
                                    "Multiple mutable borrows: " + inst.operands[0]);
                                result.isValid = false;
                            }
                            if (inst.result.has_value())
                            {
                                states[inst.result.value()] = MemoryState::BorrowedMut;
                            }
                        }
                        break;

                    case SirOpcode::Drop:
                        if (!inst.operands.empty())
                        {
                            MemoryState state = states[inst.operands[0]];
                            if (state == MemoryState::Freed)
                            {
                                result.doubleFree.push_back(inst.operands[0]);
                                result.errors.push_back(
                                    "Double free: " + inst.operands[0]);
                                result.isValid = false;
                            }
                            states[inst.operands[0]] = MemoryState::Freed;
                        }
                        break;

                    case SirOpcode::Load:
                    case SirOpcode::Store:
                        // التحقق من الوصول للذاكرة
                        if (!inst.operands.empty())
                        {
                            MemoryState state = states[inst.operands[0]];
                            if (state == MemoryState::Freed)
                            {
                                result.useAfterFree.push_back(inst.operands[0]);
                                result.errors.push_back(
                                    "Memory access after free: " + inst.operands[0]);
                                result.isValid = false;
                            }
                        }
                        break;

                    default:
                        break;
                    }
                }
            };

            // =============================================================================
            // القسم 5: المحقق الشامل (تجميع كل المحققات)
            // =============================================================================

            /**
             * @brief المحقق الشامل لـ SIR
             */
            class SirValidator
            {
            public:
                struct FullValidationResult
                {
                    bool isValid = true;
                    size_t errorCount = 0;
                    size_t warningCount = 0;
                    std::vector<std::string> errors;
                    std::vector<std::string> warnings;

                    // تفاصيل خاصة
                    std::set<std::string> unreachableBlocks;
                    std::vector<std::string> potentialLeaks;
                    std::vector<std::string> useAfterFree;
                    std::vector<std::string> doubleFree;
                };

                /**
                 * @brief التحقق الشامل من الوحدة
                 */
                FullValidationResult validate(const SirModule &module)
                {
                    FullValidationResult result;

                    // التحقق من كل دالة
                    for (const auto &function : module.functions)
                    {
                        validateFunction(function, result);
                    }

                    result.errorCount = result.errors.size();
                    result.warningCount = result.warnings.size();

                    return result;
                }

            private:
                void validateFunction(const SirFunction &function, FullValidationResult &result)
                {
                    // 1. التحقق من الكتل
                    BlockValidator blockValidator;
                    for (const auto &block : function.blocks)
                    {
                        auto blockResult = blockValidator.validate(block, function);
                        if (!blockResult.isValid)
                        {
                            result.isValid = false;
                        }
                        for (auto &err : blockResult.errors)
                        {
                            result.errors.push_back("[" + function.name + "] " + err);
                        }
                        for (auto &warn : blockResult.warnings)
                        {
                            result.warnings.push_back("[" + function.name + "] " + warn);
                        }
                    }

                    // 2. التحقق من التعليمات
                    InstructionValidator instValidator;
                    std::map<std::string, TypeInfo> context;

                    // إضافة المعلمات إلى السياق
                    for (const auto &[name, type] : function.parameters)
                    {
                        context["%" + name] = type;
                    }

                    for (const auto &block : function.blocks)
                    {
                        for (const auto &inst : block.instructions)
                        {
                            auto instResult = instValidator.validate(inst, context);
                            if (!instResult.isValid)
                            {
                                result.isValid = false;
                            }
                            for (auto &err : instResult.errors)
                            {
                                result.errors.push_back("[" + function.name + "." + block.name + "] " + err);
                            }

                            // إضافة النتيجة إلى السياق
                            if (inst.result.has_value())
                            {
                                context[inst.result.value()] = inst.resultType;
                            }
                        }
                    }

                    // 3. التحقق من تدفق التحكم
                    ControlFlowValidator cfgValidator;
                    auto cfgResult = cfgValidator.validate(function);
                    if (!cfgResult.isValid)
                    {
                        result.isValid = false;
                    }
                    for (auto &err : cfgResult.errors)
                    {
                        result.errors.push_back("[" + function.name + "] CFG: " + err);
                    }
                    for (auto &warn : cfgResult.warnings)
                    {
                        result.warnings.push_back("[" + function.name + "] CFG: " + warn);
                    }
                    result.unreachableBlocks.insert(
                        cfgResult.unreachableBlocks.begin(),
                        cfgResult.unreachableBlocks.end());

                    // 4. التحقق من الذاكرة
                    MemoryValidator memValidator;
                    auto memResult = memValidator.validate(function);
                    if (!memResult.isValid)
                    {
                        result.isValid = false;
                    }
                    for (auto &err : memResult.errors)
                    {
                        result.errors.push_back("[" + function.name + "] Memory: " + err);
                    }
                    for (auto &warn : memResult.warnings)
                    {
                        result.warnings.push_back("[" + function.name + "] Memory: " + warn);
                    }
                    result.potentialLeaks.insert(
                        result.potentialLeaks.end(),
                        memResult.potentialLeaks.begin(),
                        memResult.potentialLeaks.end());
                    result.useAfterFree.insert(
                        result.useAfterFree.end(),
                        memResult.useAfterFree.begin(),
                        memResult.useAfterFree.end());
                    result.doubleFree.insert(
                        result.doubleFree.end(),
                        memResult.doubleFree.begin(),
                        memResult.doubleFree.end());
                }
            };


        } // namespace sir
    } // namespace compiler
} // namespace sad

