package sad.android.tv.onboarding

import android.content.Context
import android.os.Handler
import android.os.Looper

/**
 * جسر شاشة الترحيب التلفزيونية - OnboardingSupportFragment
 * TV Onboarding Bridge - OnboardingSupportFragment
 * 
 * @author فريق لغة ص
 */
object SadTVOnboardingBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    
    /**
     * صفحة الترحيب / Onboarding page
     */
    data class OnboardingPage(
        val id: Int,
        val title: String,
        val description: String,
        val imageResId: Int? = null,
        val imageUrl: String? = null,
        val backgroundColor: Int = 0xFF1E88E5.toInt(),
        val animationType: AnimationType = AnimationType.FADE
    )
    
    /**
     * نوع الرسوم المتحركة / Animation type
     */
    enum class AnimationType {
        FADE,
        SLIDE_LEFT,
        SLIDE_RIGHT,
        SLIDE_UP,
        ZOOM,
        NONE
    }
    
    /**
     * تكوين الترحيب / Onboarding config
     */
    data class OnboardingConfig(
        val logoResId: Int? = null,
        val logoUrl: String? = null,
        val showStartButton: Boolean = true,
        val startButtonText: String = "ابدأ الآن",
        val showSkipButton: Boolean = false,
        val skipButtonText: String = "تخطي",
        val primaryColor: Int = 0xFF1E88E5.toInt(),
        val indicatorColor: Int = 0xFFFFFFFF.toInt(),
        val indicatorSelectedColor: Int = 0xFF1E88E5.toInt()
    )
    
    /**
     * مستمع الترحيب / Onboarding listener
     */
    interface OnboardingListener {
        fun onPageChanged(pageIndex: Int)
        fun onFinished()
        fun onSkipped()
    }
    
    private val pages = mutableListOf<OnboardingPage>()
    private var config = OnboardingConfig()
    private var onboardingListener: OnboardingListener? = null
    private var currentPageIndex = 0
    
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
    fun setConfig(newConfig: OnboardingConfig) {
        config = newConfig
    }
    
    /**
     * الحصول على التكوين / Get config
     */
    fun getConfig(): OnboardingConfig = config
    
    /**
     * تعيين المستمع / Set listener
     */
    fun setOnboardingListener(listener: OnboardingListener?) {
        onboardingListener = listener
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة الصفحات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إضافة صفحة / Add page
     */
    fun addPage(page: OnboardingPage) {
        pages.add(page)
    }
    
    /**
     * إضافة صفحة بسيطة / Add simple page
     */
    fun addSimplePage(title: String, description: String, imageResId: Int? = null) {
        pages.add(
            OnboardingPage(
                id = pages.size,
                title = title,
                description = description,
                imageResId = imageResId
            )
        )
    }
    
    /**
     * إضافة صفحات / Add pages
     */
    fun addPages(pageList: List<OnboardingPage>) {
        pages.addAll(pageList)
    }
    
    /**
     * الحصول على جميع الصفحات / Get all pages
     */
    fun getPages(): List<OnboardingPage> = pages.toList()
    
    /**
     * عدد الصفحات / Page count
     */
    fun getPageCount(): Int = pages.size
    
    /**
     * مسح الصفحات / Clear pages
     */
    fun clearPages() {
        pages.clear()
        currentPageIndex = 0
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التنقل
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الصفحة التالية / Next page
     */
    fun nextPage(): Boolean {
        if (currentPageIndex < pages.size - 1) {
            currentPageIndex++
            mainHandler.post { onboardingListener?.onPageChanged(currentPageIndex) }
            return true
        }
        return false
    }
    
    /**
     * الصفحة السابقة / Previous page
     */
    fun previousPage(): Boolean {
        if (currentPageIndex > 0) {
            currentPageIndex--
            mainHandler.post { onboardingListener?.onPageChanged(currentPageIndex) }
            return true
        }
        return false
    }
    
    /**
     * الانتقال لصفحة / Go to page
     */
    fun goToPage(index: Int): Boolean {
        if (index in 0 until pages.size) {
            currentPageIndex = index
            mainHandler.post { onboardingListener?.onPageChanged(currentPageIndex) }
            return true
        }
        return false
    }
    
    /**
     * الحصول على الصفحة الحالية / Get current page
     */
    fun getCurrentPage(): OnboardingPage? = pages.getOrNull(currentPageIndex)
    
    /**
     * الحصول على فهرس الصفحة الحالية / Get current page index
     */
    fun getCurrentPageIndex(): Int = currentPageIndex
    
    /**
     * هل هذه الصفحة الأخيرة؟ / Is last page?
     */
    fun isLastPage(): Boolean = currentPageIndex == pages.size - 1
    
    /**
     * هل هذه الصفحة الأولى؟ / Is first page?
     */
    fun isFirstPage(): Boolean = currentPageIndex == 0
    
    // ═══════════════════════════════════════════════════════════════
    //                         الإكمال
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إنهاء الترحيب / Finish onboarding
     */
    fun finish() {
        setOnboardingCompleted(true)
        mainHandler.post { onboardingListener?.onFinished() }
    }
    
    /**
     * تخطي الترحيب / Skip onboarding
     */
    fun skip() {
        setOnboardingCompleted(true)
        mainHandler.post { onboardingListener?.onSkipped() }
    }
    
    /**
     * هل اكتمل الترحيب؟ / Is onboarding completed?
     */
    fun isOnboardingCompleted(): Boolean {
        val prefs = context?.getSharedPreferences("sad_tv_onboarding", Context.MODE_PRIVATE)
        return prefs?.getBoolean("completed", false) ?: false
    }
    
    /**
     * تعيين حالة الإكمال / Set completed state
     */
    fun setOnboardingCompleted(completed: Boolean) {
        val prefs = context?.getSharedPreferences("sad_tv_onboarding", Context.MODE_PRIVATE)
        prefs?.edit()?.putBoolean("completed", completed)?.apply()
    }
    
    /**
     * إعادة تعيين الترحيب / Reset onboarding
     */
    fun reset() {
        setOnboardingCompleted(false)
        currentPageIndex = 0
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         نماذج جاهزة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إنشاء صفحات ترحيب افتراضية / Create default onboarding pages
     */
    fun createDefaultPages(): List<OnboardingPage> {
        return listOf(
            OnboardingPage(
                id = 0,
                title = "مرحباً بك",
                description = "اكتشف عالماً من الترفيه على شاشتك الكبيرة",
                backgroundColor = 0xFF1E88E5.toInt()
            ),
            OnboardingPage(
                id = 1,
                title = "تصفح بسهولة",
                description = "استخدم جهاز التحكم للتنقل بين المحتوى واختيار ما تفضله",
                backgroundColor = 0xFF43A047.toInt()
            ),
            OnboardingPage(
                id = 2,
                title = "شاهد في أي وقت",
                description = "استأنف المشاهدة من حيث توقفت على أي جهاز",
                backgroundColor = 0xFFE53935.toInt()
            ),
            OnboardingPage(
                id = 3,
                title = "ابدأ الآن",
                description = "استمتع بأفضل المحتوى العربي والعالمي",
                backgroundColor = 0xFF8E24AA.toInt()
            )
        )
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        pages.clear()
        onboardingListener = null
        context = null
    }
}
