// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadLoggingBridge.kt
// الوصف: جسر Kotlin للتسجيل والمراقبة
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.util.Log
import java.io.File
import java.io.FileWriter
import java.io.PrintWriter
import java.io.StringWriter
import java.text.SimpleDateFormat
import java.util.*
import java.util.concurrent.ConcurrentLinkedQueue
import java.util.concurrent.atomic.AtomicBoolean

/**
 * جسر التسجيل — SadLoggingBridge
 */
object SadLoggingBridge {
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // مستويات التسجيل
    // ═══════════════════════════════════════════════════════════════════════════════
    
    enum class LogLevel(val priority: Int, val arabicName: String, val symbol: String) {
        VERBOSE(1, "تفصيلي", "📝"),
        DEBUG(2, "تصحيح", "🔧"),
        INFO(3, "معلومات", "ℹ️"),
        WARNING(4, "تحذير", "⚠️"),
        ERROR(5, "خطأ", "❌"),
        FATAL(6, "كارثي", "💀")
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // سجل الرسائل
    // ═══════════════════════════════════════════════════════════════════════════════
    
    data class LogEntry(
        val timestamp: Long,
        val level: LogLevel,
        val tag: String,
        val message: String,
        val throwable: Throwable? = null,
        val extras: Map<String, Any>? = null
    ) {
        fun format(includeTimestamp: Boolean = true, includeLevel: Boolean = true): String {
            val sb = StringBuilder()
            
            if (includeTimestamp) {
                val dateFormat = SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS", Locale.getDefault())
                sb.append("[${dateFormat.format(Date(timestamp))}] ")
            }
            
            if (includeLevel) {
                sb.append("[${level.arabicName}] ")
            }
            
            sb.append("[$tag] $message")
            
            throwable?.let {
                sb.append("\n")
                sb.append(getStackTraceString(it))
            }
            
            return sb.toString()
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // الإعدادات
    // ═══════════════════════════════════════════════════════════════════════════════
    
    private var minLogLevel: LogLevel = LogLevel.DEBUG
    private var defaultTag: String = "ص"
    private var logToFile: AtomicBoolean = AtomicBoolean(false)
    private var logFile: File? = null
    private var logToConsole: AtomicBoolean = AtomicBoolean(true)
    private var maxInMemoryLogs: Int = 1000
    private val inMemoryLogs = ConcurrentLinkedQueue<LogEntry>()
    private var logListeners = mutableListOf<(LogEntry) -> Unit>()
    
    @JvmStatic
    fun setMinLevel(level: LogLevel) {
        minLogLevel = level
    }
    
    @JvmStatic
    fun setMinLevelByName(levelName: String) {
        minLogLevel = LogLevel.values().find { 
            it.name.equals(levelName, ignoreCase = true) || it.arabicName == levelName 
        } ?: LogLevel.DEBUG
    }
    
    @JvmStatic
    fun setDefaultTag(tag: String) {
        defaultTag = tag
    }
    
    @JvmStatic
    fun enableFileLogging(file: File) {
        logFile = file
        logToFile.set(true)
    }
    
    @JvmStatic
    fun enableFileLogging(filePath: String) {
        enableFileLogging(File(filePath))
    }
    
    @JvmStatic
    fun disableFileLogging() {
        logToFile.set(false)
    }
    
    @JvmStatic
    fun enableConsoleLogging() {
        logToConsole.set(true)
    }
    
    @JvmStatic
    fun disableConsoleLogging() {
        logToConsole.set(false)
    }
    
    @JvmStatic
    fun setMaxInMemoryLogs(max: Int) {
        maxInMemoryLogs = max
    }
    
    @JvmStatic
    fun addLogListener(listener: (LogEntry) -> Unit) {
        logListeners.add(listener)
    }
    
    @JvmStatic
    fun clearLogListeners() {
        logListeners.clear()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // وظائف التسجيل الأساسية
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun log(level: LogLevel, tag: String, message: String, throwable: Throwable? = null, extras: Map<String, Any>? = null) {
        if (level.priority < minLogLevel.priority) return
        
        val entry = LogEntry(
            timestamp = System.currentTimeMillis(),
            level = level,
            tag = tag,
            message = message,
            throwable = throwable,
            extras = extras
        )
        
        // Console logging
        if (logToConsole.get()) {
            logToAndroid(entry)
        }
        
        // File logging
        if (logToFile.get()) {
            logToFileInternal(entry)
        }
        
        // In-memory logging
        addToInMemoryLogs(entry)
        
        // Notify listeners
        logListeners.forEach { it(entry) }
    }
    
    private fun logToAndroid(entry: LogEntry) {
        val fullMessage = if (entry.throwable != null) {
            "${entry.message}\n${getStackTraceString(entry.throwable)}"
        } else {
            entry.message
        }
        
        when (entry.level) {
            LogLevel.VERBOSE -> Log.v(entry.tag, fullMessage)
            LogLevel.DEBUG -> Log.d(entry.tag, fullMessage)
            LogLevel.INFO -> Log.i(entry.tag, fullMessage)
            LogLevel.WARNING -> Log.w(entry.tag, fullMessage)
            LogLevel.ERROR -> Log.e(entry.tag, fullMessage)
            LogLevel.FATAL -> Log.wtf(entry.tag, fullMessage)
        }
    }
    
    private fun logToFileInternal(entry: LogEntry) {
        logFile?.let { file ->
            try {
                FileWriter(file, true).use { writer ->
                    writer.appendLine(entry.format())
                }
            } catch (e: Exception) {
                Log.e(defaultTag, "خطأ في الكتابة للملف: ${e.message}")
            }
        }
    }
    
    private fun addToInMemoryLogs(entry: LogEntry) {
        inMemoryLogs.offer(entry)
        while (inMemoryLogs.size > maxInMemoryLogs) {
            inMemoryLogs.poll()
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // وظائف التسجيل المختصرة
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun v(message: String, tag: String = defaultTag) = log(LogLevel.VERBOSE, tag, message)
    
    @JvmStatic
    fun d(message: String, tag: String = defaultTag) = log(LogLevel.DEBUG, tag, message)
    
    @JvmStatic
    fun i(message: String, tag: String = defaultTag) = log(LogLevel.INFO, tag, message)
    
    @JvmStatic
    fun w(message: String, tag: String = defaultTag) = log(LogLevel.WARNING, tag, message)
    
    @JvmStatic
    fun e(message: String, tag: String = defaultTag) = log(LogLevel.ERROR, tag, message)
    
    @JvmStatic
    fun f(message: String, tag: String = defaultTag) = log(LogLevel.FATAL, tag, message)
    
    // With throwable
    @JvmStatic
    fun v(message: String, throwable: Throwable, tag: String = defaultTag) = 
        log(LogLevel.VERBOSE, tag, message, throwable)
    
    @JvmStatic
    fun d(message: String, throwable: Throwable, tag: String = defaultTag) = 
        log(LogLevel.DEBUG, tag, message, throwable)
    
    @JvmStatic
    fun i(message: String, throwable: Throwable, tag: String = defaultTag) = 
        log(LogLevel.INFO, tag, message, throwable)
    
    @JvmStatic
    fun w(message: String, throwable: Throwable, tag: String = defaultTag) = 
        log(LogLevel.WARNING, tag, message, throwable)
    
    @JvmStatic
    fun e(message: String, throwable: Throwable, tag: String = defaultTag) = 
        log(LogLevel.ERROR, tag, message, throwable)
    
    @JvmStatic
    fun f(message: String, throwable: Throwable, tag: String = defaultTag) = 
        log(LogLevel.FATAL, tag, message, throwable)
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // استعلام السجلات
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getRecentLogs(count: Int = 100): List<LogEntry> {
        return inMemoryLogs.toList().takeLast(count)
    }
    
    @JvmStatic
    fun getLogsByLevel(level: LogLevel): List<LogEntry> {
        return inMemoryLogs.filter { it.level == level }
    }
    
    @JvmStatic
    fun getLogsByTag(tag: String): List<LogEntry> {
        return inMemoryLogs.filter { it.tag == tag }
    }
    
    @JvmStatic
    fun searchLogs(query: String, ignoreCase: Boolean = true): List<LogEntry> {
        return inMemoryLogs.filter { 
            it.message.contains(query, ignoreCase) || it.tag.contains(query, ignoreCase)
        }
    }
    
    @JvmStatic
    fun getLogsSince(timestamp: Long): List<LogEntry> {
        return inMemoryLogs.filter { it.timestamp >= timestamp }
    }
    
    @JvmStatic
    fun getErrorLogs(): List<LogEntry> {
        return inMemoryLogs.filter { it.level == LogLevel.ERROR || it.level == LogLevel.FATAL }
    }
    
    @JvmStatic
    fun clearInMemoryLogs() {
        inMemoryLogs.clear()
    }
    
    @JvmStatic
    fun clearLogFile() {
        logFile?.let { file ->
            try {
                FileWriter(file, false).use { it.write("") }
            } catch (e: Exception) {
                Log.e(defaultTag, "خطأ في مسح الملف: ${e.message}")
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // تصدير السجلات
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun exportLogsToString(includeTimestamp: Boolean = true, includeLevel: Boolean = true): String {
        val sb = StringBuilder()
        inMemoryLogs.forEach { entry ->
            sb.appendLine(entry.format(includeTimestamp, includeLevel))
        }
        return sb.toString()
    }
    
    @JvmStatic
    fun exportLogsToFile(file: File, includeTimestamp: Boolean = true, includeLevel: Boolean = true): Boolean {
        return try {
            FileWriter(file).use { writer ->
                inMemoryLogs.forEach { entry ->
                    writer.appendLine(entry.format(includeTimestamp, includeLevel))
                }
            }
            true
        } catch (e: Exception) {
            Log.e(defaultTag, "خطأ في تصدير السجلات: ${e.message}")
            false
        }
    }
    
    @JvmStatic
    fun exportLogsToJson(): String {
        val sb = StringBuilder()
        sb.append("[")
        
        val entries = inMemoryLogs.toList()
        entries.forEachIndexed { index, entry ->
            sb.append("{")
            sb.append("\"timestamp\":${entry.timestamp},")
            sb.append("\"level\":\"${entry.level.name}\",")
            sb.append("\"levelArabic\":\"${entry.level.arabicName}\",")
            sb.append("\"tag\":\"${escapeJson(entry.tag)}\",")
            sb.append("\"message\":\"${escapeJson(entry.message)}\"")
            entry.throwable?.let {
                sb.append(",\"stacktrace\":\"${escapeJson(getStackTraceString(it))}\"")
            }
            sb.append("}")
            if (index < entries.size - 1) sb.append(",")
        }
        
        sb.append("]")
        return sb.toString()
    }
    
    private fun escapeJson(text: String): String {
        return text
            .replace("\\", "\\\\")
            .replace("\"", "\\\"")
            .replace("\n", "\\n")
            .replace("\r", "\\r")
            .replace("\t", "\\t")
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // قياس الأداء
    // ═══════════════════════════════════════════════════════════════════════════════
    
    private val timers = mutableMapOf<String, Long>()
    
    @JvmStatic
    fun startTimer(label: String) {
        timers[label] = System.nanoTime()
        d("⏱️ بدء القياس: $label")
    }
    
    @JvmStatic
    fun stopTimer(label: String): Long {
        val startTime = timers.remove(label)
        return if (startTime != null) {
            val duration = (System.nanoTime() - startTime) / 1_000_000 // to milliseconds
            d("⏱️ انتهى: $label - ${duration}ms")
            duration
        } else {
            w("⏱️ المؤقت غير موجود: $label")
            -1
        }
    }
    
    @JvmStatic
    fun <T> measureTime(label: String, block: () -> T): T {
        startTimer(label)
        val result = block()
        stopTimer(label)
        return result
    }
    
    @JvmStatic
    fun logMemoryUsage(tag: String = defaultTag) {
        val runtime = Runtime.getRuntime()
        val usedMemory = (runtime.totalMemory() - runtime.freeMemory()) / 1024 / 1024
        val maxMemory = runtime.maxMemory() / 1024 / 1024
        val freeMemory = runtime.freeMemory() / 1024 / 1024
        
        i("💾 استخدام الذاكرة: ${usedMemory}MB / ${maxMemory}MB (متاح: ${freeMemory}MB)", tag)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // تنسيق الأخطاء
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getStackTraceString(throwable: Throwable): String {
        val sw = StringWriter()
        val pw = PrintWriter(sw)
        throwable.printStackTrace(pw)
        return sw.toString()
    }
    
    @JvmStatic
    fun logException(throwable: Throwable, message: String = "خطأ غير متوقع", tag: String = defaultTag) {
        e(message, throwable, tag)
    }
    
    @JvmStatic
    fun getCrashReport(throwable: Throwable): String {
        val sb = StringBuilder()
        sb.appendLine("═══════════════════════════════════════════════════════════════")
        sb.appendLine("تقرير الانهيار - Crash Report")
        sb.appendLine("═══════════════════════════════════════════════════════════════")
        sb.appendLine()
        
        val dateFormat = SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS", Locale.getDefault())
        sb.appendLine("الوقت: ${dateFormat.format(Date())}")
        sb.appendLine()
        
        sb.appendLine("نوع الخطأ: ${throwable.javaClass.simpleName}")
        sb.appendLine("الرسالة: ${throwable.message ?: "لا توجد رسالة"}")
        sb.appendLine()
        
        sb.appendLine("تتبع المكدس:")
        sb.appendLine(getStackTraceString(throwable))
        sb.appendLine()
        
        sb.appendLine("معلومات الذاكرة:")
        val runtime = Runtime.getRuntime()
        sb.appendLine("  الذاكرة المستخدمة: ${(runtime.totalMemory() - runtime.freeMemory()) / 1024 / 1024}MB")
        sb.appendLine("  الذاكرة القصوى: ${runtime.maxMemory() / 1024 / 1024}MB")
        sb.appendLine()
        
        sb.appendLine("آخر السجلات:")
        getRecentLogs(10).forEach { entry ->
            sb.appendLine("  ${entry.format()}")
        }
        
        sb.appendLine("═══════════════════════════════════════════════════════════════")
        
        return sb.toString()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // إحصائيات التسجيل
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getLogStats(): Map<String, Any> {
        val stats = mutableMapOf<String, Any>()
        
        stats["totalLogs"] = inMemoryLogs.size
        stats["errorCount"] = inMemoryLogs.count { it.level == LogLevel.ERROR }
        stats["warningCount"] = inMemoryLogs.count { it.level == LogLevel.WARNING }
        stats["infoCount"] = inMemoryLogs.count { it.level == LogLevel.INFO }
        stats["debugCount"] = inMemoryLogs.count { it.level == LogLevel.DEBUG }
        stats["verboseCount"] = inMemoryLogs.count { it.level == LogLevel.VERBOSE }
        stats["fatalCount"] = inMemoryLogs.count { it.level == LogLevel.FATAL }
        
        val tags = inMemoryLogs.groupBy { it.tag }.mapValues { it.value.size }
        stats["tagCounts"] = tags
        
        val firstLog = inMemoryLogs.firstOrNull()
        val lastLog = inMemoryLogs.lastOrNull()
        
        firstLog?.let { stats["firstLogTime"] = it.timestamp }
        lastLog?.let { stats["lastLogTime"] = it.timestamp }
        
        return stats
    }
    
    @JvmStatic
    fun getLogStatsArabic(): String {
        val stats = getLogStats()
        val sb = StringBuilder()
        
        sb.appendLine("📊 إحصائيات السجلات")
        sb.appendLine("═══════════════════════════════════════")
        sb.appendLine("إجمالي السجلات: ${stats["totalLogs"]}")
        sb.appendLine("أخطاء: ${stats["errorCount"]}")
        sb.appendLine("تحذيرات: ${stats["warningCount"]}")
        sb.appendLine("معلومات: ${stats["infoCount"]}")
        sb.appendLine("تصحيح: ${stats["debugCount"]}")
        sb.appendLine("تفصيلي: ${stats["verboseCount"]}")
        sb.appendLine("كارثي: ${stats["fatalCount"]}")
        sb.appendLine("═══════════════════════════════════════")
        
        return sb.toString()
    }
}
