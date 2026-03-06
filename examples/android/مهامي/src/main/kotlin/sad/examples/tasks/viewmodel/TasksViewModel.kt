package sad.examples.tasks.viewmodel

import android.app.Application
import android.widget.Toast
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.viewModelScope
import androidx.work.*
import com.google.gson.Gson
import com.google.gson.reflect.TypeToken
import kotlinx.coroutines.flow.*
import kotlinx.coroutines.launch
import sad.examples.tasks.model.Task
import sad.examples.tasks.workers.ReminderWorker
import java.util.concurrent.TimeUnit

/**
 * ViewModel لإدارة المهام
 * Implements Sad language مدير_المهام class
 */
class TasksViewModel(application: Application) : AndroidViewModel(application) {
    
    private val prefs = application.getSharedPreferences("مهامي_db", Application.MODE_PRIVATE)
    private val gson = Gson()
    
    private val _tasks = MutableStateFlow<List<Task>>(emptyList())
    
    /**
     * المهام النشطة - المهام_النشطة()
     */
    val activeTasks: Flow<List<Task>> = _tasks.map { tasks ->
        tasks.filter { !it.completed }.sortedBy { it.priority }
    }
    
    /**
     * المهام المكتملة - المهام_المكتملة()
     */
    val completedTasks: Flow<List<Task>> = _tasks.map { tasks ->
        tasks.filter { it.completed }.sortedByDescending { it.createdAt }
    }
    
    init {
        loadTasks()
    }
    
    /**
     * تحميل المهام من التخزين - تحميل_المهام()
     */
    private fun loadTasks() {
        val json = prefs.getString("المهام", "[]") ?: "[]"
        val type = object : TypeToken<List<Task>>() {}.type
        val tasks: List<Task> = gson.fromJson(json, type)
        _tasks.value = tasks
    }
    
    /**
     * حفظ المهام - حفظ_المهام()
     */
    private fun saveTasks() {
        val json = gson.toJson(_tasks.value)
        prefs.edit().putString("المهام", json).apply()
    }
    
    /**
     * إضافة مهمة جديدة - أضف_مهمة()
     */
    fun addTask(title: String, priority: Int = 2) {
        val task = Task(
            title = title,
            priority = priority
        )
        _tasks.value = _tasks.value + task
        saveTasks()
        showToast("تمت إضافة المهمة ✓")
        vibrate()
    }
    
    /**
     * إكمال مهمة - أكمل_مهمة()
     */
    fun completeTask(id: String) {
        _tasks.value = _tasks.value.map { task ->
            if (task.id == id) task.copy(completed = true)
            else task
        }
        saveTasks()
        showToast("أحسنت! 🎉")
        vibrateSuccess()
    }
    
    /**
     * حذف مهمة - احذف_مهمة()
     */
    fun deleteTask(id: String) {
        _tasks.value = _tasks.value.filter { it.id != id }
        saveTasks()
        vibrate()
    }
    
    /**
     * عدد المهام النشطة - عدد_النشطة()
     */
    fun activeCount(): Int = _tasks.value.count { !it.completed }
    
    /**
     * جدولة إشعار التذكير - جدول_إشعارات()
     */
    fun scheduleReminder() {
        val count = activeCount()
        
        if (count == 0) {
            showToast("لا توجد مهام للتذكير بها")
            return
        }
        
        val workRequest = OneTimeWorkRequestBuilder<ReminderWorker>()
            .setInitialDelay(8, TimeUnit.HOURS)  // التذكير بعد 8 ساعات
            .setInputData(
                workDataOf(
                    "title" to "مهامي",
                    "message" to "لديك $count مهمة تنتظرك!"
                )
            )
            .build()
        
        WorkManager.getInstance(getApplication())
            .enqueue(workRequest)
        
        showToast("سيتم تذكيرك ⏰")
    }
    
    // ============ دوال مساعدة ============
    
    private fun showToast(message: String) {
        Toast.makeText(getApplication(), message, Toast.LENGTH_SHORT).show()
    }
    
    private fun vibrate() {
        // استخدام HapticFeedback
        // يتم في Compose عبر LocalHapticFeedback
    }
    
    private fun vibrateSuccess() {
        // اهتزاز نجاح
    }
}
