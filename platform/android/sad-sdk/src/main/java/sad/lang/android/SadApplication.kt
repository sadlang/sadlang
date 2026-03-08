/**
 * =============================================================================
 * @file SadApplication.kt
 * @brief (AR) صنف قاعدة للتطبيقات باستخدام Sad SDK
 * @brief (EN) Base Application Class for Sad SDK Apps
 * =============================================================================
 */

package sad.lang.android

import android.app.Application

/**
 * صنف قاعدة للتطبيقات
 * يقوم بتهيئة SDK تلقائياً
 */
open class SadApplication : Application() {
    
    /**
     * إعدادات SDK (يمكن تخصيصها في الأصناف المشتقة)
     */
    open val sdkConfig: SadSdkConfig = SadSdkConfig()
    
    override fun onCreate() {
        super.onCreate()
        
        // Initialize SDK
        SadSdk.init(this, sdkConfig)
        
        // Run custom initialization
        onSdkReady()
    }
    
    override fun onTerminate() {
        super.onTerminate()
        SadSdk.shutdown()
    }
    
    /**
     * يُستدعى بعد تهيئة SDK
     * يمكن للأصناف المشتقة تجاوز هذه الدالة
     */
    open fun onSdkReady() {
        // Override in subclass
    }
}
