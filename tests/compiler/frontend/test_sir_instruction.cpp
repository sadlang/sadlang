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
 */

#include <cassert>
#include <iostream>
#include <memory>
#include "sir_instruction.h"
#include "sir_types.h"

// ============================================================================
// Operand Tests
// ============================================================================

void testOperandCreation() {
    std::cout << "Testing operand creation..." << std::endl;
    
    // اختبار معامل سجل / Test register operand
    auto regOp = SIROperand::makeRegister("x");
    assert(regOp.getKind() == SIROperandKind::REGISTER);
    assert(regOp.getName() == "x");
    assert(regOp.toString() == "%x");
    
    // اختبار معامل ثابت / Test constant operand
    auto constOp = SIROperand::makeConstant("42", makeI64Type());
    assert(constOp.getKind() == SIROperandKind::CONSTANT);
    assert(constOp.getName() == "42");
    assert(constOp.toString() == "42");
    
    // اختبار معامل تسمية / Test label operand
    auto labelOp = SIROperand::makeLabel("entry");
    assert(labelOp.getKind() == SIROperandKind::LABEL);
    assert(labelOp.getName() == "entry");
    assert(labelOp.toString() == "entry");
    
    // اختبار معامل عام / Test global operand
    auto globalOp = SIROperand::makeGlobal("counter");
    assert(globalOp.getKind() == SIROperandKind::GLOBAL);
    assert(globalOp.getName() == "counter");
    assert(globalOp.toString() == "$counter");
    
    std::cout << "  ✓ Operands created successfully" << std::endl;
}

// ============================================================================
// Arithmetic Instruction Tests
// ============================================================================

void testArithmeticInstructions() {
    std::cout << "Testing arithmetic instructions..." << std::endl;
    
    // اختبار ADD / Test ADD
    auto addInst = std::make_shared<SIRInstruction>(SIROpcode::ADD);
    addInst->setResult(SIROperand::makeRegister("result"));
    addInst->addOperand(SIROperand::makeRegister("a"));
    addInst->addOperand(SIROperand::makeRegister("b"));
    
    assert(addInst->getOpcode() == SIROpcode::ADD);
    assert(addInst->hasResult());
    assert(addInst->getOperands().size() == 2);
    
    std::string expected = "%result = add %a, %b";
    assert(addInst->toString() == expected);
    
    // اختبار SUB / Test SUB
    auto subInst = std::make_shared<SIRInstruction>(SIROpcode::SUB);
    subInst->setResult(SIROperand::makeRegister("result"));
    subInst->addOperand(SIROperand::makeRegister("x"));
    subInst->addOperand(SIROperand::makeConstant("10", makeI64Type()));
    
    assert(subInst->toString() == "%result = sub %x, 10");
    
    // اختبار MUL / Test MUL
    auto mulInst = std::make_shared<SIRInstruction>(SIROpcode::MUL);
    mulInst->setResult(SIROperand::makeRegister("product"));
    mulInst->addOperand(SIROperand::makeConstant("5", makeI64Type()));
    mulInst->addOperand(SIROperand::makeConstant("7", makeI64Type()));
    
    assert(mulInst->toString() == "%product = mul 5, 7");
    
    std::cout << "  ✓ Arithmetic instructions work correctly" << std::endl;
}

// ============================================================================
// Comparison Instruction Tests
// ============================================================================

void testComparisonInstructions() {
    std::cout << "Testing comparison instructions..." << std::endl;
    
    // اختبار CMP_EQ / Test CMP_EQ
    auto cmpEqInst = std::make_shared<SIRInstruction>(SIROpcode::CMP_EQ);
    cmpEqInst->setResult(SIROperand::makeRegister("cond"));
    cmpEqInst->addOperand(SIROperand::makeRegister("a"));
    cmpEqInst->addOperand(SIROperand::makeRegister("b"));
    
    assert(cmpEqInst->toString() == "%cond = cmp_eq %a, %b");
    
    // اختبار CMP_LT / Test CMP_LT
    auto cmpLtInst = std::make_shared<SIRInstruction>(SIROpcode::CMP_LT);
    cmpLtInst->setResult(SIROperand::makeRegister("less"));
    cmpLtInst->addOperand(SIROperand::makeRegister("x"));
    cmpLtInst->addOperand(SIROperand::makeConstant("100", makeI64Type()));
    
    assert(cmpLtInst->toString() == "%less = cmp_lt %x, 100");
    
    std::cout << "  ✓ Comparison instructions work correctly" << std::endl;
}

// ============================================================================
// Memory Instruction Tests
// ============================================================================

void testMemoryInstructions() {
    std::cout << "Testing memory instructions..." << std::endl;
    
    // اختبار ALLOCA / Test ALLOCA
    auto allocaInst = std::make_shared<SIRInstruction>(SIROpcode::ALLOCA);
    allocaInst->setResult(SIROperand::makeRegister("ptr"));
    allocaInst->setType(makePtrType(makeI64Type()));
    
    assert(allocaInst->toString() == "%ptr = alloca i64*");
    
    // اختبار LOAD / Test LOAD
    auto loadInst = std::make_shared<SIRInstruction>(SIROpcode::LOAD);
    loadInst->setResult(SIROperand::makeRegister("value"));
    loadInst->addOperand(SIROperand::makeRegister("ptr"));
    loadInst->setType(makeI64Type());
    
    assert(loadInst->toString() == "%value = load i64, %ptr");
    
    // اختبار STORE / Test STORE
    auto storeInst = std::make_shared<SIRInstruction>(SIROpcode::STORE);
    storeInst->addOperand(SIROperand::makeConstant("42", makeI64Type()));
    storeInst->addOperand(SIROperand::makeRegister("ptr"));
    storeInst->setType(makeI64Type());
    
    assert(storeInst->toString() == "store i64 42, %ptr");
    
    std::cout << "  ✓ Memory instructions work correctly" << std::endl;
}

// ============================================================================
// Control Flow Instruction Tests
// ============================================================================

void testControlFlowInstructions() {
    std::cout << "Testing control flow instructions..." << std::endl;
    
    // اختبار BR (unconditional) / Test BR
    auto brInst = std::make_shared<SIRInstruction>(SIROpcode::BR);
    brInst->addOperand(SIROperand::makeLabel("loop_start"));
    
    assert(brInst->toString() == "br loop_start");
    
    // اختبار BR_COND (conditional) / Test BR_COND
    auto brCondInst = std::make_shared<SIRInstruction>(SIROpcode::BR_COND);
    brCondInst->addOperand(SIROperand::makeRegister("cond"));
    brCondInst->addOperand(SIROperand::makeLabel("then_block"));
    brCondInst->addOperand(SIROperand::makeLabel("else_block"));
    
    assert(brCondInst->toString() == "br_cond %cond, then_block, else_block");
    
    // اختبار RET / Test RET
    auto retInst = std::make_shared<SIRInstruction>(SIROpcode::RET);
    retInst->addOperand(SIROperand::makeRegister("result"));
    
    assert(retInst->toString() == "ret %result");
    
    // اختبار RET void / Test RET void
    auto retVoidInst = std::make_shared<SIRInstruction>(SIROpcode::RET);
    assert(retVoidInst->toString() == "ret");
    
    std::cout << "  ✓ Control flow instructions work correctly" << std::endl;
}

// ============================================================================
// Function Call Tests
// ============================================================================

void testFunctionCalls() {
    std::cout << "Testing function call instructions..." << std::endl;
    
    // اختبار CALL / Test CALL
    auto callInst = std::make_shared<SIRInstruction>(SIROpcode::CALL);
    callInst->setResult(SIROperand::makeRegister("result"));
    callInst->addOperand(SIROperand::makeGlobal("factorial"));
    callInst->addOperand(SIROperand::makeConstant("5", makeI64Type()));
    callInst->setType(makeI64Type());
    
    assert(callInst->toString() == "%result = call i64 $factorial(5)");
    
    // اختبار CALL void / Test CALL void
    auto callVoidInst = std::make_shared<SIRInstruction>(SIROpcode::CALL);
    callVoidInst->addOperand(SIROperand::makeGlobal("print"));
    callVoidInst->addOperand(SIROperand::makeRegister("msg"));
    callVoidInst->setType(makeVoidType());
    
    assert(callVoidInst->toString() == "call void $print(%msg)");
    
    std::cout << "  ✓ Function call instructions work correctly" << std::endl;
}

// ============================================================================
// Bitwise Operation Tests
// ============================================================================

void testBitwiseOperations() {
    std::cout << "Testing bitwise operations..." << std::endl;
    
    // اختبار AND / Test AND
    auto andInst = std::make_shared<SIRInstruction>(SIROpcode::AND);
    andInst->setResult(SIROperand::makeRegister("result"));
    andInst->addOperand(SIROperand::makeRegister("a"));
    andInst->addOperand(SIROperand::makeRegister("b"));
    
    assert(andInst->toString() == "%result = and %a, %b");
    
    // اختبار OR / Test OR
    auto orInst = std::make_shared<SIRInstruction>(SIROpcode::OR);
    orInst->setResult(SIROperand::makeRegister("result"));
    orInst->addOperand(SIROperand::makeRegister("x"));
    orInst->addOperand(SIROperand::makeConstant("0xFF", makeI64Type()));
    
    assert(orInst->toString() == "%result = or %x, 0xFF");
    
    // اختبار SHL / Test SHL
    auto shlInst = std::make_shared<SIRInstruction>(SIROpcode::SHL);
    shlInst->setResult(SIROperand::makeRegister("shifted"));
    shlInst->addOperand(SIROperand::makeRegister("value"));
    shlInst->addOperand(SIROperand::makeConstant("2", makeI64Type()));
    
    assert(shlInst->toString() == "%shifted = shl %value, 2");
    
    std::cout << "  ✓ Bitwise operations work correctly" << std::endl;
}

// ============================================================================
// Opcode String Conversion Tests
// ============================================================================

void testOpcodeConversion() {
    std::cout << "Testing opcode string conversion..." << std::endl;
    
    // Test various opcodes
    assert(sirOpcodeToString(SIROpcode::ADD) == "add");
    assert(sirOpcodeToString(SIROpcode::SUB) == "sub");
    assert(sirOpcodeToString(SIROpcode::MUL) == "mul");
    assert(sirOpcodeToString(SIROpcode::LOAD) == "load");
    assert(sirOpcodeToString(SIROpcode::STORE) == "store");
    assert(sirOpcodeToString(SIROpcode::BR) == "br");
    assert(sirOpcodeToString(SIROpcode::RET) == "ret");
    assert(sirOpcodeToString(SIROpcode::CALL) == "call");
    
    std::cout << "  ✓ Opcode conversion works correctly" << std::endl;
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
