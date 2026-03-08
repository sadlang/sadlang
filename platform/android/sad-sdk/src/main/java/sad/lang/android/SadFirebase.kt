/**
 * =============================================================================
 * @file SadFirebase.kt
 * @brief (AR) واجهة Firebase للغة ص
 * @brief (EN) Firebase Interface for Sad Language
 * =============================================================================
 */

package sad.lang.android

import android.app.NotificationChannel
import android.app.NotificationManager
import android.content.Context
import android.os.Build
import android.os.Bundle
import com.google.firebase.analytics.FirebaseAnalytics
import com.google.firebase.crashlytics.FirebaseCrashlytics
import com.google.firebase.messaging.FirebaseMessaging
import com.google.firebase.messaging.FirebaseMessagingService
import com.google.firebase.messaging.RemoteMessage
import com.google.firebase.remoteconfig.FirebaseRemoteConfig
import com.google.firebase.remoteconfig.FirebaseRemoteConfigSettings
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.suspendCancellableCoroutine
import kotlinx.coroutines.withContext
import kotlin.coroutines.resume
import kotlin.coroutines.resumeWithException

/**
 * نظام Firebase للغة ص
 */
class SadFirebase(private val context: Context) {
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Analytics
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تحليلات Firebase
     */
    object Analytics {
        private var firebaseAnalytics: FirebaseAnalytics? = null
        
        /**
         * تهيئة التحليلات
         */
        fun init(context: Context) {
            firebaseAnalytics = FirebaseAnalytics.getInstance(context)
        }
        
        /**
         * تسجيل حدث
         */
        fun logEvent(name: String, params: Map<String, Any> = emptyMap()) {
            val bundle = Bundle().apply {
                params.forEach { (key, value) ->
                    when (value) {
                        is String -> putString(key, value)
                        is Int -> putInt(key, value)
                        is Long -> putLong(key, value)
                        is Double -> putDouble(key, value)
                        is Boolean -> putBoolean(key, value)
                    }
                }
            }
            firebaseAnalytics?.logEvent(name, bundle)
        }
        
        /**
         * تسجيل شاشة
         */
        fun logScreen(screenName: String, screenClass: String? = null) {
            logEvent(FirebaseAnalytics.Event.SCREEN_VIEW, mapOf(
                FirebaseAnalytics.Param.SCREEN_NAME to screenName,
                FirebaseAnalytics.Param.SCREEN_CLASS to (screenClass ?: screenName)
            ))
        }
        
        /**
         * تسجيل تسجيل دخول
         */
        fun logLogin(method: String) {
            logEvent(FirebaseAnalytics.Event.LOGIN, mapOf(
                FirebaseAnalytics.Param.METHOD to method
            ))
        }
        
        /**
         * تسجيل تسجيل حساب
         */
        fun logSignUp(method: String) {
            logEvent(FirebaseAnalytics.Event.SIGN_UP, mapOf(
                FirebaseAnalytics.Param.METHOD to method
            ))
        }
        
        /**
         * تسجيل شراء
         */
        fun logPurchase(value: Double, currency: String, itemId: String? = null) {
            val params = mutableMapOf<String, Any>(
                FirebaseAnalytics.Param.VALUE to value,
                FirebaseAnalytics.Param.CURRENCY to currency
            )
            itemId?.let { params[FirebaseAnalytics.Param.ITEM_ID] = it }
            logEvent(FirebaseAnalytics.Event.PURCHASE, params)
        }
        
        /**
         * تعيين معرّف المستخدم
         */
        fun setUserId(userId: String?) {
            firebaseAnalytics?.setUserId(userId)
        }
        
        /**
         * تعيين خاصية مستخدم
         */
        fun setUserProperty(name: String, value: String?) {
            firebaseAnalytics?.setUserProperty(name, value)
        }
        
        /**
         * تفعيل/تعطيل التحليلات
         */
        fun setEnabled(enabled: Boolean) {
            firebaseAnalytics?.setAnalyticsCollectionEnabled(enabled)
        }
        
        /**
         * إعادة تعيين بيانات التحليلات
         */
        fun resetAnalyticsData() {
            firebaseAnalytics?.resetAnalyticsData()
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Crashlytics
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تقارير الأعطال
     */
    object Crashlytics {
        private var crashlytics: FirebaseCrashlytics? = null
        
        /**
         * تهيئة Crashlytics
         */
        fun init() {
            crashlytics = FirebaseCrashlytics.getInstance()
        }
        
        /**
         * تسجيل استثناء
         */
        fun logException(exception: Throwable) {
            crashlytics?.recordException(exception)
        }
        
        /**
         * تسجيل رسالة
         */
        fun log(message: String) {
            crashlytics?.log(message)
        }
        
        /**
         * تعيين معرّف المستخدم
         */
        fun setUserId(userId: String) {
            crashlytics?.setUserId(userId)
        }
        
        /**
         * تعيين مفتاح مخصص
         */
        fun setCustomKey(key: String, value: String) {
            crashlytics?.setCustomKey(key, value)
        }
        
        fun setCustomKey(key: String, value: Boolean) {
            crashlytics?.setCustomKey(key, value)
        }
        
        fun setCustomKey(key: String, value: Int) {
            crashlytics?.setCustomKey(key, value)
        }
        
        fun setCustomKey(key: String, value: Long) {
            crashlytics?.setCustomKey(key, value)
        }
        
        fun setCustomKey(key: String, value: Float) {
            crashlytics?.setCustomKey(key, value)
        }
        
        fun setCustomKey(key: String, value: Double) {
            crashlytics?.setCustomKey(key, value)
        }
        
        /**
         * تفعيل/تعطيل Crashlytics
         */
        fun setEnabled(enabled: Boolean) {
            crashlytics?.setCrashlyticsCollectionEnabled(enabled)
        }
        
        /**
         * فرض عطل (للاختبار)
         */
        fun forceCrash() {
            throw RuntimeException("اختبار Crashlytics")
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Cloud Messaging (FCM)
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * الرسائل السحابية
     */
    object Messaging {
        
        /**
         * مستمع الإشعارات
         */
        interface NotificationListener {
            fun onMessageReceived(title: String?, body: String?, data: Map<String, String>)
            fun onTokenRefresh(token: String)
        }
        
        private var listener: NotificationListener? = null
        
        /**
         * تعيين المستمع
         */
        fun setListener(notificationListener: NotificationListener) {
            listener = notificationListener
        }
        
        /**
         * الحصول على رمز FCM
         */
        suspend fun getToken(): String? = withContext(Dispatchers.IO) {
            suspendCancellableCoroutine { continuation ->
                FirebaseMessaging.getInstance().token
                    .addOnSuccessListener { token ->
                        continuation.resume(token)
                    }
                    .addOnFailureListener { exception ->
                        continuation.resumeWithException(exception)
                    }
            }
        }
        
        /**
         * الاشتراك في موضوع
         */
        suspend fun subscribeToTopic(topic: String): Boolean = withContext(Dispatchers.IO) {
            suspendCancellableCoroutine { continuation ->
                FirebaseMessaging.getInstance().subscribeToTopic(topic)
                    .addOnSuccessListener {
                        continuation.resume(true)
                    }
                    .addOnFailureListener {
                        continuation.resume(false)
                    }
            }
        }
        
        /**
         * إلغاء الاشتراك من موضوع
         */
        suspend fun unsubscribeFromTopic(topic: String): Boolean = withContext(Dispatchers.IO) {
            suspendCancellableCoroutine { continuation ->
                FirebaseMessaging.getInstance().unsubscribeFromTopic(topic)
                    .addOnSuccessListener {
                        continuation.resume(true)
                    }
                    .addOnFailureListener {
                        continuation.resume(false)
                    }
            }
        }
        
        /**
         * معالجة الرسالة (يستدعى من FirebaseMessagingService)
         */
        internal fun handleMessage(remoteMessage: RemoteMessage) {
            val title = remoteMessage.notification?.title
            val body = remoteMessage.notification?.body
            val data = remoteMessage.data
            listener?.onMessageReceived(title, body, data)
        }
        
        /**
         * معالجة تحديث الرمز
         */
        internal fun handleTokenRefresh(token: String) {
            listener?.onTokenRefresh(token)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Remote Config
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * التكوين عن بُعد
     */
    object RemoteConfig {
        private var remoteConfig: FirebaseRemoteConfig? = null
        
        /**
         * تهيئة Remote Config
         */
        fun init(
            minimumFetchIntervalSeconds: Long = 3600,
            defaults: Map<String, Any> = emptyMap()
        ) {
            remoteConfig = FirebaseRemoteConfig.getInstance()
            
            val configSettings = FirebaseRemoteConfigSettings.Builder()
                .setMinimumFetchIntervalInSeconds(minimumFetchIntervalSeconds)
                .build()
            
            remoteConfig?.setConfigSettingsAsync(configSettings)
            
            if (defaults.isNotEmpty()) {
                remoteConfig?.setDefaultsAsync(defaults)
            }
        }
        
        /**
         * جلب التكوين
         */
        suspend fun fetch(): Boolean = withContext(Dispatchers.IO) {
            val config = remoteConfig ?: return@withContext false
            
            suspendCancellableCoroutine { continuation ->
                config.fetchAndActivate()
                    .addOnSuccessListener { changed ->
                        continuation.resume(changed)
                    }
                    .addOnFailureListener {
                        continuation.resume(false)
                    }
            }
        }
        
        /**
         * الحصول على قيمة نصية
         */
        fun getString(key: String): String {
            return remoteConfig?.getString(key) ?: ""
        }
        
        /**
         * الحصول على قيمة منطقية
         */
        fun getBoolean(key: String): Boolean {
            return remoteConfig?.getBoolean(key) ?: false
        }
        
        /**
         * الحصول على قيمة رقمية
         */
        fun getLong(key: String): Long {
            return remoteConfig?.getLong(key) ?: 0L
        }
        
        /**
         * الحصول على قيمة عشرية
         */
        fun getDouble(key: String): Double {
            return remoteConfig?.getDouble(key) ?: 0.0
        }
        
        /**
         * الحصول على جميع القيم
         */
        fun getAll(): Map<String, String> {
            return remoteConfig?.all?.mapValues { it.value.asString() } ?: emptyMap()
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Initialization
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تهيئة جميع خدمات Firebase
     */
    fun initAll(
        analyticsEnabled: Boolean = true,
        crashlyticsEnabled: Boolean = true,
        remoteConfigDefaults: Map<String, Any> = emptyMap()
    ) {
        if (analyticsEnabled) {
            Analytics.init(context)
        }
        
        if (crashlyticsEnabled) {
            Crashlytics.init()
        }
        
        RemoteConfig.init(defaults = remoteConfigDefaults)
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  FCM Service
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * خدمة استقبال الرسائل
 */
class SadFirebaseMessagingService : FirebaseMessagingService() {
    
    override fun onMessageReceived(remoteMessage: RemoteMessage) {
        super.onMessageReceived(remoteMessage)
        SadFirebase.Messaging.handleMessage(remoteMessage)
    }
    
    override fun onNewToken(token: String) {
        super.onNewToken(token)
        SadFirebase.Messaging.handleTokenRefresh(token)
    }
}
