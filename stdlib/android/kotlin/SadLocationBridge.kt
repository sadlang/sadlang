// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadLocationBridge.kt
// الوصف: جسر Kotlin لخدمات الموقع في لغة ص
// ═══════════════════════════════════════════════════════════════════════════════
//
// هذا الملف يربط دوال __اندرويد_موقع_* بـ Android FusedLocationProvider
//
// الدوال المُنفّذة:
//   __اندرويد_موقع_الحالي() -> {lat, lng, accuracy}
//   __اندرويد_موقع_تتبع(callback) -> trackerId
//   __اندرويد_موقع_اوقف(trackerId) -> bool
//
// المتطلبات:
//   implementation("com.google.android.gms:play-services-location:21.0.1")
//   
//   إضافة في AndroidManifest.xml:
//   <uses-permission android:name="android.permission.ACCESS_FINE_LOCATION" />
//   <uses-permission android:name="android.permission.ACCESS_COARSE_LOCATION" />
//
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.location.Location
import android.os.Looper
import androidx.core.app.ActivityCompat
import com.google.android.gms.location.*
import kotlinx.coroutines.*
import kotlin.coroutines.resume
import kotlin.coroutines.suspendCoroutine

/**
 * جسر الموقع — SadLocationBridge
 * 
 * يوفر واجهة للحصول على الموقع الجغرافي من كود لغة ص.
 * يستخدم FusedLocationProviderClient للدقة والكفاءة.
 */
object SadLocationBridge {
    
    // ═══════════════════════════════════════════════════════════════════════════
    // إعداد
    // ═══════════════════════════════════════════════════════════════════════════
    
    private var appContext: Context? = null
    private var fusedLocationClient: FusedLocationProviderClient? = null
    
    // تخزين callbacks التتبع
    private val trackingCallbacks = mutableMapOf<Int, LocationCallback>()
    private var trackingIdCounter = 0
    
    /**
     * تهيئة الجسر مع السياق
     */
    @JvmStatic
    fun initialize(context: Context) {
        appContext = context.applicationContext
        fusedLocationClient = LocationServices.getFusedLocationProviderClient(context)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // نتيجة الموقع
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * هيكل نتيجة الموقع
     */
    data class LocationResult(
        val success: Boolean,
        val latitude: Double,
        val longitude: Double,
        val accuracy: Float,
        val altitude: Double,
        val speed: Float,
        val bearing: Float,
        val time: Long,
        val error: String? = null
    ) {
        /**
         * تحويل إلى Map للاستخدام في لغة ص
         */
        fun toMap(): Map<String, Any?> = mapOf(
            "نجح" to success,
            "خط_العرض" to latitude,
            "خط_الطول" to longitude,
            "الدقة" to accuracy,
            "الارتفاع" to altitude,
            "السرعة" to speed,
            "الاتجاه" to bearing,
            "الوقت" to time,
            "خطأ" to error
        )
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // التحقق من الأذونات
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * التحقق من وجود إذن الموقع
     */
    @JvmStatic
    fun hasLocationPermission(): Boolean {
        val context = appContext ?: return false
        
        return ActivityCompat.checkSelfPermission(
            context,
            Manifest.permission.ACCESS_FINE_LOCATION
        ) == PackageManager.PERMISSION_GRANTED ||
        ActivityCompat.checkSelfPermission(
            context,
            Manifest.permission.ACCESS_COARSE_LOCATION
        ) == PackageManager.PERMISSION_GRANTED
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_موقع_الحالي — الحصول على الموقع الحالي
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * الحصول على الموقع الحالي (متزامن مع callback)
     * 
     * @param callback دالة الاستجابة
     */
    @JvmStatic
    fun getCurrentLocation(callback: (Map<String, Any?>) -> Unit) {
        val context = appContext
        val client = fusedLocationClient
        
        if (context == null || client == null) {
            callback(LocationResult(
                success = false,
                latitude = 0.0,
                longitude = 0.0,
                accuracy = 0f,
                altitude = 0.0,
                speed = 0f,
                bearing = 0f,
                time = 0,
                error = "لم يتم تهيئة الجسر"
            ).toMap())
            return
        }
        
        if (!hasLocationPermission()) {
            callback(LocationResult(
                success = false,
                latitude = 0.0,
                longitude = 0.0,
                accuracy = 0f,
                altitude = 0.0,
                speed = 0f,
                bearing = 0f,
                time = 0,
                error = "لا يوجد إذن للوصول للموقع"
            ).toMap())
            return
        }
        
        try {
            client.lastLocation.addOnSuccessListener { location: Location? ->
                if (location != null) {
                    callback(LocationResult(
                        success = true,
                        latitude = location.latitude,
                        longitude = location.longitude,
                        accuracy = location.accuracy,
                        altitude = location.altitude,
                        speed = location.speed,
                        bearing = location.bearing,
                        time = location.time
                    ).toMap())
                } else {
                    // إذا لم يكن هناك موقع محفوظ، طلب موقع جديد
                    requestFreshLocation(callback)
                }
            }.addOnFailureListener { e ->
                callback(LocationResult(
                    success = false,
                    latitude = 0.0,
                    longitude = 0.0,
                    accuracy = 0f,
                    altitude = 0.0,
                    speed = 0f,
                    bearing = 0f,
                    time = 0,
                    error = "فشل في الحصول على الموقع: ${e.message}"
                ).toMap())
            }
        } catch (e: SecurityException) {
            callback(LocationResult(
                success = false,
                latitude = 0.0,
                longitude = 0.0,
                accuracy = 0f,
                altitude = 0.0,
                speed = 0f,
                bearing = 0f,
                time = 0,
                error = "خطأ في الأذونات: ${e.message}"
            ).toMap())
        }
    }
    
    /**
     * طلب موقع جديد (عندما لا يكون هناك موقع محفوظ)
     */
    private fun requestFreshLocation(callback: (Map<String, Any?>) -> Unit) {
        val context = appContext
        val client = fusedLocationClient
        
        if (context == null || client == null || !hasLocationPermission()) {
            callback(LocationResult(
                success = false,
                latitude = 0.0,
                longitude = 0.0,
                accuracy = 0f,
                altitude = 0.0,
                speed = 0f,
                bearing = 0f,
                time = 0,
                error = "لا يمكن طلب موقع جديد"
            ).toMap())
            return
        }
        
        val locationRequest = LocationRequest.Builder(Priority.PRIORITY_HIGH_ACCURACY, 1000)
            .setMaxUpdates(1)
            .build()
        
        val locationCallback = object : LocationCallback() {
            override fun onLocationResult(result: LocationResult) {
                val location = result.locations.firstOrNull()
                if (location != null) {
                    callback(SadLocationBridge.LocationResult(
                        success = true,
                        latitude = location.latitude,
                        longitude = location.longitude,
                        accuracy = location.accuracy,
                        altitude = location.altitude,
                        speed = location.speed,
                        bearing = location.bearing,
                        time = location.time
                    ).toMap())
                }
                client.removeLocationUpdates(this)
            }
        }
        
        try {
            client.requestLocationUpdates(
                locationRequest,
                locationCallback,
                Looper.getMainLooper()
            )
        } catch (e: SecurityException) {
            callback(SadLocationBridge.LocationResult(
                success = false,
                latitude = 0.0,
                longitude = 0.0,
                accuracy = 0f,
                altitude = 0.0,
                speed = 0f,
                bearing = 0f,
                time = 0,
                error = "خطأ في الأذونات: ${e.message}"
            ).toMap())
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_موقع_تتبع — بدء تتبع الموقع
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * بدء تتبع الموقع
     * 
     * @param intervalMs فترة التحديث بالملي ثانية
     * @param callback دالة الاستجابة (تُستدعى عند كل تحديث)
     * @return معرف التتبع (لإيقافه لاحقاً)
     */
    @JvmStatic
    fun startTracking(
        intervalMs: Long = 5000,
        callback: (Map<String, Any?>) -> Unit
    ): Int {
        val client = fusedLocationClient
        
        if (client == null || !hasLocationPermission()) {
            return -1
        }
        
        val trackingId = ++trackingIdCounter
        
        val locationRequest = LocationRequest.Builder(Priority.PRIORITY_HIGH_ACCURACY, intervalMs)
            .setMinUpdateIntervalMillis(intervalMs / 2)
            .build()
        
        val locationCallback = object : LocationCallback() {
            override fun onLocationResult(result: LocationResult) {
                result.locations.lastOrNull()?.let { location ->
                    callback(SadLocationBridge.LocationResult(
                        success = true,
                        latitude = location.latitude,
                        longitude = location.longitude,
                        accuracy = location.accuracy,
                        altitude = location.altitude,
                        speed = location.speed,
                        bearing = location.bearing,
                        time = location.time
                    ).toMap())
                }
            }
        }
        
        trackingCallbacks[trackingId] = locationCallback
        
        try {
            client.requestLocationUpdates(
                locationRequest,
                locationCallback,
                Looper.getMainLooper()
            )
            return trackingId
        } catch (e: SecurityException) {
            trackingCallbacks.remove(trackingId)
            return -1
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_موقع_اوقف — إيقاف التتبع
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * إيقاف تتبع الموقع
     * 
     * @param trackingId معرف التتبع
     * @return true إذا نجحت العملية
     */
    @JvmStatic
    fun stopTracking(trackingId: Int): Boolean {
        val callback = trackingCallbacks.remove(trackingId) ?: return false
        fusedLocationClient?.removeLocationUpdates(callback)
        return true
    }
    
    /**
     * إيقاف جميع عمليات التتبع
     */
    @JvmStatic
    fun stopAllTracking() {
        trackingCallbacks.values.forEach { callback ->
            fusedLocationClient?.removeLocationUpdates(callback)
        }
        trackingCallbacks.clear()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال مساعدة
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * حساب المسافة بين نقطتين (بالأمتار)
     */
    @JvmStatic
    fun calculateDistance(
        lat1: Double, lng1: Double,
        lat2: Double, lng2: Double
    ): Float {
        val results = FloatArray(1)
        Location.distanceBetween(lat1, lng1, lat2, lng2, results)
        return results[0]
    }
}
