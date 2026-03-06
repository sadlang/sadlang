// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadAccessibilityBridge.kt
// الوصف: جسر Kotlin لخدمات إمكانية الوصول
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.accessibilityservice.AccessibilityServiceInfo
import android.content.Context
import android.content.Intent
import android.os.Build
import android.provider.Settings
import android.view.accessibility.AccessibilityEvent
import android.view.accessibility.AccessibilityManager

/**
 * جسر إمكانية الوصول — SadAccessibilityBridge
 */
object SadAccessibilityBridge {
    
    private var context: Context? = null
    private var accessibilityManager: AccessibilityManager? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        accessibilityManager = ctx.getSystemService(Context.ACCESSIBILITY_SERVICE) as? AccessibilityManager
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Accessibility State
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun isAccessibilityEnabled(): Boolean {
        return accessibilityManager?.isEnabled == true
    }
    
    @JvmStatic
    fun isTouchExplorationEnabled(): Boolean {
        return accessibilityManager?.isTouchExplorationEnabled == true
    }
    
    @JvmStatic
    fun isTalkBackEnabled(): Boolean {
        val services = getEnabledAccessibilityServices()
        return services.any { 
            it.contains("talkback", ignoreCase = true) ||
            it.contains("google.android.marvin.talkback", ignoreCase = true)
        }
    }
    
    @JvmStatic
    fun isScreenReaderActive(): Boolean {
        return isTouchExplorationEnabled() || isTalkBackEnabled()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Enabled Services
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getEnabledAccessibilityServices(): List<String> {
        return try {
            val enabledServices = Settings.Secure.getString(
                context?.contentResolver,
                Settings.Secure.ENABLED_ACCESSIBILITY_SERVICES
            )
            enabledServices?.split(":")?.filter { it.isNotEmpty() } ?: emptyList()
        } catch (e: Exception) {
            emptyList()
        }
    }
    
    @JvmStatic
    fun getEnabledServicesInfo(): List<Map<String, Any?>> {
        val manager = accessibilityManager ?: return emptyList()
        
        return try {
            manager.getEnabledAccessibilityServiceList(AccessibilityServiceInfo.FEEDBACK_ALL_MASK)
                .map { info ->
                    mapOf(
                        "id" to info.id,
                        "packageName" to info.resolveInfo?.serviceInfo?.packageName,
                        "description" to info.description,
                        "feedbackType" to getFeedbackTypeLabel(info.feedbackType),
                        "capabilities" to getCapabilitiesLabels(info.capabilities)
                    )
                }
        } catch (e: Exception) {
            emptyList()
        }
    }
    
    @JvmStatic
    fun getInstalledAccessibilityServices(): List<Map<String, Any?>> {
        val manager = accessibilityManager ?: return emptyList()
        
        return try {
            manager.installedAccessibilityServiceList.map { info ->
                mapOf(
                    "id" to info.id,
                    "packageName" to info.resolveInfo?.serviceInfo?.packageName,
                    "description" to info.description,
                    "settingsActivityName" to info.settingsActivityName
                )
            }
        } catch (e: Exception) {
            emptyList()
        }
    }
    
    private fun getFeedbackTypeLabel(feedbackType: Int): List<String> {
        val labels = mutableListOf<String>()
        
        if (feedbackType and AccessibilityServiceInfo.FEEDBACK_SPOKEN != 0) labels.add("منطوق")
        if (feedbackType and AccessibilityServiceInfo.FEEDBACK_HAPTIC != 0) labels.add("لمسي")
        if (feedbackType and AccessibilityServiceInfo.FEEDBACK_AUDIBLE != 0) labels.add("مسموع")
        if (feedbackType and AccessibilityServiceInfo.FEEDBACK_VISUAL != 0) labels.add("مرئي")
        if (feedbackType and AccessibilityServiceInfo.FEEDBACK_GENERIC != 0) labels.add("عام")
        if (feedbackType and AccessibilityServiceInfo.FEEDBACK_BRAILLE != 0) labels.add("برايل")
        
        return labels
    }
    
    private fun getCapabilitiesLabels(capabilities: Int): List<String> {
        val labels = mutableListOf<String>()
        
        if (capabilities and AccessibilityServiceInfo.CAPABILITY_CAN_RETRIEVE_WINDOW_CONTENT != 0) {
            labels.add("استرجاع محتوى النافذة")
        }
        if (capabilities and AccessibilityServiceInfo.CAPABILITY_CAN_REQUEST_TOUCH_EXPLORATION != 0) {
            labels.add("استكشاف اللمس")
        }
        if (capabilities and AccessibilityServiceInfo.CAPABILITY_CAN_REQUEST_FILTER_KEY_EVENTS != 0) {
            labels.add("تصفية أحداث المفاتيح")
        }
        if (capabilities and AccessibilityServiceInfo.CAPABILITY_CAN_CONTROL_MAGNIFICATION != 0) {
            labels.add("التحكم في التكبير")
        }
        if (capabilities and AccessibilityServiceInfo.CAPABILITY_CAN_PERFORM_GESTURES != 0) {
            labels.add("تنفيذ الإيماءات")
        }
        
        return labels
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Announcements
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun announce(message: String): Boolean {
        val manager = accessibilityManager ?: return false
        
        if (!manager.isEnabled) return false
        
        return try {
            val event = AccessibilityEvent.obtain(AccessibilityEvent.TYPE_ANNOUNCEMENT)
            event.text.add(message)
            event.packageName = context?.packageName
            event.className = javaClass.name
            
            manager.sendAccessibilityEvent(event)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun announceForAccessibility(message: String): Boolean {
        // Alternative method using View.announceForAccessibility would require a View
        // This is a simplified version using AccessibilityEvent
        return announce(message)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Settings
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun openAccessibilitySettings(): Boolean {
        val ctx = context ?: return false
        
        return try {
            val intent = Intent(Settings.ACTION_ACCESSIBILITY_SETTINGS).apply {
                addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            }
            ctx.startActivity(intent)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun openTalkBackSettings(): Boolean {
        val ctx = context ?: return false
        
        return try {
            val intent = Intent().apply {
                setClassName(
                    "com.google.android.marvin.talkback",
                    "com.android.talkback.TalkBackPreferencesActivity"
                )
                addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            }
            ctx.startActivity(intent)
            true
        } catch (e: Exception) {
            // Fallback to general accessibility settings
            openAccessibilitySettings()
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Font Scaling
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getFontScale(): Float {
        return try {
            Settings.System.getFloat(
                context?.contentResolver,
                Settings.System.FONT_SCALE,
                1.0f
            )
        } catch (e: Exception) {
            1.0f
        }
    }
    
    @JvmStatic
    fun isLargeFontEnabled(): Boolean {
        return getFontScale() > 1.0f
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Display Settings
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun isHighContrastTextEnabled(): Boolean {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP) {
            return false
        }
        
        return try {
            Settings.Secure.getInt(
                context?.contentResolver,
                "high_text_contrast_enabled",
                0
            ) == 1
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun isColorInversionEnabled(): Boolean {
        return try {
            Settings.Secure.getInt(
                context?.contentResolver,
                Settings.Secure.ACCESSIBILITY_DISPLAY_INVERSION_ENABLED,
                0
            ) == 1
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun isColorCorrectionEnabled(): Boolean {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP) {
            return false
        }
        
        return try {
            Settings.Secure.getInt(
                context?.contentResolver,
                "accessibility_display_daltonizer_enabled",
                0
            ) == 1
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun getColorCorrectionMode(): String {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP) {
            return "غير متاح"
        }
        
        return try {
            val mode = Settings.Secure.getInt(
                context?.contentResolver,
                "accessibility_display_daltonizer",
                0
            )
            
            when (mode) {
                0 -> "معطل"
                11 -> "عمى الألوان الأحمر-الأخضر (Deuteranomaly)"
                12 -> "عمى الألوان الأحمر-الأخضر (Protanomaly)"
                13 -> "عمى الألوان الأزرق-الأصفر (Tritanomaly)"
                else -> "غير معروف"
            }
        } catch (e: Exception) {
            "غير متاح"
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Timeout Settings
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getRecommendedTimeoutMillis(
        originalTimeout: Int,
        flags: Int = AccessibilityManager.FLAG_CONTENT_ICONS or 
                      AccessibilityManager.FLAG_CONTENT_TEXT
    ): Int {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.Q) {
            return originalTimeout
        }
        
        return try {
            accessibilityManager?.getRecommendedTimeoutMillis(originalTimeout, flags) ?: originalTimeout
        } catch (e: Exception) {
            originalTimeout
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Status
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getAccessibilityStatus(): Map<String, Any?> {
        return mapOf(
            "accessibilityEnabled" to isAccessibilityEnabled(),
            "touchExplorationEnabled" to isTouchExplorationEnabled(),
            "talkBackEnabled" to isTalkBackEnabled(),
            "screenReaderActive" to isScreenReaderActive(),
            "largeFontEnabled" to isLargeFontEnabled(),
            "fontScale" to getFontScale(),
            "highContrastTextEnabled" to isHighContrastTextEnabled(),
            "colorInversionEnabled" to isColorInversionEnabled(),
            "colorCorrectionEnabled" to isColorCorrectionEnabled(),
            "colorCorrectionMode" to getColorCorrectionMode(),
            "enabledServicesCount" to getEnabledAccessibilityServices().size
        )
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Listener
    // ═══════════════════════════════════════════════════════════════════════════════
    
    private var accessibilityStateChangeListener: AccessibilityManager.AccessibilityStateChangeListener? = null
    private var touchExplorationStateChangeListener: AccessibilityManager.TouchExplorationStateChangeListener? = null
    
    @JvmStatic
    fun addAccessibilityStateChangeListener(callback: (Boolean) -> Unit) {
        accessibilityStateChangeListener = AccessibilityManager.AccessibilityStateChangeListener { enabled ->
            callback(enabled)
        }
        accessibilityManager?.addAccessibilityStateChangeListener(accessibilityStateChangeListener!!)
    }
    
    @JvmStatic
    fun removeAccessibilityStateChangeListener() {
        accessibilityStateChangeListener?.let {
            accessibilityManager?.removeAccessibilityStateChangeListener(it)
        }
        accessibilityStateChangeListener = null
    }
    
    @JvmStatic
    fun addTouchExplorationStateChangeListener(callback: (Boolean) -> Unit) {
        touchExplorationStateChangeListener = AccessibilityManager.TouchExplorationStateChangeListener { enabled ->
            callback(enabled)
        }
        accessibilityManager?.addTouchExplorationStateChangeListener(touchExplorationStateChangeListener!!)
    }
    
    @JvmStatic
    fun removeTouchExplorationStateChangeListener() {
        touchExplorationStateChangeListener?.let {
            accessibilityManager?.removeTouchExplorationStateChangeListener(it)
        }
        touchExplorationStateChangeListener = null
    }
}
