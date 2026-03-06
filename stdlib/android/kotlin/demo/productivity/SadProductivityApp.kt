package sad.android.demo.productivity

import android.content.Context

/**
 * تطبيق الإنتاجية التجريبي - يستخدم جسور الويدجت
 * Demo Productivity Application - Uses widget bridges
 * 
 * يوضح كيفية استخدام جسور لغة ص لبناء تطبيق إنتاجية مع ويدجت
 *
 * @author فريق لغة ص
 */
object SadProductivityApp {
    
    private var context: Context? = null
    private var isInitialized = false
    
    // ═══════════════════════════════════════════════════════════════
    //                         نماذج البيانات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * مهمة / Task
     */
    data class Task(
        val id: String,
        val title: String,
        val description: String = "",
        val dueDate: Long? = null,
        val priority: Priority = Priority.MEDIUM,
        val status: TaskStatus = TaskStatus.TODO,
        val projectId: String? = null,
        val tags: List<String> = emptyList(),
        val subtasks: List<Subtask> = emptyList(),
        val reminder: Long? = null,
        val createdAt: Long = System.currentTimeMillis(),
        val completedAt: Long? = null
    )
    
    /**
     * مهمة فرعية / Subtask
     */
    data class Subtask(
        val id: String,
        val title: String,
        val isCompleted: Boolean = false
    )
    
    /**
     * الأولوية / Priority
     */
    enum class Priority {
        LOW,
        MEDIUM,
        HIGH,
        URGENT
    }
    
    /**
     * حالة المهمة / Task status
     */
    enum class TaskStatus {
        TODO,
        IN_PROGRESS,
        DONE,
        CANCELLED
    }
    
    /**
     * مشروع / Project
     */
    data class Project(
        val id: String,
        val name: String,
        val color: Int = 0xFF1E88E5.toInt(),
        val iconEmoji: String = "📁",
        val taskCount: Int = 0,
        val completedCount: Int = 0,
        val createdAt: Long = System.currentTimeMillis()
    )
    
    /**
     * حدث التقويم / Calendar event
     */
    data class CalendarEvent(
        val id: String,
        val title: String,
        val description: String = "",
        val startTime: Long,
        val endTime: Long,
        val isAllDay: Boolean = false,
        val location: String = "",
        val color: Int = 0xFF1E88E5.toInt(),
        val reminder: Long? = null,
        val recurrence: Recurrence? = null
    )
    
    /**
     * التكرار / Recurrence
     */
    enum class Recurrence {
        DAILY,
        WEEKLY,
        MONTHLY,
        YEARLY
    }
    
    /**
     * ملاحظة / Note
     */
    data class Note(
        val id: String,
        val title: String,
        val content: String,
        val isPinned: Boolean = false,
        val color: Int = 0xFFFFFFFF.toInt(),
        val tags: List<String> = emptyList(),
        val createdAt: Long = System.currentTimeMillis(),
        val updatedAt: Long = System.currentTimeMillis()
    )
    
    /**
     * عادة / Habit
     */
    data class Habit(
        val id: String,
        val name: String,
        val description: String = "",
        val frequency: HabitFrequency = HabitFrequency.DAILY,
        val targetCount: Int = 1,
        val iconEmoji: String = "⭐",
        val color: Int = 0xFF4CAF50.toInt(),
        val streak: Int = 0,
        val completions: List<Long> = emptyList()
    )
    
    /**
     * تكرار العادة / Habit frequency
     */
    enum class HabitFrequency {
        DAILY,
        WEEKLY,
        CUSTOM
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         المستمعات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * مستمع التطبيق / App listener
     */
    interface ProductivityAppListener {
        fun onTaskUpdated(task: Task)
        fun onEventUpdated(event: CalendarEvent)
        fun onNoteUpdated(note: Note)
        fun onHabitUpdated(habit: Habit)
        fun onWidgetUpdateRequested()
    }
    
    private var appListener: ProductivityAppListener? = null
    
    // البيانات المحلية
    private val tasks = mutableMapOf<String, Task>()
    private val projects = mutableMapOf<String, Project>()
    private val events = mutableMapOf<String, CalendarEvent>()
    private val notes = mutableMapOf<String, Note>()
    private val habits = mutableMapOf<String, Habit>()
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة التطبيق / Initialize app
     * 
     * ```sad
     * # مثال الاستخدام في لغة ص
     * استورد "android/demo/productivity"
     * 
     * دالة رئيسية()
     *     تطبيق_الإنتاجية.هيئ(السياق)
     *     اطبع_سطر("تم تهيئة تطبيق الإنتاجية")
     * نهاية
     * ```
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        setupSampleData()
        isInitialized = true
    }
    
    /**
     * تعيين المستمع / Set listener
     */
    fun setAppListener(listener: ProductivityAppListener?) {
        appListener = listener
    }
    
    private fun setupSampleData() {
        // مشاريع تجريبية
        val project1 = Project(
            id = "proj1",
            name = "العمل",
            iconEmoji = "💼",
            color = 0xFF1E88E5.toInt()
        )
        projects["proj1"] = project1
        
        val project2 = Project(
            id = "proj2",
            name = "شخصي",
            iconEmoji = "🏠",
            color = 0xFF4CAF50.toInt()
        )
        projects["proj2"] = project2
        
        // مهام تجريبية
        val tomorrow = System.currentTimeMillis() + 86400000
        
        tasks["task1"] = Task(
            id = "task1",
            title = "إنهاء تقرير المشروع",
            description = "تقرير الربع الأول",
            dueDate = tomorrow,
            priority = Priority.HIGH,
            projectId = "proj1"
        )
        
        tasks["task2"] = Task(
            id = "task2",
            title = "اجتماع مع الفريق",
            dueDate = System.currentTimeMillis() + 3600000,
            priority = Priority.MEDIUM,
            projectId = "proj1"
        )
        
        tasks["task3"] = Task(
            id = "task3",
            title = "تمارين رياضية",
            priority = Priority.LOW,
            projectId = "proj2"
        )
        
        // أحداث تجريبية
        events["event1"] = CalendarEvent(
            id = "event1",
            title = "اجتماع الفريق",
            startTime = System.currentTimeMillis() + 3600000,
            endTime = System.currentTimeMillis() + 7200000,
            location = "غرفة الاجتماعات"
        )
        
        // عادات تجريبية
        habits["habit1"] = Habit(
            id = "habit1",
            name = "قراءة",
            iconEmoji = "📚",
            targetCount = 30,
            streak = 5
        )
        
        habits["habit2"] = Habit(
            id = "habit2",
            name = "رياضة",
            iconEmoji = "🏃",
            streak = 3
        )
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         المهام
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على جميع المهام / Get all tasks
     * 
     * ```sad
     * متغير المهام = تطبيق_الإنتاجية.احصل_على_المهام()
     * لكل مهمة في المهام
     *     اطبع_سطر(مهمة.العنوان)
     * نهاية
     * ```
     */
    fun getTasks(): List<Task> {
        return tasks.values.sortedWith(
            compareBy<Task> { it.status == TaskStatus.DONE }
                .thenByDescending { it.priority }
                .thenBy { it.dueDate ?: Long.MAX_VALUE }
        )
    }
    
    /**
     * الحصول على مهام اليوم / Get today's tasks
     */
    fun getTodayTasks(): List<Task> {
        val startOfDay = getStartOfDay()
        val endOfDay = startOfDay + 86400000
        
        return tasks.values.filter { task ->
            task.status != TaskStatus.DONE &&
            task.dueDate != null &&
            task.dueDate in startOfDay..endOfDay
        }.sortedByDescending { it.priority }
    }
    
    /**
     * الحصول على المهام المتأخرة / Get overdue tasks
     */
    fun getOverdueTasks(): List<Task> {
        val now = System.currentTimeMillis()
        return tasks.values.filter { task ->
            task.status != TaskStatus.DONE &&
            task.dueDate != null &&
            task.dueDate < now
        }
    }
    
    /**
     * إضافة مهمة / Add task
     * 
     * ```sad
     * متغير مهمة_جديدة = تطبيق_الإنتاجية.أضف_مهمة(
     *     "إنهاء المشروع"،
     *     الأولوية: Priority.HIGH
     * )
     * ```
     */
    fun addTask(
        title: String,
        description: String = "",
        dueDate: Long? = null,
        priority: Priority = Priority.MEDIUM,
        projectId: String? = null
    ): Task {
        val id = "task_${System.currentTimeMillis()}"
        val task = Task(
            id = id,
            title = title,
            description = description,
            dueDate = dueDate,
            priority = priority,
            projectId = projectId
        )
        
        tasks[id] = task
        updateProjectTaskCount(projectId)
        requestWidgetUpdate()
        appListener?.onTaskUpdated(task)
        
        return task
    }
    
    /**
     * إكمال مهمة / Complete task
     */
    fun completeTask(taskId: String): Boolean {
        val task = tasks[taskId] ?: return false
        
        tasks[taskId] = task.copy(
            status = TaskStatus.DONE,
            completedAt = System.currentTimeMillis()
        )
        
        updateProjectTaskCount(task.projectId)
        requestWidgetUpdate()
        appListener?.onTaskUpdated(tasks[taskId]!!)
        
        return true
    }
    
    /**
     * حذف مهمة / Delete task
     */
    fun deleteTask(taskId: String): Boolean {
        val task = tasks.remove(taskId) ?: return false
        updateProjectTaskCount(task.projectId)
        requestWidgetUpdate()
        return true
    }
    
    private fun updateProjectTaskCount(projectId: String?) {
        if (projectId == null) return
        
        val project = projects[projectId] ?: return
        val projectTasks = tasks.values.filter { it.projectId == projectId }
        
        projects[projectId] = project.copy(
            taskCount = projectTasks.size,
            completedCount = projectTasks.count { it.status == TaskStatus.DONE }
        )
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         المشاريع
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على جميع المشاريع / Get all projects
     */
    fun getProjects(): List<Project> = projects.values.toList()
    
    /**
     * الحصول على مهام مشروع / Get project tasks
     */
    fun getProjectTasks(projectId: String): List<Task> {
        return tasks.values.filter { it.projectId == projectId }
    }
    
    /**
     * إضافة مشروع / Add project
     */
    fun addProject(name: String, color: Int = 0xFF1E88E5.toInt(), iconEmoji: String = "📁"): Project {
        val id = "proj_${System.currentTimeMillis()}"
        val project = Project(id = id, name = name, color = color, iconEmoji = iconEmoji)
        projects[id] = project
        return project
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التقويم
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على أحداث اليوم / Get today's events
     */
    fun getTodayEvents(): List<CalendarEvent> {
        val startOfDay = getStartOfDay()
        val endOfDay = startOfDay + 86400000
        
        return events.values.filter { event ->
            event.startTime in startOfDay..endOfDay ||
            (event.startTime < startOfDay && event.endTime > startOfDay)
        }.sortedBy { it.startTime }
    }
    
    /**
     * الحصول على أحداث الأسبوع / Get week events
     */
    fun getWeekEvents(): List<CalendarEvent> {
        val startOfDay = getStartOfDay()
        val endOfWeek = startOfDay + 7 * 86400000
        
        return events.values.filter { event ->
            event.startTime in startOfDay..endOfWeek
        }.sortedBy { it.startTime }
    }
    
    /**
     * إضافة حدث / Add event
     */
    fun addEvent(
        title: String,
        startTime: Long,
        endTime: Long,
        description: String = "",
        location: String = ""
    ): CalendarEvent {
        val id = "event_${System.currentTimeMillis()}"
        val event = CalendarEvent(
            id = id,
            title = title,
            description = description,
            startTime = startTime,
            endTime = endTime,
            location = location
        )
        
        events[id] = event
        requestWidgetUpdate()
        appListener?.onEventUpdated(event)
        
        return event
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         العادات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على جميع العادات / Get all habits
     */
    fun getHabits(): List<Habit> = habits.values.toList()
    
    /**
     * تسجيل إكمال عادة / Log habit completion
     */
    fun logHabitCompletion(habitId: String): Boolean {
        val habit = habits[habitId] ?: return false
        
        val today = getStartOfDay()
        val alreadyCompletedToday = habit.completions.any {
            it >= today && it < today + 86400000
        }
        
        if (alreadyCompletedToday) return false
        
        val newCompletions = habit.completions + System.currentTimeMillis()
        val newStreak = calculateStreak(newCompletions)
        
        habits[habitId] = habit.copy(
            completions = newCompletions,
            streak = newStreak
        )
        
        requestWidgetUpdate()
        appListener?.onHabitUpdated(habits[habitId]!!)
        
        return true
    }
    
    private fun calculateStreak(completions: List<Long>): Int {
        if (completions.isEmpty()) return 0
        
        var streak = 0
        var currentDay = getStartOfDay()
        
        val sortedCompletions = completions.sortedDescending()
        
        for (completion in sortedCompletions) {
            val completionDay = completion - (completion % 86400000)
            if (completionDay == currentDay || completionDay == currentDay - 86400000) {
                streak++
                currentDay = completionDay - 86400000
            } else {
                break
            }
        }
        
        return streak
    }
    
    /**
     * إضافة عادة / Add habit
     */
    fun addHabit(name: String, iconEmoji: String = "⭐", frequency: HabitFrequency = HabitFrequency.DAILY): Habit {
        val id = "habit_${System.currentTimeMillis()}"
        val habit = Habit(
            id = id,
            name = name,
            iconEmoji = iconEmoji,
            frequency = frequency
        )
        
        habits[id] = habit
        requestWidgetUpdate()
        
        return habit
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الملاحظات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على جميع الملاحظات / Get all notes
     */
    fun getNotes(): List<Note> {
        return notes.values
            .sortedByDescending { it.isPinned }
            .sortedByDescending { it.updatedAt }
    }
    
    /**
     * إضافة ملاحظة / Add note
     */
    fun addNote(title: String, content: String): Note {
        val id = "note_${System.currentTimeMillis()}"
        val note = Note(id = id, title = title, content = content)
        notes[id] = note
        appListener?.onNoteUpdated(note)
        return note
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الإحصائيات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إحصائيات اليوم / Today's stats
     */
    data class DailyStats(
        val totalTasks: Int,
        val completedTasks: Int,
        val upcomingEvents: Int,
        val habitsCompleted: Int,
        val totalHabits: Int,
        val overdueTasks: Int
    )
    
    /**
     * الحصول على إحصائيات اليوم / Get today's stats
     */
    fun getTodayStats(): DailyStats {
        val todayTasks = getTodayTasks()
        val todayEvents = getTodayEvents()
        val today = getStartOfDay()
        
        return DailyStats(
            totalTasks = todayTasks.size,
            completedTasks = todayTasks.count { it.status == TaskStatus.DONE },
            upcomingEvents = todayEvents.size,
            habitsCompleted = habits.values.count { habit ->
                habit.completions.any { it >= today && it < today + 86400000 }
            },
            totalHabits = habits.size,
            overdueTasks = getOverdueTasks().size
        )
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         المساعدات
    // ═══════════════════════════════════════════════════════════════
    
    private fun getStartOfDay(): Long {
        val calendar = java.util.Calendar.getInstance()
        calendar.set(java.util.Calendar.HOUR_OF_DAY, 0)
        calendar.set(java.util.Calendar.MINUTE, 0)
        calendar.set(java.util.Calendar.SECOND, 0)
        calendar.set(java.util.Calendar.MILLISECOND, 0)
        return calendar.timeInMillis
    }
    
    private fun requestWidgetUpdate() {
        appListener?.onWidgetUpdateRequested()
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        tasks.clear()
        projects.clear()
        events.clear()
        notes.clear()
        habits.clear()
        appListener = null
        context = null
        isInitialized = false
    }
}
