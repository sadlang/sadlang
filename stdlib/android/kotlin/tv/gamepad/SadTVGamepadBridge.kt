package sad.android.tv.gamepad

import android.content.Context
import android.os.Handler
import android.os.Looper
import android.view.InputDevice
import android.view.KeyEvent
import android.view.MotionEvent

/**
 * جسر ذراع التحكم التلفزيوني - Gamepad and game controller
 * TV Gamepad Bridge - Gamepad and game controller
 * 
 * @author فريق لغة ص
 */
object SadTVGamepadBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    
    /**
     * زر ذراع التحكم / Gamepad button
     */
    enum class GamepadButton {
        A,
        B,
        X,
        Y,
        L1,
        R1,
        L2,
        R2,
        L3,
        R3,
        START,
        SELECT,
        HOME,
        DPAD_UP,
        DPAD_DOWN,
        DPAD_LEFT,
        DPAD_RIGHT,
        UNKNOWN
    }
    
    /**
     * محور ذراع التحكم / Gamepad axis
     */
    enum class GamepadAxis {
        LEFT_X,
        LEFT_Y,
        RIGHT_X,
        RIGHT_Y,
        L2_TRIGGER,
        R2_TRIGGER
    }
    
    /**
     * حالة ذراع التحكم / Gamepad state
     */
    data class GamepadState(
        val deviceId: Int,
        val deviceName: String,
        val pressedButtons: Set<GamepadButton>,
        val axes: Map<GamepadAxis, Float>,
        val isConnected: Boolean
    )
    
    /**
     * حدث الزر / Button event
     */
    data class ButtonEvent(
        val button: GamepadButton,
        val isPressed: Boolean,
        val timestamp: Long = System.currentTimeMillis()
    )
    
    /**
     * حدث المحور / Axis event
     */
    data class AxisEvent(
        val axis: GamepadAxis,
        val value: Float,
        val timestamp: Long = System.currentTimeMillis()
    )
    
    /**
     * تكوين ذراع التحكم / Gamepad config
     */
    data class GamepadConfig(
        val deadZone: Float = 0.2f,
        val triggerThreshold: Float = 0.5f,
        val invertYAxis: Boolean = false,
        val sensitivity: Float = 1f
    )
    
    /**
     * مستمع ذراع التحكم / Gamepad listener
     */
    interface GamepadListener {
        fun onGamepadConnected(deviceId: Int, deviceName: String)
        fun onGamepadDisconnected(deviceId: Int)
        fun onButtonEvent(event: ButtonEvent)
        fun onAxisEvent(event: AxisEvent)
    }
    
    private var gamepadListener: GamepadListener? = null
    private var config = GamepadConfig()
    private val connectedGamepads = mutableMapOf<Int, GamepadState>()
    private val pressedButtons = mutableSetOf<GamepadButton>()
    private val axisValues = mutableMapOf<GamepadAxis, Float>()
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        scanGamepads()
    }
    
    /**
     * تعيين المستمع / Set listener
     */
    fun setGamepadListener(listener: GamepadListener?) {
        gamepadListener = listener
    }
    
    /**
     * تعيين التكوين / Set config
     */
    fun setConfig(newConfig: GamepadConfig) {
        config = newConfig
    }
    
    /**
     * الحصول على التكوين / Get config
     */
    fun getConfig(): GamepadConfig = config
    
    // ═══════════════════════════════════════════════════════════════
    //                         اكتشاف الأجهزة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * فحص أذرع التحكم / Scan gamepads
     */
    fun scanGamepads() {
        val deviceIds = InputDevice.getDeviceIds()
        
        for (id in deviceIds) {
            val device = InputDevice.getDevice(id) ?: continue
            
            if (isGamepad(device)) {
                registerGamepad(device)
            }
        }
    }
    
    private fun isGamepad(device: InputDevice): Boolean {
        val sources = device.sources
        return (sources and InputDevice.SOURCE_GAMEPAD) == InputDevice.SOURCE_GAMEPAD ||
               (sources and InputDevice.SOURCE_JOYSTICK) == InputDevice.SOURCE_JOYSTICK
    }
    
    private fun registerGamepad(device: InputDevice) {
        val state = GamepadState(
            deviceId = device.id,
            deviceName = device.name,
            pressedButtons = emptySet(),
            axes = emptyMap(),
            isConnected = true
        )
        
        connectedGamepads[device.id] = state
        mainHandler.post { gamepadListener?.onGamepadConnected(device.id, device.name) }
    }
    
    /**
     * الحصول على أذرع التحكم المتصلة / Get connected gamepads
     */
    fun getConnectedGamepads(): List<GamepadState> = connectedGamepads.values.toList()
    
    /**
     * هل هناك ذراع تحكم؟ / Is gamepad connected?
     */
    fun hasGamepad(): Boolean = connectedGamepads.isNotEmpty()
    
    // ═══════════════════════════════════════════════════════════════
    //                         معالجة الأحداث
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * معالجة حدث المفتاح / Handle key event
     */
    fun handleKeyEvent(event: KeyEvent): Boolean {
        val button = mapKeyCodeToButton(event.keyCode)
        if (button == GamepadButton.UNKNOWN) return false
        
        val isPressed = event.action == KeyEvent.ACTION_DOWN
        
        if (isPressed) {
            pressedButtons.add(button)
        } else {
            pressedButtons.remove(button)
        }
        
        val buttonEvent = ButtonEvent(button, isPressed)
        mainHandler.post { gamepadListener?.onButtonEvent(buttonEvent) }
        
        return true
    }
    
    /**
     * معالجة حدث الحركة / Handle motion event
     */
    fun handleMotionEvent(event: MotionEvent): Boolean {
        if (!isGamepad(InputDevice.getDevice(event.deviceId) ?: return false)) {
            return false
        }
        
        // المحور الأيسر
        processAxis(event, MotionEvent.AXIS_X, GamepadAxis.LEFT_X)
        processAxis(event, MotionEvent.AXIS_Y, GamepadAxis.LEFT_Y)
        
        // المحور الأيمن
        processAxis(event, MotionEvent.AXIS_Z, GamepadAxis.RIGHT_X)
        processAxis(event, MotionEvent.AXIS_RZ, GamepadAxis.RIGHT_Y)
        
        // الزنادان
        processAxis(event, MotionEvent.AXIS_LTRIGGER, GamepadAxis.L2_TRIGGER)
        processAxis(event, MotionEvent.AXIS_RTRIGGER, GamepadAxis.R2_TRIGGER)
        
        return true
    }
    
    private fun processAxis(event: MotionEvent, motionAxis: Int, gamepadAxis: GamepadAxis) {
        var value = event.getAxisValue(motionAxis)
        
        // تطبيق منطقة الميت
        if (kotlin.math.abs(value) < config.deadZone) {
            value = 0f
        }
        
        // عكس المحور Y إذا كان مفعّلاً
        if (config.invertYAxis && (gamepadAxis == GamepadAxis.LEFT_Y || gamepadAxis == GamepadAxis.RIGHT_Y)) {
            value = -value
        }
        
        // تطبيق الحساسية
        value *= config.sensitivity
        
        val previousValue = axisValues[gamepadAxis] ?: 0f
        if (value != previousValue) {
            axisValues[gamepadAxis] = value
            
            val axisEvent = AxisEvent(gamepadAxis, value)
            mainHandler.post { gamepadListener?.onAxisEvent(axisEvent) }
        }
    }
    
    private fun mapKeyCodeToButton(keyCode: Int): GamepadButton {
        return when (keyCode) {
            KeyEvent.KEYCODE_BUTTON_A -> GamepadButton.A
            KeyEvent.KEYCODE_BUTTON_B -> GamepadButton.B
            KeyEvent.KEYCODE_BUTTON_X -> GamepadButton.X
            KeyEvent.KEYCODE_BUTTON_Y -> GamepadButton.Y
            KeyEvent.KEYCODE_BUTTON_L1 -> GamepadButton.L1
            KeyEvent.KEYCODE_BUTTON_R1 -> GamepadButton.R1
            KeyEvent.KEYCODE_BUTTON_L2 -> GamepadButton.L2
            KeyEvent.KEYCODE_BUTTON_R2 -> GamepadButton.R2
            KeyEvent.KEYCODE_BUTTON_THUMBL -> GamepadButton.L3
            KeyEvent.KEYCODE_BUTTON_THUMBR -> GamepadButton.R3
            KeyEvent.KEYCODE_BUTTON_START -> GamepadButton.START
            KeyEvent.KEYCODE_BUTTON_SELECT -> GamepadButton.SELECT
            KeyEvent.KEYCODE_BUTTON_MODE -> GamepadButton.HOME
            KeyEvent.KEYCODE_DPAD_UP -> GamepadButton.DPAD_UP
            KeyEvent.KEYCODE_DPAD_DOWN -> GamepadButton.DPAD_DOWN
            KeyEvent.KEYCODE_DPAD_LEFT -> GamepadButton.DPAD_LEFT
            KeyEvent.KEYCODE_DPAD_RIGHT -> GamepadButton.DPAD_RIGHT
            else -> GamepadButton.UNKNOWN
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الحالة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * هل الزر مضغوط؟ / Is button pressed?
     */
    fun isButtonPressed(button: GamepadButton): Boolean = button in pressedButtons
    
    /**
     * الحصول على قيمة المحور / Get axis value
     */
    fun getAxisValue(axis: GamepadAxis): Float = axisValues[axis] ?: 0f
    
    /**
     * الحصول على الأزرار المضغوطة / Get pressed buttons
     */
    fun getPressedButtons(): Set<GamepadButton> = pressedButtons.toSet()
    
    /**
     * الحصول على اسم الزر بالعربية / Get button name in Arabic
     */
    fun getButtonName(button: GamepadButton): String {
        return when (button) {
            GamepadButton.A -> "أ (A)"
            GamepadButton.B -> "ب (B)"
            GamepadButton.X -> "س (X)"
            GamepadButton.Y -> "ص (Y)"
            GamepadButton.L1 -> "L1"
            GamepadButton.R1 -> "R1"
            GamepadButton.L2 -> "L2"
            GamepadButton.R2 -> "R2"
            GamepadButton.L3 -> "L3"
            GamepadButton.R3 -> "R3"
            GamepadButton.START -> "ابدأ"
            GamepadButton.SELECT -> "اختيار"
            GamepadButton.HOME -> "الرئيسية"
            GamepadButton.DPAD_UP -> "أعلى"
            GamepadButton.DPAD_DOWN -> "أسفل"
            GamepadButton.DPAD_LEFT -> "يسار"
            GamepadButton.DPAD_RIGHT -> "يمين"
            GamepadButton.UNKNOWN -> "غير معروف"
        }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        connectedGamepads.clear()
        pressedButtons.clear()
        axisValues.clear()
        gamepadListener = null
        context = null
    }
}
