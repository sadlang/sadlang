package sad.android.widgets.shortcuts

import android.app.PendingIntent
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.content.pm.ShortcutInfo
import android.content.pm.ShortcutManager
import android.graphics.drawable.Icon
import android.net.Uri
import android.os.Build
import android.os.Handler
import android.os.Looper
import android.widget.RemoteViews
import java.util.concurrent.ConcurrentHashMap

/**
 * جسر اختصارات الويدجت - تشغيل سريع للتطبيقات والإجراءات
 * Widget Shortcuts Bridge - Quick launch for apps and actions
 * 
 * @author فريق لغة ص
 */
object SadWidgetShortcutsBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private val shortcuts = ConcurrentHashMap<String, ShortcutDefinition>()
    
    /**
     * تعريف الاختصار / Shortcut definition
     */
    data class ShortcutDefinition(
        val id: String,
        val label: String,
        val longLabel: String = label,
        val iconResId: Int = 0,
        val action: ShortcutAction,
        val rank: Int = 0,
        val enabled: Boolean = true
    )
    
    /**
     * إجراء الاختصار / Shortcut action
     */
    sealed class ShortcutAction {
        data class OpenApp(val packageName: String) : ShortcutAction()
        data class OpenActivity(val className: String, val extras: Map<String, Any> = emptyMap()) : ShortcutAction()
        data class OpenUrl(val url: String) : ShortcutAction()
        data class Dial(val phoneNumber: String) : ShortcutAction()
        data class SendSms(val phoneNumber: String, val message: String = "") : ShortcutAction()
        data class SendEmail(val email: String, val subject: String = "", val body: String = "") : ShortcutAction()
        data class OpenMap(val latitude: Double, val longitude: Double, val label: String = "") : ShortcutAction()
        data class OpenSettings(val settingsAction: String = android.provider.Settings.ACTION_SETTINGS) : ShortcutAction()
        data class Broadcast(val action: String, val extras: Map<String, Any> = emptyMap()) : ShortcutAction()
        data class Custom(val intent: Intent) : ShortcutAction()
    }
    
    /**
     * معلومات التطبيق / App info
     */
    data class AppInfo(
        val packageName: String,
        val appName: String,
        val icon: android.graphics.drawable.Drawable?
    )
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة الاختصارات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تسجيل اختصار / Register shortcut
     */
    fun registerShortcut(shortcut: ShortcutDefinition): Boolean {
        shortcuts[shortcut.id] = shortcut
        return true
    }
    
    /**
     * إلغاء تسجيل اختصار / Unregister shortcut
     */
    fun unregisterShortcut(id: String): Boolean {
        return shortcuts.remove(id) != null
    }
    
    /**
     * الحصول على اختصار / Get shortcut
     */
    fun getShortcut(id: String): ShortcutDefinition? {
        return shortcuts[id]
    }
    
    /**
     * جميع الاختصارات / All shortcuts
     */
    fun getAllShortcuts(): List<ShortcutDefinition> {
        return shortcuts.values.toList()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         تنفيذ الاختصار
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تنفيذ اختصار / Execute shortcut
     */
    fun executeShortcut(id: String): Boolean {
        val shortcut = shortcuts[id] ?: return false
        return executeAction(shortcut.action)
    }
    
    /**
     * تنفيذ إجراء / Execute action
     */
    fun executeAction(action: ShortcutAction): Boolean {
        val ctx = context ?: return false
        
        val intent = createIntent(action)?.apply {
            addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
        } ?: return false
        
        try {
            ctx.startActivity(intent)
            return true
        } catch (e: Exception) {
            return false
        }
    }
    
    /**
     * إنشاء Intent / Create Intent
     */
    fun createIntent(action: ShortcutAction): Intent? {
        val ctx = context ?: return null
        
        return when (action) {
            is ShortcutAction.OpenApp -> {
                ctx.packageManager.getLaunchIntentForPackage(action.packageName)
            }
            
            is ShortcutAction.OpenActivity -> {
                Intent().apply {
                    setClassName(ctx, action.className)
                    action.extras.forEach { (key, value) ->
                        when (value) {
                            is String -> putExtra(key, value)
                            is Int -> putExtra(key, value)
                            is Long -> putExtra(key, value)
                            is Boolean -> putExtra(key, value)
                        }
                    }
                }
            }
            
            is ShortcutAction.OpenUrl -> {
                Intent(Intent.ACTION_VIEW, Uri.parse(action.url))
            }
            
            is ShortcutAction.Dial -> {
                Intent(Intent.ACTION_DIAL, Uri.parse("tel:${action.phoneNumber}"))
            }
            
            is ShortcutAction.SendSms -> {
                Intent(Intent.ACTION_SENDTO, Uri.parse("smsto:${action.phoneNumber}")).apply {
                    putExtra("sms_body", action.message)
                }
            }
            
            is ShortcutAction.SendEmail -> {
                Intent(Intent.ACTION_SENDTO, Uri.parse("mailto:${action.email}")).apply {
                    putExtra(Intent.EXTRA_SUBJECT, action.subject)
                    putExtra(Intent.EXTRA_TEXT, action.body)
                }
            }
            
            is ShortcutAction.OpenMap -> {
                val uri = Uri.parse("geo:${action.latitude},${action.longitude}?q=${action.latitude},${action.longitude}(${action.label})")
                Intent(Intent.ACTION_VIEW, uri)
            }
            
            is ShortcutAction.OpenSettings -> {
                Intent(action.settingsAction)
            }
            
            is ShortcutAction.Broadcast -> {
                Intent(action.action).apply {
                    action.extras.forEach { (key, value) ->
                        when (value) {
                            is String -> putExtra(key, value)
                            is Int -> putExtra(key, value)
                            is Long -> putExtra(key, value)
                            is Boolean -> putExtra(key, value)
                        }
                    }
                }
            }
            
            is ShortcutAction.Custom -> action.intent
        }
    }
    
    /**
     * إنشاء PendingIntent / Create PendingIntent
     */
    fun createPendingIntent(action: ShortcutAction): PendingIntent? {
        val ctx = context ?: return null
        val intent = createIntent(action) ?: return null
        
        val flags = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_IMMUTABLE
        } else {
            PendingIntent.FLAG_UPDATE_CURRENT
        }
        
        return when (action) {
            is ShortcutAction.Broadcast -> PendingIntent.getBroadcast(ctx, 0, intent, flags)
            else -> PendingIntent.getActivity(ctx, 0, intent, flags)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         ربط بـ RemoteViews
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * ربط اختصار بـ view / Bind shortcut to view
     */
    fun bindShortcutToView(views: RemoteViews, viewId: Int, shortcutId: String): Boolean {
        val shortcut = shortcuts[shortcutId] ?: return false
        val pendingIntent = createPendingIntent(shortcut.action) ?: return false
        
        views.setOnClickPendingIntent(viewId, pendingIntent)
        return true
    }
    
    /**
     * ربط إجراء بـ view / Bind action to view
     */
    fun bindActionToView(views: RemoteViews, viewId: Int, action: ShortcutAction): Boolean {
        val pendingIntent = createPendingIntent(action) ?: return false
        views.setOnClickPendingIntent(viewId, pendingIntent)
        return true
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التطبيقات المثبتة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على التطبيقات المثبتة / Get installed apps
     */
    fun getInstalledApps(): List<AppInfo> {
        val ctx = context ?: return emptyList()
        val pm = ctx.packageManager
        
        val intent = Intent(Intent.ACTION_MAIN).apply {
            addCategory(Intent.CATEGORY_LAUNCHER)
        }
        
        val apps = pm.queryIntentActivities(intent, 0)
        
        return apps.map { resolveInfo ->
            AppInfo(
                packageName = resolveInfo.activityInfo.packageName,
                appName = resolveInfo.loadLabel(pm).toString(),
                icon = resolveInfo.loadIcon(pm)
            )
        }.sortedBy { it.appName }
    }
    
    /**
     * التحقق من تثبيت تطبيق / Check if app installed
     */
    fun isAppInstalled(packageName: String): Boolean {
        val ctx = context ?: return false
        return try {
            ctx.packageManager.getPackageInfo(packageName, 0)
            true
        } catch (e: PackageManager.NameNotFoundException) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         اختصارات شائعة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * اختصارات الإعدادات / Settings shortcuts
     */
    object SettingsShortcuts {
        val WIFI = ShortcutAction.OpenSettings(android.provider.Settings.ACTION_WIFI_SETTINGS)
        val BLUETOOTH = ShortcutAction.OpenSettings(android.provider.Settings.ACTION_BLUETOOTH_SETTINGS)
        val DISPLAY = ShortcutAction.OpenSettings(android.provider.Settings.ACTION_DISPLAY_SETTINGS)
        val SOUND = ShortcutAction.OpenSettings(android.provider.Settings.ACTION_SOUND_SETTINGS)
        val BATTERY = ShortcutAction.OpenSettings(android.provider.Settings.ACTION_BATTERY_SAVER_SETTINGS)
        val LOCATION = ShortcutAction.OpenSettings(android.provider.Settings.ACTION_LOCATION_SOURCE_SETTINGS)
        val AIRPLANE = ShortcutAction.OpenSettings(android.provider.Settings.ACTION_AIRPLANE_MODE_SETTINGS)
        val DATE_TIME = ShortcutAction.OpenSettings(android.provider.Settings.ACTION_DATE_SETTINGS)
        val APPS = ShortcutAction.OpenSettings(android.provider.Settings.ACTION_APPLICATION_SETTINGS)
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        shortcuts.clear()
        context = null
    }
}
