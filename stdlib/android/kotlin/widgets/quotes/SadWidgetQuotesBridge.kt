package sad.android.widgets.quotes

import android.content.Context
import android.content.SharedPreferences
import android.os.Handler
import android.os.Looper
import java.util.concurrent.ConcurrentHashMap
import kotlin.random.Random

/**
 * جسر ويدجت الاقتباسات - عرض اقتباسات وأذكار
 * Quotes Widget Bridge - Display quotes and Azkar
 * 
 * @author فريق لغة ص
 */
object SadWidgetQuotesBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private val quoteCategories = ConcurrentHashMap<String, QuoteCategory>()
    private const val PREFS_NAME = "sad_widget_quotes"
    
    /**
     * فئة الاقتباسات / Quote category
     */
    data class QuoteCategory(
        val id: String,
        val name: String,
        val quotes: MutableList<Quote> = mutableListOf()
    )
    
    /**
     * اقتباس / Quote
     */
    data class Quote(
        val id: String,
        val text: String,
        val author: String = "",
        val source: String = "",
        val category: String = "",
        val isFavorite: Boolean = false
    )
    
    /**
     * مستمع الاقتباسات / Quote listener
     */
    interface QuoteListener {
        fun onQuoteChanged(quote: Quote)
    }
    
    private val quoteListeners = ConcurrentHashMap<Int, QuoteListener>()
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        loadDefaultCategories()
    }
    
    /**
     * إضافة مستمع / Add listener
     */
    fun addQuoteListener(widgetId: Int, listener: QuoteListener) {
        quoteListeners[widgetId] = listener
    }
    
    /**
     * إزالة مستمع / Remove listener
     */
    fun removeQuoteListener(widgetId: Int) {
        quoteListeners.remove(widgetId)
    }
    
    private fun loadDefaultCategories() {
        // أذكار الصباح
        registerCategory(QuoteCategory("morning_azkar", "أذكار الصباح").apply {
            quotes.addAll(listOf(
                Quote("m1", "أصبحنا وأصبح الملك لله، والحمد لله، لا إله إلا الله وحده لا شريك له، له الملك وله الحمد وهو على كل شيء قدير"),
                Quote("m2", "اللهم بك أصبحنا وبك أمسينا، وبك نحيا وبك نموت وإليك النشور"),
                Quote("m3", "سبحان الله وبحمده عدد خلقه ورضا نفسه وزنة عرشه ومداد كلماته"),
                Quote("m4", "اللهم إني أسألك علماً نافعاً ورزقاً طيباً وعملاً متقبلاً"),
                Quote("m5", "اللهم عافني في بدني، اللهم عافني في سمعي، اللهم عافني في بصري")
            ))
        })
        
        // أذكار المساء
        registerCategory(QuoteCategory("evening_azkar", "أذكار المساء").apply {
            quotes.addAll(listOf(
                Quote("e1", "أمسينا وأمسى الملك لله، والحمد لله، لا إله إلا الله وحده لا شريك له"),
                Quote("e2", "اللهم بك أمسينا وبك أصبحنا، وبك نحيا وبك نموت وإليك المصير"),
                Quote("e3", "أعوذ بكلمات الله التامات من شر ما خلق"),
                Quote("e4", "بسم الله الذي لا يضر مع اسمه شيء في الأرض ولا في السماء وهو السميع العليم"),
                Quote("e5", "اللهم إني أمسيت أشهدك وأشهد حملة عرشك وملائكتك وجميع خلقك أنك أنت الله لا إله إلا أنت")
            ))
        })
        
        // حكم وأمثال
        registerCategory(QuoteCategory("wisdom", "حكم وأمثال").apply {
            quotes.addAll(listOf(
                Quote("w1", "العلم نور والجهل ظلام"),
                Quote("w2", "من جدّ وجد ومن زرع حصد"),
                Quote("w3", "الصبر مفتاح الفرج"),
                Quote("w4", "في التأني السلامة وفي العجلة الندامة"),
                Quote("w5", "رب أخ لك لم تلده أمك"),
                Quote("w6", "العقل السليم في الجسم السليم"),
                Quote("w7", "خير الكلام ما قل ودل"),
                Quote("w8", "الوقت كالسيف إن لم تقطعه قطعك")
            ))
        })
        
        // اقتباسات تحفيزية
        registerCategory(QuoteCategory("motivation", "تحفيزية").apply {
            quotes.addAll(listOf(
                Quote("t1", "النجاح ليس نهاية المطاف، والفشل ليس قاتلاً، إنما الشجاعة للاستمرار هي ما يهم", "ونستون تشرشل"),
                Quote("t2", "كن أنت التغيير الذي تريد أن تراه في العالم", "غاندي"),
                Quote("t3", "الطريقة الوحيدة لعمل عظيم هي أن تحب ما تفعله", "ستيف جوبز"),
                Quote("t4", "لا تخف من التقدم ببطء، خف فقط من الوقوف في مكانك", "مثل صيني"),
                Quote("t5", "كل إنجاز عظيم كان في البداية مستحيلاً")
            ))
        })
        
        // أدعية قرآنية
        registerCategory(QuoteCategory("quran_duas", "أدعية قرآنية").apply {
            quotes.addAll(listOf(
                Quote("q1", "ربنا آتنا في الدنيا حسنة وفي الآخرة حسنة وقنا عذاب النار", "", "البقرة 201"),
                Quote("q2", "ربنا لا تؤاخذنا إن نسينا أو أخطأنا", "", "البقرة 286"),
                Quote("q3", "رب اشرح لي صدري ويسر لي أمري", "", "طه 25-26"),
                Quote("q4", "رب زدني علماً", "", "طه 114"),
                Quote("q5", "ربنا هب لنا من أزواجنا وذرياتنا قرة أعين", "", "الفرقان 74")
            ))
        })
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة الفئات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تسجيل فئة / Register category
     */
    fun registerCategory(category: QuoteCategory): Boolean {
        quoteCategories[category.id] = category
        return true
    }
    
    /**
     * الحصول على فئة / Get category
     */
    fun getCategory(categoryId: String): QuoteCategory? {
        return quoteCategories[categoryId]
    }
    
    /**
     * جميع الفئات / All categories
     */
    fun getAllCategories(): List<QuoteCategory> {
        return quoteCategories.values.toList()
    }
    
    /**
     * إضافة اقتباس / Add quote
     */
    fun addQuote(categoryId: String, quote: Quote): Boolean {
        val category = quoteCategories[categoryId] ?: return false
        category.quotes.add(quote)
        return true
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الحصول على اقتباس
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * اقتباس عشوائي من فئة / Random quote from category
     */
    fun getRandomQuote(categoryId: String): Quote? {
        val category = quoteCategories[categoryId] ?: return null
        if (category.quotes.isEmpty()) return null
        return category.quotes.random()
    }
    
    /**
     * اقتباس عشوائي من أي فئة / Random quote from any category
     */
    fun getRandomQuote(): Quote? {
        val allQuotes = quoteCategories.values.flatMap { it.quotes }
        if (allQuotes.isEmpty()) return null
        return allQuotes.random()
    }
    
    /**
     * اقتباس اليوم / Quote of the day
     */
    fun getQuoteOfTheDay(categoryId: String? = null): Quote? {
        val prefs = getPrefs() ?: return getRandomQuote(categoryId ?: "wisdom")
        
        val today = java.text.SimpleDateFormat("yyyyMMdd", java.util.Locale.US)
            .format(java.util.Date())
        
        val savedDate = prefs.getString("quote_date", "")
        val savedQuoteId = prefs.getString("quote_id", "")
        val savedCategory = prefs.getString("quote_category", "")
        
        // إذا نفس اليوم، أرجع نفس الاقتباس
        if (savedDate == today && savedQuoteId?.isNotEmpty() == true) {
            val category = quoteCategories[savedCategory]
            return category?.quotes?.find { it.id == savedQuoteId }
        }
        
        // اختر اقتباس جديد
        val quote = if (categoryId != null) {
            getRandomQuote(categoryId)
        } else {
            getRandomQuote()
        }
        
        // احفظ الاقتباس
        quote?.let {
            prefs.edit()
                .putString("quote_date", today)
                .putString("quote_id", it.id)
                .putString("quote_category", it.category)
                .apply()
        }
        
        return quote
    }
    
    /**
     * الاقتباس التالي / Next quote
     */
    fun getNextQuote(categoryId: String, currentQuoteId: String?): Quote? {
        val category = quoteCategories[categoryId] ?: return null
        if (category.quotes.isEmpty()) return null
        
        if (currentQuoteId == null) {
            return category.quotes.firstOrNull()
        }
        
        val currentIndex = category.quotes.indexOfFirst { it.id == currentQuoteId }
        val nextIndex = (currentIndex + 1) % category.quotes.size
        return category.quotes[nextIndex]
    }
    
    /**
     * الاقتباس السابق / Previous quote
     */
    fun getPreviousQuote(categoryId: String, currentQuoteId: String?): Quote? {
        val category = quoteCategories[categoryId] ?: return null
        if (category.quotes.isEmpty()) return null
        
        if (currentQuoteId == null) {
            return category.quotes.lastOrNull()
        }
        
        val currentIndex = category.quotes.indexOfFirst { it.id == currentQuoteId }
        val prevIndex = if (currentIndex <= 0) category.quotes.size - 1 else currentIndex - 1
        return category.quotes[prevIndex]
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         المفضلات
    // ═══════════════════════════════════════════════════════════════
    
    private fun getPrefs(): SharedPreferences? {
        return context?.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE)
    }
    
    /**
     * إضافة للمفضلة / Add to favorites
     */
    fun addToFavorites(quoteId: String): Boolean {
        val prefs = getPrefs() ?: return false
        val favorites = getFavoriteIds().toMutableSet()
        favorites.add(quoteId)
        return prefs.edit().putStringSet("favorites", favorites).commit()
    }
    
    /**
     * إزالة من المفضلة / Remove from favorites
     */
    fun removeFromFavorites(quoteId: String): Boolean {
        val prefs = getPrefs() ?: return false
        val favorites = getFavoriteIds().toMutableSet()
        favorites.remove(quoteId)
        return prefs.edit().putStringSet("favorites", favorites).commit()
    }
    
    /**
     * هل في المفضلة؟ / Is favorite?
     */
    fun isFavorite(quoteId: String): Boolean {
        return getFavoriteIds().contains(quoteId)
    }
    
    /**
     * الحصول على معرفات المفضلة / Get favorite IDs
     */
    fun getFavoriteIds(): Set<String> {
        return getPrefs()?.getStringSet("favorites", emptySet()) ?: emptySet()
    }
    
    /**
     * الحصول على المفضلات / Get favorites
     */
    fun getFavorites(): List<Quote> {
        val favoriteIds = getFavoriteIds()
        return quoteCategories.values
            .flatMap { it.quotes }
            .filter { favoriteIds.contains(it.id) }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        quoteCategories.clear()
        quoteListeners.clear()
        context = null
    }
}
