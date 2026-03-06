package sad.android.tv.error

import android.content.Context
import android.os.Handler
import android.os.Looper

/**
 * جسر الأخطاء التلفزيونية - ErrorSupportFragment
 * TV Error Bridge - ErrorSupportFragment
 * 
 * @author فريق لغة ص
 */
object SadTVErrorBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    
    /**
     * نوع الخطأ / Error type
     */
    enum class ErrorType {
        NETWORK,
        SERVER,
        NOT_FOUND,
        PERMISSION,
        PLAYBACK,
        SUBSCRIPTION,
        MAINTENANCE,
        GENERIC
    }
    
    /**
     * معلومات الخطأ / Error info
     */
    data class ErrorInfo(
        val type: ErrorType,
        val title: String,
        val message: String,
        val iconResId: Int? = null,
        val errorCode: String? = null,
        val canRetry: Boolean = true,
        val retryDelay: Long = 0,
        val actions: List<ErrorAction> = emptyList()
    )
    
    /**
     * إجراء الخطأ / Error action
     */
    data class ErrorAction(
        val id: String,
        val label: String,
        val iconResId: Int? = null,
        val isPrimary: Boolean = false
    )
    
    /**
     * تكوين الخطأ / Error config
     */
    data class ErrorConfig(
        val backgroundColor: Int = 0xFF1A1A1A.toInt(),
        val titleColor: Int = 0xFFFFFFFF.toInt(),
        val messageColor: Int = 0xFFCCCCCC.toInt(),
        val iconTint: Int = 0xFFFF5252.toInt(),
        val autoRetry: Boolean = false,
        val autoRetryDelay: Long = 5000,
        val maxAutoRetries: Int = 3
    )
    
    /**
     * مستمع الخطأ / Error listener
     */
    interface ErrorListener {
        fun onActionClicked(action: ErrorAction)
        fun onRetry()
        fun onDismiss()
    }
    
    private var errorListener: ErrorListener? = null
    private var config = ErrorConfig()
    private var currentError: ErrorInfo? = null
    private var autoRetryCount = 0
    
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
    fun setErrorListener(listener: ErrorListener?) {
        errorListener = listener
    }
    
    /**
     * تعيين التكوين / Set config
     */
    fun setConfig(newConfig: ErrorConfig) {
        config = newConfig
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         عرض الخطأ
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * عرض خطأ / Show error
     */
    fun showError(error: ErrorInfo) {
        currentError = error
        autoRetryCount = 0
        
        // بدء المحاولة التلقائية إذا فعّلة
        if (config.autoRetry && error.canRetry) {
            scheduleAutoRetry()
        }
    }
    
    /**
     * عرض خطأ بسيط / Show simple error
     */
    fun showError(type: ErrorType, title: String, message: String) {
        showError(ErrorInfo(
            type = type,
            title = title,
            message = message,
            actions = if (getDefaultError(type).canRetry) {
                listOf(ErrorAction("retry", "إعادة المحاولة", isPrimary = true))
            } else emptyList()
        ))
    }
    
    /**
     * إخفاء الخطأ / Hide error
     */
    fun hideError() {
        currentError = null
        mainHandler.post { errorListener?.onDismiss() }
    }
    
    /**
     * الحصول على الخطأ الحالي / Get current error
     */
    fun getCurrentError(): ErrorInfo? = currentError
    
    /**
     * هل يوجد خطأ؟ / Has error?
     */
    fun hasError(): Boolean = currentError != null
    
    // ═══════════════════════════════════════════════════════════════
    //                         إعادة المحاولة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إعادة المحاولة / Retry
     */
    fun retry() {
        mainHandler.post { errorListener?.onRetry() }
    }
    
    private fun scheduleAutoRetry() {
        if (autoRetryCount >= config.maxAutoRetries) return
        
        mainHandler.postDelayed({
            if (currentError != null) {
                autoRetryCount++
                retry()
            }
        }, config.autoRetryDelay)
    }
    
    /**
     * إعادة تعيين المحاولات / Reset retry count
     */
    fun resetRetryCount() {
        autoRetryCount = 0
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الإجراءات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * نقر إجراء / Click action
     */
    fun clickAction(actionId: String) {
        val action = currentError?.actions?.find { it.id == actionId } ?: return
        
        if (actionId == "retry") {
            retry()
        } else {
            mainHandler.post { errorListener?.onActionClicked(action) }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         أخطاء افتراضية
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على خطأ افتراضي / Get default error
     */
    fun getDefaultError(type: ErrorType): ErrorInfo {
        return when (type) {
            ErrorType.NETWORK -> ErrorInfo(
                type = ErrorType.NETWORK,
                title = "لا يوجد اتصال بالإنترنت",
                message = "يرجى التحقق من اتصالك بالإنترنت والمحاولة مرة أخرى",
                canRetry = true,
                actions = listOf(
                    ErrorAction("retry", "إعادة المحاولة", isPrimary = true),
                    ErrorAction("settings", "إعدادات الشبكة")
                )
            )
            
            ErrorType.SERVER -> ErrorInfo(
                type = ErrorType.SERVER,
                title = "خطأ في الخادم",
                message = "حدث خطأ في الخادم، يرجى المحاولة لاحقاً",
                canRetry = true,
                retryDelay = 5000,
                actions = listOf(ErrorAction("retry", "إعادة المحاولة", isPrimary = true))
            )
            
            ErrorType.NOT_FOUND -> ErrorInfo(
                type = ErrorType.NOT_FOUND,
                title = "المحتوى غير موجود",
                message = "لم نتمكن من العثور على المحتوى المطلوب",
                canRetry = false,
                actions = listOf(ErrorAction("home", "العودة للرئيسية", isPrimary = true))
            )
            
            ErrorType.PERMISSION -> ErrorInfo(
                type = ErrorType.PERMISSION,
                title = "غير مصرح",
                message = "ليس لديك الصلاحية للوصول لهذا المحتوى",
                canRetry = false,
                actions = listOf(
                    ErrorAction("login", "تسجيل الدخول", isPrimary = true),
                    ErrorAction("home", "العودة للرئيسية")
                )
            )
            
            ErrorType.PLAYBACK -> ErrorInfo(
                type = ErrorType.PLAYBACK,
                title = "خطأ في التشغيل",
                message = "لم نتمكن من تشغيل هذا المحتوى",
                canRetry = true,
                actions = listOf(
                    ErrorAction("retry", "إعادة المحاولة", isPrimary = true),
                    ErrorAction("quality", "تغيير الجودة")
                )
            )
            
            ErrorType.SUBSCRIPTION -> ErrorInfo(
                type = ErrorType.SUBSCRIPTION,
                title = "اشتراك مطلوب",
                message = "هذا المحتوى يتطلب اشتراكاً مدفوعاً",
                canRetry = false,
                actions = listOf(
                    ErrorAction("subscribe", "اشترك الآن", isPrimary = true),
                    ErrorAction("home", "العودة للرئيسية")
                )
            )
            
            ErrorType.MAINTENANCE -> ErrorInfo(
                type = ErrorType.MAINTENANCE,
                title = "صيانة مجدولة",
                message = "الخدمة تحت الصيانة، يرجى المحاولة لاحقاً",
                canRetry = true,
                retryDelay = 60000,
                actions = listOf(ErrorAction("retry", "إعادة المحاولة", isPrimary = true))
            )
            
            ErrorType.GENERIC -> ErrorInfo(
                type = ErrorType.GENERIC,
                title = "حدث خطأ",
                message = "حدث خطأ غير متوقع، يرجى المحاولة مرة أخرى",
                canRetry = true,
                actions = listOf(
                    ErrorAction("retry", "إعادة المحاولة", isPrimary = true),
                    ErrorAction("home", "العودة للرئيسية")
                )
            )
        }
    }
    
    /**
     * عرض خطأ افتراضي / Show default error
     */
    fun showDefaultError(type: ErrorType) {
        showError(getDefaultError(type))
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        hideError()
        errorListener = null
        context = null
    }
}
