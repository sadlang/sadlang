// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadLightSensorBridge.kt
// الوصف: جسر Kotlin لمستشعر الإضاءة
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager

/**
 * جسر مستشعر الإضاءة — SadLightSensorBridge
 */
object SadLightSensorBridge : SensorEventListener {
    
    private var context: Context? = null
    private var sensorManager: SensorManager? = null
    private var lightSensor: Sensor? = null
    private var isListening = false
    
    private var lastLux = 0f
    private var maxLux = 0f
    
    private var onLightChanged: ((Float) -> Unit)? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        sensorManager = ctx.getSystemService(Context.SENSOR_SERVICE) as? SensorManager
        lightSensor = sensorManager?.getDefaultSensor(Sensor.TYPE_LIGHT)
        maxLux = lightSensor?.maximumRange ?: 10000f
    }
    
    @JvmStatic
    fun isAvailable(): Boolean = lightSensor != null
    
    @JvmStatic
    fun start(): Boolean {
        val manager = sensorManager ?: return false
        val sensor = lightSensor ?: return false
        
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
    fun getLux(): Float = lastLux
    
    @JvmStatic
    fun getMaxLux(): Float = maxLux
    
    @JvmStatic
    fun getLightLevel(): String {
        return when {
            lastLux < 10 -> "مظلم جداً"
            lastLux < 50 -> "مظلم"
            lastLux < 200 -> "خافت"
            lastLux < 400 -> "معتدل"
            lastLux < 1000 -> "مضيء"
            lastLux < 10000 -> "مشرق"
            lastLux < 40000 -> "ضوء الشمس"
            else -> "ساطع جداً"
        }
    }
    
    @JvmStatic
    fun setOnLightChanged(callback: (Float) -> Unit) {
        onLightChanged = callback
    }
    
    override fun onSensorChanged(event: SensorEvent) {
        if (event.sensor.type != Sensor.TYPE_LIGHT) return
        
        lastLux = event.values[0]
        onLightChanged?.invoke(lastLux)
    }
    
    override fun onAccuracyChanged(sensor: Sensor, accuracy: Int) {
        // لا حاجة للتعامل مع هذا
    }
}
