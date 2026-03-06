package sad.android.tv.navigation

import android.content.Context
import android.os.Handler
import android.os.Looper
import android.view.KeyEvent

/**
 * جسر ملاحة التلفزيون - تنقل D-pad و إدارة التركيز
 * TV Navigation Bridge - D-pad navigation and focus management
 * 
 * @author فريق لغة ص
 */
object SadTVNavigationBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    
    /**
     * اتجاه التنقل / Navigation direction
     */
    enum class NavDirection {
        UP,
        DOWN,
        LEFT,
        RIGHT,
        CENTER,    // Enter/OK
        BACK
    }
    
    /**
     * منطقة تنقل / Navigation zone
     */
    data class NavZone(
        val id: String,
        val name: String,
        val rows: Int,
        val columns: Int,
        val wrapHorizontal: Boolean = false,
        val wrapVertical: Boolean = false
    )
    
    /**
     * موضع التنقل / Navigation position
     */
    data class NavPosition(
        val zoneId: String,
        val row: Int,
        val column: Int
    )
    
    /**
     * تكوين التنقل / Navigation config
     */
    data class NavConfig(
        val repeatDelayMs: Long = 150,
        val initialRepeatDelayMs: Long = 400,
        val enableSounds: Boolean = true,
        val enableHaptics: Boolean = false,
        val focusAnimationDuration: Long = 200
    )
    
    /**
     * مستمع التنقل / Navigation listener
     */
    interface NavigationListener {
        fun onNavigate(direction: NavDirection, from: NavPosition, to: NavPosition)
        fun onSelect(position: NavPosition)
        fun onBack(): Boolean
        fun onZoneChanged(from: String?, to: String)
    }
    
    /**
     * مزود التنقل المخصص / Custom navigation provider
     */
    interface CustomNavigationProvider {
        fun getNextPosition(from: NavPosition, direction: NavDirection): NavPosition?
        fun canNavigate(from: NavPosition, direction: NavDirection): Boolean
    }
    
    private var config = NavConfig()
    private var currentPosition = NavPosition("main", 0, 0)
    private val zones = mutableMapOf<String, NavZone>()
    private var navigationListener: NavigationListener? = null
    private var customProvider: CustomNavigationProvider? = null
    
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
     * تعيين التكوين / Set config
     */
    fun setConfig(newConfig: NavConfig) {
        config = newConfig
    }
    
    /**
     * تعيين المستمع / Set listener
     */
    fun setNavigationListener(listener: NavigationListener?) {
        navigationListener = listener
    }
    
    /**
     * تعيين مزود مخصص / Set custom provider
     */
    fun setCustomProvider(provider: CustomNavigationProvider?) {
        customProvider = provider
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة المناطق
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تسجيل منطقة / Register zone
     */
    fun registerZone(zone: NavZone) {
        zones[zone.id] = zone
    }
    
    /**
     * إلغاء تسجيل منطقة / Unregister zone
     */
    fun unregisterZone(zoneId: String) {
        zones.remove(zoneId)
    }
    
    /**
     * الحصول على منطقة / Get zone
     */
    fun getZone(zoneId: String): NavZone? = zones[zoneId]
    
    /**
     * جميع المناطق / All zones
     */
    fun getAllZones(): List<NavZone> = zones.values.toList()
    
    // ═══════════════════════════════════════════════════════════════
    //                         التنقل
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * معالجة مفتاح / Handle key
     */
    fun handleKey(keyCode: Int): Boolean {
        val direction = keyCodeToDirection(keyCode) ?: return false
        return navigate(direction)
    }
    
    /**
     * التنقل باتجاه / Navigate in direction
     */
    fun navigate(direction: NavDirection): Boolean {
        val from = currentPosition
        
        // التحقق من زر الرجوع
        if (direction == NavDirection.BACK) {
            return mainHandler.post { navigationListener?.onBack() }.let { true }
        }
        
        // التحقق من زر التحديد
        if (direction == NavDirection.CENTER) {
            mainHandler.post { navigationListener?.onSelect(from) }
            return true
        }
        
        // حساب الموضع الجديد
        val to = calculateNextPosition(from, direction) ?: return false
        
        // تحديث الموضع
        val oldZone = currentPosition.zoneId
        currentPosition = to
        
        // إعلام المستمع
        mainHandler.post {
            if (oldZone != to.zoneId) {
                navigationListener?.onZoneChanged(oldZone, to.zoneId)
            }
            navigationListener?.onNavigate(direction, from, to)
        }
        
        return true
    }
    
    /**
     * الانتقال إلى موضع / Jump to position
     */
    fun jumpTo(position: NavPosition) {
        val oldZone = currentPosition.zoneId
        currentPosition = position
        
        if (oldZone != position.zoneId) {
            mainHandler.post { navigationListener?.onZoneChanged(oldZone, position.zoneId) }
        }
    }
    
    /**
     * الانتقال إلى منطقة / Jump to zone
     */
    fun jumpToZone(zoneId: String, row: Int = 0, column: Int = 0) {
        if (zones.containsKey(zoneId)) {
            jumpTo(NavPosition(zoneId, row, column))
        }
    }
    
    private fun calculateNextPosition(from: NavPosition, direction: NavDirection): NavPosition? {
        // استخدام المزود المخصص إذا متاح
        customProvider?.let { provider ->
            if (provider.canNavigate(from, direction)) {
                return provider.getNextPosition(from, direction)
            }
        }
        
        // الحساب الافتراضي
        val zone = zones[from.zoneId] ?: return null
        
        var newRow = from.row
        var newColumn = from.column
        
        when (direction) {
            NavDirection.UP -> {
                newRow = from.row - 1
                if (newRow < 0) {
                    newRow = if (zone.wrapVertical) zone.rows - 1 else return null
                }
            }
            NavDirection.DOWN -> {
                newRow = from.row + 1
                if (newRow >= zone.rows) {
                    newRow = if (zone.wrapVertical) 0 else return null
                }
            }
            NavDirection.LEFT -> {
                newColumn = from.column - 1
                if (newColumn < 0) {
                    newColumn = if (zone.wrapHorizontal) zone.columns - 1 else return null
                }
            }
            NavDirection.RIGHT -> {
                newColumn = from.column + 1
                if (newColumn >= zone.columns) {
                    newColumn = if (zone.wrapHorizontal) 0 else return null
                }
            }
            else -> return null
        }
        
        return NavPosition(from.zoneId, newRow, newColumn)
    }
    
    private fun keyCodeToDirection(keyCode: Int): NavDirection? {
        return when (keyCode) {
            KeyEvent.KEYCODE_DPAD_UP -> NavDirection.UP
            KeyEvent.KEYCODE_DPAD_DOWN -> NavDirection.DOWN
            KeyEvent.KEYCODE_DPAD_LEFT -> NavDirection.LEFT
            KeyEvent.KEYCODE_DPAD_RIGHT -> NavDirection.RIGHT
            KeyEvent.KEYCODE_DPAD_CENTER, KeyEvent.KEYCODE_ENTER -> NavDirection.CENTER
            KeyEvent.KEYCODE_BACK -> NavDirection.BACK
            else -> null
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الحالة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على الموضع الحالي / Get current position
     */
    fun getCurrentPosition(): NavPosition = currentPosition
    
    /**
     * الحصول على المنطقة الحالية / Get current zone
     */
    fun getCurrentZone(): NavZone? = zones[currentPosition.zoneId]
    
    /**
     * التحقق من إمكانية التنقل / Check if can navigate
     */
    fun canNavigate(direction: NavDirection): Boolean {
        if (direction == NavDirection.CENTER || direction == NavDirection.BACK) return true
        return calculateNextPosition(currentPosition, direction) != null
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        zones.clear()
        navigationListener = null
        customProvider = null
        context = null
    }
}
