package sad.android.tv.headers

import android.content.Context
import android.os.Handler
import android.os.Looper

/**
 * جسر رؤوس التلفزيون - Headers and HeadersSupportFragment
 * TV Headers Bridge - Headers and HeadersSupportFragment
 * 
 * @author فريق لغة ص
 */
object SadTVHeadersBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    
    /**
     * عنصر الرأس / Header item
     */
    data class HeaderItem(
        val id: Long,
        val title: String,
        val iconResId: Int? = null,
        val description: String = "",
        val badge: String? = null,
        val isSelected: Boolean = false
    )
    
    /**
     * تكوين الرؤوس / Headers config
     */
    data class HeadersConfig(
        val enabled: Boolean = true,
        val position: HeadersPosition = HeadersPosition.LEFT,
        val width: Int = 250,
        val backgroundColor: Int = 0xFF1A1A1A.toInt(),
        val selectedColor: Int = 0xFF1E88E5.toInt(),
        val textColor: Int = 0xFFFFFFFF.toInt(),
        val textSize: Float = 18f,
        val showIcons: Boolean = true,
        val showDividers: Boolean = false,
        val animateFocus: Boolean = true
    )
    
    /**
     * موقع الرؤوس / Headers position
     */
    enum class HeadersPosition {
        LEFT,
        TOP,
        HIDDEN
    }
    
    /**
     * حالة الرؤوس / Headers state
     */
    enum class HeadersState {
        SHOWN,
        HIDDEN,
        DISABLED
    }
    
    /**
     * مستمع الرؤوس / Headers listener
     */
    interface HeadersListener {
        fun onHeaderSelected(header: HeaderItem)
        fun onHeaderClicked(header: HeaderItem)
        fun onHeadersVisibilityChanged(visible: Boolean)
    }
    
    private var headersListener: HeadersListener? = null
    private var config = HeadersConfig()
    private val headers = mutableListOf<HeaderItem>()
    private var selectedIndex = 0
    private var currentState = HeadersState.SHOWN
    
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
    fun setHeadersListener(listener: HeadersListener?) {
        headersListener = listener
    }
    
    /**
     * تعيين التكوين / Set config
     */
    fun setConfig(newConfig: HeadersConfig) {
        config = newConfig
        
        if (!config.enabled) {
            currentState = HeadersState.DISABLED
        }
    }
    
    /**
     * الحصول على التكوين / Get config
     */
    fun getConfig(): HeadersConfig = config
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة الرؤوس
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إضافة رأس / Add header
     */
    fun addHeader(header: HeaderItem) {
        headers.add(header)
        if (headers.size == 1) {
            selectHeader(0)
        }
    }
    
    /**
     * إضافة رأس بسيط / Add simple header
     */
    fun addHeader(id: Long, title: String, iconResId: Int? = null) {
        addHeader(HeaderItem(id, title, iconResId))
    }
    
    /**
     * تعيين الرؤوس / Set headers
     */
    fun setHeaders(newHeaders: List<HeaderItem>) {
        headers.clear()
        headers.addAll(newHeaders)
        if (headers.isNotEmpty()) {
            selectHeader(0)
        }
    }
    
    /**
     * الحصول على الرؤوس / Get headers
     */
    fun getHeaders(): List<HeaderItem> = headers.toList()
    
    /**
     * الحصول على رأس / Get header
     */
    fun getHeader(index: Int): HeaderItem? = headers.getOrNull(index)
    
    /**
     * الحصول على رأس بالمعرّف / Get header by id
     */
    fun getHeaderById(id: Long): HeaderItem? = headers.find { it.id == id }
    
    /**
     * عدد الرؤوس / Header count
     */
    fun getHeaderCount(): Int = headers.size
    
    /**
     * مسح الرؤوس / Clear headers
     */
    fun clearHeaders() {
        headers.clear()
        selectedIndex = 0
    }
    
    /**
     * تحديث رأس / Update header
     */
    fun updateHeader(id: Long, update: (HeaderItem) -> HeaderItem): Boolean {
        val index = headers.indexOfFirst { it.id == id }
        if (index < 0) return false
        
        headers[index] = update(headers[index])
        return true
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التحديد
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تحديد رأس / Select header
     */
    fun selectHeader(index: Int): Boolean {
        if (index !in 0 until headers.size) return false
        
        // إلغاء تحديد السابق
        if (selectedIndex in 0 until headers.size) {
            headers[selectedIndex] = headers[selectedIndex].copy(isSelected = false)
        }
        
        // تحديد الجديد
        selectedIndex = index
        headers[index] = headers[index].copy(isSelected = true)
        
        mainHandler.post { headersListener?.onHeaderSelected(headers[index]) }
        return true
    }
    
    /**
     * تحديد رأس بالمعرّف / Select header by id
     */
    fun selectHeaderById(id: Long): Boolean {
        val index = headers.indexOfFirst { it.id == id }
        return selectHeader(index)
    }
    
    /**
     * الحصول على الرأس المحدد / Get selected header
     */
    fun getSelectedHeader(): HeaderItem? = headers.getOrNull(selectedIndex)
    
    /**
     * الحصول على فهرس المحدد / Get selected index
     */
    fun getSelectedIndex(): Int = selectedIndex
    
    // ═══════════════════════════════════════════════════════════════
    //                         التنقل
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الرأس التالي / Next header
     */
    fun selectNext(): Boolean {
        return selectHeader(selectedIndex + 1)
    }
    
    /**
     * الرأس السابق / Previous header
     */
    fun selectPrevious(): Boolean {
        return selectHeader(selectedIndex - 1)
    }
    
    /**
     * النقر على المحدد / Click selected
     */
    fun clickSelected() {
        val header = getSelectedHeader() ?: return
        mainHandler.post { headersListener?.onHeaderClicked(header) }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         العرض
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إظهار الرؤوس / Show headers
     */
    fun showHeaders() {
        if (!config.enabled) return
        
        currentState = HeadersState.SHOWN
        mainHandler.post { headersListener?.onHeadersVisibilityChanged(true) }
    }
    
    /**
     * إخفاء الرؤوس / Hide headers
     */
    fun hideHeaders() {
        currentState = HeadersState.HIDDEN
        mainHandler.post { headersListener?.onHeadersVisibilityChanged(false) }
    }
    
    /**
     * تبديل العرض / Toggle visibility
     */
    fun toggleHeaders() {
        if (currentState == HeadersState.SHOWN) {
            hideHeaders()
        } else if (currentState == HeadersState.HIDDEN) {
            showHeaders()
        }
    }
    
    /**
     * الحصول على الحالة / Get state
     */
    fun getState(): HeadersState = currentState
    
    /**
     * هل ظاهرة؟ / Are visible?
     */
    fun isVisible(): Boolean = currentState == HeadersState.SHOWN
    
    // ═══════════════════════════════════════════════════════════════
    //                         رؤوس افتراضية
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إنشاء رؤوس افتراضية / Create default headers
     */
    fun createDefaultHeaders() {
        clearHeaders()
        
        addHeader(HeaderItem(0, "الرئيسية", description = "الصفحة الرئيسية"))
        addHeader(HeaderItem(1, "أفلام", description = "جميع الأفلام"))
        addHeader(HeaderItem(2, "مسلسلات", description = "جميع المسلسلات"))
        addHeader(HeaderItem(3, "أطفال", description = "محتوى الأطفال"))
        addHeader(HeaderItem(4, "وثائقي", description = "الأفلام الوثائقية"))
        addHeader(HeaderItem(5, "الإعدادات", description = "إعدادات التطبيق"))
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        headers.clear()
        headersListener = null
        context = null
    }
}
