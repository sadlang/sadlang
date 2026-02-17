// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// sad_gesture.cpp — تنفيذ نظام كشف الإيماءات المتقدم
// ─────────────────────────────────────────────────────────────────────────────
// يُنفّذ: نقرة، نقرة مزدوجة، ضغط طويل، سحب، تمرير، فيزياء التمرير
// ═══════════════════════════════════════════════════════════════════════════════
#include "sad_gesture.h"
#include "sad_ui_framework.h"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <chrono>

namespace sad { namespace gesture {

// ───── ثوابت كشف الإيماءات ─────
static const float DRAG_THRESHOLD = 8.0f;       // أقل مسافة لاعتبارها سحب (بكسل)
static const float SWIPE_MIN_VELOCITY = 300.0f;  // أقل سرعة لاعتبارها تمرير
static const float SWIPE_MIN_DISTANCE = 50.0f;   // أقل مسافة لتمرير
static const double LONG_PRESS_MS = 500.0;       // مدة الضغط الطويل
static const double DOUBLE_TAP_MS = 300.0;       // أقصى فترة بين نقرتين

// ───── مستمع إيماءة ─────
struct GestureListener {
    GestureListenerId id;
    int widgetId;
    GestureType type;
    std::function<void(const GestureData&)> callback;
};

// ───── حالة اللمس الحالية ─────
struct TouchState {
    bool isDown = false;            // هل الزر مضغوط؟
    float downX = 0, downY = 0;    // نقطة الضغط الأولى
    float lastX = 0, lastY = 0;    // آخر نقطة حركة
    float currentX = 0, currentY = 0;
    double downTime = 0;            // وقت الضغط
    double lastMoveTime = 0;        // وقت آخر حركة
    bool isDragging = false;        // هل تجاوز عتبة السحب؟
    int targetWidget = 0;           // الودجت المضغوط عليه
    
    // لكشف النقرة المزدوجة
    double lastTapTime = 0;
    float lastTapX = 0, lastTapY = 0;
    int tapCount = 0;
    
    // لكشف الضغط الطويل
    bool longPressTriggered = false;
    
    // لحساب السرعة — تتبع آخر عدة نقاط
    struct VelocitySample {
        float x, y;
        double time;
    };
    std::vector<VelocitySample> velocitySamples;
};

// ───── المتغيرات العالمية ─────
static std::vector<GestureListener> g_listeners;
static GestureListenerId g_nextListenerId = 1;
static TouchState g_touch;
static std::unordered_map<int, ScrollState> g_scrollStates;

// ═══════════════════════════════════════════════════════════════════
// دوال مساعدة داخلية
// ═══════════════════════════════════════════════════════════════════

static double now_ms() {
    auto t = std::chrono::steady_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(t).count() / 1000.0;
}

// استدعاء جميع المستمعين المطابقين
static void fireGesture(int widgetId, GestureType type, const GestureData& data) {
    for (const auto& listener : g_listeners) {
        if (listener.widgetId == widgetId && listener.type == type) {
            try {
                listener.callback(data);
            } catch (...) {}
        }
        // مستمع عالمي (widgetId == 0 يعني أي ودجت)
        if (listener.widgetId == 0 && listener.type == type) {
            try {
                listener.callback(data);
            } catch (...) {}
        }
    }
}

// حساب السرعة من العينات الأخيرة
static void calculateVelocity(float& vx, float& vy) {
    const auto& samples = g_touch.velocitySamples;
    if (samples.size() < 2) { vx = 0; vy = 0; return; }
    
    // نستخدم آخر عينتين أو متوسط مرجح
    const auto& last = samples.back();
    const auto& prev = samples[samples.size() - 2];
    double dt = (last.time - prev.time) / 1000.0; // بالثواني
    if (dt < 0.001) { vx = 0; vy = 0; return; }
    
    vx = static_cast<float>((last.x - prev.x) / dt);
    vy = static_cast<float>((last.y - prev.y) / dt);
    
    // تنعيم بمتوسط إذا توفرت عينات كافية
    if (samples.size() >= 4) {
        const auto& p2 = samples[samples.size() - 3];
        double dt2 = (prev.time - p2.time) / 1000.0;
        if (dt2 > 0.001) {
            float vx2 = static_cast<float>((prev.x - p2.x) / dt2);
            float vy2 = static_cast<float>((prev.y - p2.y) / dt2);
            vx = vx * 0.6f + vx2 * 0.4f;
            vy = vy * 0.6f + vy2 * 0.4f;
        }
    }
}

// بحث تكراري عن أعلى ودجت عند نقطة
static int findWidgetAtRecursive(int id, float x, float y) {
    sad::ui::Widget* w = sad::ui::getWidget(id);
    if (!w || !w->visible) return 0;
    
    // فحص الحدود
    if (x < w->cx || x > w->cx + w->cw || y < w->cy || y > w->cy + w->ch)
        return 0;
    
    // فحص الأبناء من الأخير (الأعلى) إلى الأول
    for (int i = (int)w->children.size() - 1; i >= 0; i--) {
        int found = findWidgetAtRecursive(w->children[i], x, y);
        if (found > 0) return found;
    }
    
    return id;
}

// ═══════════════════════════════════════════════════════════════════
// تسجيل المستمعين
// ═══════════════════════════════════════════════════════════════════

GestureListenerId onGesture(int widgetId, GestureType type,
                            std::function<void(const GestureData&)> callback) {
    GestureListener listener;
    listener.id = g_nextListenerId++;
    listener.widgetId = widgetId;
    listener.type = type;
    listener.callback = callback;
    g_listeners.push_back(listener);
    return listener.id;
}

GestureListenerId onTap(int widgetId, std::function<void(const GestureData&)> cb) {
    return onGesture(widgetId, GestureType::Tap, cb);
}
GestureListenerId onDoubleTap(int widgetId, std::function<void(const GestureData&)> cb) {
    return onGesture(widgetId, GestureType::DoubleTap, cb);
}
GestureListenerId onLongPress(int widgetId, std::function<void(const GestureData&)> cb) {
    return onGesture(widgetId, GestureType::LongPress, cb);
}
GestureListenerId onDragStart(int widgetId, std::function<void(const GestureData&)> cb) {
    return onGesture(widgetId, GestureType::DragStart, cb);
}
GestureListenerId onDragUpdate(int widgetId, std::function<void(const GestureData&)> cb) {
    return onGesture(widgetId, GestureType::DragUpdate, cb);
}
GestureListenerId onDragEnd(int widgetId, std::function<void(const GestureData&)> cb) {
    return onGesture(widgetId, GestureType::DragEnd, cb);
}
GestureListenerId onSwipe(int widgetId, std::function<void(const GestureData&)> cb) {
    // نسجّل لكل الاتجاهات
    onGesture(widgetId, GestureType::SwipeLeft, cb);
    onGesture(widgetId, GestureType::SwipeRight, cb);
    onGesture(widgetId, GestureType::SwipeUp, cb);
    return onGesture(widgetId, GestureType::SwipeDown, cb);
}
GestureListenerId onHover(int widgetId, std::function<void(const GestureData&)> cb) {
    return onGesture(widgetId, GestureType::Hover, cb);
}
GestureListenerId onHoverExit(int widgetId, std::function<void(const GestureData&)> cb) {
    return onGesture(widgetId, GestureType::HoverExit, cb);
}

// ═══════════════════════════════════════════════════════════════════
// إدارة المستمعين
// ═══════════════════════════════════════════════════════════════════

void removeGestureListener(GestureListenerId id) {
    g_listeners.erase(
        std::remove_if(g_listeners.begin(), g_listeners.end(),
            [id](const GestureListener& l) { return l.id == id; }),
        g_listeners.end()
    );
}

void removeAllGestureListeners(int widgetId) {
    g_listeners.erase(
        std::remove_if(g_listeners.begin(), g_listeners.end(),
            [widgetId](const GestureListener& l) { return l.widgetId == widgetId; }),
        g_listeners.end()
    );
}

void clearAllGestureListeners() {
    g_listeners.clear();
    g_scrollStates.clear();
}

int getGestureListenerCount() {
    return static_cast<int>(g_listeners.size());
}

// ═══════════════════════════════════════════════════════════════════
// معالجة أحداث الماوس — تُكوِّن إيماءات عالية المستوى
// ═══════════════════════════════════════════════════════════════════

void processMouseDown(float x, float y, int button) {
    if (button != 1) return; // زر يسار فقط
    
    double t = now_ms();
    
    g_touch.isDown = true;
    g_touch.downX = x;
    g_touch.downY = y;
    g_touch.lastX = x;
    g_touch.lastY = y;
    g_touch.currentX = x;
    g_touch.currentY = y;
    g_touch.downTime = t;
    g_touch.lastMoveTime = t;
    g_touch.isDragging = false;
    g_touch.longPressTriggered = false;
    g_touch.velocitySamples.clear();
    g_touch.velocitySamples.push_back({x, y, t});
    
    // البحث عن الودجت المستهدف
    g_touch.targetWidget = findWidgetAt(x, y);
    
    // بدء سحب للتمرير
    auto it = g_scrollStates.find(g_touch.targetWidget);
    if (it != g_scrollStates.end()) {
        it->second.isDragging = true;
        it->second.isAnimating = false;
        it->second.velocity = 0;
    }
}

void processMouseUp(float x, float y, int button) {
    if (button != 1 || !g_touch.isDown) return;
    
    double t = now_ms();
    float dx = x - g_touch.downX;
    float dy = y - g_touch.downY;
    float dist = std::sqrt(dx*dx + dy*dy);
    double duration = t - g_touch.downTime;
    
    float vx = 0, vy = 0;
    calculateVelocity(vx, vy);
    
    GestureData data;
    data.widgetId = g_touch.targetWidget;
    data.x = x;
    data.y = y;
    data.startX = g_touch.downX;
    data.startY = g_touch.downY;
    data.velocityX = vx;
    data.velocityY = vy;
    data.totalDeltaX = dx;
    data.totalDeltaY = dy;
    data.timestamp = t;
    
    if (g_touch.isDragging) {
        // ─── نهاية السحب ───
        data.type = GestureType::DragEnd;
        fireGesture(g_touch.targetWidget, GestureType::DragEnd, data);
        
        // كشف التمرير (Swipe)
        float speed = std::sqrt(vx*vx + vy*vy);
        if (speed > SWIPE_MIN_VELOCITY && dist > SWIPE_MIN_DISTANCE) {
            if (std::abs(dx) > std::abs(dy)) {
                // تمرير أفقي
                GestureType swipeType = dx > 0 ? GestureType::SwipeRight : GestureType::SwipeLeft;
                data.type = swipeType;
                fireGesture(g_touch.targetWidget, swipeType, data);
            } else {
                // تمرير رأسي
                GestureType swipeType = dy > 0 ? GestureType::SwipeDown : GestureType::SwipeUp;
                data.type = swipeType;
                fireGesture(g_touch.targetWidget, swipeType, data);
            }
        }
        
        // بدء حركة الزخم للتمرير
        auto it = g_scrollStates.find(g_touch.targetWidget);
        if (it != g_scrollStates.end()) {
            it->second.isDragging = false;
            if (std::abs(vy) > it->second.physics.minVelocity) {
                startMomentumScroll(g_touch.targetWidget, vy);
            }
        }
        
    } else if (!g_touch.longPressTriggered && dist < DRAG_THRESHOLD) {
        // ─── نقرة ───
        data.type = GestureType::Tap;
        
        // كشف نقرة مزدوجة
        double timeSinceLastTap = t - g_touch.lastTapTime;
        float tapDist = std::sqrt(
            (x - g_touch.lastTapX) * (x - g_touch.lastTapX) +
            (y - g_touch.lastTapY) * (y - g_touch.lastTapY)
        );
        
        if (timeSinceLastTap < DOUBLE_TAP_MS && tapDist < DRAG_THRESHOLD * 3) {
            g_touch.tapCount++;
            data.tapCount = g_touch.tapCount;
            if (g_touch.tapCount == 2) {
                data.type = GestureType::DoubleTap;
                fireGesture(g_touch.targetWidget, GestureType::DoubleTap, data);
                g_touch.tapCount = 0;
            }
        } else {
            g_touch.tapCount = 1;
            data.tapCount = 1;
        }
        
        fireGesture(g_touch.targetWidget, GestureType::Tap, data);
        
        g_touch.lastTapTime = t;
        g_touch.lastTapX = x;
        g_touch.lastTapY = y;
    }
    
    g_touch.isDown = false;
    g_touch.isDragging = false;
}

void processMouseMove(float x, float y) {
    double t = now_ms();
    
    // التحويم (Hover) — حتى لو لم يكن الزر مضغوطاً
    static int lastHoveredWidget = 0;
    int hoveredWidget = findWidgetAt(x, y);
    
    if (hoveredWidget != lastHoveredWidget) {
        // خروج من الودجت السابق
        if (lastHoveredWidget > 0) {
            GestureData exitData;
            exitData.type = GestureType::HoverExit;
            exitData.widgetId = lastHoveredWidget;
            exitData.x = x;
            exitData.y = y;
            exitData.timestamp = t;
            fireGesture(lastHoveredWidget, GestureType::HoverExit, exitData);
        }
        // دخول الودجت الجديد
        if (hoveredWidget > 0) {
            GestureData enterData;
            enterData.type = GestureType::Hover;
            enterData.widgetId = hoveredWidget;
            enterData.x = x;
            enterData.y = y;
            enterData.timestamp = t;
            fireGesture(hoveredWidget, GestureType::Hover, enterData);
        }
        lastHoveredWidget = hoveredWidget;
    }
    
    if (!g_touch.isDown) return;
    
    float dx = x - g_touch.lastX;
    float dy = y - g_touch.lastY;
    
    g_touch.currentX = x;
    g_touch.currentY = y;
    
    // إضافة عينة سرعة
    g_touch.velocitySamples.push_back({x, y, t});
    if (g_touch.velocitySamples.size() > 10) {
        g_touch.velocitySamples.erase(g_touch.velocitySamples.begin());
    }
    
    float totalDx = x - g_touch.downX;
    float totalDy = y - g_touch.downY;
    float dist = std::sqrt(totalDx*totalDx + totalDy*totalDy);
    
    if (!g_touch.isDragging && dist > DRAG_THRESHOLD) {
        // ─── بداية السحب ───
        g_touch.isDragging = true;
        
        GestureData data;
        data.type = GestureType::DragStart;
        data.widgetId = g_touch.targetWidget;
        data.x = x;
        data.y = y;
        data.startX = g_touch.downX;
        data.startY = g_touch.downY;
        data.timestamp = t;
        fireGesture(g_touch.targetWidget, GestureType::DragStart, data);
    }
    
    if (g_touch.isDragging) {
        // ─── تحديث السحب ───
        GestureData data;
        data.type = GestureType::DragUpdate;
        data.widgetId = g_touch.targetWidget;
        data.x = x;
        data.y = y;
        data.startX = g_touch.downX;
        data.startY = g_touch.downY;
        data.deltaX = dx;
        data.deltaY = dy;
        data.totalDeltaX = totalDx;
        data.totalDeltaY = totalDy;
        data.timestamp = t;
        fireGesture(g_touch.targetWidget, GestureType::DragUpdate, data);
        
        // تحديث التمرير إذا كان الودجت قابلاً للتمرير
        auto it = g_scrollStates.find(g_touch.targetWidget);
        if (it != g_scrollStates.end() && it->second.isDragging) {
            float newOffset = it->second.offset - dy;
            float maxOffset = it->second.contentSize - it->second.viewportSize;
            
            // تطبيق فيزياء التجاوز
            if (it->second.physics.type == ScrollPhysicsType::Bouncing) {
                if (newOffset < 0) {
                    newOffset = newOffset * it->second.physics.bounceFactor;
                } else if (newOffset > maxOffset && maxOffset > 0) {
                    float over = newOffset - maxOffset;
                    newOffset = maxOffset + over * it->second.physics.bounceFactor;
                }
            } else if (it->second.physics.type == ScrollPhysicsType::Clamped) {
                newOffset = std::max(0.0f, std::min(newOffset, maxOffset));
            }
            
            it->second.offset = newOffset;
            
            // تحديث scrollY على الودجت
            sad::ui::Widget* w = sad::ui::getWidget(g_touch.targetWidget);
            if (w) w->scrollY = it->second.offset;
        }
    }
    
    g_touch.lastX = x;
    g_touch.lastY = y;
    g_touch.lastMoveTime = t;
}

void processMouseWheel(float x, float y, float scrollAmount) {
    int target = findWidgetAt(x, y);
    if (target <= 0) return;
    
    double t = now_ms();
    GestureData data;
    data.widgetId = target;
    data.x = x;
    data.y = y;
    data.scrollDelta = scrollAmount;
    data.timestamp = t;
    
    data.type = GestureType::ScrollUpdate;
    fireGesture(target, GestureType::ScrollUpdate, data);
    
    // تحديث التمرير
    auto it = g_scrollStates.find(target);
    if (it != g_scrollStates.end()) {
        float newOffset = it->second.offset - scrollAmount * 40.0f;
        float maxOffset = it->second.contentSize - it->second.viewportSize;
        
        if (it->second.physics.type == ScrollPhysicsType::Clamped ||
            it->second.physics.type == ScrollPhysicsType::Never) {
            newOffset = std::max(0.0f, std::min(newOffset, std::max(0.0f, maxOffset)));
        }
        
        it->second.offset = newOffset;
        sad::ui::Widget* w = sad::ui::getWidget(target);
        if (w) w->scrollY = it->second.offset;
    }
}

// ═══════════════════════════════════════════════════════════════════
// تحديث الإيماءات — يُستدعى كل إطار
// ═══════════════════════════════════════════════════════════════════

void updateGestures(double deltaMs) {
    double t = now_ms();
    
    // ─── كشف الضغط الطويل ───
    if (g_touch.isDown && !g_touch.isDragging && !g_touch.longPressTriggered) {
        double holdDuration = t - g_touch.downTime;
        if (holdDuration >= LONG_PRESS_MS) {
            g_touch.longPressTriggered = true;
            
            GestureData data;
            data.type = GestureType::LongPress;
            data.widgetId = g_touch.targetWidget;
            data.x = g_touch.currentX;
            data.y = g_touch.currentY;
            data.startX = g_touch.downX;
            data.startY = g_touch.downY;
            data.timestamp = t;
            fireGesture(g_touch.targetWidget, GestureType::LongPress, data);
        }
    }
    
    // ─── فيزياء التمرير بالزخم ───
    for (auto& [widgetId, ss] : g_scrollStates) {
        if (!ss.isAnimating) continue;
        
        float dt = static_cast<float>(deltaMs / 1000.0);
        if (dt <= 0) continue;
        
        // تطبيق الاحتكاك
        float friction = 1.0f - ss.physics.friction * dt * 60.0f;
        if (friction < 0) friction = 0;
        ss.velocity *= friction;
        
        // تحريك
        ss.offset += ss.velocity * dt;
        
        float maxOffset = ss.contentSize - ss.viewportSize;
        
        // فيزياء الارتداد (iOS-like)
        if (ss.physics.type == ScrollPhysicsType::Bouncing) {
            if (ss.offset < 0) {
                ss.overscroll = ss.offset;
                // قوة إرجاع مطاطية
                float springForce = -ss.offset * 10.0f;
                ss.velocity += springForce * dt;
            } else if (ss.offset > maxOffset && maxOffset > 0) {
                ss.overscroll = ss.offset - maxOffset;
                float springForce = -(ss.offset - maxOffset) * 10.0f;
                ss.velocity += springForce * dt;
            } else {
                ss.overscroll = 0;
            }
        } else if (ss.physics.type == ScrollPhysicsType::Clamped) {
            ss.offset = std::max(0.0f, std::min(ss.offset, std::max(0.0f, maxOffset)));
            if (ss.offset <= 0 || ss.offset >= maxOffset) {
                ss.velocity = 0;
            }
        } else if (ss.physics.type == ScrollPhysicsType::Page) {
            // تمرير الصفحات — ننجذب لأقرب صفحة
            if (std::abs(ss.velocity) < ss.physics.minVelocity * 2) {
                float pageSize = ss.viewportSize;
                if (pageSize > 0) {
                    float targetPage = std::round(ss.offset / pageSize);
                    float targetOffset = targetPage * pageSize;
                    float springForce = (targetOffset - ss.offset) * 15.0f;
                    ss.velocity += springForce * dt;
                }
            }
        }
        
        // إيقاف عند سرعة منخفضة جداً (مع عدم التجاوز)
        if (std::abs(ss.velocity) < ss.physics.minVelocity && std::abs(ss.overscroll) < 1.0f) {
            ss.velocity = 0;
            ss.isAnimating = false;
            
            // ضبط على الحدود
            if (ss.offset < 0) ss.offset = 0;
            if (maxOffset > 0 && ss.offset > maxOffset) ss.offset = maxOffset;
        }
        
        // تحديث الودجت
        sad::ui::Widget* w = sad::ui::getWidget(widgetId);
        if (w) w->scrollY = ss.offset;
    }
}

// ═══════════════════════════════════════════════════════════════════
// فيزياء التمرير
// ═══════════════════════════════════════════════════════════════════

void setScrollPhysics(int widgetId, ScrollPhysicsType type) {
    auto& ss = g_scrollStates[widgetId];
    ss.widgetId = widgetId;
    ss.physics.type = type;
}

void setScrollPhysicsCustom(int widgetId, const ScrollPhysics& physics) {
    auto& ss = g_scrollStates[widgetId];
    ss.widgetId = widgetId;
    ss.physics = physics;
}

void startMomentumScroll(int widgetId, float velocity) {
    auto& ss = g_scrollStates[widgetId];
    ss.widgetId = widgetId;
    
    // تقييد السرعة القصوى
    if (velocity > ss.physics.maxVelocity) velocity = ss.physics.maxVelocity;
    if (velocity < -ss.physics.maxVelocity) velocity = -ss.physics.maxVelocity;
    
    ss.velocity = velocity;
    ss.isAnimating = true;
    ss.isDragging = false;
}

void stopScroll(int widgetId) {
    auto it = g_scrollStates.find(widgetId);
    if (it != g_scrollStates.end()) {
        it->second.velocity = 0;
        it->second.isAnimating = false;
    }
}

ScrollState getScrollState(int widgetId) {
    auto it = g_scrollStates.find(widgetId);
    if (it != g_scrollStates.end()) return it->second;
    ScrollState ss;
    ss.widgetId = widgetId;
    return ss;
}

void setScrollContentSize(int widgetId, float size) {
    auto& ss = g_scrollStates[widgetId];
    ss.widgetId = widgetId;
    ss.contentSize = size;
}

// ═══════════════════════════════════════════════════════════════════
// أدوات مساعدة
// ═══════════════════════════════════════════════════════════════════

bool isPointInWidget(int widgetId, float x, float y) {
    sad::ui::Widget* w = sad::ui::getWidget(widgetId);
    if (!w) return false;
    return (x >= w->cx && x <= w->cx + w->cw &&
            y >= w->cy && y <= w->cy + w->ch);
}

int findWidgetAt(float x, float y) {
    // نحتاج جذر التطبيق — نبحث في جميع الودجات
    // نبدأ من الودجت الأعلى ترتيباً (الأخير)
    // هذا تبسيط — في التطبيق الحقيقي نبدأ من جذر التطبيق
    for (int i = 5000; i >= 1; i--) {
        sad::ui::Widget* w = sad::ui::getWidget(i);
        if (!w || !w->visible) continue;
        if (w->children.empty() &&
            x >= w->cx && x <= w->cx + w->cw &&
            y >= w->cy && y <= w->cy + w->ch) {
            return i;
        }
    }
    return 0;
}

void screenToLocal(int widgetId, float screenX, float screenY,
                   float& localX, float& localY) {
    sad::ui::Widget* w = sad::ui::getWidget(widgetId);
    if (!w) { localX = screenX; localY = screenY; return; }
    localX = screenX - w->cx;
    localY = screenY - w->cy;
}

}} // namespace sad::gesture
