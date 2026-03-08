/**
 * =============================================================================
 * @file SadPermissions.kt
 * @brief (AR) إدارة الأذونات لأندرويد
 * @brief (EN) Android Permissions Management
 * =============================================================================
 */

package sad.lang.android

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.os.Build
import androidx.activity.result.ActivityResultLauncher
import androidx.activity.result.contract.ActivityResultContracts
import androidx.core.content.ContextCompat
import androidx.fragment.app.FragmentActivity

/**
 * مدير الأذونات للغة ص
 */
class SadPermissions(private val context: Context) {
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Permission Groups
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * مجموعات الأذونات
     */
    object Groups {
        val CAMERA = arrayOf(Manifest.permission.CAMERA)
        
        val MICROPHONE = arrayOf(Manifest.permission.RECORD_AUDIO)
        
        val LOCATION = arrayOf(
            Manifest.permission.ACCESS_FINE_LOCATION,
            Manifest.permission.ACCESS_COARSE_LOCATION
        )
        
        val STORAGE = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            arrayOf(
                Manifest.permission.READ_MEDIA_IMAGES,
                Manifest.permission.READ_MEDIA_VIDEO,
                Manifest.permission.READ_MEDIA_AUDIO
            )
        } else {
            arrayOf(
                Manifest.permission.READ_EXTERNAL_STORAGE,
                Manifest.permission.WRITE_EXTERNAL_STORAGE
            )
        }
        
        val BLUETOOTH = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            arrayOf(
                Manifest.permission.BLUETOOTH_SCAN,
                Manifest.permission.BLUETOOTH_CONNECT
            )
        } else {
            arrayOf(
                Manifest.permission.BLUETOOTH,
                Manifest.permission.BLUETOOTH_ADMIN
            )
        }
        
        val NOTIFICATIONS = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            arrayOf(Manifest.permission.POST_NOTIFICATIONS)
        } else {
            emptyArray()
        }
        
        val CONTACTS = arrayOf(
            Manifest.permission.READ_CONTACTS,
            Manifest.permission.WRITE_CONTACTS
        )
        
        val PHONE = arrayOf(
            Manifest.permission.READ_PHONE_STATE,
            Manifest.permission.CALL_PHONE
        )
        
        val SMS = arrayOf(
            Manifest.permission.SEND_SMS,
            Manifest.permission.READ_SMS
        )
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Check Permissions
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * فحص إذن واحد
     */
    fun isGranted(permission: String): Boolean {
        return ContextCompat.checkSelfPermission(context, permission) == 
            PackageManager.PERMISSION_GRANTED
    }
    
    /**
     * فحص مجموعة أذونات
     */
    fun areGranted(permissions: Array<String>): Boolean {
        return permissions.all { isGranted(it) }
    }
    
    /**
     * فحص إذن الكاميرا
     */
    fun hasCameraPermission(): Boolean = areGranted(Groups.CAMERA)
    
    /**
     * فحص إذن الميكروفون
     */
    fun hasMicrophonePermission(): Boolean = areGranted(Groups.MICROPHONE)
    
    /**
     * فحص إذن الموقع
     */
    fun hasLocationPermission(): Boolean = areGranted(Groups.LOCATION)
    
    /**
     * فحص إذن التخزين
     */
    fun hasStoragePermission(): Boolean = areGranted(Groups.STORAGE)
    
    /**
     * فحص إذن البلوتوث
     */
    fun hasBluetoothPermission(): Boolean = areGranted(Groups.BLUETOOTH)
    
    /**
     * فحص إذن الإشعارات
     */
    fun hasNotificationPermission(): Boolean {
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            areGranted(Groups.NOTIFICATIONS)
        } else {
            true // لا حاجة للإذن في الإصدارات القديمة
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Request Permissions
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * مساعد لطلب الأذونات
     */
    class PermissionRequester(private val activity: FragmentActivity) {
        
        private var launcher: ActivityResultLauncher<Array<String>>? = null
        private var callback: ((Map<String, Boolean>) -> Unit)? = null
        
        /**
         * تهيئة المُطلق
         * يجب استدعاؤها في onCreate
         */
        fun register() {
            launcher = activity.registerForActivityResult(
                ActivityResultContracts.RequestMultiplePermissions()
            ) { results ->
                callback?.invoke(results)
                callback = null
            }
        }
        
        /**
         * طلب أذونات
         */
        fun request(
            permissions: Array<String>,
            onResult: (Map<String, Boolean>) -> Unit
        ) {
            callback = onResult
            launcher?.launch(permissions)
        }
        
        /**
         * طلب إذن الكاميرا
         */
        fun requestCamera(onResult: (Boolean) -> Unit) {
            request(Groups.CAMERA) { results ->
                onResult(results.values.all { it })
            }
        }
        
        /**
         * طلب إذن الميكروفون
         */
        fun requestMicrophone(onResult: (Boolean) -> Unit) {
            request(Groups.MICROPHONE) { results ->
                onResult(results.values.all { it })
            }
        }
        
        /**
         * طلب إذن الموقع
         */
        fun requestLocation(onResult: (Boolean) -> Unit) {
            request(Groups.LOCATION) { results ->
                onResult(results.values.all { it })
            }
        }
        
        /**
         * طلب إذن التخزين
         */
        fun requestStorage(onResult: (Boolean) -> Unit) {
            request(Groups.STORAGE) { results ->
                onResult(results.values.all { it })
            }
        }
        
        /**
         * طلب إذن البلوتوث
         */
        fun requestBluetooth(onResult: (Boolean) -> Unit) {
            request(Groups.BLUETOOTH) { results ->
                onResult(results.values.all { it })
            }
        }
        
        /**
         * طلب إذن الإشعارات
         */
        fun requestNotifications(onResult: (Boolean) -> Unit) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                request(Groups.NOTIFICATIONS) { results ->
                    onResult(results.values.all { it })
                }
            } else {
                onResult(true)
            }
        }
        
        /**
         * طلب جميع الأذونات الأساسية
         */
        fun requestAllBasic(onResult: (Boolean) -> Unit) {
            val allPermissions = mutableListOf<String>()
            allPermissions.addAll(Groups.CAMERA)
            allPermissions.addAll(Groups.MICROPHONE)
            allPermissions.addAll(Groups.LOCATION)
            
            request(allPermissions.toTypedArray()) { results ->
                onResult(results.values.all { it })
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Rationale
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * هل يجب إظهار تبرير للإذن؟
     */
    fun shouldShowRationale(activity: FragmentActivity, permission: String): Boolean {
        return activity.shouldShowRequestPermissionRationale(permission)
    }
    
    /**
     * الحصول على نص تبرير للإذن
     */
    fun getRationaleText(permission: String): String {
        return when {
            permission == Manifest.permission.CAMERA -> 
                "نحتاج إذن الكاميرا لالتقاط الصور"
            permission == Manifest.permission.RECORD_AUDIO -> 
                "نحتاج إذن الميكروفون للتسجيل الصوتي"
            permission.contains("LOCATION") -> 
                "نحتاج إذن الموقع لتحديد موقعك"
            permission.contains("STORAGE") || permission.contains("MEDIA") -> 
                "نحتاج إذن التخزين للوصول للملفات"
            permission.contains("BLUETOOTH") -> 
                "نحتاج إذن البلوتوث للاتصال بالأجهزة"
            permission == Manifest.permission.POST_NOTIFICATIONS -> 
                "نحتاج إذن الإشعارات لإرسال التنبيهات"
            else -> 
                "نحتاج هذا الإذن لتشغيل هذه الميزة"
        }
    }
}
