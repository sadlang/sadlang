// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadPressureSensorBridge.kt
// الوصف: جسر Kotlin لمستشعر الضغط الجوي
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager

/**
 * جسر مستشعر الضغط الجوي — SadPressureSensorBridge
 */
object SadPressureSensorBridge : SensorEventListener {
    
    private var context: Context? = null
    private var sensorManager: SensorManager? = null
    private var pressureSensor: Sensor? = null
    private var isListening = false
    
    private var lastPressure = 0f  // hPa (mbar)
    
    private var onPressureChanged: ((Float) -> Unit)? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        sensorManager = ctx.getSystemService(Context.SENSOR_SERVICE) as? SensorManager
        pressureSensor = sensorManager?.getDefaultSensor(Sensor.TYPE_PRESSURE)
    }
    
    @JvmStatic
    fun isAvailable(): Boolean = pressureSensor != null
    
    @JvmStatic
    fun start(): Boolean {
        val manager = sensorManager ?: return false
        val sensor = pressureSensor ?: return false
        
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
    fun getPressure(): Float = lastPressure
    
    /**
     * حساب الارتفاع بالأمتار بناءً على الضغط الجوي
     * باستخدام معادلة الهيبسومتري
     */
    @JvmStatic
    fun getAltitude(seaLevelPressure: Float = SensorManager.PRESSURE_STANDARD_ATMOSPHERE): Float {
        return SensorManager.getAltitude(seaLevelPressure, lastPressure)
    }
    
    @JvmStatic
    fun getWeatherPrediction(): String {
        return when {
            lastPressure < 1000 -> "عاصف"
            lastPressure < 1013 -> "غائم/ممطر"
            lastPressure < 1020 -> "متغير"
            lastPressure < 1030 -> "جيد"
            else -> "صافي جداً"
        }
    }
    
    @JvmStatic
    fun setOnPressureChanged(callback: (Float) -> Unit) {
        onPressureChanged = callback
    }
    
    override fun onSensorChanged(event: SensorEvent) {
        if (event.sensor.type != Sensor.TYPE_PRESSURE) return
        
        lastPressure = event.values[0]
        onPressureChanged?.invoke(lastPressure)
    }
    
    override fun onAccuracyChanged(sensor: Sensor, accuracy: Int) {
        // لا حاجة للتعامل مع هذا
    }
}
