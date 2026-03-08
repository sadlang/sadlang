/**
 * =============================================================================
 * @file SadPermissions.kt
 * @brief (AR) نظام الصلاحيات — Runtime Permissions
 * @brief (EN) Permissions System — Runtime Permissions
 * 
 * المهمة: T248 - Permissions System
 * المرحلة: سبتمبر 2026 — Android Sensors
 * =============================================================================
 * 
 * 🎯 الميزات
 * ══════════
 * ✓ طلب صلاحيات وقت التشغيل
 * ✓ التحقق من الصلاحيات
 * ✓ شرح سبب الصلاحية
 * ✓ فتح إعدادات التطبيق
 * ✓ دعم جميع صلاحيات Android 13+
 * 
 * =============================================================================
 */

package sad.platform.android.permissions

import android.Manifest
import android.app.Activity
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.net.Uri
import android.os.Build
import android.provider.Settings
import androidx.activity.ComponentActivity
import androidx.activity.result.ActivityResultLauncher
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AlertDialog
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat

// =============================================================================
// الأنواع
// =============================================================================

/**
 * أنواع الصلاحيات
 */
enum class Permission(val androidManifest: String) {
    // الكاميرا
    CAMERA(Manifest.permission.CAMERA),
    
    // الموقع
    LOCATION_FINE(Manifest.permission.ACCESS_FINE_LOCATION),
    LOCATION_COARSE(Manifest.permission.ACCESS_COARSE_LOCATION),
    LOCATION_BACKGROUND(Manifest.permission.ACCESS_BACKGROUND_LOCATION),
    
    // التخزين
    READ_STORAGE(Manifest.permission.READ_EXTERNAL_STORAGE),
    WRITE_STORAGE(Manifest.permission.WRITE_EXTERNAL_STORAGE),
    MEDIA_IMAGES(Manifest.permission.READ_MEDIA_IMAGES),
    MEDIA_VIDEO(Manifest.permission.READ_MEDIA_VIDEO),
    MEDIA_AUDIO(Manifest.permission.READ_MEDIA_AUDIO),
    
    // الميكروفون
    MICROPHONE(Manifest.permission.RECORD_AUDIO),
    
    // الهاتف
    PHONE(Manifest.permission.CALL_PHONE),
    READ_PHONE_STATE(Manifest.permission.READ_PHONE_STATE),
    
    // الرسائل
    SMS_SEND(Manifest.permission.SEND_SMS),
    SMS_READ(Manifest.permission.READ_SMS),
    
    // جهات الاتصال
    CONTACTS_READ(Manifest.permission.READ_CONTACTS),
    CONTACTS_WRITE(Manifest.permission.WRITE_CONTACTS),
    
    // التقويم
    CALENDAR_READ(Manifest.permission.READ_CALENDAR),
    CALENDAR_WRITE(Manifest.permission.WRITE_CALENDAR),
    
    // المستشعرات
    BODY_SENSORS(Manifest.permission.BODY_SENSORS),
    ACTIVITY_RECOGNITION(Manifest.permission.ACTIVITY_RECOGNITION),
    
    // البلوتوث
    BLUETOOTH(Manifest.permission.BLUETOOTH),
    BLUETOOTH_ADMIN(Manifest.permission.BLUETOOTH_ADMIN),
    BLUETOOTH_CONNECT(Manifest.permission.BLUETOOTH_CONNECT),
    BLUETOOTH_SCAN(Manifest.permission.BLUETOOTH_SCAN),
    
    // الإشعارات
    NOTIFICATIONS(Manifest.permission.POST_NOTIFICATIONS),
    
    // الخلفية
    FOREGROUND_SERVICE(Manifest.permission.FOREGROUND_SERVICE),
    WAKE_LOCK(Manifest.permission.WAKE_LOCK);
    
    /**
     * الوصف العربي للصلاحية
     */
    fun getArabicDescription(): String = when (this) {
        CAMERA -> "الوصول للكاميرا لالتقاط الصور والفيديو"
        LOCATION_FINE -> "الوصول الدقيق للموقع الجغرافي"
        LOCATION_COARSE -> "الوصول التقريبي للموقع الجغرافي"
        LOCATION_BACKGROUND -> "الوصول للموقع في الخلفية"
        READ_STORAGE -> "قراءة الملفات من الجهاز"
        WRITE_STORAGE -> "حفظ الملفات على الجهاز"
        MEDIA_IMAGES -> "الوصول للصور"
        MEDIA_VIDEO -> "الوصول لمقاطع الفيديو"
        MEDIA_AUDIO -> "الوصول للملفات الصوتية"
        MICROPHONE -> "تسجيل الصوت عبر الميكروفون"
        PHONE -> "إجراء مكالمات هاتفية"
        READ_PHONE_STATE -> "قراءة معلومات الهاتف"
        SMS_SEND -> "إرسال رسائل SMS"
        SMS_READ -> "قراءة رسائل SMS"
        CONTACTS_READ -> "قراءة جهات الاتصال"
        CONTACTS_WRITE -> "تعديل جهات الاتصال"
        CALENDAR_READ -> "قراءة التقويم"
        CALENDAR_WRITE -> "تعديل التقويم"
        BODY_SENSORS -> "الوصول لمستشعرات الجسم"
        ACTIVITY_RECOGNITION -> "تتبع النشاط البدني"
        BLUETOOTH -> "استخدام البلوتوث"
        BLUETOOTH_ADMIN -> "إدارة البلوتوث"
        BLUETOOTH_CONNECT -> "الاتصال بأجهزة البلوتوث"
        BLUETOOTH_SCAN -> "البحث عن أجهزة البلوتوث"
        NOTIFICATIONS -> "عرض الإشعارات"
        FOREGROUND_SERVICE -> "تشغيل خدمة في المقدمة"
        WAKE_LOCK -> "منع الجهاز من النوم"
    }
}

/**
 * نتيجة طلب الصلاحية
 */
enum class PermissionResult {
    GRANTED,            // تم المنح
    DENIED,             // تم الرفض
    DENIED_PERMANENTLY  // تم الرفض نهائياً (Don't ask again)
}

/**
 * مستمع الصلاحيات
 */
interface SadPermissionListener {
    fun onPermissionResult(permission: Permission, result: PermissionResult)
    fun onMultiplePermissionsResult(results: Map<Permission, PermissionResult>)
}

// =============================================================================
// SadPermissions
// =============================================================================

class SadPermissions(private val activity: ComponentActivity) {
    
    // ─────────────────────────────────────────────
    // الخصائص
    // ─────────────────────────────────────────────
    
    var listener: SadPermissionListener? = null
    
    private var pendingPermission: Permission? = null
    private var pendingPermissions: List<Permission> = emptyList()
    
    private val singlePermissionLauncher: ActivityResultLauncher<String> =
        activity.registerForActivityResult(
            ActivityResultContracts.RequestPermission()
        ) { isGranted ->
            val permission = pendingPermission ?: return@registerForActivityResult
            val result = when {
                isGranted -> PermissionResult.GRANTED
                shouldShowRationale(permission) -> PermissionResult.DENIED
                else -> PermissionResult.DENIED_PERMANENTLY
            }
            listener?.onPermissionResult(permission, result)
            pendingPermission = null
        }
    
    private val multiplePermissionsLauncher: ActivityResultLauncher<Array<String>> =
        activity.registerForActivityResult(
            ActivityResultContracts.RequestMultiplePermissions()
        ) { permissions ->
            val results = mutableMapOf<Permission, PermissionResult>()
            
            for (perm in pendingPermissions) {
                val isGranted = permissions[perm.androidManifest] ?: false
                results[perm] = when {
                    isGranted -> PermissionResult.GRANTED
                    shouldShowRationale(perm) -> PermissionResult.DENIED
                    else -> PermissionResult.DENIED_PERMANENTLY
                }
            }
            
            listener?.onMultiplePermissionsResult(results)
            pendingPermissions = emptyList()
        }
    
    // ─────────────────────────────────────────────
    // التحقق من الصلاحيات
    // ─────────────────────────────────────────────
    
    /**
     * هل الصلاحية ممنوحة؟
     */
    fun isGranted(permission: Permission): Boolean {
        return ContextCompat.checkSelfPermission(
            activity,
            permission.androidManifest
        ) == PackageManager.PERMISSION_GRANTED
    }
    
    /**
     * هل جميع الصلاحيات ممنوحة؟
     */
    fun areAllGranted(permissions: List<Permission>): Boolean {
        return permissions.all { isGranted(it) }
    }
    
    /**
     * هل يجب شرح سبب الصلاحية؟
     */
    fun shouldShowRationale(permission: Permission): Boolean {
        return ActivityCompat.shouldShowRequestPermissionRationale(
            activity,
            permission.androidManifest
        )
    }
    
    // ─────────────────────────────────────────────
    // طلب الصلاحيات
    // ─────────────────────────────────────────────
    
    /**
     * طلب صلاحية واحدة
     */
    fun request(permission: Permission) {
        if (isGranted(permission)) {
            listener?.onPermissionResult(permission, PermissionResult.GRANTED)
            return
        }
        
        pendingPermission = permission
        singlePermissionLauncher.launch(permission.androidManifest)
    }
    
    /**
     * طلب صلاحية مع شرح
     */
    fun requestWithRationale(
        permission: Permission,
        title: String = "مطلوب صلاحية",
        message: String = permission.getArabicDescription()
    ) {
        if (isGranted(permission)) {
            listener?.onPermissionResult(permission, PermissionResult.GRANTED)
            return
        }
        
        if (shouldShowRationale(permission)) {
            showRationaleDialog(title, message) {
                request(permission)
            }
        } else {
            request(permission)
        }
    }
    
    /**
     * طلب صلاحيات متعددة
     */
    fun requestMultiple(permissions: List<Permission>) {
        val notGranted = permissions.filter { !isGranted(it) }
        
        if (notGranted.isEmpty()) {
            val results = permissions.associateWith { PermissionResult.GRANTED }
            listener?.onMultiplePermissionsResult(results)
            return
        }
        
        pendingPermissions = notGranted
        multiplePermissionsLauncher.launch(
            notGranted.map { it.androidManifest }.toTypedArray()
        )
    }
    
    /**
     * طلب صلاحيات مجموعة (مثل: الكاميرا + الموقع)
     */
    fun requestGroup(vararg permissions: Permission) {
        requestMultiple(permissions.toList())
    }
    
    // ─────────────────────────────────────────────
    // مجموعات شائعة
    // ─────────────────────────────────────────────
    
    /**
     * صلاحيات الكاميرا الكاملة
     */
    fun requestCameraPermissions() {
        requestMultiple(listOf(
            Permission.CAMERA,
            Permission.WRITE_STORAGE
        ))
    }
    
    /**
     * صلاحيات الموقع الكاملة
     */
    fun requestLocationPermissions(includeBackground: Boolean = false) {
        val permissions = mutableListOf(
            Permission.LOCATION_FINE,
            Permission.LOCATION_COARSE
        )
        if (includeBackground && Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            permissions.add(Permission.LOCATION_BACKGROUND)
        }
        requestMultiple(permissions)
    }
    
    /**
     * صلاحيات التخزين
     */
    fun requestStoragePermissions() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            requestMultiple(listOf(
                Permission.MEDIA_IMAGES,
                Permission.MEDIA_VIDEO,
                Permission.MEDIA_AUDIO
            ))
        } else {
            requestMultiple(listOf(
                Permission.READ_STORAGE,
                Permission.WRITE_STORAGE
            ))
        }
    }
    
    // ─────────────────────────────────────────────
    // الإعدادات
    // ─────────────────────────────────────────────
    
    /**
     * فتح إعدادات التطبيق
     */
    fun openAppSettings() {
        val intent = Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS).apply {
            data = Uri.fromParts("package", activity.packageName, null)
        }
        activity.startActivity(intent)
    }
    
    /**
     * عرض dialog للانتقال للإعدادات (عند الرفض النهائي)
     */
    fun showSettingsDialog(
        title: String = "الصلاحية مطلوبة",
        message: String = "يرجى منح الصلاحية من إعدادات التطبيق"
    ) {
        AlertDialog.Builder(activity)
            .setTitle(title)
            .setMessage(message)
            .setPositiveButton("فتح الإعدادات") { _, _ -> openAppSettings() }
            .setNegativeButton("إلغاء", null)
            .show()
    }
    
    // ─────────────────────────────────────────────
    // Private
    // ─────────────────────────────────────────────
    
    private fun showRationaleDialog(
        title: String,
        message: String,
        onAccept: () -> Unit
    ) {
        AlertDialog.Builder(activity)
            .setTitle(title)
            .setMessage(message)
            .setPositiveButton("موافق") { _, _ -> onAccept() }
            .setNegativeButton("إلغاء", null)
            .show()
    }
}

// =============================================================================
// دوال مساعدة للغة ص (JNI)
// =============================================================================

/**
 * إنشاء مدير الصلاحيات
 */
@JvmStatic
fun صلاحية_انشئ(activity: ComponentActivity): SadPermissions = SadPermissions(activity)

/**
 * التحقق من صلاحية
 */
@JvmStatic
fun صلاحية_موجودة(permissions: SadPermissions, name: String): Boolean {
    val perm = Permission.values().find { it.name == name } ?: return false
    return permissions.isGranted(perm)
}

/**
 * طلب صلاحية الكاميرا
 */
@JvmStatic
fun صلاحية_كاميرا(permissions: SadPermissions) {
    permissions.requestCameraPermissions()
}

/**
 * طلب صلاحية الموقع
 */
@JvmStatic
fun صلاحية_موقع(permissions: SadPermissions) {
    permissions.requestLocationPermissions()
}

/**
 * طلب صلاحية التخزين
 */
@JvmStatic
fun صلاحية_تخزين(permissions: SadPermissions) {
    permissions.requestStoragePermissions()
}

/**
 * طلب صلاحية الميكروفون
 */
@JvmStatic
fun صلاحية_ميكروفون(permissions: SadPermissions) {
    permissions.request(Permission.MICROPHONE)
}

/**
 * فتح إعدادات التطبيق
 */
@JvmStatic
fun صلاحية_إعدادات(permissions: SadPermissions) {
    permissions.openAppSettings()
}
