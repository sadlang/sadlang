package sad.examples.tasks

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.style.TextDecoration
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.lifecycle.viewmodel.compose.viewModel
import sad.examples.tasks.ui.theme.SadTasksTheme
import sad.examples.tasks.viewmodel.TasksViewModel
import sad.examples.tasks.model.Task

/**
 * النشاط الرئيسي لتطبيق مهامي
 * Main activity for Sad Tasks app
 * 
 * هذا التطبيق يوضح استخدام جسور لغة ص مع أندرويد
 */
class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            SadTasksTheme {
                TasksApp()
            }
        }
    }
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun TasksApp(viewModel: TasksViewModel = viewModel()) {
    var newTaskText by remember { mutableStateOf("") }
    var selectedTab by remember { mutableIntStateOf(0) }
    
    val activeTasks by viewModel.activeTasks.collectAsState(initial = emptyList())
    val completedTasks by viewModel.completedTasks.collectAsState(initial = emptyList())
    
    Scaffold(
        topBar = {
            TopAppBar(
                title = { Text("📝 مهامي") },
                colors = TopAppBarDefaults.topAppBarColors(
                    containerColor = MaterialTheme.colorScheme.primary,
                    titleContentColor = MaterialTheme.colorScheme.onPrimary
                )
            )
        },
        floatingActionButton = {
            FloatingActionButton(
                onClick = { viewModel.scheduleReminder() }
            ) {
                Icon(Icons.Default.Notifications, contentDescription = "تذكير")
            }
        }
    ) { padding ->
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(padding)
                .padding(16.dp)
        ) {
            // حقل إدخال مهمة جديدة
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.spacedBy(8.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                OutlinedTextField(
                    value = newTaskText,
                    onValueChange = { newTaskText = it },
                    placeholder = { Text("أضف مهمة جديدة...") },
                    modifier = Modifier.weight(1f),
                    singleLine = true
                )
                
                FilledIconButton(
                    onClick = {
                        if (newTaskText.isNotBlank()) {
                            viewModel.addTask(newTaskText)
                            newTaskText = ""
                        }
                    }
                ) {
                    Icon(Icons.Default.Add, contentDescription = "إضافة")
                }
            }
            
            Spacer(modifier = Modifier.height(16.dp))
            
            // التبويبات
            TabRow(selectedTabIndex = selectedTab) {
                Tab(
                    selected = selectedTab == 0,
                    onClick = { selectedTab = 0 },
                    text = { Text("نشطة (${activeTasks.size})") }
                )
                Tab(
                    selected = selectedTab == 1,
                    onClick = { selectedTab = 1 },
                    text = { Text("مكتملة") }
                )
            }
            
            Spacer(modifier = Modifier.height(8.dp))
            
            // قائمة المهام
            when (selectedTab) {
                0 -> TasksList(
                    tasks = activeTasks,
                    onComplete = { viewModel.completeTask(it) },
                    onDelete = { viewModel.deleteTask(it) },
                    isActive = true
                )
                1 -> TasksList(
                    tasks = completedTasks,
                    onComplete = { },
                    onDelete = { viewModel.deleteTask(it) },
                    isActive = false
                )
            }
        }
    }
}

@Composable
fun TasksList(
    tasks: List<Task>,
    onComplete: (String) -> Unit,
    onDelete: (String) -> Unit,
    isActive: Boolean
) {
    if (tasks.isEmpty()) {
        Box(
            modifier = Modifier.fillMaxSize(),
            contentAlignment = Alignment.Center
        ) {
            Column(horizontalAlignment = Alignment.CenterHorizontally) {
                Text(
                    text = if (isActive) "🎉" else "📋",
                    fontSize = 64.sp
                )
                Spacer(modifier = Modifier.height(8.dp))
                Text(
                    text = if (isActive) "لا توجد مهام!" else "لم تكمل أي مهمة بعد",
                    color = Color.Gray
                )
            }
        }
    } else {
        LazyColumn(
            verticalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            items(tasks, key = { it.id }) { task ->
                TaskCard(
                    task = task,
                    onComplete = { onComplete(task.id) },
                    onDelete = { onDelete(task.id) },
                    isActive = isActive
                )
            }
        }
    }
}

@Composable
fun TaskCard(
    task: Task,
    onComplete: () -> Unit,
    onDelete: () -> Unit,
    isActive: Boolean
) {
    Card(
        modifier = Modifier.fillMaxWidth()
    ) {
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(12.dp),
            horizontalArrangement = Arrangement.spacedBy(12.dp),
            verticalAlignment = Alignment.CenterVertically
        ) {
            // أيقونة الحالة
            IconButton(onClick = onComplete, enabled = isActive) {
                Icon(
                    imageVector = if (task.completed) Icons.Default.CheckCircle else Icons.Default.RadioButtonUnchecked,
                    contentDescription = "حالة",
                    tint = if (task.completed) Color(0xFF4CAF50) else Color.Gray
                )
            }
            
            // نص المهمة
            Column(modifier = Modifier.weight(1f)) {
                Text(
                    text = task.title,
                    fontSize = 16.sp,
                    textDecoration = if (task.completed) TextDecoration.LineThrough else TextDecoration.None
                )
                Text(
                    text = task.formattedDate,
                    fontSize = 12.sp,
                    color = Color.Gray
                )
            }
            
            // رمز الأولوية
            if (isActive) {
                Text(text = task.priorityEmoji)
            }
            
            // زر الحذف
            IconButton(onClick = onDelete) {
                Icon(
                    imageVector = Icons.Default.Delete,
                    contentDescription = "حذف",
                    tint = Color.Red.copy(alpha = 0.7f)
                )
            }
        }
    }
}
