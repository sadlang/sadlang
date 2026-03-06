package sad.android.demo.productivity.widget

/**
 * ويدجت العادات - Habits Widget
 * 
 * @author فريق لغة ص
 */
object SadHabitsWidget {
    
    /**
     * تكوين ويدجت العادات / Habits widget config
     */
    data class HabitsWidgetConfig(
        val title: String = "عاداتي",
        val maxHabits: Int = 4,
        val showStreak: Boolean = true,
        val showProgress: Boolean = true,
        val theme: SadTasksWidget.WidgetTheme = SadTasksWidget.WidgetTheme.SYSTEM
    )
    
    /**
     * عنصر عادة للويدجت / Habit widget item
     */
    data class HabitWidgetItem(
        val id: String,
        val name: String,
        val emoji: String,
        val color: Int,
        val streak: Int,
        val isCompletedToday: Boolean,
        val progress: Float
    )
    
    /**
     * بيانات الويدجت / Widget data
     */
    data class HabitsWidgetData(
        val title: String,
        val habits: List<HabitWidgetItem>,
        val totalCompleted: Int,
        val totalHabits: Int,
        val overallProgress: Float
    )
    
    private var config = HabitsWidgetConfig()
    
    /**
     * تعيين التكوين / Set config
     */
    fun setConfig(newConfig: HabitsWidgetConfig) {
        config = newConfig
    }
    
    /**
     * الحصول على بيانات الويدجت / Get widget data
     */
    fun getWidgetData(): HabitsWidgetData {
        val app = sad.android.demo.productivity.SadProductivityApp
        val habits = app.getHabits()
        
        val startOfDay = getStartOfDay()
        
        val items = habits.take(config.maxHabits).map { habit ->
            val isCompletedToday = habit.completions.any { 
                it >= startOfDay && it < startOfDay + 86400000 
            }
            
            HabitWidgetItem(
                id = habit.id,
                name = habit.name,
                emoji = habit.iconEmoji,
                color = habit.color,
                streak = habit.streak,
                isCompletedToday = isCompletedToday,
                progress = if (isCompletedToday) 1f else 0f
            )
        }
        
        val completedToday = items.count { it.isCompletedToday }
        
        return HabitsWidgetData(
            title = config.title,
            habits = items,
            totalCompleted = completedToday,
            totalHabits = habits.size,
            overallProgress = if (habits.isNotEmpty()) completedToday.toFloat() / habits.size else 0f
        )
    }
    
    private fun getStartOfDay(): Long {
        val calendar = java.util.Calendar.getInstance()
        calendar.set(java.util.Calendar.HOUR_OF_DAY, 0)
        calendar.set(java.util.Calendar.MINUTE, 0)
        calendar.set(java.util.Calendar.SECOND, 0)
        calendar.set(java.util.Calendar.MILLISECOND, 0)
        return calendar.timeInMillis
    }
}
