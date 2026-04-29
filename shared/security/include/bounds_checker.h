// ============================================================================
// bounds_checker.h — فحص حدود الذاكرة والمصفوفات والمؤشرات
// (AR) فئة BoundsChecker: فحص فهارس المصفوفات، المؤشرات، المحاذاة، التداخل،
//      حجم التخصيص، النسخ الآمن، وفحص الشرائح. مشتركة بين المترجم والمفسر و VM.
// (EN) BoundsChecker class: array index checks, pointer/alignment/overlap checks,
//      allocation size guards, safe copy, slice validation. Shared across the
//      compiler, interpreter, and VM layers.
// ============================================================================
//
// (AR) لماذا في الطبقة المشتركة؟
//      كل مسار تنفيذ في لغة ص يحتاج لفحص الحدود:
//        - المترجم: عند توليد تعليمات الوصول للمصفوفات
//        - المفسر: عند تقييم تعبيرات المصفوفات
//        - VM: عند تنفيذ تعليمات BYTECODE_LOAD/STORE
//        - runtime: عند استدعاءات FFI من المستخدم
//      وضعها في `compiler/` كان يحرم باقي المسارات منها.
// ============================================================================

#ifndef SAD_SHARED_SECURITY_BOUNDS_CHECKER_H
#define SAD_SHARED_SECURITY_BOUNDS_CHECKER_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>

namespace Sad {
namespace Security {

/**
 * @brief فئة فحص الحدود — تمنع تجاوز ذاكرة وحوادث وصول غير آمنة
 * @brief (EN) Bounds-checking class — prevents out-of-bounds access and unsafe memory ops
 *
 * (AR) جميع الدوال static inline لتقليل تكلفة الاستدعاء وفتح المجال للـinlining
 *      من قبل المترجم. لا حالة داخلية — فئة مساعدة بحتة.
 * (EN) All methods are static inline to enable cross-TU inlining. No internal state.
 */
class BoundsChecker {
public:
    // ------------------------------------------------------------------
    // (AR) فحص فهارس المصفوفات
    // (EN) Array index checks
    // ------------------------------------------------------------------

    /// @brief (AR) يفحص ما إذا كان الفهرس داخل حدود المصفوفة
    /// @brief (EN) Returns true if index is within array bounds
    static inline bool checkArrayIndex(std::size_t index, std::size_t size) noexcept {
        return index < size;
    }

    /// @brief (AR) يؤكد على صحة الفهرس ويرمي std::out_of_range إذا تجاوز
    /// @brief (EN) Asserts index validity, throws std::out_of_range on violation
    static inline void assertArrayIndex(std::size_t index, std::size_t size,
                                        const char* arrayName) {
        if (!checkArrayIndex(index, size)) {
            throw std::out_of_range(
                std::string("Array index out of bounds: ") + arrayName +
                "[" + std::to_string(index) + "] with size " + std::to_string(size));
        }
    }

    /// @brief (AR) يفحص ما إذا كان النطاق [start, end] داخل حدود المصفوفة
    /// @brief (EN) Returns true if [start, end] range is within bounds
    static inline bool checkArrayRange(std::size_t start, std::size_t end,
                                       std::size_t size) noexcept {
        return start <= end && end <= size;
    }

    /// @brief (AR) يؤكد على صحة النطاق ويرمي std::out_of_range إذا تجاوز
    /// @brief (EN) Asserts range validity
    static inline void assertArrayRange(std::size_t start, std::size_t end,
                                        std::size_t size, const char* arrayName) {
        if (!checkArrayRange(start, end, size)) {
            throw std::out_of_range(
                std::string("Array range out of bounds: ") + arrayName +
                "[" + std::to_string(start) + ".." + std::to_string(end) +
                "] with size " + std::to_string(size));
        }
    }

    // ------------------------------------------------------------------
    // (AR) فحص المؤشرات (null + المحاذاة)
    // (EN) Pointer checks (null + alignment)
    // ------------------------------------------------------------------

    /// @brief (AR) يفحص أن المؤشر ليس فارغاً
    /// @brief (EN) Returns true if pointer is not null
    static inline bool checkNotNull(const void* ptr) noexcept { return ptr != nullptr; }

    /// @brief (AR) يؤكد أن المؤشر ليس فارغاً ويرمي std::invalid_argument
    /// @brief (EN) Asserts non-null pointer
    static inline void assertNotNull(const void* ptr, const char* name) {
        if (!checkNotNull(ptr)) {
            throw std::invalid_argument(std::string("Null pointer: ") + name);
        }
    }

    /// @brief (AR) يفحص محاذاة المؤشر على حدود معينة
    /// @brief (EN) Returns true if pointer is aligned to the given boundary
    static inline bool checkAlignment(const void* ptr, std::size_t alignment) noexcept {
        return (reinterpret_cast<std::uintptr_t>(ptr) % alignment) == 0;
    }

    /// @brief (AR) يؤكد على محاذاة المؤشر
    /// @brief (EN) Asserts pointer alignment
    static inline void assertAlignment(const void* ptr, std::size_t alignment,
                                       const char* name) {
        if (!checkAlignment(ptr, alignment)) {
            throw std::invalid_argument(
                std::string("Misaligned pointer: ") + name +
                " (required alignment: " + std::to_string(alignment) + ")");
        }
    }

    // ------------------------------------------------------------------
    // (AR) فحص حجم التخصيص
    // (EN) Allocation size guards
    // ------------------------------------------------------------------

    /// @brief (AR) يفحص أن حجم التخصيص ضمن الحد المسموح (1GB)
    /// @brief (EN) Returns true if allocation size is sane (≤ 1GB)
    static inline bool checkAllocationSize(std::size_t size) noexcept {
        constexpr std::size_t MAX_ALLOC = 1024ULL * 1024 * 1024;  // 1 GB
        return size > 0 && size <= MAX_ALLOC;
    }

    /// @brief (AR) يؤكد على حجم تخصيص آمن ويرمي std::bad_alloc
    /// @brief (EN) Asserts safe allocation size
    static inline void assertAllocationSize(std::size_t size) {
        if (!checkAllocationSize(size)) {
            throw std::bad_alloc();
        }
    }

    /// @brief (AR) يفحص ضرب حجمين مع كشف الفيض
    /// @brief (EN) Multiplies two sizes, detecting overflow
    static inline bool checkSizeMultiply(std::size_t a, std::size_t b,
                                         std::size_t& result) noexcept {
        if (a == 0 || b == 0) { result = 0; return true; }
        if (a > std::numeric_limits<std::size_t>::max() / b) return false;
        result = a * b;
        return true;
    }

    // ------------------------------------------------------------------
    // (AR) فحص تداخل الذاكرة + النسخ الآمن
    // (EN) Memory overlap + safe copy
    // ------------------------------------------------------------------

    /// @brief (AR) يفحص أن منطقتَي ذاكرة لا تتداخلان
    /// @brief (EN) Returns true if [src, src+srcSize) and [dst, dst+dstSize) do not overlap
    static inline bool checkNoOverlap(const void* src, std::size_t srcSize,
                                      const void* dst, std::size_t dstSize) noexcept {
        const auto srcStart = reinterpret_cast<std::uintptr_t>(src);
        const auto srcEnd = srcStart + srcSize;
        const auto dstStart = reinterpret_cast<std::uintptr_t>(dst);
        const auto dstEnd = dstStart + dstSize;
        return srcEnd <= dstStart || dstEnd <= srcStart;
    }

    /// @brief (AR) نسخ آمن مع فحص الحدود والاختيار التلقائي بين memcpy/memmove
    /// @brief (EN) Safe copy with bounds and overlap checks (memcpy or memmove)
    static inline bool safeCopy(void* dst, std::size_t dstSize,
                                const void* src, std::size_t srcSize) noexcept {
        if (!checkNotNull(dst) || !checkNotNull(src)) return false;
        if (srcSize > dstSize) return false;
        if (checkNoOverlap(src, srcSize, dst, dstSize)) {
            std::memcpy(dst, src, srcSize);
        } else {
            std::memmove(dst, src, srcSize);
        }
        return true;
    }

    // ------------------------------------------------------------------
    // (AR) فحص الشرائح (slices)
    // (EN) Slice validation
    // ------------------------------------------------------------------

    /// @brief (AR) يفحص صحة شريحة (offset + length) داخل منطقة ذاكرة
    /// @brief (EN) Returns true if [offset, offset+length) fits within base region
    static inline bool checkSlice(const void* basePtr, std::size_t baseSize,
                                  std::size_t offset, std::size_t length) noexcept {
        if (!checkNotNull(basePtr)) return false;
        if (offset > baseSize) return false;
        if (length > baseSize - offset) return false;
        return true;
    }

    /// @brief (AR) يؤكد على صحة الشريحة
    /// @brief (EN) Asserts slice validity
    static inline void assertSlice(const void* basePtr, std::size_t baseSize,
                                   std::size_t offset, std::size_t length,
                                   const char* name) {
        if (!checkSlice(basePtr, baseSize, offset, length)) {
            throw std::out_of_range(
                std::string("Invalid slice: ") + name +
                " (offset: " + std::to_string(offset) +
                ", length: " + std::to_string(length) +
                ", base size: " + std::to_string(baseSize) + ")");
        }
    }
};

}  // namespace Security
}  // namespace Sad

#endif  // SAD_SHARED_SECURITY_BOUNDS_CHECKER_H
