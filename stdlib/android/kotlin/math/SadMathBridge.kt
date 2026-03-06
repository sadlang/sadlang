// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadMathBridge.kt
// الوصف: جسر Kotlin للعمليات الرياضية
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import kotlin.math.*

/**
 * جسر الرياضيات — SadMathBridge
 */
object SadMathBridge {
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // الثوابت الرياضية
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    val PI: Double = Math.PI
    
    @JvmStatic
    val E: Double = Math.E
    
    @JvmStatic
    val TAU: Double = 2 * Math.PI
    
    @JvmStatic
    val GOLDEN_RATIO: Double = (1 + sqrt(5.0)) / 2
    
    @JvmStatic
    val SQRT2: Double = sqrt(2.0)
    
    @JvmStatic
    val SQRT3: Double = sqrt(3.0)
    
    @JvmStatic
    val LN2: Double = ln(2.0)
    
    @JvmStatic
    val LN10: Double = ln(10.0)
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // العمليات الأساسية
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun abs(x: Double): Double = kotlin.math.abs(x)
    
    @JvmStatic
    fun absInt(x: Int): Int = kotlin.math.abs(x)
    
    @JvmStatic
    fun sign(x: Double): Double = kotlin.math.sign(x)
    
    @JvmStatic
    fun signInt(x: Int): Int = kotlin.math.sign(x)
    
    @JvmStatic
    fun max(a: Double, b: Double): Double = maxOf(a, b)
    
    @JvmStatic
    fun maxInt(a: Int, b: Int): Int = maxOf(a, b)
    
    @JvmStatic
    fun min(a: Double, b: Double): Double = minOf(a, b)
    
    @JvmStatic
    fun minInt(a: Int, b: Int): Int = minOf(a, b)
    
    @JvmStatic
    fun clamp(value: Double, min: Double, max: Double): Double {
        return when {
            value < min -> min
            value > max -> max
            else -> value
        }
    }
    
    @JvmStatic
    fun clampInt(value: Int, min: Int, max: Int): Int {
        return when {
            value < min -> min
            value > max -> max
            else -> value
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // التقريب
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun round(x: Double): Long = kotlin.math.round(x).toLong()
    
    @JvmStatic
    fun roundToInt(x: Double): Int = kotlin.math.round(x).toInt()
    
    @JvmStatic
    fun floor(x: Double): Double = kotlin.math.floor(x)
    
    @JvmStatic
    fun floorToInt(x: Double): Int = kotlin.math.floor(x).toInt()
    
    @JvmStatic
    fun ceil(x: Double): Double = kotlin.math.ceil(x)
    
    @JvmStatic
    fun ceilToInt(x: Double): Int = kotlin.math.ceil(x).toInt()
    
    @JvmStatic
    fun truncate(x: Double): Double = kotlin.math.truncate(x)
    
    @JvmStatic
    fun roundToDecimal(x: Double, decimalPlaces: Int): Double {
        val factor = 10.0.pow(decimalPlaces)
        return kotlin.math.round(x * factor) / factor
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // القوى والجذور
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun pow(base: Double, exponent: Double): Double = base.pow(exponent)
    
    @JvmStatic
    fun powInt(base: Double, exponent: Int): Double = base.pow(exponent)
    
    @JvmStatic
    fun sqrt(x: Double): Double = kotlin.math.sqrt(x)
    
    @JvmStatic
    fun cbrt(x: Double): Double = kotlin.math.cbrt(x)
    
    @JvmStatic
    fun nthRoot(x: Double, n: Double): Double {
        return if (x < 0 && n.toInt() % 2 == 1) {
            -abs(x).pow(1.0 / n)
        } else {
            x.pow(1.0 / n)
        }
    }
    
    @JvmStatic
    fun square(x: Double): Double = x * x
    
    @JvmStatic
    fun cube(x: Double): Double = x * x * x
    
    @JvmStatic
    fun exp(x: Double): Double = kotlin.math.exp(x)
    
    @JvmStatic
    fun expm1(x: Double): Double = kotlin.math.expm1(x)
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // اللوغاريتمات
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun ln(x: Double): Double = kotlin.math.ln(x)
    
    @JvmStatic
    fun ln1p(x: Double): Double = kotlin.math.ln1p(x)
    
    @JvmStatic
    fun log10(x: Double): Double = kotlin.math.log10(x)
    
    @JvmStatic
    fun log2(x: Double): Double = kotlin.math.log2(x)
    
    @JvmStatic
    fun log(x: Double, base: Double): Double = kotlin.math.log(x, base)
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // الدوال المثلثية
    // ═══════════════════════════════════════════════════════════════════════════════
    
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
    fun hypot(x: Double, y: Double): Double = kotlin.math.hypot(x, y)
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // الدوال الزائدية
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun sinh(x: Double): Double = kotlin.math.sinh(x)
    
    @JvmStatic
    fun cosh(x: Double): Double = kotlin.math.cosh(x)
    
    @JvmStatic
    fun tanh(x: Double): Double = kotlin.math.tanh(x)
    
    @JvmStatic
    fun asinh(x: Double): Double = kotlin.math.asinh(x)
    
    @JvmStatic
    fun acosh(x: Double): Double = kotlin.math.acosh(x)
    
    @JvmStatic
    fun atanh(x: Double): Double = kotlin.math.atanh(x)
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // تحويل الزوايا
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun toRadians(degrees: Double): Double = Math.toRadians(degrees)
    
    @JvmStatic
    fun toDegrees(radians: Double): Double = Math.toDegrees(radians)
    
    @JvmStatic
    fun normalizeAngle(degrees: Double): Double {
        var result = degrees % 360
        if (result < 0) result += 360
        return result
    }
    
    @JvmStatic
    fun normalizeRadians(radians: Double): Double {
        var result = radians % TAU
        if (result < 0) result += TAU
        return result
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // نظرية الأعداد
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun gcd(a: Long, b: Long): Long {
        var x = kotlin.math.abs(a)
        var y = kotlin.math.abs(b)
        while (y != 0L) {
            val temp = y
            y = x % y
            x = temp
        }
        return x
    }
    
    @JvmStatic
    fun gcdInt(a: Int, b: Int): Int = gcd(a.toLong(), b.toLong()).toInt()
    
    @JvmStatic
    fun lcm(a: Long, b: Long): Long = (a / gcd(a, b)) * b
    
    @JvmStatic
    fun lcmInt(a: Int, b: Int): Int = lcm(a.toLong(), b.toLong()).toInt()
    
    @JvmStatic
    fun isPrime(n: Long): Boolean {
        if (n < 2) return false
        if (n == 2L || n == 3L) return true
        if (n % 2 == 0L || n % 3 == 0L) return false
        var i = 5L
        while (i * i <= n) {
            if (n % i == 0L || n % (i + 2) == 0L) return false
            i += 6
        }
        return true
    }
    
    @JvmStatic
    fun isPrimeInt(n: Int): Boolean = isPrime(n.toLong())
    
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
    fun fibonacci(n: Int): Long {
        if (n <= 0) return 0
        if (n == 1) return 1
        var a = 0L
        var b = 1L
        for (i in 2..n) {
            val temp = a + b
            a = b
            b = temp
        }
        return b
    }
    
    @JvmStatic
    fun combination(n: Int, r: Int): Long {
        if (r > n || r < 0) return 0
        if (r == 0 || r == n) return 1
        val k = minOf(r, n - r)
        var result = 1L
        for (i in 0 until k) {
            result = result * (n - i) / (i + 1)
        }
        return result
    }
    
    @JvmStatic
    fun permutation(n: Int, r: Int): Long {
        if (r > n || r < 0) return 0
        var result = 1L
        for (i in 0 until r) {
            result *= (n - i)
        }
        return result
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // الإحصاء
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun mean(values: DoubleArray): Double = values.average()
    
    @JvmStatic
    fun meanList(values: List<Double>): Double = values.average()
    
    @JvmStatic
    fun median(values: DoubleArray): Double {
        val sorted = values.sorted()
        val n = sorted.size
        return if (n % 2 == 0) {
            (sorted[n / 2 - 1] + sorted[n / 2]) / 2.0
        } else {
            sorted[n / 2]
        }
    }
    
    @JvmStatic
    fun variance(values: DoubleArray): Double {
        val m = mean(values)
        return values.sumOf { (it - m) * (it - m) } / values.size
    }
    
    @JvmStatic
    fun standardDeviation(values: DoubleArray): Double = sqrt(variance(values))
    
    @JvmStatic
    fun sum(values: DoubleArray): Double = values.sum()
    
    @JvmStatic
    fun sumList(values: List<Double>): Double = values.sum()
    
    @JvmStatic
    fun product(values: DoubleArray): Double = values.reduce { a, b -> a * b }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // الاستيفاء
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun lerp(start: Double, end: Double, t: Double): Double {
        return start + (end - start) * t
    }
    
    @JvmStatic
    fun inverseLerp(start: Double, end: Double, value: Double): Double {
        return (value - start) / (end - start)
    }
    
    @JvmStatic
    fun remap(
        value: Double,
        inputStart: Double,
        inputEnd: Double,
        outputStart: Double,
        outputEnd: Double
    ): Double {
        val t = inverseLerp(inputStart, inputEnd, value)
        return lerp(outputStart, outputEnd, t)
    }
    
    @JvmStatic
    fun smoothStep(edge0: Double, edge1: Double, x: Double): Double {
        val t = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0)
        return t * t * (3.0 - 2.0 * t)
    }
    
    @JvmStatic
    fun smootherStep(edge0: Double, edge1: Double, x: Double): Double {
        val t = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0)
        return t * t * t * (t * (t * 6.0 - 15.0) + 10.0)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // الأعداد العشوائية
    // ═══════════════════════════════════════════════════════════════════════════════
    
    private val random = java.util.Random()
    
    @JvmStatic
    fun random(): Double = random.nextDouble()
    
    @JvmStatic
    fun randomInt(bound: Int): Int = random.nextInt(bound)
    
    @JvmStatic
    fun randomInRange(min: Double, max: Double): Double {
        return min + random.nextDouble() * (max - min)
    }
    
    @JvmStatic
    fun randomIntInRange(min: Int, max: Int): Int {
        return min + random.nextInt(max - min + 1)
    }
    
    @JvmStatic
    fun randomLong(): Long = random.nextLong()
    
    @JvmStatic
    fun randomBoolean(): Boolean = random.nextBoolean()
    
    @JvmStatic
    fun randomGaussian(mean: Double = 0.0, stdDev: Double = 1.0): Double {
        return mean + random.nextGaussian() * stdDev
    }
    
    @JvmStatic
    fun setSeed(seed: Long) {
        random.setSeed(seed)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // العمليات على البتات
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun bitAnd(a: Int, b: Int): Int = a and b
    
    @JvmStatic
    fun bitOr(a: Int, b: Int): Int = a or b
    
    @JvmStatic
    fun bitXor(a: Int, b: Int): Int = a xor b
    
    @JvmStatic
    fun bitNot(a: Int): Int = a.inv()
    
    @JvmStatic
    fun bitShiftLeft(a: Int, bits: Int): Int = a shl bits
    
    @JvmStatic
    fun bitShiftRight(a: Int, bits: Int): Int = a shr bits
    
    @JvmStatic
    fun bitShiftRightUnsigned(a: Int, bits: Int): Int = a ushr bits
    
    @JvmStatic
    fun bitCount(a: Int): Int = Integer.bitCount(a)
    
    @JvmStatic
    fun highestOneBit(a: Int): Int = Integer.highestOneBit(a)
    
    @JvmStatic
    fun lowestOneBit(a: Int): Int = Integer.lowestOneBit(a)
    
    @JvmStatic
    fun numberOfLeadingZeros(a: Int): Int = Integer.numberOfLeadingZeros(a)
    
    @JvmStatic
    fun numberOfTrailingZeros(a: Int): Int = Integer.numberOfTrailingZeros(a)
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // المقارنة والتحقق
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun isNaN(x: Double): Boolean = x.isNaN()
    
    @JvmStatic
    fun isInfinite(x: Double): Boolean = x.isInfinite()
    
    @JvmStatic
    fun isFinite(x: Double): Boolean = x.isFinite()
    
    @JvmStatic
    fun isEven(x: Int): Boolean = x % 2 == 0
    
    @JvmStatic
    fun isOdd(x: Int): Boolean = x % 2 != 0
    
    @JvmStatic
    fun isPositive(x: Double): Boolean = x > 0
    
    @JvmStatic
    fun isNegative(x: Double): Boolean = x < 0
    
    @JvmStatic
    fun isZero(x: Double, epsilon: Double = 1e-10): Boolean = abs(x) < epsilon
    
    @JvmStatic
    fun almostEqual(a: Double, b: Double, epsilon: Double = 1e-10): Boolean {
        return abs(a - b) < epsilon
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // الهندسة
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun distance(x1: Double, y1: Double, x2: Double, y2: Double): Double {
        return hypot(x2 - x1, y2 - y1)
    }
    
    @JvmStatic
    fun distance3D(x1: Double, y1: Double, z1: Double, x2: Double, y2: Double, z2: Double): Double {
        return sqrt((x2 - x1).pow(2) + (y2 - y1).pow(2) + (z2 - z1).pow(2))
    }
    
    @JvmStatic
    fun triangleArea(base: Double, height: Double): Double = 0.5 * base * height
    
    @JvmStatic
    fun triangleAreaHeron(a: Double, b: Double, c: Double): Double {
        val s = (a + b + c) / 2
        return sqrt(s * (s - a) * (s - b) * (s - c))
    }
    
    @JvmStatic
    fun circleArea(radius: Double): Double = PI * radius * radius
    
    @JvmStatic
    fun circleCircumference(radius: Double): Double = 2 * PI * radius
    
    @JvmStatic
    fun sphereVolume(radius: Double): Double = (4.0 / 3.0) * PI * radius.pow(3)
    
    @JvmStatic
    fun sphereSurfaceArea(radius: Double): Double = 4 * PI * radius * radius
    
    @JvmStatic
    fun cylinderVolume(radius: Double, height: Double): Double = PI * radius * radius * height
    
    @JvmStatic
    fun coneVolume(radius: Double, height: Double): Double = (1.0 / 3.0) * PI * radius * radius * height
}
