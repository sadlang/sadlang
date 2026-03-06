package sad.android.tv.voice

import android.content.Context
import android.os.Handler
import android.os.Looper

/**
 * جسر الصوت التلفزيوني - Voice commands and speech recognition
 * TV Voice Bridge - Voice commands and speech recognition
 * 
 * @author فريق لغة ص
 */
object SadTVVoiceBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    
    /**
     * حالة التعرف على الصوت / Recognition state
     */
    enum class RecognitionState {
        IDLE,
        LISTENING,
        PROCESSING,
        SUCCESS,
        ERROR
    }
    
    /**
     * نتيجة التعرف / Recognition result
     */
    data class RecognitionResult(
        val text: String,
        val confidence: Float,
        val alternatives: List<String> = emptyList(),
        val isPartial: Boolean = false
    )
    
    /**
     * أمر صوتي / Voice command
     */
    data class VoiceCommand(
        val id: String,
        val patterns: List<String>,
        val action: String,
        val parameters: Map<String, String> = emptyMap()
    )
    
    /**
     * تكوين التعرف على الصوت / Recognition config
     */
    data class VoiceConfig(
        val language: String = "ar",
        val partialResults: Boolean = true,
        val maxAlternatives: Int = 3,
        val offlineMode: Boolean = false,
        val continuousListening: Boolean = false
    )
    
    /**
     * مستمع الصوت / Voice listener
     */
    interface VoiceListener {
        fun onStateChanged(state: RecognitionState)
        fun onResult(result: RecognitionResult)
        fun onCommandRecognized(command: VoiceCommand, parameters: Map<String, String>)
        fun onError(error: String, errorCode: Int)
    }
    
    private var voiceListener: VoiceListener? = null
    private var config = VoiceConfig()
    private var currentState = RecognitionState.IDLE
    private val registeredCommands = mutableListOf<VoiceCommand>()
    
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
    fun setVoiceListener(listener: VoiceListener?) {
        voiceListener = listener
    }
    
    /**
     * تعيين التكوين / Set config
     */
    fun setConfig(newConfig: VoiceConfig) {
        config = newConfig
    }
    
    /**
     * الحصول على التكوين / Get config
     */
    fun getConfig(): VoiceConfig = config
    
    // ═══════════════════════════════════════════════════════════════
    //                         التعرف على الصوت
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * بدء الاستماع / Start listening
     */
    fun startListening() {
        setState(RecognitionState.LISTENING)
        // في التطبيق الحقيقي: استخدام SpeechRecognizer
    }
    
    /**
     * إيقاف الاستماع / Stop listening
     */
    fun stopListening() {
        setState(RecognitionState.IDLE)
    }
    
    /**
     * إلغاء الاستماع / Cancel listening
     */
    fun cancel() {
        setState(RecognitionState.IDLE)
    }
    
    /**
     * الحصول على الحالة / Get state
     */
    fun getState(): RecognitionState = currentState
    
    /**
     * هل يستمع؟ / Is listening?
     */
    fun isListening(): Boolean = currentState == RecognitionState.LISTENING
    
    private fun setState(state: RecognitionState) {
        currentState = state
        mainHandler.post { voiceListener?.onStateChanged(state) }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الأوامر الصوتية
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تسجيل أمر صوتي / Register voice command
     */
    fun registerCommand(command: VoiceCommand) {
        registeredCommands.add(command)
    }
    
    /**
     * تسجيل أمر بسيط / Register simple command
     */
    fun registerCommand(id: String, patterns: List<String>, action: String) {
        registeredCommands.add(VoiceCommand(id, patterns, action))
    }
    
    /**
     * إلغاء تسجيل أمر / Unregister command
     */
    fun unregisterCommand(id: String) {
        registeredCommands.removeIf { it.id == id }
    }
    
    /**
     * مسح جميع الأوامر / Clear all commands
     */
    fun clearCommands() {
        registeredCommands.clear()
    }
    
    /**
     * الحصول على الأوامر المسجلة / Get registered commands
     */
    fun getRegisteredCommands(): List<VoiceCommand> = registeredCommands.toList()
    
    /**
     * مطابقة النص مع الأوامر / Match text with commands
     */
    fun matchCommand(text: String): VoiceCommand? {
        val normalizedText = normalizeText(text)
        
        return registeredCommands.find { command ->
            command.patterns.any { pattern ->
                val normalizedPattern = normalizeText(pattern)
                normalizedText.contains(normalizedPattern)
            }
        }
    }
    
    private fun normalizeText(text: String): String {
        return text.trim().lowercase()
            .replace("أ", "ا")
            .replace("إ", "ا")
            .replace("آ", "ا")
            .replace("ة", "ه")
            .replace("ى", "ي")
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         أوامر افتراضية
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تسجيل أوامر التشغيل الافتراضية / Register default playback commands
     */
    fun registerDefaultPlaybackCommands() {
        registerCommand(
            VoiceCommand(
                id = "play",
                patterns = listOf("شغل", "افتح", "ابدأ", "تشغيل"),
                action = "PLAY"
            )
        )
        
        registerCommand(
            VoiceCommand(
                id = "pause",
                patterns = listOf("وقف", "أوقف", "إيقاف مؤقت", "توقف"),
                action = "PAUSE"
            )
        )
        
        registerCommand(
            VoiceCommand(
                id = "stop",
                patterns = listOf("أوقف تماما", "إيقاف", "قف"),
                action = "STOP"
            )
        )
        
        registerCommand(
            VoiceCommand(
                id = "next",
                patterns = listOf("التالي", "التالية", "الحلقة التالية", "القادم"),
                action = "NEXT"
            )
        )
        
        registerCommand(
            VoiceCommand(
                id = "previous",
                patterns = listOf("السابق", "السابقة", "الحلقة السابقة", "ارجع"),
                action = "PREVIOUS"
            )
        )
        
        registerCommand(
            VoiceCommand(
                id = "search",
                patterns = listOf("ابحث عن", "ابحث", "بحث عن", "دور على"),
                action = "SEARCH"
            )
        )
        
        registerCommand(
            VoiceCommand(
                id = "home",
                patterns = listOf("الرئيسية", "الصفحة الرئيسية", "الرئيسيه"),
                action = "HOME"
            )
        )
    }
    
    /**
     * تسجيل أوامر التنقل / Register navigation commands
     */
    fun registerNavigationCommands() {
        registerCommand(
            VoiceCommand(
                id = "up",
                patterns = listOf("فوق", "أعلى", "الأعلى"),
                action = "NAV_UP"
            )
        )
        
        registerCommand(
            VoiceCommand(
                id = "down",
                patterns = listOf("تحت", "أسفل", "الأسفل"),
                action = "NAV_DOWN"
            )
        )
        
        registerCommand(
            VoiceCommand(
                id = "left",
                patterns = listOf("يسار", "اليسار", "شمال"),
                action = "NAV_LEFT"
            )
        )
        
        registerCommand(
            VoiceCommand(
                id = "right",
                patterns = listOf("يمين", "اليمين"),
                action = "NAV_RIGHT"
            )
        )
        
        registerCommand(
            VoiceCommand(
                id = "select",
                patterns = listOf("اختر", "اختار", "موافق", "نعم"),
                action = "SELECT"
            )
        )
        
        registerCommand(
            VoiceCommand(
                id = "back",
                patterns = listOf("رجوع", "ارجع", "خلف", "عودة"),
                action = "BACK"
            )
        )
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معالجة النتائج
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * معالجة نتيجة التعرف / Process recognition result
     */
    fun processResult(result: RecognitionResult) {
        mainHandler.post { voiceListener?.onResult(result) }
        
        // محاولة مطابقة مع أمر
        val command = matchCommand(result.text)
        if (command != null) {
            val params = extractParameters(result.text, command)
            mainHandler.post { voiceListener?.onCommandRecognized(command, params) }
        }
    }
    
    private fun extractParameters(text: String, command: VoiceCommand): Map<String, String> {
        val params = mutableMapOf<String, String>()
        
        // استخراج النص بعد كلمة البحث
        if (command.action == "SEARCH") {
            val searchTerms = listOf("ابحث عن", "ابحث", "بحث عن", "دور على")
            for (term in searchTerms) {
                if (text.contains(term)) {
                    val query = text.substringAfter(term).trim()
                    if (query.isNotEmpty()) {
                        params["query"] = query
                    }
                    break
                }
            }
        }
        
        return params
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        cancel()
        registeredCommands.clear()
        voiceListener = null
        context = null
    }
}
