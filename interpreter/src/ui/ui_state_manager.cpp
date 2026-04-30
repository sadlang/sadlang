// ======================================================================
// ui_state_manager.cpp — تنفيذ مدير الحالة التفاعلية
// ======================================================================
// الوصف بالعربية:
//   تنفيذ UIStateManager — يدير دورة الحياة التفاعلية:
//   تسجيل المكونات → مراقبة التغييرات → جدولة إعادة البناء → تنفيذ
//
// English Description:
//   UIStateManager implementation — manages the reactive lifecycle:
//   register components → watch changes → schedule rebuild → execute
// ======================================================================

#include "ui_state_manager.h"
#include <iostream>
#include <algorithm>

namespace Sad
{
    namespace Interpreter
    {

        // ─────────────────────────────────────────────────────────────────
        // registerComponent — تسجيل مكون واجهة جديد
        // ─────────────────────────────────────────────────────────────────
        void UIStateManager::registerComponent(
            Data::ObjectInstance * obj,
            const std::string &componentName,
            const std::unordered_set<std::string> &stateFields)
        {
            if (!obj)
                return;

            std::lock_guard<std::mutex> lock(mutex_);

            // (AR) إنشاء سجل المكون
            // (EN) Create component record
            UIComponentRecord record;
            record.instance = obj; // مؤشر خام مُدار بـGC
            record.componentName = componentName;
            record.stateFields = stateFields;

            Data::ObjectInstance *rawPtr = obj;
            components_[rawPtr] = std::move(record);

            // (AR) ربط observer على setField — يُستدعى عند كل تعيين حقل
            //      نستخدم raw pointer في الـ capture لأن الـ callback
            //      مخزّن في الكائن نفسه (لا يحتاج shared_ptr)
            //
            // (EN) Hook setField observer — called on every field assignment
            //      We use raw pointer in capture because the callback
            //      is stored in the object itself (doesn't need shared_ptr)
            obj->setOnFieldChanged(
                [this, rawPtr](Data::ObjectInstance *changedObj,
                               const std::string &fieldName,
                               const Data::Value &oldValue,
                               const Data::Value &newValue)
                {
                    // (AR) التحقق أن هذا هو نفس الكائن المسجّل
                    // (EN) Verify this is the same registered object
                    if (changedObj == rawPtr)
                    {
                        onStateFieldChanged(changedObj, fieldName, oldValue, newValue);
                    }
                });

#ifdef DEBUG_UI_STATE
            std::cout << "[UIState] تم تسجيل مكون: " << componentName
                      << " (" << stateFields.size() << " حقل حالة)"
                      << std::endl;
#endif
        }

        // ─────────────────────────────────────────────────────────────────
        // unregisterComponent — إلغاء تسجيل مكون
        // ─────────────────────────────────────────────────────────────────
        void UIStateManager::unregisterComponent(Data::ObjectInstance *obj)
        {
            if (!obj)
                return;

            std::lock_guard<std::mutex> lock(mutex_);

            auto it = components_.find(obj);
            if (it != components_.end())
            {
                // (AR) إزالة observer من الكائن (مؤشر خام مُدار بـGC)
                // (EN) Remove observer from object (raw GC-managed pointer)
                auto *strongRef = it->second.instance;
                if (strongRef)
                {
                    strongRef->removeOnFieldChanged();
                }
                components_.erase(it);
            }
        }

        // ─────────────────────────────────────────────────────────────────
        // onStateFieldChanged — يُستدعى عند تغيير أي حقل في مكون مسجّل
        // ─────────────────────────────────────────────────────────────────
        void UIStateManager::onStateFieldChanged(
            Data::ObjectInstance *obj,
            const std::string &fieldName,
            const Data::Value &oldValue,
            const Data::Value &newValue)
        {
            // (AR) لا نحتاج قفل هنا — نقرأ فقط ولا نعدّل components_
            //      (Lock-free path لأداء عالي في حلقة الأحداث)
            //
            // (EN) No lock needed here — read-only, no modification to components_
            //      (Lock-free path for high performance in event loop)

            auto it = components_.find(obj);
            if (it == components_.end())
                return;

            const auto &record = it->second;

            // (AR) تحقق أن الحقل المتغير هو حقل @حالة
            //      الحقول العادية لا تُطلق إعادة بناء
            //
            // (EN) Check that changed field is a @حالة field
            //      Regular fields don't trigger rebuild
            if (record.stateFields.find(fieldName) == record.stateFields.end())
                return;

            // (AR) تحقق أن القيمة تغيّرت فعلاً (تجنب إعادة بناء غير ضرورية)
            // (EN) Check value actually changed (avoid unnecessary rebuild)
            try
            {
                if ((oldValue == newValue).toBool())
                    return;
            }
            catch (...)
            {
                // (AR) إذا فشلت المقارنة (أنواع مختلفة)، نعتبرها تغييراً
                // (EN) If comparison fails (different types), treat as changed
            }

#ifdef DEBUG_UI_STATE
            std::cout << "[UIState] تغيير حالة: " << record.componentName
                      << "." << fieldName
                      << " ← " << newValue.toString()
                      << std::endl;
#endif

            // (AR) جدولة إعادة البناء
            // (EN) Schedule rebuild
            scheduleRebuild();
        }

        // ─────────────────────────────────────────────────────────────────
        // scheduleRebuild — جدولة إعادة بناء
        // ─────────────────────────────────────────────────────────────────
        void UIStateManager::scheduleRebuild()
        {
            changeCounter_++;
            rebuildPending_.store(true, std::memory_order_release);
        }

        // ─────────────────────────────────────────────────────────────────
        // flush — تنفيذ إعادة البناء المعلّقة
        // ─────────────────────────────────────────────────────────────────
        void UIStateManager::flush()
        {
            // (AR) فحص سريع بدون قفل (atomic check)
            // (EN) Fast check without lock (atomic)
            if (!rebuildPending_.load(std::memory_order_acquire))
                return;

            // (AR) تحقق أن هناك تغييرات جديدة منذ آخر flush
            // (EN) Verify there are new changes since last flush
            if (changeCounter_ == lastFlushedCounter_)
            {
                rebuildPending_.store(false, std::memory_order_release);
                return;
            }

            // (AR) حفظ العداد قبل الاستدعاء — قد تحصل تغييرات أثناء rebuild
            // (EN) Save counter before callback — changes may occur during rebuild
            uint64_t currentCounter = changeCounter_;

            // (AR) تنظيف المكونات الفارغة (مؤشر raw مُدار بـGC؛ NULL = منتهي)
            // (EN) Clean up null components (GC-managed raw pointer; NULL = expired)
            {
                std::lock_guard<std::mutex> lock(mutex_);
                for (auto it = components_.begin(); it != components_.end();)
                {
                    if (it->second.instance == nullptr)
                    {
                        it = components_.erase(it);
                    }
                    else
                    {
                        ++it;
                    }
                }
            }

            // (AR) استدعاء callback إعادة البناء
            // (EN) Invoke rebuild callback
            if (rebuildCallback_)
            {
                rebuildCallback_();
            }

            // (AR) تحديث العداد
            // (EN) Update counter
            lastFlushedCounter_ = currentCounter;

            // (AR) تحقق إذا لم تحصل تغييرات جديدة أثناء rebuild
            // (EN) Check if no new changes occurred during rebuild
            if (changeCounter_ == currentCounter)
            {
                rebuildPending_.store(false, std::memory_order_release);
            }
        }

        // ─────────────────────────────────────────────────────────────────
        // setRebuildCallback — تسجيل دالة إعادة البناء
        // ─────────────────────────────────────────────────────────────────
        void UIStateManager::setRebuildCallback(std::function<void()> callback)
        {
            rebuildCallback_ = std::move(callback);
        }

        // ─────────────────────────────────────────────────────────────────
        // reset — إعادة تعيين
        // ─────────────────────────────────────────────────────────────────
        void UIStateManager::reset()
        {
            std::lock_guard<std::mutex> lock(mutex_);

            // (AR) إزالة observers من جميع الكائنات الحية
            // (EN) Remove observers from all live objects
            for (auto &[ptr, record] : components_)
            {
                auto *strongRef = record.instance;
                if (strongRef)
                {
                    strongRef->removeOnFieldChanged();
                }
            }

            components_.clear();
            rebuildCallback_ = nullptr;
            rebuildPending_.store(false);
            changeCounter_ = 0;
            lastFlushedCounter_ = 0;
        }

        // ─────────────────────────────────────────────────────────────────
        // componentCount — عدد المكونات المسجّلة
        // ─────────────────────────────────────────────────────────────────
        size_t UIStateManager::componentCount() const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return components_.size();
        }

        // ─────────────────────────────────────────────────────────────────
        // isRegistered — هل المكون مسجّل؟
        // ─────────────────────────────────────────────────────────────────
        bool UIStateManager::isRegistered(Data::ObjectInstance *obj) const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return components_.find(obj) != components_.end();
        }

    } // namespace Interpreter
} // namespace Sad
