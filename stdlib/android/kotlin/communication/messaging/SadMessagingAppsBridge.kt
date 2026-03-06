package sad.android.communication.messaging

import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.net.Uri

/**
 * جسر تطبيقات المراسلة الفورية
 * Instant Messaging Bridge - واتساب، تيليجرام، إلخ
 * 
 * @author فريق لغة ص
 */
object SadMessagingAppsBridge {
    
    /**
     * نتيجة الإرسال / Send result
     */
    data class MessagingResult(
        val success: Boolean,
        val app: String,
        val error: String? = null
    )
    
    // ═══════════════════════════════════════════════════════════════
    //                          واتساب
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إرسال رسالة واتساب / Send WhatsApp message
     */
    fun sendWhatsApp(context: Context, phone: String, message: String): MessagingResult {
        return try {
            val formattedPhone = phone.replace("+", "").replace(" ", "")
            val uri = Uri.parse("https://wa.me/$formattedPhone?text=${Uri.encode(message)}")
            
            val intent = Intent(Intent.ACTION_VIEW, uri).apply {
                setPackage("com.whatsapp")
                flags = Intent.FLAG_ACTIVITY_NEW_TASK
            }
            
            if (isAppInstalled(context, "com.whatsapp")) {
                context.startActivity(intent)
                MessagingResult(true, "WhatsApp")
            } else {
                // جرب WhatsApp Business
                intent.setPackage("com.whatsapp.w4b")
                if (isAppInstalled(context, "com.whatsapp.w4b")) {
                    context.startActivity(intent)
                    MessagingResult(true, "WhatsApp Business")
                } else {
                    MessagingResult(false, "WhatsApp", "التطبيق غير مثبت")
                }
            }
        } catch (e: Exception) {
            MessagingResult(false, "WhatsApp", e.message)
        }
    }
    
    /**
     * فتح محادثة واتساب / Open WhatsApp chat
     */
    fun openWhatsAppChat(context: Context, phone: String): MessagingResult {
        return sendWhatsApp(context, phone, "")
    }
    
    /**
     * مشاركة عبر واتساب / Share via WhatsApp
     */
    fun shareViaWhatsApp(context: Context, text: String): MessagingResult {
        return try {
            val intent = Intent(Intent.ACTION_SEND).apply {
                type = "text/plain"
                setPackage("com.whatsapp")
                putExtra(Intent.EXTRA_TEXT, text)
                flags = Intent.FLAG_ACTIVITY_NEW_TASK
            }
            context.startActivity(intent)
            MessagingResult(true, "WhatsApp")
        } catch (e: Exception) {
            MessagingResult(false, "WhatsApp", e.message)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                          تيليجرام
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إرسال رسالة تيليجرام / Send Telegram message
     */
    fun sendTelegram(context: Context, username: String, message: String? = null): MessagingResult {
        return try {
            val uri = if (message != null) {
                Uri.parse("https://t.me/$username?text=${Uri.encode(message)}")
            } else {
                Uri.parse("https://t.me/$username")
            }
            
            val intent = Intent(Intent.ACTION_VIEW, uri).apply {
                setPackage("org.telegram.messenger")
                flags = Intent.FLAG_ACTIVITY_NEW_TASK
            }
            
            if (isAppInstalled(context, "org.telegram.messenger")) {
                context.startActivity(intent)
                MessagingResult(true, "Telegram")
            } else {
                // جرب Telegram X
                intent.setPackage("org.thunderdog.challegram")
                if (isAppInstalled(context, "org.thunderdog.challegram")) {
                    context.startActivity(intent)
                    MessagingResult(true, "Telegram X")
                } else {
                    MessagingResult(false, "Telegram", "التطبيق غير مثبت")
                }
            }
        } catch (e: Exception) {
            MessagingResult(false, "Telegram", e.message)
        }
    }
    
    /**
     * مشاركة عبر تيليجرام / Share via Telegram
     */
    fun shareViaTelegram(context: Context, text: String): MessagingResult {
        return try {
            val intent = Intent(Intent.ACTION_SEND).apply {
                type = "text/plain"
                setPackage("org.telegram.messenger")
                putExtra(Intent.EXTRA_TEXT, text)
                flags = Intent.FLAG_ACTIVITY_NEW_TASK
            }
            context.startActivity(intent)
            MessagingResult(true, "Telegram")
        } catch (e: Exception) {
            MessagingResult(false, "Telegram", e.message)
        }
    }
    
    /**
     * الانضمام لقناة/مجموعة تيليجرام / Join Telegram channel/group
     */
    fun joinTelegramChannel(context: Context, channelUsername: String): MessagingResult {
        return try {
            val uri = Uri.parse("https://t.me/$channelUsername")
            val intent = Intent(Intent.ACTION_VIEW, uri).apply {
                flags = Intent.FLAG_ACTIVITY_NEW_TASK
            }
            context.startActivity(intent)
            MessagingResult(true, "Telegram")
        } catch (e: Exception) {
            MessagingResult(false, "Telegram", e.message)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                          سيجنال
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * فتح سيجنال / Open Signal
     */
    fun openSignal(context: Context): MessagingResult {
        return openApp(context, "org.thoughtcrime.securesms", "Signal")
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                          فيسبوك ماسنجر
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * فتح ماسنجر / Open Messenger
     */
    fun openMessenger(context: Context, userId: String? = null): MessagingResult {
        return try {
            val uri = if (userId != null) {
                Uri.parse("fb-messenger://user/$userId")
            } else {
                Uri.parse("fb-messenger://")
            }
            
            val intent = Intent(Intent.ACTION_VIEW, uri).apply {
                setPackage("com.facebook.orca")
                flags = Intent.FLAG_ACTIVITY_NEW_TASK
            }
            
            if (isAppInstalled(context, "com.facebook.orca")) {
                context.startActivity(intent)
                MessagingResult(true, "Messenger")
            } else {
                MessagingResult(false, "Messenger", "التطبيق غير مثبت")
            }
        } catch (e: Exception) {
            MessagingResult(false, "Messenger", e.message)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                          فايبر
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إرسال رسالة فايبر / Send Viber message
     */
    fun sendViber(context: Context, phone: String): MessagingResult {
        return try {
            val uri = Uri.parse("viber://chat?number=$phone")
            val intent = Intent(Intent.ACTION_VIEW, uri).apply {
                flags = Intent.FLAG_ACTIVITY_NEW_TASK
            }
            
            if (isAppInstalled(context, "com.viber.voip")) {
                context.startActivity(intent)
                MessagingResult(true, "Viber")
            } else {
                MessagingResult(false, "Viber", "التطبيق غير مثبت")
            }
        } catch (e: Exception) {
            MessagingResult(false, "Viber", e.message)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                          سناب شات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * فتح بروفايل سناب شات / Open Snapchat profile
     */
    fun openSnapchatProfile(context: Context, username: String): MessagingResult {
        return try {
            val uri = Uri.parse("https://www.snapchat.com/add/$username")
            val intent = Intent(Intent.ACTION_VIEW, uri).apply {
                setPackage("com.snapchat.android")
                flags = Intent.FLAG_ACTIVITY_NEW_TASK
            }
            
            if (isAppInstalled(context, "com.snapchat.android")) {
                context.startActivity(intent)
                MessagingResult(true, "Snapchat")
            } else {
                MessagingResult(false, "Snapchat", "التطبيق غير مثبت")
            }
        } catch (e: Exception) {
            MessagingResult(false, "Snapchat", e.message)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                          ديسكورد
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * فتح سيرفر ديسكورد / Open Discord server
     */
    fun openDiscordServer(context: Context, inviteCode: String): MessagingResult {
        return try {
            val uri = Uri.parse("https://discord.gg/$inviteCode")
            val intent = Intent(Intent.ACTION_VIEW, uri).apply {
                flags = Intent.FLAG_ACTIVITY_NEW_TASK
            }
            context.startActivity(intent)
            MessagingResult(true, "Discord")
        } catch (e: Exception) {
            MessagingResult(false, "Discord", e.message)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                          مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * فتح تطبيق / Open app
     */
    private fun openApp(context: Context, packageName: String, appName: String): MessagingResult {
        return try {
            val intent = context.packageManager.getLaunchIntentForPackage(packageName)
            if (intent != null) {
                intent.flags = Intent.FLAG_ACTIVITY_NEW_TASK
                context.startActivity(intent)
                MessagingResult(true, appName)
            } else {
                MessagingResult(false, appName, "التطبيق غير مثبت")
            }
        } catch (e: Exception) {
            MessagingResult(false, appName, e.message)
        }
    }
    
    /**
     * التحقق من تثبيت التطبيق / Check if app is installed
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
     * جلب التطبيقات المتاحة / Get available apps
     */
    fun getAvailableApps(context: Context): List<String> {
        val apps = mutableListOf<String>()
        
        mapOf(
            "com.whatsapp" to "واتساب",
            "com.whatsapp.w4b" to "واتساب أعمال",
            "org.telegram.messenger" to "تيليجرام",
            "org.thunderdog.challegram" to "تيليجرام X",
            "org.thoughtcrime.securesms" to "سيجنال",
            "com.facebook.orca" to "ماسنجر",
            "com.viber.voip" to "فايبر",
            "com.snapchat.android" to "سناب شات",
            "com.discord" to "ديسكورد"
        ).forEach { (pkg, name) ->
            if (isAppInstalled(context, pkg)) {
                apps.add(name)
            }
        }
        
        return apps
    }
}
