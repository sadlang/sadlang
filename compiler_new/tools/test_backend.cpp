// ===================================================================
// اختبار الخلفية LLVM - test_backend.cpp
// Test LLVM Backend - Creates SIR programmatically and generates LLVM IR
// ===================================================================
// بسم الله الرحمن الرحيم
// ===================================================================

#include "sir_module.h"
#include "sir_instruction.h"
#include "sir_types.h"
#include "llvm_codegen.h"

#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>
#include <memory>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace Sad::Compiler::SIR;
using namespace Sad::LLVM;

/// اختبار 1: دالة رئيسية بسيطة تطبع "مرحبا بالعالم"
/// Test 1: Simple main function that prints "Hello World"
std::shared_ptr<SIRModule> createHelloWorldModule() {
    auto module = std::make_shared<SIRModule>("hello_world");
    
    // إنشاء الدالة الرئيسية / Create main function
    auto mainFunc = std::make_shared<SIRFunction>("main", SIRType::I64);
    
    // كتلة الدخول / Entry block
    auto entryBlock = std::make_shared<SIRBasicBlock>("entry");
    
    // اطبع_سطر("مرحبا بالعالم") → CALL @اطبع_سطر("مرحبا بالعالم")
    auto printInst = SIRInstruction::CallVoid(
        SIROperand::Function("اطبع_سطر"),
        { SIROperand::ConstantString("مرحبا بالعالم من لغة ص!\n") }
    );
    entryBlock->addInstruction(printInst);
    
    // إرجاع 0 / Return 0
    auto retInst = SIRInstruction::Return(SIROperand::ConstantI64(0));
    entryBlock->addInstruction(retInst);
    
    mainFunc->addBasicBlock(entryBlock);
    module->addFunction(mainFunc);
    
    return module;
}

/// اختبار 2: حسابات رياضية بسيطة
/// Test 2: Simple arithmetic
std::shared_ptr<SIRModule> createArithmeticModule() {
    auto module = std::make_shared<SIRModule>("arithmetic_test");
    
    auto mainFunc = std::make_shared<SIRFunction>("main", SIRType::I64);
    auto entryBlock = std::make_shared<SIRBasicBlock>("entry");
    
    // متغير أ = 10 / var a = 10
    auto allocA = SIRInstruction::Alloc(
        SIROperand::Register("a_ptr", SIRType::PTR),
        SIRType::I64,
        SIROperand::ConstantI64(1)
    );
    entryBlock->addInstruction(allocA);
    
    auto storeA = SIRInstruction::Store(
        SIROperand::ConstantI64(10),
        SIROperand::Register("a_ptr", SIRType::PTR)
    );
    entryBlock->addInstruction(storeA);
    
    // متغير ب = 20 / var b = 20
    auto allocB = SIRInstruction::Alloc(
        SIROperand::Register("b_ptr", SIRType::PTR),
        SIRType::I64,
        SIROperand::ConstantI64(1)
    );
    entryBlock->addInstruction(allocB);
    
    auto storeB = SIRInstruction::Store(
        SIROperand::ConstantI64(20),
        SIROperand::Register("b_ptr", SIRType::PTR)
    );
    entryBlock->addInstruction(storeB);
    
    // متغير ج = أ + ب / var c = a + b
    auto loadA = SIRInstruction::Load(
        SIROperand::Register("a_val", SIRType::I64),
        SIROperand::Register("a_ptr", SIRType::PTR)
    );
    entryBlock->addInstruction(loadA);
    
    auto loadB = SIRInstruction::Load(
        SIROperand::Register("b_val", SIRType::I64),
        SIROperand::Register("b_ptr", SIRType::PTR)
    );
    entryBlock->addInstruction(loadB);
    
    auto addInst = SIRInstruction::Binary(
        SIROpcode::ADD_I64,
        SIROperand::Register("c_val", SIRType::I64),
        SIROperand::Register("a_val", SIRType::I64),
        SIROperand::Register("b_val", SIRType::I64)
    );
    entryBlock->addInstruction(addInst);
    
    // إرجاع ج / Return c
    auto retInst = SIRInstruction::Return(SIROperand::Register("c_val", SIRType::I64));
    entryBlock->addInstruction(retInst);
    
    mainFunc->addBasicBlock(entryBlock);
    module->addFunction(mainFunc);
    
    return module;
}

/// اختبار 3: تدفق التحكم (إذا/وإلا)
/// Test 3: Control flow (if/else)
std::shared_ptr<SIRModule> createControlFlowModule() {
    auto module = std::make_shared<SIRModule>("control_flow_test");
    
    auto mainFunc = std::make_shared<SIRFunction>("main", SIRType::I64);
    
    // كتلة الدخول / Entry block
    auto entryBlock = std::make_shared<SIRBasicBlock>("entry");
    
    // متغير س = 42 / var x = 42
    auto cmpInst = SIRInstruction::Binary(
        SIROpcode::GT,
        SIROperand::Register("cond", SIRType::BOOL),
        SIROperand::ConstantI64(42),
        SIROperand::ConstantI64(10)
    );
    entryBlock->addInstruction(cmpInst);
    
    // إذا (س > 10) → then وإلا → else
    auto branchInst = SIRInstruction::BranchCond(
        SIROperand::Register("cond", SIRType::BOOL),
        SIROperand::Label("then"),
        SIROperand::Label("else_block")
    );
    entryBlock->addInstruction(branchInst);
    
    // كتلة then / Then block
    auto thenBlock = std::make_shared<SIRBasicBlock>("then");
    auto printThen = SIRInstruction::CallVoid(
        SIROperand::Function("اطبع_سطر"),
        { SIROperand::ConstantString("42 أكبر من 10\n") }
    );
    thenBlock->addInstruction(printThen);
    auto brToEnd1 = SIRInstruction::Branch(SIROperand::Label("end"));
    thenBlock->addInstruction(brToEnd1);
    
    // كتلة else / Else block
    auto elseBlock = std::make_shared<SIRBasicBlock>("else_block");
    auto printElse = SIRInstruction::CallVoid(
        SIROperand::Function("اطبع_سطر"),
        { SIROperand::ConstantString("42 أصغر من أو يساوي 10\n") }
    );
    elseBlock->addInstruction(printElse);
    auto brToEnd2 = SIRInstruction::Branch(SIROperand::Label("end"));
    elseBlock->addInstruction(brToEnd2);
    
    // كتلة النهاية / End block
    auto endBlock = std::make_shared<SIRBasicBlock>("end");
    auto retInst = SIRInstruction::Return(SIROperand::ConstantI64(0));
    endBlock->addInstruction(retInst);
    
    mainFunc->addBasicBlock(entryBlock);
    mainFunc->addBasicBlock(thenBlock);
    mainFunc->addBasicBlock(elseBlock);
    mainFunc->addBasicBlock(endBlock);
    module->addFunction(mainFunc);
    
    return module;
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::cout << "=== اختبار خلفية LLVM لـ لغة ص ===" << std::endl;
    std::cout << "=== Sad Language LLVM Backend Test ===" << std::endl;
    std::cout << std::endl;

    // اختر الاختبار / Select test
    int testNum = 1;
    if (argc > 1) {
        testNum = std::atoi(argv[1]);
    }

    std::shared_ptr<SIRModule> module;
    std::string outputFile;
    
    switch (testNum) {
        case 1:
            std::cout << "[اختبار 1] مرحبا بالعالم / Hello World" << std::endl;
            module = createHelloWorldModule();
            outputFile = "test_hello.ll";
            break;
        case 2:
            std::cout << "[اختبار 2] حسابات رياضية / Arithmetic" << std::endl;
            module = createArithmeticModule();
            outputFile = "test_arithmetic.ll";
            break;
        case 3:
            std::cout << "[اختبار 3] تدفق التحكم / Control Flow" << std::endl;
            module = createControlFlowModule();
            outputFile = "test_control_flow.ll";
            break;
        default:
            std::cerr << "اختبار غير معروف: " << testNum << std::endl;
            return 1;
    }

    // طباعة SIR / Print SIR
    std::cout << "\n--- SIR Module ---" << std::endl;
    module->print();
    
    // إنشاء LLVM IR / Generate LLVM IR
    std::cout << "\n--- Generating LLVM IR ---" << std::endl;
    
    LLVMCodeGen codegen;
    if (!codegen.initialize("sad_test")) {
        std::cerr << "خطأ: فشل في تهيئة LLVM CodeGen" << std::endl;
        return 1;
    }
    
    auto llvmModule = codegen.generate(module);
    
    // طباعة LLVM IR من الوحدة المُعادة
    // Print LLVM IR from the returned module  
    std::cout << "\n--- LLVM IR ---" << std::endl;
    if (llvmModule) {
        std::string irStr;
        llvm::raw_string_ostream os(irStr);
        llvmModule->print(os, nullptr);
        std::cout << irStr << std::endl;
        
        // حفظ إلى ملف / Save to file
        std::error_code EC;
        llvm::raw_fd_ostream fileStream(outputFile, EC);
        if (!EC) {
            llvmModule->print(fileStream, nullptr);
            std::cout << "\n✓ تم حفظ LLVM IR إلى: " << outputFile << std::endl;
        } else {
            std::cerr << "\n✗ فشل في حفظ الملف: " << outputFile << std::endl;
        }
        
        std::cout << "✓ LLVM IR تم توليده بنجاح" << std::endl;
    } else {
        std::cerr << "خطأ: فشل في توليد LLVM IR" << std::endl;
    }

    std::cout << "\n=== انتهى الاختبار ===" << std::endl;
    return 0;
}
