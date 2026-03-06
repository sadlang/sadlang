<template>
  <div class="notes-panel" :class="{ expanded: isExpanded }">
    <!-- رأس اللوحة -->
    <button class="notes-toggle" @click="togglePanel" :style="{ '--accent': levelColor }">
      <span class="toggle-icon">{{ isExpanded ? '▼' : '◀' }}</span>
      <span class="toggle-icon-emoji">📝</span>
      <span class="toggle-title">ملاحظاتي</span>
      <span v-if="hasExistingNote && !isExpanded" class="has-note-dot" />
      <span class="toggle-actions">
        <button
          class="bookmark-btn"
          :class="{ active: bookmarked }"
          @click.stop="handleBookmark"
          :title="bookmarked ? 'إزالة الإشارة المرجعية' : 'إضافة إشارة مرجعية'"
        >
          {{ bookmarked ? '⭐' : '☆' }}
        </button>
      </span>
    </button>

    <!-- محتوى الملاحظات -->
    <Transition name="notes-slide">
      <div v-if="isExpanded" class="notes-body">
        <div class="notes-editor">
          <textarea
            ref="textareaRef"
            v-model="noteContent"
            class="notes-textarea"
            :placeholder="'اكتب ملاحظاتك هنا...\n\n💡 الملاحظات تُحفظ تلقائياً'"
            rows="5"
            dir="rtl"
            @input="debouncedSave"
          />
          <div class="notes-footer">
            <span class="char-count">{{ noteContent.length }} حرف</span>
            <span v-if="lastSaved" class="save-status">
              <span class="save-dot" /> حُفظ {{ timeAgo(lastSaved) }}
            </span>
            <div class="notes-actions">
              <button
                v-if="noteContent.trim()"
                class="action-btn delete-btn"
                @click="handleDelete"
                title="حذف الملاحظة"
              >
                🗑️
              </button>
              <button
                v-if="noteContent.trim()"
                class="action-btn copy-btn"
                @click="handleCopy"
                title="نسخ الملاحظة"
              >
                {{ copied ? '✓' : '📋' }}
              </button>
            </div>
          </div>
        </div>
      </div>
    </Transition>
  </div>
</template>

<script setup lang="ts">
import { ref, watch, onMounted, nextTick } from 'vue'
import { useNotes } from '../composables/useNotes'

const props = defineProps<{
  lessonId: string
  lessonTitle: string
  sectionId?: string
  levelId?: number
  levelColor: string
}>()

const { getNote, saveNote, deleteNote, toggleBookmark, isBookmarked, hasNote } = useNotes()

const isExpanded = ref(false)
const noteContent = ref('')
const bookmarked = ref(false)
const lastSaved = ref<number | null>(null)
const copied = ref(false)
const hasExistingNote = ref(false)
const textareaRef = ref<HTMLTextAreaElement>()

let saveTimeout: ReturnType<typeof setTimeout> | null = null

// تحميل الملاحظة الحالية
function loadNote() {
  const note = getNote(props.lessonId)
  if (note) {
    noteContent.value = note.content
    lastSaved.value = note.updatedAt
    hasExistingNote.value = note.content.trim().length > 0
  } else {
    noteContent.value = ''
    lastSaved.value = null
    hasExistingNote.value = false
  }
  bookmarked.value = isBookmarked(props.lessonId)
}

// حفظ مع تأخير
function debouncedSave() {
  if (saveTimeout) clearTimeout(saveTimeout)
  saveTimeout = setTimeout(() => {
    if (noteContent.value.trim()) {
      saveNote(props.lessonId, noteContent.value, {
        lessonTitle: props.lessonTitle,
        sectionId: props.sectionId,
        levelId: props.levelId,
      })
      lastSaved.value = Date.now()
      hasExistingNote.value = true
    } else if (hasExistingNote.value) {
      // المحتوى فارغ — حذف الملاحظة
      deleteNote(props.lessonId)
      lastSaved.value = null
      hasExistingNote.value = false
    }
  }, 600)
}

function togglePanel() {
  isExpanded.value = !isExpanded.value
  if (isExpanded.value) {
    nextTick(() => textareaRef.value?.focus())
  }
}

function handleBookmark() {
  const result = toggleBookmark(props.lessonId, {
    lessonTitle: props.lessonTitle,
    sectionId: props.sectionId,
    levelId: props.levelId,
  })
  bookmarked.value = result
}

function handleDelete() {
  if (confirm('هل تريد حذف هذه الملاحظة؟')) {
    deleteNote(props.lessonId)
    noteContent.value = ''
    lastSaved.value = null
    hasExistingNote.value = false
  }
}

function handleCopy() {
  navigator.clipboard.writeText(noteContent.value)
  copied.value = true
  setTimeout(() => { copied.value = false }, 1500)
}

function timeAgo(ts: number): string {
  const diff = Math.floor((Date.now() - ts) / 1000)
  if (diff < 10) return 'الآن'
  if (diff < 60) return `منذ ${diff} ث`
  if (diff < 3600) return `منذ ${Math.floor(diff / 60)} د`
  if (diff < 86400) return `منذ ${Math.floor(diff / 3600)} س`
  return `منذ ${Math.floor(diff / 86400)} ي`
}

// مراقبة تغيير الدرس
watch(() => props.lessonId, () => {
  loadNote()
  isExpanded.value = false
})

onMounted(() => loadNote())
</script>

<style scoped>
.notes-panel {
  margin: 0 1.25rem 1rem;
  border: 1px dashed var(--vp-c-divider);
  border-radius: 12px;
  overflow: hidden;
  transition: border-color 0.2s;
}
.notes-panel.expanded {
  border-color: var(--vp-c-brand-1);
  border-style: solid;
}

/* ── رأس اللوحة ── */
.notes-toggle {
  width: 100%;
  display: flex;
  align-items: center;
  gap: 0.5rem;
  padding: 0.65rem 1rem;
  background: transparent;
  border: none;
  cursor: pointer;
  font-size: 0.85rem;
  color: var(--vp-c-text-2);
  direction: rtl;
  transition: all 0.15s;
}
.notes-toggle:hover {
  color: var(--vp-c-text-1);
  background: var(--vp-c-bg-soft);
}
.toggle-icon {
  font-size: 0.65rem;
  transition: transform 0.2s;
  opacity: 0.5;
}
.toggle-icon-emoji {
  font-size: 1rem;
}
.toggle-title {
  font-weight: 600;
  flex: 1;
  text-align: right;
}
.has-note-dot {
  width: 7px;
  height: 7px;
  border-radius: 50%;
  background: var(--vp-c-brand-1);
  animation: pulse-dot 2s infinite;
}
@keyframes pulse-dot {
  0%, 100% { opacity: 1; }
  50% { opacity: 0.4; }
}
.toggle-actions {
  display: flex;
  gap: 0.25rem;
}
.bookmark-btn {
  background: none;
  border: none;
  font-size: 1.1rem;
  cursor: pointer;
  padding: 0.15rem 0.3rem;
  border-radius: 6px;
  transition: all 0.15s;
  filter: grayscale(0.8);
}
.bookmark-btn:hover,
.bookmark-btn.active {
  filter: grayscale(0);
  transform: scale(1.15);
}

/* ── محتوى الملاحظات ── */
.notes-body {
  padding: 0 1rem 1rem;
}
.notes-editor {
  display: flex;
  flex-direction: column;
  gap: 0.5rem;
}
.notes-textarea {
  width: 100%;
  min-height: 100px;
  max-height: 300px;
  padding: 0.75rem;
  border: 1px solid var(--vp-c-divider);
  border-radius: 8px;
  background: var(--vp-c-bg);
  color: var(--vp-c-text-1);
  font-family: inherit;
  font-size: 0.88rem;
  line-height: 1.7;
  resize: vertical;
  transition: border-color 0.15s;
}
.notes-textarea:focus {
  outline: none;
  border-color: var(--vp-c-brand-1);
  box-shadow: 0 0 0 3px rgba(100, 108, 255, 0.08);
}
.notes-textarea::placeholder {
  color: var(--vp-c-text-3);
}

/* ── تذييل ── */
.notes-footer {
  display: flex;
  align-items: center;
  gap: 0.75rem;
  direction: rtl;
  font-size: 0.75rem;
  color: var(--vp-c-text-3);
}
.char-count {
  font-variant-numeric: tabular-nums;
}
.save-status {
  display: flex;
  align-items: center;
  gap: 0.3rem;
}
.save-dot {
  width: 5px;
  height: 5px;
  border-radius: 50%;
  background: #22c55e;
}
.notes-actions {
  margin-right: auto;
  display: flex;
  gap: 0.25rem;
}
.action-btn {
  background: none;
  border: 1px solid var(--vp-c-divider);
  border-radius: 6px;
  padding: 0.2rem 0.45rem;
  cursor: pointer;
  font-size: 0.8rem;
  transition: all 0.15s;
}
.action-btn:hover {
  background: var(--vp-c-bg-soft);
}
.delete-btn:hover {
  border-color: #ef4444;
  color: #ef4444;
}

/* ── انتقال ── */
.notes-slide-enter-active,
.notes-slide-leave-active {
  transition: all 0.25s ease;
}
.notes-slide-enter-from,
.notes-slide-leave-to {
  opacity: 0;
  max-height: 0;
  padding: 0;
}
.notes-slide-enter-to,
.notes-slide-leave-from {
  opacity: 1;
  max-height: 400px;
}

/* ── الوضع الداكن ── */
.dark .notes-textarea {
  background: var(--vp-c-bg-alt);
}
</style>
