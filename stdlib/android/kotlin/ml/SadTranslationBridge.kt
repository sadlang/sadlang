// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadTranslationBridge.kt
// الوصف: جسر Kotlin للترجمة باستخدام ML Kit
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import com.google.mlkit.common.model.DownloadConditions
import com.google.mlkit.nl.languageid.LanguageIdentification
import com.google.mlkit.nl.languageid.LanguageIdentificationOptions
import com.google.mlkit.nl.translate.TranslateLanguage
import com.google.mlkit.nl.translate.Translation
import com.google.mlkit.nl.translate.Translator
import com.google.mlkit.nl.translate.TranslatorOptions

/**
 * جسر الترجمة — SadTranslationBridge
 */
object SadTranslationBridge {
    
    private var context: Context? = null
    private val translators = mutableMapOf<String, Translator>()
    private val downloadedModels = mutableSetOf<String>()
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Language Detection
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun identifyLanguage(
        text: String,
        onSuccess: (String) -> Unit,
        onError: (String) -> Unit
    ) {
        val languageIdentifier = LanguageIdentification.getClient()
        
        languageIdentifier.identifyLanguage(text)
            .addOnSuccessListener { languageCode ->
                if (languageCode == "und") {
                    onSuccess("غير محدد")
                } else {
                    onSuccess(languageCode)
                }
            }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في تحديد اللغة")
            }
    }
    
    @JvmStatic
    fun identifyPossibleLanguages(
        text: String,
        confidenceThreshold: Float = 0.3f,
        onSuccess: (List<Map<String, Any?>>) -> Unit,
        onError: (String) -> Unit
    ) {
        val options = LanguageIdentificationOptions.Builder()
            .setConfidenceThreshold(confidenceThreshold)
            .build()
        
        val languageIdentifier = LanguageIdentification.getClient(options)
        
        languageIdentifier.identifyPossibleLanguages(text)
            .addOnSuccessListener { languages ->
                val results = languages.map { lang ->
                    mapOf(
                        "languageCode" to lang.languageTag,
                        "languageName" to getLanguageName(lang.languageTag),
                        "confidence" to lang.confidence
                    )
                }
                onSuccess(results)
            }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في تحديد اللغات")
            }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Translation
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun translate(
        text: String,
        sourceLanguage: String,
        targetLanguage: String,
        onSuccess: (String) -> Unit,
        onError: (String) -> Unit
    ) {
        val key = "$sourceLanguage->$targetLanguage"
        
        val translator = translators[key] ?: createTranslator(sourceLanguage, targetLanguage).also {
            translators[key] = it
        }
        
        // التحقق من تحميل النموذج
        if (!downloadedModels.contains(key)) {
            downloadModel(translator, key) { success ->
                if (success) {
                    performTranslation(translator, text, onSuccess, onError)
                } else {
                    onError("فشل في تحميل نموذج الترجمة")
                }
            }
        } else {
            performTranslation(translator, text, onSuccess, onError)
        }
    }
    
    @JvmStatic
    fun translateToArabic(
        text: String,
        sourceLanguage: String,
        onSuccess: (String) -> Unit,
        onError: (String) -> Unit
    ) {
        translate(text, sourceLanguage, "ar", onSuccess, onError)
    }
    
    @JvmStatic
    fun translateFromArabic(
        text: String,
        targetLanguage: String,
        onSuccess: (String) -> Unit,
        onError: (String) -> Unit
    ) {
        translate(text, "ar", targetLanguage, onSuccess, onError)
    }
    
    @JvmStatic
    fun translateAutoDetect(
        text: String,
        targetLanguage: String,
        onSuccess: (String, String) -> Unit,
        onError: (String) -> Unit
    ) {
        identifyLanguage(text, { detectedLanguage ->
            if (detectedLanguage == "غير محدد") {
                onError("لم يتم التعرف على لغة النص")
            } else {
                translate(text, detectedLanguage, targetLanguage, { translatedText ->
                    onSuccess(translatedText, detectedLanguage)
                }, onError)
            }
        }, onError)
    }
    
    private fun createTranslator(sourceLanguage: String, targetLanguage: String): Translator {
        val options = TranslatorOptions.Builder()
            .setSourceLanguage(sourceLanguage)
            .setTargetLanguage(targetLanguage)
            .build()
        
        return Translation.getClient(options)
    }
    
    private fun downloadModel(translator: Translator, key: String, onComplete: (Boolean) -> Unit) {
        val conditions = DownloadConditions.Builder()
            .requireWifi()
            .build()
        
        translator.downloadModelIfNeeded(conditions)
            .addOnSuccessListener {
                downloadedModels.add(key)
                onComplete(true)
            }
            .addOnFailureListener {
                onComplete(false)
            }
    }
    
    private fun performTranslation(
        translator: Translator,
        text: String,
        onSuccess: (String) -> Unit,
        onError: (String) -> Unit
    ) {
        translator.translate(text)
            .addOnSuccessListener { translatedText ->
                onSuccess(translatedText)
            }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في الترجمة")
            }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Model Management
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun downloadTranslationModel(
        sourceLanguage: String,
        targetLanguage: String,
        requireWifi: Boolean = true,
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        val key = "$sourceLanguage->$targetLanguage"
        val translator = createTranslator(sourceLanguage, targetLanguage)
        translators[key] = translator
        
        val conditions = DownloadConditions.Builder().apply {
            if (requireWifi) requireWifi()
        }.build()
        
        translator.downloadModelIfNeeded(conditions)
            .addOnSuccessListener {
                downloadedModels.add(key)
                onSuccess()
            }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في تحميل النموذج")
            }
    }
    
    @JvmStatic
    fun isModelDownloaded(sourceLanguage: String, targetLanguage: String): Boolean {
        return downloadedModels.contains("$sourceLanguage->$targetLanguage")
    }
    
    @JvmStatic
    fun getDownloadedModels(): List<String> {
        return downloadedModels.toList()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Language Codes
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getSupportedLanguages(): List<Map<String, String>> {
        return listOf(
            mapOf("code" to "ar", "name" to "العربية"),
            mapOf("code" to "en", "name" to "الإنجليزية"),
            mapOf("code" to "fr", "name" to "الفرنسية"),
            mapOf("code" to "de", "name" to "الألمانية"),
            mapOf("code" to "es", "name" to "الإسبانية"),
            mapOf("code" to "it", "name" to "الإيطالية"),
            mapOf("code" to "pt", "name" to "البرتغالية"),
            mapOf("code" to "ru", "name" to "الروسية"),
            mapOf("code" to "zh", "name" to "الصينية"),
            mapOf("code" to "ja", "name" to "اليابانية"),
            mapOf("code" to "ko", "name" to "الكورية"),
            mapOf("code" to "tr", "name" to "التركية"),
            mapOf("code" to "hi", "name" to "الهندية"),
            mapOf("code" to "id", "name" to "الإندونيسية"),
            mapOf("code" to "nl", "name" to "الهولندية"),
            mapOf("code" to "pl", "name" to "البولندية"),
            mapOf("code" to "sv", "name" to "السويدية"),
            mapOf("code" to "th", "name" to "التايلاندية"),
            mapOf("code" to "uk", "name" to "الأوكرانية"),
            mapOf("code" to "vi", "name" to "الفيتنامية")
        )
    }
    
    private fun getLanguageName(code: String): String {
        return when (code) {
            "ar" -> "العربية"
            "en" -> "الإنجليزية"
            "fr" -> "الفرنسية"
            "de" -> "الألمانية"
            "es" -> "الإسبانية"
            "it" -> "الإيطالية"
            "pt" -> "البرتغالية"
            "ru" -> "الروسية"
            "zh" -> "الصينية"
            "ja" -> "اليابانية"
            "ko" -> "الكورية"
            "tr" -> "التركية"
            "hi" -> "الهندية"
            "id" -> "الإندونيسية"
            "nl" -> "الهولندية"
            "pl" -> "البولندية"
            "sv" -> "السويدية"
            "th" -> "التايلاندية"
            "uk" -> "الأوكرانية"
            "vi" -> "الفيتنامية"
            "und" -> "غير محدد"
            else -> code
        }
    }
    
    // Language code constants
    object Languages {
        const val ARABIC = TranslateLanguage.ARABIC
        const val ENGLISH = TranslateLanguage.ENGLISH
        const val FRENCH = TranslateLanguage.FRENCH
        const val GERMAN = TranslateLanguage.GERMAN
        const val SPANISH = TranslateLanguage.SPANISH
        const val ITALIAN = TranslateLanguage.ITALIAN
        const val PORTUGUESE = TranslateLanguage.PORTUGUESE
        const val RUSSIAN = TranslateLanguage.RUSSIAN
        const val CHINESE = TranslateLanguage.CHINESE
        const val JAPANESE = TranslateLanguage.JAPANESE
        const val KOREAN = TranslateLanguage.KOREAN
        const val TURKISH = TranslateLanguage.TURKISH
        const val HINDI = TranslateLanguage.HINDI
        const val INDONESIAN = TranslateLanguage.INDONESIAN
        const val DUTCH = TranslateLanguage.DUTCH
        const val POLISH = TranslateLanguage.POLISH
        const val SWEDISH = TranslateLanguage.SWEDISH
        const val THAI = TranslateLanguage.THAI
        const val UKRAINIAN = TranslateLanguage.UKRAINIAN
        const val VIETNAMESE = TranslateLanguage.VIETNAMESE
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Cleanup
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun close() {
        translators.values.forEach { it.close() }
        translators.clear()
        downloadedModels.clear()
    }
    
    @JvmStatic
    fun closeTranslator(sourceLanguage: String, targetLanguage: String) {
        val key = "$sourceLanguage->$targetLanguage"
        translators[key]?.close()
        translators.remove(key)
        downloadedModels.remove(key)
    }
}
