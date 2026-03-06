// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadArrayBridge.kt
// الوصف: جسر Kotlin لعمليات المصفوفات
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

/**
 * جسر المصفوفات — SadArrayBridge
 */
object SadArrayBridge {
    
    // الإنشاء
    @JvmStatic
    fun <T> create(vararg elements: T): List<T> = elements.toList()
    
    @JvmStatic
    fun range(start: Int, end: Int, step: Int = 1): List<Int> {
        return (start until end step step).toList()
    }
    
    @JvmStatic
    fun rangeClosed(start: Int, end: Int, step: Int = 1): List<Int> {
        return (start..end step step).toList()
    }
    
    @JvmStatic
    fun repeat(value: Any, times: Int): List<Any> {
        return List(times) { value }
    }
    
    @JvmStatic
    fun fill(size: Int, value: Any): List<Any> = repeat(value, size)
    
    // الوصول
    @JvmStatic
    fun <T> get(list: List<T>, index: Int): T? {
        return list.getOrNull(index)
    }
    
    @JvmStatic
    fun <T> first(list: List<T>): T? = list.firstOrNull()
    
    @JvmStatic
    fun <T> last(list: List<T>): T? = list.lastOrNull()
    
    @JvmStatic
    fun <T> take(list: List<T>, n: Int): List<T> = list.take(n)
    
    @JvmStatic
    fun <T> takeLast(list: List<T>, n: Int): List<T> = list.takeLast(n)
    
    @JvmStatic
    fun <T> drop(list: List<T>, n: Int): List<T> = list.drop(n)
    
    @JvmStatic
    fun <T> dropLast(list: List<T>, n: Int): List<T> = list.dropLast(n)
    
    @JvmStatic
    fun <T> slice(list: List<T>, start: Int, end: Int): List<T> {
        return list.subList(start, minOf(end, list.size))
    }
    
    // التعديل (إرجاع قائمة جديدة)
    @JvmStatic
    fun <T> add(list: List<T>, element: T): List<T> = list + element
    
    @JvmStatic
    fun <T> addAll(list: List<T>, elements: List<T>): List<T> = list + elements
    
    @JvmStatic
    fun <T> insert(list: List<T>, index: Int, element: T): List<T> {
        val mutable = list.toMutableList()
        mutable.add(index, element)
        return mutable
    }
    
    @JvmStatic
    fun <T> remove(list: List<T>, element: T): List<T> = list - element
    
    @JvmStatic
    fun <T> removeAt(list: List<T>, index: Int): List<T> {
        val mutable = list.toMutableList()
        mutable.removeAt(index)
        return mutable
    }
    
    @JvmStatic
    fun <T> set(list: List<T>, index: Int, element: T): List<T> {
        val mutable = list.toMutableList()
        mutable[index] = element
        return mutable
    }
    
    // البحث
    @JvmStatic
    fun <T> indexOf(list: List<T>, element: T): Int = list.indexOf(element)
    
    @JvmStatic
    fun <T> lastIndexOf(list: List<T>, element: T): Int = list.lastIndexOf(element)
    
    @JvmStatic
    fun <T> contains(list: List<T>, element: T): Boolean = list.contains(element)
    
    @JvmStatic
    fun <T> containsAll(list: List<T>, elements: List<T>): Boolean = list.containsAll(elements)
    
    @JvmStatic
    fun <T> count(list: List<T>): Int = list.size
    
    @JvmStatic
    fun <T> countMatches(list: List<T>, element: T): Int = list.count { it == element }
    
    @JvmStatic
    fun <T> isEmpty(list: List<T>): Boolean = list.isEmpty()
    
    @JvmStatic
    fun <T> isNotEmpty(list: List<T>): Boolean = list.isNotEmpty()
    
    // الترتيب
    @JvmStatic
    fun <T : Comparable<T>> sort(list: List<T>): List<T> = list.sorted()
    
    @JvmStatic
    fun <T : Comparable<T>> sortDescending(list: List<T>): List<T> = list.sortedDescending()
    
    @JvmStatic
    fun <T> reverse(list: List<T>): List<T> = list.reversed()
    
    @JvmStatic
    fun <T> shuffle(list: List<T>): List<T> = list.shuffled()
    
    // التحويل
    @JvmStatic
    fun <T> distinct(list: List<T>): List<T> = list.distinct()
    
    @JvmStatic
    fun <T> flatten(list: List<List<T>>): List<T> = list.flatten()
    
    @JvmStatic
    fun <T> chunked(list: List<T>, size: Int): List<List<T>> = list.chunked(size)
    
    @JvmStatic
    fun <T> windowed(list: List<T>, size: Int, step: Int = 1): List<List<T>> {
        return list.windowed(size, step, partialWindows = false)
    }
    
    @JvmStatic
    fun <T> zip(list1: List<T>, list2: List<T>): List<Pair<T, T>> = list1.zip(list2)
    
    @JvmStatic
    fun <T> unzip(pairs: List<Pair<T, T>>): Pair<List<T>, List<T>> = pairs.unzip()
    
    // الدمج
    @JvmStatic
    fun <T> concat(vararg lists: List<T>): List<T> = lists.flatMap { it }
    
    @JvmStatic
    fun <T> join(list: List<T>, separator: String = ", "): String = list.joinToString(separator)
    
    // العمليات المجمعة
    @JvmStatic
    fun sum(list: List<Number>): Double = list.sumOf { it.toDouble() }
    
    @JvmStatic
    fun average(list: List<Number>): Double = list.map { it.toDouble() }.average()
    
    @JvmStatic
    fun <T : Comparable<T>> max(list: List<T>): T? = list.maxOrNull()
    
    @JvmStatic
    fun <T : Comparable<T>> min(list: List<T>): T? = list.minOrNull()
    
    @JvmStatic
    fun product(list: List<Number>): Double {
        return list.map { it.toDouble() }.reduce { acc, d -> acc * d }
    }
    
    // عمليات المجموعات
    @JvmStatic
    fun <T> union(list1: List<T>, list2: List<T>): List<T> = (list1 + list2).distinct()
    
    @JvmStatic
    fun <T> intersect(list1: List<T>, list2: List<T>): List<T> = list1.intersect(list2.toSet()).toList()
    
    @JvmStatic
    fun <T> difference(list1: List<T>, list2: List<T>): List<T> = list1.minus(list2.toSet())
    
    @JvmStatic
    fun <T> symmetricDifference(list1: List<T>, list2: List<T>): List<T> {
        return (list1.minus(list2.toSet())) + (list2.minus(list1.toSet()))
    }
    
    // التحقق
    @JvmStatic
    fun <T> allMatch(list: List<T>, predicate: (T) -> Boolean): Boolean = list.all(predicate)
    
    @JvmStatic
    fun <T> anyMatch(list: List<T>, predicate: (T) -> Boolean): Boolean = list.any(predicate)
    
    @JvmStatic
    fun <T> noneMatch(list: List<T>, predicate: (T) -> Boolean): Boolean = list.none(predicate)
    
    // التجميع
    @JvmStatic
    fun <T, K> groupBy(list: List<T>, keySelector: (T) -> K): Map<K, List<T>> = list.groupBy(keySelector)
    
    @JvmStatic
    fun <T> partition(list: List<T>, predicate: (T) -> Boolean): Pair<List<T>, List<T>> {
        return list.partition(predicate)
    }
    
    // التكرار
    @JvmStatic
    fun <T> forEach(list: List<T>, action: (T) -> Unit) = list.forEach(action)
    
    @JvmStatic
    fun <T> forEachIndexed(list: List<T>, action: (Int, T) -> Unit) = list.forEachIndexed(action)
    
    // التحويل إلى أنواع أخرى
    @JvmStatic
    fun <T> toSet(list: List<T>): Set<T> = list.toSet()
    
    @JvmStatic
    fun <T> toMutable(list: List<T>): MutableList<T> = list.toMutableList()
    
    @JvmStatic
    fun toIntArray(list: List<Int>): IntArray = list.toIntArray()
    
    @JvmStatic
    fun toDoubleArray(list: List<Double>): DoubleArray = list.toDoubleArray()
    
    @JvmStatic
    fun toStringArray(list: List<String>): Array<String> = list.toTypedArray()
}
