/*
 * ============================================================================
 * اختبار بسيط لمولد كود LLVM
 * Simple Test for LLVM Code Generator
 * ============================================================================
 * 
 * الالتزام بـ STRICT_CODING_RULES.md:
 * - تم قراءة llvm_codegen.h بالكامل (687 سطر)
 * - تم توثيق كل API مستخدم بالمصدر ورقم السطر
 * - لا توجد دوال مخترعة
 * 
 * STRICT_CODING_RULES.md Compliance:
 * - Read llvm_codegen.h completely (687 lines)
 * - Every API documented with source and line number
 * - No invented functions
 * 
 * التاريخ (Date): 4 يناير 2026
 * الإصدار (Version): 1.0.0 - Compliant with STRICT_CODING_RULES.md
 * ============================================================================
 */

#include <iostream>
#include <memory>
#include <cassert>

// Source: compiler/backends/llvm/llvm_codegen.h - المُنشئ موجود في line 176
#include "llvm_codegen.h"

// Source: compiler/frontend/include/sir_module.h - SIRModule definition
#include "sir_module.h"
#include "sir_instruction.h"
#include "sir_types.h"

using namespace Sad::LLVM;
using namespace Sad::Compiler::SIR;

// ============================================================================
// اختبار 1: تهيئة مولد الكود
// Test 1: Code Generator Initialization
// ============================================================================

/**
 * اختبار تهيئة LLVMCodeGen
 * Test LLVMCodeGen initialization
 * 
 * Source: llvm_codegen.h:186-192 - initialize() method
 */
bool testInitialization() {
    std::cout << "\n=== Test 1: Initialization ===\n";
    
    // Source: llvm_codegen.h:176 - LLVMCodeGen constructor
    LLVMCodeGen codegen;
    
    // Source: llvm_codegen.h:192 - initialize(moduleName) method
    bool initialized = codegen.initialize("test_module");
    
    if (!initialized) {
        std::cerr << "❌ Failed to initialize code generator\n";
        return false;
    }
    
    // Source: llvm_codegen.h:211 - getModule() method (line 211)
    llvm::Module* module = codegen.getModule();
    
    if (!module) {
        std::cerr << "❌ Module is null after initialization\n";
        return false;
    }
    
    std::cout << "✅ Module name: " << module->getName().str() << "\n";
    std::cout << "✅ Initialization successful\n";
    
    return true;
}

// ============================================================================
// اختبار 2: التحقق من الوحدة الفارغة
// Test 2: Verify Empty Module
// ============================================================================

/**
 * اختبار التحقق من وحدة فارغة
 * Test verifying empty module
 * 
 * Source: llvm_codegen.h:219 - verify() method
 */
bool testEmptyModuleVerification() {
    std::cout << "\n=== Test 2: Empty Module Verification ===\n";
    
    // Source: llvm_codegen.h:176 - LLVMCodeGen constructor
    LLVMCodeGen codegen;
    
    // Source: llvm_codegen.h:192 - initialize(moduleName) method
    codegen.initialize("empty_module");
    
    // Source: llvm_codegen.h:219 - verify() method
    bool valid = codegen.verify();
    
    if (!valid) {
        std::cerr << "❌ Empty module verification failed\n";
        return false;
    }
    
    std::cout << "✅ Empty module is valid\n";
    
    return true;
}

// ============================================================================
// اختبار 3: الأنواع الأساسية
// Test 3: Primitive Types
// ============================================================================

/**
 * اختبار الحصول على الأنواع الأساسية
 * Test getting primitive types
 * 
 * Source: llvm_codegen.h:569-576 - Primitive type getters
 */
bool testPrimitiveTypes() {
    std::cout << "\n=== Test 3: Primitive Types ===\n";
    
    // Source: llvm_codegen.h:176 - LLVMCodeGen constructor
    LLVMCodeGen codegen;
    
    // Source: llvm_codegen.h:192 - initialize(moduleName) method
    codegen.initialize("types_module");
    
    // Source: llvm_codegen.h:569 - getVoidType() method
    llvm::Type* voidType = codegen.getVoidType();
    if (!voidType || !voidType->isVoidTy()) {
        std::cerr << "❌ Void type is invalid\n";
        return false;
    }
    std::cout << "✅ Void type: valid\n";
    
    // Source: llvm_codegen.h:570 - getInt1Type() method
    llvm::Type* boolType = codegen.getInt1Type();
    if (!boolType || !boolType->isIntegerTy(1)) {
        std::cerr << "❌ Bool type is invalid\n";
        return false;
    }
    std::cout << "✅ Bool type (i1): valid\n";
    
    // Source: llvm_codegen.h:571 - getInt8Type() method
    llvm::Type* i8Type = codegen.getInt8Type();
    if (!i8Type || !i8Type->isIntegerTy(8)) {
        std::cerr << "❌ Int8 type is invalid\n";
        return false;
    }
    std::cout << "✅ Int8 type (i8): valid\n";
    
    // Source: llvm_codegen.h:574 - getInt64Type() method
    llvm::Type* i64Type = codegen.getInt64Type();
    if (!i64Type || !i64Type->isIntegerTy(64)) {
        std::cerr << "❌ Int64 type is invalid\n";
        return false;
    }
    std::cout << "✅ Int64 type (i64): valid\n";
    
    // Source: llvm_codegen.h:575 - getFloatType() method
    llvm::Type* floatType = codegen.getFloatType();
    if (!floatType || !floatType->isFloatTy()) {
        std::cerr << "❌ Float type is invalid\n";
        return false;
    }
    std::cout << "✅ Float type (float): valid\n";
    
    // Source: llvm_codegen.h:576 - getDoubleType() method
    llvm::Type* doubleType = codegen.getDoubleType();
    if (!doubleType || !doubleType->isDoubleTy()) {
        std::cerr << "❌ Double type is invalid\n";
        return false;
    }
    std::cout << "✅ Double type (double): valid\n";
    
    return true;
}

// ============================================================================
// اختبار 4: الثوابت
// Test 4: Constants
// ============================================================================

/**
 * اختبار إنشاء الثوابت
 * Test creating constants
 * 
 * Source: llvm_codegen.h:582-586 - Constant creation methods
 */
bool testConstants() {
    std::cout << "\n=== Test 4: Constants ===\n";
    
    // Source: llvm_codegen.h:176 - LLVMCodeGen constructor
    LLVMCodeGen codegen;
    
    // Source: llvm_codegen.h:192 - initialize(moduleName) method
    codegen.initialize("constants_module");
    
    // Source: llvm_codegen.h:582 - getConstantInt(value, bits) method
    llvm::Constant* intConst = codegen.getConstantInt(42, 64);
    if (!intConst) {
        std::cerr << "❌ Failed to create integer constant\n";
        return false;
    }
    std::cout << "✅ Integer constant (42): created\n";
    
    // Source: llvm_codegen.h:583 - getConstantFloat(value, isDouble) method
    llvm::Constant* floatConst = codegen.getConstantFloat(3.14, false);
    if (!floatConst) {
        std::cerr << "❌ Failed to create float constant\n";
        return false;
    }
    std::cout << "✅ Float constant (3.14): created\n";
    
    // Source: llvm_codegen.h:584 - getConstantString(value) method
    llvm::Constant* strConst = codegen.getConstantString("Hello, LLVM!");
    if (!strConst) {
        std::cerr << "❌ Failed to create string constant\n";
        return false;
    }
    std::cout << "✅ String constant (\"Hello, LLVM!\"): created\n";
    
    // Source: llvm_codegen.h:585 - getConstantBool(value) method
    llvm::Constant* boolConst = codegen.getConstantBool(true);
    if (!boolConst) {
        std::cerr << "❌ Failed to create boolean constant\n";
        return false;
    }
    std::cout << "✅ Boolean constant (true): created\n";
    
    return true;
}

// ============================================================================
// اختبار 5: حفظ LLVM IR إلى نص
// Test 5: Save LLVM IR to String
// ============================================================================

/**
 * اختبار حفظ LLVM IR كنص
 * Test saving LLVM IR as string
 * 
 * Source: llvm_codegen.h:604 - emitToString() method
 */
bool testEmitToString() {
    std::cout << "\n=== Test 5: Emit to String ===\n";
    
    // Source: llvm_codegen.h:176 - LLVMCodeGen constructor
    LLVMCodeGen codegen;
    
    // Source: llvm_codegen.h:192 - initialize(moduleName) method
    codegen.initialize("string_test_module");
    
    // Source: llvm_codegen.h:604 - emitToString() method
    std::string irText = codegen.emitToString();
    
    if (irText.empty()) {
        std::cerr << "❌ Generated IR text is empty\n";
        return false;
    }
    
    // التحقق من أن النص يحتوي على اسم الوحدة
    // Verify text contains module name
    if (irText.find("string_test_module") == std::string::npos) {
        std::cerr << "❌ IR text doesn't contain module name\n";
        return false;
    }
    
    std::cout << "✅ IR text generated successfully\n";
    std::cout << "   Length: " << irText.size() << " characters\n";
    
    return true;
}

// ============================================================================
// اختبار 6: معالجة الأخطاء
// Test 6: Error Handling
// ============================================================================

/**
 * اختبار معالجة الأخطاء
 * Test error handling
 * 
 * Source: llvm_codegen.h:641-653 - Error handling methods
 */
bool testErrorHandling() {
    std::cout << "\n=== Test 6: Error Handling ===\n";
    
    // Source: llvm_codegen.h:176 - LLVMCodeGen constructor
    LLVMCodeGen codegen;
    
    // Source: llvm_codegen.h:192 - initialize(moduleName) method
    codegen.initialize("error_test_module");
    
    // Source: llvm_codegen.h:648 - hasErrors() method (line 648)
    // يجب أن لا توجد أخطاء في البداية
    if (codegen.hasErrors()) {
        std::cerr << "❌ Has errors before any error reported\n";
        return false;
    }
    std::cout << "✅ No errors initially\n";
    
    // Source: llvm_codegen.h:641 - reportError(message) method
    codegen.reportError("Test error message");
    
    // Source: llvm_codegen.h:648 - hasErrors() method
    if (!codegen.hasErrors()) {
        std::cerr << "❌ Should have errors after reporting\n";
        return false;
    }
    std::cout << "✅ Error reported successfully\n";
    
    // Source: llvm_codegen.h:653 - getErrors() method
    const std::vector<std::string>& errors = codegen.getErrors();
    
    if (errors.empty()) {
        std::cerr << "❌ Error list is empty after reporting\n";
        return false;
    }
    
    if (errors[0] != "Test error message") {
        std::cerr << "❌ Error message doesn't match\n";
        return false;
    }
    
    std::cout << "✅ Error retrieved: \"" << errors[0] << "\"\n";
    
    return true;
}

// ============================================================================
// الدالة الرئيسية
// Main Function
// ============================================================================

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════╗\n";
    std::cout << "║  اختبار مولد كود LLVM - LLVM Code Generator Test      ║\n";
    std::cout << "║  Compliant with STRICT_CODING_RULES.md                  ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════╝\n";
    
    int passed = 0;
    int failed = 0;
    
    // تشغيل الاختبارات / Run tests
    if (testInitialization()) passed++; else failed++;
    if (testEmptyModuleVerification()) passed++; else failed++;
    if (testPrimitiveTypes()) passed++; else failed++;
    if (testConstants()) passed++; else failed++;
    if (testEmitToString()) passed++; else failed++;
    if (testErrorHandling()) passed++; else failed++;
    
    // النتيجة النهائية / Final results
    std::cout << "\n╔══════════════════════════════════════════════════════════╗\n";
    std::cout << "║  النتائج النهائية / Final Results                      ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════╣\n";
    std::cout << "║  ✅ Passed: " << passed << " tests                                   ║\n";
    std::cout << "║  ❌ Failed: " << failed << " tests                                   ║\n";
    std::cout << "║  📊 Total:  " << (passed + failed) << " tests                                   ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════╝\n";
    
    return (failed == 0) ? 0 : 1;
}
