/**
 * ملف: ui_bridge_events.cpp
 * المسار: interpreter/src/ui/ui_bridge_events.cpp
 *
 * الوصف: الأحداث والتنقل والمؤقتات والحوارات — مُقتطع من ui_bridge.cpp
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 */

#include "ui_bridge.h"
#include "interpreter_core.h"
#include "object_instance.h"
#include "widget_builder.h" // (AR) لدعم registerWidgetBuilderEvents

#include "sad_ui/types.h"
#include "sad_ui/ir.h"

#include <iostream>
#include <algorithm>

namespace Sad
{
    namespace Interpreter
    {
        void UIBridge::handleEvent(
            sad::ui::IREventType eventType,
            const std::string &handlerId,
            const sad::ui::IRNode *node,
            const sad::ui::EventData &eventData)
        {
            if (!interpreter_)
                return;

            for (const auto &event : node->getEvents())
            {
                if (event.type == eventType)
                {
                    try
                    {
                        const auto &expr = event.expression;

                        // (AR) تحضير بيانات الحدث كخريطة Value لتمريرها للمعالج
                        Data::Value::MapType eventMap;
                        eventMap["x"] = Data::Value(static_cast<double>(eventData.x));
                        eventMap["y"] = Data::Value(static_cast<double>(eventData.y));
                        eventMap["deltaX"] = Data::Value(static_cast<double>(eventData.deltaX));
                        eventMap["deltaY"] = Data::Value(static_cast<double>(eventData.deltaY));
                        if (eventData.keyCode != 0)
                        {
                            eventMap["keyCode"] = Data::Value(eventData.keyCode);
                        }
                        if (!eventData.keyName.empty())
                        {
                            eventMap["keyName"] = Data::Value(eventData.keyName);
                        }
                        eventMap["button"] = Data::Value(eventData.button);
                        eventMap["shift"] = Data::Value(eventData.shiftKey);
                        eventMap["ctrl"] = Data::Value(eventData.ctrlKey);
                        eventMap["alt"] = Data::Value(eventData.altKey);

                        // ─── بيانات اللمس (Touch Data) ───
                        if (eventData.isTouch)
                        {
                            eventMap["\xd9\x84\xd9\x85\xd8\xb3"] = Data::Value(true); // لمس
                            eventMap["isTouch"] = Data::Value(true);
                            eventMap["\xd8\xb6\xd8\xba\xd8\xb7"] = Data::Value(static_cast<double>(eventData.pressure)); // ضغط
                            eventMap["pressure"] = Data::Value(static_cast<double>(eventData.pressure));
                            eventMap["\xd9\x85\xd8\xb9\xd8\xb1\xd9\x81_\xd8\xa7\xd9\x84\xd8\xa5\xd8\xb5\xd8\xa8\xd8\xb9"] = Data::Value(static_cast<int>(eventData.fingerId)); // معرف_الإصبع
                            eventMap["fingerId"] = Data::Value(static_cast<int>(eventData.fingerId));
                            eventMap["\xd8\xb9\xd8\xaf\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xa3\xd8\xb5\xd8\xa7\xd8\xa8\xd8\xb9"] = Data::Value(eventData.touchCount); // عدد_الأصابع
                            eventMap["touchCount"] = Data::Value(eventData.touchCount);
                        }
                        if (eventData.angle != 0.0f)
                        {
                            eventMap["angle"] = Data::Value(static_cast<double>(eventData.angle));
                        }
                        if (!eventData.value.empty())
                        {
                            eventMap["value"] = Data::Value(eventData.value);
                        }
                        if (!eventData.customData.empty())
                        {
                            eventMap["data"] = Data::Value(eventData.customData);
                        }

                        // (AR) تحضير الوسائط
                        std::vector<Data::Value> args;
                        if (node->getType() == sad::ui::UINodeType::Slider)
                        {
                            const auto *valProp = node->findProperty(
                                "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9"); // قيمة
                            if (valProp)
                            {
                                if (auto *iv = std::get_if<int64_t>(&valProp->value))
                                    args.push_back(Data::Value(static_cast<int>(*iv)));
                                else if (auto *dv = std::get_if<double>(&valProp->value))
                                    args.push_back(Data::Value(*dv));
                            }
                        }
                        // (AR) إضافة خريطة بيانات الحدث كوسيط
                        args.push_back(Data::Value(std::move(eventMap)));

                        // (AR) استدعاء المعالج
                        auto it = eventHandlers_.find(expr);
                        if (it != eventHandlers_.end())
                        {
                            invokeHandler(expr, args);
                        }
                        else
                        {
                            interpreter_->callUserFunction(expr, args);
                        }

                        // (AR) إعادة بناء UI بعد تنفيذ الحدث
                        rebuildUI();
                    }
                    catch (const std::exception &e)
                    {
                        std::cerr << "\xd8\xae\xd8\xb7\xd8\xa3 \xd9\x85\xd8\xb9\xd8\xa7\xd9\x84\xd8\xac\xd8\xa9 \xd8\xa7\xd9\x84\xd8\xad\xd8\xaf\xd8\xab '"
                                  << sad::ui::irEventTypeToString(eventType)
                                  << "': " << e.what() << std::endl;
                    }
                    break;
                }
            }
        }

        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
        // ״¥״¹״§״¯״© ״¨†״§״¡ ״§„ˆ״§״¬‡״© (Reactive Rebuild)
        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

        void UIBridge::rebuildUI()
        {
            if (!activeWindow_)
                return;

            // (AR) ״¥״°״§ ƒ״§†״× ״¯״§„״© ״¨†‘״§״¡ …״³״¬‘„״©״ †״³״×״¯״¹‡״§ „״¥†״×״§״¬ ״´״¬״±״© ״¬״¯״¯״©
            // (EN) If builder function is registered, invoke it to produce new tree
            if (!builderFunc_.isVoid() && (builderFunc_.isFunction() || builderFunc_.isCallable()) && interpreter_)
            {
                try
                {
                    auto funcRef = builderFunc_.toFunction();
                    if (funcRef)
                    {
                        auto newWidget = interpreter_->callUserFunction(funcRef->registeredName, {});
                        rootWidget_ = newWidget;
                    }
                }
                catch (const std::exception &e)
                {
                    std::cerr << "״®״·״£  ״¥״¹״§״¯״© ״¨†״§״¡ ״§„ˆ״§״¬‡״©: " << e.what() << std::endl;
                }
            }

            if (rootWidget_.isVoid())
                return;

            // (AR) استخدام PlatformWindow* مباشرة — applyPatches و setContent
            //      متوفران كدوال virtual في الواجهة المجردة لجميع المنصات
            // (EN) Using PlatformWindow* directly — applyPatches and setContent
            //      are virtual methods available on all platforms
            auto *window = activeWindow_;

            // …״³״­ ״§„€ handlers ״§„‚״¯…״© (״³״×״¹״§״¯ ״×״³״¬„‡״§ ״¹†״¯ ״§„״×״­ˆ„)
            // ═══ نظام الأجيال: بدلاً من مسح كل المعالجات، نزيد رقم الجيل ═══
            // (AR) المعالجات المُسجّلة في هذه الدورة ستحمل الجيل الجديد
            //      بعد convertToIR، نمسح فقط معالجات الأجيال القديمة
            // (EN) Generation system: increment gen, sweep old after convertToIR
            handlerGeneration_++;
            handlerCounter_ = 0;

            // ״¥״¹״§״¯״© ״×״­ˆ„ ״´״¬״±״© ״§„€ widget ״¥„‰ IR
            auto newIR = convertToIR(rootWidget_);

            // ═══════════════════════════════════════════════════════════════════
            // (AR) كنس المعالجات القديمة: حذف كل معالج لم يُعاد تسجيله
            //      في الجيل الحالي (يعني العقدة التي كانت تملكه أُزيلت)
            // (EN) Sweep old handlers: remove any handler not re-registered
            //      in the current generation (its owning node was removed)
            // ═══════════════════════════════════════════════════════════════════
            for (auto it = eventHandlers_.begin(); it != eventHandlers_.end();)
            {
                if (it->second.generation < handlerGeneration_)
                {
                    handlerOwners_.erase(it->first);
                    it = eventHandlers_.erase(it);
                }
                else
                {
                    ++it;
                }
            }

            if (newIR)
            {
                // ═══ Reconciler — المطابقة الذكية بدلاً من المقارنة الشاملة ═══
                // (AR) إذا كانت هناك شجرة سابقة → نستخدم الـ Reconciler
                //      لمقارنة التغييرات وتطبيقها موضعياً على الشجرة الحية
                // (EN) If previous tree exists → use Reconciler for smart diffing
                if (previousIR_)
                {
                    auto diffResult = reconciler_.diff(previousIR_, newIR);

                    if (diffResult.isEmpty())
                    {
                        // (AR) الشجرة لم تتغير — تخطي إعادة التخطيط
                        stateChanged_ = false;
                        pendingTransitionType_.clear();
                        return;
                    }

                    // (AR) الشجرة تغيرت — تطبيق الرقع على الشجرة الحية
                    // (EN) Tree changed — apply patches to live tree
                    if (reconciler_.patch(previousIR_, diffResult))
                    {
                        // (AR) إذا كان هناك انتقال مُعلّق، نستخدم setContent الكامل
                        //      لأن الانتقالات تحتاج إعادة رسم كاملة مع تحريك
                        if (!pendingTransitionType_.empty())
                        {
                            if (!pendingExitTransitionType_.empty())
                            {
                                window->setContentWithExitTransition(
                                    previousIR_,
                                    pendingTransitionType_,
                                    pendingExitTransitionType_,
                                    pendingTransitionDuration_);
                                pendingExitTransitionType_.clear();
                            }
                            else
                            {
                                window->setContentWithTransition(
                                    previousIR_,
                                    pendingTransitionType_,
                                    pendingTransitionDuration_);
                            }
                            pendingTransitionType_.clear();
                        }
                        else
                        {
                            // (AR) تحديث جزئي — applyPatches بدلاً من setContent الكامل
                            //      الشجرة previousIR_ تم تعديلها موضعياً بواسطة patch()
                            //      نحتاج فقط إعادة تخطيط ورسم — بدون إعادة تهيئة كل التحريكات
                            // (EN) Partial update — applyPatches instead of full setContent
                            bool needsRelayout = false;
                            for (const auto &p : diffResult.patches)
                            {
                                if (p.type == sad::ui::PatchType::INSERT_CHILD ||
                                    p.type == sad::ui::PatchType::REMOVE_CHILD ||
                                    p.type == sad::ui::PatchType::REORDER_CHILDREN ||
                                    p.type == sad::ui::PatchType::REPLACE)
                                {
                                    needsRelayout = true;
                                    break;
                                }
                                // (AR) تحديث خصائص الأبعاد يتطلب إعادة تخطيط
                                if (p.type == sad::ui::PatchType::UPDATE_PROPS)
                                {
                                    for (const auto &prop : p.updatedProps)
                                    {
                                        if (prop.key == "\xd8\xb9\xd8\xb1\xd8\xb6" ||                         // عرض
                                            prop.key == "\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9" || // ارتفاع
                                            prop.key == "\xd8\xad\xd8\xb4\xd9\x88" ||                         // حشو
                                            prop.key == "\xd9\x87\xd8\xa7\xd9\x85\xd8\xb4" ||                 // هامش
                                            prop.key == "width" || prop.key == "height" ||
                                            prop.key == "padding" || prop.key == "margin" ||
                                            prop.key == "\xd8\xad\xd8\xac\xd9\x85_\xd8\xa7\xd9\x84\xd8\xae\xd8\xb7" || // حجم_الخط
                                            prop.key == "fontSize" || prop.key == "font_size" ||
                                            prop.key == "\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89" || // محتوى
                                            prop.key == "content" ||
                                            prop.key == "\xd9\x86\xd8\xb5" || // نص
                                            prop.key == "text")
                                        {
                                            needsRelayout = true;
                                            break;
                                        }
                                    }
                                }
                            }
                            window->applyPatches(diffResult.size(), needsRelayout);
                        }
                    }
                    else
                    {
                        // (AR) فشل التطبيق الموضعي → fallback لاستبدال كامل
                        // (EN) Patch failed → fallback to full replacement
                        previousIR_ = newIR;
                        window->setContent(newIR);
                    }
                }
                else
                {
                    // (AR) لا توجد شجرة سابقة — أول عرض أو بعد التنقل
                    // (EN) No previous tree — first render or after navigation
                    previousIR_ = newIR;

                    if (!pendingTransitionType_.empty())
                    {
                        if (!pendingExitTransitionType_.empty())
                        {
                            window->setContentWithExitTransition(
                                newIR,
                                pendingTransitionType_,
                                pendingExitTransitionType_,
                                pendingTransitionDuration_);
                            pendingExitTransitionType_.clear();
                        }
                        else
                        {
                            window->setContentWithTransition(
                                newIR,
                                pendingTransitionType_,
                                pendingTransitionDuration_);
                        }
                        pendingTransitionType_.clear();
                    }
                    else
                    {
                        window->setContent(newIR);
                    }
                }

                stateChanged_ = false;
            }
        }

        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
        // ״×״³״¬„ ˆ״§״³״×״¯״¹״§״¡ handlers ״§„״£״­״¯״§״«
        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
        // Tree Diffing ג€” …‚״§״±†״© ״´״¬״±״×† IR „״§ƒ״×״´״§ ״§„״×״÷״±״§״× ״§„״¨״µ״±״©
        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

        bool UIBridge::treesEqual(const std::shared_ptr<sad::ui::IRNode> &a,
                                  const std::shared_ptr<sad::ui::IRNode> &b)
        {
            // ƒ„״×״§‡…״§ ״§״±״÷״×״§† ג€” …״×״³״§ˆ״×״§†
            if (!a && !b)
                return true;
            // ״¥״­״¯״§‡…״§ ״§״±״÷״© ג€” …״®״×„״×״§†
            if (!a || !b)
                return false;

            // …‚״§״±†״© †ˆ״¹ ״§„״¹†״µ״±
            if (a->getType() != b->getType())
                return false;

            // …‚״§״±†״© ״§„״®״µ״§״¦״µ ״§„״¨״µ״±״© (†״µ״ „ˆ†״ ״£״¨״¹״§״¯...)
            const auto &aProps = a->getProperties();
            const auto &bProps = b->getProperties();
            if (aProps.size() != bProps.size())
                return false;

            for (size_t i = 0; i < aProps.size(); i++)
            {
                if (aProps[i].key != bProps[i].key)
                    return false;
                if (aProps[i].value != bProps[i].value)
                    return false;
            }

            // …‚״§״±†״© ״§„״£״­״¯״§״« (…״¹״±‘״§״× ״§„€ handlers ״­״×…״© „״£† ״§„״¹״¯‘״§״¯ ״µ‘״±)
            const auto &aEvents = a->getEvents();
            const auto &bEvents = b->getEvents();
            if (aEvents.size() != bEvents.size())
                return false;

            for (size_t i = 0; i < aEvents.size(); i++)
            {
                if (aEvents[i].type != bEvents[i].type)
                    return false;
                if (aEvents[i].expression != bEvents[i].expression)
                    return false;
            }

            // …‚״§״±†״© ״§„״£״¨†״§״¡ (״×ƒ״±״§״±״©)
            if (a->childCount() != b->childCount())
                return false;

            for (size_t i = 0; i < a->childCount(); i++)
            {
                if (!treesEqual(a->getChildren()[i], b->getChildren()[i]))
                    return false;
            }

            return true;
        }

        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

        std::string UIBridge::registerHandler(const Data::Value &handler,
                                              const std::string &stableId)
        {
            // (AR) إذا وُجد معرّف مستقر نستخدمه، وإلا نولّد معرّف بالعداد
            // (EN) Use stable ID if provided, otherwise generate from counter
            std::string id = stableId.empty()
                                 ? "__handler_" + std::to_string(handlerCounter_++)
                                 : stableId;

            // (AR) تسجيل المعالج مع رقم الجيل الحالي
            // (EN) Register handler with current generation number
            HandlerEntry entry;
            entry.handler = handler;
            entry.generation = handlerGeneration_;
            eventHandlers_[id] = std::move(entry);
            return id;
        }

        std::string UIBridge::registerHandler(const Data::Value &handler,
                                              Data::ObjectInstance *owner,
                                              const std::string &stableId)
        {
            // (AR) إذا وُجد معرّف مستقر نستخدمه، وإلا نولّد معرّف بالعداد
            std::string id = stableId.empty()
                                 ? "__handler_" + std::to_string(handlerCounter_++)
                                 : stableId;

            // (AR) تسجيل المعالج مع رقم الجيل والمالك
            HandlerEntry entry;
            entry.handler = handler;
            entry.generation = handlerGeneration_;
            if (owner)
            {
                entry.owner = owner;
                handlerOwners_[id] = owner;
            }
            eventHandlers_[id] = std::move(entry);
            return id;
        }

        Data::Value UIBridge::invokeHandler(const std::string &handlerId,
                                            const std::vector<Data::Value> &args)
        {
            auto it = eventHandlers_.find(handlerId);
            if (it == eventHandlers_.end())
            {
                return Data::Value();
            }

            const auto &handler = it->second.handler;

            // ═══════════════════════════════════════════════════════════════
            // (AR) فحص إذا كان للـ handler كائن مالك (مكون واجهة)
            //      إذا وُجد، نُنفذ الدالة في سياق الكائن (هذا + الحقول)
            //      وننسخ التعديلات على الحقول بعد التنفيذ
            // (EN) Check if handler has an owner object (UI component)
            //      If found, execute function in object context (هذا + fields)
            //      and copy field modifications back after execution
            // ═══════════════════════════════════════════════════════════════
            Data::ObjectInstance * ownerObj = nullptr;
            // (AR) أولاً نحاول من HandlerEntry.owner المباشر
            ownerObj = it->second.owner;
            // (AR) احتياط: إذا لم يكن في HandlerEntry، نبحث في handlerOwners_ القديمة
            if (!ownerObj)
            {
                auto ownerIt = handlerOwners_.find(handlerId);
                if (ownerIt != handlerOwners_.end())
                {
                    ownerObj = ownerIt->second;
                }
            }

            if (handler.isFunction())
            {
                auto funcRef = handler.toFunction();
                if (funcRef && interpreter_)
                {
                    if (ownerObj)
                    {
                        // (AR) تنفيذ في سياق الكائن المالك — يُعرّف هذا ومتغيرات الحقول
                        // (EN) Execute in owner object context — defines هذا and field variables
                        return interpreter_->callFunctionInObjectContext(
                            funcRef->registeredName, ownerObj, args);
                    }
                    return interpreter_->callUserFunction(funcRef->registeredName, args);
                }
            }
            else if (handler.isString())
            {
                const std::string &funcName = handler.toString();
                if (interpreter_)
                {
                    if (ownerObj)
                    {
                        return interpreter_->callFunctionInObjectContext(
                            funcName, ownerObj, args);
                    }
                    return interpreter_->callUserFunction(funcName, args);
                }
            }

            return Data::Value();
        }

        bool UIBridge::isEventProperty(const std::string &key) const
        {
            // (AR) نستخدم الآن الدالة المركزية من types.h
            // (EN) Now using centralized function from types.h
            return sad::ui::isKnownEventName(key);
        }

        void UIBridge::extractEventProperties(
            const std::string &key,
            const Data::Value &val,
            std::shared_ptr<sad::ui::IRNode> &irNode)
        {
            // (AR) تحويل اسم الخاصية إلى نوع حدث مركزي
            // (EN) Convert property name to centralized event type
            sad::ui::IREventType evtType = sad::ui::stringToIREventType(key);

            sad::ui::IREvent event;
            event.type = evtType;
            if (evtType == sad::ui::IREventType::Custom)
            {
                event.customEventName = key;
            }

            if (val.isFunction() || val.isCallable())
            {
                // (AR) توليد معرّف مستقر: nodeId + ":" + اسم_الحدث
                // (EN) Generate stable ID: nodeId + ":" + event_name
                std::string stableId = irNode->getId() + ":" + key;
                event.expression = registerHandler(val, stableId);
            }
            else if (val.isString())
            {
                event.expression = val.toString();
            }
            else
            {
                return;
            }

            irNode->addEvent(event);
        }

        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
        // †״¸״§… ״§„״×†‚„ (Navigation Stack)
        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

        void UIBridge::navigateTo(const Data::Value &page)
        {
            // ״­״¸ ״§„״µ״­״© ״§„״­״§„״©  ״§„ƒˆ…״©
            navigationStack_.push_back(rootWidget_);

            // ״×״¹† ״§„״µ״­״© ״§„״¬״¯״¯״© ƒ״¬״°״±
            rootWidget_ = page;

            // ״¥״¹״§״¯״© ״¨†״§״¡ ״§„ˆ״§״¬‡״©
            rebuildUI();
        }
        void UIBridge::navigateWithTransition(const Data::Value &page,
                                              const std::string &transitionType,
                                              float durationSec)
        {
            // (AR) حفظ الصفحة الحالية في الكومة
            navigationStack_.push_back(rootWidget_);

            // (AR) تعيين الصفحة الجديدة كجذر
            rootWidget_ = page;

            // (AR) تخزين نوع الانتقال المطلوب — سيُستخدم في rebuildUI
            pendingTransitionType_ = transitionType;
            pendingTransitionDuration_ = durationSec;

            // (AR) إعادة بناء الواجهة — ستكتشف rebuildUI وجود انتقال مُعلّق
            rebuildUI();
        }

        void UIBridge::navigateWithExitTransition(const Data::Value &page,
                                                  const std::string &entryTransition,
                                                  const std::string &exitTransition,
                                                  float durationSec)
        {
            // (AR) حفظ الصفحة الحالية في الكومة
            navigationStack_.push_back(rootWidget_);

            // (AR) تعيين الصفحة الجديدة كجذر
            rootWidget_ = page;

            // (AR) تخزين نوعي الانتقال (خروج + دخول) — سيُستخدمان في rebuildUI
            pendingTransitionType_ = entryTransition;
            pendingTransitionDuration_ = durationSec;
            pendingExitTransitionType_ = exitTransition;
            pendingExitTransitionDuration_ = durationSec;

            // (AR) إعادة بناء الواجهة
            rebuildUI();
        }
        bool UIBridge::navigateBackWithTransition(const std::string &transitionType,
                                                  float durationSec)
        {
            if (navigationStack_.empty())
                return false;

            // (AR) استعادة الصفحة السابقة
            rootWidget_ = navigationStack_.back();
            navigationStack_.pop_back();

            // (AR) تخزين نوع الانتقال المطلوب — سيُستخدم في rebuildUI
            pendingTransitionType_ = transitionType;
            pendingTransitionDuration_ = durationSec;

            // (AR) إعادة بناء الواجهة
            rebuildUI();
            return true;
        }
        bool UIBridge::navigateBack()
        {
            if (navigationStack_.empty())
            {
                return false;
            }

            // ״§״³״×״¹״§״¯״© ״§„״µ״­״© ״§„״³״§״¨‚״©
            rootWidget_ = navigationStack_.back();
            navigationStack_.pop_back();

            // ״¥״¹״§״¯״© ״¨†״§״¡ ״§„ˆ״§״¬‡״©
            rebuildUI();
            return true;
        }

        void UIBridge::navigateToRoot()
        {
            if (navigationStack_.empty())
                return;

            // ״§״³״×״¹״§״¯״© ״§„״µ״­״© ״§„״£ˆ„‰
            rootWidget_ = navigationStack_.front();
            navigationStack_.clear();

            // ״¥״¹״§״¯״© ״¨†״§״¡ ״§„ˆ״§״¬‡״©
            rebuildUI();
        }

        void UIBridge::replacePage(const Data::Value &page)
        {
            // ״§״³״×״¨״¯״§„ ״¨״¯ˆ† ״¥״¶״§״© „„ƒˆ…״©
            rootWidget_ = page;
            rebuildUI();
        }

        void UIBridge::showSnackbar(const std::string &message, int durationMs)
        {
            // (AR) طباعة الرسالة + طلب إعادة رسم
            std::cout << "[\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84\xd8\xa9] " << message << std::endl;

            if (activeWindow_)
            {
                activeWindow_->invalidate();
            }
        }

        void UIBridge::closeWindow()
        {
            // (AR) إغلاق النافذة عبر الواجهة المجردة — كل منصة تتعامل معها بطريقتها
            if (activeWindow_)
            {
                activeWindow_->close();
            }
        }

        void UIBridge::setWindowTitle(const std::string &title)
        {
            // (AR) تعيين عنوان النافذة عبر الواجهة المجردة
            if (activeWindow_)
            {
                activeWindow_->setTitle(title);
            }
        }

        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
        //  †״¸״§… 1: ״§„…״₪‚״×״§״× (Timer System)
        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

        int UIBridge::scheduleTimeout(const Data::Value &callback, int delayMs)
        {
            TimerEntry entry;
            entry.id = nextTimerId_++;
            entry.callback = callback;
            entry.intervalMs = 0; // …״±״© ˆ״§״­״¯״©
            entry.cancelled = false;
            entry.fireTimeMs = (activeWindow_ ? activeWindow_->getTicksMs() : 0) + static_cast<uint32_t>(delayMs);
            timers_.push_back(std::move(entry));
            return timers_.back().id;
        }

        int UIBridge::scheduleInterval(const Data::Value &callback, int intervalMs)
        {
            TimerEntry entry;
            entry.id = nextTimerId_++;
            entry.callback = callback;
            entry.intervalMs = intervalMs;
            entry.cancelled = false;
            entry.fireTimeMs = (activeWindow_ ? activeWindow_->getTicksMs() : 0) + static_cast<uint32_t>(intervalMs);
            timers_.push_back(std::move(entry));
            return timers_.back().id;
        }

        void UIBridge::cancelTimer(int timerId)
        {
            for (auto &t : timers_)
            {
                if (t.id == timerId)
                {
                    t.cancelled = true;
                    break;
                }
            }
        }

        void UIBridge::updateTimers()
        {
            if (timers_.empty())
                return;

            if (!activeWindow_)
                return;
            uint32_t now = activeWindow_->getTicksMs();

            // ״¬…״¹ ״§„…״₪‚״×״§״× ״§„…״·„ˆ״¨ ״×†״°‡״§
            std::vector<TimerEntry> toFire;
            for (auto &t : timers_)
            {
                if (!t.cancelled && now >= t.fireTimeMs)
                {
                    toFire.push_back(t);
                    if (t.intervalMs > 0)
                    {
                        // ״×ƒ״±״§״±: ״¥״¹״§״¯״© ״¬״¯ˆ„״©
                        t.fireTimeMs = now + static_cast<uint32_t>(t.intervalMs);
                    }
                    else
                    {
                        t.cancelled = true; // …״±״© ˆ״§״­״¯״©: ״¥„״÷״§״¡ ״¨״¹״¯ ״§„״×†״°
                    }
                }
            }

            // ״×†״° ״§„״¯ˆ״§„
            for (const auto &t : toFire)
            {
                if (t.callback.isFunction())
                {
                    auto funcRef = t.callback.toFunction();
                    if (funcRef && interpreter_)
                    {
                        try
                        {
                            interpreter_->callUserFunction(funcRef->registeredName, {});
                        }
                        catch (...)
                        {
                        }
                    }
                }
            }

            // ״×†״¸ ״§„…״₪‚״×״§״× ״§„…„״÷״©
            timers_.erase(
                std::remove_if(timers_.begin(), timers_.end(),
                               [](const TimerEntry &t)
                               { return t.cancelled; }),
                timers_.end());

            // ״¥״¹״§״¯״© ״±״³… ״¥† ƒ״§† ‡†״§ƒ …״₪‚״×״§״× †״°״×
            if (!toFire.empty())
            {
                rebuildUI();
            }
        }

        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
        //  †״¸״§… 2: ״§„״­״§״¸״© (Clipboard System)
        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

        bool UIBridge::copyToClipboard(const std::string &text)
        {
            if (activeWindow_)
            {
                return activeWindow_->copyToClipboard(text);
            }
            return false;
        }

        std::string UIBridge::pasteFromClipboard()
        {
            if (activeWindow_)
            {
                return activeWindow_->pasteFromClipboard();
            }
            return "";
        }

        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
        //  †״¸״§… 3: ״§„״­ˆ״§״±״§״× (Dialog System)
        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

        void UIBridge::showAlert(const std::string &title, const std::string &message)
        {
            if (activeWindow_)
            {
                activeWindow_->showAlertDialog(title, message);
            }
        }

        bool UIBridge::showConfirm(const std::string &title, const std::string &message)
        {
            if (activeWindow_)
            {
                return activeWindow_->showConfirmDialog(title, message);
            }
            return true;
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // (AR) تسجيل أحداث WidgetBuilder تكرارياً:
        //      يمر على جميع الأبناء WidgetBuilder ويسجّل أحداثهم (__event_*)
        //      في eventHandlers_ ليتمكن handleEvent من استدعائها لاحقاً.
        //      يُستدعى من convertNodeToIR بعد تسجيل أحداث العنصر الجذر.
        // (EN) Recursively register events from child WidgetBuilders.
        //      Called from convertNodeToIR after registering root events.
        // ═══════════════════════════════════════════════════════════════════════════════
        void UIBridge::registerWidgetBuilderEvents(
            const std::shared_ptr<sad::ui::IRNode> &irNode,
            Sad::Interpreter::WidgetBuilder *wb)
        {
            if (!wb)
                return;

            // (AR) تمرّ على الأبناء WidgetBuilder (المحفوظين عند addChildBuilder)
            const auto &childBuilders = wb->getChildBuilders();
            const auto &childIRNodes = irNode->getChildren();

            for (size_t i = 0; i < childBuilders.size() && i < childIRNodes.size(); ++i)
            {
                auto *childWBPtr = childBuilders[i];
                if (!childWBPtr)
                    continue;
                auto *childWB = childWBPtr;

                auto &childIR = childIRNodes[i];

                // (AR) تسجيل أحداث هذا الابن
                for (auto &[key, val] : childWB->fields)
                {
                    if (key.rfind("__event_", 0) == 0)
                    {
                        // (AR) استخراج owner من حقل __owner إن وجد
                        Data::ObjectInstance * ownerPtr = nullptr;
                        auto *ownerField = childWB->getField("__owner");
                        if (ownerField && ownerField->isObject())
                        {
                            ownerPtr = ownerField->toObject();
                        }

                        std::string stableId = childIR->getId() + ":" + key;
                        std::string handlerId = registerHandler(val, ownerPtr, stableId);
                        auto &events = childIR->getMutableEvents();
                        for (auto &evt : events)
                        {
                            if (evt.expression == key)
                            {
                                evt.expression = handlerId;
                                break;
                            }
                        }
                    }
                }

                // (AR) تمرير تكراري للأحفاد
                registerWidgetBuilderEvents(childIR, childWB);
            }
        }

    } // namespace Interpreter
} // namespace Sad
