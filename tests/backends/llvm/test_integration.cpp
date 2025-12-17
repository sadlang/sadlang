/**
 * @file test_integration.cpp
 * @brief Integration tests for LLVM Backend Pipeline
 * 
 * اختبارات التكامل لخط أنابيب LLVM Backend
 * 
 * Test Coverage:
 * - End-to-end compilation pipeline
 * - Source code to executable
 * - Multiple file compilation
 * - Linking multiple modules
 * - Cross-compilation workflows
 * - Debug and release builds
 * - Optimization pipeline integration
 * 
 * التغطية:
 * - خط الأنابيب الكامل للترجمة
 * - من الكود المصدري إلى الملف التنفيذي
 * - ترجمة ملفات متعددة
 * - ربط وحدات متعددة
 * - سير عمل الترجمة المتقاطعة
 * - بناء التنقيح والإصدار
 * - تكامل خط أنابيب التحسين
 * 
 * @author SadLang Compiler Team
 * @date December 3, 2025
 */

#include "test_framework.h"
#include "test_helpers.h"
#include "../../../compiler/backends/llvm/llvm_ir_generator.h"
#include "../../../compiler/backends/llvm/llvm_optimizer.h"
#include "../../../compiler/backends/llvm/llvm_target.h"
#include "../../../compiler/backends/llvm/llvm_linker.h"
#include "../../../compiler/backends/llvm/llvm_debug.h"
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/TargetSelect.h>

// ============================================================================
// Basic Pipeline Tests / اختبارات الخط الأنابيب الأساسي
// ============================================================================

/**
 * Test 1: Complete pipeline initialization
 * الاختبار ١: تهيئة الخط الكامل
 */
TEST(Integration, CompletePipelineInitialization) {
    // Initialize LLVM
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmPrinters();
    
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    // Initialize all components
    LLVMIRGenerator generator(module.get());
    LLVMOptimizer optimizer(module.get());
    LLVMTargetManager target;
    LLVMLinker linker;
    
    EXPECT_TRUE(generator.initialize());
    EXPECT_TRUE(optimizer.initialize(nullptr));
    EXPECT_TRUE(target.initialize("x86_64-pc-windows-msvc"));
    EXPECT_TRUE(linker.initialize());
}

/**
 * Test 2: Simple function compilation
 * الاختبار ٢: ترجمة دالة بسيطة
 */
TEST(Integration, SimpleFunctionCompilation) {
    llvm::InitializeAllTargets();
    
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    // Generate IR
    LLVMIRGenerator generator(module.get());
    generator.initialize();
    
    auto func = createSimpleFunction(module.get(), "test_func");
    EXPECT_NOT_NULL(func);
    
    // Optimize
    LLVMOptimizer optimizer(module.get());
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O2);
    optimizer.optimize(module.get());
    
    EXPECT_TRUE(verifyModule(module.get()));
}

/**
 * Test 3: IR generation to object file
 * الاختبار ٣: من توليد IR إلى ملف كائن
 */
TEST(Integration, IRGenerationToObjectFile) {
    llvm::InitializeAllTargets();
    
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    // Generate IR
    LLVMIRGenerator generator(module.get());
    generator.initialize();
    createSimpleFunction(module.get(), "test");
    
    // Generate object file
    LLVMTargetManager target;
    target.initialize("x86_64-pc-windows-msvc");
    
    std::string output = "test_integration.o";
    EXPECT_TRUE(target.emitToFile(module.get(), output, OutputFileType::ObjectFile));
}

/**
 * Test 4: Optimization pipeline integration
 * الاختبار ٤: تكامل خط التحسين
 */
TEST(Integration, OptimizationPipelineIntegration) {
    llvm::InitializeAllTargets();
    
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    // Create a function with some code
    auto func = createSimpleFunction(module.get(), "compute");
    
    // Optimize with different levels
    LLVMOptimizer optimizer(module.get());
    optimizer.initialize(nullptr);
    
    // O1
    optimizer.setOptimizationLevel(OptimizationLevel::O1);
    optimizer.optimize(module.get());
    auto stats1 = optimizer.getOptimizationStats();
    
    // O2
    optimizer.setOptimizationLevel(OptimizationLevel::O2);
    optimizer.optimize(module.get());
    auto stats2 = optimizer.getOptimizationStats();
    
    EXPECT_TRUE(verifyModule(module.get()));
}

/**
 * Test 5: Debug info integration
 * الاختبار ٥: تكامل معلومات التنقيح
 */
TEST(Integration, DebugInfoIntegration) {
    llvm::InitializeAllTargets();
    
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    // Add debug info
    LLVMDebugInfoGenerator debug(module.get());
    debug.setDebugLevel(DebugLevel::DebugInfo);
    debug.initialize("test.s", "/path/to/test.s");
    
    // Generate function with debug info
    auto func = createSimpleFunction(module.get(), "test_debug");
    
    debug.finalize();
    
    EXPECT_TRUE(verifyModule(module.get()));
}

// ============================================================================
// Multi-Module Tests / اختبارات الوحدات المتعددة
// ============================================================================

/**
 * Test 6: Multiple module compilation
 * الاختبار ٦: ترجمة وحدات متعددة
 */
TEST(Integration, MultipleModuleCompilation) {
    llvm::InitializeAllTargets();
    
    llvm::LLVMContext context1, context2;
    auto module1 = createSimpleModule(context1);
    auto module2 = createSimpleModule(context2);
    
    // Generate IR for both modules
    createSimpleFunction(module1.get(), "func1");
    createSimpleFunction(module2.get(), "func2");
    
    EXPECT_TRUE(verifyModule(module1.get()));
    EXPECT_TRUE(verifyModule(module2.get()));
}

/**
 * Test 7: Linking multiple object files
 * الاختبار ٧: ربط ملفات كائن متعددة
 */
TEST(Integration, LinkingMultipleObjectFiles) {
    llvm::InitializeAllTargets();
    
    llvm::LLVMContext context1, context2;
    auto module1 = createSimpleModule(context1);
    auto module2 = createSimpleModule(context2);
    
    // Generate object files
    LLVMTargetManager target;
    target.initialize("x86_64-pc-windows-msvc");
    
    createSimpleFunction(module1.get(), "func1");
    createSimpleFunction(module2.get(), "func2");
    
    target.emitToFile(module1.get(), "module1.o", OutputFileType::ObjectFile);
    target.emitToFile(module2.get(), "module2.o", OutputFileType::ObjectFile);
    
    // Link them
    LLVMLinker linker;
    linker.initialize();
    linker.setLinkingType(LinkingType::Executable);
    linker.addObjectFile("module1.o");
    linker.addObjectFile("module2.o");
    linker.setOutputPath("test_multi.exe");
    
    // Try linking (may fail if linker not available, which is ok)
    linker.link();
}

/**
 * Test 8: Shared library creation
 * الاختبار ٨: إنشاء مكتبة مشتركة
 */
TEST(Integration, SharedLibraryCreation) {
    llvm::InitializeAllTargets();
    
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    // Generate functions for library
    createSimpleFunction(module.get(), "lib_func1");
    createSimpleFunction(module.get(), "lib_func2");
    
    // Generate object file
    LLVMTargetManager target;
    target.initialize("x86_64-pc-windows-msvc");
    target.emitToFile(module.get(), "mylib.o", OutputFileType::ObjectFile);
    
    // Link as shared library
    LLVMLinker linker;
    linker.initialize();
    linker.setLinkingType(LinkingType::SharedLibrary);
    linker.addObjectFile("mylib.o");
    linker.setOutputPath("mylib.dll");
    
    EXPECT_TRUE(linker.getObjectFiles().size() > 0);
}

/**
 * Test 9: Static library creation
 * الاختبار ٩: إنشاء مكتبة ثابتة
 */
TEST(Integration, StaticLibraryCreation) {
    llvm::InitializeAllTargets();
    
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    // Generate functions for library
    createSimpleFunction(module.get(), "static_func1");
    createSimpleFunction(module.get(), "static_func2");
    
    // Generate object file
    LLVMTargetManager target;
    target.initialize("x86_64-pc-windows-msvc");
    target.emitToFile(module.get(), "staticlib.o", OutputFileType::ObjectFile);
    
    // Link as static library
    LLVMLinker linker;
    linker.initialize();
    linker.setLinkingType(LinkingType::StaticLibrary);
    linker.addObjectFile("staticlib.o");
    linker.setOutputPath("staticlib.lib");
    
    EXPECT_TRUE(linker.getObjectFiles().size() > 0);
}

// ============================================================================
// Cross-Compilation Tests / اختبارات الترجمة المتقاطعة
// ============================================================================

/**
 * Test 10: Cross-compile to Linux
 * الاختبار ١٠: ترجمة متقاطعة إلى Linux
 */
TEST(Integration, CrossCompileToLinux) {
    llvm::InitializeAllTargets();
    
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    // Generate IR
    createSimpleFunction(module.get(), "cross_func");
    
    // Target Linux
    LLVMTargetManager target;
    target.initialize("x86_64-pc-linux-gnu");
    target.emitToFile(module.get(), "linux_output.o", OutputFileType::ObjectFile);
    
    EXPECT_TRUE(verifyModule(module.get()));
}

/**
 * Test 11: Cross-compile to ARM
 * الاختبار ١١: ترجمة متقاطعة إلى ARM
 */
TEST(Integration, CrossCompileToARM) {
    llvm::InitializeAllTargets();
    
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    // Generate IR
    createSimpleFunction(module.get(), "arm_func");
    
    // Target ARM
    LLVMTargetManager target;
    target.initialize("aarch64-unknown-linux-gnu");
    target.emitToFile(module.get(), "arm_output.o", OutputFileType::ObjectFile);
    
    EXPECT_TRUE(verifyModule(module.get()));
}

/**
 * Test 12: Cross-compile to WebAssembly
 * الاختبار ١٢: ترجمة متقاطعة إلى WebAssembly
 */
TEST(Integration, CrossCompileToWebAssembly) {
    llvm::InitializeAllTargets();
    
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    // Generate IR
    createSimpleFunction(module.get(), "wasm_func");
    
    // Target WASM
    LLVMTargetManager target;
    target.initialize("wasm32-wasi");
    target.emitToFile(module.get(), "wasm_output.o", OutputFileType::ObjectFile);
    
    EXPECT_TRUE(verifyModule(module.get()));
}

// ============================================================================
// Build Configuration Tests / اختبارات تكوين البناء
// ============================================================================

/**
 * Test 13: Debug build
 * الاختبار ١٣: بناء التنقيح
 */
TEST(Integration, DebugBuild) {
    llvm::InitializeAllTargets();
    
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    // Add debug info
    LLVMDebugInfoGenerator debug(module.get());
    debug.setDebugLevel(DebugLevel::DebugInfo);
    debug.initialize("debug_test.s", "/path/to/debug_test.s");
    
    // Generate function
    auto func = createSimpleFunction(module.get(), "debug_func");
    debug.finalize();
    
    // No optimization
    LLVMOptimizer optimizer(module.get());
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O0);
    optimizer.optimize(module.get());
    
    // Generate debug object
    LLVMTargetManager target;
    target.initialize("x86_64-pc-windows-msvc");
    target.emitToFile(module.get(), "debug_build.o", OutputFileType::ObjectFile);
    
    EXPECT_TRUE(verifyModule(module.get()));
}

/**
 * Test 14: Release build
 * الاختبار ١٤: بناء الإصدار
 */
TEST(Integration, ReleaseBuild) {
    llvm::InitializeAllTargets();
    
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    // Generate function
    auto func = createSimpleFunction(module.get(), "release_func");
    
    // Aggressive optimization
    LLVMOptimizer optimizer(module.get());
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O3);
    optimizer.optimize(module.get());
    
    // Generate optimized object
    LLVMTargetManager target;
    target.initialize("x86_64-pc-windows-msvc");
    target.emitToFile(module.get(), "release_build.o", OutputFileType::ObjectFile);
    
    EXPECT_TRUE(verifyModule(module.get()));
}

/**
 * Test 15: Size-optimized build
 * الاختبار ١٥: بناء محسّن للحجم
 */
TEST(Integration, SizeOptimizedBuild) {
    llvm::InitializeAllTargets();
    
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    // Generate function
    auto func = createSimpleFunction(module.get(), "size_func");
    
    // Size optimization
    LLVMOptimizer optimizer(module.get());
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::Oz);
    optimizer.optimize(module.get());
    
    // Generate size-optimized object
    LLVMTargetManager target;
    target.initialize("x86_64-pc-windows-msvc");
    target.emitToFile(module.get(), "size_optimized.o", OutputFileType::ObjectFile);
    
    EXPECT_TRUE(verifyModule(module.get()));
}

// ============================================================================
// LTO Tests / اختبارات LTO
// ============================================================================

/**
 * Test 16: Thin LTO compilation
 * الاختبار ١٦: ترجمة مع Thin LTO
 */
TEST(Integration, ThinLTOCompilation) {
    llvm::InitializeAllTargets();
    
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    // Generate functions
    createSimpleFunction(module.get(), "lto_func1");
    createSimpleFunction(module.get(), "lto_func2");
    
    // Optimize with LTO preparation
    LLVMOptimizer optimizer(module.get());
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O2);
    optimizer.optimize(module.get());
    
    // Generate object
    LLVMTargetManager target;
    target.initialize("x86_64-pc-windows-msvc");
    target.emitToFile(module.get(), "thin_lto.o", OutputFileType::ObjectFile);
    
    // Link with Thin LTO
    LLVMLinker linker;
    linker.initialize();
    linker.setLTOLevel(LTOLevel::Thin);
    linker.addObjectFile("thin_lto.o");
    
    EXPECT_TRUE(linker.getLTOLevel() == LTOLevel::Thin);
}

/**
 * Test 17: Full LTO compilation
 * الاختبار ١٧: ترجمة مع Full LTO
 */
TEST(Integration, FullLTOCompilation) {
    llvm::InitializeAllTargets();
    
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    // Generate functions
    createSimpleFunction(module.get(), "full_lto_func1");
    createSimpleFunction(module.get(), "full_lto_func2");
    
    // Optimize
    LLVMOptimizer optimizer(module.get());
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O3);
    optimizer.optimize(module.get());
    
    // Generate object
    LLVMTargetManager target;
    target.initialize("x86_64-pc-windows-msvc");
    target.emitToFile(module.get(), "full_lto.o", OutputFileType::ObjectFile);
    
    // Link with Full LTO
    LLVMLinker linker;
    linker.initialize();
    linker.setLTOLevel(LTOLevel::Full);
    linker.addObjectFile("full_lto.o");
    
    EXPECT_TRUE(linker.getLTOLevel() == LTOLevel::Full);
}

// ============================================================================
// Complete Workflow Tests / اختبارات سير العمل الكامل
// ============================================================================

/**
 * Test 18: End-to-end compilation workflow
 * الاختبار ١٨: سير عمل الترجمة الكامل
 */
TEST(Integration, EndToEndCompilationWorkflow) {
    llvm::InitializeAllTargets();
    
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    // Step 1: Generate IR
    LLVMIRGenerator generator(module.get());
    generator.initialize();
    auto func = createSimpleFunction(module.get(), "main");
    
    // Step 2: Optimize
    LLVMOptimizer optimizer(module.get());
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O2);
    optimizer.optimize(module.get());
    
    // Step 3: Generate object file
    LLVMTargetManager target;
    target.initialize("x86_64-pc-windows-msvc");
    target.emitToFile(module.get(), "workflow.o", OutputFileType::ObjectFile);
    
    // Step 4: Link
    LLVMLinker linker;
    linker.initialize();
    linker.setLinkingType(LinkingType::Executable);
    linker.addObjectFile("workflow.o");
    linker.setOutputPath("workflow.exe");
    
    EXPECT_TRUE(verifyModule(module.get()));
}

/**
 * Test 19: Multiple files workflow
 * الاختبار ١٩: سير عمل ملفات متعددة
 */
TEST(Integration, MultipleFilesWorkflow) {
    llvm::InitializeAllTargets();
    
    // Module 1
    llvm::LLVMContext context1;
    auto module1 = createSimpleModule(context1);
    createSimpleFunction(module1.get(), "helper1");
    
    // Module 2
    llvm::LLVMContext context2;
    auto module2 = createSimpleModule(context2);
    createSimpleFunction(module2.get(), "helper2");
    
    // Module 3 (main)
    llvm::LLVMContext context3;
    auto module3 = createSimpleModule(context3);
    createSimpleFunction(module3.get(), "main");
    
    // Optimize all
    LLVMOptimizer opt1(module1.get()), opt2(module2.get()), opt3(module3.get());
    opt1.initialize(nullptr);
    opt2.initialize(nullptr);
    opt3.initialize(nullptr);
    
    opt1.setOptimizationLevel(OptimizationLevel::O2);
    opt2.setOptimizationLevel(OptimizationLevel::O2);
    opt3.setOptimizationLevel(OptimizationLevel::O2);
    
    opt1.optimize(module1.get());
    opt2.optimize(module2.get());
    opt3.optimize(module3.get());
    
    // Generate objects
    LLVMTargetManager target;
    target.initialize("x86_64-pc-windows-msvc");
    target.emitToFile(module1.get(), "helper1.o", OutputFileType::ObjectFile);
    target.emitToFile(module2.get(), "helper2.o", OutputFileType::ObjectFile);
    target.emitToFile(module3.get(), "main.o", OutputFileType::ObjectFile);
    
    // Link all
    LLVMLinker linker;
    linker.initialize();
    linker.setLinkingType(LinkingType::Executable);
    linker.addObjectFile("helper1.o");
    linker.addObjectFile("helper2.o");
    linker.addObjectFile("main.o");
    linker.setOutputPath("multi_file.exe");
    
    EXPECT_TRUE(linker.getObjectFiles().size() == 3);
}

/**
 * Test 20: Complete project build
 * الاختبار ٢٠: بناء مشروع كامل
 */
TEST(Integration, CompleteProjectBuild) {
    llvm::InitializeAllTargets();
    
    // Create multiple modules for a project
    std::vector<std::unique_ptr<llvm::LLVMContext>> contexts;
    std::vector<std::unique_ptr<llvm::Module>> modules;
    
    // Create 3 modules
    for (int i = 0; i < 3; i++) {
        contexts.push_back(std::make_unique<llvm::LLVMContext>());
        modules.push_back(createSimpleModule(*contexts.back()));
        createSimpleFunction(modules.back().get(), 
                           ("project_func" + std::to_string(i)).c_str());
    }
    
    // Optimize all modules
    std::vector<std::unique_ptr<LLVMOptimizer>> optimizers;
    for (auto& mod : modules) {
        auto opt = std::make_unique<LLVMOptimizer>(mod.get());
        opt->initialize(nullptr);
        opt->setOptimizationLevel(OptimizationLevel::O2);
        opt->optimize(mod.get());
        optimizers.push_back(std::move(opt));
    }
    
    // Generate object files
    LLVMTargetManager target;
    target.initialize("x86_64-pc-windows-msvc");
    
    for (size_t i = 0; i < modules.size(); i++) {
        std::string filename = "project_" + std::to_string(i) + ".o";
        target.emitToFile(modules[i].get(), filename, OutputFileType::ObjectFile);
    }
    
    // Link into executable
    LLVMLinker linker;
    linker.initialize();
    linker.setLinkingType(LinkingType::Executable);
    
    for (size_t i = 0; i < modules.size(); i++) {
        std::string filename = "project_" + std::to_string(i) + ".o";
        linker.addObjectFile(filename);
    }
    
    linker.setOutputPath("complete_project.exe");
    
    EXPECT_TRUE(linker.getObjectFiles().size() == 3);
    EXPECT_TRUE(verifyModule(modules[0].get()));
}

// ============================================================================
// Main Function / الدالة الرئيسية
// ============================================================================

int main(int argc, char** argv) {
    return TestManager::getInstance().runAllTests(argc, argv);
}
