package sad.android.communication.assistant

import android.app.Activity
import android.content.Context
import android.content.Intent
import android.os.Build
import android.os.Bundle
import android.speech.RecognitionListener
import android.speech.RecognizerIntent
import android.speech.SpeechRecognizer
import android.speech.tts.TextToSpeech
import android.speech.tts.UtteranceProgressListener
import java.util.*
import java.util.concurrent.ConcurrentHashMap

/**
 * جسر المساعد الصوتي - التعرف على الكلام وتحويل النص إلى كلام
 * Voice Assistant Bridge - Speech recognition and text-to-speech
 * 
 * @author فريق لغة ص
 */
object SadVoiceAssistantBridge {
    
    private var tts: TextToSpeech? = null
    private var speechRecognizer: SpeechRecognizer? = null
    private var ttsReady = false
    private val utteranceCallbacks = ConcurrentHashMap<String, () -> Unit>()
    
    /**
     * حالة TTS / TTS state
     */
    enum class TtsState {
        NOT_INITIALIZED,
        INITIALIZING,
        READY,
        ERROR
    }
    
    /**
     * مستمع التعرف على الكلام / Speech recognition listener
     */
    interface SpeechListener {
        fun onReadyForSpeech()
        fun onBeginningOfSpeech()
        fun onEndOfSpeech()
        fun onResult(text: String, confidence: Float)
        fun onPartialResult(text: String)
        fun onError(errorCode: Int, message: String)
    }
    
    /**
     * مستمع TTS / TTS listener
     */
    interface TtsListener {
        fun onStart(utteranceId: String)
        fun onDone(utteranceId: String)
        fun onError(utteranceId: String, errorCode: Int)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         تحويل النص إلى كلام
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة TTS / Initialize TTS
     */
    fun initializeTts(context: Context, locale: Locale = Locale("ar"), onReady: (Boolean) -> Unit) {
        tts = TextToSpeech(context.applicationContext) { status ->
            if (status == TextToSpeech.SUCCESS) {
                val result = tts?.setLanguage(locale)
                ttsReady = result != TextToSpeech.LANG_MISSING_DATA && 
                          result != TextToSpeech.LANG_NOT_SUPPORTED
                onReady(ttsReady)
            } else {
                ttsReady = false
                onReady(false)
            }
        }
    }
    
    /**
     * نطق نص / Speak text
     */
    fun speak(
        text: String,
        queueMode: Int = TextToSpeech.QUEUE_FLUSH,
        utteranceId: String = UUID.randomUUID().toString(),
        onComplete: (() -> Unit)? = null
    ): Boolean {
        if (!ttsReady || tts == null) return false
        
        onComplete?.let { utteranceCallbacks[utteranceId] = it }
        
        val params = Bundle()
        return tts?.speak(text, queueMode, params, utteranceId) == TextToSpeech.SUCCESS
    }
    
    /**
     * نطق نص مع انتظار الانتهاء / Speak and wait
     */
    fun speakAndWait(text: String, onComplete: () -> Unit): Boolean {
        return speak(text, onComplete = onComplete)
    }
    
    /**
     * إضافة نص للطابور / Add to queue
     */
    fun addToQueue(text: String): Boolean {
        return speak(text, queueMode = TextToSpeech.QUEUE_ADD)
    }
    
    /**
     * إيقاف النطق / Stop speaking
     */
    fun stopSpeaking(): Boolean {
        return tts?.stop() == TextToSpeech.SUCCESS
    }
    
    /**
     * هل يتحدث الآن؟ / Is speaking?
     */
    fun isSpeaking(): Boolean {
        return tts?.isSpeaking == true
    }
    
    /**
     * تغيير سرعة النطق / Set speech rate
     */
    fun setSpeechRate(rate: Float): Boolean {
        return tts?.setSpeechRate(rate) == TextToSpeech.SUCCESS
    }
    
    /**
     * تغيير نبرة الصوت / Set pitch
     */
    fun setPitch(pitch: Float): Boolean {
        return tts?.setPitch(pitch) == TextToSpeech.SUCCESS
    }
    
    /**
     * تغيير اللغة / Set language
     */
    fun setLanguage(locale: Locale): Boolean {
        val result = tts?.setLanguage(locale)
        return result != TextToSpeech.LANG_MISSING_DATA && result != TextToSpeech.LANG_NOT_SUPPORTED
    }
    
    /**
     * اللغات المتاحة / Available languages
     */
    fun getAvailableLanguages(): Set<Locale> {
        return tts?.availableLanguages ?: emptySet()
    }
    
    /**
     * تعيين مستمع TTS / Set TTS listener
     */
    fun setTtsListener(listener: TtsListener) {
        tts?.setOnUtteranceProgressListener(object : UtteranceProgressListener() {
            override fun onStart(utteranceId: String?) {
                utteranceId?.let { listener.onStart(it) }
            }
            
            override fun onDone(utteranceId: String?) {
                utteranceId?.let { id ->
                    listener.onDone(id)
                    utteranceCallbacks.remove(id)?.invoke()
                }
            }
            
            @Deprecated("Deprecated in API 21")
            override fun onError(utteranceId: String?) {
                utteranceId?.let { listener.onError(it, -1) }
            }
            
            override fun onError(utteranceId: String?, errorCode: Int) {
                utteranceId?.let { listener.onError(it, errorCode) }
            }
        })
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التعرف على الكلام
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * هل التعرف على الكلام متاح؟ / Is speech recognition available?
     */
    fun isSpeechRecognitionAvailable(context: Context): Boolean {
        return SpeechRecognizer.isRecognitionAvailable(context)
    }
    
    /**
     * بدء التعرف على الكلام / Start speech recognition
     */
    fun startListening(context: Context, listener: SpeechListener, locale: Locale = Locale("ar")) {
        if (!isSpeechRecognitionAvailable(context)) {
            listener.onError(-1, "التعرف على الكلام غير متاح")
            return
        }
        
        speechRecognizer?.destroy()
        speechRecognizer = SpeechRecognizer.createSpeechRecognizer(context)
        
        speechRecognizer?.setRecognitionListener(object : RecognitionListener {
            override fun onReadyForSpeech(params: Bundle?) {
                listener.onReadyForSpeech()
            }
            
            override fun onBeginningOfSpeech() {
                listener.onBeginningOfSpeech()
            }
            
            override fun onRmsChanged(rmsdB: Float) {}
            override fun onBufferReceived(buffer: ByteArray?) {}
            
            override fun onEndOfSpeech() {
                listener.onEndOfSpeech()
            }
            
            override fun onError(error: Int) {
                listener.onError(error, getErrorMessage(error))
            }
            
            override fun onResults(results: Bundle?) {
                val matches = results?.getStringArrayList(SpeechRecognizer.RESULTS_RECOGNITION)
                val confidences = results?.getFloatArray(SpeechRecognizer.CONFIDENCE_SCORES)
                
                if (!matches.isNullOrEmpty()) {
                    listener.onResult(matches[0], confidences?.firstOrNull() ?: 0f)
                }
            }
            
            override fun onPartialResults(partialResults: Bundle?) {
                val matches = partialResults?.getStringArrayList(SpeechRecognizer.RESULTS_RECOGNITION)
                if (!matches.isNullOrEmpty()) {
                    listener.onPartialResult(matches[0])
                }
            }
            
            override fun onEvent(eventType: Int, params: Bundle?) {}
        })
        
        val intent = Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH).apply {
            putExtra(RecognizerIntent.EXTRA_LANGUAGE_MODEL, RecognizerIntent.LANGUAGE_MODEL_FREE_FORM)
            putExtra(RecognizerIntent.EXTRA_LANGUAGE, locale.toString())
            putExtra(RecognizerIntent.EXTRA_PARTIAL_RESULTS, true)
            putExtra(RecognizerIntent.EXTRA_MAX_RESULTS, 5)
        }
        
        speechRecognizer?.startListening(intent)
    }
    
    /**
     * إيقاف الاستماع / Stop listening
     */
    fun stopListening() {
        speechRecognizer?.stopListening()
    }
    
    /**
     * إلغاء الاستماع / Cancel listening
     */
    fun cancelListening() {
        speechRecognizer?.cancel()
    }
    
    /**
     * فتح نشاط التعرف على الكلام / Open speech recognition activity
     */
    fun openSpeechRecognizer(activity: Activity, requestCode: Int, prompt: String = "تحدث الآن") {
        val intent = Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH).apply {
            putExtra(RecognizerIntent.EXTRA_LANGUAGE_MODEL, RecognizerIntent.LANGUAGE_MODEL_FREE_FORM)
            putExtra(RecognizerIntent.EXTRA_LANGUAGE, Locale("ar").toString())
            putExtra(RecognizerIntent.EXTRA_PROMPT, prompt)
        }
        
        activity.startActivityForResult(intent, requestCode)
    }
    
    /**
     * معالجة نتيجة التعرف / Handle recognition result
     */
    fun handleSpeechResult(requestCode: Int, resultCode: Int, data: Intent?): String? {
        if (resultCode == Activity.RESULT_OK && data != null) {
            val results = data.getStringArrayListExtra(RecognizerIntent.EXTRA_RESULTS)
            return results?.firstOrNull()
        }
        return null
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         Google Assistant
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * فتح Google Assistant / Open Google Assistant
     */
    fun openGoogleAssistant(context: Context): Boolean {
        return try {
            val intent = Intent(Intent.ACTION_VOICE_COMMAND)
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            context.startActivity(intent)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * إرسال استعلام إلى Assistant / Send query to Assistant
     */
    fun queryAssistant(context: Context, query: String): Boolean {
        return try {
            val intent = Intent(Intent.ACTION_WEB_SEARCH)
            intent.putExtra("query", query)
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            context.startActivity(intent)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التنظيف
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        tts?.stop()
        tts?.shutdown()
        tts = null
        ttsReady = false
        
        speechRecognizer?.destroy()
        speechRecognizer = null
        
        utteranceCallbacks.clear()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    private fun getErrorMessage(errorCode: Int): String {
        return when (errorCode) {
            SpeechRecognizer.ERROR_AUDIO -> "خطأ في الصوت"
            SpeechRecognizer.ERROR_CLIENT -> "خطأ في العميل"
            SpeechRecognizer.ERROR_INSUFFICIENT_PERMISSIONS -> "لا توجد صلاحية"
            SpeechRecognizer.ERROR_NETWORK -> "خطأ في الشبكة"
            SpeechRecognizer.ERROR_NETWORK_TIMEOUT -> "انتهت المهلة"
            SpeechRecognizer.ERROR_NO_MATCH -> "لا توجد نتائج"
            SpeechRecognizer.ERROR_RECOGNIZER_BUSY -> "الخدمة مشغولة"
            SpeechRecognizer.ERROR_SERVER -> "خطأ في الخادم"
            SpeechRecognizer.ERROR_SPEECH_TIMEOUT -> "لم يتم الكلام"
            else -> "خطأ غير معروف"
        }
    }
}
