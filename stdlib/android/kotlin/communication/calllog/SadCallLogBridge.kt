package sad.android.communication.calllog

import android.Manifest
import android.content.ContentResolver
import android.content.Context
import android.content.pm.PackageManager
import android.database.Cursor
import android.provider.CallLog
import androidx.core.content.ContextCompat
import java.text.SimpleDateFormat
import java.util.*

/**
 * جسر سجل المكالمات - قراءة وحذف سجل المكالمات
 * Call Log Bridge - Read and delete call history
 * 
 * @author فريق لغة ص
 */
object SadCallLogBridge {
    
    /**
     * نوع المكالمة / Call type
     */
    enum class CallType(val value: Int) {
        INCOMING(CallLog.Calls.INCOMING_TYPE),
        OUTGOING(CallLog.Calls.OUTGOING_TYPE),
        MISSED(CallLog.Calls.MISSED_TYPE),
        VOICEMAIL(CallLog.Calls.VOICEMAIL_TYPE),
        REJECTED(CallLog.Calls.REJECTED_TYPE),
        BLOCKED(CallLog.Calls.BLOCKED_TYPE)
    }
    
    /**
     * سجل مكالمة / Call record
     */
    data class CallRecord(
        val id: Long,
        val number: String,
        val name: String?,
        val type: CallType,
        val date: Long,
        val duration: Long,      // بالثواني
        val isNew: Boolean,
        val formattedDate: String,
        val formattedDuration: String
    )
    
    /**
     * فلتر البحث / Search filter
     */
    data class CallLogFilter(
        val types: List<CallType>? = null,
        val number: String? = null,
        val afterDate: Long? = null,
        val beforeDate: Long? = null,
        val minDuration: Int? = null,
        val limit: Int = 100
    )
    
    // ═══════════════════════════════════════════════════════════════
    //                         القراءة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * جلب جميع المكالمات / Get all calls
     */
    fun getAllCalls(context: Context, limit: Int = 100): List<CallRecord> {
        return getCalls(context, CallLogFilter(limit = limit))
    }
    
    /**
     * جلب المكالمات الواردة / Get incoming calls
     */
    fun getIncomingCalls(context: Context, limit: Int = 50): List<CallRecord> {
        return getCalls(context, CallLogFilter(types = listOf(CallType.INCOMING), limit = limit))
    }
    
    /**
     * جلب المكالمات الصادرة / Get outgoing calls
     */
    fun getOutgoingCalls(context: Context, limit: Int = 50): List<CallRecord> {
        return getCalls(context, CallLogFilter(types = listOf(CallType.OUTGOING), limit = limit))
    }
    
    /**
     * جلب المكالمات الفائتة / Get missed calls
     */
    fun getMissedCalls(context: Context, limit: Int = 50): List<CallRecord> {
        return getCalls(context, CallLogFilter(types = listOf(CallType.MISSED), limit = limit))
    }
    
    /**
     * جلب المكالمات المرفوضة / Get rejected calls
     */
    fun getRejectedCalls(context: Context, limit: Int = 50): List<CallRecord> {
        return getCalls(context, CallLogFilter(types = listOf(CallType.REJECTED), limit = limit))
    }
    
    /**
     * جلب مكالمات رقم معين / Get calls for specific number
     */
    fun getCallsForNumber(context: Context, number: String, limit: Int = 50): List<CallRecord> {
        return getCalls(context, CallLogFilter(number = number, limit = limit))
    }
    
    /**
     * جلب مكالمات اليوم / Get today's calls
     */
    fun getTodayCalls(context: Context): List<CallRecord> {
        val calendar = Calendar.getInstance()
        calendar.set(Calendar.HOUR_OF_DAY, 0)
        calendar.set(Calendar.MINUTE, 0)
        calendar.set(Calendar.SECOND, 0)
        
        return getCalls(context, CallLogFilter(afterDate = calendar.timeInMillis))
    }
    
    /**
     * جلب مكالمات باستخدام فلتر / Get calls with filter
     */
    fun getCalls(context: Context, filter: CallLogFilter): List<CallRecord> {
        if (!hasPermission(context)) return emptyList()
        
        val calls = mutableListOf<CallRecord>()
        val selection = buildSelection(filter)
        val selectionArgs = buildSelectionArgs(filter)
        
        val cursor: Cursor? = context.contentResolver.query(
            CallLog.Calls.CONTENT_URI,
            arrayOf(
                CallLog.Calls._ID,
                CallLog.Calls.NUMBER,
                CallLog.Calls.CACHED_NAME,
                CallLog.Calls.TYPE,
                CallLog.Calls.DATE,
                CallLog.Calls.DURATION,
                CallLog.Calls.NEW
            ),
            selection,
            selectionArgs,
            "${CallLog.Calls.DATE} DESC"
        )
        
        cursor?.use {
            var count = 0
            while (it.moveToNext() && count < filter.limit) {
                val typeValue = it.getInt(it.getColumnIndexOrThrow(CallLog.Calls.TYPE))
                val callType = CallType.values().find { t -> t.value == typeValue } ?: CallType.INCOMING
                
                val date = it.getLong(it.getColumnIndexOrThrow(CallLog.Calls.DATE))
                val duration = it.getLong(it.getColumnIndexOrThrow(CallLog.Calls.DURATION))
                
                calls.add(CallRecord(
                    id = it.getLong(it.getColumnIndexOrThrow(CallLog.Calls._ID)),
                    number = it.getString(it.getColumnIndexOrThrow(CallLog.Calls.NUMBER)) ?: "",
                    name = it.getString(it.getColumnIndexOrThrow(CallLog.Calls.CACHED_NAME)),
                    type = callType,
                    date = date,
                    duration = duration,
                    isNew = it.getInt(it.getColumnIndexOrThrow(CallLog.Calls.NEW)) == 1,
                    formattedDate = formatDate(date),
                    formattedDuration = formatDuration(duration)
                ))
                count++
            }
        }
        
        return calls
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الإحصائيات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * عدد المكالمات الفائتة غير المقروءة / Count unread missed calls
     */
    fun getUnreadMissedCallsCount(context: Context): Int {
        if (!hasPermission(context)) return 0
        
        val cursor = context.contentResolver.query(
            CallLog.Calls.CONTENT_URI,
            arrayOf("COUNT(*) as count"),
            "${CallLog.Calls.TYPE} = ? AND ${CallLog.Calls.NEW} = 1",
            arrayOf(CallLog.Calls.MISSED_TYPE.toString()),
            null
        )
        
        return cursor?.use {
            if (it.moveToFirst()) it.getInt(0) else 0
        } ?: 0
    }
    
    /**
     * إجمالي مدة المكالمات / Total call duration
     */
    fun getTotalCallDuration(context: Context, type: CallType? = null): Long {
        if (!hasPermission(context)) return 0
        
        val selection = type?.let { "${CallLog.Calls.TYPE} = ?" }
        val selectionArgs = type?.let { arrayOf(it.value.toString()) }
        
        val cursor = context.contentResolver.query(
            CallLog.Calls.CONTENT_URI,
            arrayOf("SUM(${CallLog.Calls.DURATION}) as total"),
            selection,
            selectionArgs,
            null
        )
        
        return cursor?.use {
            if (it.moveToFirst()) it.getLong(0) else 0
        } ?: 0
    }
    
    /**
     * عدد المكالمات حسب النوع / Count calls by type
     */
    fun getCallCountByType(context: Context): Map<CallType, Int> {
        if (!hasPermission(context)) return emptyMap()
        
        val counts = mutableMapOf<CallType, Int>()
        CallType.values().forEach { type ->
            val cursor = context.contentResolver.query(
                CallLog.Calls.CONTENT_URI,
                arrayOf("COUNT(*) as count"),
                "${CallLog.Calls.TYPE} = ?",
                arrayOf(type.value.toString()),
                null
            )
            
            cursor?.use {
                if (it.moveToFirst()) counts[type] = it.getInt(0)
            }
        }
        
        return counts
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الحذف
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * حذف سجل مكالمة / Delete call record
     */
    fun deleteCall(context: Context, callId: Long): Boolean {
        if (!hasPermission(context)) return false
        
        return context.contentResolver.delete(
            CallLog.Calls.CONTENT_URI,
            "${CallLog.Calls._ID} = ?",
            arrayOf(callId.toString())
        ) > 0
    }
    
    /**
     * حذف مكالمات رقم معين / Delete calls for number
     */
    fun deleteCallsForNumber(context: Context, number: String): Int {
        if (!hasPermission(context)) return 0
        
        return context.contentResolver.delete(
            CallLog.Calls.CONTENT_URI,
            "${CallLog.Calls.NUMBER} = ?",
            arrayOf(number)
        )
    }
    
    /**
     * حذف جميع سجلات المكالمات / Delete all call logs
     */
    fun deleteAllCalls(context: Context): Int {
        if (!hasPermission(context)) return 0
        
        return context.contentResolver.delete(
            CallLog.Calls.CONTENT_URI,
            null,
            null
        )
    }
    
    /**
     * حذف مكالمات قبل تاريخ معين / Delete calls before date
     */
    fun deleteCallsBeforeDate(context: Context, date: Long): Int {
        if (!hasPermission(context)) return 0
        
        return context.contentResolver.delete(
            CallLog.Calls.CONTENT_URI,
            "${CallLog.Calls.DATE} < ?",
            arrayOf(date.toString())
        )
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         تحديث
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * وضع علامة مقروء على المكالمات الفائتة / Mark missed calls as read
     */
    fun markMissedCallsAsRead(context: Context): Int {
        if (!hasPermission(context)) return 0
        
        val values = android.content.ContentValues().apply {
            put(CallLog.Calls.NEW, 0)
        }
        
        return context.contentResolver.update(
            CallLog.Calls.CONTENT_URI,
            values,
            "${CallLog.Calls.TYPE} = ? AND ${CallLog.Calls.NEW} = 1",
            arrayOf(CallLog.Calls.MISSED_TYPE.toString())
        )
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الأذونات
    // ═══════════════════════════════════════════════════════════════
    
    fun hasPermission(context: Context): Boolean {
        return ContextCompat.checkSelfPermission(
            context,
            Manifest.permission.READ_CALL_LOG
        ) == PackageManager.PERMISSION_GRANTED
    }
    
    fun hasWritePermission(context: Context): Boolean {
        return ContextCompat.checkSelfPermission(
            context,
            Manifest.permission.WRITE_CALL_LOG
        ) == PackageManager.PERMISSION_GRANTED
    }
    
    fun requiredPermissions(): Array<String> = arrayOf(
        Manifest.permission.READ_CALL_LOG,
        Manifest.permission.WRITE_CALL_LOG
    )
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    private fun buildSelection(filter: CallLogFilter): String? {
        val conditions = mutableListOf<String>()
        
        filter.types?.let {
            val typeValues = it.map { t -> t.value.toString() }
            conditions.add("${CallLog.Calls.TYPE} IN (${typeValues.joinToString(",")})")
        }
        
        filter.number?.let {
            conditions.add("${CallLog.Calls.NUMBER} LIKE ?")
        }
        
        filter.afterDate?.let {
            conditions.add("${CallLog.Calls.DATE} >= ?")
        }
        
        filter.beforeDate?.let {
            conditions.add("${CallLog.Calls.DATE} <= ?")
        }
        
        filter.minDuration?.let {
            conditions.add("${CallLog.Calls.DURATION} >= ?")
        }
        
        return if (conditions.isEmpty()) null else conditions.joinToString(" AND ")
    }
    
    private fun buildSelectionArgs(filter: CallLogFilter): Array<String>? {
        val args = mutableListOf<String>()
        
        filter.number?.let { args.add("%$it%") }
        filter.afterDate?.let { args.add(it.toString()) }
        filter.beforeDate?.let { args.add(it.toString()) }
        filter.minDuration?.let { args.add(it.toString()) }
        
        return if (args.isEmpty()) null else args.toTypedArray()
    }
    
    private fun formatDate(timestamp: Long): String {
        return SimpleDateFormat("yyyy/MM/dd HH:mm", Locale.getDefault()).format(Date(timestamp))
    }
    
    private fun formatDuration(seconds: Long): String {
        val hours = seconds / 3600
        val minutes = (seconds % 3600) / 60
        val secs = seconds % 60
        
        return when {
            hours > 0 -> String.format("%d:%02d:%02d", hours, minutes, secs)
            else -> String.format("%d:%02d", minutes, secs)
        }
    }
}
