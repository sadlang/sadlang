// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadProximityBridge.kt
// الوصف: جسر Kotlin لمستشعر القرب
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager

/**
 * جسر مستشعر القرب — SadProximityBridge
 */
object SadProximityBridge : SensorEventListener {
    
    private var context: Context? = null
    private var sensorManager: SensorManager? = null
    private var proximitySensor: Sensor? = null
    private var isListening = false
    
    private var lastDistance = 0f
    private var maxRange = 0f
    private var isNear = false
    
    private var onProximityChanged: ((Float, Boolean) -> Unit)? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        sensorManager = ctx.getSystemService(Context.SENSOR_SERVICE) as? SensorManager
        proximitySensor = sensorManager?.getDefaultSensor(Sensor.TYPE_PROXIMITY)
        maxRange = proximitySensor?.maximumRange ?: 5f
    }
    
    @JvmStatic
    fun isAvailable(): Boolean = proximitySensor != null
    
    @JvmStatic
    fun start(): Boolean {
        val manager = sensorManager ?: return false
        val sensor = proximitySensor ?: return false
        
        if (isListening) return true
        
        isListening = manager.registerListener(this, sensor, SensorManager.SENSOR_DELAY_NORMAL)
        return isListening
    }
    
    @JvmStatic
    fun stop() {
        sensorManager?.unregisterListener(this)
        isListening = false
    }
    
    @JvmStatic
    fun isListening(): Boolean = isListening
    
    @JvmStatic
    fun getDistance(): Float = lastDistance
    
    @JvmStatic
    fun isNear(): Boolean = isNear
    
    @JvmStatic
    fun getMaxRange(): Float = maxRange
    
    @JvmStatic
    fun setOnProximityChanged(callback: (Float, Boolean) -> Unit) {
        onProximityChanged = callback
    }
    
    override fun onSensorChanged(event: SensorEvent) {
        if (event.sensor.type != Sensor.TYPE_PROXIMITY) return
        
        lastDistance = event.values[0]
        isNear = lastDistance < maxRange
        
        onProximityChanged?.invoke(lastDistance, isNear)
    }
    
    override fun onAccuracyChanged(sensor: Sensor, accuracy: Int) {
        // لا حاجة للتعامل مع هذا
    }
}
