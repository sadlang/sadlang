package sad.android.communication.directshare

import android.content.ComponentName
import android.content.Context
import android.content.Intent
import android.content.pm.ShortcutInfo
import android.content.pm.ShortcutManager
import android.graphics.drawable.Icon
import android.os.Build
import androidx.annotation.RequiresApi
import androidx.core.content.pm.ShortcutInfoCompat
import androidx.core.content.pm.ShortcutManagerCompat
import androidx.core.graphics.drawable.IconCompat

/**
 * جسر المشاركة المباشرة - أهداف المشاركة السريعة
 * Direct Share Bridge - Quick share targets
 * 
 * @author فريق لغة ص
 */
object SadDirectShareBridge {
    
    /**
     * هدف مشاركة / Share target
     */
    data class ShareTarget(
        val id: String,
        val label: String,
        val iconResId: Int? = null,
        val iconUri: String? = null,
        val category: String,
        val rank: Int = 0,
        val extras: Map<String, String> = emptyMap()
    )
    
    /**
     * فئة المشاركة / Share category
     */
    object ShareCategory {
        const val MESSAGING = "sad.share.category.MESSAGING"
        const val SOCIAL = "sad.share.category.SOCIAL"
        const val CONTACTS = "sad.share.category.CONTACTS"
        const val FILES = "sad.share.category.FILES"
        const val TEXT = "sad.share.category.TEXT"
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة الاختصارات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إضافة هدف مشاركة / Add share target
     */
    fun addShareTarget(
        context: Context,
        target: ShareTarget,
        targetActivity: Class<*>
    ): Boolean {
        val shortcut = buildShortcut(context, target, targetActivity)
        return ShortcutManagerCompat.pushDynamicShortcut(context, shortcut)
    }
    
    /**
     * إضافة أهداف متعددة / Add multiple share targets
     */
    fun addShareTargets(
        context: Context,
        targets: List<ShareTarget>,
        targetActivity: Class<*>
    ): Boolean {
        return targets.all { target ->
            addShareTarget(context, target, targetActivity)
        }
    }
    
    /**
     * تحديث هدف مشاركة / Update share target
     */
    fun updateShareTarget(
        context: Context,
        target: ShareTarget,
        targetActivity: Class<*>
    ): Boolean {
        val shortcut = buildShortcut(context, target, targetActivity)
        return ShortcutManagerCompat.updateShortcuts(context, listOf(shortcut))
    }
    
    /**
     * إزالة هدف مشاركة / Remove share target
     */
    fun removeShareTarget(context: Context, targetId: String) {
        ShortcutManagerCompat.removeDynamicShortcuts(context, listOf(targetId))
    }
    
    /**
     * إزالة جميع أهداف المشاركة / Remove all share targets
     */
    fun removeAllShareTargets(context: Context) {
        ShortcutManagerCompat.removeAllDynamicShortcuts(context)
    }
    
    /**
     * جلب جميع أهداف المشاركة / Get all share targets
     */
    fun getShareTargets(context: Context): List<ShareTarget> {
        return ShortcutManagerCompat.getDynamicShortcuts(context).map { shortcut ->
            ShareTarget(
                id = shortcut.id,
                label = shortcut.shortLabel?.toString() ?: "",
                category = shortcut.categories?.firstOrNull() ?: "",
                rank = shortcut.rank
            )
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الإبلاغ عن الاستخدام
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الإبلاغ عن استخدام هدف / Report target usage
     */
    fun reportShareTargetUsed(context: Context, targetId: String) {
        ShortcutManagerCompat.reportShortcutUsed(context, targetId)
    }
    
    /**
     * تعطيل هدف مشاركة / Disable share target
     */
    @RequiresApi(Build.VERSION_CODES.N_MR1)
    fun disableShareTarget(context: Context, targetId: String, message: String) {
        val shortcutManager = context.getSystemService(ShortcutManager::class.java)
        shortcutManager?.disableShortcuts(listOf(targetId), message)
    }
    
    /**
     * تمكين هدف مشاركة / Enable share targets
     */
    @RequiresApi(Build.VERSION_CODES.N_MR1)
    fun enableShareTargets(context: Context, targetIds: List<String>) {
        val shortcutManager = context.getSystemService(ShortcutManager::class.java)
        shortcutManager?.enableShortcuts(targetIds)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معالجة المشاركة الواردة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * معالجة intent المشاركة / Handle share intent
     */
    fun handleShareIntent(intent: Intent): ShareData? {
        if (intent.action != Intent.ACTION_SEND && 
            intent.action != Intent.ACTION_SEND_MULTIPLE) {
            return null
        }
        
        val type = intent.type ?: return null
        
        return when {
            type.startsWith("text/") -> {
                val text = intent.getStringExtra(Intent.EXTRA_TEXT)
                val subject = intent.getStringExtra(Intent.EXTRA_SUBJECT)
                ShareData.Text(text ?: "", subject)
            }
            type.startsWith("image/") -> {
                val uri = intent.getParcelableExtra<android.net.Uri>(Intent.EXTRA_STREAM)
                ShareData.Image(uri)
            }
            type.startsWith("video/") -> {
                val uri = intent.getParcelableExtra<android.net.Uri>(Intent.EXTRA_STREAM)
                ShareData.Video(uri)
            }
            type == "*/*" || type.startsWith("application/") -> {
                val uri = intent.getParcelableExtra<android.net.Uri>(Intent.EXTRA_STREAM)
                ShareData.File(uri, type)
            }
            else -> null
        }
    }
    
    /**
     * معالجة المشاركة المتعددة / Handle multiple share
     */
    fun handleMultipleShareIntent(intent: Intent): List<ShareData>? {
        if (intent.action != Intent.ACTION_SEND_MULTIPLE) return null
        
        val type = intent.type ?: return null
        val uris = intent.getParcelableArrayListExtra<android.net.Uri>(Intent.EXTRA_STREAM)
            ?: return null
        
        return uris.map { uri ->
            when {
                type.startsWith("image/") -> ShareData.Image(uri)
                type.startsWith("video/") -> ShareData.Video(uri)
                else -> ShareData.File(uri, type)
            }
        }
    }
    
    /**
     * جلب معرف الهدف من Intent / Get target ID from intent
     */
    fun getTargetIdFromIntent(intent: Intent): String? {
        return intent.getStringExtra("sad.direct_share.target_id")
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         أنواع البيانات
    // ═══════════════════════════════════════════════════════════════
    
    sealed class ShareData {
        data class Text(val content: String, val subject: String?) : ShareData()
        data class Image(val uri: android.net.Uri?) : ShareData()
        data class Video(val uri: android.net.Uri?) : ShareData()
        data class File(val uri: android.net.Uri?, val mimeType: String) : ShareData()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         المعلومات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحد الأقصى لعدد الاختصارات / Max shortcuts count
     */
    fun getMaxShortcutCount(context: Context): Int {
        return ShortcutManagerCompat.getMaxShortcutCountPerActivity(context)
    }
    
    /**
     * هل المشاركة المباشرة مدعومة؟ / Is direct share supported?
     */
    fun isDirectShareSupported(): Boolean {
        return Build.VERSION.SDK_INT >= Build.VERSION_CODES.M
    }
    
    /**
     * هل الاختصارات الديناميكية مدعومة؟ / Are dynamic shortcuts supported?
     */
    fun isDynamicShortcutsSupported(): Boolean {
        return Build.VERSION.SDK_INT >= Build.VERSION_CODES.N_MR1
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    private fun buildShortcut(
        context: Context,
        target: ShareTarget,
        targetActivity: Class<*>
    ): ShortcutInfoCompat {
        val intent = Intent(context, targetActivity).apply {
            action = Intent.ACTION_SEND
            putExtra("sad.direct_share.target_id", target.id)
            target.extras.forEach { (key, value) ->
                putExtra(key, value)
            }
        }
        
        val builder = ShortcutInfoCompat.Builder(context, target.id)
            .setShortLabel(target.label)
            .setLongLabel(target.label)
            .setIntent(intent)
            .setCategories(setOf(target.category))
            .setRank(target.rank)
            .setIsConversation()
        
        // إضافة الأيقونة
        target.iconResId?.let { resId ->
            builder.setIcon(IconCompat.createWithResource(context, resId))
        }
        
        target.iconUri?.let { uri ->
            android.net.Uri.parse(uri)?.let { parsedUri ->
                builder.setIcon(IconCompat.createWithContentUri(parsedUri))
            }
        }
        
        return builder.build()
    }
}
