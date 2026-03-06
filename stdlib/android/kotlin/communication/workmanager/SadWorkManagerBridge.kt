package sad.android.communication.workmanager

import android.content.Context
import androidx.work.*
import java.util.concurrent.TimeUnit

/**
 * جسر WorkManager - جدولة المهام في الخلفية
 * WorkManager Bridge - Background task scheduling
 * 
 * @author فريق لغة ص
 */
object SadWorkManagerBridge {
    
    /**
     * نوع العمل / Work type
     */
    enum class WorkType {
        ONE_TIME,       // مرة واحدة
        PERIODIC        // دوري
    }
    
    /**
     * سياسة الشبكة / Network type
     */
    enum class NetworkRequirement {
        NONE,               // لا يحتاج شبكة
        CONNECTED,          // أي اتصال
        UNMETERED,          // Wi-Fi فقط
        NOT_ROAMING,        // ليس تجوال
        METERED             // بيانات
    }
    
    /**
     * قيود العمل / Work constraints
     */
    data class WorkConstraints(
        val networkType: NetworkRequirement = NetworkRequirement.NONE,
        val requiresCharging: Boolean = false,
        val requiresBatteryNotLow: Boolean = false,
        val requiresDeviceIdle: Boolean = false,
        val requiresStorageNotLow: Boolean = false
    )
    
    /**
     * طلب عمل / Work request
     */
    data class WorkRequest(
        val tag: String,
        val workerClass: Class<out ListenableWorker>,
        val type: WorkType = WorkType.ONE_TIME,
        val constraints: WorkConstraints = WorkConstraints(),
        val initialDelay: Long = 0,
        val delayUnit: TimeUnit = TimeUnit.SECONDS,
        val inputData: Map<String, Any?> = emptyMap(),
        // للعمل الدوري
        val repeatInterval: Long = 15,
        val repeatUnit: TimeUnit = TimeUnit.MINUTES,
        val flexInterval: Long? = null,
        // سياسة الاستبدال
        val existingWorkPolicy: ExistingWorkPolicy = ExistingWorkPolicy.REPLACE
    )
    
    /**
     * معلومات حالة العمل / Work status info
     */
    data class WorkStatus(
        val id: java.util.UUID,
        val tag: String,
        val state: String,
        val progress: Int,
        val outputData: Map<String, Any?>
    )
    
    // ═══════════════════════════════════════════════════════════════
    //                         إنشاء العمل
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * جدولة عمل / Schedule work
     */
    fun scheduleWork(context: Context, request: WorkRequest): java.util.UUID {
        val workManager = WorkManager.getInstance(context)
        val constraints = buildConstraints(request.constraints)
        val inputData = buildInputData(request.inputData)
        
        return when (request.type) {
            WorkType.ONE_TIME -> {
                val workRequest = OneTimeWorkRequestBuilder(request.workerClass)
                    .setConstraints(constraints)
                    .setInputData(inputData)
                    .addTag(request.tag)
                    .apply {
                        if (request.initialDelay > 0) {
                            setInitialDelay(request.initialDelay, request.delayUnit)
                        }
                    }
                    .build()
                
                workManager.enqueueUniqueWork(
                    request.tag,
                    request.existingWorkPolicy,
                    workRequest
                )
                workRequest.id
            }
            WorkType.PERIODIC -> {
                val workRequest = if (request.flexInterval != null) {
                    PeriodicWorkRequestBuilder(
                        request.workerClass,
                        request.repeatInterval,
                        request.repeatUnit,
                        request.flexInterval,
                        request.repeatUnit
                    )
                } else {
                    PeriodicWorkRequestBuilder(
                        request.workerClass,
                        request.repeatInterval,
                        request.repeatUnit
                    )
                }
                    .setConstraints(constraints)
                    .setInputData(inputData)
                    .addTag(request.tag)
                    .apply {
                        if (request.initialDelay > 0) {
                            setInitialDelay(request.initialDelay, request.delayUnit)
                        }
                    }
                    .build()
                
                workManager.enqueueUniquePeriodicWork(
                    request.tag,
                    ExistingPeriodicWorkPolicy.valueOf(request.existingWorkPolicy.name),
                    workRequest
                )
                workRequest.id
            }
        }
    }
    
    /**
     * جدولة عمل بسيط / Schedule simple work
     */
    inline fun <reified W : ListenableWorker> scheduleSimpleWork(
        context: Context,
        tag: String,
        data: Map<String, Any?> = emptyMap()
    ): java.util.UUID {
        return scheduleWork(context, WorkRequest(
            tag = tag,
            workerClass = W::class.java,
            inputData = data
        ))
    }
    
    /**
     * جدولة عمل دوري / Schedule periodic work
     */
    inline fun <reified W : ListenableWorker> schedulePeriodicWork(
        context: Context,
        tag: String,
        intervalMinutes: Long = 15,
        data: Map<String, Any?> = emptyMap()
    ): java.util.UUID {
        return scheduleWork(context, WorkRequest(
            tag = tag,
            workerClass = W::class.java,
            type = WorkType.PERIODIC,
            repeatInterval = intervalMinutes,
            repeatUnit = TimeUnit.MINUTES,
            inputData = data
        ))
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         سلاسل العمل
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إنشاء سلسلة عمل / Create work chain
     */
    fun beginChain(
        context: Context,
        uniqueName: String,
        policy: ExistingWorkPolicy = ExistingWorkPolicy.REPLACE,
        firstWork: OneTimeWorkRequest
    ): WorkContinuation {
        return WorkManager.getInstance(context)
            .beginUniqueWork(uniqueName, policy, firstWork)
    }
    
    /**
     * إنشاء طلب عمل مرة واحدة / Create one-time work request
     */
    inline fun <reified W : ListenableWorker> createOneTimeRequest(
        tag: String,
        data: Map<String, Any?> = emptyMap(),
        constraints: WorkConstraints = WorkConstraints()
    ): OneTimeWorkRequest {
        return OneTimeWorkRequestBuilder<W>()
            .setConstraints(buildConstraints(constraints))
            .setInputData(buildInputData(data))
            .addTag(tag)
            .build()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الإلغاء
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إلغاء عمل بالتاج / Cancel by tag
     */
    fun cancelByTag(context: Context, tag: String) {
        WorkManager.getInstance(context).cancelAllWorkByTag(tag)
    }
    
    /**
     * إلغاء عمل بالمعرف / Cancel by ID
     */
    fun cancelById(context: Context, id: java.util.UUID) {
        WorkManager.getInstance(context).cancelWorkById(id)
    }
    
    /**
     * إلغاء عمل فريد / Cancel unique work
     */
    fun cancelUnique(context: Context, uniqueName: String) {
        WorkManager.getInstance(context).cancelUniqueWork(uniqueName)
    }
    
    /**
     * إلغاء جميع الأعمال / Cancel all work
     */
    fun cancelAll(context: Context) {
        WorkManager.getInstance(context).cancelAllWork()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الاستعلام
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * جلب حالة العمل / Get work status
     */
    fun getWorkStatus(context: Context, id: java.util.UUID): WorkStatus? {
        val workInfo = WorkManager.getInstance(context)
            .getWorkInfoById(id)
            .get() ?: return null
        
        return WorkStatus(
            id = workInfo.id,
            tag = workInfo.tags.firstOrNull() ?: "",
            state = workInfo.state.name,
            progress = workInfo.progress.getInt("progress", 0),
            outputData = extractData(workInfo.outputData)
        )
    }
    
    /**
     * جلب أعمال بالتاج / Get works by tag
     */
    fun getWorksByTag(context: Context, tag: String): List<WorkStatus> {
        return WorkManager.getInstance(context)
            .getWorkInfosByTag(tag)
            .get()
            .map { workInfo ->
                WorkStatus(
                    id = workInfo.id,
                    tag = workInfo.tags.firstOrNull() ?: "",
                    state = workInfo.state.name,
                    progress = workInfo.progress.getInt("progress", 0),
                    outputData = extractData(workInfo.outputData)
                )
            }
    }
    
    /**
     * هل العمل قيد التشغيل؟ / Is work running?
     */
    fun isWorkRunning(context: Context, tag: String): Boolean {
        return getWorksByTag(context, tag).any {
            it.state == WorkInfo.State.RUNNING.name || it.state == WorkInfo.State.ENQUEUED.name
        }
    }
    
    /**
     * مراقبة حالة العمل / Observe work status
     */
    fun observeWork(
        context: Context,
        id: java.util.UUID,
        lifecycleOwner: androidx.lifecycle.LifecycleOwner,
        observer: (WorkStatus?) -> Unit
    ) {
        WorkManager.getInstance(context)
            .getWorkInfoByIdLiveData(id)
            .observe(lifecycleOwner) { workInfo ->
                observer(workInfo?.let {
                    WorkStatus(
                        id = it.id,
                        tag = it.tags.firstOrNull() ?: "",
                        state = it.state.name,
                        progress = it.progress.getInt("progress", 0),
                        outputData = extractData(it.outputData)
                    )
                })
            }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    fun buildConstraints(constraints: WorkConstraints): Constraints {
        return Constraints.Builder()
            .setRequiredNetworkType(
                when (constraints.networkType) {
                    NetworkRequirement.NONE -> NetworkType.NOT_REQUIRED
                    NetworkRequirement.CONNECTED -> NetworkType.CONNECTED
                    NetworkRequirement.UNMETERED -> NetworkType.UNMETERED
                    NetworkRequirement.NOT_ROAMING -> NetworkType.NOT_ROAMING
                    NetworkRequirement.METERED -> NetworkType.METERED
                }
            )
            .setRequiresCharging(constraints.requiresCharging)
            .setRequiresBatteryNotLow(constraints.requiresBatteryNotLow)
            .setRequiresDeviceIdle(constraints.requiresDeviceIdle)
            .setRequiresStorageNotLow(constraints.requiresStorageNotLow)
            .build()
    }
    
    fun buildInputData(data: Map<String, Any?>): Data {
        val builder = Data.Builder()
        data.forEach { (key, value) ->
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
                        value.isArrayOf<Int>() -> builder.putIntArray(key, (value as Array<Int>).toIntArray())
                    }
                }
            }
        }
        return builder.build()
    }
    
    private fun extractData(data: Data): Map<String, Any?> {
        val result = mutableMapOf<String, Any?>()
        data.keyValueMap.forEach { (key, value) ->
            result[key] = value
        }
        return result
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         بناء الطلبات
    // ═══════════════════════════════════════════════════════════════
    
    private inline fun <reified W : ListenableWorker> OneTimeWorkRequestBuilder(): OneTimeWorkRequest.Builder {
        return OneTimeWorkRequest.Builder(W::class.java)
    }
    
    private fun OneTimeWorkRequestBuilder(workerClass: Class<out ListenableWorker>): OneTimeWorkRequest.Builder {
        return OneTimeWorkRequest.Builder(workerClass)
    }
    
    private inline fun <reified W : ListenableWorker> PeriodicWorkRequestBuilder(
        repeatInterval: Long,
        repeatIntervalTimeUnit: TimeUnit
    ): PeriodicWorkRequest.Builder {
        return PeriodicWorkRequest.Builder(W::class.java, repeatInterval, repeatIntervalTimeUnit)
    }
    
    private fun PeriodicWorkRequestBuilder(
        workerClass: Class<out ListenableWorker>,
        repeatInterval: Long,
        repeatIntervalTimeUnit: TimeUnit
    ): PeriodicWorkRequest.Builder {
        return PeriodicWorkRequest.Builder(workerClass, repeatInterval, repeatIntervalTimeUnit)
    }
    
    private fun PeriodicWorkRequestBuilder(
        workerClass: Class<out ListenableWorker>,
        repeatInterval: Long,
        repeatIntervalTimeUnit: TimeUnit,
        flexInterval: Long,
        flexIntervalTimeUnit: TimeUnit
    ): PeriodicWorkRequest.Builder {
        return PeriodicWorkRequest.Builder(
            workerClass,
            repeatInterval, repeatIntervalTimeUnit,
            flexInterval, flexIntervalTimeUnit
        )
    }
}
