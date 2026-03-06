// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadIntentBridge.kt
// الوصف: جسر Kotlin للـ Intents والتنقل بين الأنشطة
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.app.Activity
import android.content.ActivityNotFoundException
import android.content.Context
import android.content.Intent
import android.net.Uri
import android.os.Bundle
import android.provider.ContactsContract
import android.provider.MediaStore
import android.provider.Settings
import androidx.core.net.toUri
import java.io.File

/**
 * جسر الـ Intents — SadIntentBridge
 */
object SadIntentBridge {
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // إنشاء Intent
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun createIntent(action: String? = null): Intent {
        return if (action != null) Intent(action) else Intent()
    }
    
    @JvmStatic
    fun createActivityIntent(context: Context, activityClass: Class<*>): Intent {
        return Intent(context, activityClass)
    }
    
    @JvmStatic
    fun createViewIntent(uri: String): Intent {
        return Intent(Intent.ACTION_VIEW, Uri.parse(uri))
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // إضافة البيانات
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun putExtra(intent: Intent, key: String, value: Any): Intent {
        when (value) {
            is String -> intent.putExtra(key, value)
            is Int -> intent.putExtra(key, value)
            is Long -> intent.putExtra(key, value)
            is Float -> intent.putExtra(key, value)
            is Double -> intent.putExtra(key, value)
            is Boolean -> intent.putExtra(key, value)
            is Char -> intent.putExtra(key, value)
            is CharSequence -> intent.putExtra(key, value)
            is IntArray -> intent.putExtra(key, value)
            is LongArray -> intent.putExtra(key, value)
            is FloatArray -> intent.putExtra(key, value)
            is DoubleArray -> intent.putExtra(key, value)
            is BooleanArray -> intent.putExtra(key, value)
            is CharArray -> intent.putExtra(key, value)
            is Bundle -> intent.putExtra(key, value)
            is java.io.Serializable -> intent.putExtra(key, value)
            is android.os.Parcelable -> intent.putExtra(key, value)
        }
        return intent
    }
    
    @JvmStatic
    fun putExtras(intent: Intent, extras: Map<String, Any>): Intent {
        extras.forEach { (key, value) -> putExtra(intent, key, value) }
        return intent
    }
    
    @JvmStatic
    fun setData(intent: Intent, uri: String): Intent {
        intent.data = Uri.parse(uri)
        return intent
    }
    
    @JvmStatic
    fun setType(intent: Intent, type: String): Intent {
        intent.type = type
        return intent
    }
    
    @JvmStatic
    fun setDataAndType(intent: Intent, uri: String, type: String): Intent {
        intent.setDataAndType(Uri.parse(uri), type)
        return intent
    }
    
    @JvmStatic
    fun addFlags(intent: Intent, vararg flags: Int): Intent {
        flags.forEach { intent.addFlags(it) }
        return intent
    }
    
    @JvmStatic
    fun addCategory(intent: Intent, category: String): Intent {
        intent.addCategory(category)
        return intent
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // استخراج البيانات
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getStringExtra(intent: Intent, key: String, default: String = ""): String {
        return intent.getStringExtra(key) ?: default
    }
    
    @JvmStatic
    fun getIntExtra(intent: Intent, key: String, default: Int = 0): Int {
        return intent.getIntExtra(key, default)
    }
    
    @JvmStatic
    fun getLongExtra(intent: Intent, key: String, default: Long = 0L): Long {
        return intent.getLongExtra(key, default)
    }
    
    @JvmStatic
    fun getBooleanExtra(intent: Intent, key: String, default: Boolean = false): Boolean {
        return intent.getBooleanExtra(key, default)
    }
    
    @JvmStatic
    fun getDoubleExtra(intent: Intent, key: String, default: Double = 0.0): Double {
        return intent.getDoubleExtra(key, default)
    }
    
    @JvmStatic
    fun hasExtra(intent: Intent, key: String): Boolean {
        return intent.hasExtra(key)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // فتح التطبيقات الخارجية
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun openUrl(context: Context, url: String): Boolean {
        return try {
            val intent = Intent(Intent.ACTION_VIEW, Uri.parse(url))
            context.startActivity(intent)
            true
        } catch (e: ActivityNotFoundException) {
            false
        }
    }
    
    @JvmStatic
    fun openDialer(context: Context, phoneNumber: String): Boolean {
        return try {
            val intent = Intent(Intent.ACTION_DIAL, Uri.parse("tel:$phoneNumber"))
            context.startActivity(intent)
            true
        } catch (e: ActivityNotFoundException) {
            false
        }
    }
    
    @JvmStatic
    fun makeCall(context: Context, phoneNumber: String): Boolean {
        return try {
            val intent = Intent(Intent.ACTION_CALL, Uri.parse("tel:$phoneNumber"))
            context.startActivity(intent)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun sendSms(context: Context, phoneNumber: String, message: String = ""): Boolean {
        return try {
            val intent = Intent(Intent.ACTION_SENDTO, Uri.parse("smsto:$phoneNumber"))
            intent.putExtra("sms_body", message)
            context.startActivity(intent)
            true
        } catch (e: ActivityNotFoundException) {
            false
        }
    }
    
    @JvmStatic
    fun sendEmail(
        context: Context,
        to: Array<String>,
        subject: String = "",
        body: String = "",
        cc: Array<String>? = null,
        bcc: Array<String>? = null
    ): Boolean {
        return try {
            val intent = Intent(Intent.ACTION_SENDTO).apply {
                data = Uri.parse("mailto:")
                putExtra(Intent.EXTRA_EMAIL, to)
                putExtra(Intent.EXTRA_SUBJECT, subject)
                putExtra(Intent.EXTRA_TEXT, body)
                cc?.let { putExtra(Intent.EXTRA_CC, it) }
                bcc?.let { putExtra(Intent.EXTRA_BCC, it) }
            }
            context.startActivity(intent)
            true
        } catch (e: ActivityNotFoundException) {
            false
        }
    }
    
    @JvmStatic
    fun openMaps(context: Context, latitude: Double, longitude: Double, label: String = ""): Boolean {
        return try {
            val uri = if (label.isNotEmpty()) {
                Uri.parse("geo:$latitude,$longitude?q=$latitude,$longitude($label)")
            } else {
                Uri.parse("geo:$latitude,$longitude")
            }
            val intent = Intent(Intent.ACTION_VIEW, uri)
            context.startActivity(intent)
            true
        } catch (e: ActivityNotFoundException) {
            false
        }
    }
    
    @JvmStatic
    fun searchMaps(context: Context, query: String): Boolean {
        return try {
            val uri = Uri.parse("geo:0,0?q=${Uri.encode(query)}")
            val intent = Intent(Intent.ACTION_VIEW, uri)
            context.startActivity(intent)
            true
        } catch (e: ActivityNotFoundException) {
            false
        }
    }
    
    @JvmStatic
    fun navigateTo(context: Context, latitude: Double, longitude: Double): Boolean {
        return try {
            val uri = Uri.parse("google.navigation:q=$latitude,$longitude")
            val intent = Intent(Intent.ACTION_VIEW, uri)
            intent.setPackage("com.google.android.apps.maps")
            context.startActivity(intent)
            true
        } catch (e: ActivityNotFoundException) {
            // Fall back to web maps
            openUrl(context, "https://www.google.com/maps/dir/?api=1&destination=$latitude,$longitude")
        }
    }
    
    @JvmStatic
    fun openPlayStore(context: Context, packageName: String = context.packageName): Boolean {
        return try {
            val intent = Intent(Intent.ACTION_VIEW, Uri.parse("market://details?id=$packageName"))
            context.startActivity(intent)
            true
        } catch (e: ActivityNotFoundException) {
            openUrl(context, "https://play.google.com/store/apps/details?id=$packageName")
        }
    }
    
    @JvmStatic
    fun openYouTube(context: Context, videoId: String): Boolean {
        return try {
            val intent = Intent(Intent.ACTION_VIEW, Uri.parse("vnd.youtube:$videoId"))
            context.startActivity(intent)
            true
        } catch (e: ActivityNotFoundException) {
            openUrl(context, "https://www.youtube.com/watch?v=$videoId")
        }
    }
    
    @JvmStatic
    fun openWhatsApp(context: Context, phoneNumber: String, message: String = ""): Boolean {
        return try {
            val url = "https://api.whatsapp.com/send?phone=$phoneNumber&text=${Uri.encode(message)}"
            val intent = Intent(Intent.ACTION_VIEW, Uri.parse(url))
            context.startActivity(intent)
            true
        } catch (e: ActivityNotFoundException) {
            false
        }
    }
    
    @JvmStatic
    fun openTelegram(context: Context, username: String): Boolean {
        return try {
            val intent = Intent(Intent.ACTION_VIEW, Uri.parse("tg://resolve?domain=$username"))
            context.startActivity(intent)
            true
        } catch (e: ActivityNotFoundException) {
            openUrl(context, "https://t.me/$username")
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // المشاركة
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun shareText(context: Context, text: String, title: String = "مشاركة"): Boolean {
        return try {
            val intent = Intent(Intent.ACTION_SEND).apply {
                type = "text/plain"
                putExtra(Intent.EXTRA_TEXT, text)
            }
            context.startActivity(Intent.createChooser(intent, title))
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun shareFile(context: Context, uri: Uri, mimeType: String, title: String = "مشاركة"): Boolean {
        return try {
            val intent = Intent(Intent.ACTION_SEND).apply {
                type = mimeType
                putExtra(Intent.EXTRA_STREAM, uri)
                addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
            }
            context.startActivity(Intent.createChooser(intent, title))
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun shareImage(context: Context, imageUri: Uri, title: String = "مشاركة صورة"): Boolean {
        return shareFile(context, imageUri, "image/*", title)
    }
    
    @JvmStatic
    fun shareMultipleFiles(context: Context, uris: List<Uri>, mimeType: String = "*/*", title: String = "مشاركة"): Boolean {
        return try {
            val intent = Intent(Intent.ACTION_SEND_MULTIPLE).apply {
                type = mimeType
                putParcelableArrayListExtra(Intent.EXTRA_STREAM, ArrayList(uris))
                addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
            }
            context.startActivity(Intent.createChooser(intent, title))
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // الإعدادات
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun openSettings(context: Context): Boolean {
        return try {
            context.startActivity(Intent(Settings.ACTION_SETTINGS))
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun openAppSettings(context: Context): Boolean {
        return try {
            val intent = Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS).apply {
                data = Uri.fromParts("package", context.packageName, null)
            }
            context.startActivity(intent)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun openWifiSettings(context: Context): Boolean {
        return try {
            context.startActivity(Intent(Settings.ACTION_WIFI_SETTINGS))
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun openBluetoothSettings(context: Context): Boolean {
        return try {
            context.startActivity(Intent(Settings.ACTION_BLUETOOTH_SETTINGS))
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun openLocationSettings(context: Context): Boolean {
        return try {
            context.startActivity(Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS))
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun openNotificationSettings(context: Context): Boolean {
        return try {
            val intent = Intent(Settings.ACTION_APP_NOTIFICATION_SETTINGS).apply {
                putExtra(Settings.EXTRA_APP_PACKAGE, context.packageName)
            }
            context.startActivity(intent)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // التقاط الوسائط
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun createCameraIntent(): Intent {
        return Intent(MediaStore.ACTION_IMAGE_CAPTURE)
    }
    
    @JvmStatic
    fun createVideoCaptureIntent(): Intent {
        return Intent(MediaStore.ACTION_VIDEO_CAPTURE)
    }
    
    @JvmStatic
    fun createGalleryPickIntent(allowMultiple: Boolean = false): Intent {
        return Intent(Intent.ACTION_PICK, MediaStore.Images.Media.EXTERNAL_CONTENT_URI).apply {
            if (allowMultiple) {
                putExtra(Intent.EXTRA_ALLOW_MULTIPLE, true)
            }
        }
    }
    
    @JvmStatic
    fun createFilePickerIntent(mimeType: String = "*/*"): Intent {
        return Intent(Intent.ACTION_GET_CONTENT).apply {
            type = mimeType
            addCategory(Intent.CATEGORY_OPENABLE)
        }
    }
    
    @JvmStatic
    fun createDocumentPickerIntent(vararg mimeTypes: String): Intent {
        return Intent(Intent.ACTION_OPEN_DOCUMENT).apply {
            addCategory(Intent.CATEGORY_OPENABLE)
            type = if (mimeTypes.size == 1) mimeTypes[0] else "*/*"
            if (mimeTypes.size > 1) {
                putExtra(Intent.EXTRA_MIME_TYPES, mimeTypes)
            }
        }
    }
    
    @JvmStatic
    fun createContactPickerIntent(): Intent {
        return Intent(Intent.ACTION_PICK, ContactsContract.Contacts.CONTENT_URI)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Flags المفيدة
    // ═══════════════════════════════════════════════════════════════════════════════
    
    object Flags {
        val CLEAR_TOP = Intent.FLAG_ACTIVITY_CLEAR_TOP
        val CLEAR_TASK = Intent.FLAG_ACTIVITY_CLEAR_TASK
        val NEW_TASK = Intent.FLAG_ACTIVITY_NEW_TASK
        val SINGLE_TOP = Intent.FLAG_ACTIVITY_SINGLE_TOP
        val NO_HISTORY = Intent.FLAG_ACTIVITY_NO_HISTORY
        val EXCLUDE_FROM_RECENTS = Intent.FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS
        val MULTIPLE_TASK = Intent.FLAG_ACTIVITY_MULTIPLE_TASK
        val REORDER_TO_FRONT = Intent.FLAG_ACTIVITY_REORDER_TO_FRONT
    }
}
