// ═══════════════════════════════════════════════════════════════
// إدارة تقدم المتعلم — Learning Progress Composable
// ═══════════════════════════════════════════════════════════════
import { ref, computed, watch } from 'vue'

const STORAGE_KEY = 'sad-learn-progress'

interface ProgressState {
  completed: Record<string, boolean>  // lessonId → true
  lastLesson: string | null
  lastLevel: number
}

function loadProgress(): ProgressState {
  try {
    const raw = localStorage.getItem(STORAGE_KEY)
    if (raw) return JSON.parse(raw)
  } catch { /* ignore */ }
  return { completed: {}, lastLesson: null, lastLevel: 1 }
}

function saveProgress(state: ProgressState) {
  try {
    localStorage.setItem(STORAGE_KEY, JSON.stringify(state))
  } catch { /* ignore */ }
}

const state = ref<ProgressState>(loadProgress())

watch(state, (s) => saveProgress(s), { deep: true })

export function useProgress() {
  const completedCount = computed(() => Object.keys(state.value.completed).length)

  function isCompleted(lessonId: string): boolean {
    return !!state.value.completed[lessonId]
  }

  function markCompleted(lessonId: string) {
    state.value.completed[lessonId] = true
  }

  function markIncomplete(lessonId: string) {
    delete state.value.completed[lessonId]
    state.value = { ...state.value }
  }

  function toggleCompleted(lessonId: string) {
    if (isCompleted(lessonId)) markIncomplete(lessonId)
    else markCompleted(lessonId)
  }

  function getLevelProgress(levelId: number, lessonIds: string[]): number {
    if (lessonIds.length === 0) return 0
    const done = lessonIds.filter(id => isCompleted(id)).length
    return Math.round((done / lessonIds.length) * 100)
  }

  function getSectionProgress(sectionLessonIds: string[]): number {
    if (sectionLessonIds.length === 0) return 0
    const done = sectionLessonIds.filter(id => isCompleted(id)).length
    return Math.round((done / sectionLessonIds.length) * 100)
  }

  function setLastLesson(lessonId: string, levelId: number) {
    state.value.lastLesson = lessonId
    state.value.lastLevel = levelId
  }

  function getLastLevel(): number {
    return state.value.lastLevel || 1
  }

  function getLastLesson(): string | null {
    return state.value.lastLesson
  }

  function resetProgress() {
    state.value = { completed: {}, lastLesson: null, lastLevel: 1 }
  }

  return {
    completedCount,
    isCompleted,
    markCompleted,
    markIncomplete,
    toggleCompleted,
    getLevelProgress,
    getSectionProgress,
    setLastLesson,
    getLastLevel,
    getLastLesson,
    resetProgress,
  }
}
