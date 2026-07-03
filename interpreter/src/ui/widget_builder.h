/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: widget_builder.h
 * المسار: interpreter/src/ui/widget_builder.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * الوصف بالعربية:
 * ───────────────
 * صنف WidgetBuilder — مبني عنصر الواجهة الذي يغلّف IRNode مباشرة.
 *
 * هذا الصنف هو قلب نظام الواجهات التصريحي الجديد. بدلاً من إنشاء
 * ObjectInstance بحقول _نوع/_خصائص/_أبناء (نظام الخرائط القديم)،
 * يغلّف WidgetBuilder هيكل IRNode مباشرة ويدعم سلسلة المعدّلات:
 *
 *   نص("مرحبا").حجم(24).لون(.أزرق)
 *       ↓
 *   WidgetBuilder يحتوي IRNode{Text, حجم=24, لون=أزرق}
 *       ↓
 *   UIBridge يستخرج IRNode مباشرة — بدون تحويل خرائط
 *
 * يَرث من ObjectInstance ليتوافق مع نظام Value في المفسر
 * (Value::OBJECT يحمل shared_ptr<ObjectInstance>).
 *
 * كل طريقة معدّل (حجم، لون، حشوة، ...) تُعدّل IRNode وتُرجع this
 * لدعم التسلسل (method chaining).
 *
 * English Description:
 * ────────────────────
 * WidgetBuilder class — wraps IRNode directly for declarative UI.
 * Inherits ObjectInstance for Value system compatibility.
 * Each modifier method modifies the IRNode and returns this for chaining.
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_INTERPRETER_WIDGET_BUILDER_H
#define SAD_INTERPRETER_WIDGET_BUILDER_H

#include "object_instance.h"
#include "value.h"
#include "sad_ui/ir.h"
#include "sad_ui/types.h"

#include <memory>
#include <string>
#include <atomic>
#include <functional>
#include <vector> // childBuilders_ (كان يُضمَّن عبورًا فقط — تصريحٌ مباشر)

namespace Sad
{
    namespace Interpreter
    {

        /**
         * @class WidgetBuilder
         * @brief (AR) مبني عنصر واجهة — يغلّف IRNode مباشرة ويدعم سلسلة المعدّلات
         * @brief (EN) Widget builder — wraps IRNode directly, supports modifier chain
         *
         * المسار الكامل:
         *   كود ص → Parser → UIWidgetExprNode → WidgetBuilder(IRNode) → UIBridge → Backend
         *
         * استخدام:
         *   - ExpressionEvaluator::visitUIWidgetExpr ينشئ WidgetBuilder بدلاً من ObjectInstance
         *   - UIBridge::convertNodeToIR يستخرج IRNode من WidgetBuilder مباشرة
         *   - دوال البناء المدمجة (نص، زر، عمود...) تنشئ WidgetBuilder
         */
        class WidgetBuilder : public Data::ObjectInstance
        {
        public:
            // ──────────────────────────────────────────────────────
            // الإنشاء والاستخلاص
            // ──────────────────────────────────────────────────────

            /**
             * @brief (AR) إنشاء WidgetBuilder من نوع عنصر
             * @brief (EN) Create WidgetBuilder from a widget node type
             * @param type نوع العنصر (Text, Button, Column, ...)
             */
            explicit WidgetBuilder(sad::ui::UINodeType type);

            /**
             * @brief (AR) إنشاء WidgetBuilder من IRNode موجود
             * @brief (EN) Create WidgetBuilder from existing IRNode
             * @param node عقدة IR موجودة
             */
            explicit WidgetBuilder(std::shared_ptr<sad::ui::IRNode> node);

            /**
             * @brief (AR) هل هذا الكائن WidgetBuilder؟ (للتمييز السريع)
             * @brief (EN) Is this object a WidgetBuilder? (fast type check)
             */
            bool isWidgetBuilder() const { return true; }

            /**
             * @brief (AR) الحصول على عقدة IR المغلَّفة
             * @brief (EN) Get the wrapped IRNode
             */
            std::shared_ptr<sad::ui::IRNode> getIRNode() const { return irNode_; }

            /**
             * @brief (AR) الحصول على نوع العنصر
             * @brief (EN) Get widget type
             */
            sad::ui::UINodeType getNodeType() const { return irNode_->getType(); }

            // ──────────────────────────────────────────────────────
            // إضافة خصائص مباشرة
            // ──────────────────────────────────────────────────────

            /**
             * @brief (AR) تعيين خاصية نصية على IRNode
             * @brief (EN) Set string property on IRNode
             */
            void setIRProperty(const std::string &key, const std::string &value);

            /**
             * @brief (AR) تعيين خاصية رقمية على IRNode
             * @brief (EN) Set integer property on IRNode
             */
            void setIRProperty(const std::string &key, int64_t value);

            /**
             * @brief (AR) تعيين خاصية عشرية على IRNode
             * @brief (EN) Set double property on IRNode
             */
            void setIRProperty(const std::string &key, double value);

            /**
             * @brief (AR) تعيين خاصية منطقية على IRNode
             * @brief (EN) Set boolean property on IRNode
             */
            void setIRProperty(const std::string &key, bool value);

            /**
             * @brief (AR) تعيين خاصية من Value (يحدد النوع تلقائياً)
             * @brief (EN) Set property from Value (auto-detects type)
             */
            void setIRPropertyFromValue(const std::string &key, const Data::Value &val);

            // ──────────────────────────────────────────────────────
            // إضافة أحداث
            // ──────────────────────────────────────────────────────

            /**
             * @brief (AR) تسجيل حدث على IRNode
             * @brief (EN) Register event on IRNode
             * @param eventType نوع الحدث (OnTap, OnChange, ...)
             * @param handlerId معرّف المعالج المُسجّل في UIBridge
             */
            void addIREvent(sad::ui::IREventType eventType, const std::string &handlerId);

            /**
             * @brief (AR) تسجيل حدث باسم نصي (يُحوَّل تلقائياً إلى IREventType)
             * @brief (EN) Register event by string name (auto-converted to IREventType)
             */
            void addIREvent(const std::string &eventName, const std::string &handlerId);

            // ──────────────────────────────────────────────────────
            // إضافة أبناء
            // ──────────────────────────────────────────────────────

            /**
             * @brief (AR) إضافة عنصر ابن (WidgetBuilder أو IRNode)
             * @brief (EN) Add child widget (WidgetBuilder or IRNode)
             */
            void addChild(std::shared_ptr<sad::ui::IRNode> childNode);

            /**
             * @brief (AR) إضافة WidgetBuilder كابن — يحفظ المرجع للوصول لاحقاً للأحداث (مؤشر مُدار بـGC)
             * @brief (EN) Add WidgetBuilder as child — keeps ref for later event registration (GC-managed pointer)
             */
            void addChildBuilder(WidgetBuilder *child);

            /**
             * @brief (AR) الحصول على قائمة WidgetBuilder الأبناء (للتسجيل التكراري للأحداث)
             * @brief (EN) Get child WidgetBuilders (for recursive event registration)
             */
            const std::vector<WidgetBuilder *> &getChildBuilders() const { return childBuilders_; }

            // ──────────────────────────────────────────────────────
            // تلميح العرض
            // ──────────────────────────────────────────────────────

            /**
             * @brief (AR) تعيين طريقة العرض (أصلي، مرسوم، هجين)
             * @brief (EN) Set rendering hint (native, canvas, hybrid)
             */
            void setRenderingHint(sad::ui::RenderingHint hint);

            // ──────────────────────────────────────────────────────
            // التحريكات
            // ──────────────────────────────────────────────────────

            /**
             * @brief (AR) إضافة تحريك على العنصر
             * @brief (EN) Add animation to the widget
             * @param anim بنية التحريك الكاملة
             */
            void addAnimation(const sad::ui::IRAnimation &anim);

            /**
             * @brief (AR) بدء تسلسل تحريك جديد — يُنشئ تحريك ويضيفه فوراً، والمعدّلات التالية تُعدّله
             * @brief (EN) Start new animation chain — creates anim and adds it immediately, following modifiers edit it
             * @param typeName اسم نوع التحريك ("ظهور"، "تكبير"، "انزلاق_يمين"...)
             */
            void startAnimationChain(const std::string &typeName);

            /**
             * @brief (AR) هل نحن في وسط تسلسل تحريك؟ (هل يوجد تحريك حالي قابل للتعديل)
             * @brief (EN) Are we in the middle of an animation chain? (has current modifiable animation)
             */
            bool isInAnimationChain() const { return hasActiveAnimation_; }

            /**
             * @brief (AR) الحصول على آخر تحريك مُضاف للتعديل
             * @brief (EN) Get the last added animation for modification
             */
            sad::ui::IRAnimation &currentAnimation();

            /**
             * @brief (AR) تطبيق تعديل على جميع التحريكات النشطة في المجموعة المركبة الحالية
             * @brief (EN) Apply a modifier to all active animations in the current compound group
             * @param modifier دالة تقبل مرجع IRAnimation وتعدّله
             */
            void applyToActiveAnimations(std::function<void(sad::ui::IRAnimation &)> modifier);

            /**
             * @brief (AR) بدء وضع المجموعة المركبة — عدة startAnimationChain متتالية = مجموعة واحدة
             * @brief (EN) Begin compound batch mode — multiple startAnimationChain calls form one group
             */
            void beginCompoundBatch();

            /**
             * @brief (AR) نهاية وضع المجموعة المركبة
             * @brief (EN) End compound batch mode
             */
            void endCompoundBatch();

            /**
             * @brief (AR) إنهاء تسلسل التحريك — يعلّم أن التحريك لم يعد قابلاً للتعديل
             * @brief (EN) Finish animation chain — mark animation as no longer modifiable
             */
            void commitAnimation();

            // ──────────────────────────────────────────────────────
            // الثوابت والمعرف الفريد
            // ──────────────────────────────────────────────────────

            /// (AR) اسم الصنف الوهمي (للتمييز في نظام Value)
            static constexpr const char *CLASS_NAME = "__WidgetBuilder__";

            /// (AR) توليد معرّف فريد لعناصر الواجهة
            static size_t nextWidgetId();

        private:
            /// (AR) عقدة IR المغلَّفة — القلب الفعلي للعنصر
            std::shared_ptr<sad::ui::IRNode> irNode_;

            /// (AR) قائمة WidgetBuilder الأبناء — مؤشرات خام مُدارة بـGC (B-step5b)
            std::vector<WidgetBuilder *> childBuilders_;

            /// (AR) عداد المعرفات الفريدة
            static std::atomic<size_t> widgetIdCounter_;

            /// (AR) هل يوجد تحريك حالي قابل للتعديل؟
            bool hasActiveAnimation_ = false;

            /// (AR) هل نحن في وضع المجموعة المركبة؟ (عدة أنواع بفاصلة أو وسائط متعددة)
            bool compoundBatchActive_ = false;

            /// (AR) مؤشر بداية المجموعة المركبة الحالية في قائمة التحريكات
            ///      يُستخدم لتطبيق المعدّلات (.مدة، .منحنى...) على كل تحريكات المجموعة
            size_t compoundStartIndex_ = 0;
        };

        /**
         * @brief (AR) فحص ما إذا كان ObjectInstance هو WidgetBuilder
         * @brief (EN) Check if an ObjectInstance is a WidgetBuilder
         *
         * يستخدم اسم الصنف الداخلي __WidgetBuilder__ للتمييز.
         */
        inline bool isWidgetBuilder(const Data::ObjectInstance *obj)
        {
            if (!obj)
                return false;
            return obj->getClassName() == WidgetBuilder::CLASS_NAME;
        }

        /**
         * @brief (AR) تحويل ObjectInstance إلى WidgetBuilder (مع فحص النوع)
         * @brief (EN) Cast ObjectInstance to WidgetBuilder (with type check)
         */
        inline WidgetBuilder *asWidgetBuilder(Data::ObjectInstance *obj)
        {
            if (!isWidgetBuilder(obj))
                return nullptr;
            return static_cast<WidgetBuilder *>(obj);
        }

        inline const WidgetBuilder *asWidgetBuilder(const Data::ObjectInstance *obj)
        {
            if (!isWidgetBuilder(obj))
                return nullptr;
            return static_cast<const WidgetBuilder *>(obj);
        }

    } // namespace Interpreter
} // namespace Sad

#endif // SAD_INTERPRETER_WIDGET_BUILDER_H
