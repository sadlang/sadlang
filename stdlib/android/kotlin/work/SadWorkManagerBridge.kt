// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadWorkManagerBridge.kt
// الوصف: جسر Kotlin لإدارة المهام الخلفية WorkManager
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import androidx.work.*
import java.util.*
import java.util.concurrent.TimeUnit

/**
 * جسر إدارة المهام الخلفية — SadWorkManagerBridge
 */
object SadWorkManagerBridge {
    
    private var context: Context? = null
    private var workManager: WorkManager? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        workManager = WorkManager.getInstance(ctx.applicationContext)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // One-Time Work
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun enqueueOneTimeWork(
        workerClass: Class<out ListenableWorker>,
        inputData: Map<String, Any>? = null,
        constraints: Map<String, Any>? = null,
        tags: List<String>? = null,
        uniqueWorkName: String? = null,
        existingWorkPolicy: String = "KEEP"
    ): String {
        val wm = workManager ?: return ""
        
        val workRequestBuilder = OneTimeWorkRequest.Builder(workerClass)
        
        // Input data
        inputData?.let {
            workRequestBuilder.setInputData(buildDataFromMap(it))
        }
        
        // Constraints
        constraints?.let {
            workRequestBuilder.setConstraints(buildConstraints(it))
        }
        
        // Tags
        tags?.forEach { tag ->
            workRequestBuilder.addTag(tag)
        }
        
        val workRequest = workRequestBuilder.build()
        
        // Enqueue
        if (uniqueWorkName != null) {
            val policy = when (existingWorkPolicy.uppercase()) {
                "REPLACE" -> ExistingWorkPolicy.REPLACE
                "APPEND" -> ExistingWorkPolicy.APPEND
                "APPEND_OR_REPLACE" -> ExistingWorkPolicy.APPEND_OR_REPLACE
                else -> ExistingWorkPolicy.KEEP
            }
            wm.enqueueUniqueWork(uniqueWorkName, policy, workRequest)
        } else {
            wm.enqueue(workRequest)
        }
        
        return workRequest.id.toString()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Periodic Work
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun enqueuePeriodicWork(
        workerClass: Class<out ListenableWorker>,
        repeatIntervalMinutes: Long,
        flexIntervalMinutes: Long? = null,
        inputData: Map<String, Any>? = null,
        constraints: Map<String, Any>? = null,
        tags: List<String>? = null,
        uniqueWorkName: String,
        existingPolicy: String = "KEEP"
    ): String {
        val wm = workManager ?: return ""
        
        val workRequestBuilder = if (flexIntervalMinutes != null) {
            PeriodicWorkRequest.Builder(
                workerClass,
                repeatIntervalMinutes, TimeUnit.MINUTES,
                flexIntervalMinutes, TimeUnit.MINUTES
            )
        } else {
            PeriodicWorkRequest.Builder(
                workerClass,
                repeatIntervalMinutes, TimeUnit.MINUTES
            )
        }
        
        // Input data
        inputData?.let {
            workRequestBuilder.setInputData(buildDataFromMap(it))
        }
        
        // Constraints
        constraints?.let {
            workRequestBuilder.setConstraints(buildConstraints(it))
        }
        
        // Tags
        tags?.forEach { tag ->
            workRequestBuilder.addTag(tag)
        }
        
        val workRequest = workRequestBuilder.build()
        
        val policy = when (existingPolicy.uppercase()) {
            "REPLACE" -> ExistingPeriodicWorkPolicy.REPLACE
            "UPDATE" -> ExistingPeriodicWorkPolicy.UPDATE
            else -> ExistingPeriodicWorkPolicy.KEEP
        }
        
        wm.enqueueUniquePeriodicWork(uniqueWorkName, policy, workRequest)
        
        return workRequest.id.toString()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Work Chain
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun beginChain(workerClass: Class<out ListenableWorker>): WorkContinuation? {
        val wm = workManager ?: return null
        val workRequest = OneTimeWorkRequest.from(workerClass)
        return wm.beginWith(workRequest)
    }
    
    @JvmStatic
    fun chainThen(
        continuation: WorkContinuation,
        workerClass: Class<out ListenableWorker>
    ): WorkContinuation {
        val workRequest = OneTimeWorkRequest.from(workerClass)
        return continuation.then(workRequest)
    }
    
    @JvmStatic
    fun enqueueChain(continuation: WorkContinuation): Operation {
        return continuation.enqueue()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Cancel Work
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun cancelWorkById(workId: String): Boolean {
        val wm = workManager ?: return false
        
        return try {
            val uuid = UUID.fromString(workId)
            wm.cancelWorkById(uuid)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun cancelWorkByTag(tag: String): Boolean {
        val wm = workManager ?: return false
        
        return try {
            wm.cancelAllWorkByTag(tag)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun cancelUniqueWork(uniqueWorkName: String): Boolean {
        val wm = workManager ?: return false
        
        return try {
            wm.cancelUniqueWork(uniqueWorkName)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun cancelAllWork(): Boolean {
        val wm = workManager ?: return false
        
        return try {
            wm.cancelAllWork()
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Work Status
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getWorkInfoById(workId: String, callback: (Map<String, Any?>?) -> Unit) {
        val wm = workManager ?: run {
            callback(null)
            return
        }
        
        try {
            val uuid = UUID.fromString(workId)
            val workInfo = wm.getWorkInfoById(uuid).get()
            
            callback(workInfo?.let { workInfoToMap(it) })
        } catch (e: Exception) {
            callback(null)
        }
    }
    
    @JvmStatic
    fun getWorkInfosByTag(tag: String, callback: (List<Map<String, Any?>>) -> Unit) {
        val wm = workManager ?: run {
            callback(emptyList())
            return
        }
        
        try {
            val workInfos = wm.getWorkInfosByTag(tag).get()
            callback(workInfos.map { workInfoToMap(it) })
        } catch (e: Exception) {
            callback(emptyList())
        }
    }
    
    @JvmStatic
    fun getWorkInfosForUniqueWork(uniqueWorkName: String, callback: (List<Map<String, Any?>>) -> Unit) {
        val wm = workManager ?: run {
            callback(emptyList())
            return
        }
        
        try {
            val workInfos = wm.getWorkInfosForUniqueWork(uniqueWorkName).get()
            callback(workInfos.map { workInfoToMap(it) })
        } catch (e: Exception) {
            callback(emptyList())
        }
    }
    
    @JvmStatic
    fun pruneWork(): Boolean {
        val wm = workManager ?: return false
        
        return try {
            wm.pruneWork()
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Observe Work
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun observeWorkById(workId: String): androidx.lifecycle.LiveData<WorkInfo>? {
        val wm = workManager ?: return null
        
        return try {
            val uuid = UUID.fromString(workId)
            wm.getWorkInfoByIdLiveData(uuid)
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun observeWorkByTag(tag: String): androidx.lifecycle.LiveData<List<WorkInfo>>? {
        val wm = workManager ?: return null
        return wm.getWorkInfosByTagLiveData(tag)
    }
    
    @JvmStatic
    fun observeUniqueWork(uniqueWorkName: String): androidx.lifecycle.LiveData<List<WorkInfo>>? {
        val wm = workManager ?: return null
        return wm.getWorkInfosForUniqueWorkLiveData(uniqueWorkName)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Helper Functions
    // ═══════════════════════════════════════════════════════════════════════════════
    
    private fun buildDataFromMap(map: Map<String, Any>): Data {
        val builder = Data.Builder()
        
        map.forEach { (key, value) ->
            when (value) {
                is String -> builder.putString(key, value)
                is Int -> builder.putInt(key, value)
                is Long -> builder.putLong(key, value)
                is Float -> builder.putFloat(key, value)
                is Double -> builder.putDouble(key, value)
                is Boolean -> builder.putBoolean(key, value)
                is Array<*> -> {
                    when {
                        value.isArrayOf<String>() -> builder.putStringArray(key, value as Array<String>)
                    }
                }
            }
        }
        
        return builder.build()
    }
    
    private fun buildConstraints(map: Map<String, Any>): Constraints {
        val builder = Constraints.Builder()
        
        (map["networkType"] as? String)?.let { networkType ->
            val type = when (networkType.uppercase()) {
                "CONNECTED" -> NetworkType.CONNECTED
                "METERED" -> NetworkType.METERED
                "NOT_ROAMING" -> NetworkType.NOT_ROAMING
                "UNMETERED" -> NetworkType.UNMETERED
                else -> NetworkType.NOT_REQUIRED
            }
            builder.setRequiredNetworkType(type)
        }
        
        (map["requiresCharging"] as? Boolean)?.let {
            builder.setRequiresCharging(it)
        }
        
        (map["requiresBatteryNotLow"] as? Boolean)?.let {
            builder.setRequiresBatteryNotLow(it)
        }
        
        (map["requiresStorageNotLow"] as? Boolean)?.let {
            builder.setRequiresStorageNotLow(it)
        }
        
        (map["requiresDeviceIdle"] as? Boolean)?.let {
            builder.setRequiresDeviceIdle(it)
        }
        
        return builder.build()
    }
    
    private fun workInfoToMap(workInfo: WorkInfo): Map<String, Any?> {
        return mapOf(
            "id" to workInfo.id.toString(),
            "state" to getStateLabel(workInfo.state),
            "stateArabic" to getStateArabicLabel(workInfo.state),
            "tags" to workInfo.tags.toList(),
            "runAttemptCount" to workInfo.runAttemptCount,
            "outputData" to workInfo.outputData.keyValueMap
        )
    }
    
    private fun getStateLabel(state: WorkInfo.State): String {
        return when (state) {
            WorkInfo.State.ENQUEUED -> "ENQUEUED"
            WorkInfo.State.RUNNING -> "RUNNING"
            WorkInfo.State.SUCCEEDED -> "SUCCEEDED"
            WorkInfo.State.FAILED -> "FAILED"
            WorkInfo.State.BLOCKED -> "BLOCKED"
            WorkInfo.State.CANCELLED -> "CANCELLED"
        }
    }
    
    private fun getStateArabicLabel(state: WorkInfo.State): String {
        return when (state) {
            WorkInfo.State.ENQUEUED -> "في قائمة الانتظار"
            WorkInfo.State.RUNNING -> "قيد التنفيذ"
            WorkInfo.State.SUCCEEDED -> "نجحت"
            WorkInfo.State.FAILED -> "فشلت"
            WorkInfo.State.BLOCKED -> "محجوبة"
            WorkInfo.State.CANCELLED -> "ملغية"
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Predefined Constraints
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getNetworkConstraints(): Map<String, Any> {
        return mapOf("networkType" to "CONNECTED")
    }
    
    @JvmStatic
    fun getWifiConstraints(): Map<String, Any> {
        return mapOf("networkType" to "UNMETERED")
    }
    
    @JvmStatic
    fun getChargingConstraints(): Map<String, Any> {
        return mapOf(
            "requiresCharging" to true,
            "requiresBatteryNotLow" to true
        )
    }
    
    @JvmStatic
    fun getIdleConstraints(): Map<String, Any> {
        return mapOf("requiresDeviceIdle" to true)
    }
    
    @JvmStatic
    fun getSyncConstraints(): Map<String, Any> {
        return mapOf(
            "networkType" to "CONNECTED",
            "requiresBatteryNotLow" to true
        )
    }
}
