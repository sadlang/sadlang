/*
 * اختبارات تعليمات SIR - SIR Instruction Tests
 * 
 * الوصف: اختبارات شاملة لنظام التعليمات في SIR
 * Description: Comprehensive tests for SIR instruction system
 *
 * يختبر:
 * Tests:
 * - إنشاء المعاملات / Operand creation
 * - إنشاء التعليمات / Instruction creation
 * - التعليمات الحسابية / Arithmetic instructions
 * - تعليمات الذاكرة / Memory instructions
 * - تعليمات التحكم / Control instructions
 *
 * Updated: February 2026 - Aligned with current SIR API
 *   (SIROperandType, SIROperand::Register, ConstantI64, etc.)
 */

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include "sir_instruction.h"
#include "sir_types.h"

using namespace Sad::Compiler::SIR;

// ============================================================================
// Operand Tests
// ============================================================================

void testOperandCreation() {
    std::cout << "Testing operand creation..." << std::endl;
    
    // اختبار معامل سجل / Test register operand
    auto regOp = SIROperand::Register("x", SadTypeKind::Integer);
    assert(regOp.type == SIROperandType::REGISTER);
    assert(regOp.name == "x");
    assert(regOp.dataType == SadTypeKind::Integer);
    assert(regOp.toString() == "%x");
    
    // اختبار معامل ثابت صحيح / Test integer constant operand
    auto constOp = SIROperand::ConstantI64(42);
    assert(constOp.type == SIROperandType::CONSTANT);
    assert(constOp.dataType == SadTypeKind::Integer);
    assert(constOp.intValue == 42);
    assert(constOp.toString() == "42");
    
    // اختبار معامل ثابت عشري / Test float constant operand
    auto floatOp = SIROperand::ConstantF64(3.14);
    assert(floatOp.type == SIROperandType::CONSTANT);
    assert(floatOp.dataType == SadTypeKind::Float);
    
    // اختبار معامل ثابت منطقي / Test boolean constant operand
    auto boolOp = SIROperand::ConstantBool(true);
    assert(boolOp.type == SIROperandType::CONSTANT);
    assert(boolOp.dataType == SadTypeKind::Boolean);
    assert(boolOp.toString() == "true");
    
    // اختبار معامل نصي / Test string constant operand
    auto strOp = SIROperand::ConstantString("hello");
    assert(strOp.type == SIROperandType::CONSTANT);
    assert(strOp.dataType == SadTypeKind::String);
    
    // اختبار معامل تسمية / Test label operand
    auto labelOp = SIROperand::Label("entry");
    assert(labelOp.type == SIROperandType::LABEL);
    assert(labelOp.name == "entry");
    assert(labelOp.toString() == "label %entry");
    
    // اختبار معامل عام / Test global operand
    auto globalOp = SIROperand::Global("counter", SadTypeKind::Integer);
    assert(globalOp.type == SIROperandType::GLOBAL);
    assert(globalOp.name == "counter");
    assert(globalOp.toString() == "$counter");
    
    // اختبار معامل دالة / Test function operand
    auto funcOp = SIROperand::Function("myFunc");
    assert(funcOp.type == SIROperandType::FUNCTION);
    assert(funcOp.name == "myFunc");
    assert(funcOp.toString() == "@myFunc");
    
    std::cout << "  ✓ Operands created successfully" << std::endl;
}

// ============================================================================
// Arithmetic Instruction Tests
// ============================================================================

void testArithmeticInstructions() {
    std::cout << "Testing arithmetic instructions..." << std::endl;
    
    // اختبار ADD / Test ADD (using factory method)
    auto addInst = SIRInstruction::Binary(
        SIROpcode::ADD_I64,
        SIROperand::Register("result", SadTypeKind::Integer),
        SIROperand::Register("a", SadTypeKind::Integer),
        SIROperand::Register("b", SadTypeKind::Integer)
    );
    assert(addInst.opcode == SIROpcode::ADD_I64);
    assert(addInst.hasResult());
    assert(addInst.operands.size() == 2);
    assert(addInst.toString() == "%result = add.i64 %a, %b");
    
    // اختبار SUB / Test SUB (manual construction)
    SIRInstruction subInst(SIROpcode::SUB_I64);
    subInst.result = SIROperand::Register("result", SadTypeKind::Integer);
    subInst.operands.push_back(SIROperand::Register("x", SadTypeKind::Integer));
    subInst.operands.push_back(SIROperand::ConstantI64(10));
    assert(subInst.toString() == "%result = sub.i64 %x, 10");
    
    // اختبار MUL / Test MUL
    auto mulInst = SIRInstruction::Binary(
        SIROpcode::MUL_I64,
        SIROperand::Register("product", SadTypeKind::Integer),
        SIROperand::ConstantI64(5),
        SIROperand::ConstantI64(7)
    );
    assert(mulInst.toString() == "%product = mul.i64 5, 7");
    
    // اختبار DIV / Test DIV
    auto divInst = SIRInstruction::Binary(
        SIROpcode::DIV_I64,
        SIROperand::Register("quotient", SadTypeKind::Integer),
        SIROperand::Register("a", SadTypeKind::Integer),
        SIROperand::ConstantI64(2)
    );
    assert(divInst.opcode == SIROpcode::DIV_I64);
    assert(divInst.hasResult());
    
    // اختبار NEG / Test NEG (unary)
    auto negInst = SIRInstruction::Unary(
        SIROpcode::NEG,
        SIROperand::Register("negated", SadTypeKind::Integer),
        SIROperand::Register("x", SadTypeKind::Integer)
    );
    assert(negInst.operands.size() == 1);
    assert(negInst.toString() == "%negated = neg %x");
    
    std::cout << "  ✓ Arithmetic instructions work correctly" << std::endl;
}

// ============================================================================
// Comparison Instruction Tests
// ============================================================================

void testComparisonInstructions() {
    std::cout << "Testing comparison instructions..." << std::endl;
    
    // اختبار EQ / Test EQ
    auto eqInst = SIRInstruction::Binary(
        SIROpcode::EQ,
        SIROperand::Register("cond", SadTypeKind::Boolean),
        SIROperand::Register("a", SadTypeKind::Integer),
        SIROperand::Register("b", SadTypeKind::Integer)
    );
    assert(eqInst.toString() == "%cond = eq %a, %b");
    
    // اختبار LT / Test LT
    auto ltInst = SIRInstruction::Binary(
        SIROpcode::LT,
        SIROperand::Register("less", SadTypeKind::Boolean),
        SIROperand::Register("x", SadTypeKind::Integer),
        SIROperand::ConstantI64(100)
    );
    assert(ltInst.toString() == "%less = lt %x, 100");
    
    // اختبار GE / Test GE
    auto geInst = SIRInstruction::Binary(
        SIROpcode::GE,
        SIROperand::Register("ge_result", SadTypeKind::Boolean),
        SIROperand::Register("a", SadTypeKind::Integer),
        SIROperand::Register("b", SadTypeKind::Integer)
    );
    assert(geInst.opcode == SIROpcode::GE);
    
    std::cout << "  ✓ Comparison instructions work correctly" << std::endl;
}

// ============================================================================
// Memory Instruction Tests
// ============================================================================

void testMemoryInstructions() {
    std::cout << "Testing memory instructions..." << std::endl;
    
    // اختبار LOAD / Test LOAD (using factory)
    auto loadInst = SIRInstruction::Load(
        SIROperand::Register("value", SadTypeKind::Integer),
        SIROperand::Register("ptr", SadTypeKind::Pointer)
    );
    assert(loadInst.opcode == SIROpcode::LOAD);
    assert(loadInst.hasResult());
    assert(loadInst.operands.size() == 1);
    assert(loadInst.toString() == "%value = load %ptr");
    
    // اختبار STORE / Test STORE (using factory)
    auto storeInst = SIRInstruction::Store(
        SIROperand::ConstantI64(42),
        SIROperand::Register("ptr", SadTypeKind::Pointer)
    );
    assert(storeInst.opcode == SIROpcode::STORE);
    assert(!storeInst.hasResult());
    assert(storeInst.operands.size() == 2);
    assert(storeInst.toString() == "store 42, %ptr");
    
    // اختبار ALLOC / Test ALLOC (using factory)
    auto allocInst = SIRInstruction::Alloc(
        SIROperand::Register("ptr", SadTypeKind::Pointer),
        SadTypeKind::Integer,
        SIROperand::ConstantI64(1)
    );
    assert(allocInst.opcode == SIROpcode::ALLOC);
    assert(allocInst.hasResult());
    
    std::cout << "  ✓ Memory instructions work correctly" << std::endl;
}

// ============================================================================
// Control Flow Instruction Tests
// ============================================================================

void testControlFlowInstructions() {
    std::cout << "Testing control flow instructions..." << std::endl;
    
    // اختبار BR (unconditional) / Test BR (using factory)
    auto brInst = SIRInstruction::Branch(SIROperand::Label("loop_start"));
    assert(brInst.opcode == SIROpcode::BR);
    assert(brInst.isTerminatorInst());
    assert(brInst.toString() == "br label %loop_start");
    
    // اختبار BR_COND (conditional) / Test BR_COND (using factory)
    auto brCondInst = SIRInstruction::BranchCond(
        SIROperand::Register("cond", SadTypeKind::Boolean),
        SIROperand::Label("then_block"),
        SIROperand::Label("else_block")
    );
    assert(brCondInst.opcode == SIROpcode::BR_COND);
    assert(brCondInst.isTerminatorInst());
    assert(brCondInst.operands.size() == 3);
    assert(brCondInst.toString() == "br.cond %cond, label %then_block, label %else_block");
    
    // اختبار RET / Test RET (using factory)
    auto retInst = SIRInstruction::Return(SIROperand::Register("result", SadTypeKind::Integer));
    assert(retInst.opcode == SIROpcode::RET);
    assert(retInst.isTerminatorInst());
    assert(retInst.toString() == "ret %result");
    
    // اختبار RET void / Test RET void (using factory)
    auto retVoidInst = SIRInstruction::ReturnVoid();
    assert(retVoidInst.opcode == SIROpcode::RET_VOID);
    assert(retVoidInst.isTerminatorInst());
    assert(retVoidInst.toString() == "ret.void");
    
    std::cout << "  ✓ Control flow instructions work correctly" << std::endl;
}

// ============================================================================
// Function Call Tests
// ============================================================================

void testFunctionCalls() {
    std::cout << "Testing function call instructions..." << std::endl;
    
    // اختبار CALL / Test CALL (using factory)
    auto callInst = SIRInstruction::Call(
        SIROperand::Register("result", SadTypeKind::Integer),
        SIROperand::Function("factorial"),
        { SIROperand::ConstantI64(5) }
    );
    assert(callInst.opcode == SIROpcode::CALL);
    assert(callInst.hasResult());
    // operands = [function, arg1]
    assert(callInst.operands.size() == 2);
    
    // اختبار CALL void / Test CALL void (using factory)
    auto callVoidInst = SIRInstruction::CallVoid(
        SIROperand::Function("print"),
        { SIROperand::Register("msg", SadTypeKind::String) }
    );
    assert(callVoidInst.opcode == SIROpcode::CALL);
    assert(!callVoidInst.hasResult());
    
    // اختبار CALL مع عدة معاملات / Test CALL with multiple args
    auto callMultiInst = SIRInstruction::Call(
        SIROperand::Register("sum", SadTypeKind::Integer),
        SIROperand::Function("add_three"),
        { SIROperand::ConstantI64(1), SIROperand::ConstantI64(2), SIROperand::ConstantI64(3) }
    );
    assert(callMultiInst.operands.size() == 4); // function + 3 args
    
    std::cout << "  ✓ Function call instructions work correctly" << std::endl;
}

// ============================================================================
// Bitwise Operation Tests
// ============================================================================

void testBitwiseOperations() {
    std::cout << "Testing bitwise operations..." << std::endl;
    
    // اختبار AND / Test AND
    auto andInst = SIRInstruction::Binary(
        SIROpcode::AND,
        SIROperand::Register("result", SadTypeKind::Integer),
        SIROperand::Register("a", SadTypeKind::Integer),
        SIROperand::Register("b", SadTypeKind::Integer)
    );
    assert(andInst.toString() == "%result = and %a, %b");
    
    // اختبار OR / Test OR
    auto orInst = SIRInstruction::Binary(
        SIROpcode::OR,
        SIROperand::Register("result", SadTypeKind::Integer),
        SIROperand::Register("x", SadTypeKind::Integer),
        SIROperand::ConstantI64(0xFF)
    );
    assert(orInst.toString() == "%result = or %x, 255");
    
    // اختبار SHL / Test SHL
    auto shlInst = SIRInstruction::Binary(
        SIROpcode::SHL,
        SIROperand::Register("shifted", SadTypeKind::Integer),
        SIROperand::Register("value", SadTypeKind::Integer),
        SIROperand::ConstantI64(2)
    );
    assert(shlInst.toString() == "%shifted = shl %value, 2");
    
    // اختبار XOR / Test XOR
    auto xorInst = SIRInstruction::Binary(
        SIROpcode::XOR,
        SIROperand::Register("xor_result", SadTypeKind::Integer),
        SIROperand::Register("a", SadTypeKind::Integer),
        SIROperand::Register("b", SadTypeKind::Integer)
    );
    assert(xorInst.opcode == SIROpcode::XOR);
    
    std::cout << "  ✓ Bitwise operations work correctly" << std::endl;
}

// ============================================================================
// Opcode String Conversion Tests
// ============================================================================

void testOpcodeConversion() {
    std::cout << "Testing opcode string conversion..." << std::endl;
    
    // Test various opcodes
    assert(std::string(sirOpcodeToString(SIROpcode::ADD_I64)) == "add.i64");
    assert(std::string(sirOpcodeToString(SIROpcode::SUB_I64)) == "sub.i64");
    assert(std::string(sirOpcodeToString(SIROpcode::MUL_I64)) == "mul.i64");
    assert(std::string(sirOpcodeToString(SIROpcode::LOAD)) == "load");
    assert(std::string(sirOpcodeToString(SIROpcode::STORE)) == "store");
    assert(std::string(sirOpcodeToString(SIROpcode::BR)) == "br");
    assert(std::string(sirOpcodeToString(SIROpcode::RET)) == "ret");
    assert(std::string(sirOpcodeToString(SIROpcode::CALL)) == "call");
    assert(std::string(sirOpcodeToString(SIROpcode::AND)) == "and");
    assert(std::string(sirOpcodeToString(SIROpcode::EQ)) == "eq");
    
    std::cout << "  ✓ Opcode conversion works correctly" << std::endl;
}

// ============================================================================
// Basic Block Tests
// ============================================================================

void testBasicBlocks() {
    std::cout << "Testing basic blocks..." << std::endl;
    
    SIRBasicBlock block("entry");
    assert(block.name == "entry");
    assert(block.empty());
    assert(block.size() == 0);
    
    block.addInstruction(SIRInstruction::Binary(
        SIROpcode::ADD_I64,
        SIROperand::Register("t1", SadTypeKind::Integer),
        SIROperand::Register("a", SadTypeKind::Integer),
        SIROperand::Register("b", SadTypeKind::Integer)
    ));
    assert(block.size() == 1);
    assert(!block.empty());
    
    block.addInstruction(SIRInstruction::Return(
        SIROperand::Register("t1", SadTypeKind::Integer)
    ));
    assert(block.size() == 2);
    
    auto* terminator = block.getTerminator();
    assert(terminator != nullptr);
    assert(terminator->opcode == SIROpcode::RET);
    
    std::cout << "  ✓ Basic blocks work correctly" << std::endl;
}

// ============================================================================
// Factory Method Tests
// ============================================================================

void testFactoryMethods() {
    std::cout << "Testing factory methods..." << std::endl;
    
    // PHI instruction
    auto phiInst = SIRInstruction::Phi(
        SIROperand::Register("merged", SadTypeKind::Integer),
        {
            { SIROperand::Register("val1", SadTypeKind::Integer), SIROperand::Label("bb1") },
            { SIROperand::Register("val2", SadTypeKind::Integer), SIROperand::Label("bb2") }
        }
    );
    assert(phiInst.opcode == SIROpcode::PHI);
    assert(phiInst.hasResult());
    assert(phiInst.operands.size() == 4); // val1, bb1, val2, bb2
    
    std::cout << "  ✓ Factory methods work correctly" << std::endl;
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "SIR Instruction Test Suite" << std::endl;
    std::cout << "اختبارات تعليمات SIR" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    try {
        testOperandCreation();
        testArithmeticInstructions();
        testComparisonInstructions();
        testMemoryInstructions();
        testControlFlowInstructions();
        testFunctionCalls();
        testBitwiseOperations();
        testOpcodeConversion();
        testBasicBlocks();
        testFactoryMethods();
        
        std::cout << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "✓ All tests passed!" << std::endl;
        std::cout << "✓ جميع الاختبارات نجحت!" << std::endl;
        std::cout << "========================================" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << std::endl;
        std::cerr << "========================================" << std::endl;
        std::cerr << "✗ Test failed: " << e.what() << std::endl;
        std::cerr << "✗ فشل الاختبار" << std::endl;
        std::cerr << "========================================" << std::endl;
        
        return 1;
    }
}
