// ==============================================================================
// event_bus.h - نظام ناقل الأحداث / Event Bus System
// ==============================================================================
// الوصف: نظام اتصال مركزي يسمح للمكونات بالتواصل دون اقتران مباشر
// Description: Central communication system allowing components to communicate
//              without direct coupling
// ==============================================================================
// المميزات / Features:
//   - نشر واشتراك / Publish-Subscribe pattern
//   - أحداث مكتوبة بقوة / Strongly typed events
//   - أولويات المستمعين / Listener priorities
//   - تصفية الأحداث / Event filtering
//   - أحداث مؤجلة / Deferred events
//   - تتبع الأحداث للتنقيح / Event tracing for debugging
// ==============================================================================
// الاستخدام / Usage:
//   // تعريف حدث مخصص / Define custom event
//   struct PlayerDamageEvent : public Event {
//       Float32 damage;
//       UInt32 attackerId;
//   };
//
//   // الاشتراك / Subscribe
//   EventBus::GetInstance().Subscribe<PlayerDamageEvent>([](const PlayerDamageEvent& e) {
//       std::cout << "تلقي ضرر: " << e.damage << std::endl;
//   });
//
//   // النشر / Publish
//   EventBus::GetInstance().Publish(PlayerDamageEvent{10.0f, 5});
// ==============================================================================

#ifndef SAD_GRAPHICS_EVENT_BUS_H
#define SAD_GRAPHICS_EVENT_BUS_H

#include "types.h"
#include <functional>
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include <typeindex>
#include <queue>
#include <algorithm>
#include <mutex>

namespace sad {
namespace graphics {

// ==============================================================================
// الثوابت / Constants
// ==============================================================================

/// أقصى عدد أحداث مؤجلة / Maximum deferred events
constexpr size_t MAX_DEFERRED_EVENTS = 1024;

/// أولوية افتراضية / Default priority
constexpr Int32 DEFAULT_EVENT_PRIORITY = 0;

// ==============================================================================
// الأحداث الأساسية / Base Events
// ==============================================================================

/// الحدث الأساسي - الفئة الأم لجميع الأحداث
/// Base Event - Parent class for all events
/// كل حدث يرث من هذه الفئة ويضيف البيانات الخاصة به
struct Event {
    /// هل تم استهلاك الحدث؟ / Was the event consumed?
    /// عندما يكون true، لن يتم تمرير الحدث للمستمعين التاليين
    mutable bool consumed = false;
    
    /// الطابع الزمني / Timestamp
    Float64 timestamp = 0.0;
    
    /// معرف المصدر / Source identifier
    UInt32 sourceId = 0;
    
    /// المدمر الافتراضي / Virtual destructor
    virtual ~Event() = default;
    
    /// الحصول على اسم نوع الحدث / Get event type name
    virtual const char* GetTypeName() const { return "Event"; }
    
    /// استهلاك الحدث - يمنع الانتشار للمستمعين الآخرين
    /// Consume event - prevents propagation to other listeners
    void Consume() const { consumed = true; }
    
    /// هل تم استهلاك الحدث؟ / Is event consumed?
    bool IsConsumed() const { return consumed; }
};

// ==============================================================================
// ماكرو تعريف الحدث / Event Definition Macro
// ==============================================================================

/// ماكرو لتسهيل تعريف الأحداث المخصصة
/// Macro to simplify custom event definition
#define DEFINE_EVENT(EventName) \
    struct EventName : public sad::graphics::Event { \
        static constexpr const char* StaticTypeName = #EventName; \
        const char* GetTypeName() const override { return StaticTypeName; } \
    }

/// ماكرو لتعريف حدث مع بيانات
/// Macro to define event with data
#define DEFINE_EVENT_WITH_DATA(EventName, ...) \
    struct EventName : public sad::graphics::Event { \
        static constexpr const char* StaticTypeName = #EventName; \
        const char* GetTypeName() const override { return StaticTypeName; } \
        __VA_ARGS__ \
    }

// ==============================================================================
// واصف المستمع / Listener Descriptor
// ==============================================================================

/// معرف المستمع / Listener identifier
using ListenerId = UInt32;

/// دالة المستمع / Listener function type
template<typename T>
using EventCallback = std::function<void(const T&)>;

/// معلومات المستمع / Listener info
struct ListenerInfo {
    ListenerId id;              // معرف فريد / Unique ID
    Int32 priority;             // الأولوية (أعلى = يُستدعى أولاً) / Priority (higher = called first)
    std::string tag;            // علامة للتصفية / Tag for filtering
    bool enabled;               // مفعّل / Enabled
    bool once;                  // استدعاء مرة واحدة ثم حذف / Call once then remove
};

// ==============================================================================
// غلاف المستمع / Listener Wrapper
// ==============================================================================

/// غلاف داخلي للمستمع / Internal listener wrapper
class IListenerWrapper {
public:
    virtual ~IListenerWrapper() = default;
    virtual void Invoke(const Event& event) = 0;
    
    ListenerInfo info;
};

/// غلاف مستمع مكتوب / Typed listener wrapper
template<typename T>
class ListenerWrapper : public IListenerWrapper {
public:
    ListenerWrapper(EventCallback<T> callback) : m_callback(std::move(callback)) {}
    
    void Invoke(const Event& event) override {
        if (info.enabled) {
            m_callback(static_cast<const T&>(event));
        }
    }
    
private:
    EventCallback<T> m_callback;
};

// ==============================================================================
// الأحداث المدمجة / Built-in Events
// ==============================================================================

/// حدث تحديث الإطار / Frame update event
struct FrameUpdateEvent : public Event {
    static constexpr const char* StaticTypeName = "FrameUpdateEvent";
    const char* GetTypeName() const override { return StaticTypeName; }
    
    Float32 deltaTime;      // الوقت منذ الإطار السابق / Time since last frame
    UInt64 frameNumber;     // رقم الإطار / Frame number
};

/// حدث بدء اللعبة / Game start event
struct GameStartEvent : public Event {
    static constexpr const char* StaticTypeName = "GameStartEvent";
    const char* GetTypeName() const override { return StaticTypeName; }
};

/// حدث إيقاف اللعبة / Game pause event
struct GamePauseEvent : public Event {
    static constexpr const char* StaticTypeName = "GamePauseEvent";
    const char* GetTypeName() const override { return StaticTypeName; }
    
    bool paused;    // هل تم الإيقاف / Is paused
};

/// حدث استئناف اللعبة / Game resume event
struct GameResumeEvent : public Event {
    static constexpr const char* StaticTypeName = "GameResumeEvent";
    const char* GetTypeName() const override { return StaticTypeName; }
};

/// حدث إنهاء اللعبة / Game end event
struct GameEndEvent : public Event {
    static constexpr const char* StaticTypeName = "GameEndEvent";
    const char* GetTypeName() const override { return StaticTypeName; }
};

/// حدث تغيير المشهد / Scene change event
struct SceneChangeEvent : public Event {
    static constexpr const char* StaticTypeName = "SceneChangeEvent";
    const char* GetTypeName() const override { return StaticTypeName; }
    
    std::string previousScene;  // المشهد السابق / Previous scene
    std::string nextScene;      // المشهد التالي / Next scene
};

/// حدث تغيير حجم النافذة / Window resize event
struct WindowResizeEvent : public Event {
    static constexpr const char* StaticTypeName = "WindowResizeEvent";
    const char* GetTypeName() const override { return StaticTypeName; }
    
    Int32 width;    // العرض الجديد / New width
    Int32 height;   // الارتفاع الجديد / New height
    Int32 oldWidth; // العرض القديم / Old width
    Int32 oldHeight;// الارتفاع القديم / Old height
};

/// حدث ضغط مفتاح / Key press event
struct KeyPressEvent : public Event {
    static constexpr const char* StaticTypeName = "KeyPressEvent";
    const char* GetTypeName() const override { return StaticTypeName; }
    
    Int32 keyCode;      // رمز المفتاح / Key code
    bool shift;         // مفتاح Shift مضغوط / Shift pressed
    bool ctrl;          // مفتاح Ctrl مضغوط / Ctrl pressed
    bool alt;           // مفتاح Alt مضغوط / Alt pressed
    bool isRepeat;      // هل هو تكرار / Is repeat
};

/// حدث إفلات مفتاح / Key release event
struct KeyReleaseEvent : public Event {
    static constexpr const char* StaticTypeName = "KeyReleaseEvent";
    const char* GetTypeName() const override { return StaticTypeName; }
    
    Int32 keyCode;  // رمز المفتاح / Key code
};

/// حدث نقر الفأرة / Mouse click event
struct MouseClickEvent : public Event {
    static constexpr const char* StaticTypeName = "MouseClickEvent";
    const char* GetTypeName() const override { return StaticTypeName; }
    
    Float32 x;          // الموقع الأفقي / X position
    Float32 y;          // الموقع العمودي / Y position
    Int32 button;       // زر الفأرة (0=يسار، 1=وسط، 2=يمين) / Mouse button
    Int32 clicks;       // عدد النقرات / Click count
};

/// حدث حركة الفأرة / Mouse move event
struct MouseMoveEvent : public Event {
    static constexpr const char* StaticTypeName = "MouseMoveEvent";
    const char* GetTypeName() const override { return StaticTypeName; }
    
    Float32 x;          // الموقع الأفقي / X position
    Float32 y;          // الموقع العمودي / Y position
    Float32 deltaX;     // الفرق الأفقي / X delta
    Float32 deltaY;     // الفرق العمودي / Y delta
};

/// حدث عجلة الفأرة / Mouse wheel event
struct MouseWheelEvent : public Event {
    static constexpr const char* StaticTypeName = "MouseWheelEvent";
    const char* GetTypeName() const override { return StaticTypeName; }
    
    Float32 scrollX;    // التمرير الأفقي / Horizontal scroll
    Float32 scrollY;    // التمرير العمودي / Vertical scroll
};

/// حدث تصادم / Collision event
struct CollisionEvent : public Event {
    static constexpr const char* StaticTypeName = "CollisionEvent";
    const char* GetTypeName() const override { return StaticTypeName; }
    
    UInt32 entityA;     // الكيان الأول / First entity
    UInt32 entityB;     // الكيان الثاني / Second entity
    Vec2 contactPoint;  // نقطة التلامس / Contact point
    Vec2 normal;        // المتجه العمودي / Normal vector
    Float32 penetration;// العمق / Penetration depth
};

/// حدث تحميل مورد / Resource load event
struct ResourceLoadEvent : public Event {
    static constexpr const char* StaticTypeName = "ResourceLoadEvent";
    const char* GetTypeName() const override { return StaticTypeName; }
    
    std::string resourcePath;   // مسار المورد / Resource path
    std::string resourceType;   // نوع المورد / Resource type
    bool success;               // هل نجح التحميل / Load success
    std::string errorMessage;   // رسالة الخطأ / Error message
};

// ==============================================================================
// ناقل الأحداث / Event Bus
// ==============================================================================

/// ناقل الأحداث المركزي
/// Central Event Bus
/// 
/// نمط المراقب (Observer Pattern) مع دعم للأولويات والتصفية
/// يسمح للمكونات بالتواصل دون معرفة بعضها البعض مباشرة
/// 
/// الاستخدام:
///   1. الاشتراك في حدث باستخدام Subscribe
///   2. نشر حدث باستخدام Publish
///   3. إلغاء الاشتراك باستخدام Unsubscribe
class EventBus {
public:
    // ==========================================================================
    // Singleton
    // ==========================================================================
    
    /// الحصول على المثيل الوحيد / Get singleton instance
    static EventBus& GetInstance() {
        static EventBus instance;
        return instance;
    }
    
    // ==========================================================================
    // الاشتراك / Subscription
    // ==========================================================================
    
    /// الاشتراك في حدث
    /// Subscribe to an event
    /// @tparam T نوع الحدث / Event type
    /// @param callback دالة الاستدعاء / Callback function
    /// @param priority الأولوية (أعلى = يُستدعى أولاً) / Priority (higher = called first)
    /// @param tag علامة اختيارية للتصفية / Optional tag for filtering
    /// @return معرف المستمع لإلغاء الاشتراك لاحقاً / Listener ID for later unsubscription
    template<typename T>
    ListenerId Subscribe(EventCallback<T> callback, 
                        Int32 priority = DEFAULT_EVENT_PRIORITY,
                        const std::string& tag = "") {
        static_assert(std::is_base_of<Event, T>::value, 
                      "T يجب أن يرث من Event / T must derive from Event");
        
        auto wrapper = std::make_unique<ListenerWrapper<T>>(std::move(callback));
        wrapper->info.id = ++m_nextListenerId;
        wrapper->info.priority = priority;
        wrapper->info.tag = tag;
        wrapper->info.enabled = true;
        wrapper->info.once = false;
        
        ListenerId id = wrapper->info.id;
        
        std::lock_guard<std::mutex> lock(m_mutex);
        auto& listeners = m_listeners[std::type_index(typeid(T))];
        listeners.push_back(std::move(wrapper));
        
        // ترتيب حسب الأولوية / Sort by priority
        SortListeners(listeners);
        
        return id;
    }
    
    /// الاشتراك لمرة واحدة
    /// Subscribe once (auto-unsubscribe after first call)
    template<typename T>
    ListenerId SubscribeOnce(EventCallback<T> callback,
                            Int32 priority = DEFAULT_EVENT_PRIORITY,
                            const std::string& tag = "") {
        auto wrapper = std::make_unique<ListenerWrapper<T>>(std::move(callback));
        wrapper->info.id = ++m_nextListenerId;
        wrapper->info.priority = priority;
        wrapper->info.tag = tag;
        wrapper->info.enabled = true;
        wrapper->info.once = true;
        
        ListenerId id = wrapper->info.id;
        
        std::lock_guard<std::mutex> lock(m_mutex);
        auto& listeners = m_listeners[std::type_index(typeid(T))];
        listeners.push_back(std::move(wrapper));
        SortListeners(listeners);
        
        return id;
    }
    
    // ==========================================================================
    // إلغاء الاشتراك / Unsubscription
    // ==========================================================================
    
    /// إلغاء الاشتراك بالمعرف / Unsubscribe by ID
    void Unsubscribe(ListenerId id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        for (auto& pair : m_listeners) {
            auto& listeners = pair.second;
            listeners.erase(
                std::remove_if(listeners.begin(), listeners.end(),
                    [id](const auto& wrapper) {
                        return wrapper->info.id == id;
                    }),
                listeners.end()
            );
        }
    }
    
    /// إلغاء جميع اشتراكات نوع حدث
    /// Unsubscribe all listeners for event type
    template<typename T>
    void UnsubscribeAll() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_listeners.erase(std::type_index(typeid(T)));
    }
    
    /// إلغاء الاشتراكات بالعلامة / Unsubscribe by tag
    void UnsubscribeByTag(const std::string& tag) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        for (auto& pair : m_listeners) {
            auto& listeners = pair.second;
            listeners.erase(
                std::remove_if(listeners.begin(), listeners.end(),
                    [&tag](const auto& wrapper) {
                        return wrapper->info.tag == tag;
                    }),
                listeners.end()
            );
        }
    }
    
    /// مسح جميع المستمعين / Clear all listeners
    void ClearAll() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_listeners.clear();
    }
    
    // ==========================================================================
    // النشر / Publishing
    // ==========================================================================
    
    /// نشر حدث فوراً / Publish event immediately
    template<typename T>
    void Publish(const T& event) {
        static_assert(std::is_base_of<Event, T>::value,
                      "T يجب أن يرث من Event / T must derive from Event");
        
        // تسجيل الحدث للتنقيح / Log event for debugging
        if (m_tracing) {
            TraceEvent(event);
        }
        
        std::vector<ListenerId> toRemove;
        
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            
            auto it = m_listeners.find(std::type_index(typeid(T)));
            if (it == m_listeners.end()) return;
            
            for (auto& wrapper : it->second) {
                if (event.consumed) break;  // الحدث مستهلك / Event consumed
                
                wrapper->Invoke(event);
                
                if (wrapper->info.once) {
                    toRemove.push_back(wrapper->info.id);
                }
            }
        }
        
        // حذف المستمعين لمرة واحدة / Remove once listeners
        for (auto id : toRemove) {
            Unsubscribe(id);
        }
    }
    
    /// نشر حدث (نسخة rvalue) / Publish event (rvalue version)
    template<typename T>
    void Publish(T&& event) {
        Publish(static_cast<const T&>(event));
    }
    
    /// تأجيل نشر حدث / Defer event publishing
    /// الأحداث المؤجلة تُنشر عند استدعاء ProcessDeferredEvents
    template<typename T>
    void PublishDeferred(const T& event) {
        std::lock_guard<std::mutex> lock(m_deferredMutex);
        
        if (m_deferredEvents.size() < MAX_DEFERRED_EVENTS) {
            m_deferredEvents.push({
                std::type_index(typeid(T)),
                std::make_shared<T>(event)
            });
        }
    }
    
    /// معالجة الأحداث المؤجلة / Process deferred events
    void ProcessDeferredEvents() {
        std::queue<DeferredEvent> toProcess;
        
        {
            std::lock_guard<std::mutex> lock(m_deferredMutex);
            std::swap(toProcess, m_deferredEvents);
        }
        
        while (!toProcess.empty()) {
            auto& deferred = toProcess.front();
            PublishDeferredInternal(deferred.type, deferred.event);
            toProcess.pop();
        }
    }
    
    // ==========================================================================
    // التحكم / Control
    // ==========================================================================
    
    /// تفعيل/تعطيل مستمع / Enable/disable listener
    void SetListenerEnabled(ListenerId id, bool enabled) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        for (auto& pair : m_listeners) {
            for (auto& wrapper : pair.second) {
                if (wrapper->info.id == id) {
                    wrapper->info.enabled = enabled;
                    return;
                }
            }
        }
    }
    
    /// تغيير أولوية مستمع / Change listener priority
    void SetListenerPriority(ListenerId id, Int32 priority) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        for (auto& pair : m_listeners) {
            for (auto& wrapper : pair.second) {
                if (wrapper->info.id == id) {
                    wrapper->info.priority = priority;
                    SortListeners(pair.second);
                    return;
                }
            }
        }
    }
    
    // ==========================================================================
    // التنقيح / Debugging
    // ==========================================================================
    
    /// تفعيل تتبع الأحداث / Enable event tracing
    void EnableTracing(bool enabled) { m_tracing = enabled; }
    
    /// هل التتبع مفعّل / Is tracing enabled
    bool IsTracingEnabled() const { return m_tracing; }
    
    /// الحصول على إحصائيات / Get statistics
    struct Statistics {
        size_t totalListeners;      // إجمالي المستمعين / Total listeners
        size_t eventTypesCount;     // عدد أنواع الأحداث / Event types count
        size_t deferredEventsCount; // الأحداث المؤجلة / Deferred events count
        UInt64 eventsPublished;     // الأحداث المنشورة / Events published
    };
    
    Statistics GetStatistics() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        Statistics stats;
        stats.totalListeners = 0;
        for (const auto& pair : m_listeners) {
            stats.totalListeners += pair.second.size();
        }
        stats.eventTypesCount = m_listeners.size();
        stats.deferredEventsCount = m_deferredEvents.size();
        stats.eventsPublished = m_eventsPublished;
        
        return stats;
    }
    
    /// تعيين دالة تتبع مخصصة / Set custom trace function
    using TraceCallback = std::function<void(const std::string& eventName, const Event& event)>;
    void SetTraceCallback(TraceCallback callback) {
        m_traceCallback = std::move(callback);
    }

private:
    // ==========================================================================
    // الأعضاء الخاصة / Private Members
    // ==========================================================================
    
    EventBus() = default;
    ~EventBus() = default;
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;
    
    /// حدث مؤجل / Deferred event
    struct DeferredEvent {
        std::type_index type;
        std::shared_ptr<Event> event;
    };
    
    /// ترتيب المستمعين حسب الأولوية / Sort listeners by priority
    void SortListeners(std::vector<std::unique_ptr<IListenerWrapper>>& listeners) {
        std::sort(listeners.begin(), listeners.end(),
            [](const auto& a, const auto& b) {
                return a->info.priority > b->info.priority;
            });
    }
    
    /// تتبع حدث / Trace event
    void TraceEvent(const Event& event) {
        ++m_eventsPublished;
        if (m_traceCallback) {
            m_traceCallback(event.GetTypeName(), event);
        }
    }
    
    /// نشر حدث مؤجل داخلياً / Publish deferred event internally
    void PublishDeferredInternal(std::type_index type, std::shared_ptr<Event> event) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        auto it = m_listeners.find(type);
        if (it == m_listeners.end()) return;
        
        for (auto& wrapper : it->second) {
            if (event->consumed) break;
            wrapper->Invoke(*event);
        }
    }
    
    // الخريطة: نوع الحدث -> قائمة المستمعين
    // Map: event type -> listener list
    std::unordered_map<std::type_index, std::vector<std::unique_ptr<IListenerWrapper>>> m_listeners;
    
    // الأحداث المؤجلة / Deferred events
    std::queue<DeferredEvent> m_deferredEvents;
    
    // القفل للمزامنة / Mutex for synchronization
    mutable std::mutex m_mutex;
    mutable std::mutex m_deferredMutex;
    
    // معرف المستمع التالي / Next listener ID
    ListenerId m_nextListenerId = 0;
    
    // إحصائيات / Statistics
    UInt64 m_eventsPublished = 0;
    
    // التتبع / Tracing
    bool m_tracing = false;
    TraceCallback m_traceCallback;
};

// ==============================================================================
// مساعدات / Helpers
// ==============================================================================

/// اختصار للحصول على ناقل الأحداث / Shortcut to get event bus
inline EventBus& Events() {
    return EventBus::GetInstance();
}

} // namespace graphics
} // namespace sad

#endif // SAD_GRAPHICS_EVENT_BUS_H
