/*
 * اختبارات وحدات SIR - SIR Module Tests
 * 
 * الوصف: اختبارات شاملة لنظام الوحدات في SIR
 * Description: Comprehensive tests for SIR module system
 *
 * يختبر:
 * Tests:
 * - إنشاء الوحدات / Module creation
 * - إنشاء الدوال / Function creation
 * - الكتل الأساسية / Basic blocks
 * - المتغيرات العامة / Global variables
 * - الأصناف / Classes
 */

#include <cassert>
#include <iostream>
#include <memory>
#include "sir_module.h"
#include "sir_instruction.h"
#include "sir_types.h"

// ============================================================================
// Basic Block Tests
// ============================================================================

void testBasicBlock() {
    std::cout << "Testing basic blocks..." << std::endl;
    
    auto block = std::make_shared<SIRBasicBlock>("entry");
    assert(block->name == "entry");
    assert(block->instructions.empty());
    
    // إضافة تعليمة / Add instruction
    SIRInstruction addInst(SIROpcode::ADD);
    addInst.setResult(SIROperand::makeRegister("sum"));
    addInst.addOperand(SIROperand::makeRegister("a"));
    addInst.addOperand(SIROperand::makeRegister("b"));
    
    block->addInstruction(addInst);
    assert(block->instructions.size() == 1);
    
    // إضافة تعليمة return / Add return instruction
    SIRInstruction retInst(SIROpcode::RET);
    retInst.addOperand(SIROperand::makeRegister("sum"));
    
    block->addInstruction(retInst);
    assert(block->instructions.size() == 2);
    
    std::cout << "  ✓ Basic blocks work correctly" << std::endl;
}

// ============================================================================
// Function Tests
// ============================================================================

void testFunction() {
    std::cout << "Testing functions..." << std::endl;
    
    // إنشاء دالة / Create function
    auto func = std::make_shared<SIRFunction>("add", makeI64Type());
    assert(func->getName() == "add");
    assert(func->getReturnType()->getBaseType() == SIRType::I64);
    
    // إضافة معاملات / Add parameters
    func->addParameter(SIRParameter{"a", makeI64Type()});
    func->addParameter(SIRParameter{"b", makeI64Type()});
    assert(func->getParameters().size() == 2);
    
    // إنشاء كتلة أساسية / Create basic block
    auto block = std::make_shared<SIRBasicBlock>("entry");
    
    // إضافة تعليمات / Add instructions
    SIRInstruction addInst(SIROpcode::ADD);
    addInst.setResult(SIROperand::makeRegister("sum"));
    addInst.addOperand(SIROperand::makeRegister("a"));
    addInst.addOperand(SIROperand::makeRegister("b"));
    block->addInstruction(addInst);
    
    SIRInstruction retInst(SIROpcode::RET);
    retInst.addOperand(SIROperand::makeRegister("sum"));
    block->addInstruction(retInst);
    
    // إضافة الكتلة للدالة / Add block to function
    func->addBasicBlock(block);
    assert(func->getBasicBlocks().size() == 1);
    
    // التحقق من صحة الدالة / Validate function
    assert(func->validate());
    
    // اختبار toString / Test toString
    std::string funcStr = func->toString();
    assert(funcStr.find("define i64 @add") != std::string::npos);
    assert(funcStr.find("entry:") != std::string::npos);
    assert(funcStr.find("add") != std::string::npos);
    assert(funcStr.find("ret") != std::string::npos);
    
    std::cout << "  ✓ Functions work correctly" << std::endl;
}

// ============================================================================
// Global Variable Tests
// ============================================================================

void testGlobalVariable() {
    std::cout << "Testing global variables..." << std::endl;
    
    // متغير عام قابل للتغيير / Mutable global variable
    auto globalVar = std::make_shared<SIRGlobalVariable>(
        "counter", makeI64Type(), "0", false
    );
    
    assert(globalVar->getName() == "counter");
    assert(globalVar->getType()->getBaseType() == SIRType::I64);
    assert(!globalVar->getIsConstant());
    
    std::string globalStr = globalVar->toString();
    assert(globalStr.find("$counter") != std::string::npos);
    assert(globalStr.find("global") != std::string::npos);
    assert(globalStr.find("i64") != std::string::npos);
    
    // ثابت عام / Constant global
    auto constVar = std::make_shared<SIRGlobalVariable>(
        "PI", makeF64Type(), "3.14159", true
    );
    
    assert(constVar->getIsConstant());
    std::string constStr = constVar->toString();
    assert(constStr.find("constant") != std::string::npos);
    
    std::cout << "  ✓ Global variables work correctly" << std::endl;
}

// ============================================================================
// Class Tests
// ============================================================================

void testClass() {
    std::cout << "Testing classes..." << std::endl;
    
    // إنشاء صنف / Create class
    auto cls = std::make_shared<SIRClass>("Point", "");
    assert(cls->getName() == "Point");
    
    // إضافة حقول / Add fields
    cls->addField("x", makeI64Type());
    cls->addField("y", makeI64Type());
    
    assert(cls->getField("x") != nullptr);
    assert(cls->getField("y") != nullptr);
    assert(cls->getField("z") == nullptr);
    
    // إنشاء دالة عضو / Create method
    auto method = std::make_shared<SIRFunction>("distance", makeF64Type());
    method->addParameter(SIRParameter{"this", makePtrType(makeI64Type())});
    
    auto block = std::make_shared<SIRBasicBlock>("entry");
    SIRInstruction retInst(SIROpcode::RET);
    retInst.addOperand(SIROperand::makeConstant("0.0", makeF64Type()));
    block->addInstruction(retInst);
    method->addBasicBlock(block);
    
    cls->addMethod(method);
    assert(cls->getMethod("distance") != nullptr);
    assert(cls->getMethod("unknown") == nullptr);
    
    // اختبار toString / Test toString
    std::string clsStr = cls->toString();
    assert(clsStr.find("class Point") != std::string::npos);
    assert(clsStr.find("i64 x") != std::string::npos);
    assert(clsStr.find("i64 y") != std::string::npos);
    
    std::cout << "  ✓ Classes work correctly" << std::endl;
}

// ============================================================================
// Class Inheritance Tests
// ============================================================================

void testClassInheritance() {
    std::cout << "Testing class inheritance..." << std::endl;
    
    // إنشاء صنف أساسي / Create base class
    auto baseClass = std::make_shared<SIRClass>("Shape", "");
    baseClass->addField("color", makeI64Type());
    
    // إنشاء صنف مشتق / Create derived class
    auto derivedClass = std::make_shared<SIRClass>("Circle", "Shape");
    derivedClass->addField("radius", makeF64Type());
    
    assert(derivedClass->getParentClass() == "Shape");
    
    std::string derivedStr = derivedClass->toString();
    assert(derivedStr.find("extends Shape") != std::string::npos);
    
    std::cout << "  ✓ Class inheritance works correctly" << std::endl;
}

// ============================================================================
// Module Tests
// ============================================================================

void testModule() {
    std::cout << "Testing modules..." << std::endl;
    
    auto module = std::make_shared<SIRModule>();
    
    // إضافة متغير عام / Add global variable
    auto globalVar = std::make_shared<SIRGlobalVariable>(
        "counter", makeI64Type(), "0", false
    );
    module->addGlobalVariable(globalVar);
    
    assert(module->getGlobalVariable("counter") != nullptr);
    assert(module->getGlobalVariable("unknown") == nullptr);
    
    // إضافة دالة main / Add main function
    auto mainFunc = std::make_shared<SIRFunction>("main", makeI64Type());
    
    auto block = std::make_shared<SIRBasicBlock>("entry");
    SIRInstruction retInst(SIROpcode::RET);
    retInst.addOperand(SIROperand::makeConstant("0", makeI64Type()));
    block->addInstruction(retInst);
    
    mainFunc->addBasicBlock(block);
    module->addFunction(mainFunc);
    
    assert(module->getFunction("main") != nullptr);
    assert(module->getFunction("unknown") == nullptr);
    
    // إضافة صنف / Add class
    auto cls = std::make_shared<SIRClass>("Point", "");
    cls->addField("x", makeI64Type());
    module->addClass(cls);
    
    assert(module->getClass("Point") != nullptr);
    
    // التحقق من صحة الوحدة / Validate module
    assert(module->validate());
    
    // اختبار toString / Test toString
    std::string moduleStr = module->toString();
    assert(moduleStr.find("SIR Module") != std::string::npos);
    assert(moduleStr.find("$counter") != std::string::npos);
    assert(moduleStr.find("@main") != std::string::npos);
    assert(moduleStr.find("class Point") != std::string::npos);
    
    std::cout << "  ✓ Modules work correctly" << std::endl;
}

// ============================================================================
// String Constant Tests
// ============================================================================

void testStringConstants() {
    std::cout << "Testing string constants..." << std::endl;
    
    auto module = std::make_shared<SIRModule>();
    
    // إضافة ثوابت نصية / Add string constants
    std::string name1 = module->addStringConstant("Hello, World!");
    std::string name2 = module->addStringConstant("Test string");
    std::string name3 = module->addStringConstant("Hello, World!"); // Duplicate
    
    assert(name1 == name3); // Should reuse existing constant
    assert(name1 != name2);
    
    assert(name1 == ".str0");
    assert(name2 == ".str1");
    
    std::string moduleStr = module->toString();
    assert(moduleStr.find(".str0 = \"Hello, World!\"") != std::string::npos);
    assert(moduleStr.find(".str1 = \"Test string\"") != std::string::npos);
    
    std::cout << "  ✓ String constants work correctly" << std::endl;
}

// ============================================================================
// Complete Program Test
// ============================================================================

void testCompleteProgram() {
    std::cout << "Testing complete program..." << std::endl;
    
    auto module = std::make_shared<SIRModule>();
    
    // برنامج بسيط: دالة تجمع رقمين / Simple program: function adds two numbers
    auto addFunc = std::make_shared<SIRFunction>("add", makeI64Type());
    addFunc->addParameter(SIRParameter{"a", makeI64Type()});
    addFunc->addParameter(SIRParameter{"b", makeI64Type()});
    
    auto addBlock = std::make_shared<SIRBasicBlock>("entry");
    
    SIRInstruction addInst(SIROpcode::ADD);
    addInst.setResult(SIROperand::makeRegister("sum"));
    addInst.addOperand(SIROperand::makeRegister("a"));
    addInst.addOperand(SIROperand::makeRegister("b"));
    addBlock->addInstruction(addInst);
    
    SIRInstruction retInst(SIROpcode::RET);
    retInst.addOperand(SIROperand::makeRegister("sum"));
    addBlock->addInstruction(retInst);
    
    addFunc->addBasicBlock(addBlock);
    module->addFunction(addFunc);
    
    // دالة main / Main function
    auto mainFunc = std::make_shared<SIRFunction>("main", makeI64Type());
    auto mainBlock = std::make_shared<SIRBasicBlock>("entry");
    
    SIRInstruction callInst(SIROpcode::CALL);
    callInst.setResult(SIROperand::makeRegister("result"));
    callInst.addOperand(SIROperand::makeGlobal("add"));
    callInst.addOperand(SIROperand::makeConstant("5", makeI64Type()));
    callInst.addOperand(SIROperand::makeConstant("7", makeI64Type()));
    callInst.setType(makeI64Type());
    mainBlock->addInstruction(callInst);
    
    SIRInstruction mainRetInst(SIROpcode::RET);
    mainRetInst.addOperand(SIROperand::makeRegister("result"));
    mainBlock->addInstruction(mainRetInst);
    
    mainFunc->addBasicBlock(mainBlock);
    module->addFunction(mainFunc);
    
    // التحقق من صحة البرنامج / Validate program
    assert(module->validate());
    
    // اختبار الإخراج الكامل / Test complete output
    std::string programStr = module->toString();
    assert(!programStr.empty());
    assert(programStr.find("@add") != std::string::npos);
    assert(programStr.find("@main") != std::string::npos);
    
    std::cout << "  ✓ Complete program works correctly" << std::endl;
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "SIR Module Test Suite" << std::endl;
    std::cout << "اختبارات وحدات SIR" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    try {
        testBasicBlock();
        testFunction();
        testGlobalVariable();
        testClass();
        testClassInheritance();
        testModule();
        testStringConstants();
        testCompleteProgram();
        
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
