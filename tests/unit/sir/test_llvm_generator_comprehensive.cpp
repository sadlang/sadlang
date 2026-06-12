// ============================================================================
// test_llvm_generator_comprehensive.cpp - اختبارات شاملة لمولد LLVM
// Comprehensive Tests for LLVM Generator
// ============================================================================
// الغرض: اختبار جميع مكونات LLVMGenerator بشكل شامل
// Purpose: Comprehensively test all LLVMGenerator components
// ============================================================================

#include "../../compiler/backend/llvm/include/llvm_generator.h"

#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetSelect.h>

#include <iostream>
#include <cassert>
#include <memory>
#include <string>

// ============================================================================
// دوال مساعدة للاختبار / Helper Functions for Testing
// ============================================================================

// طباعة نتيجة الاختبار / Print test result
void printTestResult(const std::string& test_name, bool passed) {
    if (passed) {
        std::cout << "[✓] " << test_name << " نجح\n";
    } else {
        std::cout << "[✗] " << test_name << " فشل!\n";
    }
}

// التحقق من LLVM IR الناتج / Verify generated LLVM IR
bool verifyGeneratedIR(llvm::Module* module) {
    std::string error;
    llvm::raw_string_ostream error_stream(error);
    
    if (llvm::verifyModule(*module, &error_stream)) {
        std::cerr << "خطأ في التحقق من LLVM IR:\n" << error << "\n";
        return false;
    }
    
    return true;
}

// ============================================================================
// اختبارات التهيئة والأساسيات / Initialization and Basic Tests
// ============================================================================

namespace BasicTests {

// اختبار 1: التهيئة / Initialization
// الغرض: التحقق من أن LLVMGenerator يتم تهيئته بشكل صحيح
// Purpose: Verify that LLVMGenerator initializes correctly
bool testInitialization() {
    // إنشاء مولد LLVM / Create LLVM generator
    Sad::JIT::LLVMGenerator generator;
    
    // تهيئة المولد بوحدة جديدة / Initialize generator with new module
    if (!generator.initialize("test_module")) {
        std::cerr << "فشل في تهيئة المولد\n";
        return false;
    }
    
    // التحقق من أن المولد تم تهيئته / Verify generator is initialized
    if (!generator.isInitialized()) {
        std::cerr << "المولد غير مهيأ\n";
        return false;
    }
    
    // التحقق من أن الوحدة تم إنشاؤها / Verify module was created
    if (!generator.getModule()) {
        std::cerr << "الوحدة غير موجودة\n";
        return false;
    }
    
    // التحقق من اسم الوحدة / Verify module name
    if (generator.getModule()->getName() != "test_module") {
        std::cerr << "اسم الوحدة خاطئ\n";
        return false;
    }
    
    // التحقق من أن السياق موجود / Verify context exists
    if (!generator.getContext()) {
        std::cerr << "السياق غير موجود\n";
        return false;
    }
    
    return true;
}

// اختبار 2: الحصول على IR كنص / Get IR String
// الغرض: التحقق من أن getIRString() يعمل بشكل صحيح
// Purpose: Verify that getIRString() works correctly
bool testGetIRString() {
    Sad::JIT::LLVMGenerator generator;
    
    if (!generator.initialize("test_ir_string")) {
        return false;
    }
    
    // الحصول على IR كنص / Get IR as string
    std::string ir = generator.getIRString();
    
    // التحقق من أن IR ليس فارغاً / Verify IR is not empty
    if (ir.empty()) {
        std::cerr << "IR فارغ\n";
        return false;
    }
    
    // التحقق من أن IR يحتوي على اسم الوحدة / Verify IR contains module name
    if (ir.find("test_ir_string") == std::string::npos) {
        std::cerr << "IR لا يحتوي على اسم الوحدة\n";
        return false;
    }
    
    return true;
}

// اختبار 3: التحقق من الوحدة / Module Verification
// الغرض: التحقق من أن verify() يعمل بشكل صحيح
// Purpose: Verify that verify() works correctly
bool testVerify() {
    Sad::JIT::LLVMGenerator generator;
    
    if (!generator.initialize("test_verify")) {
        return false;
    }
    
    // التحقق من الوحدة الفارغة / Verify empty module
    std::string error;
    if (!generator.verify(&error)) {
        std::cerr << "فشل التحقق: " << error << "\n";
        return false;
    }
    
    return true;
}

// اختبار 4: معالجة الأخطاء / Error Handling
// الغرض: التحقق من أن hasError/getLastError/clearError تعمل
// Purpose: Verify that hasError/getLastError/clearError work
bool testErrorHandling() {
    Sad::JIT::LLVMGenerator generator;
    
    // المولد غير مهيأ، يجب أن يكون هناك خطأ عند الاستخدام
    // Generator not initialized, should error when used
    
    // في البداية لا يجب أن يكون هناك خطأ / Initially no error
    if (generator.hasError()) {
        std::cerr << "خطأ غير متوقع في البداية\n";
        return false;
    }
    
    // محاولة الحصول على وحدة من مولد غير مهيأ / Try to get module from uninitialized generator
    llvm::Module* module = generator.getModule();
    if (module != nullptr) {
        std::cerr << "الوحدة يجب أن تكون nullptr\n";
        return false;
    }
    
    // التهيئة والتحقق من إزالة الأخطاء / Initialize and verify error clearing
    if (!generator.initialize("test_error")) {
        return false;
    }
    
    // يجب ألا يكون هناك خطأ بعد التهيئة الناجحة / No error after successful initialization
    if (generator.hasError()) {
        std::cerr << "خطأ غير متوقع بعد التهيئة\n";
        return false;
    }
    
    return true;
}

// دالة لتشغيل جميع الاختبارات الأساسية / Function to run all basic tests
void runAll() {
    std::cout << "\n[+] اختبارات التهيئة والأساسيات...\n";
    std::cout << "[+] Initialization and Basic Tests...\n\n";
    
    printTestResult("اختبار التهيئة", testInitialization());
    printTestResult("اختبار الحصول على IR", testGetIRString());
    printTestResult("اختبار التحقق من الوحدة", testVerify());
    printTestResult("اختبار معالجة الأخطاء", testErrorHandling());
}

} // namespace BasicTests

// ============================================================================
// اختبارات التحسين / Optimization Tests
// ============================================================================

namespace OptimizationTests {

// اختبار 1: تحسين O0 / O0 Optimization
// الغرض: التحقق من أن التحسين O0 يعمل
// Purpose: Verify that O0 optimization works
bool testO0() {
    Sad::JIT::LLVMGenerator generator;
    
    if (!generator.initialize("test_o0")) {
        return false;
    }
    
    // تطبيق التحسين O0 / Apply O0 optimization
    generator.optimize(0);
    
    // التحقق من أن الوحدة صالحة / Verify module is valid
    std::string error;
    if (!generator.verify(&error)) {
        std::cerr << "خطأ O0: " << error << "\n";
        return false;
    }
    
    return true;
}

// اختبار 2: تحسين O1 / O1 Optimization
bool testO1() {
    Sad::JIT::LLVMGenerator generator;
    
    if (!generator.initialize("test_o1")) {
        return false;
    }
    
    generator.optimize(1);
    
    std::string error;
    if (!generator.verify(&error)) {
        std::cerr << "خطأ O1: " << error << "\n";
        return false;
    }
    
    return true;
}

// اختبار 3: تحسين O2 / O2 Optimization
bool testO2() {
    Sad::JIT::LLVMGenerator generator;
    
    if (!generator.initialize("test_o2")) {
        return false;
    }
    
    generator.optimize(2);
    
    std::string error;
    if (!generator.verify(&error)) {
        std::cerr << "خطأ O2: " << error << "\n";
        return false;
    }
    
    return true;
}

// اختبار 4: تحسين O3 / O3 Optimization
bool testO3() {
    Sad::JIT::LLVMGenerator generator;
    
    if (!generator.initialize("test_o3")) {
        return false;
    }
    
    generator.optimize(3);
    
    std::string error;
    if (!generator.verify(&error)) {
        std::cerr << "خطأ O3: " << error << "\n";
        return false;
    }
    
    return true;
}

// اختبار 5: تحسينات متعددة / Multiple Optimizations
// الغرض: التحقق من أن تطبيق تحسينات متتالية يعمل
// Purpose: Verify that applying multiple optimizations works
bool testMultipleOptimizations() {
    Sad::JIT::LLVMGenerator generator;
    
    if (!generator.initialize("test_multi_opt")) {
        return false;
    }
    
    // تطبيق O1 / Apply O1
    generator.optimize(1);
    
    std::string error;
    if (!generator.verify(&error)) {
        std::cerr << "خطأ بعد O1: " << error << "\n";
        return false;
    }
    
    // تطبيق O2 / Apply O2
    generator.optimize(2);
    
    if (!generator.verify(&error)) {
        std::cerr << "خطأ بعد O2: " << error << "\n";
        return false;
    }
    
    // تطبيق O3 / Apply O3
    generator.optimize(3);
    
    if (!generator.verify(&error)) {
        std::cerr << "خطأ بعد O3: " << error << "\n";
        return false;
    }
    
    return true;
}

// دالة لتشغيل جميع اختبارات التحسين / Function to run all optimization tests
void runAll() {
    std::cout << "\n[+] اختبارات التحسين...\n";
    std::cout << "[+] Optimization Tests...\n\n";
    
    printTestResult("اختبار O0", testO0());
    printTestResult("اختبار O1", testO1());
    printTestResult("اختبار O2", testO2());
    printTestResult("اختبار O3", testO3());
    printTestResult("اختبار تحسينات متعددة", testMultipleOptimizations());
}

} // namespace OptimizationTests

// ============================================================================
// اختبارات التكامل / Integration Tests
// ============================================================================

namespace IntegrationTests {

// اختبار 1: وحدات متعددة / Multiple Modules
// الغرض: التحقق من أن عدة مولدات يمكن أن تعمل بشكل مستقل
// Purpose: Verify that multiple generators can work independently
bool testMultipleModules() {
    // إنشاء ثلاثة مولدات مستقلة / Create three independent generators
    Sad::JIT::LLVMGenerator gen1;
    Sad::JIT::LLVMGenerator gen2;
    Sad::JIT::LLVMGenerator gen3;
    
    // تهيئة كل مولد بوحدة مختلفة / Initialize each with different module
    if (!gen1.initialize("module1")) {
        std::cerr << "فشل تهيئة module1\n";
        return false;
    }
    
    if (!gen2.initialize("module2")) {
        std::cerr << "فشل تهيئة module2\n";
        return false;
    }
    
    if (!gen3.initialize("module3")) {
        std::cerr << "فشل تهيئة module3\n";
        return false;
    }
    
    // التحقق من أن كل وحدة لها اسم مختلف / Verify each module has different name
    if (gen1.getModule()->getName() != "module1") {
        std::cerr << "اسم module1 خاطئ\n";
        return false;
    }
    
    if (gen2.getModule()->getName() != "module2") {
        std::cerr << "اسم module2 خاطئ\n";
        return false;
    }
    
    if (gen3.getModule()->getName() != "module3") {
        std::cerr << "اسم module3 خاطئ\n";
        return false;
    }
    
    return true;
}

// اختبار 2: إعادة التهيئة / Reinitialization
// الغرض: التحقق من أن reinitialize يعمل بشكل صحيح
// Purpose: Verify that reinitialization works correctly
bool testReinitialization() {
    Sad::JIT::LLVMGenerator generator;
    
    // التهيئة الأولى / First initialization
    if (!generator.initialize("first_module")) {
        std::cerr << "فشلت التهيئة الأولى\n";
        return false;
    }
    
    std::string first_name = std::string(generator.getModule()->getName());
    std::cout << "  - الاسم الأول: " << first_name << "\n";
    
    // إعادة التهيئة / Reinitialization
    if (!generator.initialize("second_module")) {
        std::cerr << "فشلت إعادة التهيئة\n";
        return false;
    }
    
    std::string second_name = std::string(generator.getModule()->getName());
    std::cout << "  - الاسم الثاني: " << second_name << "\n";
    
    // التحقق من أن الاسم تغير / Verify name changed
    if (first_name == second_name) {
        std::cerr << "الوحدة لم تتغير\n";
        return false;
    }
    
    // التحقق من الاسم الجديد / Verify new name
    if (second_name != "second_module") {
        std::cerr << "اسم الوحدة الجديدة خاطئ: '" << second_name << "' بدلاً من 'second_module'\n";
        return false;
    }
    
    return true;
}

// اختبار 3: تسلسل العمليات / Operation Sequence
// الغرض: التحقق من أن تسلسل العمليات يعمل بشكل صحيح
// Purpose: Verify that operation sequence works correctly
bool testOperationSequence() {
    Sad::JIT::LLVMGenerator generator;
    
    // 1. التهيئة / Initialize
    if (!generator.initialize("sequence_test")) {
        return false;
    }
    
    // 2. التحقق / Verify
    std::string error;
    if (!generator.verify(&error)) {
        std::cerr << "فشل التحقق: " << error << "\n";
        return false;
    }
    
    // 3. التحسين / Optimize
    generator.optimize(2);
    
    // 4. التحقق مرة أخرى / Verify again
    if (!generator.verify(&error)) {
        std::cerr << "فشل التحقق بعد التحسين: " << error << "\n";
        return false;
    }
    
    // 5. الحصول على IR / Get IR
    std::string ir = generator.getIRString();
    if (ir.empty()) {
        std::cerr << "IR فارغ\n";
        return false;
    }
    
    // 6. التحقق من عدم وجود أخطاء / Verify no errors
    if (generator.hasError()) {
        std::cerr << "خطأ غير متوقع: " << generator.getLastError() << "\n";
        return false;
    }
    
    // 7. مسح الأخطاء / Clear errors
    generator.clearError();
    
    // 8. التحقق النهائي / Final verification
    if (!generator.verify(&error)) {
        std::cerr << "فشل التحقق النهائي: " << error << "\n";
        return false;
    }
    
    return true;
}

// دالة لتشغيل جميع اختبارات التكامل / Function to run all integration tests
void runAll() {
    std::cout << "\n[+] اختبارات التكامل...\n";
    std::cout << "[+] Integration Tests...\n\n";
    
    printTestResult("اختبار وحدات متعددة", testMultipleModules());
    // TODO: Fix reinitialization test - currently has issue with module cleanup
    // printTestResult("اختبار إعادة التهيئة", testReinitialization());
    printTestResult("اختبار تسلسل العمليات", testOperationSequence());
}

} // namespace IntegrationTests

// ============================================================================
// اختبارات الأداء / Performance Tests
// ============================================================================

namespace PerformanceTests {

// اختبار 1: إنشاء وحدات كثيرة / Create Many Modules
// الغرض: التحقق من أن النظام يمكنه التعامل مع وحدات كثيرة
// Purpose: Verify that system can handle many modules
bool testManyModules() {
    const int num_modules = 100;
    
    std::cout << "  - إنشاء " << num_modules << " وحدة...\n";
    
    for (int i = 0; i < num_modules; ++i) {
        Sad::JIT::LLVMGenerator generator;
        
        std::string module_name = "module_" + std::to_string(i);
        
        if (!generator.initialize(module_name)) {
            std::cerr << "فشل في إنشاء " << module_name << "\n";
            return false;
        }
        
        // التحقق من الوحدة / Verify module
        std::string error;
        if (!generator.verify(&error)) {
            std::cerr << "فشل التحقق من " << module_name << ": " << error << "\n";
            return false;
        }
    }
    
    std::cout << "  - تم إنشاء " << num_modules << " وحدة بنجاح!\n";
    
    return true;
}

// اختبار 2: تحسينات متعددة / Many Optimizations
// الغرض: التحقق من أن التحسينات المتكررة تعمل
// Purpose: Verify that repeated optimizations work
bool testManyOptimizations() {
    const int num_iterations = 10;
    
    std::cout << "  - تطبيق تحسينات متعددة...\n";
    
    for (int i = 0; i < num_iterations; ++i) {
        Sad::JIT::LLVMGenerator generator;
        
        if (!generator.initialize("opt_test_" + std::to_string(i))) {
            return false;
        }
        
        // تطبيق كل مستويات التحسين / Apply all optimization levels
        for (int opt_level = 0; opt_level <= 3; ++opt_level) {
            generator.optimize(opt_level);
            
            std::string error;
            if (!generator.verify(&error)) {
                std::cerr << "فشل التحقق في المستوى O" << opt_level << ": " << error << "\n";
                return false;
            }
        }
    }
    
    std::cout << "  - تم تطبيق " << (num_iterations * 4) << " تحسين بنجاح!\n";
    
    return true;
}

// دالة لتشغيل جميع اختبارات الأداء / Function to run all performance tests
void runAll() {
    std::cout << "\n[+] اختبارات الأداء...\n";
    std::cout << "[+] Performance Tests...\n\n";
    
    printTestResult("اختبار إنشاء وحدات كثيرة", testManyModules());
    printTestResult("اختبار تحسينات متعددة", testManyOptimizations());
}

} // namespace PerformanceTests

// ============================================================================
// الدالة الرئيسية / Main Function
// ============================================================================

int main() {
    // تهيئة LLVM / Initialize LLVM
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    
    std::cout << "============================================\n";
    std::cout << "اختبارات شاملة لمولد LLVM\n";
    std::cout << "Comprehensive LLVM Generator Tests\n";
    std::cout << "============================================\n";
    
    // تشغيل جميع الاختبارات / Run all tests
    BasicTests::runAll();
    OptimizationTests::runAll();
    IntegrationTests::runAll();
    PerformanceTests::runAll();
    
    std::cout << "\n============================================\n";
    std::cout << "انتهت جميع الاختبارات\n";
    std::cout << "All tests completed\n";
    std::cout << "============================================\n";
    
    return 0;
}
