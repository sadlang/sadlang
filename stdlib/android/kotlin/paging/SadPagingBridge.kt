// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadPagingBridge.kt
// الوصف: جسر Kotlin لمكتبة Paging (التحميل التدريجي)
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import androidx.lifecycle.LifecycleOwner
import androidx.lifecycle.lifecycleScope
import androidx.paging.*
import kotlinx.coroutines.flow.*
import kotlinx.coroutines.launch

/**
 * جسر Paging — SadPagingBridge
 */
object SadPagingBridge {
    
    private var context: Context? = null
    
    // Store paging sources
    private val pagingSources = mutableMapOf<String, SadPagingSource<*>>()
    private val pagers = mutableMapOf<String, Pager<Int, *>>()
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Paging Source Creation
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun <T : Any> createPagingSource(
        key: String,
        pageSize: Int = 20,
        loadPage: suspend (page: Int, pageSize: Int) -> PagingResult<T>
    ): String {
        val source = SadPagingSource(loadPage)
        pagingSources[key] = source
        
        val pager = Pager(
            config = PagingConfig(
                pageSize = pageSize,
                enablePlaceholders = false,
                initialLoadSize = pageSize
            ),
            pagingSourceFactory = { source }
        )
        
        pagers[key] = pager
        return key
    }
    
    @JvmStatic
    fun createPagingConfig(
        pageSize: Int = 20,
        prefetchDistance: Int = 10,
        enablePlaceholders: Boolean = false,
        initialLoadSize: Int = 20,
        maxSize: Int = PagingConfig.MAX_SIZE_UNBOUNDED,
        jumpThreshold: Int = PagingConfig.JUMP_THRESHOLD_UNBOUNDED
    ): PagingConfig {
        return PagingConfig(
            pageSize = pageSize,
            prefetchDistance = prefetchDistance,
            enablePlaceholders = enablePlaceholders,
            initialLoadSize = initialLoadSize,
            maxSize = maxSize,
            jumpThreshold = jumpThreshold
        )
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Collect Data
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    @Suppress("UNCHECKED_CAST")
    fun <T : Any> collectPagingData(
        key: String,
        lifecycleOwner: LifecycleOwner,
        onPageLoaded: (List<T>) -> Unit,
        onLoadStateChanged: ((LoadState) -> Unit)? = null
    ) {
        val pager = pagers[key] as? Pager<Int, T> ?: return
        
        lifecycleOwner.lifecycleScope.launch {
            pager.flow.collectLatest { pagingData ->
                val items = mutableListOf<T>()
                
                pagingData.map { item ->
                    items.add(item)
                    item
                }
                
                onPageLoaded(items)
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Load State
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getLoadStateDescription(loadState: LoadState): String {
        return when (loadState) {
            is LoadState.Loading -> "جاري التحميل"
            is LoadState.NotLoading -> if (loadState.endOfPaginationReached) "نهاية البيانات" else "مكتمل"
            is LoadState.Error -> "خطأ: ${loadState.error.message}"
        }
    }
    
    @JvmStatic
    fun isLoading(loadState: LoadState): Boolean {
        return loadState is LoadState.Loading
    }
    
    @JvmStatic
    fun isError(loadState: LoadState): Boolean {
        return loadState is LoadState.Error
    }
    
    @JvmStatic
    fun isEndOfPagination(loadState: LoadState): Boolean {
        return loadState is LoadState.NotLoading && loadState.endOfPaginationReached
    }
    
    @JvmStatic
    fun getErrorMessage(loadState: LoadState): String? {
        return (loadState as? LoadState.Error)?.error?.message
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Refresh & Retry
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun refresh(key: String) {
        pagingSources[key]?.invalidate()
    }
    
    @JvmStatic
    fun removePagingSource(key: String) {
        pagingSources.remove(key)
        pagers.remove(key)
    }
    
    @JvmStatic
    fun clearAll() {
        pagingSources.clear()
        pagers.clear()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Simple List Paging
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun <T : Any> createListPagingSource(
        key: String,
        items: List<T>,
        pageSize: Int = 20
    ): String {
        return createPagingSource(key, pageSize) { page, size ->
            val startIndex = page * size
            val endIndex = minOf(startIndex + size, items.size)
            
            if (startIndex >= items.size) {
                PagingResult(
                    data = emptyList(),
                    hasMore = false
                )
            } else {
                PagingResult(
                    data = items.subList(startIndex, endIndex),
                    hasMore = endIndex < items.size
                )
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Pagination Info
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getPagingSourceCount(): Int {
        return pagingSources.size
    }
    
    @JvmStatic
    fun getPagingSourceKeys(): List<String> {
        return pagingSources.keys.toList()
    }
    
    @JvmStatic
    fun hasPagingSource(key: String): Boolean {
        return pagingSources.containsKey(key)
    }
}

/**
 * Result class for paging
 */
data class PagingResult<T>(
    val data: List<T>,
    val hasMore: Boolean,
    val error: Throwable? = null
)

/**
 * Custom PagingSource for Sad bridges
 */
class SadPagingSource<T : Any>(
    private val loadPage: suspend (page: Int, pageSize: Int) -> PagingResult<T>
) : PagingSource<Int, T>() {
    
    override suspend fun load(params: LoadParams<Int>): LoadResult<Int, T> {
        val page = params.key ?: 0
        
        return try {
            val result = loadPage(page, params.loadSize)
            
            if (result.error != null) {
                LoadResult.Error(result.error)
            } else {
                LoadResult.Page(
                    data = result.data,
                    prevKey = if (page > 0) page - 1 else null,
                    nextKey = if (result.hasMore) page + 1 else null
                )
            }
        } catch (e: Exception) {
            LoadResult.Error(e)
        }
    }
    
    override fun getRefreshKey(state: PagingState<Int, T>): Int? {
        return state.anchorPosition?.let { anchorPosition ->
            state.closestPageToPosition(anchorPosition)?.prevKey?.plus(1)
                ?: state.closestPageToPosition(anchorPosition)?.nextKey?.minus(1)
        }
    }
}
