// ═══════════════════════════════════════════════════════════════════════════════
// (AR) ownership_tracker.h — متتبع ملكية المؤشرات في وقت التشغيل (موحَّد).
//
// (AR) مُستخرَج من compiler/include/ffi/ffi_wrapper.h كجزء من DEF-001. الموقع
//      الجديد في shared/ يمكّن:
//        - مرحلة C: ربط runtime sadc عبر C-ABI (sad_ownership_capi.h).
//        - مستهلكين مستقبليين (interpreter/VM) بدون اعتماد على compiler/.
//
// (AR) دلالات السلوك مطابقة 100% للنسخة الأصلية. الإضافة الوحيدة: أمان الخيوط
//      عبر std::mutex داخل OwnershipTracker — ضرورة لـ runtime sadc.
//
// (EN) Runtime pointer ownership tracker — extracted to shared/ to enable
//      C-ABI consumption from compiled sadc output. Behaviour identical to
//      the original; thread-safety added via internal std::mutex.
//
// @phase Memory Layer Unification — Phase B / DEF-001
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include <cstddef>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Sad
{
    namespace Ownership
    {
        namespace Runtime
        {

            // ─────────────────────────────────────────────────────────────────────────────
            // (AR) سياسة الملكية لمؤشر مسجَّل.
            // (EN) Ownership policy of a registered pointer.
            // ─────────────────────────────────────────────────────────────────────────────
            enum class OwnershipPolicy
            {
                OWNED,      // (AR) مملوك — على المسجِّل تحريره
                BORROWED,   // (AR) مُستعار — لا يحرَّر هنا
                SHARED,     // (AR) مشترك — مرجع مرات متعددة
                TRANSFERRED // (AR) منقول — لا يصلح بعد
            };

            // ─────────────────────────────────────────────────────────────────────────────
            // (AR) تحويل سياسة الملكية إلى نص عربي للتشخيص. inline ليبقى رخيصاً.
            // (EN) Diagnostic-friendly Arabic name for an OwnershipPolicy.
            // ─────────────────────────────────────────────────────────────────────────────
            inline const char *ownershipPolicyToArabic(OwnershipPolicy policy)
            {
                switch (policy)
                {
                case OwnershipPolicy::OWNED:
                    return "مملوك";
                case OwnershipPolicy::BORROWED:
                    return "مُستعار";
                case OwnershipPolicy::SHARED:
                    return "مشترك";
                case OwnershipPolicy::TRANSFERRED:
                    return "منقول";
                }
                return "غير معروف";
            }

            // ─────────────────────────────────────────────────────────────────────────────
            // (AR) معلومات تخصيص واحد. ثابتة الحقول مطابقة للنسخة الأصلية.
            // (EN) Per-allocation metadata; field set identical to original.
            // ─────────────────────────────────────────────────────────────────────────────
            struct AllocationInfo
            {
                void *ptr = nullptr;   // (AR) المؤشر / (EN) pointer
                std::size_t size = 0;  // (AR) الحجم / (EN) size in bytes
                std::string allocator; // (AR) دالة التخصيص / (EN) allocator name
                std::string file;      // (AR) ملف المصدر / (EN) source file
                int line = 0;          // (AR) رقم السطر / (EN) source line
                OwnershipPolicy policy = OwnershipPolicy::OWNED;
                bool isFreed = false; // (AR) هل حُرر؟ / (EN) freed?
            };

            // ─────────────────────────────────────────────────────────────────────────────
            // (AR) OwnershipTracker — singleton يحفظ خريطة ptr → AllocationInfo.
            //
            //      آمن للخيوط: كل دالة عامة تأخذ std::lock_guard على mutex داخلي.
            //      هذا ضروري لاستهلاك C-ABI من runtime sadc الذي قد يكون متعدد الخيوط.
            //
            // (EN) Singleton tracker mapping ptr → AllocationInfo. Thread-safe via an
            //      internal std::mutex (required for multi-threaded sadc runtime use).
            // ─────────────────────────────────────────────────────────────────────────────
            class OwnershipTracker
            {
            public:
                // (AR) النسخ ممنوع (singleton). / (EN) Singleton: non-copyable.
                OwnershipTracker(const OwnershipTracker &) = delete;
                OwnershipTracker &operator=(const OwnershipTracker &) = delete;

                // (AR) الوصول الوحيد للمثيل. / (EN) Sole accessor.
                static OwnershipTracker &instance();

                // ─── (AR) التتبع / (EN) Tracking ───
                void registerAllocation(void *ptr, std::size_t size,
                                        const std::string &allocator,
                                        const std::string &file = "", int line = 0);

                bool registerDeallocation(void *ptr, const std::string &deallocator);

                void transferOwnership(void *ptr, const std::string &newOwner);

                // ─── (AR) الفحص / (EN) Checking ───
                bool isOwned(void *ptr) const;
                bool isFreed(void *ptr) const;
                std::optional<AllocationInfo> getAllocationInfo(void *ptr) const;

                // ─── (AR) التقارير / (EN) Reports ───
                std::vector<AllocationInfo> getActiveAllocations() const;
                void printLeakReport() const;
                void clear();

            private:
                OwnershipTracker() = default;

                // (AR) البيانات + قفلها — mutable ليتمكن const-methods من القفل.
                // (EN) Data + its lock — mutable so const methods may lock.
                mutable std::mutex allocations_mutex_;
                std::unordered_map<void *, AllocationInfo> allocations_;
            };

        } // namespace Runtime
    } // namespace Ownership
} // namespace Sad
