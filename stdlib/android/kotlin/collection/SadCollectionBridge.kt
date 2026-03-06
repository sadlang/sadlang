// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadCollectionBridge.kt
// الوصف: جسر Kotlin لعمليات المجموعات
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

/**
 * جسر المجموعات — SadCollectionBridge
 */
object SadCollectionBridge {
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // List Creation
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun <T> createList(vararg items: T): List<T> = listOf(*items)
    
    @JvmStatic
    fun <T> createMutableList(vararg items: T): MutableList<T> = mutableListOf(*items)
    
    @JvmStatic
    fun createIntRange(start: Int, end: Int): List<Int> = (start..end).toList()
    
    @JvmStatic
    fun createIntRangeStep(start: Int, end: Int, step: Int): List<Int> {
        return (start..end step step).toList()
    }
    
    @JvmStatic
    fun <T> createFilledList(size: Int, defaultValue: T): List<T> {
        return List(size) { defaultValue }
    }
    
    @JvmStatic
    fun createFilledIntList(size: Int, init: (Int) -> Int): List<Int> {
        return List(size) { init(it) }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Map Creation
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun <K, V> createMap(vararg pairs: Pair<K, V>): Map<K, V> = mapOf(*pairs)
    
    @JvmStatic
    fun <K, V> createMutableMap(vararg pairs: Pair<K, V>): MutableMap<K, V> = mutableMapOf(*pairs)
    
    @JvmStatic
    fun <K, V> emptyMap(): Map<K, V> = emptyMap()
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Set Creation
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun <T> createSet(vararg items: T): Set<T> = setOf(*items)
    
    @JvmStatic
    fun <T> createMutableSet(vararg items: T): MutableSet<T> = mutableSetOf(*items)
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // List Operations
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun <T> first(list: List<T>): T? = list.firstOrNull()
    
    @JvmStatic
    fun <T> last(list: List<T>): T? = list.lastOrNull()
    
    @JvmStatic
    fun <T> getAt(list: List<T>, index: Int): T? {
        return if (index in list.indices) list[index] else null
    }
    
    @JvmStatic
    fun <T> indexOf(list: List<T>, element: T): Int = list.indexOf(element)
    
    @JvmStatic
    fun <T> lastIndexOf(list: List<T>, element: T): Int = list.lastIndexOf(element)
    
    @JvmStatic
    fun <T> contains(list: List<T>, element: T): Boolean = list.contains(element)
    
    @JvmStatic
    fun <T> isEmpty(list: List<T>): Boolean = list.isEmpty()
    
    @JvmStatic
    fun <T> isNotEmpty(list: List<T>): Boolean = list.isNotEmpty()
    
    @JvmStatic
    fun <T> size(list: List<T>): Int = list.size
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Transformations
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun <T, R> map(list: List<T>, transform: (T) -> R): List<R> = list.map(transform)
    
    @JvmStatic
    fun <T> filter(list: List<T>, predicate: (T) -> Boolean): List<T> = list.filter(predicate)
    
    @JvmStatic
    fun <T> filterNot(list: List<T>, predicate: (T) -> Boolean): List<T> = list.filterNot(predicate)
    
    @JvmStatic
    @Suppress("UNCHECKED_CAST")
    fun <T : Any> filterNotNull(list: List<T?>): List<T> = list.filterNotNull()
    
    @JvmStatic
    fun <T, R> flatMap(list: List<T>, transform: (T) -> Iterable<R>): List<R> = list.flatMap(transform)
    
    @JvmStatic
    fun <T> flatten(list: List<List<T>>): List<T> = list.flatten()
    
    @JvmStatic
    fun <T> distinct(list: List<T>): List<T> = list.distinct()
    
    @JvmStatic
    fun <T, K> distinctBy(list: List<T>, selector: (T) -> K): List<T> = list.distinctBy(selector)
    
    @JvmStatic
    fun <T> reversed(list: List<T>): List<T> = list.reversed()
    
    @JvmStatic
    fun <T : Comparable<T>> sorted(list: List<T>): List<T> = list.sorted()
    
    @JvmStatic
    fun <T : Comparable<T>> sortedDescending(list: List<T>): List<T> = list.sortedDescending()
    
    @JvmStatic
    fun <T, R : Comparable<R>> sortedBy(list: List<T>, selector: (T) -> R): List<T> = list.sortedBy(selector)
    
    @JvmStatic
    fun <T, R : Comparable<R>> sortedByDescending(list: List<T>, selector: (T) -> R): List<T> = 
        list.sortedByDescending(selector)
    
    @JvmStatic
    fun <T> shuffled(list: List<T>): List<T> = list.shuffled()
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Slicing
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun <T> take(list: List<T>, n: Int): List<T> = list.take(n)
    
    @JvmStatic
    fun <T> takeLast(list: List<T>, n: Int): List<T> = list.takeLast(n)
    
    @JvmStatic
    fun <T> takeWhile(list: List<T>, predicate: (T) -> Boolean): List<T> = list.takeWhile(predicate)
    
    @JvmStatic
    fun <T> drop(list: List<T>, n: Int): List<T> = list.drop(n)
    
    @JvmStatic
    fun <T> dropLast(list: List<T>, n: Int): List<T> = list.dropLast(n)
    
    @JvmStatic
    fun <T> dropWhile(list: List<T>, predicate: (T) -> Boolean): List<T> = list.dropWhile(predicate)
    
    @JvmStatic
    fun <T> slice(list: List<T>, start: Int, end: Int): List<T> = list.subList(start, end)
    
    @JvmStatic
    fun <T> chunked(list: List<T>, size: Int): List<List<T>> = list.chunked(size)
    
    @JvmStatic
    fun <T> windowed(list: List<T>, size: Int, step: Int = 1): List<List<T>> = 
        list.windowed(size, step)
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Aggregations
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun <T, R> fold(list: List<T>, initial: R, operation: (R, T) -> R): R = 
        list.fold(initial, operation)
    
    @JvmStatic
    fun <T> reduce(list: List<T>, operation: (T, T) -> T): T? = 
        if (list.isEmpty()) null else list.reduce(operation)
    
    @JvmStatic
    fun sumOfInt(list: List<Int>): Int = list.sum()
    
    @JvmStatic
    fun sumOfDouble(list: List<Double>): Double = list.sum()
    
    @JvmStatic
    fun averageOfInt(list: List<Int>): Double = list.average()
    
    @JvmStatic
    fun averageOfDouble(list: List<Double>): Double = list.average()
    
    @JvmStatic
    fun <T : Comparable<T>> max(list: List<T>): T? = list.maxOrNull()
    
    @JvmStatic
    fun <T : Comparable<T>> min(list: List<T>): T? = list.minOrNull()
    
    @JvmStatic
    fun <T, R : Comparable<R>> maxBy(list: List<T>, selector: (T) -> R): T? = list.maxByOrNull(selector)
    
    @JvmStatic
    fun <T, R : Comparable<R>> minBy(list: List<T>, selector: (T) -> R): T? = list.minByOrNull(selector)
    
    @JvmStatic
    fun count(list: List<*>): Int = list.count()
    
    @JvmStatic
    fun <T> countWhere(list: List<T>, predicate: (T) -> Boolean): Int = list.count(predicate)
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Predicates
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun <T> all(list: List<T>, predicate: (T) -> Boolean): Boolean = list.all(predicate)
    
    @JvmStatic
    fun <T> any(list: List<T>, predicate: (T) -> Boolean): Boolean = list.any(predicate)
    
    @JvmStatic
    fun <T> none(list: List<T>, predicate: (T) -> Boolean): Boolean = list.none(predicate)
    
    @JvmStatic
    fun <T> find(list: List<T>, predicate: (T) -> Boolean): T? = list.find(predicate)
    
    @JvmStatic
    fun <T> findLast(list: List<T>, predicate: (T) -> Boolean): T? = list.findLast(predicate)
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Combining
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun <T> concat(list1: List<T>, list2: List<T>): List<T> = list1 + list2
    
    @JvmStatic
    fun <T> union(list1: List<T>, list2: List<T>): List<T> = (list1 + list2).distinct()
    
    @JvmStatic
    fun <T> intersect(list1: List<T>, list2: List<T>): List<T> = list1.filter { it in list2 }
    
    @JvmStatic
    fun <T> subtract(list1: List<T>, list2: List<T>): List<T> = list1.filter { it !in list2 }
    
    @JvmStatic
    fun <A, B> zip(list1: List<A>, list2: List<B>): List<Pair<A, B>> = list1.zip(list2)
    
    @JvmStatic
    fun <T> unzip(list: List<Pair<*, *>>): Pair<List<*>, List<*>> {
        val first = list.map { it.first }
        val second = list.map { it.second }
        return Pair(first, second)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Grouping
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun <T, K> groupBy(list: List<T>, keySelector: (T) -> K): Map<K, List<T>> = 
        list.groupBy(keySelector)
    
    @JvmStatic
    fun <T, K, V> groupBy(
        list: List<T>,
        keySelector: (T) -> K,
        valueTransform: (T) -> V
    ): Map<K, List<V>> = list.groupBy(keySelector, valueTransform)
    
    @JvmStatic
    fun <T> partition(list: List<T>, predicate: (T) -> Boolean): Pair<List<T>, List<T>> = 
        list.partition(predicate)
    
    @JvmStatic
    fun <T, K> associateBy(list: List<T>, keySelector: (T) -> K): Map<K, T> = 
        list.associateBy(keySelector)
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Conversion
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun <T> toSet(list: List<T>): Set<T> = list.toSet()
    
    @JvmStatic
    fun <T> toList(set: Set<T>): List<T> = set.toList()
    
    @JvmStatic
    fun <K, V> toList(map: Map<K, V>): List<Pair<K, V>> = map.toList()
    
    @JvmStatic
    fun <T> toMutableList(list: List<T>): MutableList<T> = list.toMutableList()
    
    @JvmStatic
    fun <T> toMutableSet(set: Set<T>): MutableSet<T> = set.toMutableSet()
    
    @JvmStatic
    fun <K, V> toMutableMap(map: Map<K, V>): MutableMap<K, V> = map.toMutableMap()
    
    @JvmStatic
    fun toIntArray(list: List<Int>): IntArray = list.toIntArray()
    
    @JvmStatic
    fun toDoubleArray(list: List<Double>): DoubleArray = list.toDoubleArray()
    
    @JvmStatic
    fun intArrayToList(array: IntArray): List<Int> = array.toList()
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // String Operations
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun <T> joinToString(
        list: List<T>,
        separator: String = ", ",
        prefix: String = "",
        postfix: String = ""
    ): String = list.joinToString(separator, prefix, postfix)
    
    @JvmStatic
    fun <T> joinToString(
        list: List<T>,
        separator: String,
        transform: (T) -> String
    ): String = list.joinToString(separator) { transform(it) }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Mutable Operations
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun <T> add(list: MutableList<T>, element: T): Boolean = list.add(element)
    
    @JvmStatic
    fun <T> addAt(list: MutableList<T>, index: Int, element: T) = list.add(index, element)
    
    @JvmStatic
    fun <T> addAll(list: MutableList<T>, elements: Collection<T>): Boolean = list.addAll(elements)
    
    @JvmStatic
    fun <T> remove(list: MutableList<T>, element: T): Boolean = list.remove(element)
    
    @JvmStatic
    fun <T> removeAt(list: MutableList<T>, index: Int): T = list.removeAt(index)
    
    @JvmStatic
    fun <T> removeAll(list: MutableList<T>, predicate: (T) -> Boolean): Boolean = 
        list.removeAll(predicate)
    
    @JvmStatic
    fun <T> clear(list: MutableList<T>) = list.clear()
    
    @JvmStatic
    fun <T> set(list: MutableList<T>, index: Int, element: T): T = list.set(index, element)
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Map Operations
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun <K, V> getKeys(map: Map<K, V>): Set<K> = map.keys
    
    @JvmStatic
    fun <K, V> getValues(map: Map<K, V>): Collection<V> = map.values
    
    @JvmStatic
    fun <K, V> getEntries(map: Map<K, V>): Set<Map.Entry<K, V>> = map.entries
    
    @JvmStatic
    fun <K, V> containsKey(map: Map<K, V>, key: K): Boolean = map.containsKey(key)
    
    @JvmStatic
    fun <K, V> containsValue(map: Map<K, V>, value: V): Boolean = map.containsValue(value)
    
    @JvmStatic
    fun <K, V> getOrDefault(map: Map<K, V>, key: K, defaultValue: V): V = 
        map.getOrDefault(key, defaultValue)
    
    @JvmStatic
    fun <K, V> mapKeys(map: Map<K, V>, transform: (Map.Entry<K, V>) -> K): Map<K, V> = 
        map.mapKeys { transform(it) }
    
    @JvmStatic
    fun <K, V, R> mapValues(map: Map<K, V>, transform: (Map.Entry<K, V>) -> R): Map<K, R> = 
        map.mapValues { transform(it) }
    
    @JvmStatic
    fun <K, V> filterKeys(map: Map<K, V>, predicate: (K) -> Boolean): Map<K, V> = 
        map.filterKeys(predicate)
    
    @JvmStatic
    fun <K, V> filterValues(map: Map<K, V>, predicate: (V) -> Boolean): Map<K, V> = 
        map.filterValues(predicate)
}
