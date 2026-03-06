// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadPermissionBridge.kt
// الوصف: جسر Kotlin لإدارة الأذونات في لغة ص
// ═══════════════════════════════════════════════════════════════════════════════
//
// هذا الملف يربط دوال __اندرويد_اذونات_* بـ Android Permission APIs
//
// الدوال المُنفّذة:
//   __اندرويد_اذونات_تحقق(permission) -> bool
//   __اندرويد_اذونات_اطلب(permissions[]) -> void
//   __اندرويد_اذونات_شرح(permission) -> bool
//
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.Manifest
import android.app.Activity
import android.content.Context
import android.content.pm.PackageManager
import androidx.activity.result.ActivityResultLauncher
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat

/**
 * جسر الأذونات — SadPermissionBridge
 * 
 * يوفر واجهة لإدارة الأذونات من كود لغة ص.
 */
object SadPermissionBridge {
    
    // ═══════════════════════════════════════════════════════════════════════════
    // ثوابت — ترجمة الأذونات من العربية
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * خريطة الأذونات من العربية إلى Android
     */
    private val permissionMap = mapOf(
        // الكاميرا
        "كاميرا" to Manifest.permission.CAMERA,
        "الكاميرا" to Manifest.permission.CAMERA,
        
        // الموقع
        "موقع" to Manifest.permission.ACCESS_FINE_LOCATION,
        "الموقع" to Manifest.permission.ACCESS_FINE_LOCATION,
        "موقع_دقيق" to Manifest.permission.ACCESS_FINE_LOCATION,
        "موقع_تقريبي" to Manifest.permission.ACCESS_COARSE_LOCATION,
        
        // التخزين
        "تخزين" to Manifest.permission.WRITE_EXTERNAL_STORAGE,
        "قراءة_ملفات" to Manifest.permission.READ_EXTERNAL_STORAGE,
        "كتابة_ملفات" to Manifest.permission.WRITE_EXTERNAL_STORAGE,
        
        // الإشعارات (Android 13+)
        "اشعارات" to "android.permission.POST_NOTIFICATIONS",
        "الإشعارات" to "android.permission.POST_NOTIFICATIONS",
        
        // جهات الاتصال
        "جهات_اتصال" to Manifest.permission.READ_CONTACTS,
        "قراءة_جهات_اتصال" to Manifest.permission.READ_CONTACTS,
        "كتابة_جهات_اتصال" to Manifest.permission.WRITE_CONTACTS,
        
        // الهاتف
        "هاتف" to Manifest.permission.CALL_PHONE,
        "اتصال" to Manifest.permission.CALL_PHONE,
        
        // الرسائل
        "رسائل" to Manifest.permission.SEND_SMS,
        "ارسال_رسائل" to Manifest.permission.SEND_SMS,
        "قراءة_رسائل" to Manifest.permission.READ_SMS,
        
        // التسجيل الصوتي
        "ميكروفون" to Manifest.permission.RECORD_AUDIO,
        "تسجيل_صوتي" to Manifest.permission.RECORD_AUDIO,
        
        // التقويم
        "تقويم" to Manifest.permission.READ_CALENDAR,
        "قراءة_تقويم" to Manifest.permission.READ_CALENDAR,
        "كتابة_تقويم" to Manifest.permission.WRITE_CALENDAR,
        
        // البلوتوث
        "بلوتوث" to Manifest.permission.BLUETOOTH,
        "بلوتوث_متصل" to Manifest.permission.BLUETOOTH_CONNECT
    )
    
    // ═══════════════════════════════════════════════════════════════════════════
    // إعداد
    // ═══════════════════════════════════════════════════════════════════════════
    
    private var activity: Activity? = null
    private var permissionCallback: ((Map<String, Boolean>) -> Unit)? = null
    
    /**
     * تهيئة الجسر
     */
    @JvmStatic
    fun initialize(act: Activity) {
        activity = act
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_اذونات_ترجم — ترجمة إذن عربي
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * ترجمة اسم الإذن من العربية
     */
    @JvmStatic
    fun translatePermission(arabicName: String): String {
        return permissionMap[arabicName] ?: arabicName
    }
    
    /**
     * ترجمة قائمة أذونات
     */
    @JvmStatic
    fun translatePermissions(arabicNames: List<String>): List<String> {
        return arabicNames.map { translatePermission(it) }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_اذونات_تحقق — التحقق من إذن
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * التحقق من وجود إذن
     * 
     * @param permission الإذن (عربي أو Android)
     * @return true إذا الإذن ممنوح
     */
    @JvmStatic
    fun hasPermission(permission: String): Boolean {
        val act = activity ?: return false
        val androidPermission = translatePermission(permission)
        
        return ContextCompat.checkSelfPermission(
            act,
            androidPermission
        ) == PackageManager.PERMISSION_GRANTED
    }
    
    /**
     * التحقق من قائمة أذونات
     */
    @JvmStatic
    fun hasPermissions(permissions: List<String>): Map<String, Boolean> {
        return permissions.associateWith { hasPermission(it) }
    }
    
    /**
     * التحقق من جميع الأذونات
     */
    @JvmStatic
    fun hasAllPermissions(permissions: List<String>): Boolean {
        return permissions.all { hasPermission(it) }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_اذونات_اطلب — طلب أذونات
    // ═══════════════════════════════════════════════════════════════════════════
    
    private const val PERMISSION_REQUEST_CODE = 1001
    
    /**
     * طلب أذونات
     * 
     * @param permissions قائمة الأذونات (عربي أو Android)
     */
    @JvmStatic
    fun requestPermissions(permissions: List<String>) {
        val act = activity ?: return
        
        val androidPermissions = translatePermissions(permissions)
            .filter { !hasPermission(it) }
            .toTypedArray()
        
        if (androidPermissions.isEmpty()) return
        
        ActivityCompat.requestPermissions(
            act,
            androidPermissions,
            PERMISSION_REQUEST_CODE
        )
    }
    
    /**
     * طلب إذن واحد
     */
    @JvmStatic
    fun requestPermission(permission: String) {
        requestPermissions(listOf(permission))
    }
    
    /**
     * معالجة نتيجة طلب الأذونات
     * يجب استدعاؤها من onRequestPermissionsResult
     */
    @JvmStatic
    fun handlePermissionResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray,
        callback: ((Map<String, Boolean>) -> Unit)? = null
    ) {
        if (requestCode != PERMISSION_REQUEST_CODE) return
        
        val result = mutableMapOf<String, Boolean>()
        permissions.forEachIndexed { index, permission ->
            result[permission] = grantResults[index] == PackageManager.PERMISSION_GRANTED
        }
        
        callback?.invoke(result)
        permissionCallback?.invoke(result)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_اذونات_شرح — هل يجب شرح الإذن؟
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * هل يجب إظهار شرح للإذن؟
     * (يُستخدم عندما يرفض المستخدم الإذن سابقاً)
     */
    @JvmStatic
    fun shouldShowRationale(permission: String): Boolean {
        val act = activity ?: return false
        val androidPermission = translatePermission(permission)
        
        return ActivityCompat.shouldShowRequestPermissionRationale(
            act,
            androidPermission
        )
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال مساعدة
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * قائمة الأذونات المتاحة بالعربية
     */
    @JvmStatic
    fun getAvailableArabicPermissions(): List<String> {
        return permissionMap.keys.toList()
    }
    
    /**
     * الحصول على وصف الإذن
     */
    @JvmStatic
    fun getPermissionDescription(permission: String): String {
        return when (translatePermission(permission)) {
            Manifest.permission.CAMERA -> "الوصول للكاميرا لالتقاط الصور والفيديو"
            Manifest.permission.ACCESS_FINE_LOCATION -> "الوصول للموقع الدقيق"
            Manifest.permission.ACCESS_COARSE_LOCATION -> "الوصول للموقع التقريبي"
            Manifest.permission.WRITE_EXTERNAL_STORAGE -> "حفظ الملفات على الجهاز"
            Manifest.permission.READ_EXTERNAL_STORAGE -> "قراءة الملفات من الجهاز"
            Manifest.permission.RECORD_AUDIO -> "تسجيل الصوت"
            Manifest.permission.READ_CONTACTS -> "قراءة جهات الاتصال"
            Manifest.permission.CALL_PHONE -> "إجراء مكالمات هاتفية"
            Manifest.permission.SEND_SMS -> "إرسال رسائل نصية"
            "android.permission.POST_NOTIFICATIONS" -> "إرسال إشعارات"
            else -> "إذن غير معروف"
        }
    }
}
