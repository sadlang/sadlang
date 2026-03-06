package sad.android.tv.screensaver

import android.content.Context
import android.os.Handler
import android.os.Looper

/**
 * جسر شاشة التوقف التلفزيونية - Daydream / Screen saver
 * TV Screensaver Bridge - Daydream / Screen saver
 * 
 * @author فريق لغة ص
 */
object SadTVScreensaverBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    
    /**
     * نوع شاشة التوقف / Screensaver type
     */
    enum class ScreensaverType {
        CLOCK,
        PHOTOS,
        AMBIENT,
        CUSTOM,
        NONE
    }
    
    /**
     * حالة شاشة التوقف / Screensaver state
     */
    enum class ScreensaverState {
        INACTIVE,
        ACTIVE,
        INTERACTIVE
    }
    
    /**
     * تكوين شاشة التوقف / Screensaver config
     */
    data class ScreensaverConfig(
        val type: ScreensaverType = ScreensaverType.CLOCK,
        val idleTimeout: Long = 5 * 60 * 1000, // 5 دقائق
        val showClock: Boolean = true,
        val showWeather: Boolean = false,
        val showNotifications: Boolean = true,
        val brightness: Float = 0.3f,
        val photoSources: List<String> = emptyList(),
        val photoInterval: Long = 10000, // 10 ثواني
        val animationType: AnimationType = AnimationType.FADE
    )
    
    /**
     * نوع الحركة / Animation type
     */
    enum class AnimationType {
        NONE,
        FADE,
        SLIDE,
        ZOOM,
        KEN_BURNS
    }
    
    /**
     * عنصر شاشة التوقف / Screensaver item
     */
    data class ScreensaverItem(
        val id: String,
        val type: ItemType,
        val content: String,
        val x: Float = 0.5f,
        val y: Float = 0.5f,
        val textSize: Float = 48f,
        val color: Int = 0xFFFFFFFF.toInt()
    )
    
    /**
     * نوع العنصر / Item type
     */
    enum class ItemType {
        TEXT,
        CLOCK,
        DATE,
        IMAGE,
        WEATHER
    }
    
    /**
     * مستمع شاشة التوقف / Screensaver listener
     */
    interface ScreensaverListener {
        fun onStateChanged(state: ScreensaverState)
        fun onInteraction()
        fun onPhotoChanged(photoUri: String)
    }
    
    private var screensaverListener: ScreensaverListener? = null
    private var config = ScreensaverConfig()
    private var currentState = ScreensaverState.INACTIVE
    private val items = mutableListOf<ScreensaverItem>()
    private var idleStartTime: Long = 0
    
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
     * تعيين المستمع / Set listener
     */
    fun setScreensaverListener(listener: ScreensaverListener?) {
        screensaverListener = listener
    }
    
    /**
     * تعيين التكوين / Set config
     */
    fun setConfig(newConfig: ScreensaverConfig) {
        config = newConfig
    }
    
    /**
     * الحصول على التكوين / Get config
     */
    fun getConfig(): ScreensaverConfig = config
    
    // ═══════════════════════════════════════════════════════════════
    //                         التحكم
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * بدء شاشة التوقف / Start screensaver
     */
    fun start() {
        if (config.type == ScreensaverType.NONE) return
        
        setState(ScreensaverState.ACTIVE)
    }
    
    /**
     * إيقاف شاشة التوقف / Stop screensaver
     */
    fun stop() {
        setState(ScreensaverState.INACTIVE)
        resetIdleTimer()
    }
    
    /**
     * إعادة تعيين مؤقت الخمول / Reset idle timer
     */
    fun resetIdleTimer() {
        idleStartTime = System.currentTimeMillis()
    }
    
    /**
     * التحقق من الخمول / Check idle
     */
    fun checkIdle(): Boolean {
        val idleTime = System.currentTimeMillis() - idleStartTime
        if (idleTime >= config.idleTimeout && currentState == ScreensaverState.INACTIVE) {
            start()
            return true
        }
        return false
    }
    
    /**
     * معالجة التفاعل / Handle interaction
     */
    fun handleInteraction() {
        if (currentState == ScreensaverState.ACTIVE) {
            setState(ScreensaverState.INTERACTIVE)
            mainHandler.post { screensaverListener?.onInteraction() }
            
            // إعطاء وقت قصير قبل إيقاف شاشة التوقف
            mainHandler.postDelayed({ stop() }, 500)
        }
        resetIdleTimer()
    }
    
    /**
     * الحصول على الحالة / Get state
     */
    fun getState(): ScreensaverState = currentState
    
    /**
     * هل نشطة؟ / Is active?
     */
    fun isActive(): Boolean = currentState == ScreensaverState.ACTIVE
    
    private fun setState(state: ScreensaverState) {
        currentState = state
        mainHandler.post { screensaverListener?.onStateChanged(state) }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         العناصر
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إضافة عنصر / Add item
     */
    fun addItem(item: ScreensaverItem) {
        items.add(item)
    }
    
    /**
     * مسح العناصر / Clear items
     */
    fun clearItems() {
        items.clear()
    }
    
    /**
     * الحصول على العناصر / Get items
     */
    fun getItems(): List<ScreensaverItem> = items.toList()
    
    /**
     * إنشاء عناصر افتراضية / Create default items
     */
    fun createDefaultItems() {
        clearItems()
        
        if (config.showClock) {
            addItem(ScreensaverItem(
                id = "clock",
                type = ItemType.CLOCK,
                content = "",
                y = 0.4f,
                textSize = 96f
            ))
            
            addItem(ScreensaverItem(
                id = "date",
                type = ItemType.DATE,
                content = "",
                y = 0.55f,
                textSize = 32f,
                color = 0xFFCCCCCC.toInt()
            ))
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الصور
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تعيين مصادر الصور / Set photo sources
     */
    fun setPhotoSources(sources: List<String>) {
        config = config.copy(photoSources = sources)
    }
    
    /**
     * الحصول على الصورة التالية / Get next photo
     */
    fun getNextPhoto(): String? {
        val sources = config.photoSources
        if (sources.isEmpty()) return null
        
        return sources.random()
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        stop()
        items.clear()
        screensaverListener = null
        context = null
    }
}
