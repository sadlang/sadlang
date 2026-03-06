package sad.android.tv.search

import android.content.Context
import android.os.Handler
import android.os.Looper
import java.util.concurrent.CopyOnWriteArrayList

/**
 * جسر بحث التلفزيون - SearchFragment للبحث الصوتي والنصي
 * TV Search Bridge - SearchFragment for voice and text search
 * 
 * @author فريق لغة ص
 */
object SadTVSearchBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private val searchHistory = CopyOnWriteArrayList<String>()
    private val suggestions = CopyOnWriteArrayList<String>()
    
    /**
     * نتيجة البحث / Search result
     */
    data class SearchResult(
        val id: String,
        val title: String,
        val description: String = "",
        val imageUrl: String = "",
        val category: String = "",
        val relevanceScore: Float = 0f,
        val metadata: Map<String, Any> = emptyMap()
    )
    
    /**
     * مجموعة نتائج / Results group
     */
    data class SearchResultGroup(
        val title: String,
        val results: List<SearchResult>
    )
    
    /**
     * تكوين البحث / Search config
     */
    data class SearchConfig(
        val hint: String = "ابحث...",
        val enableVoiceSearch: Boolean = true,
        val minQueryLength: Int = 2,
        val debounceMs: Long = 300,
        val maxHistoryItems: Int = 20,
        val showSuggestions: Boolean = true,
        val badgeColor: Int = 0xFF1E88E5.toInt(),
        val recognizerLanguage: String = "ar"
    )
    
    /**
     * مستمع البحث / Search listener
     */
    interface SearchListener {
        fun onQueryChanged(query: String)
        fun onQuerySubmitted(query: String)
        fun onResultClicked(result: SearchResult)
        fun onVoiceSearchRequested()
        fun onSearchError(error: String)
    }
    
    /**
     * مزود البحث / Search provider
     */
    interface SearchProvider {
        fun performSearch(query: String, callback: (List<SearchResultGroup>) -> Unit)
        fun getSuggestions(query: String, callback: (List<String>) -> Unit)
    }
    
    private var config = SearchConfig()
    private var searchListener: SearchListener? = null
    private var searchProvider: SearchProvider? = null
    private var currentQuery = ""
    private var searchRunnable: Runnable? = null
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        loadSearchHistory()
    }
    
    /**
     * تعيين التكوين / Set config
     */
    fun setConfig(newConfig: SearchConfig) {
        config = newConfig
    }
    
    /**
     * تعيين المستمع / Set listener
     */
    fun setSearchListener(listener: SearchListener?) {
        searchListener = listener
    }
    
    /**
     * تعيين مزود البحث / Set search provider
     */
    fun setSearchProvider(provider: SearchProvider?) {
        searchProvider = provider
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         البحث
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تعيين استعلام البحث / Set search query
     */
    fun setQuery(query: String) {
        currentQuery = query
        
        // إلغاء البحث السابق
        searchRunnable?.let { mainHandler.removeCallbacks(it) }
        
        // إعلام بتغيير الاستعلام
        mainHandler.post { searchListener?.onQueryChanged(query) }
        
        // تأخير البحث (debounce)
        if (query.length >= config.minQueryLength) {
            searchRunnable = Runnable {
                loadSuggestions(query)
            }
            mainHandler.postDelayed(searchRunnable!!, config.debounceMs)
        }
    }
    
    /**
     * إرسال استعلام البحث / Submit search query
     */
    fun submitQuery(query: String) {
        currentQuery = query
        
        // حفظ في السجل
        if (query.isNotBlank()) {
            addToHistory(query)
        }
        
        // إعلام بالإرسال
        mainHandler.post { searchListener?.onQuerySubmitted(query) }
        
        // تنفيذ البحث
        performSearch(query)
    }
    
    /**
     * تنفيذ البحث / Perform search
     */
    fun performSearch(query: String) {
        val provider = searchProvider
        if (provider == null) {
            // بحث افتراضي بدون مزود
            mainHandler.post { searchListener?.onSearchError("لم يتم تعيين مزود بحث") }
            return
        }
        
        provider.performSearch(query) { results ->
            mainHandler.post {
                // المعالجة تتم عبر المستمع
            }
        }
    }
    
    private fun loadSuggestions(query: String) {
        val provider = searchProvider ?: return
        
        provider.getSuggestions(query) { newSuggestions ->
            suggestions.clear()
            suggestions.addAll(newSuggestions)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         البحث الصوتي
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * بدء البحث الصوتي / Start voice search
     */
    fun startVoiceSearch() {
        if (!config.enableVoiceSearch) return
        mainHandler.post { searchListener?.onVoiceSearchRequested() }
    }
    
    /**
     * معالجة نتيجة البحث الصوتي / Handle voice result
     */
    fun handleVoiceResult(recognizedText: String) {
        if (recognizedText.isNotBlank()) {
            setQuery(recognizedText)
            submitQuery(recognizedText)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         سجل البحث
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إضافة للسجل / Add to history
     */
    fun addToHistory(query: String) {
        // إزالة التكرار
        searchHistory.remove(query)
        
        // إضافة في البداية
        searchHistory.add(0, query)
        
        // قص إذا تجاوز الحد
        while (searchHistory.size > config.maxHistoryItems) {
            searchHistory.removeAt(searchHistory.size - 1)
        }
        
        saveSearchHistory()
    }
    
    /**
     * الحصول على السجل / Get history
     */
    fun getSearchHistory(): List<String> = searchHistory.toList()
    
    /**
     * مسح السجل / Clear history
     */
    fun clearSearchHistory() {
        searchHistory.clear()
        saveSearchHistory()
    }
    
    /**
     * الحصول على الاقتراحات / Get suggestions
     */
    fun getSuggestions(): List<String> = suggestions.toList()
    
    private fun loadSearchHistory() {
        val ctx = context ?: return
        val prefs = ctx.getSharedPreferences("sad_tv_search", Context.MODE_PRIVATE)
        val history = prefs.getStringSet("history", emptySet()) ?: emptySet()
        searchHistory.clear()
        searchHistory.addAll(history)
    }
    
    private fun saveSearchHistory() {
        val ctx = context ?: return
        val prefs = ctx.getSharedPreferences("sad_tv_search", Context.MODE_PRIVATE)
        prefs.edit().putStringSet("history", searchHistory.toSet()).apply()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الاستعلام الحالي
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على الاستعلام الحالي / Get current query
     */
    fun getCurrentQuery(): String = currentQuery
    
    /**
     * مسح الاستعلام / Clear query
     */
    fun clearQuery() {
        currentQuery = ""
        suggestions.clear()
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        searchRunnable?.let { mainHandler.removeCallbacks(it) }
        searchHistory.clear()
        suggestions.clear()
        searchListener = null
        searchProvider = null
        context = null
    }
}
