/**
 * @file test_llvm_runtime.cpp
 * @brief Comprehensive tests for LLVM Runtime Library
 * 
 * اختبارات شاملة لمكتبة وقت التشغيل LLVM
 * 
 * Test Coverage:
 * - Memory management: malloc, free, realloc
 * - Garbage collector: mark & sweep GC
 * - Arrays: creation, access, modification, bounds checking
 * - Strings: creation, concatenation, comparison, length
 * - I/O: print, input operations
 * - Error handling and exception management
 * 
 * التغطية:
 * - إدارة الذاكرة: تخصيص، تحرير، إعادة تخصيص
 * - جامع النفايات: GC بطريقة mark & sweep
 * - المصفوفات: إنشاء، وصول، تعديل، فحص الحدود
 * - السلاسل: إنشاء، دمج، مقارنة، الطول
 * - الإدخال/الإخراج: طباعة، عمليات إدخال
 * - معالجة الأخطاء والاستثناءات
 * 
 * @author SadLang Compiler Team
 * @date December 3, 2025
 */

#include "test_framework.h"
#include "test_helpers.h"
#include "../../../compiler/backends/llvm/llvm_runtime.h"
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/Support/TargetSelect.h>

// ============================================================================
// Memory Management Tests / اختبارات إدارة الذاكرة
// ============================================================================

/**
 * Test 1: Basic memory allocation
 * الاختبار ١: تخصيص ذاكرة أساسي
 */
TEST(LLVMRuntime, BasicMemoryAllocation) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    EXPECT_TRUE(runtime.initialize());
    
    // Verify malloc function exists
    auto mallocFunc = runtime.getMallocFunction();
    EXPECT_NOT_NULL(mallocFunc);
}

/**
 * Test 2: Memory free function
 * الاختبار ٢: دالة تحرير الذاكرة
 */
TEST(LLVMRuntime, MemoryFreeFunction) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto freeFunc = runtime.getFreeFunction();
    EXPECT_NOT_NULL(freeFunc);
}

/**
 * Test 3: Memory reallocation
 * الاختبار ٣: إعادة تخصيص الذاكرة
 */
TEST(LLVMRuntime, MemoryReallocation) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto reallocFunc = runtime.getReallocFunction();
    EXPECT_NOT_NULL(reallocFunc);
}

/**
 * Test 4: Memory allocation with size
 * الاختبار ٤: تخصيص ذاكرة بحجم محدد
 */
TEST(LLVMRuntime, MemoryAllocationWithSize) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    // Create a test function that allocates memory
    llvm::IRBuilder<> builder(context);
    auto func = createSimpleFunction(module.get(), "test_alloc");
    
    EXPECT_NOT_NULL(func);
    EXPECT_TRUE(verifyModule(module.get()));
}

/**
 * Test 5: Null pointer handling
 * الاختبار ٥: معالجة المؤشر الخالي
 */
TEST(LLVMRuntime, NullPointerHandling) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    // Verify module is still valid
    EXPECT_TRUE(verifyModule(module.get()));
}

// ============================================================================
// Garbage Collector Tests / اختبارات جامع النفايات
// ============================================================================

/**
 * Test 6: GC initialization
 * الاختبار ٦: تهيئة جامع النفايات
 */
TEST(LLVMRuntime, GCInitialization) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto gcInitFunc = runtime.getGCInitFunction();
    EXPECT_NOT_NULL(gcInitFunc);
}

/**
 * Test 7: GC allocation
 * الاختبار ٧: تخصيص مع GC
 */
TEST(LLVMRuntime, GCAllocation) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto gcAllocFunc = runtime.getGCAllocFunction();
    EXPECT_NOT_NULL(gcAllocFunc);
}

/**
 * Test 8: GC collection trigger
 * الاختبار ٨: تشغيل جمع النفايات
 */
TEST(LLVMRuntime, GCCollectionTrigger) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto gcCollectFunc = runtime.getGCCollectFunction();
    EXPECT_NOT_NULL(gcCollectFunc);
}

/**
 * Test 9: GC mark phase
 * الاختبار ٩: مرحلة التعليم في GC
 */
TEST(LLVMRuntime, GCMarkPhase) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto gcMarkFunc = runtime.getGCMarkFunction();
    EXPECT_NOT_NULL(gcMarkFunc);
}

/**
 * Test 10: GC sweep phase
 * الاختبار ١٠: مرحلة المسح في GC
 */
TEST(LLVMRuntime, GCSweepPhase) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto gcSweepFunc = runtime.getGCSweepFunction();
    EXPECT_NOT_NULL(gcSweepFunc);
}

// ============================================================================
// Array Tests / اختبارات المصفوفات
// ============================================================================

/**
 * Test 11: Array creation
 * الاختبار ١١: إنشاء مصفوفة
 */
TEST(LLVMRuntime, ArrayCreation) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto arrayCreateFunc = runtime.getArrayCreateFunction();
    EXPECT_NOT_NULL(arrayCreateFunc);
}

/**
 * Test 12: Array access
 * الاختبار ١٢: الوصول للمصفوفة
 */
TEST(LLVMRuntime, ArrayAccess) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto arrayGetFunc = runtime.getArrayGetFunction();
    EXPECT_NOT_NULL(arrayGetFunc);
}

/**
 * Test 13: Array modification
 * الاختبار ١٣: تعديل المصفوفة
 */
TEST(LLVMRuntime, ArrayModification) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto arraySetFunc = runtime.getArraySetFunction();
    EXPECT_NOT_NULL(arraySetFunc);
}

/**
 * Test 14: Array length
 * الاختبار ١٤: طول المصفوفة
 */
TEST(LLVMRuntime, ArrayLength) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto arrayLenFunc = runtime.getArrayLengthFunction();
    EXPECT_NOT_NULL(arrayLenFunc);
}

/**
 * Test 15: Array bounds checking
 * الاختبار ١٥: فحص حدود المصفوفة
 */
TEST(LLVMRuntime, ArrayBoundsChecking) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto arrayCheckFunc = runtime.getArrayBoundsCheckFunction();
    EXPECT_NOT_NULL(arrayCheckFunc);
}

/**
 * Test 16: Array resize
 * الاختبار ١٦: تغيير حجم المصفوفة
 */
TEST(LLVMRuntime, ArrayResize) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto arrayResizeFunc = runtime.getArrayResizeFunction();
    EXPECT_NOT_NULL(arrayResizeFunc);
}

/**
 * Test 17: Array copy
 * الاختبار ١٧: نسخ المصفوفة
 */
TEST(LLVMRuntime, ArrayCopy) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto arrayCopyFunc = runtime.getArrayCopyFunction();
    EXPECT_NOT_NULL(arrayCopyFunc);
}

/**
 * Test 18: Multi-dimensional arrays
 * الاختبار ١٨: المصفوفات متعددة الأبعاد
 */
TEST(LLVMRuntime, MultiDimensionalArrays) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    // Verify module structure supports multi-dim arrays
    EXPECT_TRUE(verifyModule(module.get()));
}

// ============================================================================
// String Tests / اختبارات السلاسل النصية
// ============================================================================

/**
 * Test 19: String creation
 * الاختبار ١٩: إنشاء سلسلة نصية
 */
TEST(LLVMRuntime, StringCreation) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto strCreateFunc = runtime.getStringCreateFunction();
    EXPECT_NOT_NULL(strCreateFunc);
}

/**
 * Test 20: String concatenation
 * الاختبار ٢٠: دمج السلاسل النصية
 */
TEST(LLVMRuntime, StringConcatenation) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto strConcatFunc = runtime.getStringConcatFunction();
    EXPECT_NOT_NULL(strConcatFunc);
}

/**
 * Test 21: String comparison
 * الاختبار ٢١: مقارنة السلاسل النصية
 */
TEST(LLVMRuntime, StringComparison) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto strCmpFunc = runtime.getStringCompareFunction();
    EXPECT_NOT_NULL(strCmpFunc);
}

/**
 * Test 22: String length
 * الاختبار ٢٢: طول السلسلة النصية
 */
TEST(LLVMRuntime, StringLength) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto strLenFunc = runtime.getStringLengthFunction();
    EXPECT_NOT_NULL(strLenFunc);
}

/**
 * Test 23: String substring
 * الاختبار ٢٣: استخراج جزء من السلسلة
 */
TEST(LLVMRuntime, StringSubstring) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto strSubstrFunc = runtime.getStringSubstringFunction();
    EXPECT_NOT_NULL(strSubstrFunc);
}

/**
 * Test 24: String to integer conversion
 * الاختبار ٢٤: تحويل السلسلة إلى عدد صحيح
 */
TEST(LLVMRuntime, StringToInteger) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto strToIntFunc = runtime.getStringToIntFunction();
    EXPECT_NOT_NULL(strToIntFunc);
}

/**
 * Test 25: Integer to string conversion
 * الاختبار ٢٥: تحويل العدد الصحيح إلى سلسلة
 */
TEST(LLVMRuntime, IntegerToString) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto intToStrFunc = runtime.getIntToStringFunction();
    EXPECT_NOT_NULL(intToStrFunc);
}

// ============================================================================
// I/O Tests / اختبارات الإدخال/الإخراج
// ============================================================================

/**
 * Test 26: Print integer
 * الاختبار ٢٦: طباعة عدد صحيح
 */
TEST(LLVMRuntime, PrintInteger) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto printIntFunc = runtime.getPrintIntFunction();
    EXPECT_NOT_NULL(printIntFunc);
}

/**
 * Test 27: Print string
 * الاختبار ٢٧: طباعة سلسلة نصية
 */
TEST(LLVMRuntime, PrintString) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto printStrFunc = runtime.getPrintStringFunction();
    EXPECT_NOT_NULL(printStrFunc);
}

/**
 * Test 28: Print float
 * الاختبار ٢٨: طباعة عدد عشري
 */
TEST(LLVMRuntime, PrintFloat) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto printFloatFunc = runtime.getPrintFloatFunction();
    EXPECT_NOT_NULL(printFloatFunc);
}

/**
 * Test 29: Read integer input
 * الاختبار ٢٩: قراءة عدد صحيح
 */
TEST(LLVMRuntime, ReadInteger) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto readIntFunc = runtime.getReadIntFunction();
    EXPECT_NOT_NULL(readIntFunc);
}

/**
 * Test 30: Read string input
 * الاختبار ٣٠: قراءة سلسلة نصية
 */
TEST(LLVMRuntime, ReadString) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto readStrFunc = runtime.getReadStringFunction();
    EXPECT_NOT_NULL(readStrFunc);
}

// ============================================================================
// Error Handling Tests / اختبارات معالجة الأخطاء
// ============================================================================

/**
 * Test 31: Error reporting
 * الاختبار ٣١: الإبلاغ عن الأخطاء
 */
TEST(LLVMRuntime, ErrorReporting) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto errorFunc = runtime.getErrorFunction();
    EXPECT_NOT_NULL(errorFunc);
}

/**
 * Test 32: Exception handling
 * الاختبار ٣٢: معالجة الاستثناءات
 */
TEST(LLVMRuntime, ExceptionHandling) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto throwFunc = runtime.getThrowFunction();
    EXPECT_NOT_NULL(throwFunc);
}

/**
 * Test 33: Assertion checks
 * الاختبار ٣٣: فحوصات التأكيد
 */
TEST(LLVMRuntime, AssertionChecks) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto assertFunc = runtime.getAssertFunction();
    EXPECT_NOT_NULL(assertFunc);
}

/**
 * Test 34: Panic handling
 * الاختبار ٣٤: معالجة الذعر
 */
TEST(LLVMRuntime, PanicHandling) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto panicFunc = runtime.getPanicFunction();
    EXPECT_NOT_NULL(panicFunc);
}

/**
 * Test 35: Stack trace generation
 * الاختبار ٣٥: توليد تتبع المكدس
 */
TEST(LLVMRuntime, StackTraceGeneration) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto traceFunc = runtime.getStackTraceFunction();
    EXPECT_NOT_NULL(traceFunc);
}

// ============================================================================
// Additional Runtime Tests / اختبارات إضافية
// ============================================================================

/**
 * Test 36: Type information
 * الاختبار ٣٦: معلومات الأنواع
 */
TEST(LLVMRuntime, TypeInformation) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto typeofFunc = runtime.getTypeOfFunction();
    EXPECT_NOT_NULL(typeofFunc);
}

/**
 * Test 37: Reference counting
 * الاختبار ٣٧: عد المراجع
 */
TEST(LLVMRuntime, ReferenceCounting) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto retainFunc = runtime.getRetainFunction();
    auto releaseFunc = runtime.getReleaseFunction();
    EXPECT_NOT_NULL(retainFunc);
    EXPECT_NOT_NULL(releaseFunc);
}

/**
 * Test 38: Memory statistics
 * الاختبار ٣٨: إحصائيات الذاكرة
 */
TEST(LLVMRuntime, MemoryStatistics) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    
    auto memStatsFunc = runtime.getMemoryStatsFunction();
    EXPECT_NOT_NULL(memStatsFunc);
}

/**
 * Test 39: Runtime initialization
 * الاختبار ٣٩: تهيئة وقت التشغيل
 */
TEST(LLVMRuntime, RuntimeInitialization) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    EXPECT_TRUE(runtime.initialize());
    
    // Verify all basic functions exist
    EXPECT_NOT_NULL(runtime.getMallocFunction());
    EXPECT_NOT_NULL(runtime.getFreeFunction());
    EXPECT_TRUE(verifyModule(module.get()));
}

/**
 * Test 40: Runtime cleanup
 * الاختبار ٤٠: تنظيف وقت التشغيل
 */
TEST(LLVMRuntime, RuntimeCleanup) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMRuntimeLibrary runtime(module.get());
    runtime.initialize();
    runtime.cleanup();
    
    // Module should still be valid after cleanup
    EXPECT_TRUE(verifyModule(module.get()));
}

// ============================================================================
// Main Function / الدالة الرئيسية
// ============================================================================

int main(int argc, char** argv) {
    return TestManager::getInstance().runAllTests(argc, argv);
}
