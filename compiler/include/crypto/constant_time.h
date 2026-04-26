// ======================================================================
// constant_time.h - عمليات ثابتة الوقت / Constant-Time Operations
// ======================================================================
// الوصف بالعربية:
//   عمليات تشفيرية ثابتة الوقت لمنع هجمات التوقيت (timing attacks)
//   ضرورية لنظام التشغيل الآمن
//
// English Description:
//   Constant-time cryptographic operations to prevent timing attacks
//   Essential for a secure operating system
// ======================================================================

#ifndef SAD_CONSTANT_TIME_H
#define SAD_CONSTANT_TIME_H

#include <string>
#include <cstdint>

namespace Sad {
namespace Crypto {

/**
 * @brief Constant-time operation types / أنواع العمليات ثابتة الوقت
 */
enum class ConstTimeOp {
    Compare,        ///< مقارنة_آمنة / safe compare — constant-time comparison
    Select,         ///< اختيار_آمن / safe select — constant-time conditional select
    ZeroCheck,      ///< فحص_صفر_آمن / safe zero check
    Copy,           ///< نسخ_آمن / safe copy — constant-time copy
    Xor,            ///< حصري_آمن / safe XOR
    And,            ///< و_آمن / safe AND
    Or              ///< أو_آمن / safe OR
};

/**
 * @brief Constant-time operations helper / مساعد العمليات ثابتة الوقت
 * 
 * (AR) جميع العمليات تستغرق نفس الوقت بغض النظر عن القيم
 * (EN) All operations take the same time regardless of values
 * 
 * @example صيغة ص:
 * - إذا مقارنة_آمنة(كلمة_المرور، المتوقعة) { ... }
 * - دع النتيجة = اختيار_آمن(شرط، قيمة1، قيمة2)
 */
class ConstantTimeOps {
public:
    /**
     * @brief Constant-time byte comparison
     * (AR) مقارنة بايتات ثابتة الوقت — لا تتوقف مبكراً عند اختلاف
     * (EN) Constant-time byte comparison — never short-circuits
     */
    static bool secureCompare(const uint8_t* a, const uint8_t* b, size_t len) {
        volatile uint8_t result = 0;
        for (size_t i = 0; i < len; ++i) {
            result |= a[i] ^ b[i];
        }
        return result == 0;
    }
    
    /**
     * @brief Constant-time select
     * (AR) اختيار ثابت الوقت — يختار a إذا condition=1، b إذا condition=0
     * (EN) Constant-time select — selects a if condition=1, b if condition=0
     */
    static uint64_t secureSelect(uint64_t condition, uint64_t a, uint64_t b) {
        // condition must be 0 or 1
        uint64_t mask = ~(condition - 1);  // 0xFFF...F if 1, 0x000...0 if 0
        return (mask & a) | (~mask & b);
    }
    
    /**
     * @brief Constant-time zero check  
     * (AR) فحص الصفر ثابت الوقت
     * (EN) Constant-time check if value is zero
     */
    static uint64_t secureIsZero(uint64_t value) {
        // Returns 1 if zero, 0 if non-zero
        return 1 ^ ((value | (~value + 1)) >> 63);
    }
    
    /**
     * @brief Constant-time conditional swap
     * (AR) تبديل شرطي ثابت الوقت
     * (EN) Constant-time conditional swap
     */
    static void secureSwap(uint64_t& a, uint64_t& b, uint64_t condition) {
        uint64_t mask = ~(condition - 1);
        uint64_t t = mask & (a ^ b);
        a ^= t;
        b ^= t;
    }
    
    /**
     * @brief Generate LLVM IR for constant-time compare
     * (AR) توليد LLVM IR لمقارنة ثابتة الوقت
     */
    static std::string generateLLVMCompare(const std::string& ptrA,
                                            const std::string& ptrB,
                                            const std::string& len) {
        std::string ir;
        ir += "; Constant-time comparison - عملية مقارنة آمنة\n";
        ir += "%result = alloca i8\n";
        ir += "store i8 0, i8* %result\n";
        ir += "br label %loop\n";
        ir += "loop:\n";
        ir += "  %i = phi i64 [0, %entry], [%next_i, %loop]\n";
        ir += "  %a_ptr = getelementptr i8, i8* " + ptrA + ", i64 %i\n";
        ir += "  %b_ptr = getelementptr i8, i8* " + ptrB + ", i64 %i\n";
        ir += "  %a_val = load volatile i8, i8* %a_ptr\n";
        ir += "  %b_val = load volatile i8, i8* %b_ptr\n";
        ir += "  %xor = xor i8 %a_val, %b_val\n";
        ir += "  %prev = load volatile i8, i8* %result\n";
        ir += "  %or = or i8 %prev, %xor\n";
        ir += "  store volatile i8 %or, i8* %result\n";
        ir += "  %next_i = add i64 %i, 1\n";
        ir += "  %done = icmp eq i64 %next_i, " + len + "\n";
        ir += "  br i1 %done, label %end, label %loop\n";
        ir += "end:\n";
        ir += "  %final = load volatile i8, i8* %result\n";
        ir += "  %is_eq = icmp eq i8 %final, 0\n";
        return ir;
    }
    
    /**
     * @brief Get Arabic name for operation
     */
    static std::string getArabicName(ConstTimeOp op) {
        switch (op) {
            case ConstTimeOp::Compare:   return "مقارنة_آمنة";
            case ConstTimeOp::Select:    return "اختيار_آمن";
            case ConstTimeOp::ZeroCheck: return "فحص_صفر_آمن";
            case ConstTimeOp::Copy:      return "نسخ_آمن";
            case ConstTimeOp::Xor:       return "حصري_آمن";
            case ConstTimeOp::And:       return "و_آمن";
            case ConstTimeOp::Or:        return "أو_آمن";
        }
        return "مجهول";
    }
};

} // namespace Crypto
} // namespace Sad

#endif // SAD_CONSTANT_TIME_H
