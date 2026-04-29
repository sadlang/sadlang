// ============================================================================
// safe_arithmetic.h — حساب عددي آمن مع كشف الفيض
// (AR) فئة SafeArithmetic: جمع/طرح/ضرب/قسمة/باقي/تحويل آمن لجميع الأنواع الصحيحة.
//      كلها قوالب — لذلك يجب أن تكون header-only.
// (EN) SafeArithmetic class: safe add/sub/mul/div/mod/cast for integral types.
//      All templates → header-only.
// ============================================================================
//
// (AR) لماذا في الطبقة المشتركة؟
//      كل عملية حسابية في وقت تشغيل لغة ص يجب أن تُحمى من الفيض:
//        - المترجم: constant folding وعمليات SIR
//        - المفسر: كل تقييم تعبير حسابي
//        - VM: تنفيذ تعليمات BYTECODE_ADD/SUB/MUL/DIV
//        - runtime/FFI: عمليات حسابية على بيانات المستخدم
// ============================================================================

#ifndef SAD_SHARED_SECURITY_SAFE_ARITHMETIC_H
#define SAD_SHARED_SECURITY_SAFE_ARITHMETIC_H

#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace Sad {
namespace Security {

/**
 * @brief فئة الحساب الآمن — تكشف الفيض قبل وقوعه
 * @brief (EN) Safe arithmetic class — detects overflow before it happens
 *
 * (AR) جميع الدوال ترجع bool: true عند النجاح، false عند الفيض.
 *      النتيجة تُكتب في معامل الخرج result. هذا النمط يتجنب exceptions
 *      في hot paths ويسمح للمستخدم باختيار سلوكه.
 * (EN) All methods return bool: true on success, false on overflow.
 *      Result is written to the out-parameter. Avoids exceptions in hot paths.
 */
class SafeArithmetic {
public:
    /// @brief (AR) جمع آمن للأعداد الصحيحة (مع وبدون إشارة)
    /// @brief (EN) Safe addition for signed and unsigned integral types
    template <typename T>
    static bool safeAdd(T a, T b, T& result) noexcept {
        static_assert(std::is_integral_v<T>, "Integral type required");
        if constexpr (std::is_signed_v<T>) {
            if (b > 0 && a > std::numeric_limits<T>::max() - b) return false;
            if (b < 0 && a < std::numeric_limits<T>::min() - b) return false;
        } else {
            if (a > std::numeric_limits<T>::max() - b) return false;
        }
        result = a + b;
        return true;
    }

    /// @brief (AR) طرح آمن
    /// @brief (EN) Safe subtraction
    template <typename T>
    static bool safeSub(T a, T b, T& result) noexcept {
        static_assert(std::is_integral_v<T>, "Integral type required");
        if constexpr (std::is_signed_v<T>) {
            if (b < 0 && a > std::numeric_limits<T>::max() + b) return false;
            if (b > 0 && a < std::numeric_limits<T>::min() + b) return false;
        } else {
            if (a < b) return false;
        }
        result = a - b;
        return true;
    }

    /// @brief (AR) ضرب آمن مع معالجة الحالات الحدية
    /// @brief (EN) Safe multiplication with edge-case handling
    template <typename T>
    static bool safeMul(T a, T b, T& result) noexcept {
        static_assert(std::is_integral_v<T>, "Integral type required");
        if (a == 0 || b == 0) { result = 0; return true; }

        if constexpr (std::is_signed_v<T>) {
            // الحالة الخاصة: -1 × INT_MIN يُسبب فيضاً
            if (a == -1 && b == std::numeric_limits<T>::min()) return false;
            if (b == -1 && a == std::numeric_limits<T>::min()) return false;

            const T absA = a < 0 ? -a : a;
            const T absB = b < 0 ? -b : b;
            if (absA > std::numeric_limits<T>::max() / absB) return false;
        } else {
            if (a > std::numeric_limits<T>::max() / b) return false;
        }
        result = a * b;
        return true;
    }

    /// @brief (AR) قسمة آمنة (تكشف القسمة على صفر و INT_MIN/-1)
    /// @brief (EN) Safe division (detects div-by-zero and INT_MIN/-1)
    template <typename T>
    static bool safeDiv(T a, T b, T& result) noexcept {
        static_assert(std::is_integral_v<T>, "Integral type required");
        if (b == 0) return false;
        if constexpr (std::is_signed_v<T>) {
            if (a == std::numeric_limits<T>::min() && b == -1) return false;
        }
        result = a / b;
        return true;
    }

    /// @brief (AR) باقي القسمة الآمن
    /// @brief (EN) Safe modulo
    template <typename T>
    static bool safeMod(T a, T b, T& result) noexcept {
        static_assert(std::is_integral_v<T>, "Integral type required");
        if (b == 0) return false;
        if constexpr (std::is_signed_v<T>) {
            if (a == std::numeric_limits<T>::min() && b == -1) {
                result = 0;
                return true;
            }
        }
        result = a % b;
        return true;
    }

    /// @brief (AR) تحويل آمن بين الأنواع الصحيحة (يفحص حدود النوع الهدف)
    /// @brief (EN) Safe cast between integral types (bounds-checked)
    template <typename To, typename From>
    static bool safeCast(From value, To& result) noexcept {
        static_assert(std::is_integral_v<From>, "Integral source required");
        static_assert(std::is_integral_v<To>,   "Integral target required");

        // (AR) المقارنة الآمنة بين أنواع مختلفة الإشارة عبر تقسيم الحالات.
        // (EN) Sign-aware comparison split into cases to avoid mixed-sign warnings.
        if constexpr (std::is_signed_v<From> == std::is_signed_v<To>) {
            if (value < static_cast<From>(std::numeric_limits<To>::min()) ||
                value > static_cast<From>(std::numeric_limits<To>::max())) {
                return false;
            }
        } else if constexpr (std::is_signed_v<From> && !std::is_signed_v<To>) {
            // (AR) From موقعة، To غير موقعة → يجب أن تكون value ≥ 0.
            if (value < 0) return false;
            using UFrom = std::make_unsigned_t<From>;
            if (static_cast<UFrom>(value) > std::numeric_limits<To>::max()) return false;
        } else {
            // (AR) From غير موقعة، To موقعة → max(To) قد يكون أصغر.
            if (value > static_cast<From>(std::numeric_limits<To>::max())) return false;
        }
        result = static_cast<To>(value);
        return true;
    }

    /// @brief (AR) تحويل آمن مع رمي std::overflow_error عند الفيض.
    /// @brief (EN) Bounds-checked cast that throws std::overflow_error on overflow.
    /// (AR) مفيد في hot paths حيث الفيض يعني خطأ منطقي يستحق إيقاف التنفيذ
    ///      (مثل arity دالة > INT_MAX، أو فهرس حلقة عكسية تجاوز int).
    /// (EN) Suitable when overflow signals a logic bug worth aborting (e.g.
    ///      function arity > INT_MAX, reverse-loop counter exceeding int).
    template <typename To, typename From>
    static To assertSafeCast(From value, const char* contextName) {
        To result{};
        if (!safeCast<To>(value, result)) {
            throw std::overflow_error(
                std::string("safeCast overflow: ") + contextName);
        }
        return result;
    }

    /// @brief (AR) تحويل آمن من عشري إلى صحيح (يرفض NaN/Inf والقيم خارج النطاق)
    /// @brief (EN) Safe float-to-int conversion (rejects NaN/Inf, out-of-range)
    template <typename T>
    static bool safeFloatToInt(double value, T& result) noexcept {
        static_assert(std::is_integral_v<T>, "Integral type required");
        if (!std::isfinite(value)) return false;
        if (value < static_cast<double>(std::numeric_limits<T>::min()) ||
            value > static_cast<double>(std::numeric_limits<T>::max())) {
            return false;
        }
        result = static_cast<T>(value);
        return true;
    }
};

}  // namespace Security
}  // namespace Sad

#endif  // SAD_SHARED_SECURITY_SAFE_ARITHMETIC_H
