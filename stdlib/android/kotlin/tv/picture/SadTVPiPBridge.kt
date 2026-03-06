package sad.android.tv.picture

import android.content.Context
import android.os.Handler
import android.os.Looper

/**
 * جسر صورة داخل صورة للتلفزيون - Picture-in-Picture mode
 * TV PiP Bridge - Picture-in-Picture mode
 * 
 * @author فريق لغة ص
 */
object SadTVPiPBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    
    /**
     * حالة PiP
     */
    enum class PiPState {
        INACTIVE,
        ENTERING,
        ACTIVE,
        EXITING
    }
    
    /**
     * موضع PiP
     */
    enum class PiPPosition {
        TOP_LEFT,
        TOP_RIGHT,
        BOTTOM_LEFT,
        BOTTOM_RIGHT,
        CENTER
    }
    
    /**
     * تكوين PiP
     */
    data class PiPConfig(
        val aspectRatioWidth: Int = 16,
        val aspectRatioHeight: Int = 9,
        val position: PiPPosition = PiPPosition.BOTTOM_RIGHT,
        val autoEnterOnBackground: Boolean = true,
        val seamlessResize: Boolean = true,
        val sourceRectHint: SourceRect? = null
    )
    
    /**
     * مستطيل المصدر / Source rect
     */
    data class SourceRect(
        val left: Int,
        val top: Int,
        val right: Int,
        val bottom: Int
    )
    
    /**
     * إجراء PiP
     */
    data class PiPAction(
        val id: String,
        val title: String,
        val iconResId: Int,
        val requestCode: Int
    )
    
    /**
     * معلومات PiP
     */
    data class PiPInfo(
        val state: PiPState,
        val isSupported: Boolean,
        val currentActions: List<PiPAction>,
        val config: PiPConfig
    )
    
    /**
     * مستمع PiP
     */
    interface PiPListener {
        fun onPiPStateChanged(state: PiPState)
        fun onPiPActionClicked(actionId: String)
    }
    
    private var pipListener: PiPListener? = null
    private var config = PiPConfig()
    private var currentState = PiPState.INACTIVE
    private val actions = mutableListOf<PiPAction>()
    
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
    fun setPiPListener(listener: PiPListener?) {
        pipListener = listener
    }
    
    /**
     * تعيين التكوين / Set config
     */
    fun setConfig(newConfig: PiPConfig) {
        config = newConfig
    }
    
    /**
     * الحصول على التكوين / Get config
     */
    fun getConfig(): PiPConfig = config
    
    // ═══════════════════════════════════════════════════════════════
    //                         التحقق من الدعم
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * هل PiP مدعوم؟ / Is PiP supported?
     */
    fun isPiPSupported(): Boolean {
        val ctx = context ?: return false
        return android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O &&
               ctx.packageManager.hasSystemFeature(android.content.pm.PackageManager.FEATURE_PICTURE_IN_PICTURE)
    }
    
    /**
     * هل الجهاز في وضع PiP؟ / Is in PiP mode?
     */
    fun isInPiPMode(): Boolean {
        return currentState == PiPState.ACTIVE
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التحكم
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الدخول لوضع PiP / Enter PiP mode
     */
    fun enterPiPMode(): Boolean {
        if (!isPiPSupported()) return false
        
        setState(PiPState.ENTERING)
        // في التطبيق الحقيقي: استدعاء enterPictureInPictureMode
        mainHandler.postDelayed({
            setState(PiPState.ACTIVE)
        }, 100)
        
        return true
    }
    
    /**
     * الخروج من وضع PiP / Exit PiP mode
     */
    fun exitPiPMode() {
        if (currentState != PiPState.ACTIVE) return
        
        setState(PiPState.EXITING)
        mainHandler.postDelayed({
            setState(PiPState.INACTIVE)
        }, 100)
    }
    
    private fun setState(state: PiPState) {
        currentState = state
        mainHandler.post { pipListener?.onPiPStateChanged(state) }
    }
    
    /**
     * الحصول على الحالة / Get state
     */
    fun getState(): PiPState = currentState
    
    // ═══════════════════════════════════════════════════════════════
    //                         الإجراءات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إضافة إجراء / Add action
     */
    fun addAction(action: PiPAction) {
        if (actions.size < 3) {  // حد أقصى 3 إجراءات
            actions.add(action)
        }
    }
    
    /**
     * مسح الإجراءات / Clear actions
     */
    fun clearActions() {
        actions.clear()
    }
    
    /**
     * الحصول على الإجراءات / Get actions
     */
    fun getActions(): List<PiPAction> = actions.toList()
    
    /**
     * إضافة إجراءات وسائط افتراضية / Add default media actions
     */
    fun addDefaultMediaActions(
        playIconResId: Int,
        pauseIconResId: Int,
        nextIconResId: Int,
        prevIconResId: Int,
        isPlaying: Boolean = true
    ) {
        clearActions()
        
        addAction(PiPAction("prev", "السابق", prevIconResId, 1))
        
        if (isPlaying) {
            addAction(PiPAction("pause", "إيقاف", pauseIconResId, 2))
        } else {
            addAction(PiPAction("play", "تشغيل", playIconResId, 2))
        }
        
        addAction(PiPAction("next", "التالي", nextIconResId, 3))
    }
    
    /**
     * معالجة نقر إجراء / Handle action click
     */
    fun handleActionClick(requestCode: Int) {
        val action = actions.find { it.requestCode == requestCode }
        action?.let {
            mainHandler.post { pipListener?.onPiPActionClicked(it.id) }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         المعلومات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على معلومات PiP / Get PiP info
     */
    fun getPiPInfo(): PiPInfo {
        return PiPInfo(
            state = currentState,
            isSupported = isPiPSupported(),
            currentActions = actions.toList(),
            config = config
        )
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        actions.clear()
        pipListener = null
        context = null
    }
}
