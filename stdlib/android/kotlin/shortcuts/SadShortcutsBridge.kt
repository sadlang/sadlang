// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadShortcutsBridge.kt
// الوصف: جسر Kotlin للاختصارات (App Shortcuts)
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.content.Intent
import android.content.pm.ShortcutInfo
import android.content.pm.ShortcutManager
import android.graphics.drawable.Icon
import android.net.Uri
import android.os.Build
import android.os.PersistableBundle
import androidx.annotation.RequiresApi
import androidx.core.content.pm.ShortcutInfoCompat
import androidx.core.content.pm.ShortcutManagerCompat
import androidx.core.graphics.drawable.IconCompat

/**
 * جسر اختصارات التطبيق — SadShortcutsBridge
 */
object SadShortcutsBridge {
    
    private var context: Context? = null
    private var shortcutManager: ShortcutManager? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N_MR1) {
            shortcutManager = ctx.getSystemService(ShortcutManager::class.java)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Availability
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun isShortcutsSupported(): Boolean {
        return Build.VERSION.SDK_INT >= Build.VERSION_CODES.N_MR1
    }
    
    @JvmStatic
    fun isPinShortcutsSupported(): Boolean {
        val ctx = context ?: return false
        return ShortcutManagerCompat.isRequestPinShortcutSupported(ctx)
    }
    
    @JvmStatic
    fun getMaxShortcutCount(): Int {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N_MR1) {
            return shortcutManager?.maxShortcutCountPerActivity ?: 0
        }
        return 0
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Dynamic Shortcuts
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun addDynamicShortcut(
        id: String,
        shortLabel: String,
        longLabel: String,
        iconResId: Int,
        targetActivityClass: Class<*>,
        rank: Int = 0,
        extras: Map<String, String>? = null
    ): Boolean {
        val ctx = context ?: return false
        
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.N_MR1) {
            return false
        }
        
        return try {
            val intent = Intent(ctx, targetActivityClass).apply {
                action = Intent.ACTION_VIEW
                extras?.forEach { (key, value) ->
                    putExtra(key, value)
                }
            }
            
            val shortcut = ShortcutInfoCompat.Builder(ctx, id)
                .setShortLabel(shortLabel)
                .setLongLabel(longLabel)
                .setIcon(IconCompat.createWithResource(ctx, iconResId))
                .setIntent(intent)
                .setRank(rank)
                .build()
            
            ShortcutManagerCompat.pushDynamicShortcut(ctx, shortcut)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun addDynamicShortcutWithUrl(
        id: String,
        shortLabel: String,
        longLabel: String,
        iconResId: Int,
        url: String,
        rank: Int = 0
    ): Boolean {
        val ctx = context ?: return false
        
        return try {
            val intent = Intent(Intent.ACTION_VIEW, Uri.parse(url))
            
            val shortcut = ShortcutInfoCompat.Builder(ctx, id)
                .setShortLabel(shortLabel)
                .setLongLabel(longLabel)
                .setIcon(IconCompat.createWithResource(ctx, iconResId))
                .setIntent(intent)
                .setRank(rank)
                .build()
            
            ShortcutManagerCompat.pushDynamicShortcut(ctx, shortcut)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun removeDynamicShortcut(id: String): Boolean {
        val ctx = context ?: return false
        
        return try {
            ShortcutManagerCompat.removeDynamicShortcuts(ctx, listOf(id))
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun removeDynamicShortcuts(ids: List<String>): Boolean {
        val ctx = context ?: return false
        
        return try {
            ShortcutManagerCompat.removeDynamicShortcuts(ctx, ids)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun removeAllDynamicShortcuts(): Boolean {
        val ctx = context ?: return false
        
        return try {
            ShortcutManagerCompat.removeAllDynamicShortcuts(ctx)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun getDynamicShortcuts(): List<Map<String, Any?>> {
        val ctx = context ?: return emptyList()
        
        return try {
            ShortcutManagerCompat.getDynamicShortcuts(ctx).map { shortcut ->
                mapOf(
                    "id" to shortcut.id,
                    "shortLabel" to shortcut.shortLabel?.toString(),
                    "longLabel" to shortcut.longLabel?.toString(),
                    "rank" to shortcut.rank,
                    "isEnabled" to shortcut.isEnabled
                )
            }
        } catch (e: Exception) {
            emptyList()
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Pinned Shortcuts
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun requestPinShortcut(
        id: String,
        shortLabel: String,
        longLabel: String,
        iconResId: Int,
        targetActivityClass: Class<*>,
        extras: Map<String, String>? = null
    ): Boolean {
        val ctx = context ?: return false
        
        if (!isPinShortcutsSupported()) {
            return false
        }
        
        return try {
            val intent = Intent(ctx, targetActivityClass).apply {
                action = Intent.ACTION_VIEW
                extras?.forEach { (key, value) ->
                    putExtra(key, value)
                }
            }
            
            val shortcut = ShortcutInfoCompat.Builder(ctx, id)
                .setShortLabel(shortLabel)
                .setLongLabel(longLabel)
                .setIcon(IconCompat.createWithResource(ctx, iconResId))
                .setIntent(intent)
                .build()
            
            ShortcutManagerCompat.requestPinShortcut(ctx, shortcut, null)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun getPinnedShortcuts(): List<Map<String, Any?>> {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.N_MR1) {
            return emptyList()
        }
        
        return try {
            shortcutManager?.pinnedShortcuts?.map { shortcut ->
                mapOf(
                    "id" to shortcut.id,
                    "shortLabel" to shortcut.shortLabel?.toString(),
                    "longLabel" to shortcut.longLabel?.toString(),
                    "isEnabled" to shortcut.isEnabled,
                    "isPinned" to shortcut.isPinned,
                    "isDynamic" to shortcut.isDynamic
                )
            } ?: emptyList()
        } catch (e: Exception) {
            emptyList()
        }
    }
    
    @JvmStatic
    fun disableShortcuts(ids: List<String>, disabledMessage: String): Boolean {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.N_MR1) {
            return false
        }
        
        return try {
            shortcutManager?.disableShortcuts(ids, disabledMessage)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun enableShortcuts(ids: List<String>): Boolean {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.N_MR1) {
            return false
        }
        
        return try {
            shortcutManager?.enableShortcuts(ids)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Shortcut Usage
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun reportShortcutUsed(id: String): Boolean {
        val ctx = context ?: return false
        
        return try {
            ShortcutManagerCompat.reportShortcutUsed(ctx, id)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Update Shortcuts
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun updateShortcuts(shortcuts: List<Map<String, Any?>>): Boolean {
        val ctx = context ?: return false
        
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.N_MR1) {
            return false
        }
        
        return try {
            val shortcutInfoList = shortcuts.mapNotNull { shortcutData ->
                val id = shortcutData["id"] as? String ?: return@mapNotNull null
                val shortLabel = shortcutData["shortLabel"] as? String ?: return@mapNotNull null
                
                ShortcutInfo.Builder(ctx, id)
                    .setShortLabel(shortLabel)
                    .apply {
                        (shortcutData["longLabel"] as? String)?.let { setLongLabel(it) }
                        (shortcutData["rank"] as? Int)?.let { setRank(it) }
                    }
                    .build()
            }
            
            shortcutManager?.updateShortcuts(shortcutInfoList)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Rate Limiting
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun isRateLimitingActive(): Boolean {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.N_MR1) {
            return false
        }
        
        return shortcutManager?.isRateLimitingActive == true
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Manifest Shortcuts (Static)
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getManifestShortcuts(): List<Map<String, Any?>> {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.N_MR1) {
            return emptyList()
        }
        
        return try {
            shortcutManager?.manifestShortcuts?.map { shortcut ->
                mapOf(
                    "id" to shortcut.id,
                    "shortLabel" to shortcut.shortLabel?.toString(),
                    "longLabel" to shortcut.longLabel?.toString(),
                    "rank" to shortcut.rank,
                    "isEnabled" to shortcut.isEnabled,
                    "isDeclaredInManifest" to shortcut.isDeclaredInManifest
                )
            } ?: emptyList()
        } catch (e: Exception) {
            emptyList()
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Utility
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getShortcutsInfo(): Map<String, Any?> {
        return mapOf(
            "supported" to isShortcutsSupported(),
            "pinSupported" to isPinShortcutsSupported(),
            "maxCount" to getMaxShortcutCount(),
            "rateLimitingActive" to isRateLimitingActive(),
            "dynamicCount" to getDynamicShortcuts().size,
            "pinnedCount" to getPinnedShortcuts().size,
            "manifestCount" to getManifestShortcuts().size
        )
    }
}
