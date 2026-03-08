/**
 * =============================================================================
 * @file SadWorker.kt
 * @brief (AR) واجهة WorkManager للغة ص
 * @brief (EN) WorkManager Interface for Sad Language
 * =============================================================================
 */

package sad.lang.android

import android.content.Context
import androidx.work.*
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import java.util.concurrent.TimeUnit

/**
 * نظام المهام المجدولة للغة ص (WorkManager)
 */
object SadWorker {
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Types
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * نتيجة العمل
     */
    sealed class WorkResult {
        object Success : WorkResult()
        data class Failure(val message: String) : WorkResult()
        object Retry : WorkResult()
    }
    
    /**
     * حالة العمل
     */
    enum class WorkState {
        ENQUEUED,
        RUNNING,
        SUCCEEDED,
        FAILED,
        CANCELLED,
        BLOCKED,
        UNKNOWN
    }
    
    /**
     * قيود العمل
     */
    data class WorkConstraints(
        val requiresNetwork: Boolean = false,
        val requiresUnmeteredNetwork: Boolean = false,
        val requiresCharging: Boolean = false,
        val requiresDeviceIdle: Boolean = false,
        val requiresBatteryNotLow: Boolean = false,
        val requiresStorageNotLow: Boolean = false
    )
    
    /**
     * سياسة إعادة المحاولة
     */
    enum class BackoffPolicy {
        LINEAR,
        EXPONENTIAL
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Worker Base Class
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * منفذ العمل الأساسي
     */
    abstract class SadCoroutineWorker(
        context: Context,
        params: WorkerParameters
    ) : CoroutineWorker(context, params) {
        
        /**
         * تنفيذ العمل (يجب تنفيذها في الفئات الفرعية)
         */
        abstract suspend fun execute(inputData: Map<String, Any?>): WorkResult
        
        override suspend fun doWork(): Result = withContext(Dispatchers.IO) {
            val inputMap = inputData.keyValueMap.mapValues { it.value }
            
            when (val result = execute(inputMap)) {
                is WorkResult.Success -> Result.success()
                is WorkResult.Failure -> Result.failure(
                    workDataOf("error" to result.message)
                )
                is WorkResult.Retry -> Result.retry()
            }
        }
        
        /**
         * الحصول على قيمة من البيانات المدخلة
         */
        protected fun getString(key: String): String? = inputData.getString(key)
        protected fun getInt(key: String, default: Int = 0): Int = inputData.getInt(key, default)
        protected fun getLong(key: String, default: Long = 0L): Long = inputData.getLong(key, default)
        protected fun getBoolean(key: String, default: Boolean = false): Boolean = inputData.getBoolean(key, default)
        protected fun getDouble(key: String, default: Double = 0.0): Double = inputData.getDouble(key, default)
        protected fun getStringArray(key: String): Array<String>? = inputData.getStringArray(key)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Scheduling
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * جدولة عمل لمرة واحدة
     */
    inline fun <reified T : ListenableWorker> scheduleOnce(
        context: Context,
        tag: String,
        inputData: Map<String, Any> = emptyMap(),
        constraints: WorkConstraints = WorkConstraints(),
        initialDelay: Long = 0,
        delayUnit: TimeUnit = TimeUnit.SECONDS,
        backoffPolicy: BackoffPolicy = BackoffPolicy.EXPONENTIAL,
        backoffDelay: Long = 30,
        backoffDelayUnit: TimeUnit = TimeUnit.SECONDS
    ): String {
        val request = OneTimeWorkRequestBuilder<T>()
            .setConstraints(buildConstraints(constraints))
            .setInputData(buildInputData(inputData))
            .addTag(tag)
            .apply {
                if (initialDelay > 0) {
                    setInitialDelay(initialDelay, delayUnit)
                }
                setBackoffCriteria(
                    if (backoffPolicy == BackoffPolicy.LINEAR) BackoffPolicy.LINEAR
                    else BackoffPolicy.EXPONENTIAL,
                    backoffDelay,
                    backoffDelayUnit
                )
            }
            .build()
        
        WorkManager.getInstance(context)
            .enqueueUniqueWork(tag, ExistingWorkPolicy.REPLACE, request)
        
        return request.id.toString()
    }
    
    /**
     * جدولة عمل دوري
     */
    inline fun <reified T : ListenableWorker> schedulePeriodic(
        context: Context,
        tag: String,
        intervalMinutes: Long,
        inputData: Map<String, Any> = emptyMap(),
        constraints: WorkConstraints = WorkConstraints(),
        flexTimeMinutes: Long = intervalMinutes / 2
    ): String {
        val request = PeriodicWorkRequestBuilder<T>(
            intervalMinutes, TimeUnit.MINUTES,
            flexTimeMinutes, TimeUnit.MINUTES
        )
            .setConstraints(buildConstraints(constraints))
            .setInputData(buildInputData(inputData))
            .addTag(tag)
            .build()
        
        WorkManager.getInstance(context)
            .enqueueUniquePeriodicWork(tag, ExistingPeriodicWorkPolicy.UPDATE, request)
        
        return request.id.toString()
    }
    
    /**
     * جدولة سلسلة أعمال
     */
    inline fun <reified T1 : ListenableWorker, reified T2 : ListenableWorker> scheduleChain(
        context: Context,
        tag: String,
        inputData1: Map<String, Any> = emptyMap(),
        inputData2: Map<String, Any> = emptyMap(),
        constraints: WorkConstraints = WorkConstraints()
    ): String {
        val work1 = OneTimeWorkRequestBuilder<T1>()
            .setConstraints(buildConstraints(constraints))
            .setInputData(buildInputData(inputData1))
            .addTag("${tag}_1")
            .build()
        
        val work2 = OneTimeWorkRequestBuilder<T2>()
            .setConstraints(buildConstraints(constraints))
            .setInputData(buildInputData(inputData2))
            .addTag("${tag}_2")
            .build()
        
        WorkManager.getInstance(context)
            .beginUniqueWork(tag, ExistingWorkPolicy.REPLACE, work1)
            .then(work2)
            .enqueue()
        
        return work1.id.toString()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Management
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * إلغاء عمل بالوسم
     */
    fun cancelByTag(context: Context, tag: String) {
        WorkManager.getInstance(context).cancelAllWorkByTag(tag)
    }
    
    /**
     * إلغاء عمل بالمعرف
     */
    fun cancelById(context: Context, id: String) {
        WorkManager.getInstance(context).cancelWorkById(java.util.UUID.fromString(id))
    }
    
    /**
     * إلغاء جميع الأعمال
     */
    fun cancelAll(context: Context) {
        WorkManager.getInstance(context).cancelAllWork()
    }
    
    /**
     * الحصول على حالة العمل
     */
    suspend fun getWorkState(context: Context, tag: String): WorkState = withContext(Dispatchers.IO) {
        val workInfos = WorkManager.getInstance(context)
            .getWorkInfosByTag(tag)
            .get()
        
        if (workInfos.isEmpty()) return@withContext WorkState.UNKNOWN
        
        when (workInfos.first().state) {
            WorkInfo.State.ENQUEUED -> WorkState.ENQUEUED
            WorkInfo.State.RUNNING -> WorkState.RUNNING
            WorkInfo.State.SUCCEEDED -> WorkState.SUCCEEDED
            WorkInfo.State.FAILED -> WorkState.FAILED
            WorkInfo.State.CANCELLED -> WorkState.CANCELLED
            WorkInfo.State.BLOCKED -> WorkState.BLOCKED
        }
    }
    
    /**
     * هل العمل قيد التشغيل؟
     */
    suspend fun isRunning(context: Context, tag: String): Boolean {
        return getWorkState(context, tag) == WorkState.RUNNING
    }
    
    /**
     * هل العمل في الانتظار؟
     */
    suspend fun isEnqueued(context: Context, tag: String): Boolean {
        val state = getWorkState(context, tag)
        return state == WorkState.ENQUEUED || state == WorkState.RUNNING
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Helpers
    // ═══════════════════════════════════════════════════════════════════════════
    
    @PublishedApi
    internal fun buildConstraints(constraints: WorkConstraints): Constraints {
        return Constraints.Builder().apply {
            if (constraints.requiresNetwork) {
                setRequiredNetworkType(NetworkType.CONNECTED)
            }
            if (constraints.requiresUnmeteredNetwork) {
                setRequiredNetworkType(NetworkType.UNMETERED)
            }
            setRequiresCharging(constraints.requiresCharging)
            setRequiresDeviceIdle(constraints.requiresDeviceIdle)
            setRequiresBatteryNotLow(constraints.requiresBatteryNotLow)
            setRequiresStorageNotLow(constraints.requiresStorageNotLow)
        }.build()
    }
    
    @PublishedApi
    internal fun buildInputData(data: Map<String, Any>): Data {
        val builder = Data.Builder()
        data.forEach { (key, value) ->
            when (value) {
                is String -> builder.putString(key, value)
                is Int -> builder.putInt(key, value)
                is Long -> builder.putLong(key, value)
                is Boolean -> builder.putBoolean(key, value)
                is Double -> builder.putDouble(key, value)
                is Float -> builder.putFloat(key, value)
                is ByteArray -> builder.putByteArray(key, value)
                is Array<*> -> {
                    @Suppress("UNCHECKED_CAST")
                    if (value.isArrayOf<String>()) {
                        builder.putStringArray(key, value as Array<String>)
                    }
                }
            }
        }
        return builder.build()
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Pre-built Workers
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * عامل تنظيف الملفات المؤقتة
 */
class CleanupWorker(
    context: Context,
    params: WorkerParameters
) : SadWorker.SadCoroutineWorker(context, params) {
    
    override suspend fun execute(inputData: Map<String, Any?>): SadWorker.WorkResult {
        return try {
            val cacheDir = applicationContext.cacheDir
            cacheDir.listFiles()?.forEach { file ->
                if (file.lastModified() < System.currentTimeMillis() - 24 * 60 * 60 * 1000) {
                    file.deleteRecursively()
                }
            }
            SadWorker.WorkResult.Success
        } catch (e: Exception) {
            SadWorker.WorkResult.Failure(e.message ?: "خطأ في التنظيف")
        }
    }
}

/**
 * عامل مزامنة البيانات
 */
class SyncWorker(
    context: Context,
    params: WorkerParameters
) : SadWorker.SadCoroutineWorker(context, params) {
    
    override suspend fun execute(inputData: Map<String, Any?>): SadWorker.WorkResult {
        val endpoint = getString("endpoint") ?: return SadWorker.WorkResult.Failure("لا يوجد endpoint")
        
        return try {
            // TODO: Implement sync logic
            SadWorker.WorkResult.Success
        } catch (e: Exception) {
            if (runAttemptCount < 3) {
                SadWorker.WorkResult.Retry
            } else {
                SadWorker.WorkResult.Failure(e.message ?: "فشل المزامنة")
            }
        }
    }
}

/**
 * عامل تنفيذ كود ص
 */
class SadCodeWorker(
    context: Context,
    params: WorkerParameters
) : SadWorker.SadCoroutineWorker(context, params) {
    
    override suspend fun execute(inputData: Map<String, Any?>): SadWorker.WorkResult {
        val code = getString("code") ?: return SadWorker.WorkResult.Failure("لا يوجد كود")
        
        return try {
            val result = SadSdk.interpreter.evalAsync(code)
            when (result) {
                is SadResult.Success -> SadWorker.WorkResult.Success
                is SadResult.Error -> SadWorker.WorkResult.Failure(result.message)
            }
        } catch (e: Exception) {
            SadWorker.WorkResult.Failure(e.message ?: "خطأ في التنفيذ")
        }
    }
}
