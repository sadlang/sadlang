/*
 * ============================================================================
 * LLVM Pipeline Test - اختبار بسيط لـ Pipeline
 * Simple test for LLVM Compiler Pipeline
 * ============================================================================
 */

#include "../compiler/pipeline/llvm/include/llvm_compiler_pipeline.h"
#include "../compiler/include/compiler_options.h"
#include <iostream>
#include <fstream>

using namespace Sad::Compiler;

/**
 * إنشاء ملف اختبار بسيط / Create simple test file
 */
bool createTestFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "فشل إنشاء ملف الاختبار / Failed to create test file\n";
        return false;
    }
    
    // برنامج بسيط جداً / Very simple program
    file << R"(
دالة رئيسية() {
    متغير س = 42؛
    اطبع("مرحبا من LLVM Pipeline!")؛
    اطبع("القيمة:", س)؛
    أرجع 0؛
}
)";
    
    file.close();
    return true;
}

int main() {
    std::cout << "========================================\n";
    std::cout << "اختبار LLVM Compiler Pipeline\n";
    std::cout << "LLVM Compiler Pipeline Test\n";
    std::cout << "========================================\n\n";
    
    // 1. إنشاء ملف الاختبار / Create test file
    std::string testFile = "test_llvm_pipeline_temp.s";
    if (!createTestFile(testFile)) {
        return 1;
    }
    std::cout << "✓ تم إنشاء ملف الاختبار / Test file created: " << testFile << "\n\n";
    
    // 2. إعداد خيارات المترجم / Setup compiler options
    CompilerOptions options;
    options.optimization_level = sad::OptimizationLevel::O0;
    options.verbose = true;
    options.print_compilation_stats = true;
    options.emit_llvm_ir = true;
    options.emit_assembly = true;
    options.emit_object = true;
    options.verify_ir = true;
    
    std::cout << "✓ إعداد الخيارات / Options configured:\n";
    std::cout << "  - التحسين / Optimization: O0\n";
    std::cout << "  - Verbose: مفعل / Enabled\n";
    std::cout << "  - LLVM IR: مفعل / Enabled\n";
    std::cout << "  - Assembly: مفعل / Enabled\n";
    std::cout << "  - Object: مفعل / Enabled\n\n";
    
    // 3. إنشاء Pipeline / Create pipeline
    std::cout << "إنشاء LLVM Pipeline...\n";
    LLVM::LLVMCompilerPipeline pipeline(options);
    
    // 4. تهيئة / Initialize
    std::cout << "تهيئة Pipeline...\n";
    if (!pipeline.initialize()) {
        std::cerr << "❌ فشل التهيئة / Initialization failed\n";
        return 1;
    }
    std::cout << "✓ تمت التهيئة / Initialized successfully\n\n";
    
    // 5. الترجمة / Compile
    std::cout << "بدء الترجمة...\n";
    std::cout << "========================================\n\n";
    
    auto result = pipeline.compileFile(testFile);
    
    // 6. عرض النتائج / Display results
    std::cout << "\n========================================\n";
    if (result.success) {
        std::cout << "✅ نجحت الترجمة! / Compilation succeeded!\n\n";
        
        result.print();
        
        std::cout << "الملفات الناتجة / Output files:\n";
        if (!result.llvmIRFile.empty()) {
            std::cout << "  - LLVM IR: " << result.llvmIRFile << "\n";
        }
        if (!result.assemblyFile.empty()) {
            std::cout << "  - Assembly: " << result.assemblyFile << "\n";
        }
        if (!result.objectFile.empty()) {
            std::cout << "  - Object: " << result.objectFile << "\n";
        }
        
        std::cout << "\n✓ اختبار ناجح! / Test successful!\n";
        return 0;
    } else {
        std::cout << "❌ فشلت الترجمة / Compilation failed\n\n";
        result.print();
        return 1;
    }
}
