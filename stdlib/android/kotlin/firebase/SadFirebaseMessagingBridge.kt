// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadFirebaseMessagingBridge.kt
// الوصف: جسر Kotlin لإشعارات Firebase Cloud Messaging
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import com.google.firebase.messaging.FirebaseMessaging
import com.google.firebase.messaging.RemoteMessage

/**
 * جسر Firebase Messaging — SadFirebaseMessagingBridge
 */
object SadFirebaseMessagingBridge {
    
    private var context: Context? = null
    private var onTokenRefreshCallback: ((String) -> Unit)? = null
    private var onMessageReceivedCallback: ((Map<String, Any?>) -> Unit)? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Token Management
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getToken(
        onSuccess: (String) -> Unit,
        onError: (String) -> Unit
    ) {
        FirebaseMessaging.getInstance().token
            .addOnSuccessListener { token ->
                onSuccess(token)
            }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في جلب الرمز")
            }
    }
    
    @JvmStatic
    fun deleteToken(
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        FirebaseMessaging.getInstance().deleteToken()
            .addOnSuccessListener { onSuccess() }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في حذف الرمز")
            }
    }
    
    @JvmStatic
    fun setOnTokenRefreshListener(callback: (String) -> Unit) {
        onTokenRefreshCallback = callback
    }
    
    // يستدعى من FirebaseMessagingService
    @JvmStatic
    fun onNewToken(token: String) {
        onTokenRefreshCallback?.invoke(token)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Topic Subscriptions
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun subscribeToTopic(
        topic: String,
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        FirebaseMessaging.getInstance().subscribeToTopic(topic)
            .addOnSuccessListener { onSuccess() }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في الاشتراك بالموضوع")
            }
    }
    
    @JvmStatic
    fun unsubscribeFromTopic(
        topic: String,
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        FirebaseMessaging.getInstance().unsubscribeFromTopic(topic)
            .addOnSuccessListener { onSuccess() }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في إلغاء الاشتراك")
            }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Auto Initialization
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setAutoInitEnabled(enabled: Boolean) {
        FirebaseMessaging.getInstance().isAutoInitEnabled = enabled
    }
    
    @JvmStatic
    fun isAutoInitEnabled(): Boolean {
        return FirebaseMessaging.getInstance().isAutoInitEnabled
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Message Handling
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setOnMessageReceivedListener(callback: (Map<String, Any?>) -> Unit) {
        onMessageReceivedCallback = callback
    }
    
    // يستدعى من FirebaseMessagingService
    @JvmStatic
    fun onMessageReceived(remoteMessage: RemoteMessage) {
        val messageData = mutableMapOf<String, Any?>()
        
        // بيانات الرسالة
        messageData["data"] = remoteMessage.data
        messageData["from"] = remoteMessage.from
        messageData["messageId"] = remoteMessage.messageId
        messageData["messageType"] = remoteMessage.messageType
        messageData["sentTime"] = remoteMessage.sentTime
        messageData["collapseKey"] = remoteMessage.collapseKey
        messageData["priority"] = remoteMessage.priority
        messageData["originalPriority"] = remoteMessage.originalPriority
        messageData["ttl"] = remoteMessage.ttl
        
        // بيانات الإشعار
        remoteMessage.notification?.let { notification ->
            messageData["notification"] = mapOf(
                "title" to notification.title,
                "body" to notification.body,
                "icon" to notification.icon,
                "imageUrl" to notification.imageUrl?.toString(),
                "sound" to notification.sound,
                "tag" to notification.tag,
                "color" to notification.color,
                "clickAction" to notification.clickAction,
                "channelId" to notification.channelId,
                "link" to notification.link?.toString(),
                "ticker" to notification.ticker
            )
        }
        
        onMessageReceivedCallback?.invoke(messageData)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Delivery Metrics
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setDeliveryMetricsExportToBigQueryEnabled(enabled: Boolean) {
        FirebaseMessaging.getInstance().deliveryMetricsExportToBigQueryEnabled = enabled
    }
    
    @JvmStatic
    fun isDeliveryMetricsExportToBigQueryEnabled(): Boolean {
        return FirebaseMessaging.getInstance().deliveryMetricsExportToBigQueryEnabled
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Helper Functions
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun parseNotificationData(data: Map<String, String>): Map<String, Any?> {
        return mapOf(
            "title" to data["title"],
            "body" to data["body"],
            "imageUrl" to data["image"],
            "clickAction" to data["click_action"],
            "customData" to data.filterKeys { !it.startsWith("google.") }
        )
    }
    
    @JvmStatic
    fun isNotificationMessage(data: Map<String, String>): Boolean {
        return data.containsKey("title") || data.containsKey("body")
    }
    
    @JvmStatic
    fun isDataMessage(data: Map<String, String>): Boolean {
        return data.isNotEmpty() && !isNotificationMessage(data)
    }
}
