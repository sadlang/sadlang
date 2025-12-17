/*
 * ============================================================================
 * تنفيذ العمليات الثنائية للغة ص - Bitwise Operations Implementation
 * ============================================================================
 * 
 * هذا الملف يحتوي على التنفيذ الفعلي لجميع العمليات الثنائية
 * This file contains the actual implementation of all bitwise operations
 * ============================================================================
 */

#include "low_level/bitwise_ops.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace Sad {
namespace LowLevel {

// ============================================================================
// العمليات الثنائية الأساسية / Basic Bitwise Operations
// ============================================================================

uint64_t BitwiseOperations::bitwiseAnd(uint64_t a, uint64_t b) {
    return a & b;
}

uint64_t BitwiseOperations::bitwiseOr(uint64_t a, uint64_t b) {
    return a | b;
}

uint64_t BitwiseOperations::bitwiseXor(uint64_t a, uint64_t b) {
    return a ^ b;
}

uint64_t BitwiseOperations::bitwiseNot(uint64_t a) {
    return ~a;
}

// ============================================================================
// عمليات الإزاحة / Shift Operations
// ============================================================================

uint64_t BitwiseOperations::leftShift(uint64_t value, uint32_t count) {
    // Prevent undefined behavior for shifts >= 64
    // منع السلوك غير المحدد للإزاحات >= 64
    if (count >= 64) {
        return 0;
    }
    return value << count;
}

uint64_t BitwiseOperations::rightShiftLogical(uint64_t value, uint32_t count) {
    // Logical shift (fills with zeros)
    // إزاحة منطقية (تملأ بالأصفار)
    if (count >= 64) {
        return 0;
    }
    return value >> count;
}

int64_t BitwiseOperations::rightShiftArithmetic(int64_t value, uint32_t count) {
    // Arithmetic shift (preserves sign bit)
    // إزاحة حسابية (تحافظ على بت الإشارة)
    if (count >= 64) {
        return (value < 0) ? -1 : 0;
    }
    return value >> count;
}

// ============================================================================
// عمليات البت الفردي / Single Bit Operations
// ============================================================================

bool BitwiseOperations::testBit(uint64_t value, uint32_t bit) {
    if (bit >= 64) {
        return false;
    }
    return (value & (1ULL << bit)) != 0;
}

uint64_t BitwiseOperations::setBit(uint64_t value, uint32_t bit) {
    if (bit >= 64) {
        return value;
    }
    return value | (1ULL << bit);
}

uint64_t BitwiseOperations::clearBit(uint64_t value, uint32_t bit) {
    if (bit >= 64) {
        return value;
    }
    return value & ~(1ULL << bit);
}

uint64_t BitwiseOperations::toggleBit(uint64_t value, uint32_t bit) {
    if (bit >= 64) {
        return value;
    }
    return value ^ (1ULL << bit);
}

// ============================================================================
// عمليات متقدمة / Advanced Operations
// ============================================================================

uint32_t BitwiseOperations::countSetBits(uint64_t value) {
    // Brian Kernighan's algorithm
    // خوارزمية برايان كيرنيغان
    uint32_t count = 0;
    while (value) {
        value &= (value - 1);  // Clear the lowest set bit
        count++;
    }
    return count;
}

int32_t BitwiseOperations::findFirstSetBit(uint64_t value) {
    if (value == 0) {
        return -1;
    }
    
    int32_t position = 0;
    while ((value & 1) == 0) {
        value >>= 1;
        position++;
    }
    return position;
}

uint64_t BitwiseOperations::reverseBits(uint64_t value, uint32_t bits) {
    if (bits > 64) {
        bits = 64;
    }
    
    uint64_t result = 0;
    for (uint32_t i = 0; i < bits; i++) {
        if (value & (1ULL << i)) {
            result |= (1ULL << (bits - 1 - i));
        }
    }
    return result;
}

uint64_t BitwiseOperations::rotateLeft(uint64_t value, uint32_t count, uint32_t bits) {
    if (bits > 64 || bits == 0) {
        return value;
    }
    
    count %= bits;  // Normalize rotation count
    
    uint64_t mask = (bits == 64) ? ~0ULL : ((1ULL << bits) - 1);
    value &= mask;
    
    uint64_t leftPart = (value << count) & mask;
    uint64_t rightPart = (value >> (bits - count)) & mask;
    
    return leftPart | rightPart;
}

uint64_t BitwiseOperations::rotateRight(uint64_t value, uint32_t count, uint32_t bits) {
    if (bits > 64 || bits == 0) {
        return value;
    }
    
    count %= bits;  // Normalize rotation count
    
    uint64_t mask = (bits == 64) ? ~0ULL : ((1ULL << bits) - 1);
    value &= mask;
    
    uint64_t rightPart = (value >> count) & mask;
    uint64_t leftPart = (value << (bits - count)) & mask;
    
    return rightPart | leftPart;
}

// ============================================================================
// تحويلات النصوص / String Conversions
// ============================================================================

std::string BitwiseOperations::toBinaryString(uint64_t value, uint32_t bits) {
    if (bits > 64) {
        bits = 64;
    }
    
    std::string result;
    result.reserve(bits + (bits / 4));  // Reserve space including separators
    
    for (int i = bits - 1; i >= 0; i--) {
        result += (value & (1ULL << i)) ? '1' : '0';
        
        // Add separator every 4 bits for readability
        // إضافة فاصل كل 4 بتات للوضوح
        if (i > 0 && i % 4 == 0) {
            result += '_';
        }
    }
    
    return result;
}

std::string BitwiseOperations::toHexString(uint64_t value) {
    std::ostringstream oss;
    oss << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << value;
    return oss.str();
}

// ============================================================================
// وظائف الأجهزة / Hardware Functions
// ============================================================================

namespace Hardware {

uint64_t readRegister(uint32_t reg) {
    // This is a placeholder for actual hardware register reading
    // في التنفيذ الحقيقي، سيتم استخدام inline assembly
    // In real implementation, inline assembly would be used
    
    // Example for x86-64:
    // uint64_t value;
    // __asm__ volatile("mov %%rax, %0" : "=r"(value));
    
    return 0;  // Placeholder
}

void writeRegister(uint32_t reg, uint64_t value) {
    // This is a placeholder for actual hardware register writing
    // في التنفيذ الحقيقي، سيتم استخدام inline assembly
    
    // Example for x86-64:
    // __asm__ volatile("mov %0, %%rax" : : "r"(value));
}

bool readFlag(uint64_t flagReg, uint32_t bit) {
    return BitwiseOperations::testBit(flagReg, bit);
}

uint64_t setFlag(uint64_t flagReg, uint32_t bit) {
    return BitwiseOperations::setBit(flagReg, bit);
}

uint64_t clearFlag(uint64_t flagReg, uint32_t bit) {
    return BitwiseOperations::clearBit(flagReg, bit);
}

} // namespace Hardware

} // namespace LowLevel
} // namespace Sad
