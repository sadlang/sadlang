/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: widget_builder.cpp
 * المسار: interpreter/src/ui/widget_builder.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * الوصف بالعربية:
 * ───────────────
 * تنفيذ صنف WidgetBuilder — مبني عنصر الواجهة الذي يغلّف IRNode مباشرة.
 *
 * يُنشئ IRNode حسب النوع المُمرّر ويدعم:
 * - تعيين الخصائص بأنواعها المختلفة (نص، رقم، عشري، منطقي)
 * - تسجيل الأحداث (عند_النقر، عند_التغيير، ...)
 * - إضافة الأبناء (عناصر فرعية)
 * - تعيين طريقة العرض (أصلي، مرسوم، هجين)
 *
 * English Description:
 * ────────────────────
 * Implementation of WidgetBuilder — wraps IRNode directly.
 * Supports property setting, event registration, children, and rendering hints.
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "widget_builder.h"

namespace Sad
{
    namespace Interpreter
    {

        // ═══════════════════════════════════════════════════════════════════════════════
        // عداد المعرّفات الفريدة — يبدأ من مليون لتجنب التصادم مع كائنات أخرى
        // ═══════════════════════════════════════════════════════════════════════════════
        std::atomic<size_t> WidgetBuilder::widgetIdCounter_{1000000};

        size_t WidgetBuilder::nextWidgetId()
        {
            return widgetIdCounter_.fetch_add(1);
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // المنشئ: من نوع عنصر — ينشئ IRNode جديد
        // ═══════════════════════════════════════════════════════════════════════════════
        WidgetBuilder::WidgetBuilder(sad::ui::UINodeType type)
            : Data::ObjectInstance(nullptr, nextWidgetId()), irNode_(sad::ui::IRNode::create(type))
        {
            // (AR) تعيين اسم الصنف الوهمي للتمييز في نظام Value
            // (EN) Set internal class name for identification in Value system
            fields["__class__"] = Data::Value(std::string(CLASS_NAME));

            // (AR) تعيين معرّف فريد على IRNode
            // (EN) Set unique ID on IRNode
            irNode_->setId("widget_" + std::to_string(objectId));
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // المنشئ: من IRNode موجود
        // ═══════════════════════════════════════════════════════════════════════════════
        WidgetBuilder::WidgetBuilder(std::shared_ptr<sad::ui::IRNode> node)
            : Data::ObjectInstance(nullptr, nextWidgetId()), irNode_(std::move(node))
        {
            fields["__class__"] = Data::Value(std::string(CLASS_NAME));
            if (irNode_ && irNode_->getId().empty())
            {
                irNode_->setId("widget_" + std::to_string(objectId));
            }
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // تعيين خصائص IRNode
        // ═══════════════════════════════════════════════════════════════════════════════

        void WidgetBuilder::setIRProperty(const std::string &key, const std::string &value)
        {
            irNode_->setProperty(key, value);
        }

        void WidgetBuilder::setIRProperty(const std::string &key, int64_t value)
        {
            irNode_->setProperty(key, value);
        }

        void WidgetBuilder::setIRProperty(const std::string &key, double value)
        {
            irNode_->setProperty(key, value);
        }

        void WidgetBuilder::setIRProperty(const std::string &key, bool value)
        {
            irNode_->setProperty(key, value);
        }

        void WidgetBuilder::setIRPropertyFromValue(const std::string &key, const Data::Value &val)
        {
            if (val.isInteger())
            {
                irNode_->setProperty(key, val.toInt64());
            }
            else if (val.isDouble())
            {
                irNode_->setProperty(key, val.toDouble());
            }
            else if (val.isBoolean())
            {
                irNode_->setProperty(key, val.toBool());
            }
            else
            {
                // (AR) النصوص وأي نوع آخر — تحويل لنص
                // (EN) Strings and any other type — convert to string
                irNode_->setProperty(key, val.toString());
            }
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // إضافة أحداث
        // ═══════════════════════════════════════════════════════════════════════════════
        void WidgetBuilder::addIREvent(sad::ui::IREventType eventType, const std::string &handlerId)
        {
            sad::ui::IREvent event;
            event.type = eventType;
            event.expression = handlerId;
            irNode_->addEvent(event);
        }

        void WidgetBuilder::addIREvent(const std::string &eventName, const std::string &handlerId)
        {
            // (AR) تحويل الاسم النصي إلى نوع enum مركزي
            // (EN) Convert string name to centralized enum type
            sad::ui::IREventType evtType = sad::ui::stringToIREventType(eventName);
            sad::ui::IREvent event;
            event.type = evtType;
            if (evtType == sad::ui::IREventType::Custom)
            {
                event.customEventName = eventName;
            }
            event.expression = handlerId;
            irNode_->addEvent(event);
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // إضافة أبناء
        // ═══════════════════════════════════════════════════════════════════════════════
        void WidgetBuilder::addChild(std::shared_ptr<sad::ui::IRNode> childNode)
        {
            if (childNode)
            {
                irNode_->addChild(std::move(childNode));
            }
        }

        void WidgetBuilder::addChildBuilder(WidgetBuilder *child)
        {
            if (child && child->getIRNode())
            {
                irNode_->addChild(child->getIRNode());
                // (AR) حفظ مرجع الابن (مؤشر خام مُدار بـGC) للوصول لاحقاً لحقول __event_*
                childBuilders_.push_back(child);
            }
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // تلميح العرض
        // ═══════════════════════════════════════════════════════════════════════════════
        void WidgetBuilder::setRenderingHint(sad::ui::RenderingHint hint)
        {
            irNode_->setRenderingHint(hint);
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // التحريكات
        // ═══════════════════════════════════════════════════════════════════════════════

        void WidgetBuilder::addAnimation(const sad::ui::IRAnimation &anim)
        {
            irNode_->addAnimation(anim);
            hasActiveAnimation_ = true;
            compoundStartIndex_ = irNode_->getAnimations().size() - 1;
        }

        void WidgetBuilder::startAnimationChain(const std::string &typeName)
        {
            // (AR) إذا كان هناك تحريك نشط سابق ولم يكن مركّباً — ننهيه
            //      هذا يسمح بتسلسل: .حرّك("ظهور").مدة(0.5).حرّك("دوران").مدة(1)
            //      لكن لا ننهي عند صيغة الفاصلة المركّبة (يُستدعى عدة مرات متتالية)
            if (hasActiveAnimation_ && !compoundBatchActive_)
                commitAnimation();

            if (!compoundBatchActive_)
            {
                // (AR) بداية مجموعة مركّبة جديدة — نحفظ مؤشر البداية
                compoundStartIndex_ = irNode_->getAnimations().size();
            }

            // (AR) إنشاء تحريك جديد وإضافته — المعدّلات اللاحقة تُعدّل المجموعة بأكملها
            sad::ui::IRAnimation anim{};
            anim.type = sad::ui::stringToAnimationType(typeName);
            irNode_->addAnimation(anim);
            hasActiveAnimation_ = true;
        }

        void WidgetBuilder::beginCompoundBatch()
        {
            // (AR) بداية وضع المجموعة المركبة — startAnimationChain لن ينهي التحريك السابق
            if (hasActiveAnimation_)
                commitAnimation();
            compoundBatchActive_ = true;
            compoundStartIndex_ = irNode_->getAnimations().size();
        }

        void WidgetBuilder::endCompoundBatch()
        {
            // (AR) نهاية وضع المجموعة المركبة
            compoundBatchActive_ = false;
        }

        sad::ui::IRAnimation &WidgetBuilder::currentAnimation()
        {
            // (AR) نُرجع مرجعاً لآخر تحريك في القائمة
            return irNode_->getMutableAnimations().back();
        }

        void WidgetBuilder::applyToActiveAnimations(std::function<void(sad::ui::IRAnimation &)> modifier)
        {
            // (AR) تطبيق التعديل على جميع تحريكات المجموعة المركبة الحالية
            auto &anims = irNode_->getMutableAnimations();
            size_t start = compoundStartIndex_;
            if (start > anims.size())
                start = anims.size() > 0 ? anims.size() - 1 : 0;
            for (size_t i = start; i < anims.size(); ++i)
            {
                modifier(anims[i]);
            }
        }

        void WidgetBuilder::commitAnimation()
        {
            // (AR) نُعلّم أنه لم يعد قابلاً للتعديل — التحريكات القادمة ستُنشئ جديد
            hasActiveAnimation_ = false;
            compoundBatchActive_ = false;
        }

    } // namespace Interpreter
} // namespace Sad
