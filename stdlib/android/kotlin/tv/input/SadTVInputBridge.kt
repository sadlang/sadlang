package sad.android.tv.input

import android.content.Context
import android.os.Handler
import android.os.Looper
import android.view.KeyEvent

/**
 * جسر إدخال التلفزيون - Input methods and D-pad handling
 * TV Input Bridge - Input methods and D-pad handling
 * 
 * @author فريق لغة ص
 */
object SadTVInputBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    
    /**
     * نوع الإدخال / Input type
     */
    enum class InputType {
        DPAD,
        REMOTE,
        GAMEPAD,
        KEYBOARD,
        MOUSE,
        TOUCH
    }
    
    /**
     * حدث المفتاح / Key event
     */
    data class TVKeyEvent(
        val keyCode: Int,
        val action: Action,
        val repeatCount: Int = 0,
        val source: InputType = InputType.DPAD,
        val timestamp: Long = System.currentTimeMillis()
    )
    
    /**
     * الإجراء / Action
     */
    enum class Action {
        DOWN,
        UP,
        LONG_PRESS
    }
    
    /**
     * اتجاه التنقل / Navigation direction
     */
    enum class Direction {
        UP,
        DOWN,
        LEFT,
        RIGHT,
        CENTER,
        NONE
    }
    
    /**
     * تكوين الإدخال / Input config
     */
    data class InputConfig(
        val enableLongPress: Boolean = true,
        val longPressThreshold: Long = 500,
        val repeatDelay: Long = 50,
        val enableKeyRepeat: Boolean = true,
        val consumeBackKey: Boolean = false
    )
    
    /**
     * مستمع الإدخال / Input listener
     */
    interface InputListener {
        fun onKeyDown(event: TVKeyEvent): Boolean
        fun onKeyUp(event: TVKeyEvent): Boolean
        fun onKeyLongPress(event: TVKeyEvent): Boolean
        fun onNavigate(direction: Direction): Boolean
    }
    
    private var inputListener: InputListener? = null
    private var config = InputConfig()
    private val keyPressedTimes = mutableMapOf<Int, Long>()
    
    // رموز المفاتيح الشائعة
    companion object {
        // D-pad
        const val KEY_DPAD_UP = KeyEvent.KEYCODE_DPAD_UP
        const val KEY_DPAD_DOWN = KeyEvent.KEYCODE_DPAD_DOWN
        const val KEY_DPAD_LEFT = KeyEvent.KEYCODE_DPAD_LEFT
        const val KEY_DPAD_RIGHT = KeyEvent.KEYCODE_DPAD_RIGHT
        const val KEY_DPAD_CENTER = KeyEvent.KEYCODE_DPAD_CENTER
        
        // التحكم
        const val KEY_ENTER = KeyEvent.KEYCODE_ENTER
        const val KEY_BACK = KeyEvent.KEYCODE_BACK
        const val KEY_HOME = KeyEvent.KEYCODE_HOME
        const val KEY_MENU = KeyEvent.KEYCODE_MENU
        
        // الوسائط
        const val KEY_MEDIA_PLAY = KeyEvent.KEYCODE_MEDIA_PLAY
        const val KEY_MEDIA_PAUSE = KeyEvent.KEYCODE_MEDIA_PAUSE
        const val KEY_MEDIA_PLAY_PAUSE = KeyEvent.KEYCODE_MEDIA_PLAY_PAUSE
        const val KEY_MEDIA_STOP = KeyEvent.KEYCODE_MEDIA_STOP
        const val KEY_MEDIA_NEXT = KeyEvent.KEYCODE_MEDIA_NEXT
        const val KEY_MEDIA_PREVIOUS = KeyEvent.KEYCODE_MEDIA_PREVIOUS
        const val KEY_MEDIA_REWIND = KeyEvent.KEYCODE_MEDIA_REWIND
        const val KEY_MEDIA_FAST_FORWARD = KeyEvent.KEYCODE_MEDIA_FAST_FORWARD
        
        // الأرقام
        const val KEY_0 = KeyEvent.KEYCODE_0
        const val KEY_1 = KeyEvent.KEYCODE_1
        const val KEY_2 = KeyEvent.KEYCODE_2
        const val KEY_3 = KeyEvent.KEYCODE_3
        const val KEY_4 = KeyEvent.KEYCODE_4
        const val KEY_5 = KeyEvent.KEYCODE_5
        const val KEY_6 = KeyEvent.KEYCODE_6
        const val KEY_7 = KeyEvent.KEYCODE_7
        const val KEY_8 = KeyEvent.KEYCODE_8
        const val KEY_9 = KeyEvent.KEYCODE_9
        
        // الصوت
        const val KEY_VOLUME_UP = KeyEvent.KEYCODE_VOLUME_UP
        const val KEY_VOLUME_DOWN = KeyEvent.KEYCODE_VOLUME_DOWN
        const val KEY_VOLUME_MUTE = KeyEvent.KEYCODE_VOLUME_MUTE
        
        // القنوات
        const val KEY_CHANNEL_UP = KeyEvent.KEYCODE_CHANNEL_UP
        const val KEY_CHANNEL_DOWN = KeyEvent.KEYCODE_CHANNEL_DOWN
        
        // ألوان
        const val KEY_PROG_RED = KeyEvent.KEYCODE_PROG_RED
        const val KEY_PROG_GREEN = KeyEvent.KEYCODE_PROG_GREEN
        const val KEY_PROG_YELLOW = KeyEvent.KEYCODE_PROG_YELLOW
        const val KEY_PROG_BLUE = KeyEvent.KEYCODE_PROG_BLUE
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    /**
     * تعيين المستمع / Set listener
     */
    fun setInputListener(listener: InputListener?) {
        inputListener = listener
    }
    
    /**
     * تعيين التكوين / Set config
     */
    fun setConfig(newConfig: InputConfig) {
        config = newConfig
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معالجة المفاتيح
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * معالجة ضغط المفتاح / Handle key down
     */
    fun handleKeyDown(keyCode: Int, event: KeyEvent? = null): Boolean {
        val tvEvent = TVKeyEvent(
            keyCode = keyCode,
            action = Action.DOWN,
            repeatCount = event?.repeatCount ?: 0,
            source = detectInputSource(event)
        )
        
        // تسجيل وقت الضغط للكشف عن الضغطة الطويلة
        if (config.enableLongPress && tvEvent.repeatCount == 0) {
            keyPressedTimes[keyCode] = System.currentTimeMillis()
        }
        
        // التحقق من الضغطة الطويلة
        if (config.enableLongPress && tvEvent.repeatCount > 0) {
            val pressedTime = keyPressedTimes[keyCode] ?: System.currentTimeMillis()
            if (System.currentTimeMillis() - pressedTime >= config.longPressThreshold) {
                return inputListener?.onKeyLongPress(tvEvent.copy(action = Action.LONG_PRESS)) ?: false
            }
        }
        
        // التنقل
        val direction = keyCodeToDirection(keyCode)
        if (direction != Direction.NONE) {
            if (inputListener?.onNavigate(direction) == true) {
                return true
            }
        }
        
        return inputListener?.onKeyDown(tvEvent) ?: false
    }
    
    /**
     * معالجة رفع المفتاح / Handle key up
     */
    fun handleKeyUp(keyCode: Int, event: KeyEvent? = null): Boolean {
        keyPressedTimes.remove(keyCode)
        
        val tvEvent = TVKeyEvent(
            keyCode = keyCode,
            action = Action.UP,
            source = detectInputSource(event)
        )
        
        return inputListener?.onKeyUp(tvEvent) ?: false
    }
    
    private fun detectInputSource(event: KeyEvent?): InputType {
        event ?: return InputType.DPAD
        
        return when {
            event.device?.isVirtual == true -> InputType.REMOTE
            android.view.InputDevice.SOURCE_GAMEPAD and event.source != 0 -> InputType.GAMEPAD
            android.view.InputDevice.SOURCE_KEYBOARD and event.source != 0 -> InputType.KEYBOARD
            else -> InputType.DPAD
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التحويل
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تحويل رمز المفتاح لاتجاه / Convert key code to direction
     */
    fun keyCodeToDirection(keyCode: Int): Direction {
        return when (keyCode) {
            KEY_DPAD_UP -> Direction.UP
            KEY_DPAD_DOWN -> Direction.DOWN
            KEY_DPAD_LEFT -> Direction.LEFT
            KEY_DPAD_RIGHT -> Direction.RIGHT
            KEY_DPAD_CENTER, KEY_ENTER -> Direction.CENTER
            else -> Direction.NONE
        }
    }
    
    /**
     * تحويل رمز المفتاح لرقم / Convert key code to number
     */
    fun keyCodeToNumber(keyCode: Int): Int? {
        return when (keyCode) {
            KEY_0 -> 0
            KEY_1 -> 1
            KEY_2 -> 2
            KEY_3 -> 3
            KEY_4 -> 4
            KEY_5 -> 5
            KEY_6 -> 6
            KEY_7 -> 7
            KEY_8 -> 8
            KEY_9 -> 9
            else -> null
        }
    }
    
    /**
     * هل مفتاح تنقل؟ / Is navigation key?
     */
    fun isNavigationKey(keyCode: Int): Boolean {
        return keyCode in listOf(
            KEY_DPAD_UP, KEY_DPAD_DOWN, KEY_DPAD_LEFT, KEY_DPAD_RIGHT
        )
    }
    
    /**
     * هل مفتاح وسائط؟ / Is media key?
     */
    fun isMediaKey(keyCode: Int): Boolean {
        return keyCode in listOf(
            KEY_MEDIA_PLAY, KEY_MEDIA_PAUSE, KEY_MEDIA_PLAY_PAUSE,
            KEY_MEDIA_STOP, KEY_MEDIA_NEXT, KEY_MEDIA_PREVIOUS,
            KEY_MEDIA_REWIND, KEY_MEDIA_FAST_FORWARD
        )
    }
    
    /**
     * هل مفتاح رقم؟ / Is number key?
     */
    fun isNumberKey(keyCode: Int): Boolean {
        return keyCode in KEY_0..KEY_9
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         اسم المفتاح
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على اسم المفتاح / Get key name
     */
    fun getKeyName(keyCode: Int): String {
        return when (keyCode) {
            KEY_DPAD_UP -> "أعلى"
            KEY_DPAD_DOWN -> "أسفل"
            KEY_DPAD_LEFT -> "يسار"
            KEY_DPAD_RIGHT -> "يمين"
            KEY_DPAD_CENTER -> "اختيار"
            KEY_ENTER -> "إدخال"
            KEY_BACK -> "رجوع"
            KEY_HOME -> "الرئيسية"
            KEY_MENU -> "القائمة"
            KEY_MEDIA_PLAY -> "تشغيل"
            KEY_MEDIA_PAUSE -> "إيقاف مؤقت"
            KEY_MEDIA_PLAY_PAUSE -> "تشغيل/إيقاف"
            KEY_MEDIA_STOP -> "إيقاف"
            KEY_MEDIA_NEXT -> "التالي"
            KEY_MEDIA_PREVIOUS -> "السابق"
            KEY_MEDIA_REWIND -> "ترجيع"
            KEY_MEDIA_FAST_FORWARD -> "تقديم"
            KEY_VOLUME_UP -> "رفع الصوت"
            KEY_VOLUME_DOWN -> "خفض الصوت"
            KEY_VOLUME_MUTE -> "كتم الصوت"
            KEY_CHANNEL_UP -> "القناة التالية"
            KEY_CHANNEL_DOWN -> "القناة السابقة"
            in KEY_0..KEY_9 -> (keyCode - KEY_0).toString()
            else -> "غير معروف ($keyCode)"
        }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        keyPressedTimes.clear()
        inputListener = null
        context = null
    }
}
