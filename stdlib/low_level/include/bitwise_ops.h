/*
 * ============================================================================
 * العمليات الثنائية للغة ص - Bitwise Operations for Sad Language
 * ============================================================================
 * 
 * الوصف بالعربية:
 * ---------------
 * هذا الملف يحتوي على تعريفات العمليات الثنائية (bitwise operations).
 * هذه العمليات ضرورية لبرمجة النظام للتعامل مع الأجهزة والذاكرة
 * على مستوى البت.
 * 
 * العمليات المدعومة:
 * - AND ثنائي: & (bitwise AND)
 * - OR ثنائي: | (bitwise OR)
 * - XOR ثنائي: ^ (bitwise XOR)
 * - NOT ثنائي: ~ (bitwise NOT)
 * - الإزاحة لليسار: << (left shift)
 * - الإزاحة لليمين: >> (right shift)
 * 
 * English Description:
 * -------------------
 * This file contains bitwise operations definitions.
 * These operations are essential for system programming to handle
 * hardware and memory at the bit level.
 * 
 * Supported Operations:
 * - Bitwise AND: &
 * - Bitwise OR: |
 * - Bitwise XOR: ^
 * - Bitwise NOT: ~
 * - Left shift: <<
 * - Right shift: >>
 * 
 * المؤلف / Author: Sad Language Team
 * التاريخ / Date: December 2025
 * الإصدار / Version: 1.0.0
 * ============================================================================
 */

#ifndef SAD_BITWISE_OPS_H
#define SAD_BITWISE_OPS_H

#include <cstdint>
#include <string>

namespace Sad {
namespace LowLevel {

/**
 * نوع العملية الثنائية - Bitwise Operation Type
 */
enum class BitwiseOp {
    AND,        ///< AND ثنائي (&) / Bitwise AND
    OR,         ///< OR ثنائي (|) / Bitwise OR
    XOR,        ///< XOR ثنائي (^) / Bitwise XOR
    NOT,        ///< NOT ثنائي (~) / Bitwise NOT
    LEFT_SHIFT, ///< إزاحة لليسار (<<) / Left shift
    RIGHT_SHIFT ///< إزاحة لليمين (>>) / Right shift
};

/**
 * معالج العمليات الثنائية - Bitwise Operations Handler
 * 
 * يقوم بتنفيذ جميع العمليات الثنائية مع دعم أنواع مختلفة
 * Performs all bitwise operations with support for different types
 */
class BitwiseOperations {
public:
    /**
     * تنفيذ عملية AND ثنائية
     * Perform bitwise AND operation
     * 
     * @param a العدد الأول / First operand
     * @param b العدد الثاني / Second operand
     * @return النتيجة / Result
     */
    static uint64_t bitwiseAnd(uint64_t a, uint64_t b);
    
    /**
     * تنفيذ عملية OR ثنائية
     * Perform bitwise OR operation
     * 
     * @param a العدد الأول / First operand
     * @param b العدد الثاني / Second operand
     * @return النتيجة / Result
     */
    static uint64_t bitwiseOr(uint64_t a, uint64_t b);
    
    /**
     * تنفيذ عملية XOR ثنائية
     * Perform bitwise XOR operation
     * 
     * @param a العدد الأول / First operand
     * @param b العدد الثاني / Second operand
     * @return النتيجة / Result
     */
    static uint64_t bitwiseXor(uint64_t a, uint64_t b);
    
    /**
     * تنفيذ عملية NOT ثنائية
     * Perform bitwise NOT operation
     * 
     * @param a العدد / Operand
     * @return النتيجة / Result
     */
    static uint64_t bitwiseNot(uint64_t a);
    
    /**
     * تنفيذ إزاحة لليسار
     * Perform left shift operation
     * 
     * @param value القيمة / Value to shift
     * @param count عدد البتات / Number of bits
     * @return النتيجة / Result
     */
    static uint64_t leftShift(uint64_t value, uint32_t count);
    
    /**
     * تنفيذ إزاحة لليمين (منطقية)
     * Perform logical right shift operation
     * 
     * @param value القيمة / Value to shift
     * @param count عدد البتات / Number of bits
     * @return النتيجة / Result
     */
    static uint64_t rightShiftLogical(uint64_t value, uint32_t count);
    
    /**
     * تنفيذ إزاحة لليمين (حسابية)
     * Perform arithmetic right shift operation
     * 
     * @param value القيمة / Value to shift
     * @param count عدد البتات / Number of bits
     * @return النتيجة / Result
     */
    static int64_t rightShiftArithmetic(int64_t value, uint32_t count);
    
    /**
     * فحص بت معين
     * Test specific bit
     * 
     * @param value القيمة / Value
     * @param bit رقم البت / Bit number
     * @return true إذا كان البت 1 / if bit is 1
     */
    static bool testBit(uint64_t value, uint32_t bit);
    
    /**
     * تعيين بت معين
     * Set specific bit
     * 
     * @param value القيمة / Value
     * @param bit رقم البت / Bit number
     * @return القيمة المعدلة / Modified value
     */
    static uint64_t setBit(uint64_t value, uint32_t bit);
    
    /**
     * مسح بت معين
     * Clear specific bit
     * 
     * @param value القيمة / Value
     * @param bit رقم البت / Bit number
     * @return القيمة المعدلة / Modified value
     */
    static uint64_t clearBit(uint64_t value, uint32_t bit);
    
    /**
     * عكس بت معين
     * Toggle specific bit
     * 
     * @param value القيمة / Value
     * @param bit رقم البت / Bit number
     * @return القيمة المعدلة / Modified value
     */
    static uint64_t toggleBit(uint64_t value, uint32_t bit);
    
    /**
     * عد البتات المعينة (popcount)
     * Count set bits (popcount)
     * 
     * @param value القيمة / Value
     * @return عدد البتات المعينة / Number of set bits
     */
    static uint32_t countSetBits(uint64_t value);
    
    /**
     * إيجاد أول بت معين
     * Find first set bit
     * 
     * @param value القيمة / Value
     * @return موضع أول بت معين (-1 إذا لم يوجد) / Position of first set bit (-1 if none)
     */
    static int32_t findFirstSetBit(uint64_t value);
    
    /**
     * عكس ترتيب البتات
     * Reverse bit order
     * 
     * @param value القيمة / Value
     * @param bits عدد البتات / Number of bits
     * @return القيمة المعكوسة / Reversed value
     */
    static uint64_t reverseBits(uint64_t value, uint32_t bits);
    
    /**
     * دوران البتات لليسار
     * Rotate bits left
     * 
     * @param value القيمة / Value
     * @param count عدد المواضع / Number of positions
     * @param bits عدد البتات / Number of bits
     * @return النتيجة / Result
     */
    static uint64_t rotateLeft(uint64_t value, uint32_t count, uint32_t bits);
    
    /**
     * دوران البتات لليمين
     * Rotate bits right
     * 
     * @param value القيمة / Value
     * @param count عدد المواضع / Number of positions
     * @param bits عدد البتات / Number of bits
     * @return النتيجة / Result
     */
    static uint64_t rotateRight(uint64_t value, uint32_t count, uint32_t bits);
    
    /**
     * تحويل لتمثيل ثنائي (نص)
     * Convert to binary representation (string)
     * 
     * @param value القيمة / Value
     * @param bits عدد البتات / Number of bits
     * @return النص الثنائي / Binary string
     */
    static std::string toBinaryString(uint64_t value, uint32_t bits = 64);
    
    /**
     * تحويل لتمثيل سادس عشري (نص)
     * Convert to hexadecimal representation (string)
     * 
     * @param value القيمة / Value
     * @return النص السادس عشري / Hexadecimal string
     */
    static std::string toHexString(uint64_t value);
};

/**
 * أقنعة البتات الشائعة - Common Bit Masks
 */
namespace BitMasks {
    constexpr uint64_t BIT_0  = 0x0000000000000001ULL;
    constexpr uint64_t BIT_1  = 0x0000000000000002ULL;
    constexpr uint64_t BIT_2  = 0x0000000000000004ULL;
    constexpr uint64_t BIT_3  = 0x0000000000000008ULL;
    constexpr uint64_t BIT_4  = 0x0000000000000010ULL;
    constexpr uint64_t BIT_5  = 0x0000000000000020ULL;
    constexpr uint64_t BIT_6  = 0x0000000000000040ULL;
    constexpr uint64_t BIT_7  = 0x0000000000000080ULL;
    
    constexpr uint64_t BYTE_0 = 0x00000000000000FFULL;
    constexpr uint64_t BYTE_1 = 0x000000000000FF00ULL;
    constexpr uint64_t BYTE_2 = 0x0000000000FF0000ULL;
    constexpr uint64_t BYTE_3 = 0x00000000FF000000ULL;
    
    constexpr uint64_t WORD_0 = 0x000000000000FFFFULL;
    constexpr uint64_t WORD_1 = 0x00000000FFFF0000ULL;
    constexpr uint64_t WORD_2 = 0x0000FFFF00000000ULL;
    constexpr uint64_t WORD_3 = 0xFFFF000000000000ULL;
    
    constexpr uint64_t DWORD_0 = 0x00000000FFFFFFFFULL;
    constexpr uint64_t DWORD_1 = 0xFFFFFFFF00000000ULL;
}

/**
 * وظائف مساعدة للأجهزة - Hardware Helper Functions
 */
namespace Hardware {
    /**
     * قراءة سجل معين
     * Read specific register
     * 
     * @param reg رقم السجل / Register number
     * @return قيمة السجل / Register value
     */
    uint64_t readRegister(uint32_t reg);
    
    /**
     * كتابة لسجل معين
     * Write to specific register
     * 
     * @param reg رقم السجل / Register number
     * @param value القيمة / Value
     */
    void writeRegister(uint32_t reg, uint64_t value);
    
    /**
     * قراءة علم (flag) معين
     * Read specific flag
     * 
     * @param flagReg سجل الأعلام / Flag register
     * @param bit رقم البت / Bit number
     * @return حالة العلم / Flag status
     */
    bool readFlag(uint64_t flagReg, uint32_t bit);
    
    /**
     * تعيين علم (flag) معين
     * Set specific flag
     * 
     * @param flagReg سجل الأعلام / Flag register
     * @param bit رقم البت / Bit number
     * @return السجل المعدل / Modified register
     */
    uint64_t setFlag(uint64_t flagReg, uint32_t bit);
    
    /**
     * مسح علم (flag) معين
     * Clear specific flag
     * 
     * @param flagReg سجل الأعلام / Flag register
     * @param bit رقم البت / Bit number
     * @return السجل المعدل / Modified register
     */
    uint64_t clearFlag(uint64_t flagReg, uint32_t bit);
}

} // namespace LowLevel
} // namespace Sad

#endif // SAD_BITWISE_OPS_H
