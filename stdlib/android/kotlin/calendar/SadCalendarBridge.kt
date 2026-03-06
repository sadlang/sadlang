// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadCalendarBridge.kt
// الوصف: جسر Kotlin للتقويم
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.ContentValues
import android.content.Context
import android.provider.CalendarContract
import java.util.*

/**
 * جسر التقويم — SadCalendarBridge
 */
object SadCalendarBridge {
    
    private var context: Context? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Calendars
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getCalendars(): List<Map<String, Any?>> {
        val ctx = context ?: return emptyList()
        val calendars = mutableListOf<Map<String, Any?>>()
        
        val projection = arrayOf(
            CalendarContract.Calendars._ID,
            CalendarContract.Calendars.NAME,
            CalendarContract.Calendars.CALENDAR_DISPLAY_NAME,
            CalendarContract.Calendars.ACCOUNT_NAME,
            CalendarContract.Calendars.ACCOUNT_TYPE,
            CalendarContract.Calendars.CALENDAR_COLOR,
            CalendarContract.Calendars.VISIBLE,
            CalendarContract.Calendars.IS_PRIMARY
        )
        
        val cursor = ctx.contentResolver.query(
            CalendarContract.Calendars.CONTENT_URI,
            projection,
            null,
            null,
            null
        )
        
        cursor?.use {
            while (it.moveToNext()) {
                calendars.add(mapOf(
                    "id" to it.getLong(0),
                    "name" to it.getString(1),
                    "displayName" to it.getString(2),
                    "accountName" to it.getString(3),
                    "accountType" to it.getString(4),
                    "color" to it.getInt(5),
                    "visible" to (it.getInt(6) == 1),
                    "isPrimary" to (it.getInt(7) == 1)
                ))
            }
        }
        
        return calendars
    }
    
    @JvmStatic
    fun getPrimaryCalendarId(): Long? {
        val calendars = getCalendars()
        return calendars.firstOrNull { it["isPrimary"] == true }?.get("id") as? Long
            ?: calendars.firstOrNull()?.get("id") as? Long
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Events - Query
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getEvents(
        calendarId: Long? = null,
        startTimeMs: Long? = null,
        endTimeMs: Long? = null
    ): List<Map<String, Any?>> {
        val ctx = context ?: return emptyList()
        val events = mutableListOf<Map<String, Any?>>()
        
        val projection = arrayOf(
            CalendarContract.Events._ID,
            CalendarContract.Events.TITLE,
            CalendarContract.Events.DESCRIPTION,
            CalendarContract.Events.DTSTART,
            CalendarContract.Events.DTEND,
            CalendarContract.Events.ALL_DAY,
            CalendarContract.Events.EVENT_LOCATION,
            CalendarContract.Events.CALENDAR_ID,
            CalendarContract.Events.EVENT_COLOR,
            CalendarContract.Events.RRULE,
            CalendarContract.Events.EVENT_TIMEZONE
        )
        
        val selection = StringBuilder()
        val selectionArgs = mutableListOf<String>()
        
        calendarId?.let {
            selection.append("${CalendarContract.Events.CALENDAR_ID} = ?")
            selectionArgs.add(it.toString())
        }
        
        startTimeMs?.let {
            if (selection.isNotEmpty()) selection.append(" AND ")
            selection.append("${CalendarContract.Events.DTSTART} >= ?")
            selectionArgs.add(it.toString())
        }
        
        endTimeMs?.let {
            if (selection.isNotEmpty()) selection.append(" AND ")
            selection.append("${CalendarContract.Events.DTEND} <= ?")
            selectionArgs.add(it.toString())
        }
        
        val cursor = ctx.contentResolver.query(
            CalendarContract.Events.CONTENT_URI,
            projection,
            if (selection.isNotEmpty()) selection.toString() else null,
            if (selectionArgs.isNotEmpty()) selectionArgs.toTypedArray() else null,
            "${CalendarContract.Events.DTSTART} ASC"
        )
        
        cursor?.use {
            while (it.moveToNext()) {
                events.add(mapOf(
                    "id" to it.getLong(0),
                    "title" to it.getString(1),
                    "description" to it.getString(2),
                    "startTime" to it.getLong(3),
                    "endTime" to it.getLong(4),
                    "allDay" to (it.getInt(5) == 1),
                    "location" to it.getString(6),
                    "calendarId" to it.getLong(7),
                    "color" to it.getInt(8),
                    "rrule" to it.getString(9),
                    "timezone" to it.getString(10)
                ))
            }
        }
        
        return events
    }
    
    @JvmStatic
    fun getTodayEvents(calendarId: Long? = null): List<Map<String, Any?>> {
        val calendar = Calendar.getInstance()
        calendar.set(Calendar.HOUR_OF_DAY, 0)
        calendar.set(Calendar.MINUTE, 0)
        calendar.set(Calendar.SECOND, 0)
        calendar.set(Calendar.MILLISECOND, 0)
        val startOfDay = calendar.timeInMillis
        
        calendar.add(Calendar.DAY_OF_MONTH, 1)
        val endOfDay = calendar.timeInMillis
        
        return getEvents(calendarId, startOfDay, endOfDay)
    }
    
    @JvmStatic
    fun getUpcomingEvents(calendarId: Long? = null, days: Int = 7): List<Map<String, Any?>> {
        val now = System.currentTimeMillis()
        val calendar = Calendar.getInstance()
        calendar.add(Calendar.DAY_OF_MONTH, days)
        val endTime = calendar.timeInMillis
        
        return getEvents(calendarId, now, endTime)
    }
    
    @JvmStatic
    fun getEventById(eventId: Long): Map<String, Any?>? {
        val ctx = context ?: return null
        
        val projection = arrayOf(
            CalendarContract.Events._ID,
            CalendarContract.Events.TITLE,
            CalendarContract.Events.DESCRIPTION,
            CalendarContract.Events.DTSTART,
            CalendarContract.Events.DTEND,
            CalendarContract.Events.ALL_DAY,
            CalendarContract.Events.EVENT_LOCATION,
            CalendarContract.Events.CALENDAR_ID,
            CalendarContract.Events.RRULE,
            CalendarContract.Events.EVENT_TIMEZONE
        )
        
        val cursor = ctx.contentResolver.query(
            CalendarContract.Events.CONTENT_URI,
            projection,
            "${CalendarContract.Events._ID} = ?",
            arrayOf(eventId.toString()),
            null
        )
        
        cursor?.use {
            if (it.moveToFirst()) {
                return mapOf(
                    "id" to it.getLong(0),
                    "title" to it.getString(1),
                    "description" to it.getString(2),
                    "startTime" to it.getLong(3),
                    "endTime" to it.getLong(4),
                    "allDay" to (it.getInt(5) == 1),
                    "location" to it.getString(6),
                    "calendarId" to it.getLong(7),
                    "rrule" to it.getString(8),
                    "timezone" to it.getString(9)
                )
            }
        }
        
        return null
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Events - Create
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun addEvent(
        title: String,
        startTimeMs: Long,
        endTimeMs: Long,
        description: String? = null,
        location: String? = null,
        allDay: Boolean = false,
        calendarId: Long? = null,
        timezone: String? = null
    ): Long? {
        val ctx = context ?: return null
        
        val calId = calendarId ?: getPrimaryCalendarId() ?: return null
        val tz = timezone ?: TimeZone.getDefault().id
        
        val values = ContentValues().apply {
            put(CalendarContract.Events.CALENDAR_ID, calId)
            put(CalendarContract.Events.TITLE, title)
            put(CalendarContract.Events.DTSTART, startTimeMs)
            put(CalendarContract.Events.DTEND, endTimeMs)
            put(CalendarContract.Events.EVENT_TIMEZONE, tz)
            put(CalendarContract.Events.ALL_DAY, if (allDay) 1 else 0)
            description?.let { put(CalendarContract.Events.DESCRIPTION, it) }
            location?.let { put(CalendarContract.Events.EVENT_LOCATION, it) }
        }
        
        val uri = ctx.contentResolver.insert(CalendarContract.Events.CONTENT_URI, values)
        return uri?.lastPathSegment?.toLongOrNull()
    }
    
    @JvmStatic
    fun addAllDayEvent(
        title: String,
        dateMs: Long,
        description: String? = null,
        location: String? = null,
        calendarId: Long? = null
    ): Long? {
        val calendar = Calendar.getInstance()
        calendar.timeInMillis = dateMs
        calendar.set(Calendar.HOUR_OF_DAY, 0)
        calendar.set(Calendar.MINUTE, 0)
        calendar.set(Calendar.SECOND, 0)
        calendar.set(Calendar.MILLISECOND, 0)
        val startOfDay = calendar.timeInMillis
        
        calendar.add(Calendar.DAY_OF_MONTH, 1)
        val endOfDay = calendar.timeInMillis
        
        return addEvent(
            title = title,
            startTimeMs = startOfDay,
            endTimeMs = endOfDay,
            description = description,
            location = location,
            allDay = true,
            calendarId = calendarId,
            timezone = "UTC"
        )
    }
    
    @JvmStatic
    fun addRecurringEvent(
        title: String,
        startTimeMs: Long,
        endTimeMs: Long,
        rrule: String,
        description: String? = null,
        location: String? = null,
        calendarId: Long? = null
    ): Long? {
        val ctx = context ?: return null
        
        val calId = calendarId ?: getPrimaryCalendarId() ?: return null
        val tz = TimeZone.getDefault().id
        
        val values = ContentValues().apply {
            put(CalendarContract.Events.CALENDAR_ID, calId)
            put(CalendarContract.Events.TITLE, title)
            put(CalendarContract.Events.DTSTART, startTimeMs)
            put(CalendarContract.Events.DTEND, endTimeMs)
            put(CalendarContract.Events.EVENT_TIMEZONE, tz)
            put(CalendarContract.Events.RRULE, rrule)
            description?.let { put(CalendarContract.Events.DESCRIPTION, it) }
            location?.let { put(CalendarContract.Events.EVENT_LOCATION, it) }
        }
        
        val uri = ctx.contentResolver.insert(CalendarContract.Events.CONTENT_URI, values)
        return uri?.lastPathSegment?.toLongOrNull()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Events - Update
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun updateEvent(
        eventId: Long,
        title: String? = null,
        startTimeMs: Long? = null,
        endTimeMs: Long? = null,
        description: String? = null,
        location: String? = null
    ): Boolean {
        val ctx = context ?: return false
        
        val values = ContentValues()
        title?.let { values.put(CalendarContract.Events.TITLE, it) }
        startTimeMs?.let { values.put(CalendarContract.Events.DTSTART, it) }
        endTimeMs?.let { values.put(CalendarContract.Events.DTEND, it) }
        description?.let { values.put(CalendarContract.Events.DESCRIPTION, it) }
        location?.let { values.put(CalendarContract.Events.EVENT_LOCATION, it) }
        
        if (values.size() == 0) return false
        
        val rows = ctx.contentResolver.update(
            CalendarContract.Events.CONTENT_URI,
            values,
            "${CalendarContract.Events._ID} = ?",
            arrayOf(eventId.toString())
        )
        
        return rows > 0
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Events - Delete
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun deleteEvent(eventId: Long): Boolean {
        val ctx = context ?: return false
        
        val rows = ctx.contentResolver.delete(
            CalendarContract.Events.CONTENT_URI,
            "${CalendarContract.Events._ID} = ?",
            arrayOf(eventId.toString())
        )
        
        return rows > 0
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Reminders
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun addReminder(eventId: Long, minutesBefore: Int): Long? {
        val ctx = context ?: return null
        
        val values = ContentValues().apply {
            put(CalendarContract.Reminders.EVENT_ID, eventId)
            put(CalendarContract.Reminders.MINUTES, minutesBefore)
            put(CalendarContract.Reminders.METHOD, CalendarContract.Reminders.METHOD_ALERT)
        }
        
        val uri = ctx.contentResolver.insert(CalendarContract.Reminders.CONTENT_URI, values)
        return uri?.lastPathSegment?.toLongOrNull()
    }
    
    @JvmStatic
    fun getReminders(eventId: Long): List<Map<String, Any?>> {
        val ctx = context ?: return emptyList()
        val reminders = mutableListOf<Map<String, Any?>>()
        
        val cursor = ctx.contentResolver.query(
            CalendarContract.Reminders.CONTENT_URI,
            arrayOf(
                CalendarContract.Reminders._ID,
                CalendarContract.Reminders.MINUTES,
                CalendarContract.Reminders.METHOD
            ),
            "${CalendarContract.Reminders.EVENT_ID} = ?",
            arrayOf(eventId.toString()),
            null
        )
        
        cursor?.use {
            while (it.moveToNext()) {
                reminders.add(mapOf(
                    "id" to it.getLong(0),
                    "minutes" to it.getInt(1),
                    "method" to getReminderMethodName(it.getInt(2))
                ))
            }
        }
        
        return reminders
    }
    
    @JvmStatic
    fun deleteReminder(reminderId: Long): Boolean {
        val ctx = context ?: return false
        
        val rows = ctx.contentResolver.delete(
            CalendarContract.Reminders.CONTENT_URI,
            "${CalendarContract.Reminders._ID} = ?",
            arrayOf(reminderId.toString())
        )
        
        return rows > 0
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // RRULE Helpers
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun createDailyRrule(count: Int? = null): String {
        return if (count != null) "FREQ=DAILY;COUNT=$count" else "FREQ=DAILY"
    }
    
    @JvmStatic
    fun createWeeklyRrule(count: Int? = null, daysOfWeek: List<String>? = null): String {
        val builder = StringBuilder("FREQ=WEEKLY")
        daysOfWeek?.let { builder.append(";BYDAY=${it.joinToString(",")}") }
        count?.let { builder.append(";COUNT=$it") }
        return builder.toString()
    }
    
    @JvmStatic
    fun createMonthlyRrule(dayOfMonth: Int, count: Int? = null): String {
        val builder = StringBuilder("FREQ=MONTHLY;BYMONTHDAY=$dayOfMonth")
        count?.let { builder.append(";COUNT=$it") }
        return builder.toString()
    }
    
    @JvmStatic
    fun createYearlyRrule(count: Int? = null): String {
        return if (count != null) "FREQ=YEARLY;COUNT=$count" else "FREQ=YEARLY"
    }
    
    private fun getReminderMethodName(method: Int): String {
        return when (method) {
            CalendarContract.Reminders.METHOD_ALERT -> "تنبيه"
            CalendarContract.Reminders.METHOD_EMAIL -> "بريد إلكتروني"
            CalendarContract.Reminders.METHOD_SMS -> "رسالة نصية"
            CalendarContract.Reminders.METHOD_ALARM -> "إنذار"
            else -> "افتراضي"
        }
    }
}
