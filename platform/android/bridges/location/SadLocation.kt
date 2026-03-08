/**
 * =============================================================================
 * @file SadLocation.kt
 * @brief (AR) جسر الموقع الجغرافي — GPS + خريطة
 * @brief (EN) Location Bridge — GPS + Map
 * 
 * المهمة: T246 - GPS + خريطة
 * المرحلة: سبتمبر 2026 — Android Sensors
 * =============================================================================
 * 
 * 🎯 الميزات
 * ══════════
 * ✓ الحصول على الموقع الحالي
 * ✓ تتبع الموقع المستمر
 * ✓ Geocoding (عنوان ↔ إحداثيات)
 * ✓ حساب المسافة
 * ✓ Google Maps integration
 * 
 * =============================================================================
 */

package sad.platform.android.location

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.location.*
import android.os.Bundle
import android.os.Looper
import androidx.core.content.ContextCompat
import com.google.android.gms.location.*
import com.google.android.gms.maps.CameraUpdateFactory
import com.google.android.gms.maps.GoogleMap
import com.google.android.gms.maps.model.*
import java.util.*

// =============================================================================
// الأنواع
// =============================================================================

/**
 * موقع جغرافي
 */
data class GeoLocation(
    val latitude: Double,           // خط العرض
    val longitude: Double,          // خط الطول
    val altitude: Double = 0.0,     // الارتفاع (متر)
    val accuracy: Float = 0f,       // الدقة (متر)
    val speed: Float = 0f,          // السرعة (م/ث)
    val bearing: Float = 0f,        // الاتجاه (درجة)
    val timestamp: Long = System.currentTimeMillis()
)

/**
 * عنوان
 */
data class GeoAddress(
    val country: String = "",       // البلد
    val city: String = "",          // المدينة
    val street: String = "",        // الشارع
    val postalCode: String = "",    // الرمز البريدي
    val fullAddress: String = ""    // العنوان الكامل
)

/**
 * مستمع الموقع
 */
interface SadLocationListener {
    fun onLocationChanged(location: GeoLocation)
    fun onError(error: String)
}

// =============================================================================
// SadLocation
// =============================================================================

class SadLocation(private val context: Context) {
    
    // ─────────────────────────────────────────────
    // الخصائص
    // ─────────────────────────────────────────────
    
    private var fusedLocationClient: FusedLocationProviderClient =
        LocationServices.getFusedLocationProviderClient(context)
    
    private var geocoder: Geocoder = Geocoder(context, Locale("ar"))
    
    private var locationCallback: LocationCallback? = null
    private var isTracking = false
    
    var listener: SadLocationListener? = null
    
    // ─────────────────────────────────────────────
    // الموقع الحالي
    // ─────────────────────────────────────────────
    
    /**
     * الحصول على الموقع الحالي (مرة واحدة)
     */
    fun getCurrentLocation(callback: (GeoLocation?) -> Unit) {
        if (!hasPermission()) {
            listener?.onError("إذن الموقع غير متوفر")
            callback(null)
            return
        }
        
        try {
            fusedLocationClient.lastLocation.addOnSuccessListener { location ->
                if (location != null) {
                    callback(location.toGeoLocation())
                } else {
                    // لا يوجد موقع مخزن، نطلب موقعاً جديداً
                    requestSingleLocation(callback)
                }
            }.addOnFailureListener { e ->
                listener?.onError("فشل في الحصول على الموقع: ${e.message}")
                callback(null)
            }
        } catch (e: SecurityException) {
            listener?.onError("إذن الموقع مرفوض")
            callback(null)
        }
    }
    
    private fun requestSingleLocation(callback: (GeoLocation?) -> Unit) {
        val request = LocationRequest.Builder(Priority.PRIORITY_HIGH_ACCURACY, 1000)
            .setMaxUpdates(1)
            .build()
        
        val singleCallback = object : LocationCallback() {
            override fun onLocationResult(result: LocationResult) {
                result.lastLocation?.let { callback(it.toGeoLocation()) }
                fusedLocationClient.removeLocationUpdates(this)
            }
        }
        
        try {
            fusedLocationClient.requestLocationUpdates(
                request,
                singleCallback,
                Looper.getMainLooper()
            )
        } catch (e: SecurityException) {
            callback(null)
        }
    }
    
    // ─────────────────────────────────────────────
    // التتبع المستمر
    // ─────────────────────────────────────────────
    
    /**
     * بدء تتبع الموقع
     * @param intervalMs الفاصل الزمني بالمللي ثانية
     * @param minDistanceM أقل مسافة للتحديث (متر)
     */
    fun startTracking(intervalMs: Long = 5000, minDistanceM: Float = 10f) {
        if (isTracking) return
        
        if (!hasPermission()) {
            listener?.onError("إذن الموقع غير متوفر")
            return
        }
        
        val request = LocationRequest.Builder(Priority.PRIORITY_HIGH_ACCURACY, intervalMs)
            .setMinUpdateDistanceMeters(minDistanceM)
            .build()
        
        locationCallback = object : LocationCallback() {
            override fun onLocationResult(result: LocationResult) {
                result.lastLocation?.let {
                    listener?.onLocationChanged(it.toGeoLocation())
                }
            }
        }
        
        try {
            fusedLocationClient.requestLocationUpdates(
                request,
                locationCallback!!,
                Looper.getMainLooper()
            )
            isTracking = true
        } catch (e: SecurityException) {
            listener?.onError("إذن الموقع مرفوض")
        }
    }
    
    /**
     * إيقاف التتبع
     */
    fun stopTracking() {
        locationCallback?.let {
            fusedLocationClient.removeLocationUpdates(it)
        }
        locationCallback = null
        isTracking = false
    }
    
    // ─────────────────────────────────────────────
    // Geocoding
    // ─────────────────────────────────────────────
    
    /**
     * تحويل إحداثيات إلى عنوان
     */
    fun getAddress(lat: Double, lon: Double): GeoAddress? {
        return try {
            val addresses = geocoder.getFromLocation(lat, lon, 1)
            if (addresses.isNullOrEmpty()) null
            else addresses[0].toGeoAddress()
        } catch (e: Exception) {
            listener?.onError("فشل في الحصول على العنوان: ${e.message}")
            null
        }
    }
    
    /**
     * تحويل عنوان إلى إحداثيات
     */
    fun getCoordinates(address: String): GeoLocation? {
        return try {
            val addresses = geocoder.getFromLocationName(address, 1)
            if (addresses.isNullOrEmpty()) null
            else GeoLocation(
                latitude = addresses[0].latitude,
                longitude = addresses[0].longitude
            )
        } catch (e: Exception) {
            listener?.onError("فشل في تحويل العنوان: ${e.message}")
            null
        }
    }
    
    // ─────────────────────────────────────────────
    // حساب المسافة
    // ─────────────────────────────────────────────
    
    /**
     * حساب المسافة بين نقطتين (متر)
     */
    fun distanceBetween(
        lat1: Double, lon1: Double,
        lat2: Double, lon2: Double
    ): Float {
        val results = FloatArray(1)
        Location.distanceBetween(lat1, lon1, lat2, lon2, results)
        return results[0]
    }
    
    /**
     * حساب الاتجاه بين نقطتين (درجة)
     */
    fun bearingBetween(
        lat1: Double, lon1: Double,
        lat2: Double, lon2: Double
    ): Float {
        val loc1 = Location("").apply { latitude = lat1; longitude = lon1 }
        val loc2 = Location("").apply { latitude = lat2; longitude = lon2 }
        return loc1.bearingTo(loc2)
    }
    
    // ─────────────────────────────────────────────
    // التكامل مع الخريطة
    // ─────────────────────────────────────────────
    
    /**
     * إضافة علامة على الخريطة
     */
    fun addMarker(map: GoogleMap, location: GeoLocation, title: String = ""): Marker? {
        return map.addMarker(
            MarkerOptions()
                .position(LatLng(location.latitude, location.longitude))
                .title(title)
        )
    }
    
    /**
     * تحريك الكاميرا إلى موقع
     */
    fun moveTo(map: GoogleMap, location: GeoLocation, zoom: Float = 15f) {
        map.animateCamera(
            CameraUpdateFactory.newLatLngZoom(
                LatLng(location.latitude, location.longitude),
                zoom
            )
        )
    }
    
    /**
     * رسم مسار على الخريطة
     */
    fun drawPath(map: GoogleMap, points: List<GeoLocation>, color: Int = 0xFF0000FF.toInt()): Polyline {
        val latLngs = points.map { LatLng(it.latitude, it.longitude) }
        return map.addPolyline(
            PolylineOptions()
                .addAll(latLngs)
                .color(color)
                .width(5f)
        )
    }
    
    /**
     * رسم دائرة (منطقة)
     */
    fun drawCircle(
        map: GoogleMap,
        center: GeoLocation,
        radiusMeters: Double,
        strokeColor: Int = 0xFF0000FF.toInt(),
        fillColor: Int = 0x220000FF
    ): Circle {
        return map.addCircle(
            CircleOptions()
                .center(LatLng(center.latitude, center.longitude))
                .radius(radiusMeters)
                .strokeColor(strokeColor)
                .fillColor(fillColor)
        )
    }
    
    // ─────────────────────────────────────────────
    // Private
    // ─────────────────────────────────────────────
    
    private fun hasPermission(): Boolean {
        return ContextCompat.checkSelfPermission(
            context,
            Manifest.permission.ACCESS_FINE_LOCATION
        ) == PackageManager.PERMISSION_GRANTED
    }
    
    private fun Location.toGeoLocation() = GeoLocation(
        latitude = latitude,
        longitude = longitude,
        altitude = altitude,
        accuracy = accuracy,
        speed = speed,
        bearing = bearing,
        timestamp = time
    )
    
    private fun Address.toGeoAddress() = GeoAddress(
        country = countryName ?: "",
        city = locality ?: adminArea ?: "",
        street = thoroughfare ?: "",
        postalCode = postalCode ?: "",
        fullAddress = getAddressLine(0) ?: ""
    )
}

// =============================================================================
// دوال مساعدة للغة ص (JNI)
// =============================================================================

/**
 * إنشاء مدير الموقع
 */
@JvmStatic
fun موقع_انشئ(context: Context): SadLocation = SadLocation(context)

/**
 * الحصول على الموقع الحالي
 */
@JvmStatic
fun موقع_احصل(location: SadLocation, callback: (GeoLocation?) -> Unit) {
    location.getCurrentLocation(callback)
}

/**
 * بدء التتبع
 */
@JvmStatic
fun موقع_تتبع(location: SadLocation, intervalMs: Long = 5000) {
    location.startTracking(intervalMs)
}

/**
 * إيقاف التتبع
 */
@JvmStatic
fun موقع_أوقف(location: SadLocation) {
    location.stopTracking()
}

/**
 * تحويل إحداثيات إلى عنوان
 */
@JvmStatic
fun موقع_عنوان(location: SadLocation, lat: Double, lon: Double): String {
    return location.getAddress(lat, lon)?.fullAddress ?: ""
}

/**
 * حساب المسافة
 */
@JvmStatic
fun موقع_مسافة(
    lat1: Double, lon1: Double,
    lat2: Double, lon2: Double
): Float {
    val results = FloatArray(1)
    Location.distanceBetween(lat1, lon1, lat2, lon2, results)
    return results[0]
}
