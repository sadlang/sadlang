package sad.android.tv.channels

import android.content.Context
import android.os.Handler
import android.os.Looper
import java.util.concurrent.ConcurrentHashMap

/**
 * جسر قنوات التلفزيون - Channel API للبث المباشر
 * TV Channels Bridge - Channel API for live streaming
 * 
 * @author فريق لغة ص
 */
object SadTVChannelsBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private val channels = ConcurrentHashMap<String, LiveChannel>()
    private val epgData = ConcurrentHashMap<String, List<EPGProgram>>()
    
    /**
     * قناة مباشرة / Live channel
     */
    data class LiveChannel(
        val id: String,
        val name: String,
        val nameArabic: String = name,
        val number: Int = 0,
        val logoUrl: String = "",
        val category: ChannelCategory = ChannelCategory.GENERAL,
        val streamUrl: String = "",
        val streamType: StreamType = StreamType.HLS,
        val isHD: Boolean = false,
        val is4K: Boolean = false,
        val isFavorite: Boolean = false,
        val isLocked: Boolean = false,
        val country: String = "",
        val language: String = "ar"
    )
    
    /**
     * فئة القناة / Channel category
     */
    enum class ChannelCategory {
        GENERAL,
        NEWS,
        SPORTS,
        MOVIES,
        KIDS,
        MUSIC,
        DOCUMENTARY,
        RELIGIOUS,
        ENTERTAINMENT,
        EDUCATION
    }
    
    /**
     * نوع البث / Stream type
     */
    enum class StreamType {
        HLS,        // HTTP Live Streaming
        DASH,       // MPEG-DASH
        RTSP,
        RTMP,
        MP4,
        PROGRESSIVE
    }
    
    /**
     * برنامج EPG
     */
    data class EPGProgram(
        val id: String,
        val channelId: String,
        val title: String,
        val titleArabic: String = title,
        val description: String = "",
        val startTime: Long,         // timestamp
        val endTime: Long,
        val category: String = "",
        val imageUrl: String = "",
        val isLive: Boolean = false,
        val isRecordable: Boolean = false,
        val rating: String? = null
    )
    
    /**
     * مستمع القنوات / Channels listener
     */
    interface ChannelsListener {
        fun onChannelSelected(channel: LiveChannel)
        fun onChannelChanged(from: LiveChannel?, to: LiveChannel)
        fun onProgramChanged(channel: LiveChannel, program: EPGProgram?)
        fun onStreamError(channel: LiveChannel, error: String)
    }
    
    private var channelsListener: ChannelsListener? = null
    private var currentChannel: LiveChannel? = null
    
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
    fun setChannelsListener(listener: ChannelsListener?) {
        channelsListener = listener
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة القنوات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إضافة قناة / Add channel
     */
    fun addChannel(channel: LiveChannel) {
        channels[channel.id] = channel
    }
    
    /**
     * إضافة قنوات متعددة / Add multiple channels
     */
    fun addChannels(channelList: List<LiveChannel>) {
        channelList.forEach { channels[it.id] = it }
    }
    
    /**
     * إزالة قناة / Remove channel
     */
    fun removeChannel(channelId: String): Boolean {
        return channels.remove(channelId) != null
    }
    
    /**
     * الحصول على قناة / Get channel
     */
    fun getChannel(channelId: String): LiveChannel? = channels[channelId]
    
    /**
     * الحصول على جميع القنوات / Get all channels
     */
    fun getAllChannels(): List<LiveChannel> {
        return channels.values.sortedBy { it.number }
    }
    
    /**
     * الحصول على قنوات حسب الفئة / Get channels by category
     */
    fun getChannelsByCategory(category: ChannelCategory): List<LiveChannel> {
        return channels.values.filter { it.category == category }.sortedBy { it.number }
    }
    
    /**
     * الحصول على القنوات المفضلة / Get favorite channels
     */
    fun getFavoriteChannels(): List<LiveChannel> {
        return channels.values.filter { it.isFavorite }.sortedBy { it.number }
    }
    
    /**
     * البحث في القنوات / Search channels
     */
    fun searchChannels(query: String): List<LiveChannel> {
        val q = query.lowercase()
        return channels.values.filter {
            it.name.lowercase().contains(q) ||
            it.nameArabic.contains(q) ||
            it.number.toString() == query
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التشغيل
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تشغيل قناة / Play channel
     */
    fun playChannel(channelId: String): Boolean {
        val channel = channels[channelId] ?: return false
        val oldChannel = currentChannel
        currentChannel = channel
        
        mainHandler.post {
            channelsListener?.onChannelSelected(channel)
            channelsListener?.onChannelChanged(oldChannel, channel)
        }
        
        return true
    }
    
    /**
     * القناة التالية / Next channel
     */
    fun nextChannel(): Boolean {
        val sorted = getAllChannels()
        if (sorted.isEmpty()) return false
        
        val current = currentChannel
        val currentIndex = if (current != null) sorted.indexOfFirst { it.id == current.id } else -1
        val nextIndex = (currentIndex + 1) % sorted.size
        
        return playChannel(sorted[nextIndex].id)
    }
    
    /**
     * القناة السابقة / Previous channel
     */
    fun previousChannel(): Boolean {
        val sorted = getAllChannels()
        if (sorted.isEmpty()) return false
        
        val current = currentChannel
        val currentIndex = if (current != null) sorted.indexOfFirst { it.id == current.id } else 0
        val prevIndex = if (currentIndex <= 0) sorted.size - 1 else currentIndex - 1
        
        return playChannel(sorted[prevIndex].id)
    }
    
    /**
     * الانتقال لرقم قناة / Go to channel number
     */
    fun goToChannelNumber(number: Int): Boolean {
        val channel = channels.values.find { it.number == number } ?: return false
        return playChannel(channel.id)
    }
    
    /**
     * الحصول على القناة الحالية / Get current channel
     */
    fun getCurrentChannel(): LiveChannel? = currentChannel
    
    // ═══════════════════════════════════════════════════════════════
    //                         EPG
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تعيين بيانات EPG لقناة / Set EPG data for channel
     */
    fun setEPGData(channelId: String, programs: List<EPGProgram>) {
        epgData[channelId] = programs
    }
    
    /**
     * الحصول على EPG لقناة / Get EPG for channel
     */
    fun getEPGData(channelId: String): List<EPGProgram> {
        return epgData[channelId] ?: emptyList()
    }
    
    /**
     * الحصول على البرنامج الحالي / Get current program
     */
    fun getCurrentProgram(channelId: String): EPGProgram? {
        val now = System.currentTimeMillis()
        return epgData[channelId]?.find { now in it.startTime..it.endTime }
    }
    
    /**
     * الحصول على البرنامج التالي / Get next program
     */
    fun getNextProgram(channelId: String): EPGProgram? {
        val now = System.currentTimeMillis()
        return epgData[channelId]?.filter { it.startTime > now }?.minByOrNull { it.startTime }
    }
    
    /**
     * الحصول على برامج اليوم / Get today's programs
     */
    fun getTodayPrograms(channelId: String): List<EPGProgram> {
        val today = java.util.Calendar.getInstance().apply {
            set(java.util.Calendar.HOUR_OF_DAY, 0)
            set(java.util.Calendar.MINUTE, 0)
            set(java.util.Calendar.SECOND, 0)
        }.timeInMillis
        
        val tomorrow = today + 24 * 60 * 60 * 1000
        
        return epgData[channelId]?.filter { it.startTime in today..tomorrow } ?: emptyList()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         المفضلات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تبديل المفضلة / Toggle favorite
     */
    fun toggleFavorite(channelId: String): Boolean {
        val channel = channels[channelId] ?: return false
        channels[channelId] = channel.copy(isFavorite = !channel.isFavorite)
        return true
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الفئات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على اسم الفئة بالعربية / Get category name in Arabic
     */
    fun getCategoryNameArabic(category: ChannelCategory): String {
        return when (category) {
            ChannelCategory.GENERAL -> "عامة"
            ChannelCategory.NEWS -> "أخبار"
            ChannelCategory.SPORTS -> "رياضة"
            ChannelCategory.MOVIES -> "أفلام"
            ChannelCategory.KIDS -> "أطفال"
            ChannelCategory.MUSIC -> "موسيقى"
            ChannelCategory.DOCUMENTARY -> "وثائقية"
            ChannelCategory.RELIGIOUS -> "دينية"
            ChannelCategory.ENTERTAINMENT -> "ترفيه"
            ChannelCategory.EDUCATION -> "تعليمية"
        }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        channels.clear()
        epgData.clear()
        currentChannel = null
        channelsListener = null
        context = null
    }
}
