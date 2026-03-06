package sad.android.tv.loading

import android.content.Context
import android.os.Handler
import android.os.Looper

/**
 * جسر التحميل التلفزيوني - Loading states and spinners
 * TV Loading Bridge - Loading states and spinners
 * 
 * @author فريق لغة ص
 */
object SadTVLoadingBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    
    /**
     * نوع التحميل / Loading type
     */
    enum class LoadingType {
        SPINNER,
        PROGRESS_BAR,
        DOTS,
        SKELETON,
        SHIMMER,
        CUSTOM
    }
    
    /**
     * حالة التحميل / Loading state
     */
    enum class LoadingState {
        IDLE,
        LOADING,
        SUCCESS,
        ERROR,
        EMPTY
    }
    
    /**
     * تكوين التحميل / Loading config
     */
    data class LoadingConfig(
        val type: LoadingType = LoadingType.SPINNER,
        val message: String = "جاري التحميل...",
        val showMessage: Boolean = true,
        val spinnerSize: SpinnerSize = SpinnerSize.LARGE,
        val spinnerColor: Int = 0xFF1E88E5.toInt(),
        val backgroundColor: Int = 0xCC000000.toInt(),
        val overlayEnabled: Boolean = true,
        val cancellable: Boolean = true,
        val timeout: Long = 30000, // 30 ثانية
        val minDisplayTime: Long = 500 // الحد الأدنى للعرض
    )
    
    /**
     * حجم المؤشر / Spinner size
     */
    enum class SpinnerSize {
        SMALL,
        MEDIUM,
        LARGE,
        EXTRA_LARGE
    }
    
    /**
     * معلومات التحميل / Loading info
     */
    data class LoadingInfo(
        val id: String,
        val state: LoadingState,
        val progress: Int = 0,
        val message: String = "",
        val startTime: Long = System.currentTimeMillis()
    )
    
    /**
     * مستمع التحميل / Loading listener
     */
    interface LoadingListener {
        fun onStateChanged(id: String, state: LoadingState)
        fun onProgressChanged(id: String, progress: Int)
        fun onTimeout(id: String)
        fun onCancelled(id: String)
    }
    
    private var loadingListener: LoadingListener? = null
    private var config = LoadingConfig()
    private val loadingStates = mutableMapOf<String, LoadingInfo>()
    private var globalLoading: LoadingInfo? = null
    
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
    fun setLoadingListener(listener: LoadingListener?) {
        loadingListener = listener
    }
    
    /**
     * تعيين التكوين / Set config
     */
    fun setConfig(newConfig: LoadingConfig) {
        config = newConfig
    }
    
    /**
     * الحصول على التكوين / Get config
     */
    fun getConfig(): LoadingConfig = config
    
    // ═══════════════════════════════════════════════════════════════
    //                         التحميل العام
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * عرض التحميل العام / Show global loading
     */
    fun showLoading(message: String = config.message) {
        globalLoading = LoadingInfo(
            id = "global",
            state = LoadingState.LOADING,
            message = message
        )
        
        notifyStateChanged("global", LoadingState.LOADING)
        
        // بدء timeout
        if (config.timeout > 0) {
            mainHandler.postDelayed({
                if (globalLoading?.state == LoadingState.LOADING) {
                    timeout("global")
                }
            }, config.timeout)
        }
    }
    
    /**
     * إخفاء التحميل العام / Hide global loading
     */
    fun hideLoading(state: LoadingState = LoadingState.IDLE) {
        val loading = globalLoading ?: return
        
        // احترام الحد الأدنى للعرض
        val elapsed = System.currentTimeMillis() - loading.startTime
        val delay = if (elapsed < config.minDisplayTime) config.minDisplayTime - elapsed else 0
        
        mainHandler.postDelayed({
            globalLoading = globalLoading?.copy(state = state)
            notifyStateChanged("global", state)
        }, delay)
    }
    
    /**
     * هل التحميل نشط؟ / Is loading active?
     */
    fun isLoading(): Boolean = globalLoading?.state == LoadingState.LOADING
    
    // ═══════════════════════════════════════════════════════════════
    //                         التحميل المسمى
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * بدء تحميل مسمى / Start named loading
     */
    fun startLoading(id: String, message: String = ""): LoadingInfo {
        val info = LoadingInfo(
            id = id,
            state = LoadingState.LOADING,
            message = message
        )
        
        loadingStates[id] = info
        notifyStateChanged(id, LoadingState.LOADING)
        
        // بدء timeout
        if (config.timeout > 0) {
            mainHandler.postDelayed({
                if (loadingStates[id]?.state == LoadingState.LOADING) {
                    timeout(id)
                }
            }, config.timeout)
        }
        
        return info
    }
    
    /**
     * إنهاء تحميل مسمى / Stop named loading
     */
    fun stopLoading(id: String, state: LoadingState = LoadingState.SUCCESS) {
        val loading = loadingStates[id] ?: return
        
        val elapsed = System.currentTimeMillis() - loading.startTime
        val delay = if (elapsed < config.minDisplayTime) config.minDisplayTime - elapsed else 0
        
        mainHandler.postDelayed({
            loadingStates[id] = loadingStates[id]?.copy(state = state)
            notifyStateChanged(id, state)
        }, delay)
    }
    
    /**
     * تحديث التقدم / Update progress
     */
    fun updateProgress(id: String, progress: Int) {
        loadingStates[id] = loadingStates[id]?.copy(progress = progress.coerceIn(0, 100))
        mainHandler.post { loadingListener?.onProgressChanged(id, progress) }
    }
    
    /**
     * إلغاء التحميل / Cancel loading
     */
    fun cancelLoading(id: String) {
        loadingStates[id] = loadingStates[id]?.copy(state = LoadingState.IDLE)
        mainHandler.post { loadingListener?.onCancelled(id) }
    }
    
    /**
     * timeout
     */
    private fun timeout(id: String) {
        loadingStates[id] = loadingStates[id]?.copy(state = LoadingState.ERROR)
        mainHandler.post { loadingListener?.onTimeout(id) }
    }
    
    /**
     * الحصول على معلومات التحميل / Get loading info
     */
    fun getLoadingInfo(id: String): LoadingInfo? = loadingStates[id]
    
    /**
     * هل تحميل معين نشط؟ / Is specific loading active?
     */
    fun isLoading(id: String): Boolean = loadingStates[id]?.state == LoadingState.LOADING
    
    private fun notifyStateChanged(id: String, state: LoadingState) {
        mainHandler.post { loadingListener?.onStateChanged(id, state) }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         رسائل جاهزة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * عرض تحميل المحتوى / Show content loading
     */
    fun showContentLoading() {
        showLoading("جاري تحميل المحتوى...")
    }
    
    /**
     * عرض تحميل البحث / Show search loading
     */
    fun showSearchLoading() {
        showLoading("جاري البحث...")
    }
    
    /**
     * عرض تحميل الفيديو / Show video loading
     */
    fun showVideoLoading() {
        showLoading("جاري تجهيز الفيديو...")
    }
    
    /**
     * عرض تحميل الصفحة / Show page loading
     */
    fun showPageLoading() {
        showLoading("جاري تحميل الصفحة...")
    }
    
    /**
     * عرض تحميل تسجيل الدخول / Show login loading
     */
    fun showLoginLoading() {
        showLoading("جاري تسجيل الدخول...")
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         Skeleton Loading
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تكوين Skeleton
     */
    data class SkeletonConfig(
        val rows: Int = 5,
        val hasImage: Boolean = true,
        val imageRatio: Float = 16f / 9f,
        val animationDuration: Long = 1500,
        val baseColor: Int = 0xFF2A2A2A.toInt(),
        val highlightColor: Int = 0xFF3A3A3A.toInt()
    )
    
    private var skeletonConfig = SkeletonConfig()
    
    /**
     * تعيين تكوين Skeleton / Set skeleton config
     */
    fun setSkeletonConfig(config: SkeletonConfig) {
        skeletonConfig = config
    }
    
    /**
     * الحصول على تكوين Skeleton / Get skeleton config
     */
    fun getSkeletonConfig(): SkeletonConfig = skeletonConfig
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        loadingStates.clear()
        globalLoading = null
        loadingListener = null
        context = null
    }
}
