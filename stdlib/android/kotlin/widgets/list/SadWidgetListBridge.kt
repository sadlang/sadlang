package sad.android.widgets.list

import android.content.Context
import android.content.Intent
import android.os.Handler
import android.os.Looper
import android.widget.RemoteViews
import android.widget.RemoteViewsService
import java.util.concurrent.ConcurrentHashMap

/**
 * جسر قائمة الويدجت - قوائم ديناميكية في الويدجت
 * Widget List Bridge - Dynamic lists in widgets
 * 
 * @author فريق لغة ص
 */
object SadWidgetListBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private val listDataProviders = ConcurrentHashMap<String, ListDataProvider>()
    private val listItemTemplates = ConcurrentHashMap<String, Int>()
    
    /**
     * عنصر القائمة / List item
     */
    data class ListItem(
        val id: String,
        val data: Map<String, Any?> = emptyMap(),
        val fillInIntent: Intent? = null
    )
    
    /**
     * مزود البيانات / Data provider
     */
    interface ListDataProvider {
        fun getCount(): Int
        fun getItem(position: Int): ListItem?
        fun getItemId(position: Int): Long
        fun hasStableIds(): Boolean = true
        fun getViewTypeCount(): Int = 1
        fun getItemViewType(position: Int): Int = 0
        fun getLoadingView(): RemoteViews? = null
        fun onDataSetChanged()
    }
    
    /**
     * مزود بيانات بسيط / Simple data provider
     */
    class SimpleListDataProvider(
        private val items: MutableList<ListItem> = mutableListOf()
    ) : ListDataProvider {
        
        override fun getCount(): Int = items.size
        
        override fun getItem(position: Int): ListItem? {
            return if (position in 0 until items.size) items[position] else null
        }
        
        override fun getItemId(position: Int): Long = position.toLong()
        
        override fun onDataSetChanged() {
            // يمكن تحديث البيانات هنا
        }
        
        fun setItems(newItems: List<ListItem>) {
            items.clear()
            items.addAll(newItems)
        }
        
        fun addItem(item: ListItem) {
            items.add(item)
        }
        
        fun removeItem(id: String) {
            items.removeAll { it.id == id }
        }
        
        fun clearItems() {
            items.clear()
        }
        
        fun getItems(): List<ListItem> = items.toList()
    }
    
    /**
     * باني عنصر القائمة / List item builder
     */
    interface ListItemBuilder {
        fun buildItem(views: RemoteViews, item: ListItem, position: Int)
    }
    
    private val itemBuilders = ConcurrentHashMap<String, ListItemBuilder>()
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة المزودين
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تسجيل مزود / Register provider
     */
    fun registerDataProvider(listKey: String, provider: ListDataProvider): Boolean {
        listDataProviders[listKey] = provider
        return true
    }
    
    /**
     * الحصول على مزود / Get provider
     */
    fun getDataProvider(listKey: String): ListDataProvider? {
        return listDataProviders[listKey]
    }
    
    /**
     * إنشاء مزود بسيط / Create simple provider
     */
    fun createSimpleProvider(listKey: String, items: List<ListItem> = emptyList()): SimpleListDataProvider {
        val provider = SimpleListDataProvider(items.toMutableList())
        listDataProviders[listKey] = provider
        return provider
    }
    
    /**
     * تسجيل قالب العنصر / Register item template
     */
    fun registerItemTemplate(listKey: String, layoutResId: Int): Boolean {
        listItemTemplates[listKey] = layoutResId
        return true
    }
    
    /**
     * تسجيل باني العنصر / Register item builder
     */
    fun registerItemBuilder(listKey: String, builder: ListItemBuilder): Boolean {
        itemBuilders[listKey] = builder
        return true
    }
    
    /**
     * الحصول على قالب العنصر / Get item template
     */
    fun getItemTemplate(listKey: String): Int? {
        return listItemTemplates[listKey]
    }
    
    /**
     * الحصول على باني العنصر / Get item builder
     */
    fun getItemBuilder(listKey: String): ListItemBuilder? {
        return itemBuilders[listKey]
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         تحديث القائمة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تحديث بيانات القائمة / Update list data
     */
    fun updateListData(listKey: String, items: List<ListItem>): Boolean {
        val provider = listDataProviders[listKey]
        if (provider is SimpleListDataProvider) {
            provider.setItems(items)
            return true
        }
        return false
    }
    
    /**
     * إضافة عنصر / Add item
     */
    fun addListItem(listKey: String, item: ListItem): Boolean {
        val provider = listDataProviders[listKey]
        if (provider is SimpleListDataProvider) {
            provider.addItem(item)
            return true
        }
        return false
    }
    
    /**
     * حذف عنصر / Remove item
     */
    fun removeListItem(listKey: String, itemId: String): Boolean {
        val provider = listDataProviders[listKey]
        if (provider is SimpleListDataProvider) {
            provider.removeItem(itemId)
            return true
        }
        return false
    }
    
    /**
     * مسح القائمة / Clear list
     */
    fun clearList(listKey: String): Boolean {
        val provider = listDataProviders[listKey]
        if (provider is SimpleListDataProvider) {
            provider.clearItems()
            return true
        }
        return false
    }
    
    /**
     * إعلام تغيير البيانات / Notify data changed
     */
    fun notifyDataChanged(listKey: String) {
        listDataProviders[listKey]?.onDataSetChanged()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         RemoteViews Service
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * مصنع RemoteViews / RemoteViews factory
     */
    abstract class SadRemoteViewsFactory(
        private val ctx: Context,
        private val listKey: String
    ) : RemoteViewsService.RemoteViewsFactory {
        
        protected val provider: ListDataProvider?
            get() = listDataProviders[listKey]
        
        protected val itemTemplate: Int?
            get() = listItemTemplates[listKey]
        
        protected val itemBuilder: ListItemBuilder?
            get() = itemBuilders[listKey]
        
        override fun onCreate() {}
        
        override fun onDataSetChanged() {
            provider?.onDataSetChanged()
        }
        
        override fun onDestroy() {}
        
        override fun getCount(): Int = provider?.getCount() ?: 0
        
        override fun getViewAt(position: Int): RemoteViews? {
            val template = itemTemplate ?: return null
            val item = provider?.getItem(position) ?: return null
            val builder = itemBuilder
            
            val views = RemoteViews(ctx.packageName, template)
            builder?.buildItem(views, item, position)
            
            // إضافة intent للنقر
            item.fillInIntent?.let { views.setOnClickFillInIntent(android.R.id.content, it) }
            
            return views
        }
        
        override fun getLoadingView(): RemoteViews? = provider?.getLoadingView()
        
        override fun getViewTypeCount(): Int = provider?.getViewTypeCount() ?: 1
        
        override fun getItemId(position: Int): Long = provider?.getItemId(position) ?: position.toLong()
        
        override fun hasStableIds(): Boolean = provider?.hasStableIds() ?: true
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إنشاء عنصر / Create item
     */
    fun createItem(id: String, vararg pairs: Pair<String, Any?>): ListItem {
        return ListItem(id, pairs.toMap())
    }
    
    /**
     * إنشاء عنصر مع intent / Create item with intent
     */
    fun createItemWithIntent(id: String, intent: Intent, vararg pairs: Pair<String, Any?>): ListItem {
        return ListItem(id, pairs.toMap(), intent)
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        listDataProviders.clear()
        listItemTemplates.clear()
        itemBuilders.clear()
        context = null
    }
}
