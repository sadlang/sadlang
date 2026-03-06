package sad.android.widgets.action

import android.app.PendingIntent
import android.appwidget.AppWidgetManager
import android.content.ComponentName
import android.content.Context
import android.content.Intent
import android.net.Uri
import android.os.Build
import android.os.Handler
import android.os.Looper
import android.widget.RemoteViews
import java.util.concurrent.ConcurrentHashMap

/**
 * جسر إجراءات الويدجت - أحداث النقر والتفاعل
 * Widget Actions Bridge - Click events and interactions
 * 
 * @author فريق لغة ص
 */
object SadWidgetActionBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private val actionHandlers = ConcurrentHashMap<String, ActionHandler>()
    private var pendingIntentRequestCode = 0
    
    /**
     * نوع الإجراء / Action type
     */
    enum class ActionType {
        OPEN_ACTIVITY,
        BROADCAST,
        SERVICE,
        URL,
        DIAL,
        SEND_SMS,
        SEND_EMAIL,
        REFRESH,
        CUSTOM
    }
    
    /**
     * تعريف الإجراء / Action definition
     */
    data class ActionDefinition(
        val type: ActionType,
        val target: String,
        val extras: Map<String, Any> = emptyMap(),
        val flags: Int = 0
    )
    
    /**
     * معالج الإجراء / Action handler
     */
    interface ActionHandler {
        fun onAction(actionKey: String, extras: Map<String, Any>)
    }
    
    /**
     * مستمع اللمس / Touch listener
     */
    interface WidgetTouchListener {
        fun onClick(widgetId: Int, viewId: Int, actionKey: String)
    }
    
    private val touchListeners = mutableListOf<WidgetTouchListener>()
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    /**
     * تسجيل معالج / Register handler
     */
    fun registerActionHandler(actionKey: String, handler: ActionHandler): Boolean {
        actionHandlers[actionKey] = handler
        return true
    }
    
    /**
     * إلغاء تسجيل معالج / Unregister handler
     */
    fun unregisterActionHandler(actionKey: String): Boolean {
        return actionHandlers.remove(actionKey) != null
    }
    
    /**
     * إضافة مستمع لمس / Add touch listener
     */
    fun addTouchListener(listener: WidgetTouchListener) {
        if (!touchListeners.contains(listener)) {
            touchListeners.add(listener)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         ربط الإجراءات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * ربط إجراء نقر / Bind click action
     */
    fun bindClickAction(
        views: RemoteViews,
        viewId: Int,
        action: ActionDefinition
    ): Boolean {
        val ctx = context ?: return false
        
        val pendingIntent = createPendingIntent(ctx, action) ?: return false
        views.setOnClickPendingIntent(viewId, pendingIntent)
        return true
    }
    
    /**
     * ربط فتح نشاط / Bind open activity
     */
    fun bindOpenActivity(
        views: RemoteViews,
        viewId: Int,
        activityClass: Class<*>,
        extras: Map<String, Any> = emptyMap()
    ): Boolean {
        val ctx = context ?: return false
        
        val intent = Intent(ctx, activityClass).apply {
            flags = Intent.FLAG_ACTIVITY_NEW_TASK
            extras.forEach { (key, value) ->
                when (value) {
                    is String -> putExtra(key, value)
                    is Int -> putExtra(key, value)
                    is Long -> putExtra(key, value)
                    is Boolean -> putExtra(key, value)
                    is Float -> putExtra(key, value)
                    is Double -> putExtra(key, value)
                }
            }
        }
        
        val pendingIntent = PendingIntent.getActivity(
            ctx,
            getNextRequestCode(),
            intent,
            getPendingIntentFlags()
        )
        
        views.setOnClickPendingIntent(viewId, pendingIntent)
        return true
    }
    
    /**
     * ربط بث / Bind broadcast
     */
    fun bindBroadcast(
        views: RemoteViews,
        viewId: Int,
        action: String,
        extras: Map<String, Any> = emptyMap()
    ): Boolean {
        val ctx = context ?: return false
        
        val intent = Intent(action).apply {
            extras.forEach { (key, value) ->
                when (value) {
                    is String -> putExtra(key, value)
                    is Int -> putExtra(key, value)
                    is Long -> putExtra(key, value)
                    is Boolean -> putExtra(key, value)
                }
            }
        }
        
        val pendingIntent = PendingIntent.getBroadcast(
            ctx,
            getNextRequestCode(),
            intent,
            getPendingIntentFlags()
        )
        
        views.setOnClickPendingIntent(viewId, pendingIntent)
        return true
    }
    
    /**
     * ربط فتح رابط / Bind open URL
     */
    fun bindOpenUrl(views: RemoteViews, viewId: Int, url: String): Boolean {
        val ctx = context ?: return false
        
        val intent = Intent(Intent.ACTION_VIEW, Uri.parse(url))
        val pendingIntent = PendingIntent.getActivity(
            ctx,
            getNextRequestCode(),
            intent,
            getPendingIntentFlags()
        )
        
        views.setOnClickPendingIntent(viewId, pendingIntent)
        return true
    }
    
    /**
     * ربط اتصال / Bind dial
     */
    fun bindDial(views: RemoteViews, viewId: Int, phoneNumber: String): Boolean {
        val ctx = context ?: return false
        
        val intent = Intent(Intent.ACTION_DIAL, Uri.parse("tel:$phoneNumber"))
        val pendingIntent = PendingIntent.getActivity(
            ctx,
            getNextRequestCode(),
            intent,
            getPendingIntentFlags()
        )
        
        views.setOnClickPendingIntent(viewId, pendingIntent)
        return true
    }
    
    /**
     * ربط رسالة / Bind SMS
     */
    fun bindSendSms(views: RemoteViews, viewId: Int, phoneNumber: String, message: String = ""): Boolean {
        val ctx = context ?: return false
        
        val intent = Intent(Intent.ACTION_SENDTO, Uri.parse("smsto:$phoneNumber")).apply {
            putExtra("sms_body", message)
        }
        val pendingIntent = PendingIntent.getActivity(
            ctx,
            getNextRequestCode(),
            intent,
            getPendingIntentFlags()
        )
        
        views.setOnClickPendingIntent(viewId, pendingIntent)
        return true
    }
    
    /**
     * ربط بريد / Bind email
     */
    fun bindSendEmail(
        views: RemoteViews,
        viewId: Int,
        to: String,
        subject: String = "",
        body: String = ""
    ): Boolean {
        val ctx = context ?: return false
        
        val intent = Intent(Intent.ACTION_SENDTO, Uri.parse("mailto:$to")).apply {
            putExtra(Intent.EXTRA_SUBJECT, subject)
            putExtra(Intent.EXTRA_TEXT, body)
        }
        val pendingIntent = PendingIntent.getActivity(
            ctx,
            getNextRequestCode(),
            intent,
            getPendingIntentFlags()
        )
        
        views.setOnClickPendingIntent(viewId, pendingIntent)
        return true
    }
    
    /**
     * ربط تحديث الويدجت / Bind widget refresh
     */
    fun bindRefreshWidget(
        views: RemoteViews,
        viewId: Int,
        providerClass: Class<*>,
        widgetId: Int
    ): Boolean {
        val ctx = context ?: return false
        
        val intent = Intent(ctx, providerClass).apply {
            action = AppWidgetManager.ACTION_APPWIDGET_UPDATE
            putExtra(AppWidgetManager.EXTRA_APPWIDGET_IDS, intArrayOf(widgetId))
        }
        
        val pendingIntent = PendingIntent.getBroadcast(
            ctx,
            getNextRequestCode(),
            intent,
            getPendingIntentFlags()
        )
        
        views.setOnClickPendingIntent(viewId, pendingIntent)
        return true
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إنشاء PendingIntent
    // ═══════════════════════════════════════════════════════════════
    
    private fun createPendingIntent(ctx: Context, action: ActionDefinition): PendingIntent? {
        return when (action.type) {
            ActionType.OPEN_ACTIVITY -> {
                val intent = Intent(action.target).apply {
                    flags = Intent.FLAG_ACTIVITY_NEW_TASK or action.flags
                }
                PendingIntent.getActivity(ctx, getNextRequestCode(), intent, getPendingIntentFlags())
            }
            
            ActionType.BROADCAST -> {
                val intent = Intent(action.target)
                PendingIntent.getBroadcast(ctx, getNextRequestCode(), intent, getPendingIntentFlags())
            }
            
            ActionType.SERVICE -> {
                val intent = Intent(action.target)
                PendingIntent.getService(ctx, getNextRequestCode(), intent, getPendingIntentFlags())
            }
            
            ActionType.URL -> {
                val intent = Intent(Intent.ACTION_VIEW, Uri.parse(action.target))
                PendingIntent.getActivity(ctx, getNextRequestCode(), intent, getPendingIntentFlags())
            }
            
            ActionType.DIAL -> {
                val intent = Intent(Intent.ACTION_DIAL, Uri.parse("tel:${action.target}"))
                PendingIntent.getActivity(ctx, getNextRequestCode(), intent, getPendingIntentFlags())
            }
            
            ActionType.SEND_SMS -> {
                val intent = Intent(Intent.ACTION_SENDTO, Uri.parse("smsto:${action.target}"))
                PendingIntent.getActivity(ctx, getNextRequestCode(), intent, getPendingIntentFlags())
            }
            
            ActionType.SEND_EMAIL -> {
                val intent = Intent(Intent.ACTION_SENDTO, Uri.parse("mailto:${action.target}"))
                PendingIntent.getActivity(ctx, getNextRequestCode(), intent, getPendingIntentFlags())
            }
            
            ActionType.REFRESH, ActionType.CUSTOM -> null
        }
    }
    
    private fun getNextRequestCode(): Int {
        return ++pendingIntentRequestCode
    }
    
    private fun getPendingIntentFlags(): Int {
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_MUTABLE
        } else {
            PendingIntent.FLAG_UPDATE_CURRENT
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معالجة الإجراءات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * معالجة إجراء وارد / Handle incoming action
     */
    fun handleAction(actionKey: String, extras: Map<String, Any> = emptyMap()) {
        mainHandler.post {
            actionHandlers[actionKey]?.onAction(actionKey, extras)
        }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        actionHandlers.clear()
        touchListeners.clear()
        context = null
    }
}
