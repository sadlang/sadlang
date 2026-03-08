/**
 * =============================================================================
 * @file SadSensors.kt
 * @brief (AR) جسر المستشعرات — Accelerometer, Gyroscope, Compass, etc.
 * @brief (EN) Sensors Bridge — Accelerometer, Gyroscope, Compass, etc.
 * 
 * المهمة: T247 - مستشعرات (Accelerometer)
 * المرحلة: سبتمبر 2026 — Android Sensors
 * =============================================================================
 * 
 * 🎯 الميزات
 * ══════════
 * ✓ Accelerometer (التسارع)
 * ✓ Gyroscope (الدوران)
 * ✓ Magnetometer/Compass (البوصلة)
 * ✓ Light sensor (الإضاءة)
 * ✓ Proximity sensor (القرب)
 * ✓ Pressure sensor (الضغط)
 * ✓ Step counter (عداد الخطوات)
 * 
 * =============================================================================
 */

package sad.platform.android.sensors

import android.content.Context
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager
import kotlin.math.sqrt

// =============================================================================
// الأنواع
// =============================================================================

/**
 * نوع المستشعر
 */
enum class SensorType {
    ACCELEROMETER,      // التسارع
    GYROSCOPE,          // الدوران
    MAGNETOMETER,       // المغناطيسية
    LIGHT,              // الإضاءة
    PROXIMITY,          // القرب
    PRESSURE,           // الضغط
    STEP_COUNTER,       // عداد الخطوات
    GRAVITY,            // الجاذبية
    LINEAR_ACCELERATION,// التسارع الخطي
    ROTATION_VECTOR,    // متجه الدوران
    ORIENTATION         // الاتجاه
}

/**
 * قراءة مستشعر ثلاثي المحاور
 */
data class Vector3(
    val x: Float = 0f,
    val y: Float = 0f,
    val z: Float = 0f
) {
    val magnitude: Float get() = sqrt(x * x + y * y + z * z)
}

/**
 * قراءة البوصلة
 */
data class CompassReading(
    val azimuth: Float = 0f,    // الاتجاه (0-360)
    val pitch: Float = 0f,      // الميل الأمامي
    val roll: Float = 0f        // الميل الجانبي
)

/**
 * مستمع المستشعرات
 */
interface SadSensorListener {
    fun onAccelerometerChanged(value: Vector3) {}
    fun onGyroscopeChanged(value: Vector3) {}
    fun onMagnetometerChanged(value: Vector3) {}
    fun onCompassChanged(reading: CompassReading) {}
    fun onLightChanged(lux: Float) {}
    fun onProximityChanged(cm: Float) {}
    fun onPressureChanged(hPa: Float) {}
    fun onStepCount(steps: Int) {}
    fun onShake() {}
}

// =============================================================================
// SadSensors
// =============================================================================

class SadSensors(private val context: Context) : SensorEventListener {
    
    // ─────────────────────────────────────────────
    // الخصائص
    // ─────────────────────────────────────────────
    
    private val sensorManager: SensorManager =
        context.getSystemService(Context.SENSOR_SERVICE) as SensorManager
    
    private val activeSensors = mutableMapOf<SensorType, Sensor?>()
    
    var listener: SadSensorListener? = null
    
    // للبوصلة
    private var gravity: FloatArray? = null
    private var geomagnetic: FloatArray? = null
    
    // للاهتزاز
    private var lastAccel = 0f
    private var currentAccel = 0f
    private var shakeThreshold = 12f
    private var lastShakeTime = 0L
    
    // ─────────────────────────────────────────────
    // التحكم
    // ─────────────────────────────────────────────
    
    /**
     * تفعيل مستشعر
     * @param type نوع المستشعر
     * @param delayUs التأخير (SENSOR_DELAY_NORMAL, GAME, UI, FASTEST)
     */
    fun enable(type: SensorType, delayUs: Int = SensorManager.SENSOR_DELAY_NORMAL) {
        val androidType = type.toAndroidType()
        val sensor = sensorManager.getDefaultSensor(androidType)
        
        if (sensor == null) {
            // المستشعر غير متوفر
            return
        }
        
        sensorManager.registerListener(this, sensor, delayUs)
        activeSensors[type] = sensor
        
        // البوصلة تحتاج المغناطيسية والتسارع معاً
        if (type == SensorType.ORIENTATION) {
            enable(SensorType.ACCELEROMETER, delayUs)
            enable(SensorType.MAGNETOMETER, delayUs)
        }
    }
    
    /**
     * تعطيل مستشعر
     */
    fun disable(type: SensorType) {
        activeSensors[type]?.let {
            sensorManager.unregisterListener(this, it)
        }
        activeSensors.remove(type)
    }
    
    /**
     * تعطيل جميع المستشعرات
     */
    fun disableAll() {
        sensorManager.unregisterListener(this)
        activeSensors.clear()
    }
    
    /**
     * هل المستشعر متوفر؟
     */
    fun isAvailable(type: SensorType): Boolean {
        return sensorManager.getDefaultSensor(type.toAndroidType()) != null
    }
    
    /**
     * قائمة المستشعرات المتوفرة
     */
    fun getAvailableSensors(): List<SensorType> {
        return SensorType.values().filter { isAvailable(it) }
    }
    
    // ─────────────────────────────────────────────
    // إعدادات
    // ─────────────────────────────────────────────
    
    /**
     * تعيين حد الاهتزاز
     */
    fun setShakeThreshold(threshold: Float) {
        shakeThreshold = threshold
    }
    
    // ─────────────────────────────────────────────
    // SensorEventListener
    // ─────────────────────────────────────────────
    
    override fun onSensorChanged(event: SensorEvent) {
        when (event.sensor.type) {
            Sensor.TYPE_ACCELEROMETER -> {
                val value = Vector3(event.values[0], event.values[1], event.values[2])
                listener?.onAccelerometerChanged(value)
                
                // حفظ للبوصلة
                gravity = event.values.clone()
                updateCompass()
                
                // كشف الاهتزاز
                detectShake(value)
            }
            
            Sensor.TYPE_GYROSCOPE -> {
                listener?.onGyroscopeChanged(
                    Vector3(event.values[0], event.values[1], event.values[2])
                )
            }
            
            Sensor.TYPE_MAGNETIC_FIELD -> {
                listener?.onMagnetometerChanged(
                    Vector3(event.values[0], event.values[1], event.values[2])
                )
                
                // حفظ للبوصلة
                geomagnetic = event.values.clone()
                updateCompass()
            }
            
            Sensor.TYPE_LIGHT -> {
                listener?.onLightChanged(event.values[0])
            }
            
            Sensor.TYPE_PROXIMITY -> {
                listener?.onProximityChanged(event.values[0])
            }
            
            Sensor.TYPE_PRESSURE -> {
                listener?.onPressureChanged(event.values[0])
            }
            
            Sensor.TYPE_STEP_COUNTER -> {
                listener?.onStepCount(event.values[0].toInt())
            }
        }
    }
    
    override fun onAccuracyChanged(sensor: Sensor?, accuracy: Int) {
        // يمكن إضافة callback للدقة
    }
    
    // ─────────────────────────────────────────────
    // Private
    // ─────────────────────────────────────────────
    
    private fun updateCompass() {
        val g = gravity ?: return
        val m = geomagnetic ?: return
        
        val R = FloatArray(9)
        val I = FloatArray(9)
        
        if (SensorManager.getRotationMatrix(R, I, g, m)) {
            val orientation = FloatArray(3)
            SensorManager.getOrientation(R, orientation)
            
            // تحويل من radians إلى degrees
            val azimuth = Math.toDegrees(orientation[0].toDouble()).toFloat()
            val pitch = Math.toDegrees(orientation[1].toDouble()).toFloat()
            val roll = Math.toDegrees(orientation[2].toDouble()).toFloat()
            
            // تحويل azimuth إلى (0-360)
            val normalizedAzimuth = (azimuth + 360) % 360
            
            listener?.onCompassChanged(CompassReading(normalizedAzimuth, pitch, roll))
        }
    }
    
    private fun detectShake(accel: Vector3) {
        lastAccel = currentAccel
        currentAccel = accel.magnitude
        
        val delta = currentAccel - lastAccel
        
        if (delta > shakeThreshold) {
            val now = System.currentTimeMillis()
            if (now - lastShakeTime > 500) {  // منع التكرار السريع
                listener?.onShake()
                lastShakeTime = now
            }
        }
    }
    
    private fun SensorType.toAndroidType(): Int = when (this) {
        SensorType.ACCELEROMETER -> Sensor.TYPE_ACCELEROMETER
        SensorType.GYROSCOPE -> Sensor.TYPE_GYROSCOPE
        SensorType.MAGNETOMETER -> Sensor.TYPE_MAGNETIC_FIELD
        SensorType.LIGHT -> Sensor.TYPE_LIGHT
        SensorType.PROXIMITY -> Sensor.TYPE_PROXIMITY
        SensorType.PRESSURE -> Sensor.TYPE_PRESSURE
        SensorType.STEP_COUNTER -> Sensor.TYPE_STEP_COUNTER
        SensorType.GRAVITY -> Sensor.TYPE_GRAVITY
        SensorType.LINEAR_ACCELERATION -> Sensor.TYPE_LINEAR_ACCELERATION
        SensorType.ROTATION_VECTOR -> Sensor.TYPE_ROTATION_VECTOR
        SensorType.ORIENTATION -> Sensor.TYPE_ORIENTATION
    }
}

// =============================================================================
// دوال مساعدة للغة ص (JNI)
// =============================================================================

/**
 * إنشاء مدير المستشعرات
 */
@JvmStatic
fun مستشعر_انشئ(context: Context): SadSensors = SadSensors(context)

/**
 * تفعيل مستشعر التسارع
 */
@JvmStatic
fun مستشعر_تسارع(sensors: SadSensors, enable: Boolean = true) {
    if (enable) sensors.enable(SensorType.ACCELEROMETER)
    else sensors.disable(SensorType.ACCELEROMETER)
}

/**
 * تفعيل البوصلة
 */
@JvmStatic
fun مستشعر_بوصلة(sensors: SadSensors, enable: Boolean = true) {
    if (enable) sensors.enable(SensorType.ORIENTATION)
    else sensors.disable(SensorType.ORIENTATION)
}

/**
 * تفعيل الجيروسكوب
 */
@JvmStatic
fun مستشعر_دوران(sensors: SadSensors, enable: Boolean = true) {
    if (enable) sensors.enable(SensorType.GYROSCOPE)
    else sensors.disable(SensorType.GYROSCOPE)
}

/**
 * تفعيل مستشعر الإضاءة
 */
@JvmStatic
fun مستشعر_إضاءة(sensors: SadSensors, enable: Boolean = true) {
    if (enable) sensors.enable(SensorType.LIGHT)
    else sensors.disable(SensorType.LIGHT)
}

/**
 * تفعيل عداد الخطوات
 */
@JvmStatic
fun مستشعر_خطوات(sensors: SadSensors, enable: Boolean = true) {
    if (enable) sensors.enable(SensorType.STEP_COUNTER)
    else sensors.disable(SensorType.STEP_COUNTER)
}

/**
 * إيقاف جميع المستشعرات
 */
@JvmStatic
fun مستشعر_أوقف_الكل(sensors: SadSensors) {
    sensors.disableAll()
}

/**
 * قائمة المستشعرات المتوفرة
 */
@JvmStatic
fun مستشعر_المتوفرة(sensors: SadSensors): List<String> {
    return sensors.getAvailableSensors().map { it.name }
}
