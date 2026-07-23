/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: touch_processor.cpp
 * المسار: features/graphics/core/src/touch_processor.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * (AR) تنفيذ معالج أحداث اللمس الموحد.
 *      يتتبع الأصابع ويكشف الإيماءات ويُطلق أحداث IR.
 *
 * (EN) Implementation of unified touch event processor.
 *      Tracks fingers, detects gestures, fires IR events.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/touch_processor.h"

#include <algorithm>
#include <chrono>

namespace sad
{
    namespace ui
    {

        // ═══════════════════════════════════════════════════════════════════════════════
        // مساعدات داخلية
        // ═══════════════════════════════════════════════════════════════════════════════

        uint32_t TouchEventProcessor::now() const
        {
            if (getTimeMsCb_)
                return getTimeMsCb_();
            // fallback: steady_clock
            auto tp = std::chrono::steady_clock::now();
            return static_cast<uint32_t>(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    tp.time_since_epoch())
                    .count());
        }

        EventData TouchEventProcessor::makeTouchEventData(
            float x, float y, int64_t touchId, int64_t fingerId,
            float pressure) const
        {
            EventData data;
            data.x = x;
            data.y = y;
            data.touchId = touchId;
            data.fingerId = fingerId;
            data.pressure = pressure;
            data.touchCount = static_cast<int>(activeFingers_.size());
            data.isTouch = true;
            data.button = 0;
            return data;
        }

        void TouchEventProcessor::reset()
        {
            activeFingers_.clear();
            pinchActive_ = false;
            rotateActive_ = false;
            initialPinchDistance_ = 0.0f;
            initialRotationAngle_ = 0.0f;
            lastTapTime_ = 0;
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // onFingerDown — إصبع لمس الشاشة
        // ═══════════════════════════════════════════════════════════════════════════════

        void TouchEventProcessor::onFingerDown(
            int64_t touchId, int64_t fingerId,
            float x, float y, float pressure)
        {
            // (AR) تسجيل الإصبع في خريطة التتبع
            int64_t key = fingerKey(touchId, fingerId);
            FingerState fs;
            fs.touchId = touchId;
            fs.fingerId = fingerId;
            fs.startX = x;
            fs.startY = y;
            fs.currentX = x;
            fs.currentY = y;
            fs.pressure = pressure;
            fs.startTime = now();
            fs.dragStarted = false;
            activeFingers_[key] = fs;

            // (AR) إطلاق حدث عند_النقر (OnTap)
            if (hitTestCb_ && fireEventCb_)
            {
                const auto *node = hitTestCb_(x, y);
                if (node)
                {
                    EventData data = makeTouchEventData(x, y, touchId, fingerId, pressure);
                    for (const auto &evt : node->getEvents())
                    {
                        if (evt.type == IREventType::OnTap)
                        {
                            fireEventCb_(evt.type, evt.expression, node, data);
                        }
                    }
                }
            }

            // (AR) إذا صار لدينا إصبعان، نبدأ تتبع إيماءة القرص والدوران
            if (activeFingers_.size() == 2)
            {
                auto it = activeFingers_.begin();
                const auto &f1 = it->second;
                ++it;
                const auto &f2 = it->second;
                initialPinchDistance_ = fingerDistance(f1, f2);
                initialRotationAngle_ = fingerAngle(f1, f2);
                pinchActive_ = true;
                rotateActive_ = true;
            }
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // onFingerUp — إصبع رُفع عن الشاشة
        // ═══════════════════════════════════════════════════════════════════════════════

        void TouchEventProcessor::onFingerUp(
            int64_t touchId, int64_t fingerId,
            float x, float y)
        {
            int64_t key = fingerKey(touchId, fingerId);
            auto it = activeFingers_.find(key);
            if (it == activeFingers_.end())
                return;

            const FingerState &fs = it->second;
            uint32_t duration = now() - fs.startTime;
            float dx = x - fs.startX;
            float dy = y - fs.startY;
            float absDx = std::abs(dx);
            float absDy = std::abs(dy);

            // (AR) كشف الإيماءة وإطلاق الحدث المناسب
            if (hitTestCb_ && fireEventCb_)
            {
                const auto *node = hitTestCb_(x, y);

                if (absDx > swipeThreshold || absDy > swipeThreshold)
                {
                    // ─── Swipe ───
                    IREventType swipeType;
                    if (absDx > absDy)
                    {
                        swipeType = (dx > 0) ? IREventType::OnSwipeRight : IREventType::OnSwipeLeft;
                    }
                    else
                    {
                        swipeType = (dy > 0) ? IREventType::OnSwipeDown : IREventType::OnSwipeUp;
                    }

                    if (node)
                    {
                        EventData data = makeTouchEventData(x, y, touchId, fingerId);
                        data.deltaX = dx;
                        data.deltaY = dy;
                        for (const auto &evt : node->getEvents())
                        {
                            // (AR) OnDrag المستمرّ (دلتا تفاضليّة) أُطلق أثناء الحركة؛
                            //      فلا نُعيده هنا بالدلتا *الإجماليّة* إن جرى سحبٌ فعليّ،
                            //      وإلّا قفز العنصر ضِعف المسافة عند رفع الإصبع.
                            if (evt.type == swipeType ||
                                (evt.type == IREventType::OnDrag && !fs.dragStarted))
                            {
                                fireEventCb_(evt.type, evt.expression, node, data);
                            }
                        }
                    }
                }
                else if (duration >= longPressMs)
                {
                    // ─── Long Press ───
                    if (node)
                    {
                        EventData data = makeTouchEventData(x, y, touchId, fingerId);
                        for (const auto &evt : node->getEvents())
                        {
                            if (evt.type == IREventType::OnLongPress)
                            {
                                fireEventCb_(evt.type, evt.expression, node, data);
                            }
                        }
                    }
                }
                else
                {
                    // ─── Tap / DoubleTap ───
                    uint32_t currentTime = now();
                    if ((currentTime - lastTapTime_) <= doubleTapMs && lastTapTime_ != 0)
                    {
                        // نقرة مزدوجة
                        if (node)
                        {
                            EventData data = makeTouchEventData(x, y, touchId, fingerId);
                            for (const auto &evt : node->getEvents())
                            {
                                if (evt.type == IREventType::OnDoubleTap)
                                {
                                    fireEventCb_(evt.type, evt.expression, node, data);
                                }
                            }
                        }
                        lastTapTime_ = 0;
                    }
                    else
                    {
                        lastTapTime_ = currentTime;
                    }
                }

                // (AR) إطلاق حدث انتهاء السحب إذا كان الإصبع قد تحرك
                if (fs.dragStarted && node)
                {
                    EventData data = makeTouchEventData(x, y, touchId, fingerId);
                    data.deltaX = dx;
                    data.deltaY = dy;
                    for (const auto &evt : node->getEvents())
                    {
                        if (evt.type == IREventType::OnDragEnd)
                        {
                            fireEventCb_(evt.type, evt.expression, node, data);
                        }
                    }
                }
            }

            // (AR) حذف الإصبع من خريطة التتبع
            activeFingers_.erase(it);

            // (AR) إنهاء إيماءات الإصبعين إذا لم يعد هناك إصبعان
            if (activeFingers_.size() < 2)
            {
                pinchActive_ = false;
                rotateActive_ = false;
                initialPinchDistance_ = 0.0f;
                initialRotationAngle_ = 0.0f;
            }
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // onFingerMotion — إصبع يتحرك على الشاشة
        // ═══════════════════════════════════════════════════════════════════════════════

        void TouchEventProcessor::onFingerMotion(
            int64_t touchId, int64_t fingerId,
            float x, float y,
            float dx, float dy,
            float pressure)
        {
            int64_t key = fingerKey(touchId, fingerId);

            // (AR) تحديث موقع الإصبع
            auto it = activeFingers_.find(key);
            if (it != activeFingers_.end())
            {
                it->second.currentX = x;
                it->second.currentY = y;
                it->second.pressure = pressure;
            }

            if (!hitTestCb_ || !fireEventCb_)
                return;

            int touchCount = static_cast<int>(activeFingers_.size());

            // ─── إيماءة القرص/التكبير (Pinch-to-Zoom) — إصبعان ───
            if (pinchActive_ && touchCount == 2 && initialPinchDistance_ > 0.0f)
            {
                auto fit = activeFingers_.begin();
                const auto &f1 = fit->second;
                ++fit;
                const auto &f2 = fit->second;

                float currentDist = fingerDistance(f1, f2);
                float scale = currentDist / initialPinchDistance_;
                float midX, midY;
                fingerMidpoint(f1, f2, midX, midY);

                const auto *zoomNode = hitTestCb_(midX, midY);
                if (zoomNode)
                {
                    EventData data = makeTouchEventData(midX, midY, touchId, fingerId, pressure);
                    data.deltaY = scale - 1.0f; // >0 تكبير, <0 تصغير
                    data.value = std::to_string(scale);
                    for (const auto &evt : zoomNode->getEvents())
                    {
                        if (evt.type == IREventType::OnZoom)
                        {
                            fireEventCb_(evt.type, evt.expression, zoomNode, data);
                        }
                    }
                }

                // تحديث المسافة المرجعية للقرص المستمر
                initialPinchDistance_ = currentDist;
            }

            // ─── إيماءة الدوران (Rotate) — إصبعان ───
            if (rotateActive_ && touchCount == 2)
            {
                auto fit = activeFingers_.begin();
                const auto &f1 = fit->second;
                ++fit;
                const auto &f2 = fit->second;

                float currentAngle = fingerAngle(f1, f2);
                float dTheta = currentAngle - initialRotationAngle_;

                if (std::abs(dTheta) > rotateThreshold)
                {
                    float midX, midY;
                    fingerMidpoint(f1, f2, midX, midY);

                    const auto *rotateNode = hitTestCb_(midX, midY);
                    if (rotateNode)
                    {
                        EventData data = makeTouchEventData(midX, midY, touchId, fingerId, pressure);
                        data.angle = dTheta;
                        for (const auto &evt : rotateNode->getEvents())
                        {
                            if (evt.type == IREventType::OnRotate)
                            {
                                fireEventCb_(evt.type, evt.expression, rotateNode, data);
                            }
                        }
                    }

                    // تحديث الزاوية المرجعية
                    initialRotationAngle_ = currentAngle;
                }
            }

            // ─── السحب بإصبع واحد (Single-finger Drag) ───
            if (touchCount == 1 && it != activeFingers_.end())
            {
                float totalDx = std::abs(x - it->second.startX);
                float totalDy = std::abs(y - it->second.startY);

                if (totalDx > dragThreshold || totalDy > dragThreshold)
                {
                    // (AR) العنصر المُمسَك يُلتقط مرّةً من *نقطة بدء اللمس* ويُستعمل
                    //      طوال السحب (دلالة الإمساك، نظير معالج الفأرة): اختبار
                    //      النقر بالموضع الحاليّ كان ينقل السحب لعنصر آخر أو يوقفه
                    //      فور خروج الإصبع من العنصر — وهو حتميّ هنا لأنّ العنصر
                    //      المسحوب يتحرّك تحت الإصبع.
                    if (!it->second.dragStarted)
                        it->second.dragNode = hitTestCb_(it->second.startX, it->second.startY);
                    const auto *touchNode = it->second.dragNode;
                    if (touchNode)
                    {
                        EventData data = makeTouchEventData(x, y, touchId, fingerId, pressure);
                        data.deltaX = dx;
                        data.deltaY = dy;

                        // (AR) إطلاق OnDragStart مرة واحدة فقط
                        if (!it->second.dragStarted)
                        {
                            it->second.dragStarted = true;
                            for (const auto &evt : touchNode->getEvents())
                            {
                                if (evt.type == IREventType::OnDragStart)
                                {
                                    fireEventCb_(evt.type, evt.expression, touchNode, data);
                                }
                            }
                        }

                        // (AR) إطلاق OnDrag مستمر
                        for (const auto &evt : touchNode->getEvents())
                        {
                            if (evt.type == IREventType::OnDrag)
                            {
                                fireEventCb_(evt.type, evt.expression, touchNode, data);
                            }
                        }
                    }
                }
            }
        }

    } // namespace ui
} // namespace sad
