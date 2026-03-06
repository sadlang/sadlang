package sad.android.tv.remote

import android.content.Context
import android.os.Handler
import android.os.Looper
import android.view.KeyEvent

/**
 * جسر جهاز التحكم التلفزيوني - معالجة أزرار الريموت
 * TV Remote Bridge - Remote control button handling
 * 
 * @author فريق لغة ص
 */
object SadTVRemoteBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    
    /**
     * زر الريموت / Remote button
     */
    enum class RemoteButton {
        // التنقل
        UP, DOWN, LEFT, RIGHT, CENTER, BACK, HOME,
        
        // الوسائط
        PLAY, PAUSE, PLAY_PAUSE, STOP, FAST_FORWARD, REWIND,
        SKIP_FORWARD, SKIP_BACKWARD, RECORD,
        
        // الصوت
        VOLUME_UP, VOLUME_DOWN, MUTE,
        
        // القنوات
        CHANNEL_UP, CHANNEL_DOWN,
        
        // الأرقام
        NUM_0, NUM_1, NUM_2, NUM_3, NUM_4,
        NUM_5, NUM_6, NUM_7, NUM_8, NUM_9,
        
        // الألوان
        RED, GREEN, YELLOW, BLUE,
        
        // إضافية
        MENU, GUIDE, INFO, SETTINGS, SEARCH,
        SUBTITLE, AUDIO, PIP, SLEEP,
        
        // غير معروف
        UNKNOWN
    }
    
    /**
     * حدث الزر / Button event
     */
    data class ButtonEvent(
        val button: RemoteButton,
        val action: ButtonAction,
        val timestamp: Long = System.currentTimeMillis(),
        val repeatCount: Int = 0
    )
    
    /**
     * إجراء الزر / Button action
     */
    enum class ButtonAction {
        DOWN,       // ضغط
        UP,         // رفع الضغط
        LONG_PRESS  // ضغط مطول
    }
    
    /**
     * مستمع الريموت / Remote listener
     */
    interface RemoteListener {
        fun onButtonPressed(event: ButtonEvent): Boolean
        fun onLongPress(button: RemoteButton): Boolean
        fun onNumberEntered(number: Int)
    }
    
    /**
     * حالة إدخال الأرقام / Number input state
     */
    data class NumberInputState(
        val digits: MutableList<Int> = mutableListOf(),
        val lastInputTime: Long = 0,
        val maxDigits: Int = 4,
        val timeoutMs: Long = 2000
    )
    
    private var remoteListener: RemoteListener? = null
    private var numberInputState = NumberInputState()
    private var longPressHandler: Handler? = null
    private var longPressRunnable: Runnable? = null
    private var currentPressedButton: RemoteButton? = null
    private var longPressThreshold = 500L
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        longPressHandler = Handler(Looper.getMainLooper())
    }
    
    /**
     * تعيين المستمع / Set listener
     */
    fun setRemoteListener(listener: RemoteListener?) {
        remoteListener = listener
    }
    
    /**
     * تعيين عتبة الضغط المطول / Set long press threshold
     */
    fun setLongPressThreshold(ms: Long) {
        longPressThreshold = ms
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معالجة المفاتيح
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * معالجة حدث مفتاح / Handle key event
     */
    fun handleKeyEvent(keyCode: Int, action: Int, repeatCount: Int = 0): Boolean {
        val button = keyCodeToButton(keyCode)
        if (button == RemoteButton.UNKNOWN) return false
        
        val buttonAction = when (action) {
            KeyEvent.ACTION_DOWN -> ButtonAction.DOWN
            KeyEvent.ACTION_UP -> ButtonAction.UP
            else -> return false
        }
        
        val event = ButtonEvent(button, buttonAction, System.currentTimeMillis(), repeatCount)
        
        // معالجة الضغط المطول
        if (buttonAction == ButtonAction.DOWN && repeatCount == 0) {
            startLongPressDetection(button)
        } else if (buttonAction == ButtonAction.UP) {
            cancelLongPressDetection()
        }
        
        // معالجة الأرقام
        if (isNumberButton(button) && buttonAction == ButtonAction.DOWN) {
            handleNumberInput(button)
        }
        
        // إعلام المستمع
        return mainHandler.post { remoteListener?.onButtonPressed(event) }.let { true }
    }
    
    private fun startLongPressDetection(button: RemoteButton) {
        cancelLongPressDetection()
        currentPressedButton = button
        
        longPressRunnable = Runnable {
            currentPressedButton?.let {
                mainHandler.post { remoteListener?.onLongPress(it) }
            }
        }
        
        longPressHandler?.postDelayed(longPressRunnable!!, longPressThreshold)
    }
    
    private fun cancelLongPressDetection() {
        longPressRunnable?.let { longPressHandler?.removeCallbacks(it) }
        longPressRunnable = null
        currentPressedButton = null
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدخال الأرقام
    // ═══════════════════════════════════════════════════════════════
    
    private fun handleNumberInput(button: RemoteButton) {
        val digit = buttonToDigit(button) ?: return
        val now = System.currentTimeMillis()
        
        // إعادة تعيين إذا انتهت المهلة
        if (now - numberInputState.lastInputTime > numberInputState.timeoutMs) {
            numberInputState.digits.clear()
        }
        
        // إضافة الرقم
        if (numberInputState.digits.size < numberInputState.maxDigits) {
            numberInputState.digits.add(digit)
            numberInputState = numberInputState.copy(lastInputTime = now)
        }
        
        // إرسال الرقم بعد المهلة
        mainHandler.removeCallbacksAndMessages("number_input")
        mainHandler.postDelayed({
            if (numberInputState.digits.isNotEmpty()) {
                val number = numberInputState.digits.joinToString("").toIntOrNull() ?: 0
                remoteListener?.onNumberEntered(number)
                numberInputState.digits.clear()
            }
        }, numberInputState.timeoutMs)
    }
    
    /**
     * الحصول على الرقم المدخل الحالي / Get current input number
     */
    fun getCurrentInputNumber(): Int? {
        return if (numberInputState.digits.isNotEmpty()) {
            numberInputState.digits.joinToString("").toIntOrNull()
        } else null
    }
    
    /**
     * مسح إدخال الأرقام / Clear number input
     */
    fun clearNumberInput() {
        numberInputState.digits.clear()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التحويلات
    // ═══════════════════════════════════════════════════════════════
    
    private fun keyCodeToButton(keyCode: Int): RemoteButton {
        return when (keyCode) {
            // التنقل
            KeyEvent.KEYCODE_DPAD_UP -> RemoteButton.UP
            KeyEvent.KEYCODE_DPAD_DOWN -> RemoteButton.DOWN
            KeyEvent.KEYCODE_DPAD_LEFT -> RemoteButton.LEFT
            KeyEvent.KEYCODE_DPAD_RIGHT -> RemoteButton.RIGHT
            KeyEvent.KEYCODE_DPAD_CENTER, KeyEvent.KEYCODE_ENTER -> RemoteButton.CENTER
            KeyEvent.KEYCODE_BACK -> RemoteButton.BACK
            KeyEvent.KEYCODE_HOME -> RemoteButton.HOME
            
            // الوسائط
            KeyEvent.KEYCODE_MEDIA_PLAY -> RemoteButton.PLAY
            KeyEvent.KEYCODE_MEDIA_PAUSE -> RemoteButton.PAUSE
            KeyEvent.KEYCODE_MEDIA_PLAY_PAUSE -> RemoteButton.PLAY_PAUSE
            KeyEvent.KEYCODE_MEDIA_STOP -> RemoteButton.STOP
            KeyEvent.KEYCODE_MEDIA_FAST_FORWARD -> RemoteButton.FAST_FORWARD
            KeyEvent.KEYCODE_MEDIA_REWIND -> RemoteButton.REWIND
            KeyEvent.KEYCODE_MEDIA_NEXT -> RemoteButton.SKIP_FORWARD
            KeyEvent.KEYCODE_MEDIA_PREVIOUS -> RemoteButton.SKIP_BACKWARD
            KeyEvent.KEYCODE_MEDIA_RECORD -> RemoteButton.RECORD
            
            // الصوت
            KeyEvent.KEYCODE_VOLUME_UP -> RemoteButton.VOLUME_UP
            KeyEvent.KEYCODE_VOLUME_DOWN -> RemoteButton.VOLUME_DOWN
            KeyEvent.KEYCODE_VOLUME_MUTE, KeyEvent.KEYCODE_MUTE -> RemoteButton.MUTE
            
            // القنوات
            KeyEvent.KEYCODE_CHANNEL_UP -> RemoteButton.CHANNEL_UP
            KeyEvent.KEYCODE_CHANNEL_DOWN -> RemoteButton.CHANNEL_DOWN
            
            // الأرقام
            KeyEvent.KEYCODE_0 -> RemoteButton.NUM_0
            KeyEvent.KEYCODE_1 -> RemoteButton.NUM_1
            KeyEvent.KEYCODE_2 -> RemoteButton.NUM_2
            KeyEvent.KEYCODE_3 -> RemoteButton.NUM_3
            KeyEvent.KEYCODE_4 -> RemoteButton.NUM_4
            KeyEvent.KEYCODE_5 -> RemoteButton.NUM_5
            KeyEvent.KEYCODE_6 -> RemoteButton.NUM_6
            KeyEvent.KEYCODE_7 -> RemoteButton.NUM_7
            KeyEvent.KEYCODE_8 -> RemoteButton.NUM_8
            KeyEvent.KEYCODE_9 -> RemoteButton.NUM_9
            
            // الألوان
            KeyEvent.KEYCODE_PROG_RED -> RemoteButton.RED
            KeyEvent.KEYCODE_PROG_GREEN -> RemoteButton.GREEN
            KeyEvent.KEYCODE_PROG_YELLOW -> RemoteButton.YELLOW
            KeyEvent.KEYCODE_PROG_BLUE -> RemoteButton.BLUE
            
            // إضافية
            KeyEvent.KEYCODE_MENU -> RemoteButton.MENU
            KeyEvent.KEYCODE_GUIDE -> RemoteButton.GUIDE
            KeyEvent.KEYCODE_INFO -> RemoteButton.INFO
            KeyEvent.KEYCODE_SETTINGS -> RemoteButton.SETTINGS
            KeyEvent.KEYCODE_SEARCH -> RemoteButton.SEARCH
            KeyEvent.KEYCODE_CAPTIONS -> RemoteButton.SUBTITLE
            
            else -> RemoteButton.UNKNOWN
        }
    }
    
    private fun isNumberButton(button: RemoteButton): Boolean {
        return button in listOf(
            RemoteButton.NUM_0, RemoteButton.NUM_1, RemoteButton.NUM_2,
            RemoteButton.NUM_3, RemoteButton.NUM_4, RemoteButton.NUM_5,
            RemoteButton.NUM_6, RemoteButton.NUM_7, RemoteButton.NUM_8,
            RemoteButton.NUM_9
        )
    }
    
    private fun buttonToDigit(button: RemoteButton): Int? {
        return when (button) {
            RemoteButton.NUM_0 -> 0
            RemoteButton.NUM_1 -> 1
            RemoteButton.NUM_2 -> 2
            RemoteButton.NUM_3 -> 3
            RemoteButton.NUM_4 -> 4
            RemoteButton.NUM_5 -> 5
            RemoteButton.NUM_6 -> 6
            RemoteButton.NUM_7 -> 7
            RemoteButton.NUM_8 -> 8
            RemoteButton.NUM_9 -> 9
            else -> null
        }
    }
    
    /**
     * الحصول على اسم الزر بالعربية / Get button name in Arabic
     */
    fun getButtonNameArabic(button: RemoteButton): String {
        return when (button) {
            RemoteButton.UP -> "أعلى"
            RemoteButton.DOWN -> "أسفل"
            RemoteButton.LEFT -> "يسار"
            RemoteButton.RIGHT -> "يمين"
            RemoteButton.CENTER -> "تأكيد"
            RemoteButton.BACK -> "رجوع"
            RemoteButton.HOME -> "الرئيسية"
            RemoteButton.PLAY -> "تشغيل"
            RemoteButton.PAUSE -> "إيقاف مؤقت"
            RemoteButton.PLAY_PAUSE -> "تشغيل/إيقاف"
            RemoteButton.STOP -> "إيقاف"
            RemoteButton.FAST_FORWARD -> "تقديم"
            RemoteButton.REWIND -> "ترجيع"
            RemoteButton.VOLUME_UP -> "رفع الصوت"
            RemoteButton.VOLUME_DOWN -> "خفض الصوت"
            RemoteButton.MUTE -> "كتم"
            RemoteButton.CHANNEL_UP -> "القناة التالية"
            RemoteButton.CHANNEL_DOWN -> "القناة السابقة"
            RemoteButton.MENU -> "القائمة"
            RemoteButton.GUIDE -> "الدليل"
            RemoteButton.INFO -> "معلومات"
            RemoteButton.SETTINGS -> "الإعدادات"
            RemoteButton.SEARCH -> "بحث"
            else -> button.name
        }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        cancelLongPressDetection()
        numberInputState.digits.clear()
        remoteListener = null
        longPressHandler = null
        context = null
    }
}
