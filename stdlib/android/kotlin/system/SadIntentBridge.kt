// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadIntentBridge.kt
// الوصف: جسر Kotlin للـ Intents
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.app.Activity
import android.content.ActivityNotFoundException
import android.content.Context
import android.content.Intent
import android.net.Uri
import android.provider.ContactsContract
import android.provider.MediaStore
import android.provider.Settings

/**
 * جسر الـ Intents — SadIntentBridge
 */
object SadIntentBridge {
    
    private var activity: Activity? = null
    private var context: Context? = null
    
    @JvmStatic
    fun initialize(act: Activity) {
        activity = act
        context = act.applicationContext
    }
    
    // الروابط والويب
    @JvmStatic
    fun openUrl(url: String): Boolean {
        val act = activity ?: return false
        return try {
            val intent = Intent(Intent.ACTION_VIEW, Uri.parse(url))
            act.startActivity(intent)
            true
        } catch (e: ActivityNotFoundException) {
            false
        }
    }
    
    @JvmStatic
    fun openBrowser(url: String): Boolean = openUrl(url)
    
    // الاتصال والرسائل
    @JvmStatic
    fun dial(phoneNumber: String): Boolean {
        val act = activity ?: return false
        return try {
            val intent = Intent(Intent.ACTION_DIAL, Uri.parse("tel:$phoneNumber"))
            act.startActivity(intent)
            true
        } catch (e: ActivityNotFoundException) {
            false
        }
    }
    
    @JvmStatic
    fun call(phoneNumber: String): Boolean {
        val act = activity ?: return false
        return try {
            val intent = Intent(Intent.ACTION_CALL, Uri.parse("tel:$phoneNumber"))
            act.startActivity(intent)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun sendSms(phoneNumber: String, message: String = ""): Boolean {
        val act = activity ?: return false
        return try {
            val intent = Intent(Intent.ACTION_SENDTO, Uri.parse("smsto:$phoneNumber")).apply {
                putExtra("sms_body", message)
            }
            act.startActivity(intent)
            true
        } catch (e: ActivityNotFoundException) {
            false
        }
    }
    
    @JvmStatic
    fun sendEmail(
        to: String,
        subject: String = "",
        body: String = ""
    ): Boolean {
        val act = activity ?: return false
        return try {
            val intent = Intent(Intent.ACTION_SENDTO).apply {
                data = Uri.parse("mailto:$to")
                putExtra(Intent.EXTRA_SUBJECT, subject)
                putExtra(Intent.EXTRA_TEXT, body)
            }
            act.startActivity(intent)
            true
        } catch (e: ActivityNotFoundException) {
            false
        }
    }
    
    // الخرائط
    @JvmStatic
    fun openMap(latitude: Double, longitude: Double, label: String = ""): Boolean {
        val act = activity ?: return false
        val uri = if (label.isNotEmpty()) {
            Uri.parse("geo:$latitude,$longitude?q=$latitude,$longitude($label)")
        } else {
            Uri.parse("geo:$latitude,$longitude")
        }
        
        return try {
            val intent = Intent(Intent.ACTION_VIEW, uri)
            act.startActivity(intent)
            true
        } catch (e: ActivityNotFoundException) {
            false
        }
    }
    
    @JvmStatic
    fun openNavigation(latitude: Double, longitude: Double): Boolean {
        val act = activity ?: return false
        val uri = Uri.parse("google.navigation:q=$latitude,$longitude")
        
        return try {
            val intent = Intent(Intent.ACTION_VIEW, uri).apply {
                setPackage("com.google.android.apps.maps")
            }
            act.startActivity(intent)
            true
        } catch (e: ActivityNotFoundException) {
            openUrl("https://www.google.com/maps/dir/?api=1&destination=$latitude,$longitude")
        }
    }
    
    @JvmStatic
    fun searchMap(query: String): Boolean {
        val act = activity ?: return false
        return try {
            val intent = Intent(Intent.ACTION_VIEW, Uri.parse("geo:0,0?q=$query"))
            act.startActivity(intent)
            true
        } catch (e: ActivityNotFoundException) {
            false
        }
    }
    
    // التطبيقات
    @JvmStatic
    fun openPlayStore(packageName: String? = null): Boolean {
        val act = activity ?: return false
        val pkg = packageName ?: act.packageName
        
        return try {
            val intent = Intent(Intent.ACTION_VIEW, Uri.parse("market://details?id=$pkg"))
            act.startActivity(intent)
            true
        } catch (e: ActivityNotFoundException) {
            openUrl("https://play.google.com/store/apps/details?id=$pkg")
        }
    }
    
    @JvmStatic
    fun openApp(packageName: String): Boolean {
        val act = activity ?: return false
        val launchIntent = act.packageManager.getLaunchIntentForPackage(packageName) ?: return false
        
        return try {
            act.startActivity(launchIntent)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun isAppInstalled(packageName: String): Boolean {
        val act = activity ?: return false
        return try {
            act.packageManager.getPackageInfo(packageName, 0)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // الإعدادات
    @JvmStatic
    fun openSettings(): Boolean {
        val act = activity ?: return false
        return try {
            val intent = Intent(Settings.ACTION_SETTINGS)
            act.startActivity(intent)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun openAppSettings(): Boolean {
        val act = activity ?: return false
        return try {
            val intent = Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS).apply {
                data = Uri.fromParts("package", act.packageName, null)
            }
            act.startActivity(intent)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun openWifiSettings(): Boolean {
        val act = activity ?: return false
        return try {
            act.startActivity(Intent(Settings.ACTION_WIFI_SETTINGS))
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun openBluetoothSettings(): Boolean {
        val act = activity ?: return false
        return try {
            act.startActivity(Intent(Settings.ACTION_BLUETOOTH_SETTINGS))
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun openLocationSettings(): Boolean {
        val act = activity ?: return false
        return try {
            act.startActivity(Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS))
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // جهات الاتصال
    @JvmStatic
    fun pickContact(requestCode: Int) {
        val act = activity ?: return
        val intent = Intent(Intent.ACTION_PICK, ContactsContract.Contacts.CONTENT_URI)
        act.startActivityForResult(intent, requestCode)
    }
    
    @JvmStatic
    fun addContact(name: String, phone: String = "", email: String = ""): Boolean {
        val act = activity ?: return false
        return try {
            val intent = Intent(Intent.ACTION_INSERT).apply {
                type = ContactsContract.Contacts.CONTENT_TYPE
                putExtra(ContactsContract.Intents.Insert.NAME, name)
                if (phone.isNotEmpty()) {
                    putExtra(ContactsContract.Intents.Insert.PHONE, phone)
                }
                if (email.isNotEmpty()) {
                    putExtra(ContactsContract.Intents.Insert.EMAIL, email)
                }
            }
            act.startActivity(intent)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // التقويم
    @JvmStatic
    fun addCalendarEvent(
        title: String,
        description: String = "",
        beginTimeMillis: Long,
        endTimeMillis: Long,
        location: String = ""
    ): Boolean {
        val act = activity ?: return false
        return try {
            val intent = Intent(Intent.ACTION_INSERT).apply {
                data = android.provider.CalendarContract.Events.CONTENT_URI
                putExtra(android.provider.CalendarContract.Events.TITLE, title)
                putExtra(android.provider.CalendarContract.Events.DESCRIPTION, description)
                putExtra(android.provider.CalendarContract.Events.EVENT_LOCATION, location)
                putExtra(android.provider.CalendarContract.EXTRA_EVENT_BEGIN_TIME, beginTimeMillis)
                putExtra(android.provider.CalendarContract.EXTRA_EVENT_END_TIME, endTimeMillis)
            }
            act.startActivity(intent)
            true
        } catch (e: Exception) {
            false
        }
    }
}
