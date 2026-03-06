// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadBridgeManager.kt
// الوصف: مدير جسور لغة ص — نقطة الدخول الرئيسية
// ═══════════════════════════════════════════════════════════════════════════════
//
// هذا الملف يدير تهيئة جميع الجسور ويوفر واجهة موحدة للاستخدام.
//
// الاستخدام في MainActivity.kt:
//   class MainActivity : ComponentActivity() {
//       override fun onCreate(savedInstanceState: Bundle?) {
//           super.onCreate(savedInstanceState)
//           SadBridgeManager.initialize(this)
//           ...
//       }
//   }
//
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.app.Activity
import android.content.Context
import androidx.lifecycle.LifecycleOwner

/**
 * مدير جسور لغة ص — SadBridgeManager
 * 
 * نقطة الدخول المركزية لتهيئة واستخدام جميع الجسور.
 * يجب استدعاء initialize() في onCreate() الخاص بـ Activity.
 */
object SadBridgeManager {
    
    // ═══════════════════════════════════════════════════════════════════════════
    // حالة التهيئة
    // ═══════════════════════════════════════════════════════════════════════════
    
    private var isInitialized = false
    private var currentActivity: Activity? = null
    
    /**
     * التحقق من تهيئة الجسور
     */
    @JvmStatic
    fun isReady(): Boolean = isInitialized
    
    // ═══════════════════════════════════════════════════════════════════════════
    // التهيئة
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تهيئة جميع الجسور
     * 
     * @param activity الـ Activity الرئيسي
     * @param lifecycleOwner مالك دورة الحياة (للكاميرا)
     * 
     * مثال الاستخدام:
     * ```kotlin
     * class MainActivity : ComponentActivity() {
     *     override fun onCreate(savedInstanceState: Bundle?) {
     *         super.onCreate(savedInstanceState)
     *         SadBridgeManager.initialize(this)
     *     }
     * }
     * ```
     */
    @JvmStatic
    fun initialize(activity: Activity, lifecycleOwner: LifecycleOwner? = null) {
        currentActivity = activity
        val context = activity.applicationContext
        
        // تهيئة جسر HTTP
        // SadHttpBridge لا يحتاج تهيئة خاصة
        
        // تهيئة جسر الإشعارات
        SadNotificationBridge.initialize(context)
        
        // تهيئة جسر التخزين
        SadStorageBridge.initialize(context)
        
        // تهيئة جسر الموقع
        SadLocationBridge.initialize(context)
        
        // تهيئة جسر الكاميرا (إذا كان لدينا LifecycleOwner)
        if (lifecycleOwner != null) {
            SadCameraBridge.initialize(context, lifecycleOwner)
        } else if (activity is LifecycleOwner) {
            SadCameraBridge.initialize(context, activity)
        }
        
        // تهيئة جسر الأذونات
        SadPermissionBridge.initialize(activity)
        
        isInitialized = true
    }
    
    /**
     * تهيئة بسيطة (للاستخدام السريع)
     */
    @JvmStatic
    fun initializeSimple(context: Context) {
        SadNotificationBridge.initialize(context)
        SadStorageBridge.initialize(context)
        SadLocationBridge.initialize(context)
        isInitialized = true
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // التنظيف
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تنظيف الموارد
     * يُستدعى في onDestroy()
     */
    @JvmStatic
    fun cleanup() {
        SadCameraBridge.cleanup()
        SadLocationBridge.stopAllTracking()
        currentActivity = null
        isInitialized = false
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // واجهات الوصول السريع
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * HTTP — طلب GET سريع
     */
    @JvmStatic
    fun httpGet(url: String, callback: (Map<String, Any?>) -> Unit) {
        SadHttpBridge.httpGetAsync(url, emptyMap(), callback)
    }
    
    /**
     * HTTP — طلب POST سريع
     */
    @JvmStatic
    fun httpPost(url: String, body: String, callback: (Map<String, Any?>) -> Unit) {
        SadHttpBridge.httpPostAsync(url, body, emptyMap(), callback)
    }
    
    /**
     * إرسال إشعار سريع
     */
    @JvmStatic
    fun notify(title: String, message: String): Int {
        return SadNotificationBridge.sendNotification(title, message)
    }
    
    /**
     * حفظ قيمة
     */
    @JvmStatic
    fun save(key: String, value: Any): Boolean {
        return SadStorageBridge.save(key, value)
    }
    
    /**
     * قراءة قيمة
     */
    @JvmStatic
    fun read(key: String, default: String = ""): String {
        return SadStorageBridge.read(key, default)
    }
    
    /**
     * الحصول على الموقع
     */
    @JvmStatic
    fun getLocation(callback: (Map<String, Any?>) -> Unit) {
        SadLocationBridge.getCurrentLocation(callback)
    }
    
    /**
     * التحقق من إذن
     */
    @JvmStatic
    fun hasPermission(permission: String): Boolean {
        return SadPermissionBridge.hasPermission(permission)
    }
    
    /**
     * طلب أذونات
     */
    @JvmStatic
    fun requestPermissions(permissions: List<String>) {
        SadPermissionBridge.requestPermissions(permissions)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال مساعدة
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * الحصول على الـ Activity الحالي
     */
    @JvmStatic
    fun getActivity(): Activity? = currentActivity
    
    /**
     * طباعة معلومات التصحيح
     */
    @JvmStatic
    fun debugInfo(): String {
        return buildString {
            appendLine("═══ معلومات جسور لغة ص ═══")
            appendLine("الحالة: ${if (isInitialized) "مُهيأ" else "غير مُهيأ"}")
            appendLine("Activity: ${currentActivity?.javaClass?.simpleName ?: "لا يوجد"}")
            appendLine()
            appendLine("الجسور المتاحة:")
            appendLine("  • HTTP: متاح")
            appendLine("  • الإشعارات: متاح")
            appendLine("  • التخزين: متاح")
            appendLine("  • الموقع: متاح")
            appendLine("  • الكاميرا: متاح")
            appendLine("  • الأذونات: متاح")
        }
    }
}
