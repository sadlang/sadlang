package sad.android.tv.preferences

import android.content.Context
import android.os.Handler
import android.os.Looper

/**
 * جسر تفضيلات التلفزيون - LeanbackSettingsFragment
 * TV Preferences Bridge - LeanbackSettingsFragment
 * 
 * @author فريق لغة ص
 */
object SadTVPreferencesBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private val categories = mutableListOf<PreferenceCategory>()
    
    /**
     * فئة التفضيلات / Preference category
     */
    data class PreferenceCategory(
        val key: String,
        val title: String,
        val iconResId: Int? = null,
        val preferences: MutableList<Preference> = mutableListOf()
    )
    
    /**
     * التفضيل / Preference
     */
    sealed class Preference {
        abstract val key: String
        abstract val title: String
        abstract val summary: String
        abstract val iconResId: Int?
        
        /**
         * تفضيل تبديل / Switch preference
         */
        data class Switch(
            override val key: String,
            override val title: String,
            override val summary: String = "",
            override val iconResId: Int? = null,
            val defaultValue: Boolean = false
        ) : Preference()
        
        /**
         * تفضيل قائمة / List preference
         */
        data class ListPref(
            override val key: String,
            override val title: String,
            override val summary: String = "",
            override val iconResId: Int? = null,
            val entries: List<String>,
            val values: List<String>,
            val defaultValue: String = ""
        ) : Preference()
        
        /**
         * تفضيل نصي / Text preference
         */
        data class Text(
            override val key: String,
            override val title: String,
            override val summary: String = "",
            override val iconResId: Int? = null,
            val defaultValue: String = "",
            val inputType: InputType = InputType.TEXT
        ) : Preference()
        
        /**
         * تفضيل رقمي / Seekbar preference
         */
        data class Seekbar(
            override val key: String,
            override val title: String,
            override val summary: String = "",
            override val iconResId: Int? = null,
            val min: Int = 0,
            val max: Int = 100,
            val defaultValue: Int = 50
        ) : Preference()
        
        /**
         * تفضيل إجراء / Action preference
         */
        data class Action(
            override val key: String,
            override val title: String,
            override val summary: String = "",
            override val iconResId: Int? = null
        ) : Preference()
    }
    
    /**
     * نوع الإدخال / Input type
     */
    enum class InputType {
        TEXT,
        NUMBER,
        PASSWORD,
        EMAIL
    }
    
    /**
     * مستمع التفضيلات / Preferences listener
     */
    interface PreferencesListener {
        fun onPreferenceChanged(key: String, value: Any?)
        fun onPreferenceClicked(key: String)
    }
    
    private var preferencesListener: PreferencesListener? = null
    
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
    fun setPreferencesListener(listener: PreferencesListener?) {
        preferencesListener = listener
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة الفئات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إضافة فئة / Add category
     */
    fun addCategory(category: PreferenceCategory) {
        categories.add(category)
    }
    
    /**
     * إضافة فئة بسيطة / Add simple category
     */
    fun addCategory(key: String, title: String, iconResId: Int? = null): PreferenceCategory {
        val category = PreferenceCategory(key, title, iconResId)
        categories.add(category)
        return category
    }
    
    /**
     * الحصول على جميع الفئات / Get all categories
     */
    fun getCategories(): List<PreferenceCategory> = categories.toList()
    
    /**
     * الحصول على فئة / Get category
     */
    fun getCategory(key: String): PreferenceCategory? {
        return categories.find { it.key == key }
    }
    
    /**
     * مسح الفئات / Clear categories
     */
    fun clearCategories() {
        categories.clear()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة التفضيلات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إضافة تفضيل لفئة / Add preference to category
     */
    fun addPreference(categoryKey: String, preference: Preference): Boolean {
        val category = categories.find { it.key == categoryKey } ?: return false
        category.preferences.add(preference)
        return true
    }
    
    /**
     * الحصول على جميع التفضيلات / Get all preferences
     */
    fun getAllPreferences(): List<Preference> {
        return categories.flatMap { it.preferences }
    }
    
    /**
     * الحصول على تفضيل / Get preference
     */
    fun getPreference(key: String): Preference? {
        return getAllPreferences().find { it.key == key }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         قراءة/كتابة القيم
    // ═══════════════════════════════════════════════════════════════
    
    private fun getPrefs() = context?.getSharedPreferences("sad_tv_preferences", Context.MODE_PRIVATE)
    
    /**
     * الحصول على قيمة منطقية / Get boolean value
     */
    fun getBoolean(key: String, defaultValue: Boolean = false): Boolean {
        return getPrefs()?.getBoolean(key, defaultValue) ?: defaultValue
    }
    
    /**
     * تعيين قيمة منطقية / Set boolean value
     */
    fun setBoolean(key: String, value: Boolean) {
        getPrefs()?.edit()?.putBoolean(key, value)?.apply()
        mainHandler.post { preferencesListener?.onPreferenceChanged(key, value) }
    }
    
    /**
     * الحصول على نص / Get string
     */
    fun getString(key: String, defaultValue: String = ""): String {
        return getPrefs()?.getString(key, defaultValue) ?: defaultValue
    }
    
    /**
     * تعيين نص / Set string
     */
    fun setString(key: String, value: String) {
        getPrefs()?.edit()?.putString(key, value)?.apply()
        mainHandler.post { preferencesListener?.onPreferenceChanged(key, value) }
    }
    
    /**
     * الحصول على رقم / Get int
     */
    fun getInt(key: String, defaultValue: Int = 0): Int {
        return getPrefs()?.getInt(key, defaultValue) ?: defaultValue
    }
    
    /**
     * تعيين رقم / Set int
     */
    fun setInt(key: String, value: Int) {
        getPrefs()?.edit()?.putInt(key, value)?.apply()
        mainHandler.post { preferencesListener?.onPreferenceChanged(key, value) }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         تفضيلات افتراضية
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إنشاء تفضيلات افتراضية / Create default preferences
     */
    fun createDefaultPreferences() {
        // العامة
        addCategory("general", "عام").preferences.addAll(listOf(
            Preference.ListPref(
                key = "language",
                title = "اللغة",
                summary = "اختر لغة التطبيق",
                entries = listOf("العربية", "English"),
                values = listOf("ar", "en"),
                defaultValue = "ar"
            ),
            Preference.Switch(
                key = "auto_play",
                title = "التشغيل التلقائي",
                summary = "تشغيل الحلقة التالية تلقائياً",
                defaultValue = true
            )
        ))
        
        // العرض
        addCategory("display", "العرض").preferences.addAll(listOf(
            Preference.ListPref(
                key = "quality",
                title = "جودة الفيديو",
                summary = "اختر جودة التشغيل الافتراضية",
                entries = listOf("تلقائي", "1080p", "720p", "480p"),
                values = listOf("auto", "1080", "720", "480"),
                defaultValue = "auto"
            ),
            Preference.Switch(
                key = "subtitles",
                title = "عرض الترجمة",
                summary = "عرض الترجمة افتراضياً",
                defaultValue = true
            )
        ))
        
        // الحساب
        addCategory("account", "الحساب").preferences.addAll(listOf(
            Preference.Action(
                key = "login",
                title = "تسجيل الدخول",
                summary = "سجل دخولك للوصول لمزيد من المحتوى"
            ),
            Preference.Action(
                key = "parental",
                title = "الرقابة الأبوية",
                summary = "إدارة إعدادات المحتوى"
            )
        ))
        
        // حول
        addCategory("about", "حول").preferences.addAll(listOf(
            Preference.Action(
                key = "version",
                title = "الإصدار",
                summary = "1.0.0"
            ),
            Preference.Action(
                key = "licenses",
                title = "التراخيص",
                summary = "تراخيص المكتبات المستخدمة"
            )
        ))
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        categories.clear()
        preferencesListener = null
        context = null
    }
}
