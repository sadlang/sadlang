/*
 * ============================================================================
 * اختبارات نظام المؤشرات - Pointer System Tests
 * ============================================================================
 * 
 * الوصف بالعربية:
 * هذا الملف يحتوي على اختبارات شاملة لنظام المؤشرات
 * 
 * English Description:
 * This file contains comprehensive tests for the pointer system
 * ============================================================================
 */

#include "low_level/pointer_type.h"
#include <iostream>
#include <cassert>
#include <cstring>

using namespace Sad::LowLevel;

// ============================================================================
// اختبار 1: إنشاء المؤشرات الأساسية
// Test 1: Basic Pointer Creation
// ============================================================================
void testBasicPointerCreation() {
    std::cout << "=== اختبار 1: إنشاء المؤشرات الأساسية ===" << std::endl;
    std::cout << "=== Test 1: Basic Pointer Creation ===" << std::endl;
    
    int value = 42;
    Type* intType = new NumberType();  // Assuming NumberType exists
    
    auto* ptrMgr = PointerManager::getInstance();
    PointerInfo ptr = ptrMgr->createPointer(&value, intType);
    
    assert(ptr.isValid);
    assert(ptr.address == &value);
    assert(ptr.pointeeType == intType);
    
    std::cout << "✓ اختبار إنشاء المؤشر نجح" << std::endl;
    std::cout << "✓ Pointer creation test passed" << std::endl;
}

// ============================================================================
// اختبار 2: إلغاء مرجعية المؤشر
// Test 2: Pointer Dereferencing
// ============================================================================
void testPointerDereference() {
    std::cout << "\n=== اختبار 2: إلغاء مرجعية المؤشر ===" << std::endl;
    std::cout << "=== Test 2: Pointer Dereferencing ===" << std::endl;
    
    int value = 100;
    Type* intType = new NumberType();
    
    auto* ptrMgr = PointerManager::getInstance();
    PointerInfo ptr = ptrMgr->createPointer(&value, intType);
    
    void* derefed = ptrMgr->dereference(ptr);
    assert(derefed == &value);
    assert(*(int*)derefed == 100);
    
    std::cout << "✓ اختبار إلغاء المرجعية نجح" << std::endl;
    std::cout << "✓ Dereferencing test passed" << std::endl;
}

// ============================================================================
// اختبار 3: مؤشرات NULL
// Test 3: NULL Pointers
// ============================================================================
void testNullPointer() {
    std::cout << "\n=== اختبار 3: مؤشرات NULL ===" << std::endl;
    std::cout << "=== Test 3: NULL Pointers ===" << std::endl;
    
    Type* intType = new NumberType();
    auto* ptrMgr = PointerManager::getInstance();
    
    PointerInfo nullPtr = ptrMgr->createPointer(nullptr, intType);
    assert(!nullPtr.isValid);
    
    try {
        ptrMgr->dereference(nullPtr);
        assert(false && "Should have thrown exception");
    } catch (const std::runtime_error& e) {
        std::cout << "✓ اختبار NULL pointer نجح - تم اكتشاف الخطأ" << std::endl;
        std::cout << "✓ NULL pointer test passed - error detected" << std::endl;
    }
}

// ============================================================================
// اختبار 4: حساب المؤشر (Pointer Arithmetic)
// Test 4: Pointer Arithmetic
// ============================================================================
void testPointerArithmetic() {
    std::cout << "\n=== اختبار 4: حساب المؤشر ===" << std::endl;
    std::cout << "=== Test 4: Pointer Arithmetic ===" << std::endl;
    
    int array[] = {10, 20, 30, 40, 50};
    Type* intType = new NumberType();
    
    auto* ptrMgr = PointerManager::getInstance();
    PointerInfo ptr = ptrMgr->createPointer(&array[0], intType);
    
    // Test pointer addition
    // اختبار إضافة المؤشر
    PointerInfo ptr2 = ptrMgr->pointerAdd(ptr, 2);
    assert(*(int*)ptr2.address == 30);
    
    // Test pointer subtraction
    // اختبار طرح المؤشر
    PointerInfo ptr3 = ptrMgr->pointerSub(ptr2, 1);
    assert(*(int*)ptr3.address == 20);
    
    std::cout << "✓ اختبار حساب المؤشر نجح" << std::endl;
    std::cout << "✓ Pointer arithmetic test passed" << std::endl;
}

// ============================================================================
// اختبار 5: توافق أنواع المؤشرات
// Test 5: Pointer Type Compatibility
// ============================================================================
void testPointerTypeCompatibility() {
    std::cout << "\n=== اختبار 5: توافق أنواع المؤشرات ===" << std::endl;
    std::cout << "=== Test 5: Pointer Type Compatibility ===" << std::endl;
    
    Type* intType = new NumberType();
    Type* floatType = new NumberType();  // Assuming different
    Type* voidType = new VoidType();
    
    PointerType intPtr(intType);
    PointerType floatPtr(floatType);
    PointerType voidPtr(voidType);
    
    // Same type should be compatible
    // نفس النوع يجب أن يكون متوافقاً
    assert(intPtr.isCompatibleWith(&intPtr));
    
    // void* should be compatible with any pointer
    // void* يجب أن يكون متوافقاً مع أي مؤشر
    assert(voidPtr.isCompatibleWith(&intPtr));
    assert(intPtr.isCompatibleWith(&voidPtr));
    
    std::cout << "✓ اختبار توافق الأنواع نجح" << std::endl;
    std::cout << "✓ Type compatibility test passed" << std::endl;
}

// ============================================================================
// اختبار 6: مؤشرات متعددة المستويات
// Test 6: Multi-level Pointers
// ============================================================================
void testMultiLevelPointers() {
    std::cout << "\n=== اختبار 6: مؤشرات متعددة المستويات ===" << std::endl;
    std::cout << "=== Test 6: Multi-level Pointers ===" << std::endl;
    
    int value = 123;
    int* ptr1 = &value;
    int** ptr2 = &ptr1;
    
    Type* intType = new NumberType();
    Type* ptrType = new PointerType(intType);
    
    auto* ptrMgr = PointerManager::getInstance();
    
    // Create pointer to pointer
    // إنشاء مؤشر لمؤشر
    PointerInfo ptrToPtr = ptrMgr->createPointer(ptr2, ptrType);
    assert(ptrToPtr.isValid);
    
    std::cout << "✓ اختبار المؤشرات متعددة المستويات نجح" << std::endl;
    std::cout << "✓ Multi-level pointers test passed" << std::endl;
}

// ============================================================================
// اختبار 7: أمان المؤشرات
// Test 7: Pointer Safety
// ============================================================================
void testPointerSafety() {
    std::cout << "\n=== اختبار 7: أمان المؤشرات ===" << std::endl;
    std::cout << "=== Test 7: Pointer Safety ===" << std::endl;
    
    Type* intType = new NumberType();
    auto* ptrMgr = PointerManager::getInstance();
    
    // Test invalid pointer
    // اختبار مؤشر غير صالح
    PointerInfo invalidPtr;
    invalidPtr.isValid = false;
    
    assert(!ptrMgr->validatePointer(invalidPtr));
    
    std::cout << "✓ اختبار أمان المؤشرات نجح" << std::endl;
    std::cout << "✓ Pointer safety test passed" << std::endl;
}

// ============================================================================
// Main Test Runner
// ============================================================================
int main() {
    std::cout << "╔═══════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║   اختبارات نظام المؤشرات للغة ص                      ║" << std::endl;
    std::cout << "║   Sad Language Pointer System Tests                  ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════╝" << std::endl;
    
    try {
        testBasicPointerCreation();
        testPointerDereference();
        testNullPointer();
        testPointerArithmetic();
        testPointerTypeCompatibility();
        testMultiLevelPointers();
        testPointerSafety();
        
        std::cout << "\n╔═══════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║   ✓ جميع الاختبارات نجحت!                            ║" << std::endl;
        std::cout << "║   ✓ All tests passed!                                ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════╝" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ فشل الاختبار: " << e.what() << std::endl;
        std::cerr << "✗ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
