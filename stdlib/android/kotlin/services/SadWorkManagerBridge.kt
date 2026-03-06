// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadWorkManagerBridge.kt
// الوصف: جسر Kotlin لـ WorkManager
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import androidx.work.*
import java.util.concurrent.TimeUnit

/**
 * جسر WorkManager — SadWorkManagerBridge
 */
object SadWorkManagerBridge {
    
    private var context: Context? = null
    private var workManager: WorkManager? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        workManager = WorkManager.getInstance(ctx.applicationContext)
    }
    
    @JvmStatic
    fun scheduleOneTimeWork(
        workerClass: Class<out ListenableWorker>,
        workName: String,
        inputData: Map<String, Any>? = null,
        constraints: WorkConstraints? = null
    ): String {
        val wm = workManager ?: return ""
        
        val data = inputData?.let { buildData(it) } ?: Data.EMPTY
        val workConstraints = constraints?.build() ?: Constraints.NONE
        
        val workRequest = OneTimeWorkRequestBuilder<ListenableWorker>()
            .setInputData(data)
            .setConstraints(workConstraints)
            .addTag(workName)
            .build()
        
        wm.enqueueUniqueWork(workName, ExistingWorkPolicy.REPLACE, workRequest as OneTimeWorkRequest)
        
        return workRequest.id.toString()
    }
    
    @JvmStatic
    fun schedulePeriodicWork(
        workerClass: Class<out ListenableWorker>,
        workName: String,
        repeatIntervalMinutes: Long,
        inputData: Map<String, Any>? = null,
        constraints: WorkConstraints? = null
    ): String {
        val wm = workManager ?: return ""
        
        val data = inputData?.let { buildData(it) } ?: Data.EMPTY
        val workConstraints = constraints?.build() ?: Constraints.NONE
        
        val workRequest = PeriodicWorkRequestBuilder<ListenableWorker>(
            repeatIntervalMinutes, TimeUnit.MINUTES
        )
            .setInputData(data)
            .setConstraints(workConstraints)
            .addTag(workName)
            .build()
        
        wm.enqueueUniquePeriodicWork(
            workName,
            ExistingPeriodicWorkPolicy.REPLACE,
            workRequest as PeriodicWorkRequest
        )
        
        return workRequest.id.toString()
    }
    
    @JvmStatic
    fun cancelWork(workName: String) {
        workManager?.cancelUniqueWork(workName)
    }
    
    @JvmStatic
    fun cancelAllWork() {
        workManager?.cancelAllWork()
    }
    
    @JvmStatic
    fun cancelByTag(tag: String) {
        workManager?.cancelAllWorkByTag(tag)
    }
    
    @JvmStatic
    fun getWorkStatus(workName: String): String {
        val wm = workManager ?: return "غير معروف"
        
        val workInfos = wm.getWorkInfosForUniqueWork(workName).get()
        if (workInfos.isEmpty()) return "غير موجود"
        
        return when (workInfos[0].state) {
            WorkInfo.State.ENQUEUED -> "في الانتظار"
            WorkInfo.State.RUNNING -> "قيد التشغيل"
            WorkInfo.State.SUCCEEDED -> "ناجح"
            WorkInfo.State.FAILED -> "فاشل"
            WorkInfo.State.BLOCKED -> "محظور"
            WorkInfo.State.CANCELLED -> "ملغى"
        }
    }
    
    @JvmStatic
    fun isWorkRunning(workName: String): Boolean {
        val status = getWorkStatus(workName)
        return status == "قيد التشغيل" || status == "في الانتظار"
    }
    
    private fun buildData(map: Map<String, Any>): Data {
        val builder = Data.Builder()
        
        for ((key, value) in map) {
            when (value) {
                is String -> builder.putString(key, value)
                is Int -> builder.putInt(key, value)
                is Long -> builder.putLong(key, value)
                is Float -> builder.putFloat(key, value)
                is Double -> builder.putDouble(key, value)
                is Boolean -> builder.putBoolean(key, value)
                is Array<*> -> {
                    @Suppress("UNCHECKED_CAST")
                    when {
                        value.isArrayOf<String>() -> builder.putStringArray(key, value as Array<String>)
                    }
                }
            }
        }
        
        return builder.build()
    }
    
    /**
     * كلاس مساعد لبناء القيود
     */
    class WorkConstraints {
        private val builder = Constraints.Builder()
        
        fun requiresNetwork(): WorkConstraints {
            builder.setRequiredNetworkType(NetworkType.CONNECTED)
            return this
        }
        
        fun requiresUnmetered(): WorkConstraints {
            builder.setRequiredNetworkType(NetworkType.UNMETERED)
            return this
        }
        
        fun requiresCharging(): WorkConstraints {
            builder.setRequiresCharging(true)
            return this
        }
        
        fun requiresIdle(): WorkConstraints {
            builder.setRequiresDeviceIdle(true)
            return this
        }
        
        fun requiresStorageNotLow(): WorkConstraints {
            builder.setRequiresStorageNotLow(true)
            return this
        }
        
        fun requiresBatteryNotLow(): WorkConstraints {
            builder.setRequiresBatteryNotLow(true)
            return this
        }
        
        fun build(): Constraints = builder.build()
    }
    
    @JvmStatic
    fun createConstraints(): WorkConstraints = WorkConstraints()
}
