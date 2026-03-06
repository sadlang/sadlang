// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadGyroscopeBridge.kt
// الوصف: جسر Kotlin لمستشعر الجيروسكوب
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager

/**
 * جسر مستشعر الجيروسكوب — SadGyroscopeBridge
 */
object SadGyroscopeBridge : SensorEventListener {
    
    private var context: Context? = null
    private var sensorManager: SensorManager? = null
    private var gyroscope: Sensor? = null
    private var isListening = false
    
    private var lastX = 0f  // معدل الدوران حول محور X (rad/s)
    private var lastY = 0f  // معدل الدوران حول محور Y (rad/s)
    private var lastZ = 0f  // معدل الدوران حول محور Z (rad/s)
    private var lastTimestamp = 0L
    
    private var onRotationChanged: ((Float, Float, Float) -> Unit)? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        sensorManager = ctx.getSystemService(Context.SENSOR_SERVICE) as? SensorManager
        gyroscope = sensorManager?.getDefaultSensor(Sensor.TYPE_GYROSCOPE)
    }
    
    @JvmStatic
    fun isAvailable(): Boolean = gyroscope != null
    
    @JvmStatic
    fun start(samplingPeriodUs: Int = SensorManager.SENSOR_DELAY_NORMAL): Boolean {
        val manager = sensorManager ?: return false
        val sensor = gyroscope ?: return false
        
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
    fun getValuesDegrees(): Map<String, Float> = mapOf(
        "x" to Math.toDegrees(lastX.toDouble()).toFloat(),
        "y" to Math.toDegrees(lastY.toDouble()).toFloat(),
        "z" to Math.toDegrees(lastZ.toDouble()).toFloat()
    )
    
    @JvmStatic
    fun setOnRotationChanged(callback: (Float, Float, Float) -> Unit) {
        onRotationChanged = callback
    }
    
    override fun onSensorChanged(event: SensorEvent) {
        if (event.sensor.type != Sensor.TYPE_GYROSCOPE) return
        
        lastX = event.values[0]
        lastY = event.values[1]
        lastZ = event.values[2]
        lastTimestamp = event.timestamp
        
        onRotationChanged?.invoke(lastX, lastY, lastZ)
    }
    
    override fun onAccuracyChanged(sensor: Sensor, accuracy: Int) {
        // لا حاجة للتعامل مع هذا
    }
}
