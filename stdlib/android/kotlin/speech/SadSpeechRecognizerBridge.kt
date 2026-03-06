// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadSpeechRecognizerBridge.kt
// الوصف: جسر Kotlin للتعرف على الكلام
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.Manifest
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.os.Build
import android.os.Bundle
import android.speech.RecognitionListener
import android.speech.RecognizerIntent
import android.speech.SpeechRecognizer
import androidx.core.content.ContextCompat
import java.util.*

/**
 * جسر التعرف على الكلام — SadSpeechRecognizerBridge
 */
object SadSpeechRecognizerBridge {
    
    private var context: Context? = null
    private var speechRecognizer: SpeechRecognizer? = null
    private var isListening = false
    
    private var onResultsCallback: ((List<String>) -> Unit)? = null
    private var onPartialResultsCallback: ((List<String>) -> Unit)? = null
    private var onErrorCallback: ((Int, String) -> Unit)? = null
    private var onReadyCallback: (() -> Unit)? = null
    private var onEndCallback: (() -> Unit)? = null
    private var onRmsChangedCallback: ((Float) -> Unit)? = null
    
    @JvmStatic
    fun initialize(ctx: Context): Boolean {
        context = ctx.applicationContext
        
        if (!SpeechRecognizer.isRecognitionAvailable(ctx)) {
            return false
        }
        
        speechRecognizer = SpeechRecognizer.createSpeechRecognizer(ctx)
        setupRecognitionListener()
        
        return true
    }
    
    private fun setupRecognitionListener() {
        speechRecognizer?.setRecognitionListener(object : RecognitionListener {
            override fun onReadyForSpeech(params: Bundle?) {
                isListening = true
                onReadyCallback?.invoke()
            }
            
            override fun onBeginningOfSpeech() {
                // Speech input has begun
            }
            
            override fun onRmsChanged(rmsdB: Float) {
                onRmsChangedCallback?.invoke(rmsdB)
            }
            
            override fun onBufferReceived(buffer: ByteArray?) {
                // More sound has been received
            }
            
            override fun onEndOfSpeech() {
                isListening = false
                onEndCallback?.invoke()
            }
            
            override fun onError(error: Int) {
                isListening = false
                onErrorCallback?.invoke(error, getErrorMessage(error))
            }
            
            override fun onResults(results: Bundle?) {
                isListening = false
                val matches = results?.getStringArrayList(SpeechRecognizer.RESULTS_RECOGNITION)
                onResultsCallback?.invoke(matches ?: emptyList())
            }
            
            override fun onPartialResults(partialResults: Bundle?) {
                val matches = partialResults?.getStringArrayList(SpeechRecognizer.RESULTS_RECOGNITION)
                onPartialResultsCallback?.invoke(matches ?: emptyList())
            }
            
            override fun onEvent(eventType: Int, params: Bundle?) {
                // Reserved for adding future events
            }
        })
    }
    
    @JvmStatic
    fun destroy() {
        speechRecognizer?.destroy()
        speechRecognizer = null
        isListening = false
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Permission
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun hasRecordAudioPermission(): Boolean {
        val ctx = context ?: return false
        return ContextCompat.checkSelfPermission(ctx, Manifest.permission.RECORD_AUDIO) == 
            PackageManager.PERMISSION_GRANTED
    }
    
    @JvmStatic
    fun isRecognitionAvailable(): Boolean {
        val ctx = context ?: return false
        return SpeechRecognizer.isRecognitionAvailable(ctx)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Recognition
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun startListening(
        languageCode: String = "ar",
        partialResults: Boolean = true,
        maxResults: Int = 5
    ): Boolean {
        if (!hasRecordAudioPermission()) {
            onErrorCallback?.invoke(-1, "لا يوجد إذن للميكروفون")
            return false
        }
        
        val recognizer = speechRecognizer ?: return false
        
        val intent = Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH).apply {
            putExtra(RecognizerIntent.EXTRA_LANGUAGE_MODEL, RecognizerIntent.LANGUAGE_MODEL_FREE_FORM)
            putExtra(RecognizerIntent.EXTRA_LANGUAGE, languageCode)
            putExtra(RecognizerIntent.EXTRA_PARTIAL_RESULTS, partialResults)
            putExtra(RecognizerIntent.EXTRA_MAX_RESULTS, maxResults)
            putExtra(RecognizerIntent.EXTRA_CALLING_PACKAGE, context?.packageName)
        }
        
        try {
            recognizer.startListening(intent)
            return true
        } catch (e: Exception) {
            onErrorCallback?.invoke(-1, "فشل بدء الاستماع: ${e.message}")
            return false
        }
    }
    
    @JvmStatic
    fun startListeningArabic(): Boolean {
        return startListening("ar", true, 5)
    }
    
    @JvmStatic
    fun startListeningEnglish(): Boolean {
        return startListening("en", true, 5)
    }
    
    @JvmStatic
    fun stopListening() {
        speechRecognizer?.stopListening()
        isListening = false
    }
    
    @JvmStatic
    fun cancel() {
        speechRecognizer?.cancel()
        isListening = false
    }
    
    @JvmStatic
    fun isListening(): Boolean {
        return isListening
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Callbacks
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setOnResultsListener(callback: (List<String>) -> Unit) {
        onResultsCallback = callback
    }
    
    @JvmStatic
    fun setOnPartialResultsListener(callback: (List<String>) -> Unit) {
        onPartialResultsCallback = callback
    }
    
    @JvmStatic
    fun setOnErrorListener(callback: (Int, String) -> Unit) {
        onErrorCallback = callback
    }
    
    @JvmStatic
    fun setOnReadyListener(callback: () -> Unit) {
        onReadyCallback = callback
    }
    
    @JvmStatic
    fun setOnEndListener(callback: () -> Unit) {
        onEndCallback = callback
    }
    
    @JvmStatic
    fun setOnRmsChangedListener(callback: (Float) -> Unit) {
        onRmsChangedCallback = callback
    }
    
    @JvmStatic
    fun removeAllListeners() {
        onResultsCallback = null
        onPartialResultsCallback = null
        onErrorCallback = null
        onReadyCallback = null
        onEndCallback = null
        onRmsChangedCallback = null
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Error Messages
    // ═══════════════════════════════════════════════════════════════════════════════
    
    private fun getErrorMessage(errorCode: Int): String {
        return when (errorCode) {
            SpeechRecognizer.ERROR_AUDIO -> "خطأ في الصوت"
            SpeechRecognizer.ERROR_CLIENT -> "خطأ في العميل"
            SpeechRecognizer.ERROR_INSUFFICIENT_PERMISSIONS -> "أذونات غير كافية"
            SpeechRecognizer.ERROR_NETWORK -> "خطأ في الشبكة"
            SpeechRecognizer.ERROR_NETWORK_TIMEOUT -> "انتهاء مهلة الشبكة"
            SpeechRecognizer.ERROR_NO_MATCH -> "لم يتم التعرف على أي كلام"
            SpeechRecognizer.ERROR_RECOGNIZER_BUSY -> "المعرّف مشغول"
            SpeechRecognizer.ERROR_SERVER -> "خطأ في الخادم"
            SpeechRecognizer.ERROR_SPEECH_TIMEOUT -> "انتهاء مهلة الكلام"
            else -> "خطأ غير معروف"
        }
    }
    
    @JvmStatic
    fun getErrorDescription(errorCode: Int): String {
        return getErrorMessage(errorCode)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // One-shot Recognition
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun recognizeOnce(
        languageCode: String = "ar",
        callback: (Boolean, List<String>?, String?) -> Unit
    ) {
        // Set callbacks for one-shot recognition
        setOnResultsListener { results ->
            callback(true, results, null)
            removeAllListeners()
        }
        
        setOnErrorListener { _, message ->
            callback(false, null, message)
            removeAllListeners()
        }
        
        if (!startListening(languageCode, false, 5)) {
            callback(false, null, "فشل بدء الاستماع")
            removeAllListeners()
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Supported Languages
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getSupportedLanguages(): List<Map<String, String>> {
        // Note: This is a static list, actual availability depends on device
        return listOf(
            mapOf("code" to "ar", "name" to "العربية"),
            mapOf("code" to "ar-SA", "name" to "العربية (السعودية)"),
            mapOf("code" to "ar-EG", "name" to "العربية (مصر)"),
            mapOf("code" to "ar-AE", "name" to "العربية (الإمارات)"),
            mapOf("code" to "en", "name" to "الإنجليزية"),
            mapOf("code" to "en-US", "name" to "الإنجليزية (أمريكا)"),
            mapOf("code" to "en-GB", "name" to "الإنجليزية (بريطانيا)"),
            mapOf("code" to "fr", "name" to "الفرنسية"),
            mapOf("code" to "de", "name" to "الألمانية"),
            mapOf("code" to "es", "name" to "الإسبانية"),
            mapOf("code" to "it", "name" to "الإيطالية"),
            mapOf("code" to "zh", "name" to "الصينية"),
            mapOf("code" to "ja", "name" to "اليابانية"),
            mapOf("code" to "ko", "name" to "الكورية"),
            mapOf("code" to "ru", "name" to "الروسية"),
            mapOf("code" to "pt", "name" to "البرتغالية"),
            mapOf("code" to "tr", "name" to "التركية"),
            mapOf("code" to "hi", "name" to "الهندية"),
            mapOf("code" to "ur", "name" to "الأردية")
        )
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Status
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getStatus(): Map<String, Any> {
        return mapOf(
            "available" to isRecognitionAvailable(),
            "hasPermission" to hasRecordAudioPermission(),
            "listening" to isListening
        )
    }
}
