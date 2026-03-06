// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadCacheBridge.kt
// الوصف: جسر Kotlin للتخزين المؤقت
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.util.LruCache
import java.io.*
import java.security.MessageDigest
import java.util.concurrent.ConcurrentHashMap

/**
 * جسر التخزين المؤقت — SadCacheBridge
 */
object SadCacheBridge {
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // الذاكرة المؤقتة (Memory Cache)
    // ═══════════════════════════════════════════════════════════════════════════════
    
    private val memoryCaches = ConcurrentHashMap<String, LruCache<String, Any>>()
    private val simpleCaches = ConcurrentHashMap<String, ConcurrentHashMap<String, CacheEntry>>()
    
    data class CacheEntry(
        val value: Any,
        val createdAt: Long,
        val expiresAt: Long?
    ) {
        fun isExpired(): Boolean {
            return expiresAt != null && System.currentTimeMillis() > expiresAt
        }
    }
    
    @JvmStatic
    fun createMemoryCache(name: String, maxSize: Int = 100): Boolean {
        if (memoryCaches.containsKey(name)) return false
        
        memoryCaches[name] = object : LruCache<String, Any>(maxSize) {
            override fun sizeOf(key: String, value: Any): Int = 1
        }
        return true
    }
    
    @JvmStatic
    fun createSimpleCache(name: String): Boolean {
        if (simpleCaches.containsKey(name)) return false
        simpleCaches[name] = ConcurrentHashMap()
        return true
    }
    
    @JvmStatic
    fun put(cacheName: String, key: String, value: Any, ttlMillis: Long? = null): Boolean {
        // Try memory cache first
        memoryCaches[cacheName]?.let { cache ->
            cache.put(key, value)
            return true
        }
        
        // Try simple cache
        simpleCaches[cacheName]?.let { cache ->
            val expiresAt = ttlMillis?.let { System.currentTimeMillis() + it }
            cache[key] = CacheEntry(value, System.currentTimeMillis(), expiresAt)
            return true
        }
        
        return false
    }
    
    @JvmStatic
    fun get(cacheName: String, key: String): Any? {
        // Try memory cache first
        memoryCaches[cacheName]?.get(key)?.let { return it }
        
        // Try simple cache
        simpleCaches[cacheName]?.get(key)?.let { entry ->
            if (entry.isExpired()) {
                simpleCaches[cacheName]?.remove(key)
                return null
            }
            return entry.value
        }
        
        return null
    }
    
    @JvmStatic
    fun remove(cacheName: String, key: String): Boolean {
        memoryCaches[cacheName]?.remove(key)?.let { return true }
        simpleCaches[cacheName]?.remove(key)?.let { return true }
        return false
    }
    
    @JvmStatic
    fun clear(cacheName: String): Boolean {
        memoryCaches[cacheName]?.evictAll()?.let { return true }
        simpleCaches[cacheName]?.clear()?.let { return true }
        return false
    }
    
    @JvmStatic
    fun contains(cacheName: String, key: String): Boolean {
        memoryCaches[cacheName]?.get(key)?.let { return true }
        simpleCaches[cacheName]?.get(key)?.let { entry ->
            if (entry.isExpired()) {
                simpleCaches[cacheName]?.remove(key)
                return false
            }
            return true
        }
        return false
    }
    
    @JvmStatic
    fun size(cacheName: String): Int {
        memoryCaches[cacheName]?.let { return it.size() }
        simpleCaches[cacheName]?.let { return it.size }
        return -1
    }
    
    @JvmStatic
    fun keys(cacheName: String): List<String> {
        memoryCaches[cacheName]?.snapshot()?.keys?.let { return it.toList() }
        simpleCaches[cacheName]?.keys?.let { return it.toList() }
        return emptyList()
    }
    
    @JvmStatic
    fun deleteCache(cacheName: String): Boolean {
        memoryCaches.remove(cacheName)?.let { return true }
        simpleCaches.remove(cacheName)?.let { return true }
        return false
    }
    
    @JvmStatic
    fun clearAllCaches() {
        memoryCaches.forEach { (_, cache) -> cache.evictAll() }
        simpleCaches.forEach { (_, cache) -> cache.clear() }
    }
    
    @JvmStatic
    fun cleanExpired(cacheName: String): Int {
        var removed = 0
        simpleCaches[cacheName]?.let { cache ->
            val keysToRemove = cache.filter { it.value.isExpired() }.keys
            keysToRemove.forEach { 
                cache.remove(it)
                removed++
            }
        }
        return removed
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // التخزين على القرص (Disk Cache)
    // ═══════════════════════════════════════════════════════════════════════════════
    
    private var diskCacheDir: File? = null
    
    @JvmStatic
    fun initDiskCache(context: Context, subDir: String = "sad_cache") {
        diskCacheDir = File(context.cacheDir, subDir).apply {
            if (!exists()) mkdirs()
        }
    }
    
    @JvmStatic
    fun initDiskCacheExternal(context: Context, subDir: String = "sad_cache") {
        diskCacheDir = File(context.externalCacheDir, subDir).apply {
            if (!exists()) mkdirs()
        }
    }
    
    private fun getCacheFile(key: String): File? {
        val dir = diskCacheDir ?: return null
        val hash = md5(key)
        return File(dir, hash)
    }
    
    private fun md5(input: String): String {
        val md = MessageDigest.getInstance("MD5")
        val digest = md.digest(input.toByteArray())
        return digest.joinToString("") { "%02x".format(it) }
    }
    
    @JvmStatic
    fun putToDisk(key: String, data: ByteArray): Boolean {
        val file = getCacheFile(key) ?: return false
        return try {
            FileOutputStream(file).use { it.write(data) }
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun putStringToDisk(key: String, data: String): Boolean {
        return putToDisk(key, data.toByteArray(Charsets.UTF_8))
    }
    
    @JvmStatic
    fun getFromDisk(key: String): ByteArray? {
        val file = getCacheFile(key) ?: return null
        if (!file.exists()) return null
        return try {
            FileInputStream(file).use { it.readBytes() }
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun getStringFromDisk(key: String): String? {
        return getFromDisk(key)?.toString(Charsets.UTF_8)
    }
    
    @JvmStatic
    fun existsOnDisk(key: String): Boolean {
        val file = getCacheFile(key) ?: return false
        return file.exists()
    }
    
    @JvmStatic
    fun removeFromDisk(key: String): Boolean {
        val file = getCacheFile(key) ?: return false
        return file.delete()
    }
    
    @JvmStatic
    fun clearDiskCache(): Int {
        val dir = diskCacheDir ?: return 0
        var count = 0
        dir.listFiles()?.forEach { file ->
            if (file.delete()) count++
        }
        return count
    }
    
    @JvmStatic
    fun getDiskCacheSize(): Long {
        val dir = diskCacheDir ?: return 0
        return dir.listFiles()?.sumOf { it.length() } ?: 0
    }
    
    @JvmStatic
    fun getDiskCacheSizeFormatted(): String {
        val size = getDiskCacheSize()
        return when {
            size < 1024 -> "$size B"
            size < 1024 * 1024 -> "${size / 1024} KB"
            else -> "${size / (1024 * 1024)} MB"
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // الكائنات القابلة للتسلسل
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun putSerializable(key: String, obj: Serializable): Boolean {
        val file = getCacheFile(key) ?: return false
        return try {
            ObjectOutputStream(FileOutputStream(file)).use { it.writeObject(obj) }
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    @Suppress("UNCHECKED_CAST")
    fun <T : Serializable> getSerializable(key: String): T? {
        val file = getCacheFile(key) ?: return null
        if (!file.exists()) return null
        return try {
            ObjectInputStream(FileInputStream(file)).use { it.readObject() as T }
        } catch (e: Exception) {
            null
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // التخزين المؤقت مع TTL (Time To Live)
    // ═══════════════════════════════════════════════════════════════════════════════
    
    data class DiskCacheEntry(
        val data: ByteArray,
        val createdAt: Long,
        val ttlMillis: Long?
    ) : Serializable {
        fun isExpired(): Boolean {
            return ttlMillis != null && System.currentTimeMillis() > (createdAt + ttlMillis)
        }
    }
    
    @JvmStatic
    fun putWithTTL(key: String, data: ByteArray, ttlMillis: Long): Boolean {
        val entry = DiskCacheEntry(data, System.currentTimeMillis(), ttlMillis)
        return putSerializable("ttl_$key", entry)
    }
    
    @JvmStatic
    fun putStringWithTTL(key: String, data: String, ttlMillis: Long): Boolean {
        return putWithTTL(key, data.toByteArray(Charsets.UTF_8), ttlMillis)
    }
    
    @JvmStatic
    fun getWithTTL(key: String): ByteArray? {
        val entry = getSerializable<DiskCacheEntry>("ttl_$key") ?: return null
        if (entry.isExpired()) {
            removeFromDisk("ttl_$key")
            return null
        }
        return entry.data
    }
    
    @JvmStatic
    fun getStringWithTTL(key: String): String? {
        return getWithTTL(key)?.toString(Charsets.UTF_8)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // إحصائيات التخزين المؤقت
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getCacheStats(): Map<String, Any> {
        val stats = mutableMapOf<String, Any>()
        
        stats["memoryCacheCount"] = memoryCaches.size
        stats["simpleCacheCount"] = simpleCaches.size
        
        var totalMemoryItems = 0
        memoryCaches.forEach { (name, cache) ->
            stats["memoryCache_$name"] = cache.size()
            totalMemoryItems += cache.size()
        }
        stats["totalMemoryItems"] = totalMemoryItems
        
        var totalSimpleItems = 0
        simpleCaches.forEach { (name, cache) ->
            stats["simpleCache_$name"] = cache.size
            totalSimpleItems += cache.size
        }
        stats["totalSimpleItems"] = totalSimpleItems
        
        stats["diskCacheSize"] = getDiskCacheSize()
        stats["diskCacheSizeFormatted"] = getDiskCacheSizeFormatted()
        
        return stats
    }
    
    @JvmStatic
    fun getCacheStatsArabic(): String {
        val stats = getCacheStats()
        return buildString {
            appendLine("═══════════════════════════════════════")
            appendLine("إحصائيات التخزين المؤقت")
            appendLine("═══════════════════════════════════════")
            appendLine("عدد ذاكرة التخزين المؤقت: ${stats["memoryCacheCount"]}")
            appendLine("عدد التخزين البسيط: ${stats["simpleCacheCount"]}")
            appendLine("إجمالي عناصر الذاكرة: ${stats["totalMemoryItems"]}")
            appendLine("إجمالي العناصر البسيطة: ${stats["totalSimpleItems"]}")
            appendLine("حجم تخزين القرص: ${stats["diskCacheSizeFormatted"]}")
            appendLine("═══════════════════════════════════════")
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // التخزين المؤقت الذكي (مع كلا الطبقتين)
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun smartPut(key: String, data: String, memoryTTL: Long? = null, persistToDisk: Boolean = true): Boolean {
        // Store in memory
        if (!simpleCaches.containsKey("smart")) {
            createSimpleCache("smart")
        }
        put("smart", key, data, memoryTTL)
        
        // Store on disk if requested
        if (persistToDisk) {
            putStringToDisk(key, data)
        }
        
        return true
    }
    
    @JvmStatic
    fun smartGet(key: String): String? {
        // Try memory first
        val memoryResult = get("smart", key) as? String
        if (memoryResult != null) return memoryResult
        
        // Fall back to disk
        val diskResult = getStringFromDisk(key)
        if (diskResult != null) {
            // Restore to memory
            put("smart", key, diskResult, null)
        }
        return diskResult
    }
    
    @JvmStatic
    fun smartRemove(key: String) {
        remove("smart", key)
        removeFromDisk(key)
    }
    
    @JvmStatic
    fun smartClear() {
        clear("smart")
        clearDiskCache()
    }
}
