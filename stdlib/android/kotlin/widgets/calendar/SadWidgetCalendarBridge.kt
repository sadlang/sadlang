package sad.android.widgets.calendar

import android.content.ContentUris
import android.content.Context
import android.database.Cursor
import android.net.Uri
import android.os.Handler
import android.os.Looper
import android.provider.CalendarContract
import java.text.SimpleDateFormat
import java.util.*
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors

/**
 * جسر ويدجت التقويم - عرض الأحداث والمواعيد
 * Calendar Widget Bridge - Events and appointments display
 * 
 * يتطلب إذن READ_CALENDAR
 * 
 * @author فريق لغة ص
 */
object SadWidgetCalendarBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private val executor: ExecutorService = Executors.newSingleThreadExecutor()
    
    /**
     * حدث التقويم / Calendar event
     */
    data class CalendarEvent(
        val id: Long,
        val title: String,
        val description: String,
        val location: String,
        val startTime: Long,
        val endTime: Long,
        val allDay: Boolean,
        val calendarId: Long,
        val calendarName: String,
        val calendarColor: Int,
        val eventColor: Int?
    )
    
    /**
     * معلومات التقويم / Calendar info
     */
    data class CalendarInfo(
        val id: Long,
        val name: String,
        val accountName: String,
        val ownerAccount: String,
        val color: Int,
        val visible: Boolean
    )
    
    /**
     * معلومات اليوم / Day info
     */
    data class DayInfo(
        val date: Date,
        val dayOfMonth: Int,
        val dayOfWeek: Int,
        val month: Int,
        val year: Int,
        val isToday: Boolean,
        val isWeekend: Boolean,
        val events: List<CalendarEvent>
    )
    
    /**
     * رد نداء الأحداث / Events callback
     */
    interface CalendarCallback {
        fun onEventsLoaded(events: List<CalendarEvent>)
        fun onCalendarsLoaded(calendars: List<CalendarInfo>)
        fun onError(error: String)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         جلب الأحداث
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * جلب أحداث اليوم / Get today's events
     */
    fun getTodayEvents(callback: CalendarCallback) {
        val calendar = Calendar.getInstance()
        calendar.set(Calendar.HOUR_OF_DAY, 0)
        calendar.set(Calendar.MINUTE, 0)
        calendar.set(Calendar.SECOND, 0)
        val startOfDay = calendar.timeInMillis
        
        calendar.set(Calendar.HOUR_OF_DAY, 23)
        calendar.set(Calendar.MINUTE, 59)
        calendar.set(Calendar.SECOND, 59)
        val endOfDay = calendar.timeInMillis
        
        getEventsBetween(startOfDay, endOfDay, callback)
    }
    
    /**
     * جلب أحداث الأسبوع / Get this week's events
     */
    fun getWeekEvents(callback: CalendarCallback) {
        val calendar = Calendar.getInstance()
        calendar.set(Calendar.DAY_OF_WEEK, calendar.firstDayOfWeek)
        calendar.set(Calendar.HOUR_OF_DAY, 0)
        calendar.set(Calendar.MINUTE, 0)
        val startOfWeek = calendar.timeInMillis
        
        calendar.add(Calendar.DAY_OF_WEEK, 7)
        val endOfWeek = calendar.timeInMillis
        
        getEventsBetween(startOfWeek, endOfWeek, callback)
    }
    
    /**
     * جلب الأحداث القادمة / Get upcoming events
     */
    fun getUpcomingEvents(days: Int = 7, callback: CalendarCallback) {
        val now = System.currentTimeMillis()
        val calendar = Calendar.getInstance()
        calendar.add(Calendar.DAY_OF_YEAR, days)
        val future = calendar.timeInMillis
        
        getEventsBetween(now, future, callback)
    }
    
    /**
     * جلب أحداث بين تاريخين / Get events between dates
     */
    fun getEventsBetween(startMs: Long, endMs: Long, callback: CalendarCallback) {
        val ctx = context
        if (ctx == null) {
            callback.onError("السياق غير متاح")
            return
        }
        
        executor.execute {
            try {
                val events = queryEvents(ctx, startMs, endMs)
                mainHandler.post { callback.onEventsLoaded(events) }
            } catch (e: Exception) {
                mainHandler.post { callback.onError(e.message ?: "خطأ في جلب الأحداث") }
            }
        }
    }
    
    private fun queryEvents(ctx: Context, startMs: Long, endMs: Long): List<CalendarEvent> {
        val events = mutableListOf<CalendarEvent>()
        
        val uri = CalendarContract.Instances.CONTENT_URI.buildUpon()
            .appendPath(startMs.toString())
            .appendPath(endMs.toString())
            .build()
        
        val projection = arrayOf(
            CalendarContract.Instances.EVENT_ID,
            CalendarContract.Instances.TITLE,
            CalendarContract.Instances.DESCRIPTION,
            CalendarContract.Instances.EVENT_LOCATION,
            CalendarContract.Instances.BEGIN,
            CalendarContract.Instances.END,
            CalendarContract.Instances.ALL_DAY,
            CalendarContract.Instances.CALENDAR_ID,
            CalendarContract.Instances.CALENDAR_DISPLAY_NAME,
            CalendarContract.Instances.CALENDAR_COLOR,
            CalendarContract.Instances.DISPLAY_COLOR
        )
        
        val cursor = ctx.contentResolver.query(
            uri,
            projection,
            null,
            null,
            "${CalendarContract.Instances.BEGIN} ASC"
        )
        
        cursor?.use { c ->
            while (c.moveToNext()) {
                events.add(
                    CalendarEvent(
                        id = c.getLong(0),
                        title = c.getString(1) ?: "",
                        description = c.getString(2) ?: "",
                        location = c.getString(3) ?: "",
                        startTime = c.getLong(4),
                        endTime = c.getLong(5),
                        allDay = c.getInt(6) == 1,
                        calendarId = c.getLong(7),
                        calendarName = c.getString(8) ?: "",
                        calendarColor = c.getInt(9),
                        eventColor = c.getInt(10)
                    )
                )
            }
        }
        
        return events
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التقويمات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * جلب جميع التقويمات / Get all calendars
     */
    fun getCalendars(callback: CalendarCallback) {
        val ctx = context
        if (ctx == null) {
            callback.onError("السياق غير متاح")
            return
        }
        
        executor.execute {
            try {
                val calendars = queryCalendars(ctx)
                mainHandler.post { callback.onCalendarsLoaded(calendars) }
            } catch (e: Exception) {
                mainHandler.post { callback.onError(e.message ?: "خطأ في جلب التقويمات") }
            }
        }
    }
    
    private fun queryCalendars(ctx: Context): List<CalendarInfo> {
        val calendars = mutableListOf<CalendarInfo>()
        
        val projection = arrayOf(
            CalendarContract.Calendars._ID,
            CalendarContract.Calendars.CALENDAR_DISPLAY_NAME,
            CalendarContract.Calendars.ACCOUNT_NAME,
            CalendarContract.Calendars.OWNER_ACCOUNT,
            CalendarContract.Calendars.CALENDAR_COLOR,
            CalendarContract.Calendars.VISIBLE
        )
        
        val cursor = ctx.contentResolver.query(
            CalendarContract.Calendars.CONTENT_URI,
            projection,
            null,
            null,
            null
        )
        
        cursor?.use { c ->
            while (c.moveToNext()) {
                calendars.add(
                    CalendarInfo(
                        id = c.getLong(0),
                        name = c.getString(1) ?: "",
                        accountName = c.getString(2) ?: "",
                        ownerAccount = c.getString(3) ?: "",
                        color = c.getInt(4),
                        visible = c.getInt(5) == 1
                    )
                )
            }
        }
        
        return calendars
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معلومات الشهر
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على أيام الشهر / Get month days
     */
    fun getMonthDays(year: Int, month: Int, callback: (List<DayInfo>) -> Unit) {
        executor.execute {
            val days = mutableListOf<DayInfo>()
            val calendar = Calendar.getInstance()
            val today = Calendar.getInstance()
            
            calendar.set(year, month - 1, 1)
            val daysInMonth = calendar.getActualMaximum(Calendar.DAY_OF_MONTH)
            
            for (day in 1..daysInMonth) {
                calendar.set(Calendar.DAY_OF_MONTH, day)
                val dayOfWeek = calendar.get(Calendar.DAY_OF_WEEK)
                
                val isToday = calendar.get(Calendar.YEAR) == today.get(Calendar.YEAR) &&
                        calendar.get(Calendar.MONTH) == today.get(Calendar.MONTH) &&
                        calendar.get(Calendar.DAY_OF_MONTH) == today.get(Calendar.DAY_OF_MONTH)
                
                days.add(
                    DayInfo(
                        date = calendar.time,
                        dayOfMonth = day,
                        dayOfWeek = dayOfWeek,
                        month = month,
                        year = year,
                        isToday = isToday,
                        isWeekend = dayOfWeek == Calendar.FRIDAY || dayOfWeek == Calendar.SATURDAY,
                        events = emptyList()
                    )
                )
            }
            
            mainHandler.post { callback(days) }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التنسيق
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تنسيق وقت الحدث / Format event time
     */
    fun formatEventTime(event: CalendarEvent, locale: Locale = Locale("ar")): String {
        return if (event.allDay) {
            "طوال اليوم"
        } else {
            val formatter = SimpleDateFormat("HH:mm", locale)
            "${formatter.format(Date(event.startTime))} - ${formatter.format(Date(event.endTime))}"
        }
    }
    
    /**
     * تنسيق تاريخ الحدث / Format event date
     */
    fun formatEventDate(timestamp: Long, locale: Locale = Locale("ar")): String {
        val formatter = SimpleDateFormat("EEEE, d MMMM", locale)
        return formatter.format(Date(timestamp))
    }
    
    /**
     * الحصول على الوقت المتبقي / Get time remaining
     */
    fun getTimeUntilEvent(event: CalendarEvent): String {
        val now = System.currentTimeMillis()
        val diff = event.startTime - now
        
        if (diff < 0) return "بدأ"
        
        val minutes = diff / (1000 * 60)
        val hours = minutes / 60
        val days = hours / 24
        
        return when {
            days > 0 -> "بعد $days يوم"
            hours > 0 -> "بعد $hours ساعة"
            minutes > 0 -> "بعد $minutes دقيقة"
            else -> "الآن"
        }
    }
    
    /**
     * هل الحدث يحدث الآن؟ / Is event happening now?
     */
    fun isEventNow(event: CalendarEvent): Boolean {
        val now = System.currentTimeMillis()
        return now >= event.startTime && now <= event.endTime
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        executor.shutdown()
        context = null
    }
}
