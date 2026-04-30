// ═══════════════════════════════════════════════════════════════════════════════════
// (AR) محرّك جامع القمامة الموحَّد — Sad::Memory::GC::GarbageCollector
// (EN) Unified Garbage Collector engine — Sad::Memory::GC::GarbageCollector
// ═══════════════════════════════════════════════════════════════════════════════════
//
// (AR) صنف C++ مستقل تماماً عن LLVM يُغلِّف خوارزميات GC المستخدمة عبر:
//        1) دالة استدعاء أصلية من sadc (عبر wrappers ضمن extern "C")
//        2) المفسّر (في وضع --dev/--learn — قيد التطوير B-step3)
//        3) الـ VM (B-step3+)
//      كانت هذه المنطقة موزّعة بين llvm_runtime.cpp و llvm_runtime_io_gc.cpp مع
//      بنية GCContext عالمية بإدارة malloc/realloc يدوية. تم استخراجها وتحويلها
//      إلى صنف C++ يستخدم std::vector (CW-16: RAII) ودوال أعضاء واضحة (CW-01: SRP).
//
// (EN) LLVM-independent C++ class encapsulating the GC algorithms previously
//      duplicated/spread across llvm_runtime.cpp and llvm_runtime_io_gc.cpp. Now
//      consumed by sadc-emitted executables (via extern "C" wrappers) and — in
//      future steps — by the interpreter (--dev) and the VM.
//
// المراحل:
//   B-step2 (الحالية): استخراج المحرك + استخدامه عبر extern "C" wrappers في sadc runtime
//   B-step3 (قادمة):  ربط المفسّر/الـ VM بنفس المحرك في وضع --dev
// ═══════════════════════════════════════════════════════════════════════════════════

#ifndef SAD_MEMORY_GC_GARBAGE_COLLECTOR_H
#define SAD_MEMORY_GC_GARBAGE_COLLECTOR_H

#include <cstdint>
#include <cstddef>
#include <vector>
#include <mutex>
#include <functional>

namespace Sad
{
    namespace Memory
    {
        namespace GC
        {

            /**
             * @brief (AR) إحصائيات GC تُرجَع من getStats().
             * @brief (EN) GC statistics returned by getStats().
             */
            struct GCStats
            {
                uint64_t totalAllocated = 0; // إجمالي البايتات المخصَّصة الآن / current bytes
                uint64_t objectCount = 0;    // عدد الكائنات المتعقَّبة / tracked objects
                uint64_t collections = 0;    // عدد دورات الجمع / collection cycles run
                uint64_t rootCount = 0;      // عدد الجذور المسجَّلة / registered roots
                bool paused = false;
            };

            /**
             * @brief (AR) محرك GC mark-and-sweep موحَّد.
             * @brief (EN) Unified mark-and-sweep GC engine.
             *
             * (AR) ملاحظات أمان الخيوط:
             *   - يحمي كل الدوال العامة بـ std::mutex داخلي. هذا تحسُّن عن GCContext القديم
             *     الذي لم يكن thread-safe إطلاقاً. لكن استخدام sadc runtime لا يزال أحادي
             *     الخيط في الغالب — الـ mutex لا يكلّف شيئاً تقريباً في هذه الحالة.
             *
             * (EN) Thread-safety: all public methods are guarded by an internal mutex.
             *      This is an improvement over the previous lock-free GCContext.
             */
            class GarbageCollector
            {
            public:
                GarbageCollector();
                ~GarbageCollector();

                // (AR) ممنوع النسخ — كائن GC وحيد لكل عملية / (EN) non-copyable
                GarbageCollector(const GarbageCollector &) = delete;
                GarbageCollector &operator=(const GarbageCollector &) = delete;

                /// (AR) تسجيل كائن مع GC. آمن للاستدعاء على ptr=null (يتجاهل).
                /// (EN) Register an object. No-op if ptr is null.
                void registerObject(void *ptr, uint64_t size);

                /// (AR) تسجيل كائن مع GC مع دالة هدم مخصّصة (للكائنات المُنشأة بـ new
                ///      ذات هدّامات غير تافهة، مثل ObjectInstance). الدالة تُستدعى أثناء
                ///      المسح بدلاً من std::free() الافتراضي. آمن للاستدعاء على ptr=null.
                /// (EN) Register an object with custom destroyer (for `new`-allocated
                ///      objects with non-trivial destructors). Called during sweep
                ///      instead of std::free(). No-op if ptr is null.
                void registerObject(void *ptr, uint64_t size,
                                    std::function<void(void *)> destroyer);

                /// (AR) ربط/تحديث دالة الهدم لكائن مسجَّل مسبقاً (لا يُسجّله إن لم يكن مسجَّلاً).
                /// (EN) Attach/update the destroyer callback for an already-registered object.
                void setDestroyer(void *ptr, std::function<void(void *)> destroyer);

                /// (AR) تعداد محتوى كائن (مؤشرات الأطفال) أثناء mark phase. لو كان
                ///      الكائن من نوع له visitChildren فيُتوقّع تسجيل visitor عبر setVisitor.
                /// (EN) Set the field-pointer visitor used during mark phase for an object.
                void setVisitor(void *ptr, std::function<void(void *,
                                                              const std::function<void(void *)> &)>
                                               visitor);

                /// (AR) إلغاء تسجيل كائن (لا يحرّر الذاكرة — المتصل مسؤول).
                /// (EN) Unregister an object (does NOT free memory — caller's responsibility).
                void unregisterObject(void *ptr);

                /// (AR) تحديث مرجع كائن بعد realloc (للحفاظ على تعقّب GC).
                /// (EN) Update tracked pointer after realloc.
                void updateObjectAfterRealloc(void *oldPtr, void *newPtr, uint64_t newSize);

                /// (AR) دورة mark-and-sweep كاملة. تتجاهل الجمع إذا كان معلَّقاً.
                /// (EN) Full mark-and-sweep cycle. Skipped if paused.
                void collect();

                /// (AR) جمع تدريجي بعدد محدود من الخطوات (للحدّ من توقّف العالم).
                /// (EN) Incremental collection bounded by `steps`.
                void collectIncremental(uint64_t steps);

                /// (AR) قراءة الإحصائيات الحالية (لقطة).
                /// (EN) Snapshot of current stats.
                GCStats getStats() const;

                /// (AR) تعليق GC (collect/collectIncremental لن تفعل شيئاً حتى resume()).
                /// (EN) Pause GC.
                void pause();

                /// (AR) استئناف GC.
                /// (EN) Resume GC.
                void resume();

                /// (AR) إضافة مؤشر إلى مجموعة الجذور (للوصول من mark phase).
                /// (EN) Add a pointer to the root set.
                void addRoot(void *ptr);

                /// (AR) إزالة مؤشر من مجموعة الجذور.
                /// (EN) Remove a pointer from the root set.
                void removeRoot(void *ptr);

                /// (AR) للاختبار/التشخيص — هل الكائن متعقَّب حالياً؟
                /// (EN) Diagnostic — is this pointer currently tracked?
                bool isTracked(void *ptr) const;

            private:
                // (AR) دوال داخلية تفترض أن mutex_ مأخوذ بالفعل / (EN) lock must be held
                int64_t findIndexLocked(void *ptr) const;
                void markFromRootsLocked(std::vector<bool> &marked) const;
                void sweepUnmarkedLocked(std::vector<bool> &marked, uint64_t &freedCount);

                mutable std::mutex mutex_;
                std::vector<void *> objects_; // (AR) المؤشرات المتعقَّبة
                std::vector<uint64_t> sizes_; // (AR) أحجامها (موازية لـ objects_)
                std::vector<std::function<void(void *)>> destroyers_; // (AR) دوال هدم اختيارية موازية لـ objects_
                std::vector<std::function<void(void *,
                                               const std::function<void(void *)> &)>>
                    visitors_;              // (AR) دوال تعداد الأطفال (موازية)
                std::vector<void *> roots_; // (AR) الجذور
                uint64_t totalAllocated_;     // (AR) مجموع الأحجام
                uint64_t collections_;        // (AR) عداد الدورات
                bool paused_;

                // (AR) حالة الجمع التدريجي (موروثة من sad_llvm_gc_collect_incremental)
                // (EN) Incremental collection state
                int incPhase_; // 0=mark, 1=sweep
                uint64_t incIndex_;
                std::vector<bool> incMarked_;
            };

            /**
             * @brief (AR) المُحرّك الافتراضي العالمي (Meyer's singleton).
             * @brief (EN) Process-wide default engine (function-local static).
             *
             * (AR) يُستخدم من wrappers الـ extern "C" التي يولّدها sadc. المفسّر/الـ VM
             *      يستطيعان إنشاء instance مستقل بدلاً من المشاركة (B-step3).
             */
            GarbageCollector &defaultEngine();

        } // namespace GC
    } // namespace Memory
} // namespace Sad

#endif // SAD_MEMORY_GC_GARBAGE_COLLECTOR_H
