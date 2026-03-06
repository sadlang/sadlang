package sad.android.communication.shortcut

import android.content.Context
import android.content.Intent
import android.os.Build
import androidx.annotation.RequiresApi
import androidx.core.content.pm.ShortcutInfoCompat
import androidx.core.content.pm.ShortcutManagerCompat
import androidx.core.graphics.drawable.IconCompat

/**
 * جسر الاختصارات - إدارة اختصارات التطبيق
 * Shortcuts Bridge - App shortcuts management
 * 
 * @author فريق لغة ص
 */
object SadShortcutBridge {
    
    /**
     * نوع الاختصار / Shortcut type
     */
    enum class ShortcutType {
        STATIC,         // ثابت (معرّف في manifest)
        DYNAMIC,        // ديناميكي
        PINNED          // مثبت على الشاشة الرئيسية
    }
    
    /**
     * معلومات الاختصار / Shortcut info
     */
    data class ShortcutData(
        val id: String,
        val shortLabel: String,
        val longLabel: String? = null,
        val iconResId: Int? = null,
        val iconBitmap: android.graphics.Bitmap? = null,
        val targetActivity: Class<*>,
        val intentAction: String = Intent.ACTION_VIEW,
        val extras: Map<String, String> = emptyMap(),
        val rank: Int = 0,
        val enabled: Boolean = true,
        val categories: Set<String> = emptySet()
    )
    
    // ═══════════════════════════════════════════════════════════════
    //                         الاختصارات الديناميكية
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إضافة اختصار ديناميكي / Add dynamic shortcut
     */
    fun addDynamicShortcut(context: Context, shortcut: ShortcutData): Boolean {
        val shortcutInfo = buildShortcut(context, shortcut)
        return ShortcutManagerCompat.pushDynamicShortcut(context, shortcutInfo)
    }
    
    /**
     * إضافة اختصارات متعددة / Add multiple dynamic shortcuts
     */
    fun setDynamicShortcuts(context: Context, shortcuts: List<ShortcutData>): Boolean {
        val shortcutInfoList = shortcuts.map { buildShortcut(context, it) }
        return ShortcutManagerCompat.setDynamicShortcuts(context, shortcutInfoList)
    }
    
    /**
     * تحديث الاختصارات / Update shortcuts
     */
    fun updateShortcuts(context: Context, shortcuts: List<ShortcutData>): Boolean {
        val shortcutInfoList = shortcuts.map { buildShortcut(context, it) }
        return ShortcutManagerCompat.updateShortcuts(context, shortcutInfoList)
    }
    
    /**
     * إزالة اختصار ديناميكي / Remove dynamic shortcut
     */
    fun removeDynamicShortcut(context: Context, shortcutId: String) {
        ShortcutManagerCompat.removeDynamicShortcuts(context, listOf(shortcutId))
    }
    
    /**
     * إزالة جميع الاختصارات الديناميكية / Remove all dynamic shortcuts
     */
    fun removeAllDynamicShortcuts(context: Context) {
        ShortcutManagerCompat.removeAllDynamicShortcuts(context)
    }
    
    /**
     * جلب الاختصارات الديناميكية / Get dynamic shortcuts
     */
    fun getDynamicShortcuts(context: Context): List<String> {
        return ShortcutManagerCompat.getDynamicShortcuts(context).map { it.id }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الاختصارات المثبتة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * هل يمكن تثبيت الاختصارات؟ / Can pin shortcuts?
     */
    fun canPinShortcuts(context: Context): Boolean {
        return ShortcutManagerCompat.isRequestPinShortcutSupported(context)
    }
    
    /**
     * طلب تثبيت اختصار / Request pinned shortcut
     */
    fun requestPinnedShortcut(
        context: Context,
        shortcut: ShortcutData,
        callback: ((Boolean) -> Unit)? = null
    ): Boolean {
        if (!canPinShortcuts(context)) return false
        
        val shortcutInfo = buildShortcut(context, shortcut)
        
        val callbackIntent = if (callback != null && Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            // يمكن إنشاء PendingIntent للاستجابة
            null // بسيط: بدون callback intent
        } else {
            null
        }
        
        return ShortcutManagerCompat.requestPinShortcut(context, shortcutInfo, callbackIntent)
    }
    
    /**
     * جلب الاختصارات المثبتة / Get pinned shortcuts
     */
    fun getPinnedShortcuts(context: Context): List<String> {
        return ShortcutManagerCompat.getShortcuts(
            context,
            ShortcutManagerCompat.FLAG_MATCH_PINNED
        ).map { it.id }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التعطيل والتمكين
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تعطيل اختصارات / Disable shortcuts
     */
    @RequiresApi(Build.VERSION_CODES.N_MR1)
    fun disableShortcuts(context: Context, shortcutIds: List<String>, reason: String) {
        val shortcutManager = context.getSystemService(android.content.pm.ShortcutManager::class.java)
        shortcutManager?.disableShortcuts(shortcutIds, reason)
    }
    
    /**
     * تمكين اختصارات / Enable shortcuts
     */
    @RequiresApi(Build.VERSION_CODES.N_MR1)
    fun enableShortcuts(context: Context, shortcutIds: List<String>) {
        val shortcutManager = context.getSystemService(android.content.pm.ShortcutManager::class.java)
        shortcutManager?.enableShortcuts(shortcutIds)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الإبلاغ
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الإبلاغ عن استخدام اختصار / Report shortcut usage
     */
    fun reportShortcutUsed(context: Context, shortcutId: String) {
        ShortcutManagerCompat.reportShortcutUsed(context, shortcutId)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         المعلومات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحد الأقصى للاختصارات / Max shortcut count
     */
    fun getMaxShortcutCount(context: Context): Int {
        return ShortcutManagerCompat.getMaxShortcutCountPerActivity(context)
    }
    
    /**
     * هل الاختصارات مدعومة؟ / Are shortcuts supported?
     */
    fun isShortcutsSupported(): Boolean {
        return Build.VERSION.SDK_INT >= Build.VERSION_CODES.N_MR1
    }
    
    /**
     * هل الاختصارات محدودة السعة؟ / Is rate limited?
     */
    @RequiresApi(Build.VERSION_CODES.N_MR1)
    fun isRateLimited(context: Context): Boolean {
        val shortcutManager = context.getSystemService(android.content.pm.ShortcutManager::class.java)
        return shortcutManager?.isRateLimitingActive == true
    }
    
    /**
     * جميع الاختصارات / All shortcuts
     */
    fun getAllShortcuts(context: Context): Map<ShortcutType, List<String>> {
        val shortcuts = ShortcutManagerCompat.getShortcuts(
            context,
            ShortcutManagerCompat.FLAG_MATCH_DYNAMIC or
            ShortcutManagerCompat.FLAG_MATCH_PINNED or
            ShortcutManagerCompat.FLAG_MATCH_MANIFEST
        )
        
        val result = mutableMapOf<ShortcutType, MutableList<String>>()
        result[ShortcutType.STATIC] = mutableListOf()
        result[ShortcutType.DYNAMIC] = mutableListOf()
        result[ShortcutType.PINNED] = mutableListOf()
        
        shortcuts.forEach { shortcut ->
            when {
                shortcut.isDeclaredInManifest -> result[ShortcutType.STATIC]?.add(shortcut.id)
                shortcut.isDynamic -> result[ShortcutType.DYNAMIC]?.add(shortcut.id)
                shortcut.isPinned -> result[ShortcutType.PINNED]?.add(shortcut.id)
            }
        }
        
        return result
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الاختصارات السريعة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إنشاء اختصار محادثة سريع / Create quick conversation shortcut
     */
    fun createConversationShortcut(
        context: Context,
        conversationId: String,
        personName: String,
        personIcon: android.graphics.Bitmap?,
        targetActivity: Class<*>
    ): Boolean {
        val intent = Intent(context, targetActivity).apply {
            action = Intent.ACTION_VIEW
            putExtra("conversation_id", conversationId)
        }
        
        val builder = ShortcutInfoCompat.Builder(context, conversationId)
            .setShortLabel(personName)
            .setLongLabel("محادثة مع $personName")
            .setIntent(intent)
            .setIsConversation()
            .setCategories(setOf("sad.category.conversation"))
        
        personIcon?.let {
            builder.setIcon(IconCompat.createWithBitmap(it))
        }
        
        return ShortcutManagerCompat.pushDynamicShortcut(context, builder.build())
    }
    
    /**
     * إنشاء اختصار إجراء سريع / Create quick action shortcut
     */
    fun createActionShortcut(
        context: Context,
        actionId: String,
        actionLabel: String,
        iconResId: Int,
        targetActivity: Class<*>,
        extras: Map<String, String> = emptyMap()
    ): Boolean {
        val intent = Intent(context, targetActivity).apply {
            action = "sad.action.QUICK_ACTION"
            putExtra("action_id", actionId)
            extras.forEach { (key, value) -> putExtra(key, value) }
        }
        
        val shortcut = ShortcutInfoCompat.Builder(context, actionId)
            .setShortLabel(actionLabel)
            .setIcon(IconCompat.createWithResource(context, iconResId))
            .setIntent(intent)
            .build()
        
        return ShortcutManagerCompat.pushDynamicShortcut(context, shortcut)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    private fun buildShortcut(context: Context, data: ShortcutData): ShortcutInfoCompat {
        val intent = Intent(context, data.targetActivity).apply {
            action = data.intentAction
            data.extras.forEach { (key, value) ->
                putExtra(key, value)
            }
        }
        
        val builder = ShortcutInfoCompat.Builder(context, data.id)
            .setShortLabel(data.shortLabel)
            .setLongLabel(data.longLabel ?: data.shortLabel)
            .setIntent(intent)
            .setRank(data.rank)
        
        if (data.categories.isNotEmpty()) {
            builder.setCategories(data.categories)
        }
        
        // إضافة الأيقونة
        data.iconResId?.let { resId ->
            builder.setIcon(IconCompat.createWithResource(context, resId))
        }
        
        data.iconBitmap?.let { bitmap ->
            builder.setIcon(IconCompat.createWithBitmap(bitmap))
        }
        
        return builder.build()
    }
}
