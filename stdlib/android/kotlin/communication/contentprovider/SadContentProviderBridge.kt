package sad.android.communication.contentprovider

import android.content.ContentResolver
import android.content.ContentUris
import android.content.ContentValues
import android.content.Context
import android.database.Cursor
import android.net.Uri
import android.provider.CalendarContract
import android.provider.MediaStore

/**
 * جسر مزود المحتوى - الوصول إلى بيانات النظام
 * Content Provider Bridge - System data access
 * 
 * @author فريق لغة ص
 */
object SadContentProviderBridge {
    
    /**
     * سجل بيانات / Data record
     */
    data class DataRecord(
        val id: Long,
        val values: Map<String, Any?>
    )
    
    /**
     * نتيجة العملية / Operation result
     */
    sealed class ProviderResult<T> {
        data class Success<T>(val data: T) : ProviderResult<T>()
        data class Error<T>(val message: String) : ProviderResult<T>()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         العمليات العامة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * استعلام / Query
     */
    fun query(
        context: Context,
        uri: Uri,
        projection: Array<String>? = null,
        selection: String? = null,
        selectionArgs: Array<String>? = null,
        sortOrder: String? = null
    ): List<DataRecord> {
        val records = mutableListOf<DataRecord>()
        
        context.contentResolver.query(
            uri,
            projection,
            selection,
            selectionArgs,
            sortOrder
        )?.use { cursor ->
            while (cursor.moveToNext()) {
                records.add(cursorToRecord(cursor))
            }
        }
        
        return records
    }
    
    /**
     * إدراج / Insert
     */
    fun insert(
        context: Context,
        uri: Uri,
        values: ContentValues
    ): Uri? {
        return try {
            context.contentResolver.insert(uri, values)
        } catch (e: Exception) {
            null
        }
    }
    
    /**
     * تحديث / Update
     */
    fun update(
        context: Context,
        uri: Uri,
        values: ContentValues,
        selection: String? = null,
        selectionArgs: Array<String>? = null
    ): Int {
        return try {
            context.contentResolver.update(uri, values, selection, selectionArgs)
        } catch (e: Exception) {
            0
        }
    }
    
    /**
     * حذف / Delete
     */
    fun delete(
        context: Context,
        uri: Uri,
        selection: String? = null,
        selectionArgs: Array<String>? = null
    ): Int {
        return try {
            context.contentResolver.delete(uri, selection, selectionArgs)
        } catch (e: Exception) {
            0
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الوسائط
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * جلب الصور / Get images
     */
    fun getImages(context: Context, limit: Int = 50): List<MediaItem> {
        val items = mutableListOf<MediaItem>()
        
        val projection = arrayOf(
            MediaStore.Images.Media._ID,
            MediaStore.Images.Media.DISPLAY_NAME,
            MediaStore.Images.Media.SIZE,
            MediaStore.Images.Media.DATE_ADDED,
            MediaStore.Images.Media.DATA
        )
        
        context.contentResolver.query(
            MediaStore.Images.Media.EXTERNAL_CONTENT_URI,
            projection,
            null,
            null,
            "${MediaStore.Images.Media.DATE_ADDED} DESC"
        )?.use { cursor ->
            var count = 0
            while (cursor.moveToNext() && count < limit) {
                val id = cursor.getLong(cursor.getColumnIndexOrThrow(MediaStore.Images.Media._ID))
                items.add(MediaItem(
                    id = id,
                    name = cursor.getString(cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DISPLAY_NAME)) ?: "",
                    size = cursor.getLong(cursor.getColumnIndexOrThrow(MediaStore.Images.Media.SIZE)),
                    dateAdded = cursor.getLong(cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATE_ADDED)),
                    uri = ContentUris.withAppendedId(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, id),
                    type = MediaType.IMAGE
                ))
                count++
            }
        }
        
        return items
    }
    
    /**
     * جلب الفيديوهات / Get videos
     */
    fun getVideos(context: Context, limit: Int = 50): List<MediaItem> {
        val items = mutableListOf<MediaItem>()
        
        val projection = arrayOf(
            MediaStore.Video.Media._ID,
            MediaStore.Video.Media.DISPLAY_NAME,
            MediaStore.Video.Media.SIZE,
            MediaStore.Video.Media.DATE_ADDED,
            MediaStore.Video.Media.DURATION
        )
        
        context.contentResolver.query(
            MediaStore.Video.Media.EXTERNAL_CONTENT_URI,
            projection,
            null,
            null,
            "${MediaStore.Video.Media.DATE_ADDED} DESC"
        )?.use { cursor ->
            var count = 0
            while (cursor.moveToNext() && count < limit) {
                val id = cursor.getLong(cursor.getColumnIndexOrThrow(MediaStore.Video.Media._ID))
                items.add(MediaItem(
                    id = id,
                    name = cursor.getString(cursor.getColumnIndexOrThrow(MediaStore.Video.Media.DISPLAY_NAME)) ?: "",
                    size = cursor.getLong(cursor.getColumnIndexOrThrow(MediaStore.Video.Media.SIZE)),
                    dateAdded = cursor.getLong(cursor.getColumnIndexOrThrow(MediaStore.Video.Media.DATE_ADDED)),
                    uri = ContentUris.withAppendedId(MediaStore.Video.Media.EXTERNAL_CONTENT_URI, id),
                    type = MediaType.VIDEO,
                    duration = cursor.getLong(cursor.getColumnIndexOrThrow(MediaStore.Video.Media.DURATION))
                ))
                count++
            }
        }
        
        return items
    }
    
    /**
     * جلب الصوتيات / Get audio files
     */
    fun getAudioFiles(context: Context, limit: Int = 50): List<MediaItem> {
        val items = mutableListOf<MediaItem>()
        
        val projection = arrayOf(
            MediaStore.Audio.Media._ID,
            MediaStore.Audio.Media.DISPLAY_NAME,
            MediaStore.Audio.Media.SIZE,
            MediaStore.Audio.Media.DATE_ADDED,
            MediaStore.Audio.Media.DURATION,
            MediaStore.Audio.Media.ARTIST,
            MediaStore.Audio.Media.ALBUM
        )
        
        context.contentResolver.query(
            MediaStore.Audio.Media.EXTERNAL_CONTENT_URI,
            projection,
            null,
            null,
            "${MediaStore.Audio.Media.DATE_ADDED} DESC"
        )?.use { cursor ->
            var count = 0
            while (cursor.moveToNext() && count < limit) {
                val id = cursor.getLong(cursor.getColumnIndexOrThrow(MediaStore.Audio.Media._ID))
                items.add(MediaItem(
                    id = id,
                    name = cursor.getString(cursor.getColumnIndexOrThrow(MediaStore.Audio.Media.DISPLAY_NAME)) ?: "",
                    size = cursor.getLong(cursor.getColumnIndexOrThrow(MediaStore.Audio.Media.SIZE)),
                    dateAdded = cursor.getLong(cursor.getColumnIndexOrThrow(MediaStore.Audio.Media.DATE_ADDED)),
                    uri = ContentUris.withAppendedId(MediaStore.Audio.Media.EXTERNAL_CONTENT_URI, id),
                    type = MediaType.AUDIO,
                    duration = cursor.getLong(cursor.getColumnIndexOrThrow(MediaStore.Audio.Media.DURATION)),
                    artist = cursor.getString(cursor.getColumnIndexOrThrow(MediaStore.Audio.Media.ARTIST)),
                    album = cursor.getString(cursor.getColumnIndexOrThrow(MediaStore.Audio.Media.ALBUM))
                ))
                count++
            }
        }
        
        return items
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التقويم
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * جلب التقويمات / Get calendars
     */
    fun getCalendars(context: Context): List<Calendar> {
        val calendars = mutableListOf<Calendar>()
        
        val projection = arrayOf(
            CalendarContract.Calendars._ID,
            CalendarContract.Calendars.NAME,
            CalendarContract.Calendars.ACCOUNT_NAME,
            CalendarContract.Calendars.CALENDAR_COLOR,
            CalendarContract.Calendars.IS_PRIMARY
        )
        
        try {
            context.contentResolver.query(
                CalendarContract.Calendars.CONTENT_URI,
                projection,
                null,
                null,
                null
            )?.use { cursor ->
                while (cursor.moveToNext()) {
                    calendars.add(Calendar(
                        id = cursor.getLong(cursor.getColumnIndexOrThrow(CalendarContract.Calendars._ID)),
                        name = cursor.getString(cursor.getColumnIndexOrThrow(CalendarContract.Calendars.NAME)) ?: "",
                        accountName = cursor.getString(cursor.getColumnIndexOrThrow(CalendarContract.Calendars.ACCOUNT_NAME)) ?: "",
                        color = cursor.getInt(cursor.getColumnIndexOrThrow(CalendarContract.Calendars.CALENDAR_COLOR)),
                        isPrimary = cursor.getInt(cursor.getColumnIndexOrThrow(CalendarContract.Calendars.IS_PRIMARY)) == 1
                    ))
                }
            }
        } catch (e: SecurityException) {}
        
        return calendars
    }
    
    /**
     * جلب الأحداث / Get events
     */
    fun getEvents(
        context: Context,
        calendarId: Long? = null,
        startTime: Long? = null,
        endTime: Long? = null,
        limit: Int = 50
    ): List<CalendarEvent> {
        val events = mutableListOf<CalendarEvent>()
        
        val projection = arrayOf(
            CalendarContract.Events._ID,
            CalendarContract.Events.TITLE,
            CalendarContract.Events.DESCRIPTION,
            CalendarContract.Events.DTSTART,
            CalendarContract.Events.DTEND,
            CalendarContract.Events.EVENT_LOCATION,
            CalendarContract.Events.ALL_DAY
        )
        
        val selection = buildString {
            calendarId?.let { append("${CalendarContract.Events.CALENDAR_ID} = $it") }
            startTime?.let {
                if (isNotEmpty()) append(" AND ")
                append("${CalendarContract.Events.DTSTART} >= $it")
            }
            endTime?.let {
                if (isNotEmpty()) append(" AND ")
                append("${CalendarContract.Events.DTEND} <= $it")
            }
        }.ifEmpty { null }
        
        try {
            context.contentResolver.query(
                CalendarContract.Events.CONTENT_URI,
                projection,
                selection,
                null,
                "${CalendarContract.Events.DTSTART} ASC"
            )?.use { cursor ->
                var count = 0
                while (cursor.moveToNext() && count < limit) {
                    events.add(CalendarEvent(
                        id = cursor.getLong(cursor.getColumnIndexOrThrow(CalendarContract.Events._ID)),
                        title = cursor.getString(cursor.getColumnIndexOrThrow(CalendarContract.Events.TITLE)) ?: "",
                        description = cursor.getString(cursor.getColumnIndexOrThrow(CalendarContract.Events.DESCRIPTION)),
                        startTime = cursor.getLong(cursor.getColumnIndexOrThrow(CalendarContract.Events.DTSTART)),
                        endTime = cursor.getLong(cursor.getColumnIndexOrThrow(CalendarContract.Events.DTEND)),
                        location = cursor.getString(cursor.getColumnIndexOrThrow(CalendarContract.Events.EVENT_LOCATION)),
                        isAllDay = cursor.getInt(cursor.getColumnIndexOrThrow(CalendarContract.Events.ALL_DAY)) == 1
                    ))
                    count++
                }
            }
        } catch (e: SecurityException) {}
        
        return events
    }
    
    /**
     * إضافة حدث / Add event
     */
    fun addEvent(
        context: Context,
        calendarId: Long,
        title: String,
        startTime: Long,
        endTime: Long,
        description: String? = null,
        location: String? = null,
        isAllDay: Boolean = false
    ): Long? {
        val values = ContentValues().apply {
            put(CalendarContract.Events.CALENDAR_ID, calendarId)
            put(CalendarContract.Events.TITLE, title)
            put(CalendarContract.Events.DTSTART, startTime)
            put(CalendarContract.Events.DTEND, endTime)
            put(CalendarContract.Events.EVENT_TIMEZONE, java.util.TimeZone.getDefault().id)
            put(CalendarContract.Events.ALL_DAY, if (isAllDay) 1 else 0)
            description?.let { put(CalendarContract.Events.DESCRIPTION, it) }
            location?.let { put(CalendarContract.Events.EVENT_LOCATION, it) }
        }
        
        return try {
            val uri = context.contentResolver.insert(CalendarContract.Events.CONTENT_URI, values)
            uri?.lastPathSegment?.toLongOrNull()
        } catch (e: SecurityException) {
            null
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الأنواع
    // ═══════════════════════════════════════════════════════════════
    
    enum class MediaType {
        IMAGE, VIDEO, AUDIO
    }
    
    data class MediaItem(
        val id: Long,
        val name: String,
        val size: Long,
        val dateAdded: Long,
        val uri: Uri,
        val type: MediaType,
        val duration: Long? = null,
        val artist: String? = null,
        val album: String? = null
    )
    
    data class Calendar(
        val id: Long,
        val name: String,
        val accountName: String,
        val color: Int,
        val isPrimary: Boolean
    )
    
    data class CalendarEvent(
        val id: Long,
        val title: String,
        val description: String?,
        val startTime: Long,
        val endTime: Long,
        val location: String?,
        val isAllDay: Boolean
    )
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    private fun cursorToRecord(cursor: Cursor): DataRecord {
        val values = mutableMapOf<String, Any?>()
        val id = cursor.getLong(0)
        
        for (i in 0 until cursor.columnCount) {
            val name = cursor.getColumnName(i)
            values[name] = when (cursor.getType(i)) {
                Cursor.FIELD_TYPE_INTEGER -> cursor.getLong(i)
                Cursor.FIELD_TYPE_FLOAT -> cursor.getDouble(i)
                Cursor.FIELD_TYPE_STRING -> cursor.getString(i)
                Cursor.FIELD_TYPE_BLOB -> cursor.getBlob(i)
                else -> null
            }
        }
        
        return DataRecord(id, values)
    }
}
