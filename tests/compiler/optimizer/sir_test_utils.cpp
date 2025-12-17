/**
 * @file sir_test_utils.cpp
 * @brief تطبيق أدوات مساعدة لاختبارات SIR
 * @brief SIR Testing Utilities Implementation
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 */

#include "sir_test_utils.h"
#include <iostream>
#include <sstream>

namespace Sad {
namespace Compiler {
namespace Testing {

// ============================================================================
// SIRTestBuilder Implementation
// ============================================================================

SIRTestBuilder::SIRTestBuilder()
    : currentFunction_(nullptr)
    , tempCounter_(0)
{
}

SIRTestBuilder::~SIRTestBuilder() = default;

std::shared_ptr<SIR::SIRModule> SIRTestBuilder::createModule(const std::string& name) {
    module_ = std::make_shared<SIR::SIRModule>(name);
    tempCounter_ = 0;
    return module_;
}

SIR::SIRFunction* SIRTestBuilder::createFunction(
    const std::string& name,
    SIR::SIRType returnType
) {
    if (!module_) {
        createModule();
    }
    
    auto func = std::make_unique<SIR::SIRFunction>(name, returnType);
    currentFunction_ = func.get();
    module_->addFunction(std::move(func));
    
    return currentFunction_;
}

SIR::SIRBasicBlock* SIRTestBuilder::createBlock(const std::string& label) {
    std::string blockLabel = label.empty() ? "block_" + std::to_string(tempCounter_++) : label;
    auto block = std::make_unique<SIR::SIRBasicBlock>(blockLabel);
    return block.get();
}

SIR::SIRInstruction* SIRTestBuilder::createLoadConst(int64_t value, const std::string& dest) {
    auto inst = std::make_unique<SIR::SIRInstruction>(SIR::SIROpcode::LOAD_CONST);
    inst->setDestination(dest);
    inst->setIntValue(value);
    return inst.get();
}

SIR::SIRInstruction* SIRTestBuilder::createLoadConst(double value, const std::string& dest) {
    auto inst = std::make_unique<SIR::SIRInstruction>(SIR::SIROpcode::LOAD_CONST);
    inst->setDestination(dest);
    inst->setFloatValue(value);
    return inst.get();
}

SIR::SIRInstruction* SIRTestBuilder::createBinaryOp(
    SIR::SIROpcode op,
    const std::string& left,
    const std::string& right,
    const std::string& dest
) {
    auto inst = std::make_unique<SIR::SIRInstruction>(op);
    inst->setDestination(dest);
    inst->setOperand(0, left);
    inst->setOperand(1, right);
    return inst.get();
}

SIR::SIRInstruction* SIRTestBuilder::createUnaryOp(
    SIR::SIROpcode op,
    const std::string& operand,
    const std::string& dest
) {
    auto inst = std::make_unique<SIR::SIRInstruction>(op);
    inst->setDestination(dest);
    inst->setOperand(0, operand);
    return inst.get();
}

SIR::SIRInstruction* SIRTestBuilder::createReturn(const std::string& value) {
    auto inst = std::make_unique<SIR::SIRInstruction>(SIR::SIROpcode::RET);
    if (!value.empty()) {
        inst->setOperand(0, value);
    }
    return inst.get();
}

SIR::SIRInstruction* SIRTestBuilder::createBranch(const std::string& target) {
    auto inst = std::make_unique<SIR::SIRInstruction>(SIR::SIROpcode::BR);
    inst->setOperand(0, target);
    return inst.get();
}

SIR::SIRInstruction* SIRTestBuilder::createCondBranch(
    const std::string& condition,
    const std::string& trueTarget,
    const std::string& falseTarget
) {
    auto inst = std::make_unique<SIR::SIRInstruction>(SIR::SIROpcode::BR_COND);
    inst->setOperand(0, condition);
    inst->setOperand(1, trueTarget);
    inst->setOperand(2, falseTarget);
    return inst.get();
}

void SIRTestBuilder::addInstruction(SIR::SIRBasicBlock* block, SIR::SIRInstruction* inst) {
    if (block && inst) {
        block->addInstruction(std::unique_ptr<SIR::SIRInstruction>(inst));
    }
}

void SIRTestBuilder::addBlock(SIR::SIRFunction* func, SIR::SIRBasicBlock* block) {
    if (func && block) {
        func->addBasicBlock(std::shared_ptr<SIR::SIRBasicBlock>(block));
    }
}

void SIRTestBuilder::addFunction(SIR::SIRFunction* func) {
    if (module_ && func) {
        module_->addFunction(std::unique_ptr<SIR::SIRFunction>(func));
    }
}

std::string SIRTestBuilder::newTemp() {
    return "%t" + std::to_string(tempCounter_++);
}

void SIRTestBuilder::reset() {
    module_.reset();
    currentFunction_ = nullptr;
    tempCounter_ = 0;
}

// ============================================================================
// Quick Helper Functions
// ============================================================================

std::shared_ptr<SIR::SIRModule> createSimpleModule(const std::string& functionName) {
    SIRTestBuilder builder;
    auto module = builder.createModule("test_module");
    auto func = builder.createFunction(functionName);
    
    auto block = builder.createBlock("entry");
    auto inst1 = builder.createLoadConst(42, "%1");
    auto inst2 = builder.createReturn("%1");
    
    block->addInstruction(std::unique_ptr<SIR::SIRInstruction>(inst1));
    block->addInstruction(std::unique_ptr<SIR::SIRInstruction>(inst2));
    func->addBasicBlock(std::shared_ptr<SIR::SIRBasicBlock>(block));
    
    return module;
}

std::shared_ptr<SIR::SIRModule> createConstantArithmeticModule() {
    SIRTestBuilder builder;
    auto module = builder.createModule("const_arithmetic");
    auto func = builder.createFunction("calc");
    
    auto block = builder.createBlock("entry");
    
    // %1 = 2
    // %2 = 3
    // %3 = add %1, %2  -> 5
    // %4 = mul %3, 4   -> 20
    // ret %4
    
    block->addInstruction(std::unique_ptr<SIR::SIRInstruction>(builder.createLoadConst(2, "%1")));
    block->addInstruction(std::unique_ptr<SIR::SIRInstruction>(builder.createLoadConst(3, "%2")));
    block->addInstruction(std::unique_ptr<SIR::SIRInstruction>(builder.createBinaryOp(SIR::SIROpcode::ADD, "%1", "%2", "%3")));
    block->addInstruction(std::unique_ptr<SIR::SIRInstruction>(builder.createLoadConst(4, "%temp")));
    block->addInstruction(std::unique_ptr<SIR::SIRInstruction>(builder.createBinaryOp(SIR::SIROpcode::MUL, "%3", "%temp", "%4")));
    block->addInstruction(std::unique_ptr<SIR::SIRInstruction>(builder.createReturn("%4")));
    
    func->addBasicBlock(std::shared_ptr<SIR::SIRBasicBlock>(block));
    
    return module;
}

std::shared_ptr<SIR::SIRModule> createDeadCodeModule() {
    SIRTestBuilder builder;
    auto module = builder.createModule("dead_code");
    auto func = builder.createFunction("test");
    
    auto block = builder.createBlock("entry");
    
    // %1 = 10        <- Dead (unused)
    // %2 = mul %1, 2 <- Dead (unused)
    // %3 = 42
    // ret %3
    
    block->addInstruction(std::unique_ptr<SIR::SIRInstruction>(builder.createLoadConst(10, "%1")));
    block->addInstruction(std::unique_ptr<SIR::SIRInstruction>(builder.createLoadConst(2, "%temp")));
    block->addInstruction(std::unique_ptr<SIR::SIRInstruction>(builder.createBinaryOp(SIR::SIROpcode::MUL, "%1", "%temp", "%2")));
    block->addInstruction(std::unique_ptr<SIR::SIRInstruction>(builder.createLoadConst(42, "%3")));
    block->addInstruction(std::unique_ptr<SIR::SIRInstruction>(builder.createReturn("%3")));
    
    func->addBasicBlock(std::shared_ptr<SIR::SIRBasicBlock>(block));
    
    return module;
}

std::shared_ptr<SIR::SIRModule> createUnreachableBlocksModule() {
    SIRTestBuilder builder;
    auto module = builder.createModule("unreachable");
    auto func = builder.createFunction("test");
    
    // Entry block
    auto entry = builder.createBlock("entry");
    builder.addInstruction(entry, builder.createLoadConst(0, "%1"));
    builder.addInstruction(entry, builder.createReturn("%1"));
    builder.addBlock(func, entry);
    
    // Unreachable block
    auto unreachable = builder.createBlock("unreachable");
    builder.addInstruction(unreachable, builder.createLoadConst(99, "%2"));
    builder.addInstruction(unreachable, builder.createReturn("%2"));
    builder.addBlock(func, unreachable);
    
    return module;
}

std::shared_ptr<SIR::SIRModule> createBranchingModule() {
    SIRTestBuilder builder;
    auto module = builder.createModule("branching");
    auto func = builder.createFunction("test");
    
    // Entry: if condition then label_true else label_false
    auto entry = builder.createBlock("entry");
    builder.addInstruction(entry, builder.createLoadConst(1, "%cond"));
    builder.addInstruction(entry, builder.createCondBranch("%cond", "label_true", "label_false"));
    builder.addBlock(func, entry);
    
    // True branch
    auto trueBlock = builder.createBlock("label_true");
    builder.addInstruction(trueBlock, builder.createLoadConst(1, "%ret"));
    builder.addInstruction(trueBlock, builder.createReturn("%ret"));
    builder.addBlock(func, trueBlock);
    
    // False branch
    auto falseBlock = builder.createBlock("label_false");
    builder.addInstruction(falseBlock, builder.createLoadConst(0, "%ret"));
    builder.addInstruction(falseBlock, builder.createReturn("%ret"));
    builder.addBlock(func, falseBlock);
    
    return module;
}

std::shared_ptr<SIR::SIRModule> createComplexModule() {
    SIRTestBuilder builder;
    auto module = builder.createModule("complex");
    auto func = builder.createFunction("test");
    
    auto block = builder.createBlock("entry");
    
    // Complex expression: (2 + 3) * 4 - 5 / 1
    block->addInstruction(std::unique_ptr<SIR::SIRInstruction>(builder.createLoadConst(2, "%1")));
    block->addInstruction(std::unique_ptr<SIR::SIRInstruction>(builder.createLoadConst(3, "%2")));
    block->addInstruction(std::unique_ptr<SIR::SIRInstruction>(builder.createBinaryOp(SIR::SIROpcode::ADD, "%1", "%2", "%3")));  // 5
    
    block->addInstruction(std::unique_ptr<SIR::SIRInstruction>(builder.createLoadConst(4, "%4")));
    block->addInstruction(std::unique_ptr<SIR::SIRInstruction>(builder.createBinaryOp(SIR::SIROpcode::MUL, "%3", "%4", "%5")));  // 20
    
    block->addInstruction(std::unique_ptr<SIR::SIRInstruction>(builder.createLoadConst(5, "%6")));
    block->addInstruction(std::unique_ptr<SIR::SIRInstruction>(builder.createLoadConst(1, "%7")));
    block->addInstruction(std::unique_ptr<SIR::SIRInstruction>(builder.createBinaryOp(SIR::SIROpcode::DIV, "%6", "%7", "%8")));  // 5
    
    block->addInstruction(std::unique_ptr<SIR::SIRInstruction>(builder.createBinaryOp(SIR::SIROpcode::SUB, "%5", "%8", "%9")));  // 15
    
    block->addInstruction(std::unique_ptr<SIR::SIRInstruction>(builder.createReturn("%9")));
    
    func->addBasicBlock(std::shared_ptr<SIR::SIRBasicBlock>(block));
    
    return module;
}

// ============================================================================
// Verification Functions
// ============================================================================

int countInstructions(SIR::SIRFunction* func) {
    if (!func) return 0;
    
    int count = 0;
    auto blocks = func->getBasicBlocks();
    for (const auto& block : blocks) {
        if (block) {
            count += block->getInstructions().size();
        }
    }
    return count;
}

int countBlocks(SIR::SIRFunction* func) {
    if (!func) return 0;
    return func->getBasicBlocks().size();
}

bool hasInstruction(SIR::SIRFunction* func, SIR::SIROpcode opcode) {
    if (!func) return false;
    
    auto blocks = func->getBasicBlocks();
    for (const auto& block : blocks) {
        if (!block) continue;
        
        auto instructions = block->getInstructions();
        for (const auto& inst : instructions) {
            if (inst && inst->getOpcode() == opcode) {
                return true;
            }
        }
    }
    return false;
}

void printFunction(SIR::SIRFunction* func) {
    if (!func) {
        std::cout << "(null function)" << std::endl;
        return;
    }
    
    std::cout << "Function: " << func->name << std::endl;
    std::cout << "Blocks: " << countBlocks(func) << std::endl;
    std::cout << "Instructions: " << countInstructions(func) << std::endl;
    
    auto blocks = func->getBasicBlocks();
    for (size_t i = 0; i < blocks.size(); i++) {
        const auto& block = blocks[i];
        if (!block) continue;
        
        std::cout << "  Block " << i << ": " << block->getLabel() << std::endl;
        
        auto instructions = block->getInstructions();
        for (size_t j = 0; j < instructions.size(); j++) {
            const auto& inst = instructions[j];
            if (!inst) continue;
            
            std::cout << "    [" << j << "] " << "Opcode: " 
                      << static_cast<int>(inst->getOpcode()) << std::endl;
        }
    }
}

void printModule(std::shared_ptr<SIR::SIRModule> module) {
    if (!module) {
        std::cout << "(null module)" << std::endl;
        return;
    }
    
    std::cout << "Module: " << module->name << std::endl;
    std::cout << "Functions: " << module->getFunctions().size() << std::endl;
    
    auto functions = module->getFunctions();
    for (size_t i = 0; i < functions.size(); i++) {
        const auto& func = functions[i];
        if (func) {
            std::cout << "\n--- Function " << i << " ---" << std::endl;
            printFunction(func.get());
        }
    }
}

} // namespace Testing
} // namespace Compiler
} // namespace Sad
