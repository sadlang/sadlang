// ============================================================================
// compiler_security_arithmetic_mem.cpp — حماية تجاوز الأعداد وإدارة الذاكرة
// (AR) حسابات آمنة (جمع/طرح/ضرب/قسمة)، تخصيص ذاكرة آمن، تتبع التخصيصات
// (EN) Safe arithmetic (add/sub/mul/div), safe memory allocation, allocation tracker
// تم استخراج هذا الملف من compiler_security.cpp وفقاً لقاعدة CW-05
// ============================================================================

#include <string>
#include <vector>
#include <cstdint>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <memory>
#include <algorithm>
#include <cstring>
#include <mutex>
#include <iostream>

namespace sad
{
    namespace compiler
    {
        namespace security
        {
            // ״§„‚״³… 3: ״­…״§״© ״×״¬״§ˆ״² ״§„״£״¹״¯״§״¯ (50 ״¥״µ„״§״­)
            // =============================================================================

            /**
             * @brief ״¦״© ״§„״­״³״§״¨ ״§„״¢…†
             *
             * ״×״³״×״®״¯… „…†״¹ ״×״¬״§ˆ״² ״§„״£״¹״¯״§״¯  ״§„״¹…„״§״× ״§„״­״³״§״¨״©
             */
            class SafeArithmetic
            {
            public:
                // === ״§„״¥״µ„״§״­״§״× 101-110: ״§„״¬…״¹ ״§„״¢…† ===

                /**
                 * @brief ״¬…״¹ ״¢…† „„״£״¹״¯״§״¯ ״§„״µ״­״­״©
                 */
                template <typename T>
                static bool safeAdd(T a, T b, T &result)
                {
                    static_assert(std::is_integral_v<T>, "Integral type required");

                    if constexpr (std::is_signed_v<T>)
                    {
                        if (b > 0 && a > std::numeric_limits<T>::max() - b)
                        {
                            return false; // ״×״¬״§ˆ״² …ˆ״¬״¨
                        }
                        if (b < 0 && a < std::numeric_limits<T>::min() - b)
                        {
                            return false; // ״×״¬״§ˆ״² ״³״§„״¨
                        }
                    }
                    else
                    {
                        if (a > std::numeric_limits<T>::max() - b)
                        {
                            return false;
                        }
                    }

                    result = a + b;
                    return true;
                }

                // === ״§„״¥״µ„״§״­״§״× 111-120: ״§„״·״±״­ ״§„״¢…† ===

                /**
                 * @brief ״·״±״­ ״¢…† „„״£״¹״¯״§״¯ ״§„״µ״­״­״©
                 */
                template <typename T>
                static bool safeSub(T a, T b, T &result)
                {
                    static_assert(std::is_integral_v<T>, "Integral type required");

                    if constexpr (std::is_signed_v<T>)
                    {
                        if (b < 0 && a > std::numeric_limits<T>::max() + b)
                        {
                            return false;
                        }
                        if (b > 0 && a < std::numeric_limits<T>::min() + b)
                        {
                            return false;
                        }
                    }
                    else
                    {
                        if (a < b)
                        {
                            return false;
                        }
                    }

                    result = a - b;
                    return true;
                }

                // === ״§„״¥״µ„״§״­״§״× 121-130: ״§„״¶״±״¨ ״§„״¢…† ===

                /**
                 * @brief ״¶״±״¨ ״¢…† „„״£״¹״¯״§״¯ ״§„״µ״­״­״©
                 */
                template <typename T>
                static bool safeMul(T a, T b, T &result)
                {
                    static_assert(std::is_integral_v<T>, "Integral type required");

                    if (a == 0 || b == 0)
                    {
                        result = 0;
                        return true;
                    }

                    if constexpr (std::is_signed_v<T>)
                    {
                        // ״§„״×״¹״§…„ …״¹ ״§„״­״§„״§״× ״§„״®״§״µ״©
                        if (a == -1 && b == std::numeric_limits<T>::min())
                        {
                            return false;
                        }
                        if (b == -1 && a == std::numeric_limits<T>::min())
                        {
                            return false;
                        }

                        T absA = a < 0 ? -a : a;
                        T absB = b < 0 ? -b : b;

                        if (absA > std::numeric_limits<T>::max() / absB)
                        {
                            return false;
                        }
                    }
                    else
                    {
                        if (a > std::numeric_limits<T>::max() / b)
                        {
                            return false;
                        }
                    }

                    result = a * b;
                    return true;
                }

                // === ״§„״¥״µ„״§״­״§״× 131-140: ״§„‚״³…״© ״§„״¢…†״© ===

                /**
                 * @brief ‚״³…״© ״¢…†״© „„״£״¹״¯״§״¯ ״§„״µ״­״­״©
                 */
                template <typename T>
                static bool safeDiv(T a, T b, T &result)
                {
                    static_assert(std::is_integral_v<T>, "Integral type required");

                    if (b == 0)
                    {
                        return false; // ‚״³…״© ״¹„‰ ״µ״±
                    }

                    if constexpr (std::is_signed_v<T>)
                    {
                        if (a == std::numeric_limits<T>::min() && b == -1)
                        {
                            return false; // ״×״¬״§ˆ״²
                        }
                    }

                    result = a / b;
                    return true;
                }

                // === ״§„״¥״µ„״§״­״§״× 141-150: ״§„״¨״§‚ ״§„״¢…† ===

                /**
                 * @brief ״¨״§‚ ״§„‚״³…״© ״§„״¢…†
                 */
                template <typename T>
                static bool safeMod(T a, T b, T &result)
                {
                    static_assert(std::is_integral_v<T>, "Integral type required");

                    if (b == 0)
                    {
                        return false;
                    }

                    if constexpr (std::is_signed_v<T>)
                    {
                        if (a == std::numeric_limits<T>::min() && b == -1)
                        {
                            result = 0;
                            return true;
                        }
                    }

                    result = a % b;
                    return true;
                }

                // === ״§„״¥״µ„״§״­״§״× 151-160: ״§„״×״­ˆ„ ״§„״¢…† ===

                /**
                 * @brief ״×״­ˆ„ ״¢…† ״¨† ״§„״£†ˆ״§״¹
                 */
                template <typename To, typename From>
                static bool safeCast(From value, To &result)
                {
                    // ״­״µ ״§„״­״¯ˆ״¯
                    if (value < std::numeric_limits<To>::min() ||
                        value > std::numeric_limits<To>::max())
                    {
                        return false;
                    }

                    result = static_cast<To>(value);
                    return true;
                }

                /**
                 * @brief ״×״­ˆ„ ״¢…† …† ״¹״´״± ״¥„‰ ״µ״­״­
                 */
                template <typename T>
                static bool safeFloatToInt(double value, T &result)
                {
                    static_assert(std::is_integral_v<T>, "Integral type required");

                    if (!std::isfinite(value))
                    {
                        return false;
                    }

                    if (value < static_cast<double>(std::numeric_limits<T>::min()) ||
                        value > static_cast<double>(std::numeric_limits<T>::max()))
                    {
                        return false;
                    }

                    result = static_cast<T>(value);
                    return true;
                }
            };

            // =============================================================================
            // ״§„‚״³… 4: ״¥״¯״§״±״© ״§„״°״§ƒ״±״© ״§„״¢…†״© (50 ״¥״µ„״§״­)
            // =============================================================================

            /**
             * @brief …״®״µ״µ ״°״§ƒ״±״© ״¢…†
             */
            class SafeAllocator
            {
            public:
                // === ״§„״¥״µ„״§״­״§״× 161-170: ״§„״×״®״µ״µ ״§„״¢…† ===

                /**
                 * @brief ״×״®״µ״µ ״°״§ƒ״±״© ״¨״£…״§†
                 */
                static void *safeAlloc(size_t size)
                {
                    if (!BoundsChecker::checkAllocationSize(size))
                    {
                        throw std::bad_alloc();
                    }

                    void *ptr = std::malloc(size);
                    if (!ptr)
                    {
                        throw std::bad_alloc();
                    }

                    // ״×״µ״± ״§„״°״§ƒ״±״© „…†״¹ ״×״³״±״¨ ״§„״¨״§†״§״×
                    std::memset(ptr, 0, size);

                    return ptr;
                }

                /**
                 * @brief ״×״®״µ״µ …״µˆ״© ״¨״£…״§†
                 */
                static void *safeArrayAlloc(size_t count, size_t elementSize)
                {
                    size_t totalSize;
                    if (!BoundsChecker::checkSizeMultiply(count, elementSize, totalSize))
                    {
                        throw std::bad_alloc();
                    }

                    return safeAlloc(totalSize);
                }

                // === ״§„״¥״µ„״§״­״§״× 171-180: ״¥״¹״§״¯״© ״§„״×״®״µ״µ ״§„״¢…†״© ===

                /**
                 * @brief ״¥״¹״§״¯״© ״×״®״µ״µ ״§„״°״§ƒ״±״© ״¨״£…״§†
                 */
                static void *safeRealloc(void *ptr, size_t oldSize, size_t newSize)
                {
                    if (newSize == 0)
                    {
                        std::free(ptr);
                        return nullptr;
                    }

                    if (!BoundsChecker::checkAllocationSize(newSize))
                    {
                        throw std::bad_alloc();
                    }

                    void *newPtr = std::realloc(ptr, newSize);
                    if (!newPtr)
                    {
                        throw std::bad_alloc();
                    }

                    // ״×״µ״± ״§„״¬״²״¡ ״§„״¬״¯״¯
                    if (newSize > oldSize)
                    {
                        std::memset(static_cast<char *>(newPtr) + oldSize, 0, newSize - oldSize);
                    }

                    return newPtr;
                }

                // === ״§„״¥״µ„״§״­״§״× 181-190: ״§„״×״­״±״± ״§„״¢…† ===

                /**
                 * @brief ״×״­״±״± ״§„״°״§ƒ״±״© ״¨״£…״§†
                 */
                static void safeFree(void *ptr, size_t size)
                {
                    if (ptr)
                    {
                        // …״³״­ ״§„״¨״§†״§״× ‚״¨„ ״§„״×״­״±״± („…†״¹ ״×״³״±״¨ ״§„…״¹„ˆ…״§״× ״§„״­״³״§״³״©)
                        std::memset(ptr, 0, size);
                        std::free(ptr);
                    }
                }

                /**
                 * @brief ״×״­״±״± …״¹ ״×״µ״± ״§„…״₪״´״±
                 */
                template <typename T>
                static void safeFreeAndNull(T *&ptr, size_t size)
                {
                    if (ptr)
                    {
                        std::memset(ptr, 0, size);
                        std::free(ptr);
                        ptr = nullptr;
                    }
                }

                // === ״§„״¥״µ„״§״­״§״× 191-200: ƒ״´ ״§„״×״³״±״¨״§״× ===

                /**
                 * @brief …״×״×״¨״¹ ״§„״×״®״µ״µ״§״× „„ƒ״´ ״¹† ״§„״×״³״±״¨״§״×
                 */
                class AllocationTracker
                {
                public:
                    struct AllocationInfo
                    {
                        void *ptr;
                        size_t size;
                        const char *file;
                        int line;
                        bool freed;
                    };

                    static AllocationTracker &instance()
                    {
                        static AllocationTracker tracker;
                        return tracker;
                    }

                    void *trackAlloc(size_t size, const char *file, int line)
                    {
                        void *ptr = safeAlloc(size);

                        std::lock_guard<std::mutex> lock(mutex_);
                        allocations_.push_back({ptr, size, file, line, false});

                        return ptr;
                    }

                    void trackFree(void *ptr)
                    {
                        std::lock_guard<std::mutex> lock(mutex_);

                        for (auto &alloc : allocations_)
                        {
                            if (alloc.ptr == ptr && !alloc.freed)
                            {
                                safeFree(ptr, alloc.size);
                                alloc.freed = true;
                                return;
                            }
                        }

                        // …״­״§ˆ„״© ״×״­״±״± …״₪״´״± ״÷״± …״®״µ״µ
                        throw std::runtime_error("Attempted to free unallocated memory");
                    }

                    void reportLeaks()
                    {
                        std::lock_guard<std::mutex> lock(mutex_);

                        for (const auto &alloc : allocations_)
                        {
                            if (!alloc.freed)
                            {
                                std::cerr << "Memory leak: " << alloc.size << " bytes at "
                                          << alloc.file << ":" << alloc.line << std::endl;
                            }
                        }
                    }

                    size_t getLeakCount() const
                    {
                        std::lock_guard<std::mutex> lock(mutex_);
                        return std::count_if(allocations_.begin(), allocations_.end(),
                                             [](const AllocationInfo &a)
                                             { return !a.freed; });
                    }

                private:
                    AllocationTracker() = default;
                    mutable std::mutex mutex_;
                    std::vector<AllocationInfo> allocations_;
                };
            };

// =============================================================================
// …״§ƒ״±ˆ״§״× ״§„״£…״§†
// =============================================================================

// ״­״µ ״§„״­״¯ˆ״¯
#define SAD_CHECK_INDEX(arr, idx) \
    sad::compiler::security::BoundsChecker::assertArrayIndex((idx), (arr).size(), #arr)

#define SAD_CHECK_RANGE(arr, start, end) \
    sad::compiler::security::BoundsChecker::assertArrayRange((start), (end), (arr).size(), #arr)

#define SAD_CHECK_NOT_NULL(ptr) \
    sad::compiler::security::BoundsChecker::assertNotNull((ptr), #ptr)

// ״§„״­״³״§״¨ ״§„״¢…†
#define SAD_SAFE_ADD(a, b, result)                                             \
    if (!sad::compiler::security::SafeArithmetic::safeAdd((a), (b), (result))) \
    {                                                                          \
        throw std::overflow_error("Integer overflow in addition");             \
    }

#define SAD_SAFE_MUL(a, b, result)                                             \
    if (!sad::compiler::security::SafeArithmetic::safeMul((a), (b), (result))) \
    {                                                                          \
        throw std::overflow_error("Integer overflow in multiplication");       \
    }

// ״×״×״¨״¹ ״§„״°״§ƒ״±״© („„״×״µ״­״­ ‚״·)
#ifdef SAD_DEBUG
#define SAD_ALLOC(size) \
    sad::compiler::security::SafeAllocator::AllocationTracker::instance().trackAlloc((size), __FILE__, __LINE__)
#define SAD_FREE(ptr) \
    sad::compiler::security::SafeAllocator::AllocationTracker::instance().trackFree((ptr))
#define SAD_REPORT_LEAKS() \
    sad::compiler::security::SafeAllocator::AllocationTracker::instance().reportLeaks()
#else
#define SAD_ALLOC(size) sad::compiler::security::SafeAllocator::safeAlloc(size)
#define SAD_FREE(ptr) std::free(ptr)
#define SAD_REPORT_LEAKS()
#endif


        } // namespace security
    } // namespace compiler
} // namespace sad

