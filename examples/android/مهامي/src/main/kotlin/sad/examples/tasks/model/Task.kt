package sad.examples.tasks.model

import java.text.SimpleDateFormat
import java.util.*

/**
 * نموذج المهمة
 * Task model - mapped from Sad language بنية مهمة
 */
data class Task(
    val id: String = UUID.randomUUID().toString(),
    val title: String,
    val completed: Boolean = false,
    val createdAt: Long = System.currentTimeMillis(),
    val dueDate: Long = 0,
    val priority: Int = 2  // 1=عالية, 2=متوسطة, 3=منخفضة
) {
    /**
     * تاريخ الإنشاء بصيغة مقروءة
     */
    val formattedDate: String
        get() {
            val sdf = SimpleDateFormat("yyyy/MM/dd", Locale.getDefault())
            return sdf.format(Date(createdAt))
        }
    
    /**
     * رمز الأولوية
     * Priority emoji matching Sad language رمز_الأولوية function
     */
    val priorityEmoji: String
        get() = when (priority) {
            1 -> "🔴"    // عالية - High
            2 -> "🟡"    // متوسطة - Medium
            3 -> "🟢"    // منخفضة - Low
            else -> ""
        }
}
