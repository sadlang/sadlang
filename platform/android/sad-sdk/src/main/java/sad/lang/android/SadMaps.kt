/**
 * =============================================================================
 * @file SadMaps.kt
 * @brief (AR) واجهة Google Maps للغة ص
 * @brief (EN) Google Maps Interface for Sad Language
 * =============================================================================
 */

package sad.lang.android

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.location.Location
import androidx.core.content.ContextCompat
import com.google.android.gms.location.*
import com.google.android.gms.maps.CameraUpdateFactory
import com.google.android.gms.maps.GoogleMap
import com.google.android.gms.maps.model.*
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.suspendCancellableCoroutine
import kotlinx.coroutines.withContext
import kotlin.coroutines.resume
import kotlin.coroutines.resumeWithException

/**
 * نظام الخرائط للغة ص
 */
class SadMaps(private val context: Context) {
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Types
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * نقطة جغرافية
     */
    data class GeoPoint(
        val latitude: Double,
        val longitude: Double
    ) {
        fun toLatLng(): LatLng = LatLng(latitude, longitude)
        
        companion object {
            fun fromLatLng(latLng: LatLng) = GeoPoint(latLng.latitude, latLng.longitude)
        }
    }
    
    /**
     * علامة على الخريطة
     */
    data class MapMarker(
        val id: String,
        val position: GeoPoint,
        val title: String? = null,
        val snippet: String? = null,
        val iconResource: Int? = null,
        val draggable: Boolean = false,
        val visible: Boolean = true,
        val rotation: Float = 0f,
        val alpha: Float = 1f
    )
    
    /**
     * خط على الخريطة
     */
    data class MapPolyline(
        val id: String,
        val points: List<GeoPoint>,
        val color: Int = 0xFF0000FF.toInt(),
        val width: Float = 5f,
        val geodesic: Boolean = false,
        val clickable: Boolean = false
    )
    
    /**
     * مضلع على الخريطة
     */
    data class MapPolygon(
        val id: String,
        val points: List<GeoPoint>,
        val strokeColor: Int = 0xFF000000.toInt(),
        val fillColor: Int = 0x3300FF00.toInt(),
        val strokeWidth: Float = 3f,
        val clickable: Boolean = false
    )
    
    /**
     * دائرة على الخريطة
     */
    data class MapCircle(
        val id: String,
        val center: GeoPoint,
        val radius: Double,
        val strokeColor: Int = 0xFF0000FF.toInt(),
        val fillColor: Int = 0x330000FF.toInt(),
        val strokeWidth: Float = 3f,
        val clickable: Boolean = false
    )
    
    /**
     * نوع الخريطة
     */
    enum class MapType {
        NORMAL,
        SATELLITE,
        TERRAIN,
        HYBRID
    }
    
    /**
     * مستمع أحداث الخريطة
     */
    interface MapEventListener {
        fun onMapClick(point: GeoPoint)
        fun onMapLongClick(point: GeoPoint)
        fun onMarkerClick(markerId: String): Boolean
        fun onMarkerDrag(markerId: String, point: GeoPoint)
        fun onCameraMove(center: GeoPoint, zoom: Float)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Properties
    // ═══════════════════════════════════════════════════════════════════════════
    
    private var googleMap: GoogleMap? = null
    private val markers = mutableMapOf<String, Marker>()
    private val polylines = mutableMapOf<String, Polyline>()
    private val polygons = mutableMapOf<String, Polygon>()
    private val circles = mutableMapOf<String, Circle>()
    
    private val fusedLocationClient: FusedLocationProviderClient by lazy {
        LocationServices.getFusedLocationProviderClient(context)
    }
    
    private var eventListener: MapEventListener? = null
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Setup
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * ربط الخريطة
     */
    fun attachMap(map: GoogleMap, listener: MapEventListener? = null) {
        googleMap = map
        eventListener = listener
        
        setupMapListeners()
    }
    
    /**
     * فك ربط الخريطة
     */
    fun detachMap() {
        googleMap = null
        eventListener = null
        markers.clear()
        polylines.clear()
        polygons.clear()
        circles.clear()
    }
    
    private fun setupMapListeners() {
        googleMap?.apply {
            setOnMapClickListener { latLng ->
                eventListener?.onMapClick(GeoPoint.fromLatLng(latLng))
            }
            
            setOnMapLongClickListener { latLng ->
                eventListener?.onMapLongClick(GeoPoint.fromLatLng(latLng))
            }
            
            setOnMarkerClickListener { marker ->
                val id = markers.entries.find { it.value == marker }?.key ?: return@setOnMarkerClickListener false
                eventListener?.onMarkerClick(id) ?: false
            }
            
            setOnMarkerDragListener(object : GoogleMap.OnMarkerDragListener {
                override fun onMarkerDragStart(marker: Marker) {}
                override fun onMarkerDrag(marker: Marker) {
                    val id = markers.entries.find { it.value == marker }?.key ?: return
                    eventListener?.onMarkerDrag(id, GeoPoint.fromLatLng(marker.position))
                }
                override fun onMarkerDragEnd(marker: Marker) {
                    val id = markers.entries.find { it.value == marker }?.key ?: return
                    eventListener?.onMarkerDrag(id, GeoPoint.fromLatLng(marker.position))
                }
            })
            
            setOnCameraMoveListener {
                val center = cameraPosition.target
                val zoom = cameraPosition.zoom
                eventListener?.onCameraMove(GeoPoint.fromLatLng(center), zoom)
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Map Control
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تعيين نوع الخريطة
     */
    fun setMapType(type: MapType) {
        googleMap?.mapType = when (type) {
            MapType.NORMAL -> GoogleMap.MAP_TYPE_NORMAL
            MapType.SATELLITE -> GoogleMap.MAP_TYPE_SATELLITE
            MapType.TERRAIN -> GoogleMap.MAP_TYPE_TERRAIN
            MapType.HYBRID -> GoogleMap.MAP_TYPE_HYBRID
        }
    }
    
    /**
     * تحريك الكاميرا
     */
    fun moveCamera(point: GeoPoint, zoom: Float? = null, animate: Boolean = true) {
        val update = if (zoom != null) {
            CameraUpdateFactory.newLatLngZoom(point.toLatLng(), zoom)
        } else {
            CameraUpdateFactory.newLatLng(point.toLatLng())
        }
        
        if (animate) {
            googleMap?.animateCamera(update)
        } else {
            googleMap?.moveCamera(update)
        }
    }
    
    /**
     * ضبط حدود الكاميرا
     */
    fun fitBounds(points: List<GeoPoint>, padding: Int = 100, animate: Boolean = true) {
        if (points.isEmpty()) return
        
        val builder = LatLngBounds.Builder()
        points.forEach { builder.include(it.toLatLng()) }
        val bounds = builder.build()
        
        val update = CameraUpdateFactory.newLatLngBounds(bounds, padding)
        
        if (animate) {
            googleMap?.animateCamera(update)
        } else {
            googleMap?.moveCamera(update)
        }
    }
    
    /**
     * تكبير/تصغير
     */
    fun zoom(delta: Float, animate: Boolean = true) {
        val update = if (delta > 0) {
            CameraUpdateFactory.zoomIn()
        } else {
            CameraUpdateFactory.zoomOut()
        }
        
        if (animate) {
            googleMap?.animateCamera(update)
        } else {
            googleMap?.moveCamera(update)
        }
    }
    
    /**
     * تعيين مستوى التكبير
     */
    fun setZoom(level: Float, animate: Boolean = true) {
        val update = CameraUpdateFactory.zoomTo(level)
        if (animate) {
            googleMap?.animateCamera(update)
        } else {
            googleMap?.moveCamera(update)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  UI Controls
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تفعيل/تعطيل عناصر التحكم
     */
    fun setUiSettings(
        zoomControlsEnabled: Boolean = true,
        compassEnabled: Boolean = true,
        myLocationButtonEnabled: Boolean = true,
        rotateGesturesEnabled: Boolean = true,
        scrollGesturesEnabled: Boolean = true,
        tiltGesturesEnabled: Boolean = true,
        zoomGesturesEnabled: Boolean = true
    ) {
        googleMap?.uiSettings?.apply {
            isZoomControlsEnabled = zoomControlsEnabled
            isCompassEnabled = compassEnabled
            isMyLocationButtonEnabled = myLocationButtonEnabled
            isRotateGesturesEnabled = rotateGesturesEnabled
            isScrollGesturesEnabled = scrollGesturesEnabled
            isTiltGesturesEnabled = tiltGesturesEnabled
            isZoomGesturesEnabled = zoomGesturesEnabled
        }
    }
    
    /**
     * تفعيل طبقة الموقع الحالي
     */
    fun enableMyLocation(enable: Boolean = true) {
        if (ContextCompat.checkSelfPermission(context, Manifest.permission.ACCESS_FINE_LOCATION)
            == PackageManager.PERMISSION_GRANTED) {
            googleMap?.isMyLocationEnabled = enable
        }
    }
    
    /**
     * تفعيل طبقة المرور
     */
    fun enableTrafficLayer(enable: Boolean = true) {
        googleMap?.isTrafficEnabled = enable
    }
    
    /**
     * تفعيل طبقة المباني
     */
    fun enableBuildingsLayer(enable: Boolean = true) {
        googleMap?.isBuildingsEnabled = enable
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Markers
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * إضافة علامة
     */
    fun addMarker(marker: MapMarker): String {
        val options = MarkerOptions()
            .position(marker.position.toLatLng())
            .title(marker.title)
            .snippet(marker.snippet)
            .draggable(marker.draggable)
            .visible(marker.visible)
            .rotation(marker.rotation)
            .alpha(marker.alpha)
        
        marker.iconResource?.let { resourceId ->
            try {
                val bitmap = BitmapFactory.decodeResource(context.resources, resourceId)
                options.icon(BitmapDescriptorFactory.fromBitmap(bitmap))
            } catch (e: Exception) {
                // Use default icon
            }
        }
        
        val googleMarker = googleMap?.addMarker(options)
        if (googleMarker != null) {
            markers[marker.id] = googleMarker
        }
        
        return marker.id
    }
    
    /**
     * إزالة علامة
     */
    fun removeMarker(id: String) {
        markers[id]?.remove()
        markers.remove(id)
    }
    
    /**
     * تحديث موقع علامة
     */
    fun updateMarkerPosition(id: String, position: GeoPoint) {
        markers[id]?.position = position.toLatLng()
    }
    
    /**
     * إزالة جميع العلامات
     */
    fun clearMarkers() {
        markers.values.forEach { it.remove() }
        markers.clear()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Polylines
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * إضافة خط
     */
    fun addPolyline(polyline: MapPolyline): String {
        val options = PolylineOptions()
            .addAll(polyline.points.map { it.toLatLng() })
            .color(polyline.color)
            .width(polyline.width)
            .geodesic(polyline.geodesic)
            .clickable(polyline.clickable)
        
        val googlePolyline = googleMap?.addPolyline(options)
        if (googlePolyline != null) {
            polylines[polyline.id] = googlePolyline
        }
        
        return polyline.id
    }
    
    /**
     * إزالة خط
     */
    fun removePolyline(id: String) {
        polylines[id]?.remove()
        polylines.remove(id)
    }
    
    /**
     * إزالة جميع الخطوط
     */
    fun clearPolylines() {
        polylines.values.forEach { it.remove() }
        polylines.clear()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Polygons
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * إضافة مضلع
     */
    fun addPolygon(polygon: MapPolygon): String {
        val options = PolygonOptions()
            .addAll(polygon.points.map { it.toLatLng() })
            .strokeColor(polygon.strokeColor)
            .fillColor(polygon.fillColor)
            .strokeWidth(polygon.strokeWidth)
            .clickable(polygon.clickable)
        
        val googlePolygon = googleMap?.addPolygon(options)
        if (googlePolygon != null) {
            polygons[polygon.id] = googlePolygon
        }
        
        return polygon.id
    }
    
    /**
     * إزالة مضلع
     */
    fun removePolygon(id: String) {
        polygons[id]?.remove()
        polygons.remove(id)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Circles
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * إضافة دائرة
     */
    fun addCircle(circle: MapCircle): String {
        val options = CircleOptions()
            .center(circle.center.toLatLng())
            .radius(circle.radius)
            .strokeColor(circle.strokeColor)
            .fillColor(circle.fillColor)
            .strokeWidth(circle.strokeWidth)
            .clickable(circle.clickable)
        
        val googleCircle = googleMap?.addCircle(options)
        if (googleCircle != null) {
            circles[circle.id] = googleCircle
        }
        
        return circle.id
    }
    
    /**
     * إزالة دائرة
     */
    fun removeCircle(id: String) {
        circles[id]?.remove()
        circles.remove(id)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Location
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * الحصول على الموقع الحالي
     */
    suspend fun getCurrentLocation(): GeoPoint? = withContext(Dispatchers.IO) {
        if (ContextCompat.checkSelfPermission(context, Manifest.permission.ACCESS_FINE_LOCATION)
            != PackageManager.PERMISSION_GRANTED) {
            return@withContext null
        }
        
        suspendCancellableCoroutine { continuation ->
            fusedLocationClient.lastLocation
                .addOnSuccessListener { location: Location? ->
                    if (location != null) {
                        continuation.resume(GeoPoint(location.latitude, location.longitude))
                    } else {
                        continuation.resume(null)
                    }
                }
                .addOnFailureListener { exception ->
                    continuation.resumeWithException(exception)
                }
        }
    }
    
    /**
     * التركيز على الموقع الحالي
     */
    suspend fun focusOnCurrentLocation(zoom: Float = 15f) {
        getCurrentLocation()?.let { point ->
            moveCamera(point, zoom, animate = true)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Utilities
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * حساب المسافة بين نقطتين (بالمتر)
     */
    fun calculateDistance(from: GeoPoint, to: GeoPoint): Float {
        val results = FloatArray(1)
        Location.distanceBetween(
            from.latitude, from.longitude,
            to.latitude, to.longitude,
            results
        )
        return results[0]
    }
    
    /**
     * مسح جميع العناصر
     */
    fun clearAll() {
        clearMarkers()
        clearPolylines()
        polygons.values.forEach { it.remove() }
        polygons.clear()
        circles.values.forEach { it.remove() }
        circles.clear()
    }
    
    /**
     * التقاط صورة للخريطة
     */
    fun captureSnapshot(callback: (Bitmap?) -> Unit) {
        googleMap?.snapshot { bitmap ->
            callback(bitmap)
        }
    }
}
