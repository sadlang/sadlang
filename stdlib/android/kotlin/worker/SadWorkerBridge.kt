// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadWorkerBridge.kt
// الوصف: جسر Kotlin لـ WorkManager (المهام الخلفية)
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import androidx.work.*
import java.util.concurrent.TimeUnit
import java.util.UUID

/**
 * جسر المهام الخلفية — SadWorkerBridge
 */
object SadWorkerBridge {
    
    private lateinit var workManager: WorkManager
    private val workRequests = mutableMapOf<String, UUID>()
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // التهيئة
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun init(context: Context) {
        workManager = WorkManager.getInstance(context)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // قيود التنفيذ
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun createConstraints(
        requiresNetwork: Boolean = false,
        requiresUnmeteredNetwork: Boolean = false,
        requiresCharging: Boolean = false,
        requiresBatteryNotLow: Boolean = false,
        requiresStorageNotLow: Boolean = false,
        requiresDeviceIdle: Boolean = false
    ): Constraints {
        return Constraints.Builder().apply {
            if (requiresNetwork) {
                setRequiredNetworkType(NetworkType.CONNECTED)
            }
            if (requiresUnmeteredNetwork) {
                setRequiredNetworkType(NetworkType.UNMETERED)
            }
            setRequiresCharging(requiresCharging)
            setRequiresBatteryNotLow(requiresBatteryNotLow)
            setRequiresStorageNotLow(requiresStorageNotLow)
            setRequiresDeviceIdle(requiresDeviceIdle)
        }.build()
    }
    
    @JvmStatic
    fun createNetworkConstraint(): Constraints {
        return Constraints.Builder()
            .setRequiredNetworkType(NetworkType.CONNECTED)
            .build()
    }
    
    @JvmStatic
    fun createWifiConstraint(): Constraints {
        return Constraints.Builder()
            .setRequiredNetworkType(NetworkType.UNMETERED)
            .build()
    }
    
    @JvmStatic
    fun createChargingConstraint(): Constraints {
        return Constraints.Builder()
            .setRequiresCharging(true)
            .build()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // إنشاء المهام
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun <T : ListenableWorker> createOneTimeWork(
        workerClass: Class<T>,
        tag: String,
        inputData: Data? = null,
        constraints: Constraints? = null,
        initialDelayMinutes: Long = 0
    ): OneTimeWorkRequest {
        return OneTimeWorkRequest.Builder(workerClass).apply {
            addTag(tag)
            inputData?.let { setInputData(it) }
            constraints?.let { setConstraints(it) }
            if (initialDelayMinutes > 0) {
                setInitialDelay(initialDelayMinutes, TimeUnit.MINUTES)
            }
        }.build()
    }
    
    @JvmStatic
    fun <T : ListenableWorker> createPeriodicWork(
        workerClass: Class<T>,
        tag: String,
        repeatIntervalMinutes: Long,
        inputData: Data? = null,
        constraints: Constraints? = null,
        flexIntervalMinutes: Long = 0
    ): PeriodicWorkRequest {
        return if (flexIntervalMinutes > 0) {
            PeriodicWorkRequest.Builder(
                workerClass,
                repeatIntervalMinutes, TimeUnit.MINUTES,
                flexIntervalMinutes, TimeUnit.MINUTES
            )
        } else {
            PeriodicWorkRequest.Builder(workerClass, repeatIntervalMinutes, TimeUnit.MINUTES)
        }.apply {
            addTag(tag)
            inputData?.let { setInputData(it) }
            constraints?.let { setConstraints(it) }
        }.build()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // تنفيذ المهام
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun enqueue(request: WorkRequest, name: String? = null): UUID {
        workManager.enqueue(request)
        val id = request.id
        name?.let { workRequests[it] = id }
        return id
    }
    
    @JvmStatic
    fun enqueueUnique(
        request: OneTimeWorkRequest,
        uniqueName: String,
        existingWorkPolicy: ExistingWorkPolicy = ExistingWorkPolicy.KEEP
    ): UUID {
        workManager.enqueueUniqueWork(uniqueName, existingWorkPolicy, request)
        val id = request.id
        workRequests[uniqueName] = id
        return id
    }
    
    @JvmStatic
    fun enqueueUniquePeriodic(
        request: PeriodicWorkRequest,
        uniqueName: String,
        existingPolicy: ExistingPeriodicWorkPolicy = ExistingPeriodicWorkPolicy.KEEP
    ): UUID {
        workManager.enqueueUniquePeriodicWork(uniqueName, existingPolicy, request)
        val id = request.id
        workRequests[uniqueName] = id
        return id
    }
    
    @JvmStatic
    fun enqueueChain(requests: List<WorkRequest>): UUID {
        if (requests.isEmpty()) return UUID.randomUUID()
        
        var continuation = workManager.beginWith(requests.first() as OneTimeWorkRequest)
        for (i in 1 until requests.size) {
            continuation = continuation.then(requests[i] as OneTimeWorkRequest)
        }
        continuation.enqueue()
        
        return requests.last().id
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // إدارة المهام
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun cancelByTag(tag: String) {
        workManager.cancelAllWorkByTag(tag)
    }
    
    @JvmStatic
    fun cancelById(id: UUID) {
        workManager.cancelWorkById(id)
    }
    
    @JvmStatic
    fun cancelByName(name: String) {
        workRequests[name]?.let { cancelById(it) }
    }
    
    @JvmStatic
    fun cancelUniqueName(uniqueName: String) {
        workManager.cancelUniqueWork(uniqueName)
    }
    
    @JvmStatic
    fun cancelAll() {
        workManager.cancelAllWork()
    }
    
    @JvmStatic
    fun pruneWork() {
        workManager.pruneWork()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // حالة المهام
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getWorkInfoById(id: UUID): WorkInfo? {
        return try {
            workManager.getWorkInfoById(id).get()
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun getWorkInfoByName(name: String): WorkInfo? {
        return workRequests[name]?.let { getWorkInfoById(it) }
    }
    
    @JvmStatic
    fun getWorkInfosByTag(tag: String): List<WorkInfo> {
        return try {
            workManager.getWorkInfosByTag(tag).get()
        } catch (e: Exception) {
            emptyList()
        }
    }
    
    @JvmStatic
    fun isWorkRunning(id: UUID): Boolean {
        val info = getWorkInfoById(id)
        return info?.state == WorkInfo.State.RUNNING
    }
    
    @JvmStatic
    fun isWorkEnqueued(id: UUID): Boolean {
        val info = getWorkInfoById(id)
        return info?.state == WorkInfo.State.ENQUEUED
    }
    
    @JvmStatic
    fun isWorkSucceeded(id: UUID): Boolean {
        val info = getWorkInfoById(id)
        return info?.state == WorkInfo.State.SUCCEEDED
    }
    
    @JvmStatic
    fun isWorkFailed(id: UUID): Boolean {
        val info = getWorkInfoById(id)
        return info?.state == WorkInfo.State.FAILED
    }
    
    @JvmStatic
    fun isWorkCancelled(id: UUID): Boolean {
        val info = getWorkInfoById(id)
        return info?.state == WorkInfo.State.CANCELLED
    }
    
    @JvmStatic
    fun getWorkState(id: UUID): String {
        val info = getWorkInfoById(id) ?: return "غير موجود"
        return when (info.state) {
            WorkInfo.State.ENQUEUED -> "في الانتظار"
            WorkInfo.State.RUNNING -> "قيد التنفيذ"
            WorkInfo.State.SUCCEEDED -> "نجحت"
            WorkInfo.State.FAILED -> "فشلت"
            WorkInfo.State.BLOCKED -> "محظورة"
            WorkInfo.State.CANCELLED -> "ملغاة"
        }
    }
    
    @JvmStatic
    fun getOutputData(id: UUID): Data? {
        return getWorkInfoById(id)?.outputData
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // بيانات الإدخال
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun createData(vararg pairs: Pair<String, Any>): Data {
        return Data.Builder().apply {
            pairs.forEach { (key, value) ->
                when (value) {
                    is String -> putString(key, value)
                    is Int -> putInt(key, value)
                    is Long -> putLong(key, value)
                    is Float -> putFloat(key, value)
                    is Double -> putDouble(key, value)
                    is Boolean -> putBoolean(key, value)
                    is ByteArray -> putByteArray(key, value)
                    is IntArray -> putIntArray(key, value)
                    is LongArray -> putLongArray(key, value)
                    is FloatArray -> putFloatArray(key, value)
                    is DoubleArray -> putDoubleArray(key, value)
                    is BooleanArray -> putBooleanArray(key, value)
                    is Array<*> -> {
                        @Suppress("UNCHECKED_CAST")
                        putStringArray(key, value as Array<String>)
                    }
                }
            }
        }.build()
    }
    
    @JvmStatic
    fun createDataFromMap(map: Map<String, Any>): Data {
        return createData(*map.toList().toTypedArray())
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // استخراج البيانات
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getString(data: Data, key: String, default: String = ""): String {
        return data.getString(key) ?: default
    }
    
    @JvmStatic
    fun getInt(data: Data, key: String, default: Int = 0): Int {
        return data.getInt(key, default)
    }
    
    @JvmStatic
    fun getLong(data: Data, key: String, default: Long = 0L): Long {
        return data.getLong(key, default)
    }
    
    @JvmStatic
    fun getBoolean(data: Data, key: String, default: Boolean = false): Boolean {
        return data.getBoolean(key, default)
    }
    
    @JvmStatic
    fun getDouble(data: Data, key: String, default: Double = 0.0): Double {
        return data.getDouble(key, default)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // معلومات العمل
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getWorkInfoArabic(id: UUID): String {
        val info = getWorkInfoById(id) ?: return "معلومات العمل غير متوفرة"
        
        return buildString {
            appendLine("═══════════════════════════════════════")
            appendLine("معلومات العمل")
            appendLine("═══════════════════════════════════════")
            appendLine("المعرف: $id")
            appendLine("الحالة: ${getWorkState(id)}")
            appendLine("عدد المحاولات: ${info.runAttemptCount}")
            appendLine("الوسوم: ${info.tags.joinToString(", ")}")
            appendLine("═══════════════════════════════════════")
        }
    }
}

/**
 * عامل مخصص للغة ص
 */
abstract class SadWorker(
    context: Context,
    params: WorkerParameters
) : Worker(context, params) {
    
    /**
     * الاسم العربي للعامل
     */
    abstract val arabicName: String
    
    /**
     * الدالة الرئيسية للعمل
     */
    abstract fun performWork(): Boolean
    
    override fun doWork(): Result {
        return try {
            if (performWork()) {
                Result.success()
            } else {
                Result.failure()
            }
        } catch (e: Exception) {
            Result.failure(
                Data.Builder()
                    .putString("error", e.message)
                    .putString("errorArabic", "خطأ: ${e.message}")
                    .build()
            )
        }
    }
    
    /**
     * إرجاع نتيجة ناجحة مع بيانات
     */
    protected fun successWith(vararg pairs: Pair<String, Any>): Result {
        return Result.success(SadWorkerBridge.createData(*pairs))
    }
    
    /**
     * إرجاع نتيجة فاشلة مع سبب
     */
    protected fun failureWith(reason: String): Result {
        return Result.failure(
            Data.Builder()
                .putString("error", reason)
                .putString("errorArabic", reason)
                .build()
        )
    }
    
    /**
     * إعادة المحاولة
     */
    protected fun retryLater(): Result = Result.retry()
}
