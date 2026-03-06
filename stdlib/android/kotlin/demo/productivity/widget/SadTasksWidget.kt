package sad.android.demo.productivity.widget

import android.content.Context

/**
 * ويدجت المهام - Tasks Widget
 * 
 * @author فريق لغة ص
 */
object SadTasksWidget {
    
    /**
     * تكوين ويدجت المهام / Tasks widget config
     */
    data class TasksWidgetConfig(
        val title: String = "مهامي",
        val maxTasks: Int = 5,
        val showCompleted: Boolean = false,
        val showDueDate: Boolean = true,
        val showPriority: Boolean = true,
        val filterProject: String? = null,
        val sortBy: SortBy = SortBy.DUE_DATE,
        val theme: WidgetTheme = WidgetTheme.SYSTEM
    )
    
    /**
     * ترتيب / Sort by
     */
    enum class SortBy {
        DUE_DATE,
        PRIORITY,
        CREATED_DATE,
        ALPHABETICAL
    }
    
    /**
     * سمة الويدجت / Widget theme
     */
    enum class WidgetTheme {
        LIGHT,
        DARK,
        SYSTEM
    }
    
    /**
     * عنصر مهمة للويدجت / Task widget item
     */
    data class TaskWidgetItem(
        val id: String,
        val title: String,
        val dueText: String?,
        val priorityColor: Int,
        val isCompleted: Boolean,
        val projectEmoji: String?
    )
    
    /**
     * بيانات الويدجت / Widget data
     */
    data class TasksWidgetData(
        val title: String,
        val tasks: List<TaskWidgetItem>,
        val totalCount: Int,
        val completedCount: Int,
        val overdueCount: Int
    )
    
    private var config = TasksWidgetConfig()
    
    /**
     * تعيين التكوين / Set config
     */
    fun setConfig(newConfig: TasksWidgetConfig) {
        config = newConfig
    }
    
    /**
     * الحصول على بيانات الويدجت / Get widget data
     * 
     * ```sad
     * متغير بيانات_الويدجت = ويدجت_المهام.احصل_على_البيانات()
     * اطبع_سطر("المهام: " + بيانات_الويدجت.العدد_الكلي)
     * ```
     */
    fun getWidgetData(): TasksWidgetData {
        val app = sad.android.demo.productivity.SadProductivityApp
        
        var tasks = if (config.filterProject != null) {
            app.getProjectTasks(config.filterProject!!)
        } else {
            app.getTasks()
        }
        
        // فلترة المكتملة
        if (!config.showCompleted) {
            tasks = tasks.filter { it.status != sad.android.demo.productivity.SadProductivityApp.TaskStatus.DONE }
        }
        
        // الترتيب
        tasks = when (config.sortBy) {
            SortBy.DUE_DATE -> tasks.sortedBy { it.dueDate ?: Long.MAX_VALUE }
            SortBy.PRIORITY -> tasks.sortedByDescending { it.priority }
            SortBy.CREATED_DATE -> tasks.sortedByDescending { it.createdAt }
            SortBy.ALPHABETICAL -> tasks.sortedBy { it.title }
        }
        
        val projects = app.getProjects().associateBy { it.id }
        
        val items = tasks.take(config.maxTasks).map { task ->
            TaskWidgetItem(
                id = task.id,
                title = task.title,
                dueText = if (config.showDueDate) formatDueDate(task.dueDate) else null,
                priorityColor = getPriorityColor(task.priority),
                isCompleted = task.status == sad.android.demo.productivity.SadProductivityApp.TaskStatus.DONE,
                projectEmoji = projects[task.projectId]?.iconEmoji
            )
        }
        
        return TasksWidgetData(
            title = config.title,
            tasks = items,
            totalCount = tasks.size,
            completedCount = tasks.count { it.status == sad.android.demo.productivity.SadProductivityApp.TaskStatus.DONE },
            overdueCount = app.getOverdueTasks().size
        )
    }
    
    private fun formatDueDate(dueDate: Long?): String? {
        if (dueDate == null) return null
        
        val now = System.currentTimeMillis()
        val diff = dueDate - now
        
        return when {
            diff < 0 -> "متأخر"
            diff < 86400000 -> "اليوم"
            diff < 2 * 86400000 -> "غداً"
            diff < 7 * 86400000 -> "هذا الأسبوع"
            else -> {
                val date = java.util.Date(dueDate)
                val format = java.text.SimpleDateFormat("d/M", java.util.Locale("ar"))
                format.format(date)
            }
        }
    }
    
    private fun getPriorityColor(priority: sad.android.demo.productivity.SadProductivityApp.Priority): Int {
        return when (priority) {
            sad.android.demo.productivity.SadProductivityApp.Priority.URGENT -> 0xFFD32F2F.toInt()
            sad.android.demo.productivity.SadProductivityApp.Priority.HIGH -> 0xFFFF5722.toInt()
            sad.android.demo.productivity.SadProductivityApp.Priority.MEDIUM -> 0xFFFFB300.toInt()
            sad.android.demo.productivity.SadProductivityApp.Priority.LOW -> 0xFF4CAF50.toInt()
        }
    }
}
