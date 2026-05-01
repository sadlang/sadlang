// ======================================================================
// ui_state_manager.h — مدير الحالة التفاعلية لنظام الواجهات
// ======================================================================
// الوصف بالعربية:
//   يدير الحالة التفاعلية (@حالة) لمكونات واجهة المستخدم.
//   عندما يتغير حقل @حالة في كائن واجهة، يُخطَر المدير
//   تلقائياً ويجدول إعادة بناء شجرة العناصر (rebuildUI).
//
//   المبدأ:
//   1. عند إنشاء كائن واجهة (جديد عداد())، يسجّل observer على setField
//   2. عند تغيير حقل @حالة (عدد = 5)، يُستدعى observer
//   3. Observer يُخطر UIStateManager
//   4. UIStateManager يجدول rebuildUI (مع دمج متعدد التغييرات — batching)
//   5. في الإطار التالي يُستدعى بناء() ويُحدَّث العرض
//
// English Description:
//   Manages reactive state (@حالة) for UI components.
//   When a @حالة field changes on a UI object, the manager is notified
//   automatically and schedules a UI rebuild (rebuildUI).
//
//   Principle:
//   1. When UI object is created (جديد عداد()), register setField observer
//   2. When @حالة field changes (عدد = 5), observer is called
//   3. Observer notifies UIStateManager
//   4. UIStateManager schedules rebuildUI (with change batching)
//   5. On next frame, بناء() is called and display is updated
// ======================================================================

#pragma once

#include "value.h"
#include "object_instance.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <atomic>

namespace Sad
{
    namespace Interpreter
    {

        // =====================================================================
        // UIComponentRecord — سجل مكون واجهة مسجّل
        // =====================================================================
        //
        // (AR) يحتفظ بكل المعلومات المطلوبة لمكون واجهة واحد:
        //      - مرجع ضعيف للكائن (weak_ptr لتجنب تسريب الذاكرة)
        //      - قائمة أسماء حقول @حالة (لمعرفة أي حقول تُطلق إعادة بناء)
        //      - اسم المكون (للتشخيص)
        //
        // (EN) Holds all info needed for a single registered UI component:
        //      - weak_ptr to object (avoids memory leak)
        //      - list of @حالة field names (to know which fields trigger rebuild)
        //      - component name (for diagnostics)
        // =====================================================================
        struct UIComponentRecord
        {
            Data::ObjectInstance *instance = nullptr;    ///< مؤشر خام مُدار بـGC (B-step5b)
            std::unordered_set<std::string> stateFields; ///< أسماء حقول @حالة
            std::string componentName;                   ///< اسم المكون (واجهة عداد → "عداد")
        };

        // =====================================================================
        // UIStateManager — مدير الحالة التفاعلية
        // =====================================================================
        //
        // (AR) مفرد (Singleton) يدير جميع مكونات الواجهة المسجّلة
        //      وينسّق إعادة البناء عند تغيير الحالة.
        //
        //      دورة الحياة:
        //      1. registerComponent() عند إنشاء كائن واجهة
        //      2. onStateFieldChanged() عند تغيير حقل @حالة
        //      3. scheduleRebuild() يجدول إعادة البناء
        //      4. flush() يُنفذ جميع عمليات إعادة البناء المعلّقة
        //
        // (EN) Singleton managing all registered UI components
        //      and coordinating rebuild on state change.
        //
        //      Lifecycle:
        //      1. registerComponent() on UI object creation
        //      2. onStateFieldChanged() on @حالة field change
        //      3. scheduleRebuild() schedules rebuild
        //      4. flush() executes all pending rebuilds
        // =====================================================================
        class UIStateManager
        {
        public:
            // ─── مفرد (Singleton) ─────────────────────────────────────────
            static UIStateManager &instance()
            {
                static UIStateManager mgr;
                return mgr;
            }

            UIStateManager(const UIStateManager &) = delete;
            UIStateManager &operator=(const UIStateManager &) = delete;

            // ─── تسجيل مكون واجهة ──────────────────────────────────────
            //
            // (AR) يُنشئ سجلاً للمكون ويربط setOnFieldChanged callback
            //      يُستدعى من visitNewExpr بعد markConstructed() لمكونات الواجهة
            //
            // (EN) Creates a record for the component and hooks setOnFieldChanged
            //      Called from visitNewExpr after markConstructed() for UI components
            //
            // @param obj           — الكائن المُنشأ
            // @param componentName — اسم المكون ("عداد")
            // @param stateFields   — أسماء حقول @حالة
            void registerComponent(
                Data::ObjectInstance *obj,
                const std::string &componentName,
                const std::unordered_set<std::string> &stateFields);

            // ─── إلغاء تسجيل مكون ──────────────────────────────────────
            //
            // (AR) يزيل مكون من الإدارة (عادةً عند التدمير أو إزالة من الشجرة)
            // (EN) Removes a component from management
            //
            // @param obj — الكائن المراد إلغاء تسجيله
            void unregisterComponent(Data::ObjectInstance *obj);

            // ─── تسجيل callback لإعادة البناء ──────────────────────────
            //
            // (AR) يسجل دالة تُستدعى عند الحاجة لإعادة بناء الواجهة
            //      عادةً تكون UIBridge::rebuildUI
            //
            // (EN) Registers a callback to invoke when UI needs rebuilding
            //      Typically UIBridge::rebuildUI
            //
            // @param callback — دالة بدون معاملات
            void setRebuildCallback(std::function<void()> callback);

            // ─── تنفيذ إعادة البناء المعلّقة ──────────────────────────
            //
            // (AR) يُنفذ إعادة البناء إذا كانت هناك تغييرات معلّقة
            //      يُستدعى من حلقة الأحداث الرئيسية (كل إطار)
            //
            // (EN) Executes pending rebuild if there are queued changes
            //      Called from main event loop (every frame)
            void flush();

            // ─── هل هناك تغييرات معلّقة؟ ───────────────────────────────
            bool hasPendingChanges() const { return rebuildPending_.load(); }

            // ─── إعادة تعيين الحالة (للاختبار) ─────────────────────────
            void reset();

            // ─── عدد المكونات المسجّلة (للتشخيص) ───────────────────────
            size_t componentCount() const;

            // ─── هل المكون مسجّل؟ ──────────────────────────────────────
            bool isRegistered(Data::ObjectInstance *obj) const;

        private:
            UIStateManager() = default;

            // (AR) يُستدعى من setOnFieldChanged callback عند تغيير حقل
            // (EN) Called from setOnFieldChanged callback when field changes
            void onStateFieldChanged(
                Data::ObjectInstance *obj,
                const std::string &fieldName,
                const Data::Value &oldValue,
                const Data::Value &newValue);

            // (AR) يعلّم أن إعادة البناء مطلوبة
            // (EN) Marks that rebuild is needed
            void scheduleRebuild();

            // ─── البيانات ───────────────────────────────────────────────
            mutable std::mutex mutex_;
            std::unordered_map<Data::ObjectInstance *, UIComponentRecord> components_;
            std::function<void()> rebuildCallback_;
            std::atomic<bool> rebuildPending_{false};

            // (AR) تتبع آخر تغيير لمنع إعادة البناء المتكررة
            // (EN) Track last change to prevent redundant rebuilds
            uint64_t changeCounter_{0};
            uint64_t lastFlushedCounter_{0};
        };

    } // namespace Interpreter
} // namespace Sad
