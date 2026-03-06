package sad.android.communication.ussd

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.os.Build
import android.telephony.TelephonyManager
import androidx.annotation.RequiresApi
import androidx.core.content.ContextCompat

/**
 * جسر USSD - أكواد الخدمة السريعة
 * USSD Bridge - Unstructured Supplementary Service Data
 * 
 * @author فريق لغة ص
 */
object SadUSSDCodeBridge {
    
    /**
     * نتيجة USSD / USSD result
     */
    sealed class USSDResult {
        data class Success(val response: String) : USSDResult()
        data class Error(val errorCode: Int, val message: String) : USSDResult()
        object Cancelled : USSDResult()
    }
    
    /**
     * أكواد USSD الشائعة / Common USSD codes
     */
    object CommonCodes {
        const val CHECK_BALANCE = "*100#"
        const val CHECK_NUMBER = "*222#"
        const val IMEI = "*#06#"
        const val CALL_FORWARDING_STATUS = "*#21#"
        const val DISABLE_CALL_FORWARDING = "##21#"
        const val CALL_WAITING_STATUS = "*#43#"
        const val ENABLE_CALL_WAITING = "*43#"
        const val DISABLE_CALL_WAITING = "#43#"
    }
    
    /**
     * مستمع USSD / USSD callback
     */
    interface USSDCallback {
        fun onReceiveUssdResponse(response: String)
        fun onReceiveUssdError(errorCode: Int, errorMessage: String)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         تنفيذ USSD
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تنفيذ كود USSD / Execute USSD code
     */
    @RequiresApi(Build.VERSION_CODES.O)
    fun sendUSSDRequest(
        context: Context,
        ussdCode: String,
        callback: USSDCallback
    ): Boolean {
        if (!hasPermission(context)) return false
        
        val telephonyManager = context.getSystemService(Context.TELEPHONY_SERVICE) as TelephonyManager
        
        val ussdCallback = object : TelephonyManager.UssdResponseCallback() {
            override fun onReceiveUssdResponse(
                telephonyManager: TelephonyManager,
                request: String,
                response: CharSequence
            ) {
                callback.onReceiveUssdResponse(response.toString())
            }
            
            override fun onReceiveUssdResponseFailed(
                telephonyManager: TelephonyManager,
                request: String,
                failureCode: Int
            ) {
                val errorMessage = when (failureCode) {
                    TelephonyManager.USSD_RETURN_FAILURE -> "فشل USSD"
                    TelephonyManager.USSD_ERROR_SERVICE_UNAVAIL -> "الخدمة غير متاحة"
                    else -> "خطأ غير معروف"
                }
                callback.onReceiveUssdError(failureCode, errorMessage)
            }
        }
        
        try {
            telephonyManager.sendUssdRequest(ussdCode, ussdCallback, null)
            return true
        } catch (e: SecurityException) {
            callback.onReceiveUssdError(-1, "لا توجد صلاحية")
            return false
        } catch (e: Exception) {
            callback.onReceiveUssdError(-2, e.message ?: "خطأ")
            return false
        }
    }
    
    /**
     * تنفيذ عبر الاتصال (طريقة قديمة) / Execute via dialer (legacy)
     */
    fun dialUSSD(context: Context, ussdCode: String) {
        val encodedCode = android.net.Uri.encode(ussdCode)
        val intent = android.content.Intent(
            android.content.Intent.ACTION_CALL,
            android.net.Uri.parse("tel:$encodedCode")
        )
        
        if (hasCallPermission(context)) {
            intent.addFlags(android.content.Intent.FLAG_ACTIVITY_NEW_TASK)
            context.startActivity(intent)
        }
    }
    
    /**
     * فتح الاتصال مع USSD (بدون اتصال تلقائي) / Open dialer with USSD
     */
    fun openDialerWithUSSD(context: Context, ussdCode: String) {
        val encodedCode = android.net.Uri.encode(ussdCode)
        val intent = android.content.Intent(
            android.content.Intent.ACTION_DIAL,
            android.net.Uri.parse("tel:$encodedCode")
        )
        intent.addFlags(android.content.Intent.FLAG_ACTIVITY_NEW_TASK)
        context.startActivity(intent)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         أكواد محددة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * التحقق من الرصيد / Check balance
     */
    @RequiresApi(Build.VERSION_CODES.O)
    fun checkBalance(context: Context, callback: USSDCallback): Boolean {
        return sendUSSDRequest(context, CommonCodes.CHECK_BALANCE, callback)
    }
    
    /**
     * جلب IMEI / Get IMEI
     */
    fun getIMEI(context: Context): String? {
        if (!hasPhoneStatePermission(context)) return null
        
        val telephonyManager = context.getSystemService(Context.TELEPHONY_SERVICE) as TelephonyManager
        
        return try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                telephonyManager.imei
            } else {
                @Suppress("DEPRECATION")
                telephonyManager.deviceId
            }
        } catch (e: SecurityException) {
            null
        }
    }
    
    /**
     * فتح IMEI في الاتصال / Open IMEI in dialer
     */
    fun showIMEI(context: Context) {
        openDialerWithUSSD(context, CommonCodes.IMEI)
    }
    
    /**
     * التحقق من تحويل المكالمات / Check call forwarding
     */
    @RequiresApi(Build.VERSION_CODES.O)
    fun checkCallForwarding(context: Context, callback: USSDCallback): Boolean {
        return sendUSSDRequest(context, CommonCodes.CALL_FORWARDING_STATUS, callback)
    }
    
    /**
     * إلغاء تحويل المكالمات / Disable call forwarding
     */
    @RequiresApi(Build.VERSION_CODES.O)
    fun disableCallForwarding(context: Context, callback: USSDCallback): Boolean {
        return sendUSSDRequest(context, CommonCodes.DISABLE_CALL_FORWARDING, callback)
    }
    
    /**
     * تفعيل انتظار المكالمات / Enable call waiting
     */
    @RequiresApi(Build.VERSION_CODES.O)
    fun enableCallWaiting(context: Context, callback: USSDCallback): Boolean {
        return sendUSSDRequest(context, CommonCodes.ENABLE_CALL_WAITING, callback)
    }
    
    /**
     * تعطيل انتظار المكالمات / Disable call waiting
     */
    @RequiresApi(Build.VERSION_CODES.O)
    fun disableCallWaiting(context: Context, callback: USSDCallback): Boolean {
        return sendUSSDRequest(context, CommonCodes.DISABLE_CALL_WAITING, callback)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         تحويل المكالمات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تحويل جميع المكالمات / Forward all calls
     */
    @RequiresApi(Build.VERSION_CODES.O)
    fun setCallForwardingAll(
        context: Context,
        phoneNumber: String,
        callback: USSDCallback
    ): Boolean {
        val code = "**21*$phoneNumber#"
        return sendUSSDRequest(context, code, callback)
    }
    
    /**
     * تحويل عند عدم الرد / Forward when busy
     */
    @RequiresApi(Build.VERSION_CODES.O)
    fun setCallForwardingBusy(
        context: Context,
        phoneNumber: String,
        callback: USSDCallback
    ): Boolean {
        val code = "**67*$phoneNumber#"
        return sendUSSDRequest(context, code, callback)
    }
    
    /**
     * تحويل عند عدم الرد / Forward when no answer
     */
    @RequiresApi(Build.VERSION_CODES.O)
    fun setCallForwardingNoAnswer(
        context: Context,
        phoneNumber: String,
        seconds: Int = 20,
        callback: USSDCallback
    ): Boolean {
        val code = "**61*$phoneNumber*$seconds#"
        return sendUSSDRequest(context, code, callback)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معلومات الشبكة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * معلومات المشغل / Carrier info
     */
    data class CarrierInfo(
        val operatorName: String?,
        val operatorCode: String?,
        val simOperatorName: String?,
        val networkType: String,
        val countryCode: String?
    )
    
    fun getCarrierInfo(context: Context): CarrierInfo {
        val telephonyManager = context.getSystemService(Context.TELEPHONY_SERVICE) as TelephonyManager
        
        return CarrierInfo(
            operatorName = telephonyManager.networkOperatorName,
            operatorCode = telephonyManager.networkOperator,
            simOperatorName = telephonyManager.simOperatorName,
            networkType = getNetworkTypeName(telephonyManager),
            countryCode = telephonyManager.networkCountryIso
        )
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الأذونات
    // ═══════════════════════════════════════════════════════════════
    
    fun hasPermission(context: Context): Boolean {
        return ContextCompat.checkSelfPermission(
            context,
            Manifest.permission.CALL_PHONE
        ) == PackageManager.PERMISSION_GRANTED
    }
    
    fun hasCallPermission(context: Context): Boolean {
        return hasPermission(context)
    }
    
    fun hasPhoneStatePermission(context: Context): Boolean {
        return ContextCompat.checkSelfPermission(
            context,
            Manifest.permission.READ_PHONE_STATE
        ) == PackageManager.PERMISSION_GRANTED
    }
    
    fun requiredPermissions(): Array<String> = arrayOf(
        Manifest.permission.CALL_PHONE,
        Manifest.permission.READ_PHONE_STATE
    )
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    private fun getNetworkTypeName(tm: TelephonyManager): String {
        return when (tm.networkType) {
            TelephonyManager.NETWORK_TYPE_GPRS -> "GPRS"
            TelephonyManager.NETWORK_TYPE_EDGE -> "EDGE"
            TelephonyManager.NETWORK_TYPE_UMTS -> "UMTS"
            TelephonyManager.NETWORK_TYPE_HSDPA -> "HSDPA"
            TelephonyManager.NETWORK_TYPE_HSUPA -> "HSUPA"
            TelephonyManager.NETWORK_TYPE_HSPA -> "HSPA"
            TelephonyManager.NETWORK_TYPE_LTE -> "LTE"
            TelephonyManager.NETWORK_TYPE_NR -> "5G"
            else -> "Unknown"
        }
    }
}
