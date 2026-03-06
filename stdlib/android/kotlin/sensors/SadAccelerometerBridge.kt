// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadAccelerometerBridge.kt
// الوصف: جسر Kotlin لمستشعر التسارع
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager

/**
 * جسر مستشعر التسارع — SadAccelerometerBridge
 */
object SadAccelerometerBridge : SensorEventListener {
    
    private var context: Context? = null
    private var sensorManager: SensorManager? = null
    private var accelerometer: Sensor? = null
    private var isListening = false
    
    private var lastX = 0f
    private var lastY = 0f
    private var lastZ = 0f
    private var lastTimestamp = 0L
    
    private var onAccelerationChanged: ((Float, Float, Float) -> Unit)? = null
    private var onShake: (() -> Unit)? = null
    
    private var shakeThreshold = 12f
    private var shakeTimestamp = 0L
    private var shakeCount = 0
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        sensorManager = ctx.getSystemService(Context.SENSOR_SERVICE) as? SensorManager
        accelerometer = sensorManager?.getDefaultSensor(Sensor.TYPE_ACCELEROMETER)
    }
    
    @JvmStatic
    fun isAvailable(): Boolean = accelerometer != null
    
    @JvmStatic
    fun start(samplingPeriodUs: Int = SensorManager.SENSOR_DELAY_NORMAL): Boolean {
        val manager = sensorManager ?: return false
        val sensor = accelerometer ?: return false
        
        if (isListening) return true
        
        isListening = manager.registerListener(this, sensor, samplingPeriodUs)
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
    fun getValues(): Map<String, Float> = mapOf(
        "x" to lastX,
        "y" to lastY,
        "z" to lastZ
    )
    
    @JvmStatic
    fun getX(): Float = lastX
    
    @JvmStatic
    fun getY(): Float = lastY
    
    @JvmStatic
    fun getZ(): Float = lastZ
    
    @JvmStatic
    fun setShakeThreshold(threshold: Float) {
        shakeThreshold = threshold
    }
    
    @JvmStatic
    fun setOnAccelerationChanged(callback: (Float, Float, Float) -> Unit) {
        onAccelerationChanged = callback
    }
    
    @JvmStatic
    fun setOnShake(callback: () -> Unit) {
        onShake = callback
    }
    
    override fun onSensorChanged(event: SensorEvent) {
        if (event.sensor.type != Sensor.TYPE_ACCELEROMETER) return
        
        val x = event.values[0]
        val y = event.values[1]
        val z = event.values[2]
        
        lastX = x
        lastY = y
        lastZ = z
        lastTimestamp = event.timestamp
        
        onAccelerationChanged?.invoke(x, y, z)
        
        // اكتشاف الاهتزاز
        detectShake(x, y, z)
    }
    
    private fun detectShake(x: Float, y: Float, z: Float) {
        val gX = x / SensorManager.GRAVITY_EARTH
        val gY = y / SensorManager.GRAVITY_EARTH
        val gZ = z / SensorManager.GRAVITY_EARTH
        
        val gForce = kotlin.math.sqrt(gX * gX + gY * gY + gZ * gZ)
        
        if (gForce > shakeThreshold / SensorManager.GRAVITY_EARTH) {
            val now = System.currentTimeMillis()
            
            if (shakeTimestamp + 500 > now) {
                shakeCount++
            } else {
                shakeCount = 1
            }
            
            shakeTimestamp = now
            
            if (shakeCount >= 2) {
                onShake?.invoke()
                shakeCount = 0
            }
        }
    }
    
    override fun onAccuracyChanged(sensor: Sensor, accuracy: Int) {
        // لا حاجة للتعامل مع هذا
    }
}
