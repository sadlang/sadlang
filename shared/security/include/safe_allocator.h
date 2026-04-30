// ============================================================================
// safe_allocator.h — تخصيص ذاكرة آمن مع تتبع التسريبات
// (AR) فئة SafeAllocator: تخصيص/إعادة تخصيص/تحرير آمن مع تصفير الذاكرة.
//      AllocationTracker: متتبع داخلي للتخصيصات يكشف التسريبات (وضع التصحيح).
// (EN) SafeAllocator: safe alloc/realloc/free with zero-on-alloc and zero-on-free.
//      AllocationTracker: leak-detection tracker (debug builds).
// ============================================================================

#ifndef SAD_SHARED_SECURITY_SAFE_ALLOCATOR_H
#define SAD_SHARED_SECURITY_SAFE_ALLOCATOR_H

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <vector>

namespace Sad
{
    namespace Security
    {

        /**
         * @brief مخصص ذاكرة آمن
         * @brief (EN) Safe allocator with overflow/null-checks
         */
        class SafeAllocator
        {
        public:
            /// @brief (AR) يخصص ذاكرة مع تصفيرها لمنع تسرب البيانات السابقة
            /// @brief (EN) Allocates and zero-initializes memory
            static void *safeAlloc(std::size_t size);

            /// @brief (AR) يخصص مصفوفة مع كشف فيض الضرب count*elementSize
            /// @brief (EN) Allocates an array, detecting count*elementSize overflow
            static void *safeArrayAlloc(std::size_t count, std::size_t elementSize);

            /// @brief (AR) يعيد تخصيص الذاكرة مع تصفير الجزء المضاف
            /// @brief (EN) Reallocates, zero-initializing the new tail
            static void *safeRealloc(void *ptr, std::size_t oldSize, std::size_t newSize);

            /// @brief (AR) يحرر الذاكرة بعد تصفيرها (يمنع تسرب المعلومات الحساسة)
            /// @brief (EN) Zero-fills then frees the memory (prevents info leak)
            static void safeFree(void *ptr, std::size_t size);

            /// @brief (AR) يحرر ويصفّر المؤشر (template helper)
            /// @brief (EN) Zero-fills, frees, and nulls the pointer
            template <typename T>
            static void safeFreeAndNull(T *&ptr, std::size_t size)
            {
                if (ptr)
                {
                    std::memset(ptr, 0, size);
                    std::free(ptr);
                    ptr = nullptr;
                }
            }

            /**
             * @brief متتبع تخصيصات للكشف عن التسريبات (وضع التصحيح فقط)
             * @brief (EN) Allocation tracker for leak detection (debug builds)
             */
            class AllocationTracker
            {
            public:
                struct AllocationInfo
                {
                    void *ptr;
                    std::size_t size;
                    const char *file;
                    int line;
                    bool freed;
                };

                /// @brief (AR) singleton instance
                /// @brief (EN) Singleton accessor
                static AllocationTracker &instance();

                /// @brief (AR) يخصص ويسجّل التخصيص (مع موقع المصدر)
                /// @brief (EN) Allocates and records the allocation site
                void *trackAlloc(std::size_t size, const char *file, int line);

                /// @brief (AR) يحرر تخصيصاً مسجلاً ويرمي عند الـ double-free
                /// @brief (EN) Frees a tracked allocation, throws on double-free
                void trackFree(void *ptr);

                /// @brief (AR) يطبع التسريبات الحالية إلى stderr
                /// @brief (EN) Prints current leaks to stderr
                void reportLeaks();

                /// @brief (AR) يرجع عدد التسريبات الحالية
                /// @brief (EN) Returns current leak count
                std::size_t getLeakCount() const;

            private:
                AllocationTracker() = default;
                mutable std::mutex mutex_;
                std::vector<AllocationInfo> allocations_;
            };
        };

    } // namespace Security
} // namespace Sad

#endif // SAD_SHARED_SECURITY_SAFE_ALLOCATOR_H
