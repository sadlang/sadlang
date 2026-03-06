package sad.android.communication.event

import android.os.Handler
import android.os.Looper
import java.util.concurrent.ConcurrentHashMap
import java.util.concurrent.CopyOnWriteArrayList

/**
 * جسر الأحداث - نظام نشر/اشتراك للأحداث
 * Event Bridge - Publish/Subscribe event system
 * 
 * @author فريق لغة ص
 */
object SadEventBusBridge {
    
    private val subscribers = ConcurrentHashMap<String, CopyOnWriteArrayList<EventSubscriber>>()
    private val stickyEvents = ConcurrentHashMap<String, Any>()
    private val mainHandler = Handler(Looper.getMainLooper())
    
    /**
     * مشترك في الحدث / Event subscriber
     */
    data class EventSubscriber(
        val id: String,
        val callback: (Any) -> Unit,
        val runOnMainThread: Boolean = true,
        val priority: Int = 0,
        val once: Boolean = false
    )
    
    /**
     * حدث مخصص / Custom event
     */
    data class Event(
        val name: String,
        val data: Any? = null,
        val timestamp: Long = System.currentTimeMillis(),
        val source: String? = null
    )
    
    // ═══════════════════════════════════════════════════════════════
    //                         الاشتراك
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الاشتراك في حدث / Subscribe to event
     */
    fun subscribe(
        eventName: String,
        callback: (Any) -> Unit,
        runOnMainThread: Boolean = true,
        priority: Int = 0
    ): String {
        val subscriberId = "${eventName}_${System.currentTimeMillis()}_${(Math.random() * 10000).toInt()}"
        
        val subscriber = EventSubscriber(
            id = subscriberId,
            callback = callback,
            runOnMainThread = runOnMainThread,
            priority = priority
        )
        
        subscribers.getOrPut(eventName) { CopyOnWriteArrayList() }.apply {
            add(subscriber)
            sortByDescending { it.priority }
        }
        
        // إرسال حدث sticky إذا وجد
        stickyEvents[eventName]?.let { stickyData ->
            deliverEvent(subscriber, stickyData)
        }
        
        return subscriberId
    }
    
    /**
     * الاشتراك مرة واحدة / Subscribe once
     */
    fun subscribeOnce(
        eventName: String,
        callback: (Any) -> Unit,
        runOnMainThread: Boolean = true
    ): String {
        val subscriberId = "${eventName}_once_${System.currentTimeMillis()}"
        
        val subscriber = EventSubscriber(
            id = subscriberId,
            callback = callback,
            runOnMainThread = runOnMainThread,
            once = true
        )
        
        subscribers.getOrPut(eventName) { CopyOnWriteArrayList() }.add(subscriber)
        
        return subscriberId
    }
    
    /**
     * الاشتراك في أحداث متعددة / Subscribe to multiple events
     */
    fun subscribeMultiple(
        eventNames: List<String>,
        callback: (Event) -> Unit,
        runOnMainThread: Boolean = true
    ): List<String> {
        return eventNames.map { eventName ->
            subscribe(eventName, { data ->
                callback(Event(eventName, data))
            }, runOnMainThread)
        }
    }
    
    /**
     * الاشتراك بنمط regex / Subscribe with pattern
     */
    private val patternSubscribers = ConcurrentHashMap<String, List<Pair<Regex, EventSubscriber>>>()
    
    fun subscribePattern(
        pattern: String,
        callback: (Event) -> Unit,
        runOnMainThread: Boolean = true
    ): String {
        val subscriberId = "pattern_${System.currentTimeMillis()}"
        val regex = Regex(pattern)
        
        val subscriber = EventSubscriber(
            id = subscriberId,
            callback = { data ->
                if (data is Event) callback(data)
            },
            runOnMainThread = runOnMainThread
        )
        
        val existing = patternSubscribers[subscriberId] ?: emptyList()
        patternSubscribers[subscriberId] = existing + (regex to subscriber)
        
        return subscriberId
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إلغاء الاشتراك
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إلغاء اشتراك / Unsubscribe
     */
    fun unsubscribe(subscriberId: String): Boolean {
        subscribers.values.forEach { list ->
            val removed = list.removeIf { it.id == subscriberId }
            if (removed) return true
        }
        
        patternSubscribers.remove(subscriberId)
        return false
    }
    
    /**
     * إلغاء جميع الاشتراكات لحدث / Unsubscribe all from event
     */
    fun unsubscribeAll(eventName: String) {
        subscribers.remove(eventName)
    }
    
    /**
     * إلغاء جميع الاشتراكات / Unsubscribe all
     */
    fun unsubscribeAll() {
        subscribers.clear()
        patternSubscribers.clear()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         النشر
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * نشر حدث / Publish event
     */
    fun publish(eventName: String, data: Any? = null) {
        val eventData = data ?: Unit
        
        // إرسال للمشتركين العاديين
        subscribers[eventName]?.let { list ->
            val toRemove = mutableListOf<EventSubscriber>()
            
            list.forEach { subscriber ->
                deliverEvent(subscriber, eventData)
                if (subscriber.once) {
                    toRemove.add(subscriber)
                }
            }
            
            toRemove.forEach { list.remove(it) }
        }
        
        // إرسال للمشتركين بالنمط
        patternSubscribers.values.flatten().forEach { (regex, subscriber) ->
            if (regex.matches(eventName)) {
                deliverEvent(subscriber, Event(eventName, eventData))
            }
        }
    }
    
    /**
     * نشر حدث sticky / Publish sticky event
     */
    fun publishSticky(eventName: String, data: Any) {
        stickyEvents[eventName] = data
        publish(eventName, data)
    }
    
    /**
     * نشر حدث مؤجل / Publish delayed event
     */
    fun publishDelayed(eventName: String, data: Any? = null, delayMs: Long) {
        mainHandler.postDelayed({
            publish(eventName, data)
        }, delayMs)
    }
    
    /**
     * نشر حدث مع كائن Event / Publish Event object
     */
    fun publishEvent(event: Event) {
        publish(event.name, event)
    }
    
    /**
     * نشر أحداث متعددة / Publish multiple events
     */
    fun publishMultiple(events: List<Pair<String, Any?>>) {
        events.forEach { (name, data) ->
            publish(name, data)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الأحداث اللزجة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على حدث sticky / Get sticky event
     */
    fun getStickyEvent(eventName: String): Any? {
        return stickyEvents[eventName]
    }
    
    /**
     * إزالة حدث sticky / Remove sticky event
     */
    fun removeStickyEvent(eventName: String): Any? {
        return stickyEvents.remove(eventName)
    }
    
    /**
     * إزالة جميع الأحداث اللزجة / Remove all sticky events
     */
    fun removeAllStickyEvents() {
        stickyEvents.clear()
    }
    
    /**
     * هل يوجد حدث sticky؟ / Has sticky event?
     */
    fun hasStickyEvent(eventName: String): Boolean {
        return stickyEvents.containsKey(eventName)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         المعلومات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * عدد المشتركين في حدث / Subscriber count for event
     */
    fun subscriberCount(eventName: String): Int {
        return subscribers[eventName]?.size ?: 0
    }
    
    /**
     * هل يوجد مشتركون؟ / Has subscribers?
     */
    fun hasSubscribers(eventName: String): Boolean {
        return subscriberCount(eventName) > 0
    }
    
    /**
     * جميع الأحداث المسجلة / All registered events
     */
    fun registeredEvents(): Set<String> {
        return subscribers.keys.toSet()
    }
    
    /**
     * إجمالي المشتركين / Total subscribers count
     */
    fun totalSubscribers(): Int {
        return subscribers.values.sumOf { it.size }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         أحداث شائعة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * أسماء أحداث شائعة / Common event names
     */
    object Events {
        // أحداث دورة الحياة
        const val APP_START = "app_start"
        const val APP_STOP = "app_stop"
        const val APP_PAUSE = "app_pause"
        const val APP_RESUME = "app_resume"
        
        // أحداث الشبكة
        const val NETWORK_CONNECTED = "network_connected"
        const val NETWORK_DISCONNECTED = "network_disconnected"
        const val NETWORK_CHANGED = "network_changed"
        
        // أحداث المستخدم
        const val USER_LOGIN = "user_login"
        const val USER_LOGOUT = "user_logout"
        const val USER_UPDATED = "user_updated"
        
        // أحداث البيانات
        const val DATA_LOADED = "data_loaded"
        const val DATA_SAVED = "data_saved"
        const val DATA_DELETED = "data_deleted"
        const val DATA_SYNCED = "data_synced"
        
        // أحداث الإشعارات
        const val NOTIFICATION_RECEIVED = "notification_received"
        const val NOTIFICATION_CLICKED = "notification_clicked"
        
        // أحداث مخصصة
        const val CUSTOM = "custom"
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    private fun deliverEvent(subscriber: EventSubscriber, data: Any) {
        if (subscriber.runOnMainThread) {
            mainHandler.post {
                try {
                    subscriber.callback(data)
                } catch (e: Exception) {
                    // تجاهل الأخطاء في callbacks
                }
            }
        } else {
            try {
                subscriber.callback(data)
            } catch (e: Exception) {
                // تجاهل الأخطاء في callbacks
            }
        }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        unsubscribeAll()
        stickyEvents.clear()
    }
}
