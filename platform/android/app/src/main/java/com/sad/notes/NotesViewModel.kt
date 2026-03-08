// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  نموذج الملاحظة و ViewModel
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.notes

import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.compose.ui.graphics.Color
import androidx.lifecycle.ViewModel
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import java.text.SimpleDateFormat
import java.util.*

// ═══════════════════════════════════════════════════════════════════════════════
//  نموذج الملاحظة
// ═══════════════════════════════════════════════════════════════════════════════

data class Note(
    val id: String = UUID.randomUUID().toString(),
    val title: String,
    val content: String,
    val colorHex: String = "#FFFDE7",
    val isPinned: Boolean = false,
    val isDeleted: Boolean = false,
    val createdAt: Date = Date(),
    val modifiedAt: Date = Date()
) {
    val color: Color
        get() = parseColor(colorHex)
    
    val formattedDate: String
        get() {
            val formatter = SimpleDateFormat("d MMM yyyy", Locale("ar"))
            return formatter.format(modifiedAt)
        }
    
    companion object {
        fun parseColor(hex: String): Color {
            val cleanHex = hex.removePrefix("#")
            return try {
                Color(android.graphics.Color.parseColor("#$cleanHex"))
            } catch (e: Exception) {
                Color(0xFFFFFDE7) // أصفر فاتح افتراضي
            }
        }
        
        val availableColors = listOf(
            "#FFFDE7", // أصفر
            "#FFF3E0", // برتقالي
            "#FFEBEE", // أحمر
            "#FCE4EC", // وردي
            "#F3E5F5", // بنفسجي
            "#E3F2FD", // أزرق
            "#E0F7FA", // أزرق فاتح
            "#E8F5E9", // أخضر
            "#FAFAFA"  // رمادي
        )
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  ViewModel
// ═══════════════════════════════════════════════════════════════════════════════

class NotesViewModel : ViewModel() {
    
    private val _notes = MutableStateFlow<List<Note>>(emptyList())
    val notes: StateFlow<List<Note>> = _notes
    
    var isDarkMode by mutableStateOf(false)
        private set
    
    init {
        // بيانات تجريبية
        _notes.value = listOf(
            Note(
                title = "مرحباً بالعالم",
                content = "هذه أول ملاحظة في التطبيق!"
            ),
            Note(
                title = "قائمة المهام",
                content = "- تعلم لغة ص\n- بناء تطبيقات\n- المشاركة مع المجتمع",
                colorHex = "#E3F2FD"
            ),
            Note(
                title = "أفكار للمشروع",
                content = "• نظام إدارة المحتوى\n• تطبيق محادثة\n• لعبة تعليمية",
                colorHex = "#E8F5E9"
            )
        )
    }
    
    // ─────────────────────────────────────────────────────────────────────
    // العمليات الأساسية
    // ─────────────────────────────────────────────────────────────────────
    
    fun add(title: String, content: String, colorHex: String = "#FFFDE7") {
        val note = Note(
            title = title.trim(),
            content = content,
            colorHex = colorHex
        )
        _notes.value = listOf(note) + _notes.value
    }
    
    fun update(note: Note) {
        _notes.value = _notes.value.map { 
            if (it.id == note.id) {
                note.copy(modifiedAt = Date())
            } else {
                it
            }
        }
    }
    
    fun delete(note: Note) {
        _notes.value = _notes.value.map {
            if (it.id == note.id) {
                it.copy(isDeleted = true)
            } else {
                it
            }
        }
    }
    
    fun restore(note: Note) {
        _notes.value = _notes.value.map {
            if (it.id == note.id) {
                it.copy(isDeleted = false)
            } else {
                it
            }
        }
    }
    
    fun permanentlyDelete(note: Note) {
        _notes.value = _notes.value.filter { it.id != note.id }
    }
    
    fun togglePin(note: Note) {
        _notes.value = _notes.value.map {
            if (it.id == note.id) {
                it.copy(isPinned = !it.isPinned)
            } else {
                it
            }
        }
    }
    
    fun changeColor(note: Note, colorHex: String) {
        _notes.value = _notes.value.map {
            if (it.id == note.id) {
                it.copy(colorHex = colorHex, modifiedAt = Date())
            } else {
                it
            }
        }
    }
    
    // ─────────────────────────────────────────────────────────────────────
    // البحث والفلترة
    // ─────────────────────────────────────────────────────────────────────
    
    fun search(query: String): List<Note> {
        if (query.isBlank()) return _notes.value.filter { !it.isDeleted }
        
        return _notes.value.filter { note ->
            !note.isDeleted && (
                note.title.contains(query, ignoreCase = true) ||
                note.content.contains(query, ignoreCase = true)
            )
        }
    }
    
    val activeNotes: List<Note>
        get() = _notes.value.filter { !it.isDeleted }
    
    val pinnedNotes: List<Note>
        get() = activeNotes.filter { it.isPinned }
    
    val unpinnedNotes: List<Note>
        get() = activeNotes.filter { !it.isPinned }
    
    val deletedNotes: List<Note>
        get() = _notes.value.filter { it.isDeleted }
    
    // ─────────────────────────────────────────────────────────────────────
    // الثيم
    // ─────────────────────────────────────────────────────────────────────
    
    fun toggleDarkMode() {
        isDarkMode = !isDarkMode
    }
}
