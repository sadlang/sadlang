// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadMathBridge.kt
// الوصف: جسر Kotlin للعمليات الرياضية
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import kotlin.math.*
import kotlin.random.Random

/**
 * جسر الرياضيات — SadMathBridge
 */
object SadMathBridge {
    
    // الثوابت
    @JvmStatic
    val PI: Double = kotlin.math.PI
    
    @JvmStatic
    val E: Double = kotlin.math.E
    
    @JvmStatic
    val GOLDEN_RATIO: Double = 1.618033988749895
    
    // العمليات الأساسية
    @JvmStatic
    fun abs(x: Double): Double = kotlin.math.abs(x)
    
    @JvmStatic
    fun absInt(x: Int): Int = kotlin.math.abs(x)
    
    @JvmStatic
    fun max(a: Double, b: Double): Double = maxOf(a, b)
    
    @JvmStatic
    fun min(a: Double, b: Double): Double = minOf(a, b)
    
    @JvmStatic
    fun maxOf(vararg values: Double): Double = values.maxOrNull() ?: 0.0
    
    @JvmStatic
    fun minOf(vararg values: Double): Double = values.minOrNull() ?: 0.0
    
    @JvmStatic
    fun clamp(value: Double, min: Double, max: Double): Double {
        return value.coerceIn(min, max)
    }
    
    // الأس واللوغاريتم
    @JvmStatic
    fun pow(base: Double, exponent: Double): Double = base.pow(exponent)
    
    @JvmStatic
    fun sqrt(x: Double): Double = kotlin.math.sqrt(x)
    
    @JvmStatic
    fun cbrt(x: Double): Double = kotlin.math.cbrt(x)
    
    @JvmStatic
    fun exp(x: Double): Double = kotlin.math.exp(x)
    
    @JvmStatic
    fun log(x: Double): Double = kotlin.math.ln(x)
    
    @JvmStatic
    fun log10(x: Double): Double = kotlin.math.log10(x)
    
    @JvmStatic
    fun log2(x: Double): Double = kotlin.math.log2(x)
    
    @JvmStatic
    fun logBase(x: Double, base: Double): Double = kotlin.math.log(x, base)
    
    // الدوال المثلثية (بالراديان)
    @JvmStatic
    fun sin(x: Double): Double = kotlin.math.sin(x)
    
    @JvmStatic
    fun cos(x: Double): Double = kotlin.math.cos(x)
    
    @JvmStatic
    fun tan(x: Double): Double = kotlin.math.tan(x)
    
    @JvmStatic
    fun asin(x: Double): Double = kotlin.math.asin(x)
    
    @JvmStatic
    fun acos(x: Double): Double = kotlin.math.acos(x)
    
    @JvmStatic
    fun atan(x: Double): Double = kotlin.math.atan(x)
    
    @JvmStatic
    fun atan2(y: Double, x: Double): Double = kotlin.math.atan2(y, x)
    
    @JvmStatic
    fun sinh(x: Double): Double = kotlin.math.sinh(x)
    
    @JvmStatic
    fun cosh(x: Double): Double = kotlin.math.cosh(x)
    
    @JvmStatic
    fun tanh(x: Double): Double = kotlin.math.tanh(x)
    
    // تحويل الزوايا
    @JvmStatic
    fun toRadians(degrees: Double): Double = Math.toRadians(degrees)
    
    @JvmStatic
    fun toDegrees(radians: Double): Double = Math.toDegrees(radians)
    
    // الدوال المثلثية (بالدرجات)
    @JvmStatic
    fun sinDeg(degrees: Double): Double = sin(toRadians(degrees))
    
    @JvmStatic
    fun cosDeg(degrees: Double): Double = cos(toRadians(degrees))
    
    @JvmStatic
    fun tanDeg(degrees: Double): Double = tan(toRadians(degrees))
    
    // التقريب
    @JvmStatic
    fun round(x: Double): Long = kotlin.math.round(x).toLong()
    
    @JvmStatic
    fun roundToDecimals(x: Double, decimals: Int): Double {
        val factor = 10.0.pow(decimals)
        return kotlin.math.round(x * factor) / factor
    }
    
    @JvmStatic
    fun floor(x: Double): Double = kotlin.math.floor(x)
    
    @JvmStatic
    fun ceil(x: Double): Double = kotlin.math.ceil(x)
    
    @JvmStatic
    fun truncate(x: Double): Double = kotlin.math.truncate(x)
    
    // الأعداد العشوائية
    @JvmStatic
    fun random(): Double = Random.nextDouble()
    
    @JvmStatic
    fun randomInt(max: Int): Int = Random.nextInt(max)
    
    @JvmStatic
    fun randomInt(min: Int, max: Int): Int = Random.nextInt(min, max)
    
    @JvmStatic
    fun randomDouble(min: Double, max: Double): Double {
        return min + Random.nextDouble() * (max - min)
    }
    
    @JvmStatic
    fun randomBool(): Boolean = Random.nextBoolean()
    
    @JvmStatic
    fun randomGaussian(): Double = Random.nextGaussian()
    
    // العاملي والتوافيق
    @JvmStatic
    fun factorial(n: Int): Long {
        if (n < 0) return -1
        if (n <= 1) return 1
        var result = 1L
        for (i in 2..n) {
            result *= i
        }
        return result
    }
    
    @JvmStatic
    fun permutations(n: Int, r: Int): Long {
        if (r > n) return 0
        return factorial(n) / factorial(n - r)
    }
    
    @JvmStatic
    fun combinations(n: Int, r: Int): Long {
        if (r > n) return 0
        return factorial(n) / (factorial(r) * factorial(n - r))
    }
    
    // القاسم المشترك الأكبر والمضاعف المشترك الأصغر
    @JvmStatic
    fun gcd(a: Int, b: Int): Int {
        var x = abs(a.toDouble()).toInt()
        var y = abs(b.toDouble()).toInt()
        while (y != 0) {
            val temp = y
            y = x % y
            x = temp
        }
        return x
    }
    
    @JvmStatic
    fun lcm(a: Int, b: Int): Int {
        return abs((a * b).toDouble()).toInt() / gcd(a, b)
    }
    
    // فحص الأعداد
    @JvmStatic
    fun isPrime(n: Int): Boolean {
        if (n < 2) return false
        if (n == 2) return true
        if (n % 2 == 0) return false
        
        val sqrt = sqrt(n.toDouble()).toInt()
        for (i in 3..sqrt step 2) {
            if (n % i == 0) return false
        }
        return true
    }
    
    @JvmStatic
    fun isEven(n: Int): Boolean = n % 2 == 0
    
    @JvmStatic
    fun isOdd(n: Int): Boolean = n % 2 != 0
    
    @JvmStatic
    fun isPerfectSquare(n: Int): Boolean {
        if (n < 0) return false
        val sqrt = sqrt(n.toDouble()).toInt()
        return sqrt * sqrt == n
    }
    
    // المسافة والهندسة
    @JvmStatic
    fun distance2D(x1: Double, y1: Double, x2: Double, y2: Double): Double {
        return sqrt((x2 - x1).pow(2) + (y2 - y1).pow(2))
    }
    
    @JvmStatic
    fun distance3D(x1: Double, y1: Double, z1: Double, x2: Double, y2: Double, z2: Double): Double {
        return sqrt((x2 - x1).pow(2) + (y2 - y1).pow(2) + (z2 - z1).pow(2))
    }
    
    @JvmStatic
    fun haversineDistance(lat1: Double, lon1: Double, lat2: Double, lon2: Double): Double {
        val R = 6371.0 // نصف قطر الأرض بالكيلومتر
        
        val dLat = toRadians(lat2 - lat1)
        val dLon = toRadians(lon2 - lon1)
        
        val a = sin(dLat / 2).pow(2) + 
                cos(toRadians(lat1)) * cos(toRadians(lat2)) * sin(dLon / 2).pow(2)
        val c = 2 * atan2(sqrt(a), sqrt(1 - a))
        
        return R * c
    }
    
    // الإحصاء
    @JvmStatic
    fun mean(values: List<Double>): Double {
        if (values.isEmpty()) return 0.0
        return values.sum() / values.size
    }
    
    @JvmStatic
    fun median(values: List<Double>): Double {
        if (values.isEmpty()) return 0.0
        val sorted = values.sorted()
        val mid = sorted.size / 2
        return if (sorted.size % 2 == 0) {
            (sorted[mid - 1] + sorted[mid]) / 2
        } else {
            sorted[mid]
        }
    }
    
    @JvmStatic
    fun mode(values: List<Double>): Double? {
        if (values.isEmpty()) return null
        return values.groupingBy { it }.eachCount().maxByOrNull { it.value }?.key
    }
    
    @JvmStatic
    fun variance(values: List<Double>): Double {
        if (values.isEmpty()) return 0.0
        val mean = mean(values)
        return values.map { (it - mean).pow(2) }.sum() / values.size
    }
    
    @JvmStatic
    fun standardDeviation(values: List<Double>): Double {
        return sqrt(variance(values))
    }
    
    @JvmStatic
    fun sum(values: List<Double>): Double = values.sum()
    
    @JvmStatic
    fun product(values: List<Double>): Double = values.reduce { acc, d -> acc * d }
    
    // التحويل
    @JvmStatic
    fun lerp(start: Double, end: Double, t: Double): Double {
        return start + (end - start) * t
    }
    
    @JvmStatic
    fun map(value: Double, inMin: Double, inMax: Double, outMin: Double, outMax: Double): Double {
        return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin
    }
    
    @JvmStatic
    fun normalize(value: Double, min: Double, max: Double): Double {
        return (value - min) / (max - min)
    }
}
