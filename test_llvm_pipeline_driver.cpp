/*
 * Test LLVM Pipeline Driver
 * برنامج اختبار لخط أنابيب LLVM
 * 
 * يقوم بترجمة test_llvm_pipeline.s باستخدام LLVM Pipeline
 * Compiles test_llvm_pipeline.s using LLVM Pipeline
 */

#include "compiler/pipeline/llvm/include/llvm_compiler_pipeline.h"
#include <iostream>
#include <string>
#include <chrono>

using namespace Sad::Compiler;
using namespace Sad::Compiler::LLVM;
using namespace std;

int main(int argc, char** argv) {
    cout << "========================================\n";
    cout << "اختبار خط أنابيب LLVM\n";
    cout << "LLVM Pipeline Test\n";
    cout << "========================================\n\n";
    
    // تحديد ملف المصدر / Determine source file
    string sourceFile = "test_llvm_pipeline.s";
    if (argc > 1) {
        sourceFile = argv[1];
    }
    
    cout << "ملف المصدر / Source File: " << sourceFile << "\n\n";
    
    // إنشاء خيارات الترجمة / Create compilation options
    // ⚠️ CRITICAL: Names MUST match compiler_options.h:47-356
    // Source: C:\s\s_language\compiler\include\compiler_options.h
    CompilerOptions options;
    options.optimization_level = sad::OptimizationLevel::O2;  // Line 65: sad::OptimizationLevel optimization_level
    options.emit_debug_info = true;                           // Line 116: bool emit_debug_info
    options.emit_llvm_ir = true;                              // Line 193: bool emit_llvm_ir
    options.emit_assembly = true;                             // Line 199: bool emit_assembly
    options.verbose = true;                                   // Line 212: bool verbose
    
    // إنشاء Pipeline / Create pipeline
    cout << "1. إنشاء Pipeline...\n";
    cout << "1. Creating Pipeline...\n\n";
    LLVMCompilerPipeline pipeline(options);
    
    // تهيئة / Initialize
    cout << "2. تهيئة Pipeline...\n";
    cout << "2. Initializing Pipeline...\n\n";
    if (!pipeline.initialize()) {
        cerr << "❌ فشلت التهيئة!\n";
        cerr << "❌ Initialization failed!\n";
        return 1;
    }
    cout << "✅ تمت التهيئة بنجاح\n";
    cout << "✅ Initialization succeeded\n\n";
    
    // قياس الوقت / Measure time
    auto startTime = chrono::high_resolution_clock::now();
    
    // ترجمة الملف / Compile file
    cout << "3. ترجمة الملف...\n";
    cout << "3. Compiling file...\n\n";
    CompilationResult result = pipeline.compileFile(sourceFile);
    
    auto endTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
    
    // طباعة النتيجة / Print result
    result.print();
    
    if (!result.success) {
        cerr << "\n❌ فشلت الترجمة!\n";
        cerr << "❌ Compilation failed!\n";
        return 1;
    }
    
    // إصدار LLVM IR / Emit LLVM IR
    cout << "4. إصدار LLVM IR...\n";
    cout << "4. Emitting LLVM IR...\n\n";
    if (pipeline.emitLLVMIR("test_llvm_pipeline.ll")) {
        cout << "✅ تم حفظ: test_llvm_pipeline.ll\n";
        cout << "✅ Saved: test_llvm_pipeline.ll\n\n";
    }
    
    // إصدار Assembly / Emit assembly
    cout << "5. إصدار Assembly...\n";
    cout << "5. Emitting Assembly...\n\n";
    if (pipeline.emitAssembly("test_llvm_pipeline.asm")) {
        cout << "✅ تم حفظ: test_llvm_pipeline.asm\n";
        cout << "✅ Saved: test_llvm_pipeline.asm\n\n";
    }
    
    // إصدار Object File / Emit object file
    cout << "6. إصدار Object File...\n";
    cout << "6. Emitting Object File...\n\n";
    if (pipeline.emitObjectFile("test_llvm_pipeline.obj")) {
        cout << "✅ تم حفظ: test_llvm_pipeline.obj\n";
        cout << "✅ Saved: test_llvm_pipeline.obj\n\n";
    }
    
    // إصدار Executable / Emit executable
    cout << "7. إصدار Executable...\n";
    cout << "7. Emitting Executable...\n\n";
    if (pipeline.emitExecutable("test_llvm_pipeline.exe")) {
        cout << "✅ تم حفظ: test_llvm_pipeline.exe\n";
        cout << "✅ Saved: test_llvm_pipeline.exe\n\n";
    } else {
        cerr << "⚠️ فشل إصدار Executable (ربما يحتاج linker منفصل)\n";
        cerr << "⚠️ Executable emission failed (may need separate linker)\n\n";
    }
    
    // الوقت الإجمالي / Total time
    cout << "========================================\n";
    cout << "الوقت الإجمالي / Total Time: " << duration.count() << " ms\n";
    cout << "========================================\n\n";
    
    cout << "✅ اكتمل الاختبار بنجاح!\n";
    cout << "✅ Test completed successfully!\n\n";
    
    return 0;
}
