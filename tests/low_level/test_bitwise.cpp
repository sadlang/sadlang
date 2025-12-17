/*
 * ============================================================================
 * اختبارات العمليات الثنائية - Bitwise Operations Tests
 * ============================================================================
 * 
 * الوصف بالعربية:
 * هذا الملف يحتوي على اختبارات شاملة للعمليات الثنائية
 * 
 * English Description:
 * This file contains comprehensive tests for bitwise operations
 * ============================================================================
 */

#include "low_level/bitwise_ops.h"
#include <iostream>
#include <cassert>

using namespace Sad::LowLevel;

// ============================================================================
// اختبار 1: العمليات الأساسية
// Test 1: Basic Operations
// ============================================================================
void testBasicBitwiseOperations() {
    std::cout << "=== اختبار 1: العمليات الثنائية الأساسية ===" << std::endl;
    std::cout << "=== Test 1: Basic Bitwise Operations ===" << std::endl;
    
    // AND operation
    assert(BitwiseOperations::bitwiseAnd(0b1100, 0b1010) == 0b1000);
    assert(BitwiseOperations::bitwiseAnd(0xFF, 0x0F) == 0x0F);
    
    // OR operation
    assert(BitwiseOperations::bitwiseOr(0b1100, 0b1010) == 0b1110);
    assert(BitwiseOperations::bitwiseOr(0xF0, 0x0F) == 0xFF);
    
    // XOR operation
    assert(BitwiseOperations::bitwiseXor(0b1100, 0b1010) == 0b0110);
    assert(BitwiseOperations::bitwiseXor(0xFF, 0xFF) == 0x00);
    
    // NOT operation
    assert(BitwiseOperations::bitwiseNot(0) == ~0ULL);
    assert(BitwiseOperations::bitwiseNot(0xFF) == ~0xFFULL);
    
    std::cout << "✓ اختبار العمليات الأساسية نجح" << std::endl;
    std::cout << "✓ Basic operations test passed" << std::endl;
}

// ============================================================================
// اختبار 2: عمليات الإزاحة
// Test 2: Shift Operations
// ============================================================================
void testShiftOperations() {
    std::cout << "\n=== اختبار 2: عمليات الإزاحة ===" << std::endl;
    std::cout << "=== Test 2: Shift Operations ===" << std::endl;
    
    // Left shift
    assert(BitwiseOperations::leftShift(1, 0) == 1);
    assert(BitwiseOperations::leftShift(1, 1) == 2);
    assert(BitwiseOperations::leftShift(1, 4) == 16);
    assert(BitwiseOperations::leftShift(1, 64) == 0);  // Overflow
    
    // Right shift (logical)
    assert(BitwiseOperations::rightShiftLogical(16, 1) == 8);
    assert(BitwiseOperations::rightShiftLogical(16, 4) == 1);
    assert(BitwiseOperations::rightShiftLogical(16, 64) == 0);
    
    // Right shift (arithmetic)
    assert(BitwiseOperations::rightShiftArithmetic(-16, 1) == -8);
    assert(BitwiseOperations::rightShiftArithmetic(-16, 4) == -1);
    assert(BitwiseOperations::rightShiftArithmetic(16, 4) == 1);
    
    std::cout << "✓ اختبار الإزاحة نجح" << std::endl;
    std::cout << "✓ Shift operations test passed" << std::endl;
}

// ============================================================================
// اختبار 3: عمليات البت الفردي
// Test 3: Single Bit Operations
// ============================================================================
void testSingleBitOperations() {
    std::cout << "\n=== اختبار 3: عمليات البت الفردي ===" << std::endl;
    std::cout << "=== Test 3: Single Bit Operations ===" << std::endl;
    
    uint64_t value = 0b10100000;
    
    // Test bit
    assert(BitwiseOperations::testBit(value, 5) == true);
    assert(BitwiseOperations::testBit(value, 7) == true);
    assert(BitwiseOperations::testBit(value, 0) == false);
    assert(BitwiseOperations::testBit(value, 6) == false);
    
    // Set bit
    uint64_t result = BitwiseOperations::setBit(value, 0);
    assert(result == 0b10100001);
    
    // Clear bit
    result = BitwiseOperations::clearBit(value, 5);
    assert(result == 0b10000000);
    
    // Toggle bit
    result = BitwiseOperations::toggleBit(value, 0);
    assert(result == 0b10100001);
    result = BitwiseOperations::toggleBit(value, 5);
    assert(result == 0b10000000);
    
    std::cout << "✓ اختبار عمليات البت الفردي نجح" << std::endl;
    std::cout << "✓ Single bit operations test passed" << std::endl;
}

// ============================================================================
// اختبار 4: عد البتات
// Test 4: Bit Counting
// ============================================================================
void testBitCounting() {
    std::cout << "\n=== اختبار 4: عد البتات ===" << std::endl;
    std::cout << "=== Test 4: Bit Counting ===" << std::endl;
    
    // Count set bits (popcount)
    assert(BitwiseOperations::countSetBits(0) == 0);
    assert(BitwiseOperations::countSetBits(1) == 1);
    assert(BitwiseOperations::countSetBits(0b11111111) == 8);
    assert(BitwiseOperations::countSetBits(0b10101010) == 4);
    assert(BitwiseOperations::countSetBits(~0ULL) == 64);
    
    // Find first set bit
    assert(BitwiseOperations::findFirstSetBit(0) == -1);
    assert(BitwiseOperations::findFirstSetBit(1) == 0);
    assert(BitwiseOperations::findFirstSetBit(2) == 1);
    assert(BitwiseOperations::findFirstSetBit(0b1000) == 3);
    assert(BitwiseOperations::findFirstSetBit(0b10100000) == 5);
    
    std::cout << "✓ اختبار عد البتات نجح" << std::endl;
    std::cout << "✓ Bit counting test passed" << std::endl;
}

// ============================================================================
// اختبار 5: عكس البتات
// Test 5: Bit Reversal
// ============================================================================
void testBitReversal() {
    std::cout << "\n=== اختبار 5: عكس البتات ===" << std::endl;
    std::cout << "=== Test 5: Bit Reversal ===" << std::endl;
    
    // Reverse 8 bits
    assert(BitwiseOperations::reverseBits(0b10000000, 8) == 0b00000001);
    assert(BitwiseOperations::reverseBits(0b11110000, 8) == 0b00001111);
    assert(BitwiseOperations::reverseBits(0b10101010, 8) == 0b01010101);
    
    // Reverse 4 bits
    assert(BitwiseOperations::reverseBits(0b1000, 4) == 0b0001);
    assert(BitwiseOperations::reverseBits(0b1100, 4) == 0b0011);
    
    std::cout << "✓ اختبار عكس البتات نجح" << std::endl;
    std::cout << "✓ Bit reversal test passed" << std::endl;
}

// ============================================================================
// اختبار 6: دوران البتات
// Test 6: Bit Rotation
// ============================================================================
void testBitRotation() {
    std::cout << "\n=== اختبار 6: دوران البتات ===" << std::endl;
    std::cout << "=== Test 6: Bit Rotation ===" << std::endl;
    
    // Rotate left
    assert(BitwiseOperations::rotateLeft(0b10000001, 1, 8) == 0b00000011);
    assert(BitwiseOperations::rotateLeft(0b11110000, 4, 8) == 0b00001111);
    assert(BitwiseOperations::rotateLeft(0b10101010, 1, 8) == 0b01010101);
    
    // Rotate right
    assert(BitwiseOperations::rotateRight(0b10000001, 1, 8) == 0b11000000);
    assert(BitwiseOperations::rotateRight(0b11110000, 4, 8) == 0b00001111);
    assert(BitwiseOperations::rotateRight(0b10101010, 1, 8) == 0b01010101);
    
    // Full rotation should return original
    uint64_t original = 0b10110101;
    assert(BitwiseOperations::rotateLeft(original, 8, 8) == original);
    assert(BitwiseOperations::rotateRight(original, 8, 8) == original);
    
    std::cout << "✓ اختبار دوران البتات نجح" << std::endl;
    std::cout << "✓ Bit rotation test passed" << std::endl;
}

// ============================================================================
// اختبار 7: تحويلات النصوص
// Test 7: String Conversions
// ============================================================================
void testStringConversions() {
    std::cout << "\n=== اختبار 7: تحويلات النصوص ===" << std::endl;
    std::cout << "=== Test 7: String Conversions ===" << std::endl;
    
    // Binary string
    std::string binary = BitwiseOperations::toBinaryString(0b10101010, 8);
    assert(binary == "1010_1010");
    
    binary = BitwiseOperations::toBinaryString(0xF, 4);
    assert(binary == "1111");
    
    // Hexadecimal string
    std::string hex = BitwiseOperations::toHexString(0xDEADBEEF);
    assert(hex.find("DEADBEEF") != std::string::npos);
    
    std::cout << "✓ اختبار تحويلات النصوص نجح" << std::endl;
    std::cout << "✓ String conversions test passed" << std::endl;
}

// ============================================================================
// اختبار 8: أقنعة البتات
// Test 8: Bit Masks
// ============================================================================
void testBitMasks() {
    std::cout << "\n=== اختبار 8: أقنعة البتات ===" << std::endl;
    std::cout << "=== Test 8: Bit Masks ===" << std::endl;
    
    using namespace BitMasks;
    
    // Test individual bit masks
    assert(BIT_0 == 0x0000000000000001ULL);
    assert(BIT_7 == 0x0000000000000080ULL);
    
    // Test byte masks
    assert(BYTE_0 == 0x00000000000000FFULL);
    assert((0x12345678ULL & BYTE_0) == 0x78ULL);
    
    // Test word masks
    assert(WORD_0 == 0x000000000000FFFFULL);
    assert((0x12345678ULL & WORD_0) == 0x5678ULL);
    
    // Test dword masks
    assert(DWORD_0 == 0x00000000FFFFFFFFULL);
    assert((0x123456789ABCDEFULL & DWORD_0) == 0x89ABCDEFULL);
    
    std::cout << "✓ اختبار أقنعة البتات نجح" << std::endl;
    std::cout << "✓ Bit masks test passed" << std::endl;
}

// ============================================================================
// اختبار 9: حالات الحافة
// Test 9: Edge Cases
// ============================================================================
void testEdgeCases() {
    std::cout << "\n=== اختبار 9: حالات الحافة ===" << std::endl;
    std::cout << "=== Test 9: Edge Cases ===" << std::endl;
    
    // Zero value
    assert(BitwiseOperations::bitwiseAnd(0, 0xFF) == 0);
    assert(BitwiseOperations::bitwiseOr(0, 0) == 0);
    assert(BitwiseOperations::countSetBits(0) == 0);
    
    // All bits set
    uint64_t allBits = ~0ULL;
    assert(BitwiseOperations::countSetBits(allBits) == 64);
    assert(BitwiseOperations::bitwiseAnd(allBits, allBits) == allBits);
    
    // Out of range bit operations
    assert(BitwiseOperations::testBit(0xFF, 100) == false);
    assert(BitwiseOperations::setBit(0xFF, 100) == 0xFF);
    
    // Large shift values
    assert(BitwiseOperations::leftShift(1, 100) == 0);
    assert(BitwiseOperations::rightShiftLogical(1, 100) == 0);
    
    std::cout << "✓ اختبار حالات الحافة نجح" << std::endl;
    std::cout << "✓ Edge cases test passed" << std::endl;
}

// ============================================================================
// اختبار 10: وظائف الأجهزة
// Test 10: Hardware Functions
// ============================================================================
void testHardwareFunctions() {
    std::cout << "\n=== اختبار 10: وظائف الأجهزة ===" << std::endl;
    std::cout << "=== Test 10: Hardware Functions ===" << std::endl;
    
    using namespace Hardware;
    
    uint64_t flags = 0;
    
    // Set flags
    flags = setFlag(flags, 0);
    assert(readFlag(flags, 0) == true);
    
    flags = setFlag(flags, 5);
    assert(readFlag(flags, 5) == true);
    
    // Clear flags
    flags = clearFlag(flags, 0);
    assert(readFlag(flags, 0) == false);
    
    // Flag should still be set
    assert(readFlag(flags, 5) == true);
    
    std::cout << "✓ اختبار وظائف الأجهزة نجح" << std::endl;
    std::cout << "✓ Hardware functions test passed" << std::endl;
}

// ============================================================================
// Main Test Runner
// ============================================================================
int main() {
    std::cout << "╔═══════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║   اختبارات العمليات الثنائية للغة ص                  ║" << std::endl;
    std::cout << "║   Sad Language Bitwise Operations Tests              ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════╝" << std::endl;
    
    try {
        testBasicBitwiseOperations();
        testShiftOperations();
        testSingleBitOperations();
        testBitCounting();
        testBitReversal();
        testBitRotation();
        testStringConversions();
        testBitMasks();
        testEdgeCases();
        testHardwareFunctions();
        
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
