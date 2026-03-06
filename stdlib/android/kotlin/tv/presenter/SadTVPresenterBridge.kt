package sad.android.tv.presenter

import android.content.Context
import android.os.Handler
import android.os.Looper

/**
 * جسر المقدمات التلفزيونية - Presenter و PresenterSelector
 * TV Presenter Bridge - Presenter and PresenterSelector
 * 
 * @author فريق لغة ص
 */
object SadTVPresenterBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private val presenters = mutableMapOf<String, PresenterConfig>()
    
    /**
     * نوع المقدم / Presenter type
     */
    enum class PresenterType {
        IMAGE_CARD,
        TEXT_CARD,
        ICON_CARD,
        LIST_ROW,
        CLASS_ROW,
        FULL_WIDTH,
        CUSTOM
    }
    
    /**
     * تكوين المقدم / Presenter config
     */
    data class PresenterConfig(
        val id: String,
        val type: PresenterType,
        val itemWidth: Int = 313,
        val itemHeight: Int = 176,
        val selectedScale: Float = 1.1f,
        val selectedElevation: Float = 8f,
        val cornerRadius: Float = 8f,
        val titleColor: Int = 0xFF212121.toInt(),
        val contentColor: Int = 0xFF757575.toInt(),
        val backgroundColor: Int = 0xFFFFFFFF.toInt(),
        val showTitle: Boolean = true,
        val showContent: Boolean = true,
        val showBadge: Boolean = false,
        val animationDuration: Long = 200
    )
    
    /**
     * عنصر المقدم / Presenter item
     */
    data class PresenterItem(
        val id: String,
        val title: String,
        val content: String = "",
        val imageUrl: String = "",
        val iconResId: Int? = null,
        val badge: String? = null,
        val data: Any? = null
    )
    
    /**
     * مستمع المقدم / Presenter listener
     */
    interface PresenterListener {
        fun onItemBound(item: PresenterItem, position: Int)
        fun onItemUnbound(item: PresenterItem, position: Int)
        fun onItemFocused(item: PresenterItem)
        fun onItemClicked(item: PresenterItem)
    }
    
    private var presenterListener: PresenterListener? = null
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        registerDefaultPresenters()
    }
    
    /**
     * تعيين المستمع / Set listener
     */
    fun setPresenterListener(listener: PresenterListener?) {
        presenterListener = listener
    }
    
    private fun registerDefaultPresenters() {
        // مقدم بطاقة الصورة الافتراضي
        registerPresenter(
            PresenterConfig(
                id = "default_image_card",
                type = PresenterType.IMAGE_CARD,
                itemWidth = 313,
                itemHeight = 176
            )
        )
        
        // مقدم بطاقة الفيلم
        registerPresenter(
            PresenterConfig(
                id = "movie_card",
                type = PresenterType.IMAGE_CARD,
                itemWidth = 180,
                itemHeight = 260
            )
        )
        
        // مقدم بطاقة المسلسل
        registerPresenter(
            PresenterConfig(
                id = "series_card",
                type = PresenterType.IMAGE_CARD,
                itemWidth = 313,
                itemHeight = 176
            )
        )
        
        // مقدم الأيقونة
        registerPresenter(
            PresenterConfig(
                id = "icon_card",
                type = PresenterType.ICON_CARD,
                itemWidth = 120,
                itemHeight = 120,
                showContent = false
            )
        )
        
        // مقدم العرض الكامل
        registerPresenter(
            PresenterConfig(
                id = "full_width",
                type = PresenterType.FULL_WIDTH,
                itemWidth = 0,  // يأخذ العرض الكامل
                itemHeight = 400
            )
        )
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة المقدمات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تسجيل مقدم / Register presenter
     */
    fun registerPresenter(config: PresenterConfig) {
        presenters[config.id] = config
    }
    
    /**
     * الحصول على مقدم / Get presenter
     */
    fun getPresenter(id: String): PresenterConfig? = presenters[id]
    
    /**
     * الحصول على جميع المقدمات / Get all presenters
     */
    fun getAllPresenters(): List<PresenterConfig> = presenters.values.toList()
    
    /**
     * إزالة مقدم / Remove presenter
     */
    fun removePresenter(id: String): Boolean = presenters.remove(id) != null
    
    /**
     * تحديث مقدم / Update presenter
     */
    fun updatePresenter(id: String, update: (PresenterConfig) -> PresenterConfig): Boolean {
        val current = presenters[id] ?: return false
        presenters[id] = update(current)
        return true
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         اختيار المقدم
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * اختيار مقدم للعنصر / Select presenter for item
     */
    fun selectPresenter(item: PresenterItem): PresenterConfig {
        // منطق الاختيار الافتراضي
        return when {
            item.iconResId != null -> presenters["icon_card"]!!
            item.imageUrl.isNotEmpty() -> presenters["default_image_card"]!!
            else -> presenters["default_image_card"]!!
        }
    }
    
    /**
     * اختيار مقدم حسب الفئة / Select presenter by category
     */
    fun selectPresenterByCategory(category: String): PresenterConfig {
        return when (category) {
            "movies" -> presenters["movie_card"]!!
            "series" -> presenters["series_card"]!!
            "icons" -> presenters["icon_card"]!!
            "featured" -> presenters["full_width"]!!
            else -> presenters["default_image_card"]!!
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         أحداث دورة الحياة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * العنصر مربوط / Item bound
     */
    fun notifyItemBound(item: PresenterItem, position: Int) {
        mainHandler.post { presenterListener?.onItemBound(item, position) }
    }
    
    /**
     * العنصر غير مربوط / Item unbound
     */
    fun notifyItemUnbound(item: PresenterItem, position: Int) {
        mainHandler.post { presenterListener?.onItemUnbound(item, position) }
    }
    
    /**
     * التركيز على العنصر / Item focused
     */
    fun notifyItemFocused(item: PresenterItem) {
        mainHandler.post { presenterListener?.onItemFocused(item) }
    }
    
    /**
     * النقر على العنصر / Item clicked
     */
    fun notifyItemClicked(item: PresenterItem) {
        mainHandler.post { presenterListener?.onItemClicked(item) }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         أنماط جاهزة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * نمط Netflix / Netflix style
     */
    fun createNetflixPresenter(): PresenterConfig {
        return PresenterConfig(
            id = "netflix_style",
            type = PresenterType.IMAGE_CARD,
            itemWidth = 180,
            itemHeight = 270,
            selectedScale = 1.15f,
            backgroundColor = 0xFF141414.toInt(),
            titleColor = 0xFFFFFFFF.toInt(),
            contentColor = 0xFF808080.toInt()
        )
    }
    
    /**
     * نمط YouTube / YouTube style
     */
    fun createYouTubePresenter(): PresenterConfig {
        return PresenterConfig(
            id = "youtube_style",
            type = PresenterType.IMAGE_CARD,
            itemWidth = 320,
            itemHeight = 180,
            selectedScale = 1.05f,
            cornerRadius = 12f,
            backgroundColor = 0xFF212121.toInt(),
            titleColor = 0xFFFFFFFF.toInt()
        )
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        presenters.clear()
        presenterListener = null
        context = null
    }
}
