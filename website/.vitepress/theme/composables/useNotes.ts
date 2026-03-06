/**
 * useNotes — نظام الملاحظات والمناقشات الشخصية
 * Personal notes & bookmarks system (localStorage-based)
 *
 * يتيح للمتعلم:
 * - كتابة ملاحظات لكل درس
 * - وضع إشارات مرجعية للدروس المهمة
 * - مراجعة جميع الملاحظات في مكان واحد
 * - البحث والتصفية في الملاحظات
 */

import { ref, watch } from 'vue'

// ─── الأنواع ────────────────────────────────────────────

export interface LessonNote {
  lessonId: string
  content: string
  createdAt: number
  updatedAt: number
  bookmarked: boolean
  /** اسم الدرس (للعرض في المراجعة) */
  lessonTitle?: string
  /** معرف القسم */
  sectionId?: string
  /** رقم المستوى */
  levelId?: number
}

export interface NotesState {
  notes: Record<string, LessonNote>
  /** تاريخ آخر تعديل عام */
  lastModified: number
}

// ─── التخزين ────────────────────────────────────────────

const STORAGE_KEY = 'sad-learn-notes'

function loadNotes(): NotesState {
  if (typeof window === 'undefined') return { notes: {}, lastModified: 0 }
  try {
    const raw = localStorage.getItem(STORAGE_KEY)
    if (raw) {
      const parsed = JSON.parse(raw)
      return {
        notes: parsed.notes || {},
        lastModified: parsed.lastModified || 0,
      }
    }
  } catch {
    // تالف — نبدأ من جديد
  }
  return { notes: {}, lastModified: 0 }
}

function persistNotes(state: NotesState) {
  if (typeof window === 'undefined') return
  try {
    localStorage.setItem(STORAGE_KEY, JSON.stringify(state))
  } catch {
    // التخزين ممتلئ
  }
}

// ─── الحالة المشتركة (Singleton) ────────────────────────

const state = ref<NotesState>(loadNotes())

// حفظ تلقائي عند أي تغيير
watch(state, (val) => persistNotes(val), { deep: true })

// ─── الدوال العامة ──────────────────────────────────────

export function useNotes() {
  /**
   * الحصول على ملاحظة درس معين
   */
  function getNote(lessonId: string): LessonNote | null {
    return state.value.notes[lessonId] || null
  }

  /**
   * حفظ أو تحديث ملاحظة
   */
  function saveNote(
    lessonId: string,
    content: string,
    meta?: { lessonTitle?: string; sectionId?: string; levelId?: number }
  ) {
    const now = Date.now()
    const existing = state.value.notes[lessonId]

    state.value.notes[lessonId] = {
      lessonId,
      content,
      createdAt: existing?.createdAt || now,
      updatedAt: now,
      bookmarked: existing?.bookmarked || false,
      lessonTitle: meta?.lessonTitle || existing?.lessonTitle,
      sectionId: meta?.sectionId || existing?.sectionId,
      levelId: meta?.levelId ?? existing?.levelId,
    }
    state.value.lastModified = now
  }

  /**
   * حذف ملاحظة
   */
  function deleteNote(lessonId: string) {
    if (state.value.notes[lessonId]) {
      delete state.value.notes[lessonId]
      state.value.lastModified = Date.now()
    }
  }

  /**
   * تبديل الإشارة المرجعية
   */
  function toggleBookmark(
    lessonId: string,
    meta?: { lessonTitle?: string; sectionId?: string; levelId?: number }
  ): boolean {
    const existing = state.value.notes[lessonId]
    const now = Date.now()

    if (existing) {
      existing.bookmarked = !existing.bookmarked
      existing.updatedAt = now
    } else {
      // إنشاء ملاحظة فارغة مع إشارة مرجعية
      state.value.notes[lessonId] = {
        lessonId,
        content: '',
        createdAt: now,
        updatedAt: now,
        bookmarked: true,
        lessonTitle: meta?.lessonTitle,
        sectionId: meta?.sectionId,
        levelId: meta?.levelId,
      }
    }
    state.value.lastModified = now
    return state.value.notes[lessonId]?.bookmarked ?? false
  }

  /**
   * هل الدرس محفوظ كإشارة مرجعية؟
   */
  function isBookmarked(lessonId: string): boolean {
    return state.value.notes[lessonId]?.bookmarked ?? false
  }

  /**
   * هل للدرس ملاحظة (محتوى غير فارغ)؟
   */
  function hasNote(lessonId: string): boolean {
    const note = state.value.notes[lessonId]
    return !!note && note.content.trim().length > 0
  }

  /**
   * جميع الملاحظات (مرتبة بالأحدث)
   */
  function getAllNotes(): LessonNote[] {
    return Object.values(state.value.notes)
      .filter((n) => n.content.trim().length > 0 || n.bookmarked)
      .sort((a, b) => b.updatedAt - a.updatedAt)
  }

  /**
   * الدروس المحفوظة كإشارات مرجعية
   */
  function getBookmarkedNotes(): LessonNote[] {
    return Object.values(state.value.notes)
      .filter((n) => n.bookmarked)
      .sort((a, b) => b.updatedAt - a.updatedAt)
  }

  /**
   * البحث في الملاحظات
   */
  function searchNotes(query: string): LessonNote[] {
    if (!query.trim()) return getAllNotes()
    const q = query.trim().toLowerCase()
    return getAllNotes().filter(
      (n) =>
        n.content.toLowerCase().includes(q) ||
        (n.lessonTitle && n.lessonTitle.toLowerCase().includes(q))
    )
  }

  /**
   * ملاحظات مستوى معين
   */
  function getNotesByLevel(levelId: number): LessonNote[] {
    return getAllNotes().filter((n) => n.levelId === levelId)
  }

  /**
   * عدد الملاحظات
   */
  function getNotesCount(): number {
    return Object.values(state.value.notes).filter(
      (n) => n.content.trim().length > 0
    ).length
  }

  /**
   * عدد الإشارات المرجعية
   */
  function getBookmarksCount(): number {
    return Object.values(state.value.notes).filter((n) => n.bookmarked).length
  }

  /**
   * تصدير الملاحظات كنص
   */
  function exportNotes(): string {
    const notes = getAllNotes()
    if (notes.length === 0) return 'لا توجد ملاحظات'

    let output = '# ملاحظاتي — منصة تعلم لغة ص\n'
    output += `# التاريخ: ${new Date().toLocaleDateString('ar-SA')}\n`
    output += `# عدد الملاحظات: ${notes.length}\n\n`

    for (const note of notes) {
      output += `## ${note.lessonTitle || note.lessonId}\n`
      if (note.bookmarked) output += '⭐ إشارة مرجعية\n'
      output += `${note.content}\n`
      output += `— آخر تعديل: ${new Date(note.updatedAt).toLocaleDateString('ar-SA')}\n\n`
      output += '---\n\n'
    }

    return output
  }

  /**
   * مسح جميع الملاحظات
   */
  function clearAllNotes() {
    state.value.notes = {}
    state.value.lastModified = Date.now()
  }

  return {
    // قراءة
    getNote,
    hasNote,
    isBookmarked,
    getAllNotes,
    getBookmarkedNotes,
    searchNotes,
    getNotesByLevel,
    getNotesCount,
    getBookmarksCount,
    // كتابة
    saveNote,
    deleteNote,
    toggleBookmark,
    clearAllNotes,
    // تصدير
    exportNotes,
  }
}
