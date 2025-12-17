/**
 * @file test_llvm_debug.cpp
 * @brief Comprehensive tests for LLVM Debug Info Generator
 * 
 * اختبارات شاملة لمولد معلومات التنقيح LLVM
 * 
 * Test Coverage:
 * - 3 debug levels: None, LineTablesOnly, DebugInfo
 * - 20 types: 13 basic + 3 SadLang + 4 composite
 * - DWARF generation
 * - Source location tracking
 * - Scope management
 * - Variable and function debug info
 * 
 * التغطية:
 * - ٣ مستويات تنقيح: لا شيء، جداول الأسطر فقط، معلومات تنقيح كاملة
 * - ٢٠ نوع: ١٣ أساسي + ٣ SadLang + ٤ مركب
 * - توليد DWARF
 * - تتبع موقع المصدر
 * - إدارة النطاقات
 * - معلومات تنقيح المتغيرات والدوال
 * 
 * @author SadLang Compiler Team
 * @date December 3, 2025
 */

#include "test_framework.h"
#include "test_helpers.h"
#include "../../../compiler/backends/llvm/llvm_debug.h"
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/DIBuilder.h>

// ============================================================================
// Initialization Tests / اختبارات التهيئة
// ============================================================================

/**
 * Test 1: Basic initialization
 * الاختبار ١: التهيئة الأساسية
 */
TEST(LLVMDebug, BasicInitialization) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.initialize("test.s", "/path/to/test.s");
    
    EXPECT_TRUE(debug.isInitialized());
}

/**
 * Test 2: Initialize with different debug level
 * الاختبار ٢: التهيئة بمستويات تنقيح مختلفة
 */
TEST(LLVMDebug, InitializeWithDebugLevel) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.setDebugLevel(DebugLevel::DebugInfo);
    debug.initialize("test.s", "/path/to/test.s");
    
    EXPECT_TRUE(debug.isInitialized());
}

/**
 * Test 3: Multiple initialization
 * الاختبار ٣: تهيئة متعددة
 */
TEST(LLVMDebug, MultipleInitialization) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.initialize("test1.s", "/path/to/test1.s");
    debug.initialize("test2.s", "/path/to/test2.s");
    
    EXPECT_TRUE(debug.isInitialized());
}

/**
 * Test 4: Finalize debug info
 * الاختبار ٤: إنهاء معلومات التنقيح
 */
TEST(LLVMDebug, FinalizeDebugInfo) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.initialize("test.s", "/path/to/test.s");
    debug.finalize();
    
    EXPECT_TRUE(verifyModule(module.get()));
}

// ============================================================================
// Debug Level Tests / اختبارات مستويات التنقيح
// ============================================================================

/**
 * Test 5: None debug level
 * الاختبار ٥: مستوى تنقيح: لا شيء
 */
TEST(LLVMDebug, DebugLevel_None) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.setDebugLevel(DebugLevel::None);
    debug.initialize("test.s", "/path/to/test.s");
    
    EXPECT_TRUE(debug.getDebugLevel() == DebugLevel::None);
}

/**
 * Test 6: LineTablesOnly debug level
 * الاختبار ٦: مستوى تنقيح: جداول الأسطر فقط
 */
TEST(LLVMDebug, DebugLevel_LineTablesOnly) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.setDebugLevel(DebugLevel::LineTablesOnly);
    debug.initialize("test.s", "/path/to/test.s");
    
    EXPECT_TRUE(debug.getDebugLevel() == DebugLevel::LineTablesOnly);
}

/**
 * Test 7: Full debug info level
 * الاختبار ٧: مستوى تنقيح: معلومات كاملة
 */
TEST(LLVMDebug, DebugLevel_DebugInfo) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.setDebugLevel(DebugLevel::DebugInfo);
    debug.initialize("test.s", "/path/to/test.s");
    
    EXPECT_TRUE(debug.getDebugLevel() == DebugLevel::DebugInfo);
}

// ============================================================================
// Basic Type Tests / اختبارات الأنواع الأساسية
// ============================================================================

/**
 * Test 8: Create void type
 * الاختبار ٨: إنشاء نوع void
 */
TEST(LLVMDebug, CreateVoidType) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.initialize("test.s", "/path/to/test.s");
    
    auto voidType = debug.createBasicType("void", 0, 0);
    EXPECT_NOT_NULL(voidType);
}

/**
 * Test 9: Create boolean type
 * الاختبار ٩: إنشاء نوع منطقي
 */
TEST(LLVMDebug, CreateBooleanType) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.initialize("test.s", "/path/to/test.s");
    
    auto boolType = debug.createBasicType("bool", 1, llvm::dwarf::DW_ATE_boolean);
    EXPECT_NOT_NULL(boolType);
}

/**
 * Test 10: Create integer types
 * الاختبار ١٠: إنشاء أنواع صحيحة
 */
TEST(LLVMDebug, CreateIntegerTypes) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.initialize("test.s", "/path/to/test.s");
    
    auto int8 = debug.createBasicType("i8", 8, llvm::dwarf::DW_ATE_signed);
    auto int16 = debug.createBasicType("i16", 16, llvm::dwarf::DW_ATE_signed);
    auto int32 = debug.createBasicType("i32", 32, llvm::dwarf::DW_ATE_signed);
    auto int64 = debug.createBasicType("i64", 64, llvm::dwarf::DW_ATE_signed);
    
    EXPECT_NOT_NULL(int8);
    EXPECT_NOT_NULL(int16);
    EXPECT_NOT_NULL(int32);
    EXPECT_NOT_NULL(int64);
}

/**
 * Test 11: Create unsigned integer types
 * الاختبار ١١: إنشاء أنواع صحيحة غير موقعة
 */
TEST(LLVMDebug, CreateUnsignedTypes) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.initialize("test.s", "/path/to/test.s");
    
    auto uint8 = debug.createBasicType("u8", 8, llvm::dwarf::DW_ATE_unsigned);
    auto uint16 = debug.createBasicType("u16", 16, llvm::dwarf::DW_ATE_unsigned);
    auto uint32 = debug.createBasicType("u32", 32, llvm::dwarf::DW_ATE_unsigned);
    auto uint64 = debug.createBasicType("u64", 64, llvm::dwarf::DW_ATE_unsigned);
    
    EXPECT_NOT_NULL(uint8);
    EXPECT_NOT_NULL(uint16);
    EXPECT_NOT_NULL(uint32);
    EXPECT_NOT_NULL(uint64);
}

/**
 * Test 12: Create floating-point types
 * الاختبار ١٢: إنشاء أنواع عائمة
 */
TEST(LLVMDebug, CreateFloatingPointTypes) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.initialize("test.s", "/path/to/test.s");
    
    auto float32 = debug.createBasicType("f32", 32, llvm::dwarf::DW_ATE_float);
    auto float64 = debug.createBasicType("f64", 64, llvm::dwarf::DW_ATE_float);
    
    EXPECT_NOT_NULL(float32);
    EXPECT_NOT_NULL(float64);
}

/**
 * Test 13: Create character type
 * الاختبار ١٣: إنشاء نوع حرف
 */
TEST(LLVMDebug, CreateCharacterType) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.initialize("test.s", "/path/to/test.s");
    
    auto charType = debug.createBasicType("char", 8, llvm::dwarf::DW_ATE_unsigned_char);
    EXPECT_NOT_NULL(charType);
}

// ============================================================================
// SadLang Type Tests / اختبارات أنواع SadLang
// ============================================================================

/**
 * Test 14: Create string type
 * الاختبار ١٤: إنشاء نوع نصي
 */
TEST(LLVMDebug, CreateStringType) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.initialize("test.s", "/path/to/test.s");
    
    auto stringType = debug.createStringType();
    EXPECT_NOT_NULL(stringType);
}

/**
 * Test 15: Create array type
 * الاختبار ١٥: إنشاء نوع مصفوفة
 */
TEST(LLVMDebug, CreateArrayType) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.initialize("test.s", "/path/to/test.s");
    
    auto elementType = debug.createBasicType("i32", 32, llvm::dwarf::DW_ATE_signed);
    auto arrayType = debug.createArrayType(elementType, 10);
    EXPECT_NOT_NULL(arrayType);
}

/**
 * Test 16: Create dictionary/map type
 * الاختبار ١٦: إنشاء نوع قاموس
 */
TEST(LLVMDebug, CreateDictionaryType) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.initialize("test.s", "/path/to/test.s");
    
    auto keyType = debug.createBasicType("i32", 32, llvm::dwarf::DW_ATE_signed);
    auto valueType = debug.createBasicType("i32", 32, llvm::dwarf::DW_ATE_signed);
    auto dictType = debug.createDictionaryType(keyType, valueType);
    EXPECT_NOT_NULL(dictType);
}

// ============================================================================
// Composite Type Tests / اختبارات الأنواع المركبة
// ============================================================================

/**
 * Test 17: Create pointer type
 * الاختبار ١٧: إنشاء نوع مؤشر
 */
TEST(LLVMDebug, CreatePointerType) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.initialize("test.s", "/path/to/test.s");
    
    auto baseType = debug.createBasicType("i32", 32, llvm::dwarf::DW_ATE_signed);
    auto ptrType = debug.createPointerType(baseType);
    EXPECT_NOT_NULL(ptrType);
}

/**
 * Test 18: Create structure type
 * الاختبار ١٨: إنشاء نوع بنية
 */
TEST(LLVMDebug, CreateStructureType) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.initialize("test.s", "/path/to/test.s");
    
    std::vector<llvm::Metadata*> members;
    auto structType = debug.createStructType("MyStruct", members, 64);
    EXPECT_NOT_NULL(structType);
}

/**
 * Test 19: Create class type
 * الاختبار ١٩: إنشاء نوع صنف
 */
TEST(LLVMDebug, CreateClassType) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.initialize("test.s", "/path/to/test.s");
    
    std::vector<llvm::Metadata*> members;
    auto classType = debug.createClassType("MyClass", members, 128);
    EXPECT_NOT_NULL(classType);
}

/**
 * Test 20: Create function type
 * الاختبار ٢٠: إنشاء نوع دالة
 */
TEST(LLVMDebug, CreateFunctionType) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.initialize("test.s", "/path/to/test.s");
    
    auto returnType = debug.createBasicType("i32", 32, llvm::dwarf::DW_ATE_signed);
    std::vector<llvm::Metadata*> params;
    auto funcType = debug.createFunctionType(returnType, params);
    EXPECT_NOT_NULL(funcType);
}

// ============================================================================
// Source Location Tests / اختبارات موقع المصدر
// ============================================================================

/**
 * Test 21: Set location
 * الاختبار ٢١: تحديد الموقع
 */
TEST(LLVMDebug, SetLocation) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.initialize("test.s", "/path/to/test.s");
    
    debug.setLocation(10, 5);
    auto loc = debug.getLocation();
    EXPECT_NOT_NULL(loc);
}

/**
 * Test 22: Track multiple locations
 * الاختبار ٢٢: تتبع مواقع متعددة
 */
TEST(LLVMDebug, TrackMultipleLocations) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.initialize("test.s", "/path/to/test.s");
    
    debug.setLocation(10, 5);
    auto loc1 = debug.getLocation();
    
    debug.setLocation(20, 10);
    auto loc2 = debug.getLocation();
    
    EXPECT_NOT_NULL(loc1);
    EXPECT_NOT_NULL(loc2);
}

// ============================================================================
// Scope Management Tests / اختبارات إدارة النطاقات
// ============================================================================

/**
 * Test 23: Create lexical scope
 * الاختبار ٢٣: إنشاء نطاق معجمي
 */
TEST(LLVMDebug, CreateLexicalScope) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.initialize("test.s", "/path/to/test.s");
    
    auto scope = debug.createLexicalScope(10);
    EXPECT_NOT_NULL(scope);
}

/**
 * Test 24: Push and pop scope
 * الاختبار ٢٤: دفع وسحب النطاق
 */
TEST(LLVMDebug, PushPopScope) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.initialize("test.s", "/path/to/test.s");
    
    auto scope = debug.createLexicalScope(10);
    debug.pushScope(scope);
    debug.popScope();
    
    // Should not crash
    EXPECT_TRUE(true);
}

/**
 * Test 25: Nested scopes
 * الاختبار ٢٥: نطاقات متداخلة
 */
TEST(LLVMDebug, NestedScopes) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.initialize("test.s", "/path/to/test.s");
    
    auto scope1 = debug.createLexicalScope(10);
    debug.pushScope(scope1);
    
    auto scope2 = debug.createLexicalScope(15);
    debug.pushScope(scope2);
    
    debug.popScope();
    debug.popScope();
    
    EXPECT_TRUE(true);
}

// ============================================================================
// Variable Debug Info Tests / اختبارات معلومات تنقيح المتغيرات
// ============================================================================

/**
 * Test 26: Create local variable
 * الاختبار ٢٦: إنشاء متغير محلي
 */
TEST(LLVMDebug, CreateLocalVariable) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.initialize("test.s", "/path/to/test.s");
    
    auto varType = debug.createBasicType("i32", 32, llvm::dwarf::DW_ATE_signed);
    auto var = debug.createLocalVariable("x", varType, 10);
    EXPECT_NOT_NULL(var);
}

/**
 * Test 27: Create parameter variable
 * الاختبار ٢٧: إنشاء متغير معامل
 */
TEST(LLVMDebug, CreateParameterVariable) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.initialize("test.s", "/path/to/test.s");
    
    auto paramType = debug.createBasicType("i32", 32, llvm::dwarf::DW_ATE_signed);
    auto param = debug.createParameterVariable("arg", paramType, 1, 5);
    EXPECT_NOT_NULL(param);
}

/**
 * Test 28: Create global variable
 * الاختبار ٢٨: إنشاء متغير عام
 */
TEST(LLVMDebug, CreateGlobalVariable) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.initialize("test.s", "/path/to/test.s");
    
    auto globalType = debug.createBasicType("i32", 32, llvm::dwarf::DW_ATE_signed);
    auto global = debug.createGlobalVariable("g_var", globalType, 1);
    EXPECT_NOT_NULL(global);
}

// ============================================================================
// Function Debug Info Tests / اختبارات معلومات تنقيح الدوال
// ============================================================================

/**
 * Test 29: Create function debug info
 * الاختبار ٢٩: إنشاء معلومات تنقيح للدالة
 */
TEST(LLVMDebug, CreateFunctionDebugInfo) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    auto func = createSimpleFunction(module.get(), "test_func");
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.initialize("test.s", "/path/to/test.s");
    
    auto returnType = debug.createBasicType("void", 0, 0);
    std::vector<llvm::Metadata*> params;
    auto funcType = debug.createFunctionType(returnType, params);
    
    auto funcDebug = debug.createFunction("test_func", "test_func", funcType, 10, func);
    EXPECT_NOT_NULL(funcDebug);
}

/**
 * Test 30: Create function with parameters debug info
 * الاختبار ٣٠: إنشاء معلومات تنقيح لدالة مع معاملات
 */
TEST(LLVMDebug, CreateFunctionWithParametersDebugInfo) {
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    auto func = createSimpleFunction(module.get(), "test_func_params");
    
    LLVMDebugInfoGenerator debug(module.get());
    debug.initialize("test.s", "/path/to/test.s");
    
    auto returnType = debug.createBasicType("i32", 32, llvm::dwarf::DW_ATE_signed);
    
    std::vector<llvm::Metadata*> params;
    auto paramType = debug.createBasicType("i32", 32, llvm::dwarf::DW_ATE_signed);
    params.push_back(paramType);
    
    auto funcType = debug.createFunctionType(returnType, params);
    auto funcDebug = debug.createFunction("test_func_params", "test_func_params", 
                                          funcType, 15, func);
    
    EXPECT_NOT_NULL(funcDebug);
}

// ============================================================================
// Main Function / الدالة الرئيسية
// ============================================================================

int main(int argc, char** argv) {
    return TestManager::getInstance().runAllTests(argc, argv);
}
