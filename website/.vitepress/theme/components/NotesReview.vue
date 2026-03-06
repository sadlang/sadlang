<template>
  <Teleport to="body">
    <div class="notes-review-overlay" @click.self="$emit('close')">
      <div class="notes-review-modal" dir="rtl">
        <!-- رأس المودال -->
        <div class="review-header">
          <div class="header-right">
            <span class="header-icon">📓</span>
            <h2 class="header-title">ملاحظاتي وإشاراتي المرجعية</h2>
          </div>
          <div class="header-actions">
            <button
              v-if="allNotes.length > 0"
              class="export-btn"
              @click="handleExport"
              title="تصدير الملاحظات"
            >
              {{ exportDone ? '✓ نُسخت' : '📤 تصدير' }}
            </button>
            <button class="close-btn" @click="$emit('close')" title="إغلاق">✕</button>
          </div>
        </div>

        <!-- شريط البحث والتصفية -->
        <div class="review-toolbar">
          <div class="search-box">
            <span class="search-icon">🔍</span>
            <input
              v-model="searchQuery"
              type="text"
              class="search-input"
              placeholder="بحث في الملاحظات..."
              dir="rtl"
            />
            <button v-if="searchQuery" class="clear-search" @click="searchQuery = ''">✕</button>
          </div>
          <div class="filter-tabs">
            <button
              v-for="tab in filterTabs"
              :key="tab.key"
              class="filter-tab"
              :class="{ active: activeFilter === tab.key }"
              :style="{ '--accent': levelColor }"
              @click="activeFilter = tab.key"
            >
              <span>{{ tab.icon }}</span>
              <span>{{ tab.label }}</span>
              <span class="tab-count">{{ tab.count }}</span>
            </button>
          </div>
        </div>

        <!-- محتوى الملاحظات -->
        <div class="review-content">
          <template v-if="filteredNotes.length > 0">
            <div
              v-for="note in filteredNotes"
              :key="note.lessonId"
              class="note-card"
            >
              <div class="note-card-header">
                <div class="note-info">
                  <span v-if="note.bookmarked" class="bookmark-star">⭐</span>
                  <span class="note-lesson-title">{{ note.lessonTitle || note.lessonId }}</span>
                  <span v-if="note.levelId" class="note-level-badge">
                    المستوى {{ note.levelId }}
                  </span>
                </div>
                <div class="note-actions">
                  <button class="note-action-btn" @click="goToLesson(note)" title="الذهاب للدرس">
                    ← فتح
                  </button>
                  <button
                    class="note-action-btn bookmark-toggle"
                    :class="{ active: note.bookmarked }"
                    @click="handleToggleBookmark(note)"
                    :title="note.bookmarked ? 'إزالة الإشارة' : 'إضافة إشارة'"
                  >
                    {{ note.bookmarked ? '⭐' : '☆' }}
                  </button>
                  <button
                    class="note-action-btn delete-action"
                    @click="handleDeleteNote(note)"
                    title="حذف"
                  >
                    🗑️
                  </button>
                </div>
              </div>
              <div v-if="note.content.trim()" class="note-card-body">
                <p class="note-text">{{ truncate(note.content, 300) }}</p>
              </div>
              <div class="note-card-footer">
                <span class="note-date">{{ formatDate(note.updatedAt) }}</span>
                <span v-if="note.content.trim()" class="note-chars">
                  {{ note.content.length }} حرف
                </span>
              </div>
            </div>
          </template>

          <!-- حالة فارغة -->
          <div v-else class="empty-state">
            <div class="empty-icon">{{ activeFilter === 'bookmarks' ? '☆' : '📝' }}</div>
            <p class="empty-title">
              {{ searchQuery
                ? 'لا توجد نتائج'
                : activeFilter === 'bookmarks'
                  ? 'لا توجد إشارات مرجعية بعد'
                  : 'لا توجد ملاحظات بعد'
              }}
            </p>
            <p class="empty-desc">
              {{ searchQuery
                ? 'جرّب كلمات بحث مختلفة'
                : 'ابدأ بكتابة ملاحظاتك أثناء الدروس!'
              }}
            </p>
          </div>
        </div>

        <!-- إحصائيات -->
        <div v-if="allNotes.length > 0" class="review-footer">
          <span>📝 {{ notesCount }} ملاحظة</span>
          <span>⭐ {{ bookmarksCount }} إشارة مرجعية</span>
        </div>
      </div>
    </div>
  </Teleport>
</template>

<script setup lang="ts">
import { ref, computed, onMounted, onUnmounted } from 'vue'
import { useNotes, type LessonNote } from '../composables/useNotes'

const props = defineProps<{
  levelColor: string
}>()

const emit = defineEmits<{
  (e: 'close'): void
  (e: 'goToLesson', lessonId: string, levelId?: number, sectionId?: string): void
}>()

const {
  getAllNotes,
  getBookmarkedNotes,
  searchNotes,
  getNotesCount,
  getBookmarksCount,
  toggleBookmark,
  deleteNote,
  exportNotes,
} = useNotes()

const searchQuery = ref('')
const activeFilter = ref<'all' | 'bookmarks'>('all')
const exportDone = ref(false)

// بيانات محسوبة
const allNotes = computed(() => getAllNotes())
const bookmarks = computed(() => getBookmarkedNotes())
const notesCount = computed(() => getNotesCount())
const bookmarksCount = computed(() => getBookmarksCount())

const filterTabs = computed(() => [
  { key: 'all' as const, icon: '📝', label: 'الكل', count: allNotes.value.length },
  { key: 'bookmarks' as const, icon: '⭐', label: 'المرجعية', count: bookmarks.value.length },
])

const filteredNotes = computed(() => {
  let notes: LessonNote[]
  if (activeFilter.value === 'bookmarks') {
    notes = bookmarks.value
  } else {
    notes = allNotes.value
  }

  if (searchQuery.value.trim()) {
    const q = searchQuery.value.trim().toLowerCase()
    notes = notes.filter(
      (n) =>
        n.content.toLowerCase().includes(q) ||
        (n.lessonTitle && n.lessonTitle.toLowerCase().includes(q))
    )
  }

  return notes
})

// أحداث
function goToLesson(note: LessonNote) {
  emit('goToLesson', note.lessonId, note.levelId, note.sectionId)
  emit('close')
}

function handleToggleBookmark(note: LessonNote) {
  toggleBookmark(note.lessonId)
}

function handleDeleteNote(note: LessonNote) {
  if (confirm('هل تريد حذف هذه الملاحظة؟')) {
    deleteNote(note.lessonId)
  }
}

function handleExport() {
  const text = exportNotes()
  navigator.clipboard.writeText(text)
  exportDone.value = true
  setTimeout(() => { exportDone.value = false }, 2000)
}

// مساعدات
function truncate(text: string, max: number): string {
  return text.length > max ? text.slice(0, max) + '...' : text
}

function formatDate(ts: number): string {
  return new Date(ts).toLocaleDateString('ar-SA', {
    year: 'numeric',
    month: 'short',
    day: 'numeric',
    hour: '2-digit',
    minute: '2-digit',
  })
}

// القفل والوصول
function onKeydown(e: KeyboardEvent) {
  if (e.key === 'Escape') emit('close')
}

onMounted(() => {
  document.body.style.overflow = 'hidden'
  document.addEventListener('keydown', onKeydown)
})

onUnmounted(() => {
  document.body.style.overflow = ''
  document.removeEventListener('keydown', onKeydown)
})
</script>

<style scoped>
.notes-review-overlay {
  position: fixed;
  inset: 0;
  z-index: 999;
  background: rgba(0, 0, 0, 0.5);
  display: flex;
  align-items: center;
  justify-content: center;
  padding: 1.5rem;
  backdrop-filter: blur(4px);
}

.notes-review-modal {
  width: 100%;
  max-width: 700px;
  max-height: 85vh;
  background: var(--vp-c-bg);
  border: 1px solid var(--vp-c-divider);
  border-radius: 16px;
  box-shadow: 0 25px 60px rgba(0, 0, 0, 0.2);
  display: flex;
  flex-direction: column;
  overflow: hidden;
}

/* ── الرأس ── */
.review-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 1rem 1.25rem;
  border-bottom: 1px solid var(--vp-c-divider);
  background: var(--vp-c-bg-soft);
}
.header-right {
  display: flex;
  align-items: center;
  gap: 0.6rem;
}
.header-icon {
  font-size: 1.5rem;
}
.header-title {
  margin: 0;
  font-size: 1.1rem;
  font-weight: 700;
  color: var(--vp-c-text-1);
}
.header-actions {
  display: flex;
  gap: 0.5rem;
}
.export-btn {
  padding: 0.35rem 0.8rem;
  border: 1px solid var(--vp-c-divider);
  border-radius: 8px;
  background: var(--vp-c-bg);
  cursor: pointer;
  font-size: 0.8rem;
  color: var(--vp-c-text-2);
  transition: all 0.15s;
}
.export-btn:hover {
  background: var(--vp-c-bg-alt);
  color: var(--vp-c-text-1);
}
.close-btn {
  width: 32px;
  height: 32px;
  display: flex;
  align-items: center;
  justify-content: center;
  border: 1px solid var(--vp-c-divider);
  border-radius: 8px;
  background: var(--vp-c-bg);
  cursor: pointer;
  font-size: 0.9rem;
  color: var(--vp-c-text-2);
  transition: all 0.15s;
}
.close-btn:hover {
  background: #fee2e2;
  color: #ef4444;
  border-color: #fecaca;
}

/* ── شريط البحث ── */
.review-toolbar {
  padding: 0.75rem 1.25rem;
  border-bottom: 1px solid var(--vp-c-divider);
  display: flex;
  flex-direction: column;
  gap: 0.6rem;
}
.search-box {
  display: flex;
  align-items: center;
  gap: 0.5rem;
  padding: 0.4rem 0.75rem;
  border: 1px solid var(--vp-c-divider);
  border-radius: 10px;
  background: var(--vp-c-bg);
  transition: border-color 0.15s;
}
.search-box:focus-within {
  border-color: var(--vp-c-brand-1);
}
.search-icon {
  font-size: 0.85rem;
  opacity: 0.5;
}
.search-input {
  flex: 1;
  border: none;
  outline: none;
  background: transparent;
  font-size: 0.85rem;
  color: var(--vp-c-text-1);
  font-family: inherit;
}
.search-input::placeholder {
  color: var(--vp-c-text-3);
}
.clear-search {
  background: none;
  border: none;
  cursor: pointer;
  font-size: 0.75rem;
  color: var(--vp-c-text-3);
  padding: 0.15rem 0.3rem;
  border-radius: 4px;
}
.clear-search:hover {
  background: var(--vp-c-bg-soft);
  color: var(--vp-c-text-1);
}
.filter-tabs {
  display: flex;
  gap: 0.4rem;
}
.filter-tab {
  display: flex;
  align-items: center;
  gap: 0.3rem;
  padding: 0.3rem 0.7rem;
  border: 1px solid var(--vp-c-divider);
  border-radius: 20px;
  background: var(--vp-c-bg);
  cursor: pointer;
  font-size: 0.78rem;
  color: var(--vp-c-text-2);
  transition: all 0.15s;
}
.filter-tab:hover {
  background: var(--vp-c-bg-soft);
}
.filter-tab.active {
  background: var(--accent, var(--vp-c-brand-1));
  color: #fff;
  border-color: transparent;
}
.tab-count {
  font-variant-numeric: tabular-nums;
  opacity: 0.7;
  font-size: 0.7rem;
}

/* ── محتوى الملاحظات ── */
.review-content {
  flex: 1;
  overflow-y: auto;
  padding: 1rem 1.25rem;
  display: flex;
  flex-direction: column;
  gap: 0.75rem;
}

.note-card {
  border: 1px solid var(--vp-c-divider);
  border-radius: 10px;
  overflow: hidden;
  transition: all 0.15s;
}
.note-card:hover {
  border-color: var(--vp-c-brand-1);
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.06);
}
.note-card-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 0.6rem 0.85rem;
  background: var(--vp-c-bg-soft);
  border-bottom: 1px solid var(--vp-c-divider);
}
.note-info {
  display: flex;
  align-items: center;
  gap: 0.4rem;
  flex: 1;
  min-width: 0;
}
.bookmark-star {
  font-size: 0.85rem;
}
.note-lesson-title {
  font-weight: 600;
  font-size: 0.85rem;
  color: var(--vp-c-text-1);
  white-space: nowrap;
  overflow: hidden;
  text-overflow: ellipsis;
}
.note-level-badge {
  font-size: 0.65rem;
  padding: 0.1rem 0.45rem;
  background: var(--vp-c-brand-soft);
  color: var(--vp-c-brand-1);
  border-radius: 10px;
  white-space: nowrap;
}
.note-actions {
  display: flex;
  gap: 0.2rem;
  flex-shrink: 0;
}
.note-action-btn {
  padding: 0.2rem 0.5rem;
  border: 1px solid transparent;
  border-radius: 6px;
  background: none;
  cursor: pointer;
  font-size: 0.75rem;
  color: var(--vp-c-text-2);
  transition: all 0.15s;
}
.note-action-btn:hover {
  background: var(--vp-c-bg);
  border-color: var(--vp-c-divider);
}
.delete-action:hover {
  color: #ef4444;
  border-color: #fecaca;
}

.note-card-body {
  padding: 0.65rem 0.85rem;
}
.note-text {
  margin: 0;
  font-size: 0.83rem;
  line-height: 1.65;
  color: var(--vp-c-text-2);
  white-space: pre-wrap;
  word-break: break-word;
}
.note-card-footer {
  display: flex;
  align-items: center;
  gap: 0.75rem;
  padding: 0.4rem 0.85rem;
  font-size: 0.7rem;
  color: var(--vp-c-text-3);
  border-top: 1px solid var(--vp-c-divider);
}
.note-date {
  font-variant-numeric: tabular-nums;
}

/* ── حالة فارغة ── */
.empty-state {
  flex: 1;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  padding: 3rem 1rem;
  text-align: center;
}
.empty-icon {
  font-size: 3rem;
  margin-bottom: 0.75rem;
  opacity: 0.3;
}
.empty-title {
  margin: 0 0 0.3rem;
  font-size: 1rem;
  font-weight: 600;
  color: var(--vp-c-text-2);
}
.empty-desc {
  margin: 0;
  font-size: 0.85rem;
  color: var(--vp-c-text-3);
}

/* ── التذييل ── */
.review-footer {
  display: flex;
  align-items: center;
  gap: 1.25rem;
  padding: 0.6rem 1.25rem;
  border-top: 1px solid var(--vp-c-divider);
  background: var(--vp-c-bg-soft);
  font-size: 0.78rem;
  color: var(--vp-c-text-3);
}

/* ── استجابة ── */
@media (max-width: 640px) {
  .notes-review-overlay {
    padding: 0.75rem;
  }
  .notes-review-modal {
    max-height: 90vh;
    border-radius: 12px;
  }
  .review-toolbar {
    padding: 0.6rem 1rem;
  }
}
</style>
