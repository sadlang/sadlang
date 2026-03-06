// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadBiometricBridge.kt
// الوصف: جسر Kotlin للمصادقة البيومترية (بصمة/وجه)
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import androidx.biometric.BiometricManager
import androidx.biometric.BiometricPrompt
import androidx.core.content.ContextCompat
import androidx.fragment.app.FragmentActivity

/**
 * جسر المصادقة البيومترية — SadBiometricBridge
 */
object SadBiometricBridge {
    
    private var activity: FragmentActivity? = null
    
    @JvmStatic
    fun initialize(act: FragmentActivity) {
        activity = act
    }
    
    /**
     * التحقق من توفر المصادقة البيومترية
     */
    @JvmStatic
    fun isAvailable(): Boolean {
        val context = activity ?: return false
        val biometricManager = BiometricManager.from(context)
        return biometricManager.canAuthenticate(BiometricManager.Authenticators.BIOMETRIC_STRONG) ==
                BiometricManager.BIOMETRIC_SUCCESS
    }
    
    /**
     * بدء المصادقة
     */
    @JvmStatic
    fun authenticate(
        title: String = "تأكيد الهوية",
        subtitle: String = "استخدم بصمتك للمتابعة",
        negativeButtonText: String = "إلغاء",
        callback: (Boolean, String?) -> Unit
    ) {
        val act = activity ?: return callback(false, "لم يتم التهيئة")
        
        val executor = ContextCompat.getMainExecutor(act)
        
        val biometricPrompt = BiometricPrompt(act, executor,
            object : BiometricPrompt.AuthenticationCallback() {
                override fun onAuthenticationSucceeded(result: BiometricPrompt.AuthenticationResult) {
                    callback(true, null)
                }
                
                override fun onAuthenticationError(errorCode: Int, errString: CharSequence) {
                    callback(false, errString.toString())
                }
                
                override fun onAuthenticationFailed() {
                    callback(false, "فشل التعرف")
                }
            })
        
        val promptInfo = BiometricPrompt.PromptInfo.Builder()
            .setTitle(title)
            .setSubtitle(subtitle)
            .setNegativeButtonText(negativeButtonText)
            .build()
        
        biometricPrompt.authenticate(promptInfo)
    }
}
