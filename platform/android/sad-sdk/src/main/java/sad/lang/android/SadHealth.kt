/**
 * =============================================================================
 * @file SadHealth.kt
 * @brief (AR) واجهة HealthConnect للغة ص
 * @brief (EN) HealthConnect Interface for Sad Language
 * =============================================================================
 */

package sad.lang.android

import android.content.Context
import android.content.Intent
import androidx.health.connect.client.HealthConnectClient
import androidx.health.connect.client.PermissionController
import androidx.health.connect.client.permission.HealthPermission
import androidx.health.connect.client.records.*
import androidx.health.connect.client.request.ReadRecordsRequest
import androidx.health.connect.client.time.TimeRangeFilter
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import java.time.Duration
import java.time.Instant
import java.time.ZoneId
import java.time.format.DateTimeFormatter

/**
 * نظام الصحة واللياقة للغة ص (Health Connect)
 */
class SadHealth(private val context: Context) {
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Types
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * بيانات الخطوات
     */
    data class StepsData(
        val count: Long,
        val startTime: String,
        val endTime: String
    )
    
    /**
     * بيانات نبض القلب
     */
    data class HeartRateData(
        val bpm: Long,
        val time: String
    )
    
    /**
     * بيانات النوم
     */
    data class SleepData(
        val startTime: String,
        val endTime: String,
        val durationMinutes: Long,
        val stages: List<SleepStage>
    )
    
    /**
     * مرحلة النوم
     */
    data class SleepStage(
        val stage: String,
        val startTime: String,
        val endTime: String
    )
    
    /**
     * بيانات السعرات الحرارية
     */
    data class CaloriesData(
        val calories: Double,
        val startTime: String,
        val endTime: String
    )
    
    /**
     * بيانات المسافة
     */
    data class DistanceData(
        val meters: Double,
        val startTime: String,
        val endTime: String
    )
    
    /**
     * بيانات الوزن
     */
    data class WeightData(
        val kg: Double,
        val time: String
    )
    
    /**
     * بيانات الطول
     */
    data class HeightData(
        val cm: Double,
        val time: String
    )
    
    /**
     * بيانات التمرين
     */
    data class ExerciseData(
        val type: String,
        val startTime: String,
        val endTime: String,
        val durationMinutes: Long,
        val caloriesBurned: Double?
    )
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Properties
    // ═══════════════════════════════════════════════════════════════════════════
    
    private val healthClient: HealthConnectClient? by lazy {
        try {
            HealthConnectClient.getOrCreate(context)
        } catch (e: Exception) {
            null
        }
    }
    
    private val dateFormatter = DateTimeFormatter
        .ofPattern("yyyy-MM-dd HH:mm:ss")
        .withZone(ZoneId.systemDefault())
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Availability
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * هل Health Connect متاح؟
     */
    fun isAvailable(): Boolean {
        return HealthConnectClient.getSdkStatus(context) == HealthConnectClient.SDK_AVAILABLE
    }
    
    /**
     * الحصول على حالة SDK
     */
    fun getSdkStatus(): String {
        return when (HealthConnectClient.getSdkStatus(context)) {
            HealthConnectClient.SDK_AVAILABLE -> "متاح"
            HealthConnectClient.SDK_UNAVAILABLE -> "غير متاح"
            HealthConnectClient.SDK_UNAVAILABLE_PROVIDER_UPDATE_REQUIRED -> "يتطلب تحديث"
            else -> "غير معروف"
        }
    }
    
    /**
     * Intent لتثبيت Health Connect
     */
    fun getInstallIntent(): Intent? {
        val packageName = "com.google.android.apps.healthdata"
        return try {
            context.packageManager.getLaunchIntentForPackage(packageName)
        } catch (e: Exception) {
            null
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Permissions
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * الأذونات المطلوبة
     */
    val allPermissions = setOf(
        HealthPermission.getReadPermission(StepsRecord::class),
        HealthPermission.getWritePermission(StepsRecord::class),
        HealthPermission.getReadPermission(HeartRateRecord::class),
        HealthPermission.getWritePermission(HeartRateRecord::class),
        HealthPermission.getReadPermission(SleepSessionRecord::class),
        HealthPermission.getWritePermission(SleepSessionRecord::class),
        HealthPermission.getReadPermission(TotalCaloriesBurnedRecord::class),
        HealthPermission.getWritePermission(TotalCaloriesBurnedRecord::class),
        HealthPermission.getReadPermission(DistanceRecord::class),
        HealthPermission.getWritePermission(DistanceRecord::class),
        HealthPermission.getReadPermission(WeightRecord::class),
        HealthPermission.getWritePermission(WeightRecord::class),
        HealthPermission.getReadPermission(HeightRecord::class),
        HealthPermission.getWritePermission(HeightRecord::class),
        HealthPermission.getReadPermission(ExerciseSessionRecord::class),
        HealthPermission.getWritePermission(ExerciseSessionRecord::class)
    )
    
    /**
     * الحصول على الأذونات الممنوحة
     */
    suspend fun getGrantedPermissions(): Set<String> = withContext(Dispatchers.IO) {
        healthClient?.permissionController?.getGrantedPermissions() ?: emptySet()
    }
    
    /**
     * هل لدينا جميع الأذونات؟
     */
    suspend fun hasAllPermissions(): Boolean {
        val granted = getGrantedPermissions()
        return allPermissions.all { it in granted }
    }
    
    /**
     * Intent لطلب الأذونات
     */
    fun createPermissionRequestIntent(): Intent {
        return PermissionController.createRequestPermissionResultContract()
            .createIntent(context, allPermissions)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Read Data
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * قراءة الخطوات
     */
    suspend fun readSteps(startTime: Instant, endTime: Instant): List<StepsData> = withContext(Dispatchers.IO) {
        val client = healthClient ?: return@withContext emptyList()
        
        try {
            val response = client.readRecords(
                ReadRecordsRequest(
                    StepsRecord::class,
                    timeRangeFilter = TimeRangeFilter.between(startTime, endTime)
                )
            )
            
            response.records.map { record ->
                StepsData(
                    count = record.count,
                    startTime = dateFormatter.format(record.startTime),
                    endTime = dateFormatter.format(record.endTime)
                )
            }
        } catch (e: Exception) {
            emptyList()
        }
    }
    
    /**
     * قراءة خطوات اليوم
     */
    suspend fun readTodaySteps(): Long {
        val now = Instant.now()
        val startOfDay = now.atZone(ZoneId.systemDefault())
            .toLocalDate()
            .atStartOfDay(ZoneId.systemDefault())
            .toInstant()
        
        return readSteps(startOfDay, now).sumOf { it.count }
    }
    
    /**
     * قراءة نبض القلب
     */
    suspend fun readHeartRate(startTime: Instant, endTime: Instant): List<HeartRateData> = withContext(Dispatchers.IO) {
        val client = healthClient ?: return@withContext emptyList()
        
        try {
            val response = client.readRecords(
                ReadRecordsRequest(
                    HeartRateRecord::class,
                    timeRangeFilter = TimeRangeFilter.between(startTime, endTime)
                )
            )
            
            response.records.flatMap { record ->
                record.samples.map { sample ->
                    HeartRateData(
                        bpm = sample.beatsPerMinute,
                        time = dateFormatter.format(sample.time)
                    )
                }
            }
        } catch (e: Exception) {
            emptyList()
        }
    }
    
    /**
     * قراءة بيانات النوم
     */
    suspend fun readSleep(startTime: Instant, endTime: Instant): List<SleepData> = withContext(Dispatchers.IO) {
        val client = healthClient ?: return@withContext emptyList()
        
        try {
            val response = client.readRecords(
                ReadRecordsRequest(
                    SleepSessionRecord::class,
                    timeRangeFilter = TimeRangeFilter.between(startTime, endTime)
                )
            )
            
            response.records.map { record ->
                val duration = Duration.between(record.startTime, record.endTime)
                
                SleepData(
                    startTime = dateFormatter.format(record.startTime),
                    endTime = dateFormatter.format(record.endTime),
                    durationMinutes = duration.toMinutes(),
                    stages = record.stages.map { stage ->
                        SleepStage(
                            stage = getSleepStageName(stage.stage),
                            startTime = dateFormatter.format(stage.startTime),
                            endTime = dateFormatter.format(stage.endTime)
                        )
                    }
                )
            }
        } catch (e: Exception) {
            emptyList()
        }
    }
    
    /**
     * قراءة السعرات الحرارية
     */
    suspend fun readCalories(startTime: Instant, endTime: Instant): List<CaloriesData> = withContext(Dispatchers.IO) {
        val client = healthClient ?: return@withContext emptyList()
        
        try {
            val response = client.readRecords(
                ReadRecordsRequest(
                    TotalCaloriesBurnedRecord::class,
                    timeRangeFilter = TimeRangeFilter.between(startTime, endTime)
                )
            )
            
            response.records.map { record ->
                CaloriesData(
                    calories = record.energy.inKilocalories,
                    startTime = dateFormatter.format(record.startTime),
                    endTime = dateFormatter.format(record.endTime)
                )
            }
        } catch (e: Exception) {
            emptyList()
        }
    }
    
    /**
     * قراءة المسافة
     */
    suspend fun readDistance(startTime: Instant, endTime: Instant): List<DistanceData> = withContext(Dispatchers.IO) {
        val client = healthClient ?: return@withContext emptyList()
        
        try {
            val response = client.readRecords(
                ReadRecordsRequest(
                    DistanceRecord::class,
                    timeRangeFilter = TimeRangeFilter.between(startTime, endTime)
                )
            )
            
            response.records.map { record ->
                DistanceData(
                    meters = record.distance.inMeters,
                    startTime = dateFormatter.format(record.startTime),
                    endTime = dateFormatter.format(record.endTime)
                )
            }
        } catch (e: Exception) {
            emptyList()
        }
    }
    
    /**
     * قراءة الوزن
     */
    suspend fun readWeight(startTime: Instant, endTime: Instant): List<WeightData> = withContext(Dispatchers.IO) {
        val client = healthClient ?: return@withContext emptyList()
        
        try {
            val response = client.readRecords(
                ReadRecordsRequest(
                    WeightRecord::class,
                    timeRangeFilter = TimeRangeFilter.between(startTime, endTime)
                )
            )
            
            response.records.map { record ->
                WeightData(
                    kg = record.weight.inKilograms,
                    time = dateFormatter.format(record.time)
                )
            }
        } catch (e: Exception) {
            emptyList()
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Write Data
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * كتابة الخطوات
     */
    suspend fun writeSteps(count: Long, startTime: Instant, endTime: Instant): Boolean = withContext(Dispatchers.IO) {
        val client = healthClient ?: return@withContext false
        
        try {
            val record = StepsRecord(
                count = count,
                startTime = startTime,
                endTime = endTime,
                startZoneOffset = ZoneId.systemDefault().rules.getOffset(startTime),
                endZoneOffset = ZoneId.systemDefault().rules.getOffset(endTime)
            )
            client.insertRecords(listOf(record))
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * كتابة الوزن
     */
    suspend fun writeWeight(kg: Double, time: Instant = Instant.now()): Boolean = withContext(Dispatchers.IO) {
        val client = healthClient ?: return@withContext false
        
        try {
            val record = WeightRecord(
                weight = androidx.health.connect.client.units.Mass.kilograms(kg),
                time = time,
                zoneOffset = ZoneId.systemDefault().rules.getOffset(time)
            )
            client.insertRecords(listOf(record))
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * كتابة الطول
     */
    suspend fun writeHeight(cm: Double, time: Instant = Instant.now()): Boolean = withContext(Dispatchers.IO) {
        val client = healthClient ?: return@withContext false
        
        try {
            val record = HeightRecord(
                height = androidx.health.connect.client.units.Length.centimeters(cm),
                time = time,
                zoneOffset = ZoneId.systemDefault().rules.getOffset(time)
            )
            client.insertRecords(listOf(record))
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Helpers
    // ═══════════════════════════════════════════════════════════════════════════
    
    private fun getSleepStageName(stage: Int): String {
        return when (stage) {
            SleepSessionRecord.STAGE_TYPE_AWAKE -> "مستيقظ"
            SleepSessionRecord.STAGE_TYPE_LIGHT -> "نوم خفيف"
            SleepSessionRecord.STAGE_TYPE_DEEP -> "نوم عميق"
            SleepSessionRecord.STAGE_TYPE_REM -> "نوم REM"
            SleepSessionRecord.STAGE_TYPE_OUT_OF_BED -> "خارج السرير"
            SleepSessionRecord.STAGE_TYPE_SLEEPING -> "نائم"
            else -> "غير معروف"
        }
    }
}
