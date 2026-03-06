package sad.android.communication.intent

import android.app.Activity
import android.content.ActivityNotFoundException
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.net.Uri
import android.os.Build
import android.provider.ContactsContract
import android.provider.MediaStore
import android.provider.Settings
import androidx.core.content.FileProvider

/**
 * جسر Intent - إدارة النوايا وفتح التطبيقات
 * Intent Bridge - Intent management and app launching
 * 
 * @author فريق لغة ص
 */
object SadIntentBridge {
    
    /**
     * نتيجة Intent / Intent result
     */
    sealed class IntentResult {
        object Success : IntentResult()
        data class Error(val message: String) : IntentResult()
        object NotSupported : IntentResult()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         المتصفح والروابط
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * فتح رابط في المتصفح / Open URL in browser
     */
    fun openUrl(context: Context, url: String): IntentResult {
        return try {
            val intent = Intent(Intent.ACTION_VIEW, Uri.parse(url))
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            context.startActivity(intent)
            IntentResult.Success
        } catch (e: ActivityNotFoundException) {
            IntentResult.NotSupported
        } catch (e: Exception) {
            IntentResult.Error(e.message ?: "خطأ")
        }
    }
    
    /**
     * فتح رابط في متصفح Chrome / Open in Chrome
     */
    fun openInChrome(context: Context, url: String): IntentResult {
        return try {
            val intent = Intent(Intent.ACTION_VIEW, Uri.parse(url))
            intent.setPackage("com.android.chrome")
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            context.startActivity(intent)
            IntentResult.Success
        } catch (e: ActivityNotFoundException) {
            openUrl(context, url)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الخرائط
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * فتح موقع على الخريطة / Open location on map
     */
    fun openMap(context: Context, latitude: Double, longitude: Double, label: String? = null): IntentResult {
        val uri = if (label != null) {
            Uri.parse("geo:$latitude,$longitude?q=$latitude,$longitude($label)")
        } else {
            Uri.parse("geo:$latitude,$longitude?q=$latitude,$longitude")
        }
        
        return try {
            val intent = Intent(Intent.ACTION_VIEW, uri)
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            context.startActivity(intent)
            IntentResult.Success
        } catch (e: ActivityNotFoundException) {
            IntentResult.NotSupported
        }
    }
    
    /**
     * فتح Google Maps / Open Google Maps
     */
    fun openGoogleMaps(context: Context, latitude: Double, longitude: Double): IntentResult {
        val uri = Uri.parse("https://www.google.com/maps/search/?api=1&query=$latitude,$longitude")
        return openUrl(context, uri.toString())
    }
    
    /**
     * الملاحة إلى موقع / Navigate to location
     */
    fun navigateTo(context: Context, latitude: Double, longitude: Double): IntentResult {
        val uri = Uri.parse("google.navigation:q=$latitude,$longitude")
        return try {
            val intent = Intent(Intent.ACTION_VIEW, uri)
            intent.setPackage("com.google.android.apps.maps")
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            context.startActivity(intent)
            IntentResult.Success
        } catch (e: ActivityNotFoundException) {
            IntentResult.NotSupported
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الإعدادات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * فتح إعدادات التطبيق / Open app settings
     */
    fun openAppSettings(context: Context): IntentResult {
        return try {
            val intent = Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS)
            intent.data = Uri.fromParts("package", context.packageName, null)
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            context.startActivity(intent)
            IntentResult.Success
        } catch (e: Exception) {
            IntentResult.Error(e.message ?: "خطأ")
        }
    }
    
    /**
     * فتح إعدادات Wi-Fi / Open Wi-Fi settings
     */
    fun openWifiSettings(context: Context) = openSettings(context, Settings.ACTION_WIFI_SETTINGS)
    
    /**
     * فتح إعدادات البلوتوث / Open Bluetooth settings
     */
    fun openBluetoothSettings(context: Context) = openSettings(context, Settings.ACTION_BLUETOOTH_SETTINGS)
    
    /**
     * فتح إعدادات الموقع / Open location settings
     */
    fun openLocationSettings(context: Context) = openSettings(context, Settings.ACTION_LOCATION_SOURCE_SETTINGS)
    
    /**
     * فتح إعدادات NFC / Open NFC settings
     */
    fun openNfcSettings(context: Context) = openSettings(context, Settings.ACTION_NFC_SETTINGS)
    
    /**
     * فتح إعدادات الإشعارات / Open notification settings
     */
    fun openNotificationSettings(context: Context): IntentResult {
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val intent = Intent(Settings.ACTION_APP_NOTIFICATION_SETTINGS)
            intent.putExtra(Settings.EXTRA_APP_PACKAGE, context.packageName)
            openSettingsIntent(context, intent)
        } else {
            openAppSettings(context)
        }
    }
    
    /**
     * فتح إعدادات التاريخ والوقت / Open date/time settings
     */
    fun openDateTimeSettings(context: Context) = openSettings(context, Settings.ACTION_DATE_SETTINGS)
    
    /**
     * فتح إعدادات اللغة / Open language settings
     */
    fun openLanguageSettings(context: Context) = openSettings(context, Settings.ACTION_LOCALE_SETTINGS)
    
    /**
     * فتح إعدادات الصوت / Open sound settings
     */
    fun openSoundSettings(context: Context) = openSettings(context, Settings.ACTION_SOUND_SETTINGS)
    
    /**
     * فتح إعدادات الشاشة / Open display settings
     */
    fun openDisplaySettings(context: Context) = openSettings(context, Settings.ACTION_DISPLAY_SETTINGS)
    
    /**
     * فتح إعدادات الأمان / Open security settings
     */
    fun openSecuritySettings(context: Context) = openSettings(context, Settings.ACTION_SECURITY_SETTINGS)
    
    /**
     * فتح إعدادات البطارية / Open battery settings
     */
    fun openBatterySettings(context: Context): IntentResult {
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            openSettings(context, Settings.ACTION_IGNORE_BATTERY_OPTIMIZATION_SETTINGS)
        } else {
            openSettings(context, Settings.ACTION_SETTINGS)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         المتاجر
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * فتح صفحة التطبيق في المتجر / Open app in Play Store
     */
    fun openPlayStore(context: Context, packageName: String = context.packageName): IntentResult {
        return try {
            val intent = Intent(Intent.ACTION_VIEW, Uri.parse("market://details?id=$packageName"))
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            context.startActivity(intent)
            IntentResult.Success
        } catch (e: ActivityNotFoundException) {
            openUrl(context, "https://play.google.com/store/apps/details?id=$packageName")
        }
    }
    
    /**
     * فتح صفحة المطور في المتجر / Open developer page
     */
    fun openDeveloperPage(context: Context, developerId: String): IntentResult {
        return try {
            val intent = Intent(Intent.ACTION_VIEW, Uri.parse("market://dev?id=$developerId"))
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            context.startActivity(intent)
            IntentResult.Success
        } catch (e: ActivityNotFoundException) {
            openUrl(context, "https://play.google.com/store/apps/dev?id=$developerId")
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التطبيقات الأخرى
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * فتح تطبيق / Open app
     */
    fun openApp(context: Context, packageName: String): IntentResult {
        return try {
            val intent = context.packageManager.getLaunchIntentForPackage(packageName)
            if (intent != null) {
                intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
                context.startActivity(intent)
                IntentResult.Success
            } else {
                IntentResult.NotSupported
            }
        } catch (e: Exception) {
            IntentResult.Error(e.message ?: "التطبيق غير موجود")
        }
    }
    
    /**
     * هل التطبيق مثبت؟ / Is app installed?
     */
    fun isAppInstalled(context: Context, packageName: String): Boolean {
        return try {
            context.packageManager.getPackageInfo(packageName, 0)
            true
        } catch (e: PackageManager.NameNotFoundException) {
            false
        }
    }
    
    /**
     * جلب التطبيقات المثبتة / Get installed apps
     */
    fun getInstalledApps(context: Context, includeSystem: Boolean = false): List<String> {
        val apps = mutableListOf<String>()
        val packages = context.packageManager.getInstalledPackages(0)
        
        for (packageInfo in packages) {
            if (includeSystem || (packageInfo.applicationInfo.flags and android.content.pm.ApplicationInfo.FLAG_SYSTEM) == 0) {
                apps.add(packageInfo.packageName)
            }
        }
        
        return apps
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الميديا
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * فتح الكاميرا / Open camera
     */
    fun openCamera(context: Context): IntentResult {
        return try {
            val intent = Intent(MediaStore.ACTION_IMAGE_CAPTURE)
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            context.startActivity(intent)
            IntentResult.Success
        } catch (e: Exception) {
            IntentResult.Error(e.message ?: "خطأ")
        }
    }
    
    /**
     * فتح كاميرا الفيديو / Open video camera
     */
    fun openVideoCamera(context: Context): IntentResult {
        return try {
            val intent = Intent(MediaStore.ACTION_VIDEO_CAPTURE)
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            context.startActivity(intent)
            IntentResult.Success
        } catch (e: Exception) {
            IntentResult.Error(e.message ?: "خطأ")
        }
    }
    
    /**
     * فتح معرض الصور / Open gallery
     */
    fun openGallery(context: Context): IntentResult {
        return try {
            val intent = Intent(Intent.ACTION_PICK, MediaStore.Images.Media.EXTERNAL_CONTENT_URI)
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            context.startActivity(intent)
            IntentResult.Success
        } catch (e: Exception) {
            IntentResult.Error(e.message ?: "خطأ")
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         جهات الاتصال
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * فتح جهات الاتصال / Open contacts
     */
    fun openContacts(context: Context): IntentResult {
        return try {
            val intent = Intent(Intent.ACTION_VIEW, ContactsContract.Contacts.CONTENT_URI)
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            context.startActivity(intent)
            IntentResult.Success
        } catch (e: Exception) {
            IntentResult.Error(e.message ?: "خطأ")
        }
    }
    
    /**
     * اختيار جهة اتصال / Pick contact
     */
    fun pickContact(activity: Activity, requestCode: Int) {
        val intent = Intent(Intent.ACTION_PICK, ContactsContract.Contacts.CONTENT_URI)
        activity.startActivityForResult(intent, requestCode)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الملفات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * فتح ملف / Open file
     */
    fun openFile(context: Context, uri: Uri, mimeType: String): IntentResult {
        return try {
            val intent = Intent(Intent.ACTION_VIEW)
            intent.setDataAndType(uri, mimeType)
            intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            context.startActivity(intent)
            IntentResult.Success
        } catch (e: ActivityNotFoundException) {
            IntentResult.NotSupported
        }
    }
    
    /**
     * اختيار ملف / Pick file
     */
    fun pickFile(activity: Activity, mimeType: String, requestCode: Int) {
        val intent = Intent(Intent.ACTION_GET_CONTENT)
        intent.type = mimeType
        intent.addCategory(Intent.CATEGORY_OPENABLE)
        activity.startActivityForResult(Intent.createChooser(intent, "اختر ملف"), requestCode)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    private fun openSettings(context: Context, action: String): IntentResult {
        return try {
            val intent = Intent(action)
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            context.startActivity(intent)
            IntentResult.Success
        } catch (e: Exception) {
            IntentResult.Error(e.message ?: "خطأ")
        }
    }
    
    private fun openSettingsIntent(context: Context, intent: Intent): IntentResult {
        return try {
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            context.startActivity(intent)
            IntentResult.Success
        } catch (e: Exception) {
            IntentResult.Error(e.message ?: "خطأ")
        }
    }
}
