// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadMapsBridge.kt
// الوصف: جسر Kotlin لخرائط Google Maps
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.content.Intent
import android.graphics.Bitmap
import android.graphics.Canvas
import android.net.Uri
import androidx.core.content.ContextCompat
import com.google.android.gms.maps.CameraUpdateFactory
import com.google.android.gms.maps.GoogleMap
import com.google.android.gms.maps.model.*
import kotlin.math.*

/**
 * جسر Google Maps — SadMapsBridge
 */
object SadMapsBridge {
    
    private var context: Context? = null
    private var googleMap: GoogleMap? = null
    private val markers = mutableMapOf<String, Marker>()
    private val polylines = mutableMapOf<String, Polyline>()
    private val polygons = mutableMapOf<String, Polygon>()
    private val circles = mutableMapOf<String, Circle>()
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    @JvmStatic
    fun setMap(map: GoogleMap) {
        googleMap = map
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Camera Controls
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun moveTo(latitude: Double, longitude: Double, zoom: Float = 15f) {
        googleMap?.moveCamera(
            CameraUpdateFactory.newLatLngZoom(LatLng(latitude, longitude), zoom)
        )
    }
    
    @JvmStatic
    fun animateTo(latitude: Double, longitude: Double, zoom: Float = 15f, durationMs: Int = 1000) {
        googleMap?.animateCamera(
            CameraUpdateFactory.newLatLngZoom(LatLng(latitude, longitude), zoom),
            durationMs,
            null
        )
    }
    
    @JvmStatic
    fun zoomIn() {
        googleMap?.animateCamera(CameraUpdateFactory.zoomIn())
    }
    
    @JvmStatic
    fun zoomOut() {
        googleMap?.animateCamera(CameraUpdateFactory.zoomOut())
    }
    
    @JvmStatic
    fun setZoom(zoom: Float) {
        googleMap?.animateCamera(CameraUpdateFactory.zoomTo(zoom))
    }
    
    @JvmStatic
    fun getZoom(): Float = googleMap?.cameraPosition?.zoom ?: 0f
    
    @JvmStatic
    fun getCenter(): Pair<Double, Double> {
        val position = googleMap?.cameraPosition?.target
        return Pair(position?.latitude ?: 0.0, position?.longitude ?: 0.0)
    }
    
    @JvmStatic
    fun setBearing(bearing: Float) {
        val current = googleMap?.cameraPosition ?: return
        val cameraPosition = CameraPosition.Builder(current)
            .bearing(bearing)
            .build()
        googleMap?.animateCamera(CameraUpdateFactory.newCameraPosition(cameraPosition))
    }
    
    @JvmStatic
    fun setTilt(tilt: Float) {
        val current = googleMap?.cameraPosition ?: return
        val cameraPosition = CameraPosition.Builder(current)
            .tilt(tilt.coerceIn(0f, 90f))
            .build()
        googleMap?.animateCamera(CameraUpdateFactory.newCameraPosition(cameraPosition))
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Markers
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun addMarker(
        id: String,
        latitude: Double,
        longitude: Double,
        title: String? = null,
        snippet: String? = null
    ): Boolean {
        val map = googleMap ?: return false
        
        val markerOptions = MarkerOptions()
            .position(LatLng(latitude, longitude))
            .title(title)
            .snippet(snippet)
        
        val marker = map.addMarker(markerOptions)
        if (marker != null) {
            markers[id] = marker
            return true
        }
        return false
    }
    
    @JvmStatic
    fun addMarkerWithColor(
        id: String,
        latitude: Double,
        longitude: Double,
        title: String?,
        color: Float // BitmapDescriptorFactory.HUE_*
    ): Boolean {
        val map = googleMap ?: return false
        
        val markerOptions = MarkerOptions()
            .position(LatLng(latitude, longitude))
            .title(title)
            .icon(BitmapDescriptorFactory.defaultMarker(color))
        
        val marker = map.addMarker(markerOptions)
        if (marker != null) {
            markers[id] = marker
            return true
        }
        return false
    }
    
    @JvmStatic
    fun addMarkerWithIcon(
        id: String,
        latitude: Double,
        longitude: Double,
        title: String?,
        iconResId: Int
    ): Boolean {
        val map = googleMap ?: return false
        val ctx = context ?: return false
        
        val drawable = ContextCompat.getDrawable(ctx, iconResId) ?: return false
        val bitmap = Bitmap.createBitmap(drawable.intrinsicWidth, drawable.intrinsicHeight, Bitmap.Config.ARGB_8888)
        val canvas = Canvas(bitmap)
        drawable.setBounds(0, 0, canvas.width, canvas.height)
        drawable.draw(canvas)
        
        val markerOptions = MarkerOptions()
            .position(LatLng(latitude, longitude))
            .title(title)
            .icon(BitmapDescriptorFactory.fromBitmap(bitmap))
        
        val marker = map.addMarker(markerOptions)
        if (marker != null) {
            markers[id] = marker
            return true
        }
        return false
    }
    
    @JvmStatic
    fun removeMarker(id: String): Boolean {
        val marker = markers.remove(id)
        marker?.remove()
        return marker != null
    }
    
    @JvmStatic
    fun updateMarkerPosition(id: String, latitude: Double, longitude: Double): Boolean {
        val marker = markers[id] ?: return false
        marker.position = LatLng(latitude, longitude)
        return true
    }
    
    @JvmStatic
    fun showMarkerInfo(id: String): Boolean {
        val marker = markers[id] ?: return false
        marker.showInfoWindow()
        return true
    }
    
    @JvmStatic
    fun hideMarkerInfo(id: String): Boolean {
        val marker = markers[id] ?: return false
        marker.hideInfoWindow()
        return true
    }
    
    @JvmStatic
    fun clearMarkers() {
        markers.values.forEach { it.remove() }
        markers.clear()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Polylines
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun addPolyline(
        id: String,
        points: List<Pair<Double, Double>>,
        color: Int = 0xFF0000FF.toInt(),
        width: Float = 5f
    ): Boolean {
        val map = googleMap ?: return false
        
        val polylineOptions = PolylineOptions()
            .addAll(points.map { LatLng(it.first, it.second) })
            .color(color)
            .width(width)
        
        val polyline = map.addPolyline(polylineOptions)
        polylines[id] = polyline
        return true
    }
    
    @JvmStatic
    fun removePolyline(id: String): Boolean {
        val polyline = polylines.remove(id)
        polyline?.remove()
        return polyline != null
    }
    
    @JvmStatic
    fun clearPolylines() {
        polylines.values.forEach { it.remove() }
        polylines.clear()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Polygons
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun addPolygon(
        id: String,
        points: List<Pair<Double, Double>>,
        strokeColor: Int = 0xFF0000FF.toInt(),
        fillColor: Int = 0x550000FF,
        strokeWidth: Float = 3f
    ): Boolean {
        val map = googleMap ?: return false
        
        val polygonOptions = PolygonOptions()
            .addAll(points.map { LatLng(it.first, it.second) })
            .strokeColor(strokeColor)
            .fillColor(fillColor)
            .strokeWidth(strokeWidth)
        
        val polygon = map.addPolygon(polygonOptions)
        polygons[id] = polygon
        return true
    }
    
    @JvmStatic
    fun removePolygon(id: String): Boolean {
        val polygon = polygons.remove(id)
        polygon?.remove()
        return polygon != null
    }
    
    @JvmStatic
    fun clearPolygons() {
        polygons.values.forEach { it.remove() }
        polygons.clear()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Circles
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun addCircle(
        id: String,
        latitude: Double,
        longitude: Double,
        radius: Double, // بالمتر
        strokeColor: Int = 0xFF0000FF.toInt(),
        fillColor: Int = 0x550000FF,
        strokeWidth: Float = 3f
    ): Boolean {
        val map = googleMap ?: return false
        
        val circleOptions = CircleOptions()
            .center(LatLng(latitude, longitude))
            .radius(radius)
            .strokeColor(strokeColor)
            .fillColor(fillColor)
            .strokeWidth(strokeWidth)
        
        val circle = map.addCircle(circleOptions)
        circles[id] = circle
        return true
    }
    
    @JvmStatic
    fun removeCircle(id: String): Boolean {
        val circle = circles.remove(id)
        circle?.remove()
        return circle != null
    }
    
    @JvmStatic
    fun clearCircles() {
        circles.values.forEach { it.remove() }
        circles.clear()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Map Settings
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setMapType(type: String) {
        googleMap?.mapType = when (type.lowercase()) {
            "satellite" -> GoogleMap.MAP_TYPE_SATELLITE
            "terrain" -> GoogleMap.MAP_TYPE_TERRAIN
            "hybrid" -> GoogleMap.MAP_TYPE_HYBRID
            "none" -> GoogleMap.MAP_TYPE_NONE
            else -> GoogleMap.MAP_TYPE_NORMAL
        }
    }
    
    @JvmStatic
    fun setTrafficEnabled(enabled: Boolean) {
        googleMap?.isTrafficEnabled = enabled
    }
    
    @JvmStatic
    fun setBuildingsEnabled(enabled: Boolean) {
        googleMap?.isBuildingsEnabled = enabled
    }
    
    @JvmStatic
    fun setIndoorEnabled(enabled: Boolean) {
        googleMap?.isIndoorEnabled = enabled
    }
    
    @JvmStatic
    fun setCompassEnabled(enabled: Boolean) {
        googleMap?.uiSettings?.isCompassEnabled = enabled
    }
    
    @JvmStatic
    fun setZoomControlsEnabled(enabled: Boolean) {
        googleMap?.uiSettings?.isZoomControlsEnabled = enabled
    }
    
    @JvmStatic
    fun setRotateGesturesEnabled(enabled: Boolean) {
        googleMap?.uiSettings?.isRotateGesturesEnabled = enabled
    }
    
    @JvmStatic
    fun setScrollGesturesEnabled(enabled: Boolean) {
        googleMap?.uiSettings?.isScrollGesturesEnabled = enabled
    }
    
    @JvmStatic
    fun setTiltGesturesEnabled(enabled: Boolean) {
        googleMap?.uiSettings?.isTiltGesturesEnabled = enabled
    }
    
    @JvmStatic
    fun setZoomGesturesEnabled(enabled: Boolean) {
        googleMap?.uiSettings?.isZoomGesturesEnabled = enabled
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Utilities
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun calculateDistance(
        lat1: Double, lon1: Double,
        lat2: Double, lon2: Double
    ): Double {
        // صيغة Haversine
        val R = 6371000.0 // نصف قطر الأرض بالمتر
        val dLat = Math.toRadians(lat2 - lat1)
        val dLon = Math.toRadians(lon2 - lon1)
        val a = sin(dLat / 2).pow(2) +
                cos(Math.toRadians(lat1)) * cos(Math.toRadians(lat2)) *
                sin(dLon / 2).pow(2)
        val c = 2 * atan2(sqrt(a), sqrt(1 - a))
        return R * c
    }
    
    @JvmStatic
    fun formatDistance(meters: Double): String {
        return when {
            meters < 1000 -> String.format("%.0f متر", meters)
            else -> String.format("%.2f كم", meters / 1000)
        }
    }
    
    @JvmStatic
    fun fitBounds(points: List<Pair<Double, Double>>, padding: Int = 50) {
        if (points.isEmpty()) return
        
        val builder = LatLngBounds.Builder()
        points.forEach { builder.include(LatLng(it.first, it.second)) }
        val bounds = builder.build()
        
        googleMap?.animateCamera(CameraUpdateFactory.newLatLngBounds(bounds, padding))
    }
    
    @JvmStatic
    fun openInGoogleMaps(latitude: Double, longitude: Double, label: String = "") {
        val ctx = context ?: return
        
        val uri = if (label.isNotEmpty()) {
            Uri.parse("geo:$latitude,$longitude?q=$latitude,$longitude($label)")
        } else {
            Uri.parse("geo:$latitude,$longitude?q=$latitude,$longitude")
        }
        
        val intent = Intent(Intent.ACTION_VIEW, uri).apply {
            setPackage("com.google.android.apps.maps")
            addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
        }
        
        if (intent.resolveActivity(ctx.packageManager) != null) {
            ctx.startActivity(intent)
        } else {
            // فتح في المتصفح
            val webIntent = Intent(Intent.ACTION_VIEW, 
                Uri.parse("https://maps.google.com/?q=$latitude,$longitude"))
            webIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            ctx.startActivity(webIntent)
        }
    }
    
    @JvmStatic
    fun getDirectionsUrl(
        fromLat: Double, fromLon: Double,
        toLat: Double, toLon: Double,
        mode: String = "driving" // driving, walking, bicycling, transit
    ): String {
        return "https://www.google.com/maps/dir/?api=1" +
               "&origin=$fromLat,$fromLon" +
               "&destination=$toLat,$toLon" +
               "&travelmode=$mode"
    }
    
    @JvmStatic
    fun openDirections(
        fromLat: Double, fromLon: Double,
        toLat: Double, toLon: Double,
        mode: String = "driving"
    ) {
        val ctx = context ?: return
        val url = getDirectionsUrl(fromLat, fromLon, toLat, toLon, mode)
        val intent = Intent(Intent.ACTION_VIEW, Uri.parse(url))
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
        ctx.startActivity(intent)
    }
    
    @JvmStatic
    fun clearAll() {
        clearMarkers()
        clearPolylines()
        clearPolygons()
        clearCircles()
    }
}
