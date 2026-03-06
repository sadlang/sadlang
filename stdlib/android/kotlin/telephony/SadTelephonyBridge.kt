// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadTelephonyBridge.kt
// الوصف: جسر Kotlin للاتصالات الهاتفية
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.os.Build
import android.telephony.*
import androidx.core.content.ContextCompat

/**
 * جسر الاتصالات الهاتفية — SadTelephonyBridge
 */
object SadTelephonyBridge {
    
    private var context: Context? = null
    private var telephonyManager: TelephonyManager? = null
    private var callStateListener: PhoneStateListener? = null
    
    private var onCallStateChangedCallback: ((Int, String?) -> Unit)? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        telephonyManager = ctx.getSystemService(Context.TELEPHONY_SERVICE) as? TelephonyManager
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Device Info
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getNetworkOperatorName(): String {
        return telephonyManager?.networkOperatorName ?: "غير معروف"
    }
    
    @JvmStatic
    fun getNetworkOperatorCode(): String {
        return telephonyManager?.networkOperator ?: ""
    }
    
    @JvmStatic
    fun getSimOperatorName(): String {
        return telephonyManager?.simOperatorName ?: "غير معروف"
    }
    
    @JvmStatic
    fun getSimOperatorCode(): String {
        return telephonyManager?.simOperator ?: ""
    }
    
    @JvmStatic
    fun getNetworkCountryIso(): String {
        return telephonyManager?.networkCountryIso?.uppercase() ?: ""
    }
    
    @JvmStatic
    fun getSimCountryIso(): String {
        return telephonyManager?.simCountryIso?.uppercase() ?: ""
    }
    
    @JvmStatic
    fun getDeviceSoftwareVersion(): String? {
        val ctx = context ?: return null
        
        if (ContextCompat.checkSelfPermission(ctx, Manifest.permission.READ_PHONE_STATE) 
            != PackageManager.PERMISSION_GRANTED) {
            return null
        }
        
        return try {
            telephonyManager?.deviceSoftwareVersion
        } catch (e: SecurityException) {
            null
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Network Type
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getNetworkType(): String {
        val ctx = context ?: return "غير معروف"
        
        if (ContextCompat.checkSelfPermission(ctx, Manifest.permission.READ_PHONE_STATE) 
            != PackageManager.PERMISSION_GRANTED) {
            return "يتطلب إذن"
        }
        
        return try {
            val type = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
                telephonyManager?.dataNetworkType
            } else {
                @Suppress("DEPRECATION")
                telephonyManager?.networkType
            }
            
            getNetworkTypeName(type ?: TelephonyManager.NETWORK_TYPE_UNKNOWN)
        } catch (e: SecurityException) {
            "غير معروف"
        }
    }
    
    @JvmStatic
    fun getNetworkGeneration(): String {
        val ctx = context ?: return "غير معروف"
        
        if (ContextCompat.checkSelfPermission(ctx, Manifest.permission.READ_PHONE_STATE) 
            != PackageManager.PERMISSION_GRANTED) {
            return "يتطلب إذن"
        }
        
        return try {
            val type = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
                telephonyManager?.dataNetworkType
            } else {
                @Suppress("DEPRECATION")
                telephonyManager?.networkType
            }
            
            getNetworkGeneration(type ?: TelephonyManager.NETWORK_TYPE_UNKNOWN)
        } catch (e: SecurityException) {
            "غير معروف"
        }
    }
    
    private fun getNetworkTypeName(type: Int): String {
        return when (type) {
            TelephonyManager.NETWORK_TYPE_GPRS -> "GPRS"
            TelephonyManager.NETWORK_TYPE_EDGE -> "EDGE"
            TelephonyManager.NETWORK_TYPE_CDMA -> "CDMA"
            TelephonyManager.NETWORK_TYPE_1xRTT -> "1xRTT"
            TelephonyManager.NETWORK_TYPE_IDEN -> "IDEN"
            TelephonyManager.NETWORK_TYPE_UMTS -> "UMTS"
            TelephonyManager.NETWORK_TYPE_EVDO_0 -> "EVDO_0"
            TelephonyManager.NETWORK_TYPE_EVDO_A -> "EVDO_A"
            TelephonyManager.NETWORK_TYPE_HSDPA -> "HSDPA"
            TelephonyManager.NETWORK_TYPE_HSUPA -> "HSUPA"
            TelephonyManager.NETWORK_TYPE_HSPA -> "HSPA"
            TelephonyManager.NETWORK_TYPE_EVDO_B -> "EVDO_B"
            TelephonyManager.NETWORK_TYPE_EHRPD -> "EHRPD"
            TelephonyManager.NETWORK_TYPE_HSPAP -> "HSPAP"
            TelephonyManager.NETWORK_TYPE_LTE -> "LTE"
            TelephonyManager.NETWORK_TYPE_NR -> "5G NR"
            else -> "غير معروف"
        }
    }
    
    private fun getNetworkGeneration(type: Int): String {
        return when (type) {
            TelephonyManager.NETWORK_TYPE_GPRS,
            TelephonyManager.NETWORK_TYPE_EDGE,
            TelephonyManager.NETWORK_TYPE_CDMA,
            TelephonyManager.NETWORK_TYPE_1xRTT,
            TelephonyManager.NETWORK_TYPE_IDEN -> "2G"
            
            TelephonyManager.NETWORK_TYPE_UMTS,
            TelephonyManager.NETWORK_TYPE_EVDO_0,
            TelephonyManager.NETWORK_TYPE_EVDO_A,
            TelephonyManager.NETWORK_TYPE_HSDPA,
            TelephonyManager.NETWORK_TYPE_HSUPA,
            TelephonyManager.NETWORK_TYPE_HSPA,
            TelephonyManager.NETWORK_TYPE_EVDO_B,
            TelephonyManager.NETWORK_TYPE_EHRPD,
            TelephonyManager.NETWORK_TYPE_HSPAP -> "3G"
            
            TelephonyManager.NETWORK_TYPE_LTE -> "4G"
            
            TelephonyManager.NETWORK_TYPE_NR -> "5G"
            
            else -> "غير معروف"
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // SIM State
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getSimState(): String {
        return when (telephonyManager?.simState) {
            TelephonyManager.SIM_STATE_ABSENT -> "غير موجودة"
            TelephonyManager.SIM_STATE_NETWORK_LOCKED -> "مقفلة على الشبكة"
            TelephonyManager.SIM_STATE_PIN_REQUIRED -> "تتطلب PIN"
            TelephonyManager.SIM_STATE_PUK_REQUIRED -> "تتطلب PUK"
            TelephonyManager.SIM_STATE_READY -> "جاهزة"
            TelephonyManager.SIM_STATE_NOT_READY -> "غير جاهزة"
            TelephonyManager.SIM_STATE_PERM_DISABLED -> "معطلة دائماً"
            TelephonyManager.SIM_STATE_CARD_IO_ERROR -> "خطأ في القراءة"
            TelephonyManager.SIM_STATE_CARD_RESTRICTED -> "مقيدة"
            else -> "غير معروفة"
        }
    }
    
    @JvmStatic
    fun isSimReady(): Boolean {
        return telephonyManager?.simState == TelephonyManager.SIM_STATE_READY
    }
    
    @JvmStatic
    fun hasSimCard(): Boolean {
        return telephonyManager?.simState != TelephonyManager.SIM_STATE_ABSENT &&
               telephonyManager?.simState != TelephonyManager.SIM_STATE_UNKNOWN
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Call State
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getCallState(): String {
        return when (telephonyManager?.callState) {
            TelephonyManager.CALL_STATE_IDLE -> "خامل"
            TelephonyManager.CALL_STATE_RINGING -> "يرن"
            TelephonyManager.CALL_STATE_OFFHOOK -> "مكالمة جارية"
            else -> "غير معروف"
        }
    }
    
    @JvmStatic
    fun isCallIdle(): Boolean {
        return telephonyManager?.callState == TelephonyManager.CALL_STATE_IDLE
    }
    
    @JvmStatic
    fun isCallRinging(): Boolean {
        return telephonyManager?.callState == TelephonyManager.CALL_STATE_RINGING
    }
    
    @JvmStatic
    fun isCallActive(): Boolean {
        return telephonyManager?.callState == TelephonyManager.CALL_STATE_OFFHOOK
    }
    
    @JvmStatic
    @Suppress("DEPRECATION")
    fun setOnCallStateChangedListener(callback: (Int, String?) -> Unit) {
        val ctx = context ?: return
        onCallStateChangedCallback = callback
        
        callStateListener = object : PhoneStateListener() {
            override fun onCallStateChanged(state: Int, phoneNumber: String?) {
                callback(state, phoneNumber)
            }
        }
        
        telephonyManager?.listen(callStateListener, PhoneStateListener.LISTEN_CALL_STATE)
    }
    
    @JvmStatic
    @Suppress("DEPRECATION")
    fun removeCallStateListener() {
        callStateListener?.let {
            telephonyManager?.listen(it, PhoneStateListener.LISTEN_NONE)
        }
        callStateListener = null
        onCallStateChangedCallback = null
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Phone Type
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getPhoneType(): String {
        return when (telephonyManager?.phoneType) {
            TelephonyManager.PHONE_TYPE_GSM -> "GSM"
            TelephonyManager.PHONE_TYPE_CDMA -> "CDMA"
            TelephonyManager.PHONE_TYPE_SIP -> "SIP"
            TelephonyManager.PHONE_TYPE_NONE -> "لا يوجد"
            else -> "غير معروف"
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Data State
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getDataState(): String {
        return when (telephonyManager?.dataState) {
            TelephonyManager.DATA_DISCONNECTED -> "غير متصل"
            TelephonyManager.DATA_CONNECTING -> "جاري الاتصال"
            TelephonyManager.DATA_CONNECTED -> "متصل"
            TelephonyManager.DATA_SUSPENDED -> "معلق"
            else -> "غير معروف"
        }
    }
    
    @JvmStatic
    fun isDataConnected(): Boolean {
        return telephonyManager?.dataState == TelephonyManager.DATA_CONNECTED
    }
    
    @JvmStatic
    fun isDataEnabled(): Boolean {
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            telephonyManager?.isDataEnabled == true
        } else {
            isDataConnected()
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Signal Strength
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getSignalStrength(): Map<String, Any?>? {
        val ctx = context ?: return null
        
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.P) {
            return null
        }
        
        if (ContextCompat.checkSelfPermission(ctx, Manifest.permission.READ_PHONE_STATE) 
            != PackageManager.PERMISSION_GRANTED) {
            return null
        }
        
        return try {
            val signalStrength = telephonyManager?.signalStrength
            
            mapOf(
                "level" to signalStrength?.level,
                "dbm" to getDbm(signalStrength),
                "levelDescription" to getSignalLevelDescription(signalStrength?.level ?: 0)
            )
        } catch (e: SecurityException) {
            null
        }
    }
    
    private fun getDbm(signalStrength: SignalStrength?): Int? {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            signalStrength?.cellSignalStrengths?.firstOrNull()?.let {
                return it.dbm
            }
        }
        return null
    }
    
    private fun getSignalLevelDescription(level: Int): String {
        return when (level) {
            0 -> "لا توجد إشارة"
            1 -> "ضعيفة"
            2 -> "متوسطة"
            3 -> "جيدة"
            4 -> "ممتازة"
            else -> "غير معروفة"
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Complete Info
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getTelephonyInfo(): Map<String, Any?> {
        return mapOf(
            "operatorName" to getNetworkOperatorName(),
            "operatorCode" to getNetworkOperatorCode(),
            "simOperatorName" to getSimOperatorName(),
            "simState" to getSimState(),
            "networkCountry" to getNetworkCountryIso(),
            "simCountry" to getSimCountryIso(),
            "networkType" to getNetworkType(),
            "networkGeneration" to getNetworkGeneration(),
            "phoneType" to getPhoneType(),
            "dataState" to getDataState(),
            "callState" to getCallState(),
            "isSimReady" to isSimReady(),
            "isDataConnected" to isDataConnected(),
            "signalStrength" to getSignalStrength()
        )
    }
}
