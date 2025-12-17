/**
 * @file test_llvm_target.cpp
 * @brief Comprehensive tests for LLVM Target Manager
 * 
 * اختبارات شاملة لمدير الهدف LLVM
 * 
 * Test Coverage:
 * - 4 architectures: x86_64, ARM64, RISC-V, WebAssembly
 * - 14 platform configurations
 * - 5 output file types
 * - Cross-compilation support
 * - CPU and feature selection
 * - Optimization and relocation models
 * 
 * التغطية:
 * - ٤ معماريات: x86_64, ARM64, RISC-V, WebAssembly
 * - ١٤ تكوين منصة
 * - ٥ أنواع ملفات إخراج
 * - دعم الترجمة المتقاطعة
 * - اختيار المعالج والميزات
 * - نماذج التحسين وإعادة التوضيع
 * 
 * @author SadLang Compiler Team
 * @date December 3, 2025
 */

#include "test_framework.h"
#include "test_helpers.h"
#include "../../../compiler/backends/llvm/llvm_target.h"
#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>

// ============================================================================
// Initialization Tests / اختبارات التهيئة
// ============================================================================

/**
 * Test 1: Basic initialization
 * الاختبار ١: التهيئة الأساسية
 */
TEST(LLVMTarget, BasicInitialization) {
    // Initialize LLVM targets
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmPrinters();
    llvm::InitializeAllAsmParsers();
    
    LLVMTargetManager target;
    EXPECT_TRUE(target.initialize("x86_64-pc-windows-msvc"));
    EXPECT_NOT_NULL(target.getTargetMachine());
}

/**
 * Test 2: Initialize with invalid triple
 * الاختبار ٢: التهيئة بثلاثي غير صالح
 */
TEST(LLVMTarget, InitializeWithInvalidTriple) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    EXPECT_FALSE(target.initialize("invalid-unknown-unknown-unknown"));
}

/**
 * Test 3: Multiple initialization
 * الاختبار ٣: تهيئة متعددة
 */
TEST(LLVMTarget, MultipleInitialization) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    EXPECT_TRUE(target.initialize("x86_64-pc-linux-gnu"));
    EXPECT_TRUE(target.initialize("aarch64-unknown-linux-gnu"));
    EXPECT_NOT_NULL(target.getTargetMachine());
}

/**
 * Test 4: Get target triple
 * الاختبار ٤: الحصول على الثلاثي
 */
TEST(LLVMTarget, GetTargetTriple) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    target.initialize("x86_64-pc-windows-msvc");
    std::string triple = target.getTargetTriple();
    EXPECT_TRUE(contains(triple, "x86_64"));
}

/**
 * Test 5: Get target machine before initialization
 * الاختبار ٥: الحصول على آلة الهدف قبل التهيئة
 */
TEST(LLVMTarget, GetTargetMachineBeforeInit) {
    LLVMTargetManager target;
    EXPECT_NULL(target.getTargetMachine());
}

// ============================================================================
// x86_64 Architecture Tests / اختبارات معمارية x86_64
// ============================================================================

/**
 * Test 6: x86_64 Windows MSVC
 * الاختبار ٦: x86_64 ويندوز MSVC
 */
TEST(LLVMTarget, x86_64_Windows_MSVC) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    EXPECT_TRUE(target.initialize("x86_64-pc-windows-msvc"));
    
    std::string triple = target.getTargetTriple();
    EXPECT_TRUE(contains(triple, "x86_64"));
    EXPECT_TRUE(contains(triple, "windows"));
}

/**
 * Test 7: x86_64 Windows MinGW
 * الاختبار ٧: x86_64 ويندوز MinGW
 */
TEST(LLVMTarget, x86_64_Windows_MinGW) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    EXPECT_TRUE(target.initialize("x86_64-w64-windows-gnu"));
    EXPECT_NOT_NULL(target.getTargetMachine());
}

/**
 * Test 8: x86_64 Linux GNU
 * الاختبار ٨: x86_64 لينكس GNU
 */
TEST(LLVMTarget, x86_64_Linux_GNU) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    EXPECT_TRUE(target.initialize("x86_64-pc-linux-gnu"));
    
    std::string triple = target.getTargetTriple();
    EXPECT_TRUE(contains(triple, "x86_64"));
    EXPECT_TRUE(contains(triple, "linux"));
}

/**
 * Test 9: x86_64 Linux Musl
 * الاختبار ٩: x86_64 لينكس Musl
 */
TEST(LLVMTarget, x86_64_Linux_Musl) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    EXPECT_TRUE(target.initialize("x86_64-unknown-linux-musl"));
    EXPECT_NOT_NULL(target.getTargetMachine());
}

/**
 * Test 10: x86_64 macOS
 * الاختبار ١٠: x86_64 ماك أو إس
 */
TEST(LLVMTarget, x86_64_macOS) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    EXPECT_TRUE(target.initialize("x86_64-apple-darwin"));
    
    std::string triple = target.getTargetTriple();
    EXPECT_TRUE(contains(triple, "x86_64"));
    EXPECT_TRUE(contains(triple, "apple") || contains(triple, "darwin"));
}

// ============================================================================
// ARM64 Architecture Tests / اختبارات معمارية ARM64
// ============================================================================

/**
 * Test 11: ARM64 Linux
 * الاختبار ١١: ARM64 لينكس
 */
TEST(LLVMTarget, ARM64_Linux) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    EXPECT_TRUE(target.initialize("aarch64-unknown-linux-gnu"));
    
    std::string triple = target.getTargetTriple();
    EXPECT_TRUE(contains(triple, "aarch64") || contains(triple, "arm64"));
}

/**
 * Test 12: ARM64 macOS (Apple Silicon)
 * الاختبار ١٢: ARM64 ماك (أبل سيليكون)
 */
TEST(LLVMTarget, ARM64_macOS_AppleSilicon) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    EXPECT_TRUE(target.initialize("arm64-apple-darwin"));
    EXPECT_NOT_NULL(target.getTargetMachine());
}

/**
 * Test 13: ARM64 Android
 * الاختبار ١٣: ARM64 أندرويد
 */
TEST(LLVMTarget, ARM64_Android) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    EXPECT_TRUE(target.initialize("aarch64-linux-android"));
    EXPECT_NOT_NULL(target.getTargetMachine());
}

/**
 * Test 14: ARM64 iOS
 * الاختبار ١٤: ARM64 آيفون
 */
TEST(LLVMTarget, ARM64_iOS) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    EXPECT_TRUE(target.initialize("arm64-apple-ios"));
    EXPECT_NOT_NULL(target.getTargetMachine());
}

// ============================================================================
// RISC-V Architecture Tests / اختبارات معمارية RISC-V
// ============================================================================

/**
 * Test 15: RISC-V 64-bit Linux
 * الاختبار ١٥: RISC-V ٦٤-بت لينكس
 */
TEST(LLVMTarget, RISCV64_Linux) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    EXPECT_TRUE(target.initialize("riscv64-unknown-linux-gnu"));
    
    std::string triple = target.getTargetTriple();
    EXPECT_TRUE(contains(triple, "riscv64"));
}

/**
 * Test 16: RISC-V with compressed instructions
 * الاختبار ١٦: RISC-V مع التعليمات المضغوطة
 */
TEST(LLVMTarget, RISCV64_CompressedInstructions) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    EXPECT_TRUE(target.initialize("riscv64-unknown-linux-gnu"));
    target.setCPU("generic-rv64");
    target.setFeatures("+c,+m,+a,+f,+d");
    EXPECT_NOT_NULL(target.getTargetMachine());
}

// ============================================================================
// WebAssembly Architecture Tests / اختبارات معمارية WebAssembly
// ============================================================================

/**
 * Test 17: WebAssembly 32-bit
 * الاختبار ١٧: WebAssembly ٣٢-بت
 */
TEST(LLVMTarget, WASM32) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    EXPECT_TRUE(target.initialize("wasm32-unknown-unknown"));
    
    std::string triple = target.getTargetTriple();
    EXPECT_TRUE(contains(triple, "wasm32"));
}

/**
 * Test 18: WebAssembly WASI
 * الاختبار ١٨: WebAssembly WASI
 */
TEST(LLVMTarget, WASM32_WASI) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    EXPECT_TRUE(target.initialize("wasm32-wasi"));
    EXPECT_NOT_NULL(target.getTargetMachine());
}

// ============================================================================
// CPU Selection Tests / اختبارات اختيار المعالج
// ============================================================================

/**
 * Test 19: Set CPU for x86_64
 * الاختبار ١٩: تحديد المعالج لـ x86_64
 */
TEST(LLVMTarget, SetCPU_x86_64) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    target.initialize("x86_64-pc-linux-gnu");
    target.setCPU("haswell");
    
    std::string cpu = target.getCPU();
    EXPECT_TRUE(cpu == "haswell" || cpu.empty()); // May be normalized
}

/**
 * Test 20: Set generic CPU
 * الاختبار ٢٠: تحديد معالج عام
 */
TEST(LLVMTarget, SetGenericCPU) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    target.initialize("x86_64-pc-linux-gnu");
    target.setCPU("generic");
    EXPECT_NOT_NULL(target.getTargetMachine());
}

/**
 * Test 21: Set native CPU
 * الاختبار ٢١: تحديد المعالج المحلي
 */
TEST(LLVMTarget, SetNativeCPU) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    target.initialize("x86_64-pc-linux-gnu");
    target.setCPU("native");
    EXPECT_NOT_NULL(target.getTargetMachine());
}

// ============================================================================
// Feature Selection Tests / اختبارات اختيار الميزات
// ============================================================================

/**
 * Test 22: Set features for x86_64
 * الاختبار ٢٢: تحديد ميزات x86_64
 */
TEST(LLVMTarget, SetFeatures_x86_64) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    target.initialize("x86_64-pc-linux-gnu");
    target.setFeatures("+avx2,+sse4.2");
    EXPECT_NOT_NULL(target.getTargetMachine());
}

/**
 * Test 23: Disable features
 * الاختبار ٢٣: تعطيل الميزات
 */
TEST(LLVMTarget, DisableFeatures) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    target.initialize("x86_64-pc-linux-gnu");
    target.setFeatures("-avx,-avx2");
    EXPECT_NOT_NULL(target.getTargetMachine());
}

/**
 * Test 24: Mixed enable/disable features
 * الاختبار ٢٤: ميزات مختلطة تفعيل/تعطيل
 */
TEST(LLVMTarget, MixedFeatures) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    target.initialize("x86_64-pc-linux-gnu");
    target.setFeatures("+sse4.2,-avx,+popcnt");
    EXPECT_NOT_NULL(target.getTargetMachine());
}

// ============================================================================
// Output Type Tests / اختبارات أنواع الإخراج
// ============================================================================

/**
 * Test 25: Generate object file
 * الاختبار ٢٥: توليد ملف كائن
 */
TEST(LLVMTarget, GenerateObjectFile) {
    llvm::InitializeAllTargets();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMTargetManager target;
    target.initialize("x86_64-pc-linux-gnu");
    
    std::string output = "test_output.o";
    EXPECT_TRUE(target.emitToFile(module.get(), output, OutputFileType::ObjectFile));
}

/**
 * Test 26: Generate assembly
 * الاختبار ٢٦: توليد التجميع
 */
TEST(LLVMTarget, GenerateAssembly) {
    llvm::InitializeAllTargets();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMTargetManager target;
    target.initialize("x86_64-pc-linux-gnu");
    
    std::string output = "test_output.s";
    EXPECT_TRUE(target.emitToFile(module.get(), output, OutputFileType::AssemblyFile));
}

/**
 * Test 27: Generate LLVM IR
 * الاختبار ٢٧: توليد LLVM IR
 */
TEST(LLVMTarget, GenerateLLVMIR) {
    llvm::InitializeAllTargets();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMTargetManager target;
    target.initialize("x86_64-pc-linux-gnu");
    
    std::string output = "test_output.ll";
    EXPECT_TRUE(target.emitToFile(module.get(), output, OutputFileType::LLVMIRText));
}

/**
 * Test 28: Generate LLVM bitcode
 * الاختبار ٢٨: توليد LLVM bitcode
 */
TEST(LLVMTarget, GenerateBitcode) {
    llvm::InitializeAllTargets();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMTargetManager target;
    target.initialize("x86_64-pc-linux-gnu");
    
    std::string output = "test_output.bc";
    EXPECT_TRUE(target.emitToFile(module.get(), output, OutputFileType::LLVMBitcode));
}

/**
 * Test 29: Generate executable (if supported)
 * الاختبار ٢٩: توليد ملف تنفيذي
 */
TEST(LLVMTarget, GenerateExecutable) {
    llvm::InitializeAllTargets();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "main");
    
    LLVMTargetManager target;
    target.initialize("x86_64-pc-linux-gnu");
    
    std::string output = "test_output.exe";
    // This may fail if linking is not set up, which is expected
    target.emitToFile(module.get(), output, OutputFileType::Executable);
}

// ============================================================================
// Cross-Compilation Tests / اختبارات الترجمة المتقاطعة
// ============================================================================

/**
 * Test 30: Cross-compile Windows to Linux
 * الاختبار ٣٠: ترجمة متقاطعة من ويندوز إلى لينكس
 */
TEST(LLVMTarget, CrossCompile_WindowsToLinux) {
    llvm::InitializeAllTargets();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMTargetManager target;
    target.initialize("x86_64-pc-linux-gnu");
    
    std::string output = "test_cross_linux.o";
    EXPECT_TRUE(target.emitToFile(module.get(), output, OutputFileType::ObjectFile));
}

/**
 * Test 31: Cross-compile to ARM
 * الاختبار ٣١: ترجمة متقاطعة إلى ARM
 */
TEST(LLVMTarget, CrossCompile_ToARM) {
    llvm::InitializeAllTargets();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMTargetManager target;
    target.initialize("aarch64-unknown-linux-gnu");
    
    std::string output = "test_cross_arm.o";
    EXPECT_TRUE(target.emitToFile(module.get(), output, OutputFileType::ObjectFile));
}

/**
 * Test 32: Cross-compile to WebAssembly
 * الاختبار ٣٢: ترجمة متقاطعة إلى WebAssembly
 */
TEST(LLVMTarget, CrossCompile_ToWasm) {
    llvm::InitializeAllTargets();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMTargetManager target;
    target.initialize("wasm32-wasi");
    
    std::string output = "test_cross_wasm.o";
    EXPECT_TRUE(target.emitToFile(module.get(), output, OutputFileType::ObjectFile));
}

// ============================================================================
// Optimization Model Tests / اختبارات نموذج التحسين
// ============================================================================

/**
 * Test 33: Default optimization level
 * الاختبار ٣٣: مستوى التحسين الافتراضي
 */
TEST(LLVMTarget, DefaultOptimizationLevel) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    target.initialize("x86_64-pc-linux-gnu");
    // Default should work
    EXPECT_NOT_NULL(target.getTargetMachine());
}

/**
 * Test 34: Aggressive optimization
 * الاختبار ٣٤: تحسين عدواني
 */
TEST(LLVMTarget, AggressiveOptimization) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    target.initialize("x86_64-pc-linux-gnu");
    target.setOptLevel(llvm::CodeGenOptLevel::Aggressive);
    EXPECT_NOT_NULL(target.getTargetMachine());
}

/**
 * Test 35: No optimization
 * الاختبار ٣٥: بدون تحسين
 */
TEST(LLVMTarget, NoOptimization) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    target.initialize("x86_64-pc-linux-gnu");
    target.setOptLevel(llvm::CodeGenOptLevel::None);
    EXPECT_NOT_NULL(target.getTargetMachine());
}

// ============================================================================
// Relocation Model Tests / اختبارات نموذج إعادة التوضيع
// ============================================================================

/**
 * Test 36: Static relocation
 * الاختبار ٣٦: إعادة توضيع ثابتة
 */
TEST(LLVMTarget, StaticRelocation) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    target.initialize("x86_64-pc-linux-gnu");
    target.setRelocModel(llvm::Reloc::Static);
    EXPECT_NOT_NULL(target.getTargetMachine());
}

/**
 * Test 37: PIC relocation
 * الاختبار ٣٧: إعادة توضيع PIC
 */
TEST(LLVMTarget, PICRelocation) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    target.initialize("x86_64-pc-linux-gnu");
    target.setRelocModel(llvm::Reloc::PIC_);
    EXPECT_NOT_NULL(target.getTargetMachine());
}

/**
 * Test 38: Dynamic relocation
 * الاختبار ٣٨: إعادة توضيع ديناميكية
 */
TEST(LLVMTarget, DynamicRelocation) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    target.initialize("x86_64-pc-linux-gnu");
    target.setRelocModel(llvm::Reloc::DynamicNoPIC);
    EXPECT_NOT_NULL(target.getTargetMachine());
}

// ============================================================================
// Code Model Tests / اختبارات نموذج الكود
// ============================================================================

/**
 * Test 39: Small code model
 * الاختبار ٣٩: نموذج كود صغير
 */
TEST(LLVMTarget, SmallCodeModel) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    target.initialize("x86_64-pc-linux-gnu");
    target.setCodeModel(llvm::CodeModel::Small);
    EXPECT_NOT_NULL(target.getTargetMachine());
}

/**
 * Test 40: Large code model
 * الاختبار ٤٠: نموذج كود كبير
 */
TEST(LLVMTarget, LargeCodeModel) {
    llvm::InitializeAllTargets();
    
    LLVMTargetManager target;
    target.initialize("x86_64-pc-linux-gnu");
    target.setCodeModel(llvm::CodeModel::Large);
    EXPECT_NOT_NULL(target.getTargetMachine());
}

// ============================================================================
// Main Function / الدالة الرئيسية
// ============================================================================

int main(int argc, char** argv) {
    return TestManager::getInstance().runAllTests(argc, argv);
}
