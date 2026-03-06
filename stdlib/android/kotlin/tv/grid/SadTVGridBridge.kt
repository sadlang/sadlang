package sad.android.tv.grid

import android.content.Context
import android.os.Handler
import android.os.Looper

/**
 * جسر الشبكة التلفزيونية - VerticalGridFragment
 * TV Grid Bridge - VerticalGridFragment
 * 
 * @author فريق لغة ص
 */
object SadTVGridBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    
    /**
     * عنصر الشبكة / Grid item
     */
    data class GridItem(
        val id: String,
        val title: String,
        val subtitle: String = "",
        val imageUrl: String = "",
        val badge: String? = null,
        val data: Any? = null
    )
    
    /**
     * تكوين الشبكة / Grid config
     */
    data class GridConfig(
        val columns: Int = 5,
        val itemWidth: Int = 200,
        val itemHeight: Int = 300,
        val horizontalSpacing: Int = 20,
        val verticalSpacing: Int = 20,
        val paddingTop: Int = 60,
        val paddingBottom: Int = 60,
        val paddingLeft: Int = 60,
        val paddingRight: Int = 60,
        val fadeOutEnabled: Boolean = true,
        val infiniteScroll: Boolean = false
    )
    
    /**
     * ترتيب الشبكة / Grid sort
     */
    enum class SortOrder {
        NAME_ASC,
        NAME_DESC,
        DATE_ASC,
        DATE_DESC,
        POPULARITY,
        RATING,
        CUSTOM
    }
    
    /**
     * فلتر الشبكة / Grid filter
     */
    data class GridFilter(
        val key: String,
        val value: String,
        val displayName: String
    )
    
    /**
     * صفحة الشبكة / Grid page
     */
    data class GridPage(
        val items: List<GridItem>,
        val page: Int,
        val totalPages: Int,
        val hasMore: Boolean
    )
    
    /**
     * مستمع الشبكة / Grid listener
     */
    interface GridListener {
        fun onItemSelected(item: GridItem, position: Int)
        fun onItemClicked(item: GridItem, position: Int)
        fun onLoadMore(page: Int)
        fun onScrolled(firstVisiblePosition: Int, lastVisiblePosition: Int)
    }
    
    private var gridListener: GridListener? = null
    private var config = GridConfig()
    private val items = mutableListOf<GridItem>()
    private val filters = mutableListOf<GridFilter>()
    private var selectedPosition = 0
    private var currentPage = 0
    private var totalPages = 1
    private var currentSort = SortOrder.CUSTOM
    
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
    fun setGridListener(listener: GridListener?) {
        gridListener = listener
    }
    
    /**
     * تعيين التكوين / Set config
     */
    fun setConfig(newConfig: GridConfig) {
        config = newConfig
    }
    
    /**
     * الحصول على التكوين / Get config
     */
    fun getConfig(): GridConfig = config
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة العناصر
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تعيين العناصر / Set items
     */
    fun setItems(newItems: List<GridItem>) {
        items.clear()
        items.addAll(newItems)
        currentPage = 0
    }
    
    /**
     * إضافة عناصر / Add items
     */
    fun addItems(newItems: List<GridItem>) {
        items.addAll(newItems)
    }
    
    /**
     * الحصول على العناصر / Get items
     */
    fun getItems(): List<GridItem> = items.toList()
    
    /**
     * الحصول على عنصر / Get item
     */
    fun getItem(position: Int): GridItem? = items.getOrNull(position)
    
    /**
     * عدد العناصر / Item count
     */
    fun getItemCount(): Int = items.size
    
    /**
     * مسح العناصر / Clear items
     */
    fun clearItems() {
        items.clear()
        selectedPosition = 0
        currentPage = 0
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التنقل
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تعيين الموضع المختار / Set selected position
     */
    fun setSelectedPosition(position: Int) {
        if (position in 0 until items.size) {
            selectedPosition = position
            val item = items[position]
            mainHandler.post { gridListener?.onItemSelected(item, position) }
        }
    }
    
    /**
     * الحصول على الموضع المختار / Get selected position
     */
    fun getSelectedPosition(): Int = selectedPosition
    
    /**
     * الحصول على العنصر المختار / Get selected item
     */
    fun getSelectedItem(): GridItem? = items.getOrNull(selectedPosition)
    
    /**
     * الانتقال يميناً / Move right
     */
    fun moveRight(): Boolean {
        val newPos = selectedPosition + 1
        if (newPos < items.size) {
            setSelectedPosition(newPos)
            return true
        }
        return false
    }
    
    /**
     * الانتقال يساراً / Move left
     */
    fun moveLeft(): Boolean {
        val newPos = selectedPosition - 1
        if (newPos >= 0) {
            setSelectedPosition(newPos)
            return true
        }
        return false
    }
    
    /**
     * الانتقال للأسفل / Move down
     */
    fun moveDown(): Boolean {
        val newPos = selectedPosition + config.columns
        if (newPos < items.size) {
            setSelectedPosition(newPos)
            return true
        }
        return false
    }
    
    /**
     * الانتقال للأعلى / Move up
     */
    fun moveUp(): Boolean {
        val newPos = selectedPosition - config.columns
        if (newPos >= 0) {
            setSelectedPosition(newPos)
            return true
        }
        return false
    }
    
    /**
     * النقر على العنصر الحالي / Click current item
     */
    fun clickCurrentItem() {
        val item = items.getOrNull(selectedPosition) ?: return
        mainHandler.post { gridListener?.onItemClicked(item, selectedPosition) }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الترقيم
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تحميل المزيد / Load more
     */
    fun requestLoadMore() {
        if (currentPage < totalPages - 1) {
            currentPage++
            mainHandler.post { gridListener?.onLoadMore(currentPage) }
        }
    }
    
    /**
     * تعيين صفحة / Set page
     */
    fun setPage(page: GridPage) {
        if (page.page == 0) {
            setItems(page.items)
        } else {
            addItems(page.items)
        }
        totalPages = page.totalPages
    }
    
    /**
     * هل هناك المزيد؟ / Has more?
     */
    fun hasMore(): Boolean = currentPage < totalPages - 1
    
    // ═══════════════════════════════════════════════════════════════
    //                         الترتيب والفلترة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * ترتيب العناصر / Sort items
     */
    fun sortItems(order: SortOrder) {
        currentSort = order
        
        when (order) {
            SortOrder.NAME_ASC -> items.sortBy { it.title }
            SortOrder.NAME_DESC -> items.sortByDescending { it.title }
            else -> {} // CUSTOM وغيرها تحتاج تنفيذ خاص
        }
    }
    
    /**
     * إضافة فلتر / Add filter
     */
    fun addFilter(filter: GridFilter) {
        filters.removeAll { it.key == filter.key }
        filters.add(filter)
    }
    
    /**
     * إزالة فلتر / Remove filter
     */
    fun removeFilter(key: String) {
        filters.removeAll { it.key == key }
    }
    
    /**
     * مسح الفلاتر / Clear filters
     */
    fun clearFilters() {
        filters.clear()
    }
    
    /**
     * الحصول على الفلاتر / Get filters
     */
    fun getFilters(): List<GridFilter> = filters.toList()
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        items.clear()
        filters.clear()
        gridListener = null
        context = null
    }
}
