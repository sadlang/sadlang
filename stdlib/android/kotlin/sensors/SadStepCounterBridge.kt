// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadStepCounterBridge.kt
// الوصف: جسر Kotlin لعداد الخطوات
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager

/**
 * جسر عداد الخطوات — SadStepCounterBridge
 */
object SadStepCounterBridge : SensorEventListener {
    
    private var context: Context? = null
    private var sensorManager: SensorManager? = null
    private var stepCounter: Sensor? = null
    private var stepDetector: Sensor? = null
    private var isListening = false
    
    private var totalSteps = 0f
    private var initialSteps = -1f
    private var sessionSteps = 0
    
    private var onStepDetected: (() -> Unit)? = null
    private var onStepCountChanged: ((Int) -> Unit)? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        sensorManager = ctx.getSystemService(Context.SENSOR_SERVICE) as? SensorManager
        stepCounter = sensorManager?.getDefaultSensor(Sensor.TYPE_STEP_COUNTER)
        stepDetector = sensorManager?.getDefaultSensor(Sensor.TYPE_STEP_DETECTOR)
    }
    
    @JvmStatic
    fun isAvailable(): Boolean = stepCounter != null || stepDetector != null
    
    @JvmStatic
    fun hasStepCounter(): Boolean = stepCounter != null
    
    @JvmStatic
    fun hasStepDetector(): Boolean = stepDetector != null
    
    @JvmStatic
    fun start(): Boolean {
        val manager = sensorManager ?: return false
        
        if (isListening) return true
        
        var success = false
        
        stepCounter?.let { sensor ->
            success = manager.registerListener(this, sensor, SensorManager.SENSOR_DELAY_NORMAL)
        }
        
        stepDetector?.let { sensor ->
            success = manager.registerListener(this, sensor, SensorManager.SENSOR_DELAY_NORMAL) || success
        }
        
        isListening = success
        return success
    }
    
    @JvmStatic
    fun stop() {
        sensorManager?.unregisterListener(this)
        isListening = false
    }
    
    @JvmStatic
    fun isListening(): Boolean = isListening
    
    @JvmStatic
    fun getTotalSteps(): Int = totalSteps.toInt()
    
    @JvmStatic
    fun getSessionSteps(): Int = sessionSteps
    
    @JvmStatic
    fun resetSession() {
        initialSteps = totalSteps
        sessionSteps = 0
    }
    
    @JvmStatic
    fun getCaloriesBurned(weightKg: Float = 70f): Float {
        // تقريب: 0.04 سعرة حرارية لكل خطوة لشخص وزنه 70 كجم
        val caloriesPerStep = 0.04f * (weightKg / 70f)
        return sessionSteps * caloriesPerStep
    }
    
    @JvmStatic
    fun getDistanceKm(strideMeters: Float = 0.75f): Float {
        return (sessionSteps * strideMeters) / 1000f
    }
    
    @JvmStatic
    fun setOnStepDetected(callback: () -> Unit) {
        onStepDetected = callback
    }
    
    @JvmStatic
    fun setOnStepCountChanged(callback: (Int) -> Unit) {
        onStepCountChanged = callback
    }
    
    override fun onSensorChanged(event: SensorEvent) {
        when (event.sensor.type) {
            Sensor.TYPE_STEP_COUNTER -> {
                totalSteps = event.values[0]
                
                if (initialSteps < 0) {
                    initialSteps = totalSteps
                }
                
                sessionSteps = (totalSteps - initialSteps).toInt()
                onStepCountChanged?.invoke(sessionSteps)
            }
            Sensor.TYPE_STEP_DETECTOR -> {
                onStepDetected?.invoke()
            }
        }
    }
    
    override fun onAccuracyChanged(sensor: Sensor, accuracy: Int) {
        // لا حاجة للتعامل مع هذا
    }
}
