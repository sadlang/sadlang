// ======================================================================
// pin.h — مقبض Pin<T> لتثبيت كائنات GC كجذور / RAII root pin
// ======================================================================
// (AR) الوصف:
//   مقبض RAII رفيع الوزن يُسجّل مؤشره كـ "جذر" في محرك GC الافتراضي عند
//   البناء، ويزيله عند التدمير. يضمن بقاء الكائن المُشار إليه حياً طوال
//   عمر الـ Pin (مثل تثبيت كائن JNI في JVM، أو IORef في Haskell).
//
//   يُستخدم في:
//     1. حدود FFI/UI حيث يُحتفظ بالكائن لفترة طويلة خارج نطاق المتغيرات.
//     2. منشئات الكائنات أثناء بناء الحقول (لمنع جمعها قبل اكتمال البناء).
//     3. Pin<ObjectInstance> داخل Value::variant لضمان عدم جمع الهدف ما دامت
//        Value تشير إليه.
//
// (EN) Description:
//   Lightweight RAII handle that registers its pointer as a GC root at
//   construction and removes it at destruction. Guarantees the pointee
//   stays alive for the lifetime of the Pin.
//
// ملاحظات (CW-16: RAII، CW-21: واجهة واضحة):
//   - النسخ مسموح ويُسجّل جذراً جديداً (نفس المؤشر مرّتين في root list — مقبول).
//   - النقل ينقل الجذر بدون addRoot/removeRoot إضافيين (تحسين).
//   - تخصيصه إلى nullptr يزيل الجذر السابق فوراً.
//   - آمن للاستخدام مع المؤشر الفارغ (no-op).
// ======================================================================

#pragma once

#include "memory/gc/engine/garbage_collector.h"

namespace Sad
{
    namespace Data
    {

        /**
         * @class Pin
         * @brief (AR) مقبض RAII يُثبّت كائناً مُتعقَّباً كجذر في GC طوال عمره.
         * @brief (EN) RAII handle pinning a GC-tracked object as a root for its lifetime.
         */
        template <class T>
        class Pin
        {
        public:
            // ──────────────────────────────────────────────────────────
            // (AR) المنشئات / (EN) Constructors
            // ──────────────────────────────────────────────────────────

            /// (AR) منشئ افتراضي — Pin فارغ.
            /// (EN) Default constructor — empty pin.
            Pin() noexcept : ptr_(nullptr) {}

            /// (AR) منشئ من مؤشر — يُسجّله كجذر إن لم يكن فارغاً.
            /// (EN) Construct from pointer — registers as root if not null.
            explicit Pin(T *ptr) : ptr_(ptr)
            {
                if (ptr_ != nullptr)
                {
                    ::Sad::Memory::GC::defaultEngine().addRoot(static_cast<void *>(ptr_));
                }
            }

            /// (AR) منشئ نسخ — يُسجّل جذراً إضافياً لنفس المؤشر.
            /// (EN) Copy constructor — registers an additional root for the same pointer.
            Pin(const Pin &other) : ptr_(other.ptr_)
            {
                if (ptr_ != nullptr)
                {
                    ::Sad::Memory::GC::defaultEngine().addRoot(static_cast<void *>(ptr_));
                }
            }

            /// (AR) منشئ نقل — ينقل الملكية بدون إعادة تسجيل.
            /// (EN) Move constructor — transfers ownership without re-registering.
            Pin(Pin &&other) noexcept : ptr_(other.ptr_) { other.ptr_ = nullptr; }

            // ──────────────────────────────────────────────────────────
            // (AR) الإسناد / (EN) Assignment
            // ──────────────────────────────────────────────────────────

            Pin &operator=(const Pin &other)
            {
                if (this == &other)
                {
                    return *this;
                }
                if (ptr_ != nullptr)
                {
                    ::Sad::Memory::GC::defaultEngine().removeRoot(static_cast<void *>(ptr_));
                }
                ptr_ = other.ptr_;
                if (ptr_ != nullptr)
                {
                    ::Sad::Memory::GC::defaultEngine().addRoot(static_cast<void *>(ptr_));
                }
                return *this;
            }

            Pin &operator=(Pin &&other) noexcept
            {
                if (this == &other)
                {
                    return *this;
                }
                if (ptr_ != nullptr)
                {
                    ::Sad::Memory::GC::defaultEngine().removeRoot(static_cast<void *>(ptr_));
                }
                ptr_ = other.ptr_;
                other.ptr_ = nullptr;
                return *this;
            }

            Pin &operator=(T *raw)
            {
                if (ptr_ == raw)
                {
                    return *this;
                }
                if (ptr_ != nullptr)
                {
                    ::Sad::Memory::GC::defaultEngine().removeRoot(static_cast<void *>(ptr_));
                }
                ptr_ = raw;
                if (ptr_ != nullptr)
                {
                    ::Sad::Memory::GC::defaultEngine().addRoot(static_cast<void *>(ptr_));
                }
                return *this;
            }

            // ──────────────────────────────────────────────────────────
            // (AR) الهدّام / (EN) Destructor
            // ──────────────────────────────────────────────────────────

            ~Pin()
            {
                if (ptr_ != nullptr)
                {
                    ::Sad::Memory::GC::defaultEngine().removeRoot(static_cast<void *>(ptr_));
                }
            }

            // ──────────────────────────────────────────────────────────
            // (AR) الوصول / (EN) Accessors
            // ──────────────────────────────────────────────────────────

            T *get() const noexcept { return ptr_; }
            T *operator->() const noexcept { return ptr_; }
            T &operator*() const noexcept { return *ptr_; }
            explicit operator bool() const noexcept { return ptr_ != nullptr; }

            /// (AR) إفراج الجذر بدون حذف (يُرجع الخام ويفقد التتبّع).
            /// (EN) Release root tracking without deleting; returns the raw pointer.
            T *release() noexcept
            {
                T *tmp = ptr_;
                if (ptr_ != nullptr)
                {
                    ::Sad::Memory::GC::defaultEngine().removeRoot(static_cast<void *>(ptr_));
                    ptr_ = nullptr;
                }
                return tmp;
            }

            /// (AR) إعادة الضبط إلى مؤشر آخر (أو null) مع إدارة الجذور.
            /// (EN) Reset to a different pointer (or null), updating root set.
            void reset(T *raw = nullptr) { *this = raw; }

        private:
            T *ptr_;
        };

    } // namespace Data
} // namespace Sad
