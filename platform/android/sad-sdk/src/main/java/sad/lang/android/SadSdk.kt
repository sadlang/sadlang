/**
 * =============================================================================
 * @file SadSdk.kt
 * @brief (AR) نقطة الدخول الموحدة لـ Sad Android SDK
 * @brief (EN) Unified Entry Point for Sad Android SDK
 * =============================================================================
 */

package sad.lang.android

import android.content.Context

/**
 * كائن SDK الرئيسي
 * يوفر وصولاً سهلاً لجميع مكونات SDK
 */
object SadSdk {
    
    private var context: Context? = null
    private var isInitialized = false
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Lazy Components
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * المفسر
     */
    val interpreter: SadInterpreter by lazy {
        checkInitialized()
        SadInterpreter.getInstance()
    }
    
    /**
     * التخزين
     */
    val storage: SadStorage by lazy {
        checkInitialized()
        SadStorage(context!!)
    }
    
    /**
     * الصوت
     */
    val audio: SadAudio by lazy {
        checkInitialized()
        SadAudio(context!!).also { it.init() }
    }
    
    /**
     * الشبكة
     */
    val network: SadNetwork by lazy {
        checkInitialized()
        SadNetwork(context!!).also { it.init() }
    }
    
    /**
     * الإشعارات
     */
    val notifications: SadNotifications by lazy {
        checkInitialized()
        SadNotifications(context!!).also { it.createDefaultChannel() }
    }
    
    /**
     * المشاركة
     */
    val share: SadShare by lazy {
        checkInitialized()
        SadShare(context!!)
    }
    
    /**
     * المصادقة الحيوية
     */
    val biometric: SadBiometric by lazy {
        checkInitialized()
        SadBiometric(context!!)
    }
    
    /**
     * NFC
     */
    val nfc: SadNfc by lazy {
        checkInitialized()
        SadNfc(context!!)
    }
    
    /**
     * الصحة واللياقة
     */
    val health: SadHealth by lazy {
        checkInitialized()
        SadHealth(context!!)
    }
    
    /**
     * الخرائط
     */
    val maps: SadMaps by lazy {
        checkInitialized()
        SadMaps(context!!)
    }
    
    /**
     * المشتريات
     */
    val billing: SadBilling by lazy {
        checkInitialized()
        SadBilling(context!!)
    }
    
    /**
     * Firebase
     */
    val firebase: SadFirebase by lazy {
        checkInitialized()
        SadFirebase(context!!)
    }
    
    /**
     * البلوتوث
     */
    val bluetooth: SadBluetooth by lazy {
        checkInitialized()
        SadBluetooth(context!!)
    }
    
    /**
     * الأذونات
     */
    val permissions: SadPermissions by lazy {
        checkInitialized()
        SadPermissions(context!!)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Initialization
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تهيئة SDK
     * يجب استدعاؤها في Application.onCreate()
     */
    fun init(context: Context, config: SadSdkConfig = SadSdkConfig()) {
        if (isInitialized) return
        
        this.context = context.applicationContext
        
        // Initialize interpreter
        interpreter.init(context, SadConfig(
            enableDebug = config.debugMode,
            enableStdlib = config.enableStdlib
        ))
        
        // Initialize Firebase if enabled
        if (config.enableFirebaseAnalytics || config.enableCrashReporting) {
            firebase.initAll(
                analyticsEnabled = config.enableFirebaseAnalytics,
                crashlyticsEnabled = config.enableCrashReporting
            )
        }
        
        isInitialized = true
    }
    
    /**
     * إنهاء SDK
     */
    fun shutdown() {
        if (!isInitialized) return
        
        interpreter.shutdown()
        audio.shutdown()
        network.shutdown()
        
        context = null
        isInitialized = false
    }
    
    /**
     * هل SDK مُهيّأ؟
     */
    fun isInitialized(): Boolean = isInitialized
    
    /**
     * الحصول على السياق
     */
    fun getContext(): Context {
        checkInitialized()
        return context!!
    }
    
    private fun checkInitialized() {
        if (!isInitialized) {
            throw IllegalStateException("SadSdk is not initialized. Call SadSdk.init(context) first.")
        }
    }
}

/**
 * إعدادات SDK
 */
data class SadSdkConfig(
    val debugMode: Boolean = false,
    val enableStdlib: Boolean = true,
    val enableCrashReporting: Boolean = false,
    val enableFirebaseAnalytics: Boolean = false
)

// ═══════════════════════════════════════════════════════════════════════════════
//  Extension Functions
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * تنفيذ كود لغة ص
 */
fun String.evalSad(): SadResult {
    return SadSdk.interpreter.eval(this)
}

/**
 * تنفيذ كود لغة ص (متزامن)
 */
suspend fun String.evalSadAsync(): SadResult {
    return SadSdk.interpreter.evalAsync(this)
}
