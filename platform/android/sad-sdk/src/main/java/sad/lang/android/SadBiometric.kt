/**
 * =============================================================================
 * @file SadBiometric.kt
 * @brief (AR) واجهة المصادقة الحيوية — البصمة والوجه
 * @brief (EN) Biometric Authentication Interface — Fingerprint and Face
 * =============================================================================
 */

package sad.lang.android

import android.content.Context
import androidx.biometric.BiometricManager
import androidx.biometric.BiometricPrompt
import androidx.core.content.ContextCompat
import androidx.fragment.app.FragmentActivity
import java.util.concurrent.Executor

/**
 * نظام المصادقة الحيوية للغة ص
 */
class SadBiometric(private val context: Context) {
    
    companion object {
        init {
            System.loadLibrary("sad_android")
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Native Methods
    // ═══════════════════════════════════════════════════════════════════════════
    
    private external fun nativeIsAvailable(): Boolean
    private external fun nativeSetAvailable(available: Boolean)
    private external fun nativeAuthenticate(
        title: String,
        subtitle: String,
        description: String,
        negativeButtonText: String,
        callback: Any
    )
    private external fun nativeOnAuthResult(success: Boolean, errorMessage: String)
    private external fun nativeCancel()
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Types
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * نتيجة المصادقة
     */
    sealed class AuthResult {
        object Success : AuthResult()
        data class Error(val code: Int, val message: String) : AuthResult()
        object Cancelled : AuthResult()
    }
    
    /**
     * نوع المصادقة
     */
    enum class AuthenticationType {
        FINGERPRINT,
        FACE,
        IRIS,
        ANY
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Properties
    // ═══════════════════════════════════════════════════════════════════════════
    
    private var biometricPrompt: BiometricPrompt? = null
    private var executor: Executor = ContextCompat.getMainExecutor(context)
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Availability Check
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * هل المصادقة الحيوية متاحة؟
     */
    fun isAvailable(): Boolean {
        val biometricManager = BiometricManager.from(context)
        val canAuthenticate = biometricManager.canAuthenticate(
            BiometricManager.Authenticators.BIOMETRIC_STRONG or
            BiometricManager.Authenticators.BIOMETRIC_WEAK
        )
        
        val available = canAuthenticate == BiometricManager.BIOMETRIC_SUCCESS
        nativeSetAvailable(available)
        return available
    }
    
    /**
     * سبب عدم التوفر
     */
    fun getUnavailabilityReason(): String {
        val biometricManager = BiometricManager.from(context)
        return when (biometricManager.canAuthenticate(
            BiometricManager.Authenticators.BIOMETRIC_STRONG
        )) {
            BiometricManager.BIOMETRIC_SUCCESS -> "متاحة"
            BiometricManager.BIOMETRIC_ERROR_NO_HARDWARE -> "لا يوجد جهاز حيوي"
            BiometricManager.BIOMETRIC_ERROR_HW_UNAVAILABLE -> "الجهاز غير متاح حالياً"
            BiometricManager.BIOMETRIC_ERROR_NONE_ENROLLED -> "لم يتم تسجيل بصمة"
            BiometricManager.BIOMETRIC_ERROR_SECURITY_UPDATE_REQUIRED -> "تحديث أمني مطلوب"
            BiometricManager.BIOMETRIC_ERROR_UNSUPPORTED -> "غير مدعوم"
            BiometricManager.BIOMETRIC_STATUS_UNKNOWN -> "حالة غير معروفة"
            else -> "خطأ غير معروف"
        }
    }
    
    /**
     * أنواع المصادقة المتاحة
     */
    fun getAvailableTypes(): List<AuthenticationType> {
        val types = mutableListOf<AuthenticationType>()
        val biometricManager = BiometricManager.from(context)
        
        // Check strong biometrics (fingerprint)
        if (biometricManager.canAuthenticate(BiometricManager.Authenticators.BIOMETRIC_STRONG) 
            == BiometricManager.BIOMETRIC_SUCCESS) {
            types.add(AuthenticationType.FINGERPRINT)
        }
        
        // Check weak biometrics (face, iris)
        if (biometricManager.canAuthenticate(BiometricManager.Authenticators.BIOMETRIC_WEAK)
            == BiometricManager.BIOMETRIC_SUCCESS) {
            types.add(AuthenticationType.FACE)
        }
        
        return types
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Authentication
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * بدء المصادقة
     */
    fun authenticate(
        activity: FragmentActivity,
        title: String = "المصادقة الحيوية",
        subtitle: String = "استخدم بصمتك للمتابعة",
        description: String = "",
        negativeButtonText: String = "إلغاء",
        callback: (AuthResult) -> Unit
    ) {
        if (!isAvailable()) {
            callback(AuthResult.Error(-1, getUnavailabilityReason()))
            return
        }
        
        executor = ContextCompat.getMainExecutor(activity)
        
        val authCallback = object : BiometricPrompt.AuthenticationCallback() {
            override fun onAuthenticationSucceeded(result: BiometricPrompt.AuthenticationResult) {
                super.onAuthenticationSucceeded(result)
                nativeOnAuthResult(true, "")
                callback(AuthResult.Success)
            }
            
            override fun onAuthenticationError(errorCode: Int, errString: CharSequence) {
                super.onAuthenticationError(errorCode, errString)
                val message = errString.toString()
                nativeOnAuthResult(false, message)
                
                if (errorCode == BiometricPrompt.ERROR_USER_CANCELED ||
                    errorCode == BiometricPrompt.ERROR_NEGATIVE_BUTTON) {
                    callback(AuthResult.Cancelled)
                } else {
                    callback(AuthResult.Error(errorCode, message))
                }
            }
            
            override fun onAuthenticationFailed() {
                super.onAuthenticationFailed()
                // Not called at end - user can retry
            }
        }
        
        biometricPrompt = BiometricPrompt(activity, executor, authCallback)
        
        val promptInfo = BiometricPrompt.PromptInfo.Builder()
            .setTitle(title)
            .setSubtitle(subtitle)
            .setDescription(description)
            .setNegativeButtonText(negativeButtonText)
            .setAllowedAuthenticators(
                BiometricManager.Authenticators.BIOMETRIC_STRONG or
                BiometricManager.Authenticators.BIOMETRIC_WEAK
            )
            .build()
        
        biometricPrompt?.authenticate(promptInfo)
    }
    
    /**
     * المصادقة مع تشفير
     */
    fun authenticateWithCrypto(
        activity: FragmentActivity,
        cryptoObject: BiometricPrompt.CryptoObject,
        title: String = "المصادقة الحيوية",
        subtitle: String = "استخدم بصمتك للمتابعة",
        negativeButtonText: String = "إلغاء",
        callback: (AuthResult, BiometricPrompt.CryptoObject?) -> Unit
    ) {
        if (!isAvailable()) {
            callback(AuthResult.Error(-1, getUnavailabilityReason()), null)
            return
        }
        
        executor = ContextCompat.getMainExecutor(activity)
        
        val authCallback = object : BiometricPrompt.AuthenticationCallback() {
            override fun onAuthenticationSucceeded(result: BiometricPrompt.AuthenticationResult) {
                super.onAuthenticationSucceeded(result)
                callback(AuthResult.Success, result.cryptoObject)
            }
            
            override fun onAuthenticationError(errorCode: Int, errString: CharSequence) {
                super.onAuthenticationError(errorCode, errString)
                if (errorCode == BiometricPrompt.ERROR_USER_CANCELED) {
                    callback(AuthResult.Cancelled, null)
                } else {
                    callback(AuthResult.Error(errorCode, errString.toString()), null)
                }
            }
        }
        
        biometricPrompt = BiometricPrompt(activity, executor, authCallback)
        
        val promptInfo = BiometricPrompt.PromptInfo.Builder()
            .setTitle(title)
            .setSubtitle(subtitle)
            .setNegativeButtonText(negativeButtonText)
            .build()
        
        biometricPrompt?.authenticate(promptInfo, cryptoObject)
    }
    
    /**
     * إلغاء المصادقة
     */
    fun cancel() {
        biometricPrompt?.cancelAuthentication()
        nativeCancel()
    }
}
