/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: mouse_processor.cpp
 * المسار: features/graphics/core/src/mouse_processor.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * (AR) تنفيذ معالج أحداث الفأرة/المؤشر الموحد
 *
 * هيكل المعالجة:
 *
 *   onMouseDown(button, x, y)
 *   ├── Right button → ContextMenu
 *   ├── تحديث التركيز (focusedNode_)
 *   ├── Toggle/Checkbox → تبديل مفعّل + OnChange
 *   ├── Radio → تحديد + إلغاء المجموعة + OnChange
 *   ├── Slider → حساب القيمة من X + OnChange
 *   ├── OnTap → إطلاق على العنصر المنقور
 *   └── بدء تتبع الإيماءة (gestureStart)
 *
 *   onMouseUp(button, x, y)
 *   ├── كشف الإيماءة (Swipe/LongPress/DoubleTap)
 *   ├── DragEnd + Drop
 *   └── إنهاء Slider drag
 *
 *   onMouseMove(x, y, buttons)
 *   ├── Slider drag → تحديث القيمة
 *   ├── Hover tracking (OnHover/OnHoverExit)
 *   ├── تغيير شكل المؤشر
 *   └── Drag tracking (DragStart/DragMove)
 *
 *   onMouseWheel(dx, dy, mouseX, mouseY, ctrl)
 *   ├── Ctrl+Scroll → OnZoom
 *   ├── ScrollView/LazyColumn → تمرير
 *   └── OnScroll على العنصر
 *
 * (EN) Implementation of the unified mouse/pointer event processor.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/mouse_processor.h"
#include "sad_ui/prop_keys.h" // مفاتيح الخصائص القانونيّة (SoT) — لا literals خام
#include <cmath>
#include <variant>

namespace sad
{
    namespace ui
    {

        // ═══════════════════════════════════════════════════════════════════════════════
        // onMouseDown — ضغط زر الفأرة
        // ═══════════════════════════════════════════════════════════════════════════════

        void MouseEventProcessor::onMouseDown(MouseButton button, float x, float y)
        {
            // ─── 1. نقرة يمنى → قائمة سياقية (ContextMenu) ───
            if (button == MouseButton::Right)
            {
                if (!hitTestCb_ || !fireEventCb_)
                    return;

                const auto *ctxNode = hitTestCb_(x, y);
                if (ctxNode)
                {
                    EventData data;
                    data.x = x;
                    data.y = y;
                    data.button = 2; // أيمن
                    for (const auto &evt : ctxNode->getEvents())
                    {
                        if (evt.type == IREventType::OnContextMenu)
                        {
                            fireEventCb_(evt.type, evt.expression, ctxNode, data);
                        }
                    }
                }
                return; // النقرة اليمنى لا تُكمل بقية المعالجة
            }

            // ─── 2. تسجيل بداية الإيماءة ───
            gestureStartX_ = x;
            gestureStartY_ = y;
            gestureStartTime_ = getTimeMsCb_ ? getTimeMsCb_() : 0;
            gestureActive_ = true;

            // ─── 3. البحث عن العنصر المنقور ───
            const IRNode *hitNode = hitTestCb_ ? hitTestCb_(x, y) : nullptr;
            pressedNode_ = hitNode;

            // ─── 4. تحديث التركيز ───
            //     النقر على أي عنصر يُركّزه؛ المنصة تدير SDL_StartTextInput وغيرها
            if (getFocusedNodeCb_ && setFocusedNodeCb_)
            {
                const IRNode *currentFocus = getFocusedNodeCb_();
                if (hitNode != currentFocus)
                {
                    setFocusedNodeCb_(hitNode);
                    if (invalidateCb_)
                        invalidateCb_();
                }
            }

            if (!hitNode)
                return;

            auto nodeType = hitNode->getType();

            // ─── 5. معالجة عناصر تفاعلية خاصة ───

            // Toggle / Checkbox — تبديل
            if (nodeType == UINodeType::Toggle || nodeType == UINodeType::Checkbox)
            {
                handleToggleCheckbox(hitNode, x, y);
            }

            // Radio — تحديد
            if (nodeType == UINodeType::Radio)
            {
                handleRadio(hitNode, x, y);
            }

            // Slider — بدء سحب
            if (nodeType == UINodeType::Slider)
            {
                draggedSliderNode_ = hitNode;
                handleSliderDown(hitNode, x);
            }

            // ─── 6. إطلاق أحداث النقر العادية (OnTap) ───
            fireTapEvents(hitNode, x, y);

            if (invalidateCb_)
                invalidateCb_();
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // onMouseUp — رفع زر الفأرة
        // ═══════════════════════════════════════════════════════════════════════════════

        void MouseEventProcessor::onMouseUp(MouseButton button, float x, float y)
        {
            // ─── 1. كشف الإيماءات ───
            if (gestureActive_)
            {
                gestureActive_ = false;
                detectGesture(x, y);
            }

            // ─── 2. إنهاء الضغط ───
            if (pressedNode_)
            {
                pressedNode_ = nullptr;
                if (invalidateCb_)
                    invalidateCb_();
            }

            // ─── 3. إنهاء سحب المنزلق ───
            if (draggedSliderNode_)
            {
                draggedSliderNode_ = nullptr;
            }

            // ─── 4. DragEnd / Drop ───
            if (dragging_)
            {
                if (hitTestCb_ && fireEventCb_)
                {
                    const auto *dragSource = hitTestCb_(gestureStartX_, gestureStartY_);
                    const auto *dropTarget = hitTestCb_(x, y);

                    // (AR) إطلاق OnDragEnd على العنصر الذي بدأ منه السحب
                    if (dragSource)
                    {
                        EventData data;
                        data.x = x;
                        data.y = y;
                        data.deltaX = x - gestureStartX_;
                        data.deltaY = y - gestureStartY_;
                        for (const auto &evt : dragSource->getEvents())
                        {
                            if (evt.type == IREventType::OnDragEnd)
                            {
                                fireEventCb_(evt.type, evt.expression, dragSource, data);
                            }
                        }
                    }

                    // (AR) إطلاق OnDrop على العنصر المُسقَط عليه (إن كان مختلفاً)
                    if (dropTarget && dropTarget != dragSource)
                    {
                        EventData data;
                        data.x = x;
                        data.y = y;
                        for (const auto &evt : dropTarget->getEvents())
                        {
                            if (evt.type == IREventType::OnDrop)
                            {
                                fireEventCb_(evt.type, evt.expression, dropTarget, data);
                            }
                        }
                    }
                }
                dragging_ = false;
            }
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // onMouseMove — حركة المؤشر
        // ═══════════════════════════════════════════════════════════════════════════════

        void MouseEventProcessor::onMouseMove(float x, float y, const MouseButtonState &buttons)
        {
            mouseX_ = x;
            mouseY_ = y;

            // ─── 1. سحب المنزلق (Slider) ───
            if (draggedSliderNode_ && buttons.leftPressed)
            {
                handleSliderDrag(x);
            }

            // ─── 2. تتبع التحويم (Hover) ───
            handleHoverTracking(x, y);

            // ─── 3. سحب (Drag) — إذا الزر الأيسر مضغوط ───
            if (buttons.leftPressed)
            {
                handleDragTracking(x, y);
            }
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // onMouseWheel — عجلة التمرير
        // ═══════════════════════════════════════════════════════════════════════════════

        void MouseEventProcessor::onMouseWheel(float deltaX, float deltaY,
                                               float mouseX, float mouseY,
                                               bool ctrlPressed)
        {
            // ─── 1. Ctrl+Scroll → Zoom (تكبير/تصغير) ───
            if (ctrlPressed)
            {
                if (hitTestCb_ && fireEventCb_)
                {
                    const auto *zoomNode = hitTestCb_(mouseX, mouseY);
                    if (zoomNode)
                    {
                        EventData data;
                        data.x = mouseX;
                        data.y = mouseY;
                        data.deltaY = deltaY;
                        for (const auto &evt : zoomNode->getEvents())
                        {
                            if (evt.type == IREventType::OnZoom)
                            {
                                fireEventCb_(evt.type, evt.expression, zoomNode, data);
                            }
                        }
                    }
                }
            }

            // ─── 2. تمرير ScrollView/LazyColumn ───
            if (findScrollableAtCb_ && setScrollOffsetCb_)
            {
                auto scrollHit = findScrollableAtCb_(mouseX, mouseY);
                if (scrollHit.node && scrollHit.contentHeight > scrollHit.viewHeight)
                {
                    float scrollDelta = deltaY * 40.0f; // 40 بكسل لكل خطوة
                    float newOffset = scrollHit.currentScrollOffset + scrollDelta;

                    // (AR) تقييد الإزاحة: بين -(contentHeight - viewHeight) و 0
                    float maxScroll = scrollHit.contentHeight - scrollHit.viewHeight;
                    if (newOffset > 0.0f)
                        newOffset = 0.0f;
                    if (newOffset < -maxScroll)
                        newOffset = -maxScroll;

                    setScrollOffsetCb_(scrollHit.node, newOffset);
                    if (invalidateCb_)
                        invalidateCb_();
                }
            }

            // ─── 3. إطلاق OnScroll على العنصر تحت المؤشر ───
            if (hitTestCb_ && fireEventCb_)
            {
                const auto *scrollNode = hitTestCb_(mouseX, mouseY);
                if (scrollNode)
                {
                    for (const auto &evt : scrollNode->getEvents())
                    {
                        if (evt.type == IREventType::OnScroll)
                        {
                            EventData data;
                            data.x = mouseX;
                            data.y = mouseY;
                            data.deltaY = deltaY;
                            fireEventCb_(evt.type, evt.expression, scrollNode, data);
                        }
                    }
                }
            }
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // دوال مساعدة — معالجة عناصر خاصة
        // ═══════════════════════════════════════════════════════════════════════════════

        void MouseEventProcessor::handleToggleCheckbox(const IRNode *node, float x, float y)
        {
            // (AR) تبديل خاصية "مفعّل" ثم إطلاق OnChange
            auto *mutableNode = const_cast<IRNode *>(node);
            const auto *prop = node->findProperty(props::ENABLED); // مفعّل
            bool wasOn = false;
            if (prop)
            {
                if (auto *b = std::get_if<bool>(&prop->value))
                    wasOn = *b;
                else if (auto *s = std::get_if<std::string>(&prop->value))
                {
                    wasOn = (*s == "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad" || // صحيح
                             *s == "true" || *s == "1" ||
                             *s == "\xd9\x85\xd9\x81\xd8\xb9\xd9\x84"); // مفعّل
                }
            }
            mutableNode->setProperty("\xd9\x85\xd9\x81\xd8\xb9\xd9\x84", !wasOn);

            if (fireEventCb_)
            {
                for (const auto &evt : node->getEvents())
                {
                    if (evt.type == IREventType::OnChange)
                    {
                        EventData data;
                        data.x = x;
                        data.y = y;
                        fireEventCb_(evt.type, evt.expression, node, data);
                    }
                }
            }
        }

        void MouseEventProcessor::handleRadio(const IRNode *node, float x, float y)
        {
            // (AR) تحديد هذا الزر + إلغاء تحديد الأخوة في نفس المجموعة
            auto *mutableNode = const_cast<IRNode *>(node);
            mutableNode->setProperty("\xd9\x85\xd9\x81\xd8\xb9\xd9\x84", true); // مفعّل

            // استخراج اسم المجموعة
            const auto *groupProp = node->findProperty(props::GROUP); // مجموعة
            std::string group;
            if (groupProp)
            {
                if (auto *s = std::get_if<std::string>(&groupProp->value))
                    group = *s;
            }

            // إلغاء تحديد الأخوة
            deselectRadioSiblings(node, group);

            // إطلاق OnChange
            if (fireEventCb_)
            {
                for (const auto &evt : node->getEvents())
                {
                    if (evt.type == IREventType::OnChange)
                    {
                        EventData data;
                        data.x = x;
                        data.y = y;
                        fireEventCb_(evt.type, evt.expression, node, data);
                    }
                }
            }
        }

        void MouseEventProcessor::deselectRadioSiblings(const IRNode *selectedNode,
                                                        const std::string &group)
        {
            // (AR) إلغاء تحديد جميع أزرار Radio في نفس المجموعة
            //      يتطلب الوصول إلى contentRoot عبر callback
            if (!getContentRootCb_)
                return;
            const IRNode *root = getContentRootCb_();
            if (!root)
                return;

            std::function<void(const IRNode &)> deselect;
            deselect = [&](const IRNode &parent)
            {
                for (const auto &child : parent.getChildren())
                {
                    if (child.get() != selectedNode && child->getType() == UINodeType::Radio)
                    {
                        bool sameGroup = group.empty();
                        if (!group.empty())
                        {
                            const auto *gp = child->findProperty(props::GROUP); // مجموعة
                            if (gp)
                            {
                                if (auto *gs = std::get_if<std::string>(&gp->value))
                                    sameGroup = (*gs == group);
                            }
                        }
                        if (sameGroup)
                        {
                            const_cast<IRNode *>(child.get())->setProperty("\xd9\x85\xd9\x81\xd8\xb9\xd9\x84", false);
                        }
                    }
                    deselect(*child);
                }
            };
            deselect(*root);
        }

        void MouseEventProcessor::handleSliderDown(const IRNode *node, float x)
        {
            // (AR) حساب قيمة المنزلق من موقع النقر
            if (!findNodeRectCb_)
                return;

            LayoutRect sliderRect = findNodeRectCb_(node);
            if (sliderRect.width <= 0)
                return;

            float ratio = (x - sliderRect.x) / sliderRect.width;
            if (ratio < 0.0f)
                ratio = 0.0f;
            if (ratio > 1.0f)
                ratio = 1.0f;

            int64_t newVal = static_cast<int64_t>(ratio * 100.0f);
            const_cast<IRNode *>(node)->setProperty(
                "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9", newVal); // قيمة

            if (fireEventCb_)
            {
                for (const auto &evt : node->getEvents())
                {
                    if (evt.type == IREventType::OnChange)
                    {
                        EventData data;
                        data.x = x;
                        data.y = 0;
                        fireEventCb_(evt.type, evt.expression, node, data);
                    }
                }
            }
        }

        void MouseEventProcessor::handleSliderDrag(float mouseX)
        {
            // (AR) تحديث قيمة المنزلق أثناء السحب المستمر
            if (!draggedSliderNode_ || !findNodeRectCb_)
                return;

            LayoutRect sliderRect = findNodeRectCb_(draggedSliderNode_);
            if (sliderRect.width <= 0)
                return;

            float ratio = (mouseX - sliderRect.x) / sliderRect.width;
            if (ratio < 0.0f)
                ratio = 0.0f;
            if (ratio > 1.0f)
                ratio = 1.0f;

            int64_t newVal = static_cast<int64_t>(ratio * 100.0f);
            const_cast<IRNode *>(draggedSliderNode_)->setProperty("\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9", newVal); // قيمة

            if (fireEventCb_)
            {
                for (const auto &evt : draggedSliderNode_->getEvents())
                {
                    if (evt.type == IREventType::OnChange)
                    {
                        EventData data;
                        data.x = mouseX;
                        data.y = mouseY_;
                        data.value = std::to_string(newVal);
                        fireEventCb_(evt.type, evt.expression, draggedSliderNode_, data);
                    }
                }
            }

            if (invalidateCb_)
                invalidateCb_();
        }

        void MouseEventProcessor::fireTapEvents(const IRNode *node, float x, float y)
        {
            // (AR) إطلاق أحداث OnTap على العنصر المنقور
            if (!fireEventCb_)
                return;

            for (const auto &evt : node->getEvents())
            {
                if (evt.type == IREventType::OnTap)
                {
                    EventData data;
                    data.x = x;
                    data.y = y;
                    data.button = 0;
                    fireEventCb_(evt.type, evt.expression, node, data);
                }
            }
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // كشف الإيماءات
        // ═══════════════════════════════════════════════════════════════════════════════

        void MouseEventProcessor::detectGesture(float x, float y)
        {
            // (AR) تحليل الحركة والزمن بين الضغط والرفع لتحديد نوع الإيماءة
            uint32_t now = getTimeMsCb_ ? getTimeMsCb_() : 0;
            uint32_t duration = now - gestureStartTime_;
            float dx = x - gestureStartX_;
            float dy = y - gestureStartY_;
            float absDx = std::abs(dx);
            float absDy = std::abs(dy);

            const IRNode *gestureNode = hitTestCb_ ? hitTestCb_(x, y) : nullptr;

            if (absDx > MOUSE_SWIPE_THRESHOLD || absDy > MOUSE_SWIPE_THRESHOLD)
            {
                // ─── Swipe — سحب سريع ───
                IREventType irSwipeType;
                if (absDx > absDy)
                {
                    irSwipeType = (dx > 0) ? IREventType::OnSwipeRight : IREventType::OnSwipeLeft;
                }
                else
                {
                    irSwipeType = (dy > 0) ? IREventType::OnSwipeDown : IREventType::OnSwipeUp;
                }

                if (gestureNode && fireEventCb_)
                {
                    EventData data;
                    data.x = x;
                    data.y = y;
                    data.deltaX = dx;
                    data.deltaY = dy;
                    for (const auto &evt : gestureNode->getEvents())
                    {
                        if (evt.type == irSwipeType || evt.type == IREventType::OnDrag)
                        {
                            fireEventCb_(evt.type, evt.expression, gestureNode, data);
                        }
                    }
                }
            }
            else if (duration >= MOUSE_LONG_PRESS_MS)
            {
                // ─── LongPress — ضغط مطول ───
                if (gestureNode && fireEventCb_)
                {
                    EventData data;
                    data.x = x;
                    data.y = y;
                    for (const auto &evt : gestureNode->getEvents())
                    {
                        if (evt.type == IREventType::OnLongPress)
                        {
                            fireEventCb_(evt.type, evt.expression, gestureNode, data);
                        }
                    }
                }
            }
            else
            {
                // ─── Tap / DoubleTap — نقرة / نقرة مزدوجة ───
                if ((now - lastTapTime_) <= MOUSE_DOUBLE_TAP_MS)
                {
                    // نقرة مزدوجة
                    if (gestureNode && fireEventCb_)
                    {
                        EventData data;
                        data.x = x;
                        data.y = y;
                        for (const auto &evt : gestureNode->getEvents())
                        {
                            if (evt.type == IREventType::OnDoubleTap)
                            {
                                fireEventCb_(evt.type, evt.expression, gestureNode, data);
                            }
                        }
                    }
                    lastTapTime_ = 0; // إعادة ضبط
                }
                else
                {
                    lastTapTime_ = now;
                }
            }
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // تتبع التحويم (Hover)
        // ═══════════════════════════════════════════════════════════════════════════════

        void MouseEventProcessor::handleHoverTracking(float x, float y)
        {
            // (AR) تتبع دخول/خروج المؤشر من العناصر
            const IRNode *newHover = hitTestCb_ ? hitTestCb_(x, y) : nullptr;

            if (newHover != hoveredNode_)
            {
                // إطلاق OnHoverExit على العنصر السابق
                if (hoveredNode_ && fireEventCb_)
                {
                    EventData data;
                    data.x = x;
                    data.y = y;
                    for (const auto &evt : hoveredNode_->getEvents())
                    {
                        if (evt.type == IREventType::OnHoverExit)
                        {
                            fireEventCb_(evt.type, evt.expression, hoveredNode_, data);
                        }
                    }
                }

                hoveredNode_ = newHover;

                // إطلاق OnHover على العنصر الجديد
                if (hoveredNode_ && fireEventCb_)
                {
                    EventData data;
                    data.x = x;
                    data.y = y;
                    for (const auto &evt : hoveredNode_->getEvents())
                    {
                        if (evt.type == IREventType::OnHover)
                        {
                            fireEventCb_(evt.type, evt.expression, hoveredNode_, data);
                        }
                    }
                }

                // (AR) تغيير شكل المؤشر — يد عند التحويم على عناصر تفاعلية
                if (setCursorCb_)
                {
                    if (hoveredNode_ && !hoveredNode_->getEvents().empty())
                    {
                        setCursorCb_(1); // يد (Hand/Pointer)
                    }
                    else
                    {
                        setCursorCb_(0); // سهم (Arrow)
                    }
                }

                if (invalidateCb_)
                    invalidateCb_();
            }
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // تتبع السحب (Drag)
        // ═══════════════════════════════════════════════════════════════════════════════

        void MouseEventProcessor::handleDragTracking(float x, float y)
        {
            // (AR) كشف بدء السحب وتتبع الحركة أثناء الضغط
            if (!dragging_)
            {
                // التحقق من تجاوز عتبة السحب
                float dx = std::abs(x - gestureStartX_);
                float dy = std::abs(y - gestureStartY_);
                if (dx < MOUSE_DRAG_THRESHOLD && dy < MOUSE_DRAG_THRESHOLD)
                    return; // لم نتجاوز العتبة بعد

                // ─── إطلاق OnDragStart ───
                if (hitTestCb_ && fireEventCb_)
                {
                    const auto *dragStartNode = hitTestCb_(x, y);
                    if (dragStartNode)
                    {
                        EventData data;
                        data.x = x;
                        data.y = y;
                        for (const auto &evt : dragStartNode->getEvents())
                        {
                            if (evt.type == IREventType::OnDragStart)
                            {
                                fireEventCb_(evt.type, evt.expression, dragStartNode, data);
                            }
                        }
                    }
                }
                dragging_ = true;
            }

            // (AR) ملاحظة: DragMove لا يُطلق حدث IR محدد — يُمكن إضافته لاحقاً
            //      الحركة الفعلية تُعالج في المنصة (UIEvent::DragMove)
        }

    } // namespace ui
} // namespace sad
