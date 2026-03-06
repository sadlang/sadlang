package sad.android.tv.browse

import android.content.Context
import android.os.Handler
import android.os.Looper
import java.util.concurrent.ConcurrentHashMap

/**
 * جسر تصفح التلفزيون - BrowseFragment و واجهة Leanback
 * TV Browse Bridge - BrowseFragment and Leanback interface
 * 
 * @author فريق لغة ص
 */
object SadTVBrowseBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private val rows = ConcurrentHashMap<Long, BrowseRow>()
    private var rowIdCounter = 1L
    
    /**
     * صف التصفح / Browse row
     */
    data class BrowseRow(
        val id: Long,
        val name: String,
        val items: MutableList<BrowseItem> = mutableListOf(),
        val headerStyle: HeaderStyle = HeaderStyle.DEFAULT
    )
    
    /**
     * عنصر التصفح / Browse item
     */
    data class BrowseItem(
        val id: String,
        val title: String,
        val description: String = "",
        val imageUrl: String = "",
        val category: String = "",
        val badgeText: String? = null,
        val progress: Float? = null,      // 0-1 للتقدم
        val metadata: Map<String, Any> = emptyMap()
    )
    
    /**
     * نمط الرأس / Header style
     */
    enum class HeaderStyle {
        DEFAULT,
        HIDDEN,
        SECTIONS,
        DIVIDER
    }
    
    /**
     * تكوين التصفح / Browse config
     */
    data class BrowseConfig(
        val title: String,
        val showTitle: Boolean = true,
        val showHeaders: Boolean = true,
        val brandColor: Int = 0xFF1E88E5.toInt(),
        val searchAffordanceColor: Int = 0xFF2196F3.toInt(),
        val headersPadding: Int = 32,
        val rowHeight: Int = 200,
        val cardWidth: Int = 180,
        val cardHeight: Int = 240,
        val enableSearch: Boolean = true,
        val enableOnboarding: Boolean = false
    )
    
    /**
     * مستمع التصفح / Browse listener
     */
    interface BrowseListener {
        fun onRowLoaded(row: BrowseRow)
        fun onItemSelected(row: BrowseRow, item: BrowseItem)
        fun onItemClicked(row: BrowseRow, item: BrowseItem)
        fun onSearchClicked()
    }
    
    private var config = BrowseConfig("تصفح")
    private var browseListener: BrowseListener? = null
    
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
    fun setConfig(newConfig: BrowseConfig) {
        config = newConfig
    }
    
    /**
     * الحصول على التكوين / Get config
     */
    fun getConfig(): BrowseConfig = config
    
    /**
     * تعيين المستمع / Set listener
     */
    fun setBrowseListener(listener: BrowseListener?) {
        browseListener = listener
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة الصفوف
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إنشاء صف جديد / Create new row
     */
    fun createRow(name: String, style: HeaderStyle = HeaderStyle.DEFAULT): BrowseRow {
        val row = BrowseRow(
            id = rowIdCounter++,
            name = name,
            headerStyle = style
        )
        rows[row.id] = row
        return row
    }
    
    /**
     * الحصول على صف / Get row
     */
    fun getRow(id: Long): BrowseRow? = rows[id]
    
    /**
     * الحصول على جميع الصفوف / Get all rows
     */
    fun getAllRows(): List<BrowseRow> = rows.values.toList()
    
    /**
     * حذف صف / Delete row
     */
    fun deleteRow(id: Long): Boolean = rows.remove(id) != null
    
    /**
     * مسح جميع الصفوف / Clear all rows
     */
    fun clearRows() {
        rows.clear()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة العناصر
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إضافة عنصر لصف / Add item to row
     */
    fun addItemToRow(rowId: Long, item: BrowseItem): Boolean {
        val row = rows[rowId] ?: return false
        row.items.add(item)
        return true
    }
    
    /**
     * إضافة عناصر متعددة لصف / Add multiple items to row
     */
    fun addItemsToRow(rowId: Long, items: List<BrowseItem>): Boolean {
        val row = rows[rowId] ?: return false
        row.items.addAll(items)
        mainHandler.post { browseListener?.onRowLoaded(row) }
        return true
    }
    
    /**
     * الحصول على عناصر صف / Get row items
     */
    fun getRowItems(rowId: Long): List<BrowseItem> {
        return rows[rowId]?.items?.toList() ?: emptyList()
    }
    
    /**
     * مسح عناصر صف / Clear row items
     */
    fun clearRowItems(rowId: Long): Boolean {
        val row = rows[rowId] ?: return false
        row.items.clear()
        return true
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         أحداث التفاعل
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إعلام بتحديد عنصر / Notify item selected
     */
    fun notifyItemSelected(rowId: Long, itemIndex: Int) {
        val row = rows[rowId] ?: return
        val item = row.items.getOrNull(itemIndex) ?: return
        mainHandler.post { browseListener?.onItemSelected(row, item) }
    }
    
    /**
     * إعلام بالنقر على عنصر / Notify item clicked
     */
    fun notifyItemClicked(rowId: Long, itemIndex: Int) {
        val row = rows[rowId] ?: return
        val item = row.items.getOrNull(itemIndex) ?: return
        mainHandler.post { browseListener?.onItemClicked(row, item) }
    }
    
    /**
     * إعلام بالنقر على البحث / Notify search clicked
     */
    fun notifySearchClicked() {
        mainHandler.post { browseListener?.onSearchClicked() }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         فئات شائعة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إنشاء صفوف افتراضية / Create default rows
     */
    fun createDefaultRows(): List<BrowseRow> {
        return listOf(
            createRow("مستمر المشاهدة"),
            createRow("موصى به لك"),
            createRow("جديد هذا الأسبوع"),
            createRow("الأكثر مشاهدة"),
            createRow("الفئات")
        )
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        rows.clear()
        browseListener = null
        context = null
    }
}
