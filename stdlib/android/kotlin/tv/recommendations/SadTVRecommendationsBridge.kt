package sad.android.tv.recommendations

import android.content.Context
import android.os.Handler
import android.os.Looper
import java.util.concurrent.ConcurrentHashMap

/**
 * جسر توصيات التلفزيون - Watch Next و توصيات الشاشة الرئيسية
 * TV Recommendations Bridge - Watch Next and home screen recommendations
 * 
 * @author فريق لغة ص
 */
object SadTVRecommendationsBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private val recommendations = ConcurrentHashMap<String, Recommendation>()
    private val watchNextItems = ConcurrentHashMap<String, WatchNextItem>()
    
    /**
     * نوع التوصية / Recommendation type
     */
    enum class RecommendationType {
        MOVIE,
        TV_SERIES,
        TV_EPISODE,
        MUSIC,
        CLIP,
        LIVE,
        CHANNEL
    }
    
    /**
     * توصية / Recommendation
     */
    data class Recommendation(
        val id: String,
        val title: String,
        val description: String = "",
        val imageUrl: String = "",
        val backgroundUrl: String = "",
        val type: RecommendationType = RecommendationType.MOVIE,
        val group: String = "",
        val sortKey: Int = 0,
        val badgeText: String? = null,
        val progress: Float = 0f,
        val intentUri: String = ""
    )
    
    /**
     * نوع Watch Next / Watch next type
     */
    enum class WatchNextType {
        CONTINUE,      // استكمال المشاهدة
        NEXT,          // الحلقة التالية
        NEW,           // محتوى جديد
        WATCHLIST      // قائمة المشاهدة
    }
    
    /**
     * عنصر Watch Next
     */
    data class WatchNextItem(
        val id: String,
        val title: String,
        val description: String = "",
        val imageUrl: String = "",
        val type: WatchNextType,
        val contentType: RecommendationType = RecommendationType.MOVIE,
        val lastEngagementMs: Long = System.currentTimeMillis(),
        val watchProgress: Float = 0f,         // 0-1
        val lastPosition: Long = 0,            // بالميلي ثانية
        val duration: Long = 0,
        val releaseDate: String? = null,
        val seriesId: String? = null,
        val seasonNumber: Int? = null,
        val episodeNumber: Int? = null,
        val intentUri: String = ""
    )
    
    /**
     * قناة / Channel
     */
    data class TVChannel(
        val id: String,
        val displayName: String,
        val description: String = "",
        val iconUrl: String = "",
        val internalProviderId: String = ""
    )
    
    /**
     * برنامج / Program
     */
    data class TVProgram(
        val id: String,
        val channelId: String,
        val title: String,
        val description: String = "",
        val posterUrl: String = "",
        val thumbnailUrl: String = "",
        val type: RecommendationType = RecommendationType.MOVIE,
        val releaseDate: String? = null,
        val intentUri: String = ""
    )
    
    /**
     * مستمع التوصيات / Recommendations listener
     */
    interface RecommendationsListener {
        fun onRecommendationAdded(recommendation: Recommendation)
        fun onRecommendationRemoved(id: String)
        fun onWatchNextAdded(item: WatchNextItem)
        fun onWatchNextRemoved(id: String)
    }
    
    private var recommendationsListener: RecommendationsListener? = null
    
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
    fun setRecommendationsListener(listener: RecommendationsListener?) {
        recommendationsListener = listener
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التوصيات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إضافة توصية / Add recommendation
     */
    fun addRecommendation(recommendation: Recommendation): Boolean {
        recommendations[recommendation.id] = recommendation
        mainHandler.post { recommendationsListener?.onRecommendationAdded(recommendation) }
        return true
    }
    
    /**
     * إزالة توصية / Remove recommendation
     */
    fun removeRecommendation(id: String): Boolean {
        val removed = recommendations.remove(id)
        if (removed != null) {
            mainHandler.post { recommendationsListener?.onRecommendationRemoved(id) }
        }
        return removed != null
    }
    
    /**
     * الحصول على جميع التوصيات / Get all recommendations
     */
    fun getRecommendations(): List<Recommendation> {
        return recommendations.values.sortedBy { it.sortKey }
    }
    
    /**
     * الحصول على توصيات مجموعة / Get group recommendations
     */
    fun getRecommendationsByGroup(group: String): List<Recommendation> {
        return recommendations.values.filter { it.group == group }.sortedBy { it.sortKey }
    }
    
    /**
     * مسح جميع التوصيات / Clear all recommendations
     */
    fun clearRecommendations() {
        recommendations.keys.forEach { removeRecommendation(it) }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         Watch Next
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إضافة عنصر Watch Next / Add Watch Next item
     */
    fun addWatchNextItem(item: WatchNextItem): Boolean {
        watchNextItems[item.id] = item
        mainHandler.post { recommendationsListener?.onWatchNextAdded(item) }
        return true
    }
    
    /**
     * تحديث تقدم المشاهدة / Update watch progress
     */
    fun updateWatchProgress(id: String, progress: Float, lastPosition: Long): Boolean {
        val item = watchNextItems[id] ?: return false
        watchNextItems[id] = item.copy(
            watchProgress = progress,
            lastPosition = lastPosition,
            lastEngagementMs = System.currentTimeMillis()
        )
        return true
    }
    
    /**
     * إزالة عنصر Watch Next / Remove Watch Next item
     */
    fun removeWatchNextItem(id: String): Boolean {
        val removed = watchNextItems.remove(id)
        if (removed != null) {
            mainHandler.post { recommendationsListener?.onWatchNextRemoved(id) }
        }
        return removed != null
    }
    
    /**
     * الحصول على عناصر Watch Next / Get Watch Next items
     */
    fun getWatchNextItems(): List<WatchNextItem> {
        return watchNextItems.values.sortedByDescending { it.lastEngagementMs }
    }
    
    /**
     * الحصول على عناصر مستمرة / Get continue watching items
     */
    fun getContinueWatchingItems(): List<WatchNextItem> {
        return watchNextItems.values
            .filter { it.type == WatchNextType.CONTINUE && it.watchProgress > 0 && it.watchProgress < 0.95 }
            .sortedByDescending { it.lastEngagementMs }
    }
    
    /**
     * مسح Watch Next / Clear Watch Next
     */
    fun clearWatchNext() {
        watchNextItems.keys.forEach { removeWatchNextItem(it) }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إنشاء التوصيات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إنشاء توصية فيلم / Create movie recommendation
     */
    fun createMovieRecommendation(
        id: String,
        title: String,
        description: String,
        imageUrl: String,
        year: String? = null
    ): Recommendation {
        return Recommendation(
            id = id,
            title = title,
            description = description,
            imageUrl = imageUrl,
            type = RecommendationType.MOVIE,
            badgeText = year
        )
    }
    
    /**
     * إنشاء عنصر استكمال مشاهدة / Create continue watching item
     */
    fun createContinueWatchingItem(
        id: String,
        title: String,
        imageUrl: String,
        progress: Float,
        lastPosition: Long,
        duration: Long,
        contentType: RecommendationType = RecommendationType.MOVIE
    ): WatchNextItem {
        return WatchNextItem(
            id = id,
            title = title,
            imageUrl = imageUrl,
            type = WatchNextType.CONTINUE,
            contentType = contentType,
            watchProgress = progress,
            lastPosition = lastPosition,
            duration = duration
        )
    }
    
    /**
     * إنشاء عنصر الحلقة التالية / Create next episode item
     */
    fun createNextEpisodeItem(
        id: String,
        seriesTitle: String,
        episodeTitle: String,
        imageUrl: String,
        seriesId: String,
        season: Int,
        episode: Int
    ): WatchNextItem {
        return WatchNextItem(
            id = id,
            title = "$seriesTitle: $episodeTitle",
            description = "الموسم $season - الحلقة $episode",
            imageUrl = imageUrl,
            type = WatchNextType.NEXT,
            contentType = RecommendationType.TV_EPISODE,
            seriesId = seriesId,
            seasonNumber = season,
            episodeNumber = episode
        )
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        recommendations.clear()
        watchNextItems.clear()
        recommendationsListener = null
        context = null
    }
}
