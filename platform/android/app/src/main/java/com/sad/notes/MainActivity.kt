// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  تطبيق الملاحظات — الواجهة الرئيسية
//  Notes App — Main Activity
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.notes

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.staggeredgrid.LazyVerticalStaggeredGrid
import androidx.compose.foundation.lazy.staggeredgrid.StaggeredGridCells
import androidx.compose.foundation.lazy.staggeredgrid.items
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material.icons.outlined.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalLayoutDirection
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.unit.LayoutDirection
import androidx.compose.ui.unit.dp
import androidx.lifecycle.viewmodel.compose.viewModel
import com.sad.notes.ui.theme.SadNotesTheme

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        
        // تهيئة محرك لغة ص
        SadEngine.getInstance(this)
        
        setContent {
            SadNotesTheme {
                // اتجاه من اليمين لليسار (RTL)
                CompositionLocalProvider(LocalLayoutDirection provides LayoutDirection.Rtl) {
                    NotesApp()
                }
            }
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  التطبيق الرئيسي
// ═══════════════════════════════════════════════════════════════════════════════

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun NotesApp(viewModel: NotesViewModel = viewModel()) {
    var selectedTab by remember { mutableIntStateOf(0) }
    var searchQuery by remember { mutableStateOf("") }
    var showingSearch by remember { mutableStateOf(false) }
    var showNewNoteDialog by remember { mutableStateOf(false) }
    var selectedNote by remember { mutableStateOf<Note?>(null) }
    
    val notes = viewModel.notes.collectAsState()
    
    Scaffold(
        topBar = {
            TopAppBar(
                title = {
                    if (showingSearch) {
                        OutlinedTextField(
                            value = searchQuery,
                            onValueChange = { searchQuery = it },
                            placeholder = { Text("ابحث في الملاحظات...") },
                            singleLine = true,
                            modifier = Modifier.fillMaxWidth()
                        )
                    } else {
                        Text(
                            when (selectedTab) {
                                0 -> "ملاحظاتي"
                                1 -> "المثبتة"
                                2 -> "سلة المحذوفات"
                                else -> "ملاحظاتي"
                            }
                        )
                    }
                },
                navigationIcon = {
                    if (showingSearch) {
                        IconButton(onClick = { 
                            showingSearch = false
                            searchQuery = ""
                        }) {
                            Icon(Icons.Default.ArrowBack, contentDescription = "رجوع")
                        }
                    }
                },
                actions = {
                    if (!showingSearch) {
                        IconButton(onClick = { showingSearch = true }) {
                            Icon(Icons.Default.Search, contentDescription = "بحث")
                        }
                    }
                    IconButton(onClick = { viewModel.toggleDarkMode() }) {
                        Icon(
                            if (viewModel.isDarkMode) Icons.Default.LightMode else Icons.Default.DarkMode,
                            contentDescription = "تبديل الثيم"
                        )
                    }
                }
            )
        },
        bottomBar = {
            NavigationBar {
                NavigationBarItem(
                    selected = selectedTab == 0,
                    onClick = { selectedTab = 0 },
                    icon = { Icon(Icons.Outlined.Note, contentDescription = null) },
                    label = { Text("الكل") }
                )
                NavigationBarItem(
                    selected = selectedTab == 1,
                    onClick = { selectedTab = 1 },
                    icon = { Icon(Icons.Default.PushPin, contentDescription = null) },
                    label = { Text("مثبتة") }
                )
                NavigationBarItem(
                    selected = selectedTab == 2,
                    onClick = { selectedTab = 2 },
                    icon = { Icon(Icons.Default.Delete, contentDescription = null) },
                    label = { Text("محذوفة") }
                )
            }
        },
        floatingActionButton = {
            if (selectedTab != 2) {
                ExtendedFloatingActionButton(
                    onClick = { showNewNoteDialog = true },
                    icon = { Icon(Icons.Default.Add, contentDescription = null) },
                    text = { Text("ملاحظة جديدة") }
                )
            }
        }
    ) { padding ->
        val filteredNotes = when {
            searchQuery.isNotEmpty() -> notes.value.filter { note ->
                !note.isDeleted && (
                    note.title.contains(searchQuery, ignoreCase = true) ||
                    note.content.contains(searchQuery, ignoreCase = true)
                )
            }
            selectedTab == 0 -> notes.value.filter { !it.isDeleted }
            selectedTab == 1 -> notes.value.filter { !it.isDeleted && it.isPinned }
            selectedTab == 2 -> notes.value.filter { it.isDeleted }
            else -> emptyList()
        }
        
        if (filteredNotes.isEmpty()) {
            EmptyState(
                icon = when (selectedTab) {
                    0 -> Icons.Default.Note
                    1 -> Icons.Default.PushPin
                    else -> Icons.Default.Delete
                },
                message = when (selectedTab) {
                    0 -> "لا توجد ملاحظات\nاضغط + لإضافة ملاحظة جديدة"
                    1 -> "لا توجد ملاحظات مثبتة"
                    else -> "سلة المحذوفات فارغة"
                },
                modifier = Modifier.padding(padding)
            )
        } else {
            LazyVerticalStaggeredGrid(
                columns = StaggeredGridCells.Fixed(2),
                contentPadding = PaddingValues(8.dp),
                modifier = Modifier.padding(padding)
            ) {
                items(filteredNotes) { note ->
                    NoteCard(
                        note = note,
                        onClick = { selectedNote = note },
                        onPin = { viewModel.togglePin(note) },
                        onDelete = { 
                            if (note.isDeleted) {
                                viewModel.permanentlyDelete(note)
                            } else {
                                viewModel.delete(note)
                            }
                        },
                        onRestore = { viewModel.restore(note) },
                        isInTrash = selectedTab == 2
                    )
                }
            }
        }
    }
    
    // حوار الملاحظة الجديدة
    if (showNewNoteDialog) {
        NoteEditorDialog(
            note = null,
            onDismiss = { showNewNoteDialog = false },
            onSave = { title, content ->
                viewModel.add(title, content)
                showNewNoteDialog = false
            }
        )
    }
    
    // حوار تحرير الملاحظة
    selectedNote?.let { note ->
        NoteEditorDialog(
            note = note,
            onDismiss = { selectedNote = null },
            onSave = { title, content ->
                viewModel.update(note.copy(title = title, content = content))
                selectedNote = null
            }
        )
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  بطاقة الملاحظة
// ═══════════════════════════════════════════════════════════════════════════════

@Composable
fun NoteCard(
    note: Note,
    onClick: () -> Unit,
    onPin: () -> Unit,
    onDelete: () -> Unit,
    onRestore: () -> Unit,
    isInTrash: Boolean
) {
    Card(
        modifier = Modifier
            .padding(4.dp)
            .fillMaxWidth()
            .clickable { onClick() },
        colors = CardDefaults.cardColors(containerColor = note.color),
        shape = RoundedCornerShape(12.dp)
    ) {
        Column(
            modifier = Modifier.padding(12.dp)
        ) {
            Row(
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    text = note.title,
                    style = MaterialTheme.typography.titleMedium,
                    fontWeight = FontWeight.Bold,
                    maxLines = 1,
                    overflow = TextOverflow.Ellipsis,
                    modifier = Modifier.weight(1f)
                )
                
                if (note.isPinned) {
                    Icon(
                        Icons.Default.PushPin,
                        contentDescription = "مثبتة",
                        tint = MaterialTheme.colorScheme.primary,
                        modifier = Modifier.size(16.dp)
                    )
                }
            }
            
            Spacer(modifier = Modifier.height(8.dp))
            
            Text(
                text = note.content,
                style = MaterialTheme.typography.bodyMedium,
                maxLines = 4,
                overflow = TextOverflow.Ellipsis,
                color = MaterialTheme.colorScheme.onSurfaceVariant
            )
            
            Spacer(modifier = Modifier.height(8.dp))
            
            Row(
                horizontalArrangement = Arrangement.SpaceBetween,
                modifier = Modifier.fillMaxWidth()
            ) {
                Text(
                    text = note.formattedDate,
                    style = MaterialTheme.typography.labelSmall,
                    color = MaterialTheme.colorScheme.outline
                )
                
                Row {
                    if (isInTrash) {
                        IconButton(
                            onClick = onRestore,
                            modifier = Modifier.size(24.dp)
                        ) {
                            Icon(
                                Icons.Default.Restore,
                                contentDescription = "استعادة",
                                modifier = Modifier.size(16.dp)
                            )
                        }
                    } else {
                        IconButton(
                            onClick = onPin,
                            modifier = Modifier.size(24.dp)
                        ) {
                            Icon(
                                if (note.isPinned) Icons.Default.PushPin else Icons.Outlined.PushPin,
                                contentDescription = if (note.isPinned) "إلغاء التثبيت" else "تثبيت",
                                modifier = Modifier.size(16.dp)
                            )
                        }
                    }
                    
                    IconButton(
                        onClick = onDelete,
                        modifier = Modifier.size(24.dp)
                    ) {
                        Icon(
                            Icons.Default.Delete,
                            contentDescription = "حذف",
                            tint = MaterialTheme.colorScheme.error,
                            modifier = Modifier.size(16.dp)
                        )
                    }
                }
            }
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  حوار تحرير الملاحظة
// ═══════════════════════════════════════════════════════════════════════════════

@Composable
fun NoteEditorDialog(
    note: Note?,
    onDismiss: () -> Unit,
    onSave: (String, String) -> Unit
) {
    var title by remember { mutableStateOf(note?.title ?: "") }
    var content by remember { mutableStateOf(note?.content ?: "") }
    
    AlertDialog(
        onDismissRequest = onDismiss,
        title = {
            Text(if (note == null) "ملاحظة جديدة" else "تحرير الملاحظة")
        },
        text = {
            Column {
                OutlinedTextField(
                    value = title,
                    onValueChange = { title = it },
                    label = { Text("العنوان") },
                    singleLine = true,
                    modifier = Modifier.fillMaxWidth()
                )
                
                Spacer(modifier = Modifier.height(8.dp))
                
                OutlinedTextField(
                    value = content,
                    onValueChange = { content = it },
                    label = { Text("المحتوى") },
                    minLines = 5,
                    maxLines = 10,
                    modifier = Modifier.fillMaxWidth()
                )
            }
        },
        confirmButton = {
            TextButton(
                onClick = { onSave(title, content) },
                enabled = title.isNotBlank()
            ) {
                Text("حفظ")
            }
        },
        dismissButton = {
            TextButton(onClick = onDismiss) {
                Text("إلغاء")
            }
        }
    )
}

// ═══════════════════════════════════════════════════════════════════════════════
//  حالة فارغة
// ═══════════════════════════════════════════════════════════════════════════════

@Composable
fun EmptyState(
    icon: androidx.compose.ui.graphics.vector.ImageVector,
    message: String,
    modifier: Modifier = Modifier
) {
    Box(
        modifier = modifier.fillMaxSize(),
        contentAlignment = Alignment.Center
    ) {
        Column(
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Icon(
                icon,
                contentDescription = null,
                modifier = Modifier.size(80.dp),
                tint = MaterialTheme.colorScheme.outline
            )
            
            Spacer(modifier = Modifier.height(16.dp))
            
            Text(
                text = message,
                style = MaterialTheme.typography.bodyLarge,
                color = MaterialTheme.colorScheme.outline,
                textAlign = androidx.compose.ui.text.style.TextAlign.Center
            )
        }
    }
}
