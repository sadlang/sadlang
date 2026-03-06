package sad.android.widgets.steps

import android.content.Context
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager
import android.os.Handler
import android.os.Looper
import java.text.SimpleDateFormat
import java.util.*

/**
 * جسر ويدجت عداد الخطوات - تتبع النشاط البدني
 * Steps Widget Bridge - Physical activity tracking
 * 
 * يتطلب إذن ACTIVITY_RECOGNITION (Android 10+)
 * 
 * @author فريق لغة ص
 */
object SadWidgetStepsBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private var sensorManager: SensorManager? = null
    private var stepSensor: Sensor? = null
    private var stepCounterListener: SensorEventListener? = null
    
    private var initialSteps: Int = -1
    private var currentSteps: Int = 0
    private var dailyGoal: Int = 10000
    
    private val stepListeners = mutableListOf<StepListener>()
    
    /**
     * بيانات الخطوات / Step data
     */
    data class StepData(
        val steps: Int,
        val goal: Int,
        val progress: Float,      // 0 إلى 1
        val calories: Double,     // تقريبي
        val distance: Double,     // بالكيلومتر
        val activeMinutes: Int,
        val date: String
    )
    
    /**
     * إعدادات المستخدم / User settings
     */
    data class UserSettings(
        val heightCm: Int = 170,
        val weightKg: Int = 70,
        val strideLength: Double = 0.0  // إذا 0، يحسب من الطول
    )
    
    private var userSettings = UserSettings()
    
    /**
     * مستمع الخطوات / Step listener
     */
    interface StepListener {
        fun onStepUpdate(data: StepData)
        fun onGoalReached(steps: Int)
        fun onSensorError(error: String)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        sensorManager = ctx.getSystemService(Context.SENSOR_SERVICE) as? SensorManager
        stepSensor = sensorManager?.getDefaultSensor(Sensor.TYPE_STEP_COUNTER)
    }
    
    /**
     * إضافة مستمع / Add listener
     */
    fun addStepListener(listener: StepListener) {
        if (!stepListeners.contains(listener)) {
            stepListeners.add(listener)
        }
    }
    
    /**
     * إزالة مستمع / Remove listener
     */
    fun removeStepListener(listener: StepListener) {
        stepListeners.remove(listener)
    }
    
    /**
     * تعيين إعدادات المستخدم / Set user settings
     */
    fun setUserSettings(settings: UserSettings) {
        userSettings = settings
    }
    
    /**
     * تعيين الهدف اليومي / Set daily goal
     */
    fun setDailyGoal(goal: Int) {
        dailyGoal = goal
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         المراقبة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * هل المستشعر متاح؟ / Is sensor available?
     */
    fun isSensorAvailable(): Boolean {
        return stepSensor != null
    }
    
    /**
     * بدء المراقبة / Start monitoring
     */
    fun startMonitoring(): Boolean {
        val sensor = stepSensor
        val manager = sensorManager
        
        if (sensor == null || manager == null) {
            mainHandler.post {
                stepListeners.forEach { it.onSensorError("مستشعر الخطوات غير متاح") }
            }
            return false
        }
        
        if (stepCounterListener != null) {
            return true // بالفعل يراقب
        }
        
        stepCounterListener = object : SensorEventListener {
            override fun onSensorChanged(event: SensorEvent) {
                val totalSteps = event.values[0].toInt()
                
                // تهيئة العداد الأولي
                if (initialSteps < 0) {
                    initialSteps = totalSteps
                    loadSavedSteps()
                }
                
                // حساب الخطوات اليومية
                currentSteps = totalSteps - initialSteps
                
                val data = buildStepData()
                
                mainHandler.post {
                    stepListeners.forEach { it.onStepUpdate(data) }
                    
                    // التحقق من الوصول للهدف
                    if (currentSteps >= dailyGoal && currentSteps - 1 < dailyGoal) {
                        stepListeners.forEach { it.onGoalReached(currentSteps) }
                    }
                }
                
                saveSteps()
            }
            
            override fun onAccuracyChanged(sensor: Sensor, accuracy: Int) {}
        }
        
        return manager.registerListener(
            stepCounterListener,
            sensor,
            SensorManager.SENSOR_DELAY_UI
        )
    }
    
    /**
     * إيقاف المراقبة / Stop monitoring
     */
    fun stopMonitoring() {
        stepCounterListener?.let {
            sensorManager?.unregisterListener(it)
        }
        stepCounterListener = null
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         البيانات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على بيانات الخطوات الحالية / Get current step data
     */
    fun getCurrentStepData(): StepData {
        return buildStepData()
    }
    
    /**
     * الحصول على الخطوات الحالية / Get current steps
     */
    fun getCurrentSteps(): Int {
        return currentSteps
    }
    
    private fun buildStepData(): StepData {
        val progress = (currentSteps.toFloat() / dailyGoal).coerceIn(0f, 1f)
        val strideLength = if (userSettings.strideLength > 0) {
            userSettings.strideLength
        } else {
            userSettings.heightCm * 0.415 / 100  // بالمتر
        }
        val distance = currentSteps * strideLength / 1000  // بالكيلومتر
        val calories = calculateCalories(currentSteps)
        val activeMinutes = currentSteps / 100  // تقريبي: 100 خطوة = دقيقة
        
        val dateFormat = SimpleDateFormat("yyyy-MM-dd", Locale("ar"))
        
        return StepData(
            steps = currentSteps,
            goal = dailyGoal,
            progress = progress,
            calories = calories,
            distance = distance,
            activeMinutes = activeMinutes,
            date = dateFormat.format(Date())
        )
    }
    
    private fun calculateCalories(steps: Int): Double {
        // تقريبي: 0.04 سعرة حرارية لكل خطوة لكل كيلو من الوزن
        val caloriesPerStep = 0.04 * userSettings.weightKg / 70.0
        return steps * caloriesPerStep
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الحفظ والتحميل
    // ═══════════════════════════════════════════════════════════════
    
    private fun getPrefs() = context?.getSharedPreferences("sad_widget_steps", Context.MODE_PRIVATE)
    
    private fun saveSteps() {
        val prefs = getPrefs() ?: return
        val today = SimpleDateFormat("yyyyMMdd", Locale.US).format(Date())
        
        prefs.edit()
            .putString("last_date", today)
            .putInt("steps_$today", currentSteps)
            .putInt("initial_steps", initialSteps)
            .apply()
    }
    
    private fun loadSavedSteps() {
        val prefs = getPrefs() ?: return
        val today = SimpleDateFormat("yyyyMMdd", Locale.US).format(Date())
        val savedDate = prefs.getString("last_date", "")
        
        if (savedDate == today) {
            // نفس اليوم - استعادة البيانات
            currentSteps = prefs.getInt("steps_$today", 0)
            val savedInitial = prefs.getInt("initial_steps", -1)
            if (savedInitial >= 0) {
                initialSteps = savedInitial
            }
        } else {
            // يوم جديد - إعادة تعيين
            currentSteps = 0
            // سيتم تحديث initialSteps عند أول قراءة
        }
    }
    
    /**
     * إعادة تعيين الخطوات / Reset steps
     */
    fun resetSteps() {
        initialSteps = -1
        currentSteps = 0
        
        val prefs = getPrefs() ?: return
        val today = SimpleDateFormat("yyyyMMdd", Locale.US).format(Date())
        prefs.edit()
            .putString("last_date", today)
            .putInt("steps_$today", 0)
            .putInt("initial_steps", -1)
            .apply()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التنسيق
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تنسيق الخطوات / Format steps
     */
    fun formatSteps(steps: Int): String {
        return when {
            steps >= 1000000 -> String.format("%.1fM", steps / 1000000.0)
            steps >= 1000 -> String.format("%.1fK", steps / 1000.0)
            else -> steps.toString()
        }
    }
    
    /**
     * تنسيق المسافة / Format distance
     */
    fun formatDistance(km: Double): String {
        return when {
            km >= 1 -> String.format("%.1f كم", km)
            else -> String.format("%d م", (km * 1000).toInt())
        }
    }
    
    /**
     * تنسيق السعرات / Format calories
     */
    fun formatCalories(calories: Double): String {
        return String.format("%.0f سعرة", calories)
    }
    
    /**
     * الحصول على رسالة تحفيزية / Get motivational message
     */
    fun getMotivationalMessage(progress: Float): String {
        return when {
            progress >= 1f -> "أحسنت! وصلت للهدف 🎉"
            progress >= 0.75f -> "رائع! اقتربت من الهدف 💪"
            progress >= 0.5f -> "نصف الطريق! استمر 🚶"
            progress >= 0.25f -> "بداية جيدة! 👍"
            else -> "هيا لنبدأ! 🏃"
        }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        stopMonitoring()
        stepListeners.clear()
        sensorManager = null
        stepSensor = null
        context = null
    }
}
