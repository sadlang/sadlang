package sad.android.demo.productivity.widget

/**
 * ويدجت التقويم - Calendar Widget
 * 
 * @author فريق لغة ص
 */
object SadCalendarWidget {
    
    /**
     * تكوين ويدجت التقويم / Calendar widget config
     */
    data class CalendarWidgetConfig(
        val title: String = "أحداث اليوم",
        val maxEvents: Int = 4,
        val showTime: Boolean = true,
        val showLocation: Boolean = true,
        val daysAhead: Int = 1,
        val theme: SadTasksWidget.WidgetTheme = SadTasksWidget.WidgetTheme.SYSTEM
    )
    
    /**
     * عنصر حدث للويدجت / Event widget item
     */
    data class EventWidgetItem(
        val id: String,
        val title: String,
        val timeText: String,
        val locationText: String?,
        val color: Int,
        val isAllDay: Boolean,
        val isNow: Boolean
    )
    
    /**
     * بيانات الويدجت / Widget data
     */
    data class CalendarWidgetData(
        val title: String,
        val dateText: String,
        val events: List<EventWidgetItem>,
        val hasMoreEvents: Boolean
    )
    
    private var config = CalendarWidgetConfig()
    
    /**
     * تعيين التكوين / Set config
     */
    fun setConfig(newConfig: CalendarWidgetConfig) {
        config = newConfig
    }
    
    /**
     * الحصول على بيانات الويدجت / Get widget data
     */
    fun getWidgetData(): CalendarWidgetData {
        val app = sad.android.demo.productivity.SadProductivityApp
        val events = app.getTodayEvents()
        val now = System.currentTimeMillis()
        
        val items = events.take(config.maxEvents).map { event ->
            EventWidgetItem(
                id = event.id,
                title = event.title,
                timeText = formatEventTime(event),
                locationText = if (config.showLocation && event.location.isNotEmpty()) event.location else null,
                color = event.color,
                isAllDay = event.isAllDay,
                isNow = now in event.startTime..event.endTime
            )
        }
        
        return CalendarWidgetData(
            title = config.title,
            dateText = formatDate(System.currentTimeMillis()),
            events = items,
            hasMoreEvents = events.size > config.maxEvents
        )
    }
    
    private fun formatEventTime(event: sad.android.demo.productivity.SadProductivityApp.CalendarEvent): String {
        if (event.isAllDay) return "طوال اليوم"
        
        val format = java.text.SimpleDateFormat("h:mm a", java.util.Locale("ar"))
        val startTime = format.format(java.util.Date(event.startTime))
        val endTime = format.format(java.util.Date(event.endTime))
        
        return "$startTime - $endTime"
    }
    
    private fun formatDate(timestamp: Long): String {
        val format = java.text.SimpleDateFormat("EEEE، d MMMM", java.util.Locale("ar"))
        return format.format(java.util.Date(timestamp))
    }
}
