// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadMagnetometerBridge.kt
// الوصف: جسر Kotlin لمستشعر المجال المغناطيسي (البوصلة)
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager

/**
 * جسر مستشعر المجال المغناطيسي — SadMagnetometerBridge
 */
object SadMagnetometerBridge : SensorEventListener {
    
    private var context: Context? = null
    private var sensorManager: SensorManager? = null
    private var magnetometer: Sensor? = null
    private var accelerometer: Sensor? = null
    private var isListening = false
    
    // القيم الخام للمغناطيسية (μT)
    private var lastX = 0f
    private var lastY = 0f
    private var lastZ = 0f
    
    // للبوصلة
    private var accelerometerValues = FloatArray(3)
    private var magnetometerValues = FloatArray(3)
    private var rotationMatrix = FloatArray(9)
    private var orientationAngles = FloatArray(3)
    
    private var azimuth = 0f  // الاتجاه (البوصلة)
    
    private var onMagneticFieldChanged: ((Float, Float, Float) -> Unit)? = null
    private var onHeadingChanged: ((Float, String) -> Unit)? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        sensorManager = ctx.getSystemService(Context.SENSOR_SERVICE) as? SensorManager
        magnetometer = sensorManager?.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD)
        accelerometer = sensorManager?.getDefaultSensor(Sensor.TYPE_ACCELEROMETER)
    }
    
    @JvmStatic
    fun isAvailable(): Boolean = magnetometer != null
    
    @JvmStatic
    fun start(samplingPeriodUs: Int = SensorManager.SENSOR_DELAY_UI): Boolean {
        val manager = sensorManager ?: return false
        val magSensor = magnetometer ?: return false
        val accSensor = accelerometer ?: return false
        
        if (isListening) return true
        
        val magRegistered = manager.registerListener(this, magSensor, samplingPeriodUs)
        val accRegistered = manager.registerListener(this, accSensor, samplingPeriodUs)
        
        isListening = magRegistered && accRegistered
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
    fun getMagneticField(): Map<String, Float> = mapOf(
        "x" to lastX,
        "y" to lastY,
        "z" to lastZ
    )
    
    @JvmStatic
    fun getAzimuth(): Float = azimuth
    
    @JvmStatic
    fun getAzimuthDegrees(): Float = Math.toDegrees(azimuth.toDouble()).toFloat()
    
    @JvmStatic
    fun getCardinalDirection(): String {
        val degrees = (getAzimuthDegrees() + 360) % 360
        return when {
            degrees >= 337.5 || degrees < 22.5 -> "شمال"
            degrees >= 22.5 && degrees < 67.5 -> "شمال شرق"
            degrees >= 67.5 && degrees < 112.5 -> "شرق"
            degrees >= 112.5 && degrees < 157.5 -> "جنوب شرق"
            degrees >= 157.5 && degrees < 202.5 -> "جنوب"
            degrees >= 202.5 && degrees < 247.5 -> "جنوب غرب"
            degrees >= 247.5 && degrees < 292.5 -> "غرب"
            else -> "شمال غرب"
        }
    }
    
    @JvmStatic
    fun setOnMagneticFieldChanged(callback: (Float, Float, Float) -> Unit) {
        onMagneticFieldChanged = callback
    }
    
    @JvmStatic
    fun setOnHeadingChanged(callback: (Float, String) -> Unit) {
        onHeadingChanged = callback
    }
    
    override fun onSensorChanged(event: SensorEvent) {
        when (event.sensor.type) {
            Sensor.TYPE_ACCELEROMETER -> {
                System.arraycopy(event.values, 0, accelerometerValues, 0, 3)
            }
            Sensor.TYPE_MAGNETIC_FIELD -> {
                lastX = event.values[0]
                lastY = event.values[1]
                lastZ = event.values[2]
                System.arraycopy(event.values, 0, magnetometerValues, 0, 3)
                
                onMagneticFieldChanged?.invoke(lastX, lastY, lastZ)
            }
        }
        
        // حساب الاتجاه
        val success = SensorManager.getRotationMatrix(
            rotationMatrix, null, 
            accelerometerValues, magnetometerValues
        )
        
        if (success) {
            SensorManager.getOrientation(rotationMatrix, orientationAngles)
            azimuth = orientationAngles[0]
            
            onHeadingChanged?.invoke(getAzimuthDegrees(), getCardinalDirection())
        }
    }
    
    override fun onAccuracyChanged(sensor: Sensor, accuracy: Int) {
        // لا حاجة للتعامل مع هذا
    }
}
