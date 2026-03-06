// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadTextToSpeechBridge.kt
// الوصف: جسر Kotlin للنص إلى كلام
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.os.Build
import android.os.Bundle
import android.speech.tts.TextToSpeech
import android.speech.tts.UtteranceProgressListener
import android.speech.tts.Voice
import java.io.File
import java.util.*

/**
 * جسر النص إلى كلام — SadTextToSpeechBridge
 */
object SadTextToSpeechBridge {
    
    private var context: Context? = null
    private var tts: TextToSpeech? = null
    private var isInitialized = false
    
    private var onSpeakStartCallback: ((String) -> Unit)? = null
    private var onSpeakDoneCallback: ((String) -> Unit)? = null
    private var onSpeakErrorCallback: ((String, Int) -> Unit)? = null
    
    @JvmStatic
    fun initialize(ctx: Context, callback: (Boolean) -> Unit) {
        context = ctx.applicationContext
        
        tts = TextToSpeech(ctx) { status ->
            isInitialized = status == TextToSpeech.SUCCESS
            
            if (isInitialized) {
                setupProgressListener()
            }
            
            callback(isInitialized)
        }
    }
    
    private fun setupProgressListener() {
        tts?.setOnUtteranceProgressListener(object : UtteranceProgressListener() {
            override fun onStart(utteranceId: String?) {
                utteranceId?.let { onSpeakStartCallback?.invoke(it) }
            }
            
            override fun onDone(utteranceId: String?) {
                utteranceId?.let { onSpeakDoneCallback?.invoke(it) }
            }
            
            override fun onError(utteranceId: String?) {
                utteranceId?.let { onSpeakErrorCallback?.invoke(it, -1) }
            }
            
            override fun onError(utteranceId: String?, errorCode: Int) {
                utteranceId?.let { onSpeakErrorCallback?.invoke(it, errorCode) }
            }
        })
    }
    
    @JvmStatic
    fun shutdown() {
        tts?.stop()
        tts?.shutdown()
        tts = null
        isInitialized = false
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Speaking
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun speak(text: String, utteranceId: String = UUID.randomUUID().toString()): Boolean {
        if (!isInitialized) return false
        
        val params = Bundle().apply {
            putString(TextToSpeech.Engine.KEY_PARAM_UTTERANCE_ID, utteranceId)
        }
        
        val result = tts?.speak(text, TextToSpeech.QUEUE_FLUSH, params, utteranceId)
        return result == TextToSpeech.SUCCESS
    }
    
    @JvmStatic
    fun speakAdd(text: String, utteranceId: String = UUID.randomUUID().toString()): Boolean {
        if (!isInitialized) return false
        
        val params = Bundle().apply {
            putString(TextToSpeech.Engine.KEY_PARAM_UTTERANCE_ID, utteranceId)
        }
        
        val result = tts?.speak(text, TextToSpeech.QUEUE_ADD, params, utteranceId)
        return result == TextToSpeech.SUCCESS
    }
    
    @JvmStatic
    fun stop(): Boolean {
        val result = tts?.stop()
        return result == TextToSpeech.SUCCESS
    }
    
    @JvmStatic
    fun isSpeaking(): Boolean {
        return tts?.isSpeaking == true
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Language & Voice
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setLanguage(languageCode: String, countryCode: String = ""): Boolean {
        if (!isInitialized) return false
        
        val locale = if (countryCode.isNotEmpty()) {
            Locale(languageCode, countryCode)
        } else {
            Locale(languageCode)
        }
        
        val result = tts?.setLanguage(locale)
        return result != TextToSpeech.LANG_MISSING_DATA && result != TextToSpeech.LANG_NOT_SUPPORTED
    }
    
    @JvmStatic
    fun setArabic(): Boolean {
        return setLanguage("ar")
    }
    
    @JvmStatic
    fun setEnglish(): Boolean {
        return setLanguage("en")
    }
    
    @JvmStatic
    fun getAvailableLanguages(): List<Map<String, String>> {
        if (!isInitialized) return emptyList()
        
        return try {
            tts?.availableLanguages?.map { locale ->
                mapOf(
                    "language" to locale.language,
                    "country" to locale.country,
                    "displayName" to locale.displayName,
                    "displayNameAr" to getArabicLanguageName(locale.language)
                )
            } ?: emptyList()
        } catch (e: Exception) {
            emptyList()
        }
    }
    
    private fun getArabicLanguageName(code: String): String {
        return when (code) {
            "ar" -> "العربية"
            "en" -> "الإنجليزية"
            "fr" -> "الفرنسية"
            "de" -> "الألمانية"
            "es" -> "الإسبانية"
            "it" -> "الإيطالية"
            "zh" -> "الصينية"
            "ja" -> "اليابانية"
            "ko" -> "الكورية"
            "ru" -> "الروسية"
            "pt" -> "البرتغالية"
            "tr" -> "التركية"
            "hi" -> "الهندية"
            "ur" -> "الأردية"
            else -> code
        }
    }
    
    @JvmStatic
    fun isLanguageAvailable(languageCode: String): Boolean {
        if (!isInitialized) return false
        
        val locale = Locale(languageCode)
        val result = tts?.isLanguageAvailable(locale)
        return result == TextToSpeech.LANG_AVAILABLE || 
               result == TextToSpeech.LANG_COUNTRY_AVAILABLE ||
               result == TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE
    }
    
    @JvmStatic
    fun getVoices(): List<Map<String, Any?>> {
        if (!isInitialized) return emptyList()
        
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP) {
            return emptyList()
        }
        
        return try {
            tts?.voices?.map { voice ->
                mapOf(
                    "name" to voice.name,
                    "locale" to voice.locale.toString(),
                    "quality" to voice.quality,
                    "latency" to voice.latency,
                    "requiresNetwork" to voice.isNetworkConnectionRequired
                )
            } ?: emptyList()
        } catch (e: Exception) {
            emptyList()
        }
    }
    
    @JvmStatic
    fun setVoice(voiceName: String): Boolean {
        if (!isInitialized) return false
        
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP) {
            return false
        }
        
        return try {
            val voice = tts?.voices?.find { it.name == voiceName }
            if (voice != null) {
                tts?.voice = voice
                true
            } else {
                false
            }
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun getCurrentVoice(): Map<String, Any?>? {
        if (!isInitialized) return null
        
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP) {
            return null
        }
        
        return try {
            tts?.voice?.let { voice ->
                mapOf(
                    "name" to voice.name,
                    "locale" to voice.locale.toString(),
                    "quality" to voice.quality,
                    "latency" to voice.latency
                )
            }
        } catch (e: Exception) {
            null
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Speech Parameters
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setSpeechRate(rate: Float): Boolean {
        // rate: 0.0 - 4.0, default 1.0
        val clampedRate = rate.coerceIn(0.1f, 4.0f)
        val result = tts?.setSpeechRate(clampedRate)
        return result == TextToSpeech.SUCCESS
    }
    
    @JvmStatic
    fun setPitch(pitch: Float): Boolean {
        // pitch: 0.0 - 2.0, default 1.0
        val clampedPitch = pitch.coerceIn(0.1f, 2.0f)
        val result = tts?.setPitch(clampedPitch)
        return result == TextToSpeech.SUCCESS
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Save to File
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun synthesizeToFile(
        text: String, 
        filePath: String, 
        utteranceId: String = UUID.randomUUID().toString()
    ): Boolean {
        if (!isInitialized) return false
        
        val file = File(filePath)
        
        val params = Bundle().apply {
            putString(TextToSpeech.Engine.KEY_PARAM_UTTERANCE_ID, utteranceId)
        }
        
        val result = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            tts?.synthesizeToFile(text, params, file, utteranceId)
        } else {
            @Suppress("DEPRECATION")
            tts?.synthesizeToFile(text, hashMapOf(
                TextToSpeech.Engine.KEY_PARAM_UTTERANCE_ID to utteranceId
            ), filePath)
        }
        
        return result == TextToSpeech.SUCCESS
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Callbacks
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setOnSpeakStartListener(callback: (String) -> Unit) {
        onSpeakStartCallback = callback
    }
    
    @JvmStatic
    fun setOnSpeakDoneListener(callback: (String) -> Unit) {
        onSpeakDoneCallback = callback
    }
    
    @JvmStatic
    fun setOnSpeakErrorListener(callback: (String, Int) -> Unit) {
        onSpeakErrorCallback = callback
    }
    
    @JvmStatic
    fun removeAllListeners() {
        onSpeakStartCallback = null
        onSpeakDoneCallback = null
        onSpeakErrorCallback = null
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Engine Info
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getDefaultEngine(): String? {
        return tts?.defaultEngine
    }
    
    @JvmStatic
    fun getEngines(): List<Map<String, String>> {
        return tts?.engines?.map { engine ->
            mapOf(
                "name" to engine.name,
                "label" to engine.label
            )
        } ?: emptyList()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Utilities
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun playSilence(durationMs: Long, utteranceId: String = UUID.randomUUID().toString()): Boolean {
        if (!isInitialized) return false
        
        val result = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            tts?.playSilentUtterance(durationMs, TextToSpeech.QUEUE_ADD, utteranceId)
        } else {
            @Suppress("DEPRECATION")
            tts?.playSilence(durationMs, TextToSpeech.QUEUE_ADD, hashMapOf(
                TextToSpeech.Engine.KEY_PARAM_UTTERANCE_ID to utteranceId
            ))
        }
        
        return result == TextToSpeech.SUCCESS
    }
    
    @JvmStatic
    fun getMaxSpeechInputLength(): Int {
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2) {
            TextToSpeech.getMaxSpeechInputLength()
        } else {
            4000
        }
    }
    
    @JvmStatic
    fun getStatus(): Map<String, Any?> {
        return mapOf(
            "initialized" to isInitialized,
            "speaking" to isSpeaking(),
            "defaultEngine" to getDefaultEngine(),
            "maxInputLength" to getMaxSpeechInputLength(),
            "currentVoice" to getCurrentVoice()
        )
    }
}
