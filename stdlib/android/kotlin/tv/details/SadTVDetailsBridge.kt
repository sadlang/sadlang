package sad.android.tv.details

import android.content.Context
import android.os.Handler
import android.os.Looper

/**
 * جسر تفاصيل التلفزيون - DetailsFragment لعرض تفاصيل المحتوى
 * TV Details Bridge - DetailsFragment for content details
 * 
 * @author فريق لغة ص
 */
object SadTVDetailsBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    
    /**
     * تفاصيل المحتوى / Content details
     */
    data class ContentDetails(
        val id: String,
        val title: String,
        val description: String,
        val longDescription: String = "",
        val imageUrl: String = "",
        val backgroundUrl: String = "",
        val category: String = "",
        val year: Int? = null,
        val duration: String? = null,
        val rating: String? = null,
        val ratingValue: Float? = null,
        val studio: String? = null,
        val director: String? = null,
        val actors: List<String> = emptyList(),
        val genres: List<String> = emptyList(),
        val releaseDate: String? = null,
        val contentType: ContentType = ContentType.MOVIE,
        val seasonCount: Int? = null,
        val episodeCount: Int? = null,
        val currentSeason: Int? = null,
        val currentEpisode: Int? = null,
        val progress: Float = 0f,       // 0-1
        val isFavorite: Boolean = false,
        val isWatchlisted: Boolean = false
    )
    
    /**
     * نوع المحتوى / Content type
     */
    enum class ContentType {
        MOVIE,
        TV_SERIES,
        EPISODE,
        CLIP,
        LIVE,
        DOCUMENTARY
    }
    
    /**
     * إجراء التفاصيل / Details action
     */
    data class DetailsAction(
        val id: String,
        val label: String,
        val iconResId: Int? = null,
        val subLabel: String? = null,
        val isFocused: Boolean = false
    )
    
    /**
     * عنصر ذو صلة / Related item
     */
    data class RelatedItem(
        val id: String,
        val title: String,
        val imageUrl: String,
        val category: String = "",
        val description: String = ""
    )
    
    /**
     * تكوين التفاصيل / Details config
     */
    data class DetailsConfig(
        val showDescription: Boolean = true,
        val showRelated: Boolean = true,
        val showCast: Boolean = true,
        val showActions: Boolean = true,
        val actionsBackgroundColor: Int = 0xFF1E88E5.toInt(),
        val detailsBackgroundColor: Int = 0xFF212121.toInt(),
        val logoTransitionEnabled: Boolean = true
    )
    
    /**
     * مستمع التفاصيل / Details listener
     */
    interface DetailsListener {
        fun onActionClicked(action: DetailsAction)
        fun onRelatedItemClicked(item: RelatedItem)
        fun onPlayClicked()
        fun onFavoriteToggled(isFavorite: Boolean)
    }
    
    private var currentDetails: ContentDetails? = null
    private var actions = mutableListOf<DetailsAction>()
    private var relatedItems = mutableListOf<RelatedItem>()
    private var config = DetailsConfig()
    private var detailsListener: DetailsListener? = null
    
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
    fun setConfig(newConfig: DetailsConfig) {
        config = newConfig
    }
    
    /**
     * تعيين المستمع / Set listener
     */
    fun setDetailsListener(listener: DetailsListener?) {
        detailsListener = listener
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة التفاصيل
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تعيين التفاصيل / Set details
     */
    fun setDetails(details: ContentDetails) {
        currentDetails = details
    }
    
    /**
     * الحصول على التفاصيل / Get details
     */
    fun getDetails(): ContentDetails? = currentDetails
    
    /**
     * مسح التفاصيل / Clear details
     */
    fun clearDetails() {
        currentDetails = null
        actions.clear()
        relatedItems.clear()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة الإجراءات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إضافة إجراء / Add action
     */
    fun addAction(action: DetailsAction) {
        actions.add(action)
    }
    
    /**
     * إضافة إجراءات افتراضية / Add default actions
     */
    fun addDefaultActions() {
        val details = currentDetails
        
        // زر التشغيل
        val playLabel = if (details?.progress ?: 0f > 0f) "استكمال المشاهدة" else "تشغيل"
        addAction(DetailsAction("play", playLabel))
        
        // قائمة المشاهدة
        val watchlistLabel = if (details?.isWatchlisted == true) "إزالة من القائمة" else "أضف للقائمة"
        addAction(DetailsAction("watchlist", watchlistLabel))
        
        // المفضلة
        val favoriteLabel = if (details?.isFavorite == true) "إزالة من المفضلة" else "أضف للمفضلة"
        addAction(DetailsAction("favorite", favoriteLabel))
        
        // مشاركة
        addAction(DetailsAction("share", "مشاركة"))
    }
    
    /**
     * الحصول على الإجراءات / Get actions
     */
    fun getActions(): List<DetailsAction> = actions.toList()
    
    /**
     * مسح الإجراءات / Clear actions
     */
    fun clearActions() {
        actions.clear()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         العناصر ذات الصلة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إضافة عنصر ذو صلة / Add related item
     */
    fun addRelatedItem(item: RelatedItem) {
        relatedItems.add(item)
    }
    
    /**
     * إضافة عناصر ذات صلة / Add related items
     */
    fun addRelatedItems(items: List<RelatedItem>) {
        relatedItems.addAll(items)
    }
    
    /**
     * الحصول على العناصر ذات الصلة / Get related items
     */
    fun getRelatedItems(): List<RelatedItem> = relatedItems.toList()
    
    /**
     * مسح العناصر ذات الصلة / Clear related items
     */
    fun clearRelatedItems() {
        relatedItems.clear()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         أحداث التفاعل
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إعلام بالنقر على إجراء / Notify action clicked
     */
    fun notifyActionClicked(actionId: String) {
        val action = actions.find { it.id == actionId } ?: return
        
        mainHandler.post {
            when (actionId) {
                "play" -> detailsListener?.onPlayClicked()
                "favorite" -> {
                    val newFavorite = !(currentDetails?.isFavorite ?: false)
                    detailsListener?.onFavoriteToggled(newFavorite)
                }
                else -> detailsListener?.onActionClicked(action)
            }
        }
    }
    
    /**
     * إعلام بالنقر على عنصر ذو صلة / Notify related item clicked
     */
    fun notifyRelatedItemClicked(itemId: String) {
        val item = relatedItems.find { it.id == itemId } ?: return
        mainHandler.post { detailsListener?.onRelatedItemClicked(item) }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التنسيق
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تنسيق المدة / Format duration
     */
    fun formatDuration(minutes: Int): String {
        val hours = minutes / 60
        val mins = minutes % 60
        return if (hours > 0) {
            "${hours} ساعة ${mins} دقيقة"
        } else {
            "${mins} دقيقة"
        }
    }
    
    /**
     * تنسيق التقييم / Format rating
     */
    fun formatRating(ratingValue: Float): String {
        return String.format("%.1f / 10", ratingValue)
    }
    
    /**
     * الحصول على نص نوع المحتوى / Get content type text
     */
    fun getContentTypeText(type: ContentType): String {
        return when (type) {
            ContentType.MOVIE -> "فيلم"
            ContentType.TV_SERIES -> "مسلسل"
            ContentType.EPISODE -> "حلقة"
            ContentType.CLIP -> "مقطع"
            ContentType.LIVE -> "بث مباشر"
            ContentType.DOCUMENTARY -> "وثائقي"
        }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        currentDetails = null
        actions.clear()
        relatedItems.clear()
        detailsListener = null
        context = null
    }
}
