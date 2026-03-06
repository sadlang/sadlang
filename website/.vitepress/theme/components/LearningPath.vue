<template>
  <div class="learning-path" dir="rtl">
    <!-- ═══ Header with particles ═══ -->
    <div class="lp-header">
      <div class="lp-particles" aria-hidden="true">
        <span v-for="n in 20" :key="n" class="dot" :class="'dot-' + n" />
      </div>
      <div class="lp-header-content">
        <div class="lp-badge">مسار تفاعلي</div>
        <h1 class="lp-title">
          <span class="title-deco">❰</span>
          المسار التعليمي للغة ص
          <span class="title-deco">❱</span>
        </h1>
        <p class="lp-subtitle">{{ totalLessons }} درس تفاعلي — من الصفر إلى الاحتراف</p>

        <!-- Global progress -->
        <div class="lp-global-progress">
          <div class="gp-row">
            <span class="gp-label">التقدم الكلي</span>
            <span class="gp-pct">{{ globalProgress }}%</span>
          </div>
          <div class="gp-bar">
            <div
              class="gp-fill"
              :style="{ width: globalProgress + '%', background: globalGradient }"
            >
              <div class="gp-shimmer" />
            </div>
          </div>
          <span class="gp-detail">{{ completedCount }} من {{ totalLessons }} درس مكتمل</span>
        </div>

        <!-- Continue where you left off -->
        <button
          v-if="lastLessonInfo && !activeLesson"
          class="lp-resume"
          @click="resumeLastLesson"
        >
          <span class="resume-icon">
            <svg width="16" height="16" viewBox="0 0 24 24" fill="currentColor"><path d="M8 5v14l11-7z"/></svg>
          </span>
          <span class="resume-text">متابعة: {{ lastLessonInfo.title }}</span>
          <span class="resume-arrow">←</span>
        </button>
      </div>
    </div>

    <!-- ═══ Level tabs ═══ -->
    <LevelTabs
      :levels="levels"
      :active-level="activeLevel"
      :get-progress="getLevelProgressPct"
      @select="selectLevel"
    />

    <!-- ═══ View switching with transition ═══ -->
    <Transition name="view-fade" mode="out-in">
      <SectionList
        v-if="!activeLesson"
        key="sections"
        :sections="currentSections"
        :level-color="currentLevelColor"
        :is-completed="isCompleted"
        :get-section-progress="getSectionProgressPct"
        :has-quiz="hasQuizForSection"
        @open-lesson="openLesson"
        @start-quiz="startSectionQuiz"
      />
      <LessonWizard
        v-else
        key="wizard"
        :lesson="activeLesson"
        :lesson-index="activeLessonIndex"
        :total-lessons="activeSectionLessons.length"
        :is-completed="isCompleted(activeLesson.id)"
        :level-color="currentLevelColor"
        :section-id="activeSectionId || undefined"
        :level-id="activeLevel"
        @close="closeLesson"
        @complete="completeLesson"
        @next="nextLesson"
        @prev="prevLesson"
      />
    </Transition>

    <!-- ═══ Stats & Achievements (below content when no lesson open) ═══ -->
    <template v-if="!activeLesson">
      <LearningStats
        :completed-count="completedCount"
        :total-lessons="totalLessons"
      />
      <AchievementBadges />
    </template>

    <!-- ═══ Notes Review Button (below content when no lesson) ═══ -->
    <div v-if="!activeLesson && notesCount > 0" class="notes-review-trigger">
      <button class="notes-review-btn" @click="showNotesReview = true">
        <span>📓</span>
        <span>ملاحظاتي ({{ notesCount }})</span>
        <span v-if="bookmarksCount > 0">⭐ {{ bookmarksCount }}</span>
      </button>
    </div>

    <!-- ═══ Quiz Modal ═══ -->
    <QuizModal
      v-if="showQuiz && activeQuiz"
      :title="activeQuiz.title"
      :questions="activeQuiz.questions"
      :level-color="currentLevelColor"
      @close="showQuiz = false"
      @complete="onQuizComplete"
    />

    <!-- ═══ Notes Review Modal ═══ -->
    <NotesReview
      v-if="showNotesReview"
      :level-color="currentLevelColor"
      @close="showNotesReview = false"
      @go-to-lesson="goToLessonFromNotes"
    />
  </div>
</template>

<script setup lang="ts">
import { ref, computed, onMounted } from 'vue'
import { allLevels } from '../lessons'
import type { Level, Section, Lesson } from '../lessons'
import { useProgress } from '../composables/useProgress'
import LevelTabs from './LevelTabs.vue'
import SectionList from './SectionList.vue'
import LessonWizard from './LessonWizard.vue'
import LearningStats from './LearningStats.vue'
import AchievementBadges from './AchievementBadges.vue'
import QuizModal from './QuizModal.vue'
import type { QuizQuestion } from './QuizModal.vue'
import NotesReview from './NotesReview.vue'
import { useSadWasm } from '../composables/useSadWasm'
import { useNotes } from '../composables/useNotes'
import { getQuizForSection } from '../lessons/quizzes'

const {
  completedCount, isCompleted, markCompleted,
  getLevelProgress, getSectionProgress,
  setLastLesson, getLastLevel, getLastLesson,
} = useProgress()

const levels = allLevels
const activeLevel = ref(1)
const activeLesson = ref<Lesson | null>(null)
const activeSectionId = ref<string | null>(null)

const currentLevel = computed(() => levels.find(l => l.id === activeLevel.value)!)
const currentSections = computed(() => currentLevel.value?.sections || [])
const currentLevelColor = computed(() => currentLevel.value?.color || '#10b981')

const activeSectionLessons = computed(() => {
  if (!activeSectionId.value) return []
  const sec = currentSections.value.find(s => s.id === activeSectionId.value)
  return sec?.lessons || []
})

const activeLessonIndex = computed(() => {
  if (!activeLesson.value) return 0
  return activeSectionLessons.value.findIndex(l => l.id === activeLesson.value!.id)
})

const totalLessons = computed(() =>
  levels.reduce((sum, l) => sum + l.sections.reduce((s, sec) => s + sec.lessons.length, 0), 0)
)

const allLessonIds = computed(() =>
  levels.flatMap(l => l.sections.flatMap(s => s.lessons.map(le => le.id)))
)

const globalProgress = computed(() => {
  if (allLessonIds.value.length === 0) return 0
  const done = allLessonIds.value.filter(id => isCompleted(id)).length
  return Math.round((done / allLessonIds.value.length) * 100)
})

const globalColor = computed(() => {
  const p = globalProgress.value
  if (p < 25) return '#10b981'
  if (p < 50) return '#3b82f6'
  if (p < 75) return '#8b5cf6'
  return '#f59e0b'
})

const globalGradient = computed(() => {
  const p = globalProgress.value
  if (p < 25) return 'linear-gradient(90deg, #10b981, #34d399)'
  if (p < 50) return 'linear-gradient(90deg, #10b981, #3b82f6)'
  if (p < 75) return 'linear-gradient(90deg, #3b82f6, #8b5cf6)'
  return 'linear-gradient(90deg, #8b5cf6, #f59e0b)'
})

/* ── Continue where you left off ── */
const lastLessonInfo = computed(() => {
  const id = getLastLesson()
  if (!id) return null
  for (const level of levels) {
    for (const sec of level.sections) {
      const lesson = sec.lessons.find(l => l.id === id)
      if (lesson) return { lesson, sectionId: sec.id, levelId: level.id, title: lesson.title }
    }
  }
  return null
})

function resumeLastLesson() {
  const info = lastLessonInfo.value
  if (!info) return
  activeLevel.value = info.levelId
  openLesson(info.lesson, info.sectionId)
}

function getLevelProgressPct(levelId: number): number {
  const level = levels.find(l => l.id === levelId)
  if (!level) return 0
  const ids = level.sections.flatMap(s => s.lessons.map(l => l.id))
  return getLevelProgress(levelId, ids)
}

function getSectionProgressPct(sectionLessonIds: string[]): number {
  return getSectionProgress(sectionLessonIds)
}

function selectLevel(levelId: number) {
  activeLevel.value = levelId
  activeLesson.value = null
  activeSectionId.value = null
}

function openLesson(lesson: Lesson, sectionId: string) {
  activeLesson.value = lesson
  activeSectionId.value = sectionId
  setLastLesson(lesson.id, activeLevel.value)
}

function closeLesson() {
  activeLesson.value = null
  activeSectionId.value = null
}

function completeLesson() {
  if (activeLesson.value) {
    markCompleted(activeLesson.value.id)
  }
}

function nextLesson() {
  const idx = activeLessonIndex.value
  if (idx < activeSectionLessons.value.length - 1) {
    activeLesson.value = activeSectionLessons.value[idx + 1]
    setLastLesson(activeLesson.value!.id, activeLevel.value)
  }
}

function prevLesson() {
  const idx = activeLessonIndex.value
  if (idx > 0) {
    activeLesson.value = activeSectionLessons.value[idx - 1]
    setLastLesson(activeLesson.value!.id, activeLevel.value)
  }
}

onMounted(() => {
  activeLevel.value = getLastLevel()
  // Auto-load WASM for code execution
  const { loadWasm } = useSadWasm()
  loadWasm()
})

/* ── Notes system ── */
const { getNotesCount, getBookmarksCount } = useNotes()
const showNotesReview = ref(false)
const notesCount = computed(() => getNotesCount())
const bookmarksCount = computed(() => getBookmarksCount())

function goToLessonFromNotes(lessonId: string, levelId?: number, sectionId?: string) {
  // التنقل إلى الدرس من مراجعة الملاحظات
  if (levelId) activeLevel.value = levelId
  if (sectionId) {
    const sec = currentSections.value.find(s => s.id === sectionId)
    if (sec) {
      const lesson = sec.lessons.find(l => l.id === lessonId)
      if (lesson) {
        openLesson(lesson, sectionId)
        return
      }
    }
  }
  // بحث شامل في كل المستويات
  for (const level of levels) {
    for (const sec of level.sections) {
      const lesson = sec.lessons.find(l => l.id === lessonId)
      if (lesson) {
        activeLevel.value = level.id
        openLesson(lesson, sec.id)
        return
      }
    }
  }
}

/* ── Quiz system ── */
const showQuiz = ref(false)
const activeQuiz = ref<{ title: string; questions: QuizQuestion[] } | null>(null)

function hasQuizForSection(sectionId: string): boolean {
  const quiz = getQuizForSection(activeLevel.value, sectionId)
  return !!quiz && quiz.questions.length > 0
}

function startSectionQuiz(sectionId: string, sectionTitle: string) {
  const quiz = getQuizForSection(activeLevel.value, sectionId)
  if (!quiz) return
  activeQuiz.value = {
    title: `اختبار: ${sectionTitle}`,
    questions: quiz.questions,
  }
  showQuiz.value = true
}

function onQuizComplete(score: number, total: number) {
  showQuiz.value = false
  activeQuiz.value = null
}
</script>

<style scoped>
/* ═══════════════════════════════════════════
   Learning Path — Enhanced Design
   ═══════════════════════════════════════════ */

.learning-path {
  max-width: 960px;
  margin: 0 auto;
  padding: 1.5rem 1rem 3rem;
  font-family: 'Segoe UI', Tahoma, sans-serif;
}

/* ── View transition ── */
.view-fade-enter-active,
.view-fade-leave-active {
  transition: opacity 0.25s ease, transform 0.25s ease;
}

/* ── Notes review button ── */
.notes-review-trigger {
  display: flex;
  justify-content: center;
  margin: 1rem 0;
}
.notes-review-btn {
  display: flex;
  align-items: center;
  gap: 0.5rem;
  padding: 0.55rem 1.25rem;
  border: 1px dashed var(--vp-c-divider);
  border-radius: 25px;
  background: var(--vp-c-bg);
  cursor: pointer;
  font-size: 0.85rem;
  color: var(--vp-c-text-2);
  transition: all 0.2s;
}
.notes-review-btn:hover {
  border-color: var(--vp-c-brand-1);
  color: var(--vp-c-text-1);
  background: var(--vp-c-bg-soft);
  border-style: solid;
  box-shadow: 0 2px 8px rgba(0,0,0,0.06);
}

.view-fade-enter-from {
  opacity: 0;
  transform: translateY(12px);
}
.view-fade-leave-to {
  opacity: 0;
  transform: translateY(-8px);
}

/* ═══════════════════════════════════════════
   Header
   ═══════════════════════════════════════════ */
.lp-header {
  position: relative;
  text-align: center;
  padding: 2.5rem 1.5rem 2rem;
  margin-bottom: 2rem;
  border-radius: 20px;
  background: linear-gradient(135deg,
    color-mix(in srgb, var(--vp-c-brand-1) 8%, var(--vp-c-bg)),
    color-mix(in srgb, var(--vp-c-brand-2, #8b5cf6) 6%, var(--vp-c-bg))
  );
  border: 1px solid color-mix(in srgb, var(--vp-c-brand-1) 15%, transparent);
  overflow: hidden;
}

:root.dark .lp-header {
  background: linear-gradient(135deg,
    color-mix(in srgb, var(--vp-c-brand-1) 12%, var(--vp-c-bg)),
    color-mix(in srgb, var(--vp-c-brand-2, #8b5cf6) 8%, var(--vp-c-bg))
  );
  border-color: color-mix(in srgb, var(--vp-c-brand-1) 20%, transparent);
}

.lp-header-content {
  position: relative;
  z-index: 2;
}

/* ── Particles ── */
.lp-particles {
  position: absolute;
  inset: 0;
  z-index: 1;
  pointer-events: none;
  overflow: hidden;
}

.dot {
  position: absolute;
  width: 6px;
  height: 6px;
  border-radius: 50%;
  opacity: 0;
  animation: float-dot 8s ease-in-out infinite;
}

.dot-1  { top: 10%; right: 8%;  background: #10b981; animation-delay: 0s;   animation-duration: 7s; }
.dot-2  { top: 20%; right: 85%; background: #3b82f6; animation-delay: 0.5s; animation-duration: 9s; }
.dot-3  { top: 60%; right: 15%; background: #8b5cf6; animation-delay: 1s;   animation-duration: 6s; }
.dot-4  { top: 75%; right: 75%; background: #f59e0b; animation-delay: 1.5s; animation-duration: 8s; }
.dot-5  { top: 30%; right: 50%; background: #10b981; animation-delay: 2s;   animation-duration: 10s; }
.dot-6  { top: 85%; right: 40%; background: #3b82f6; animation-delay: 0.3s; animation-duration: 7s; }
.dot-7  { top: 15%; right: 30%; background: #ec4899; animation-delay: 1.2s; animation-duration: 9s; }
.dot-8  { top: 50%; right: 90%; background: #8b5cf6; animation-delay: 2.5s; animation-duration: 6s; }
.dot-9  { top: 40%; right: 5%;  background: #f59e0b; animation-delay: 0.8s; animation-duration: 11s; }
.dot-10 { top: 90%; right: 60%; background: #10b981; animation-delay: 1.8s; animation-duration: 8s; }
.dot-11 { top: 5%;  right: 55%; background: #3b82f6; animation-delay: 3s;   animation-duration: 7s; width: 4px; height: 4px; }
.dot-12 { top: 65%; right: 25%; background: #ec4899; animation-delay: 0.2s; animation-duration: 9s; width: 5px; height: 5px; }
.dot-13 { top: 35%; right: 70%; background: #8b5cf6; animation-delay: 1.6s; animation-duration: 10s; width: 4px; height: 4px; }
.dot-14 { top: 80%; right: 10%; background: #10b981; animation-delay: 2.2s; animation-duration: 8s; width: 5px; height: 5px; }
.dot-15 { top: 25%; right: 95%; background: #f59e0b; animation-delay: 0.7s; animation-duration: 7s; width: 4px; height: 4px; }
.dot-16 { top: 55%; right: 45%; background: #3b82f6; animation-delay: 3.2s; animation-duration: 9s; width: 3px; height: 3px; }
.dot-17 { top: 70%; right: 80%; background: #ec4899; animation-delay: 1.4s; animation-duration: 6s; width: 4px; height: 4px; }
.dot-18 { top: 45%; right: 35%; background: #10b981; animation-delay: 2.8s; animation-duration: 11s; width: 5px; height: 5px; }
.dot-19 { top: 10%; right: 65%; background: #8b5cf6; animation-delay: 0.4s; animation-duration: 8s; width: 3px; height: 3px; }
.dot-20 { top: 95%; right: 50%; background: #f59e0b; animation-delay: 1.9s; animation-duration: 7s; }

@keyframes float-dot {
  0%, 100% { opacity: 0; transform: translateY(0) scale(1); }
  20%      { opacity: 0.5; }
  50%      { opacity: 0.35; transform: translateY(-18px) scale(1.3); }
  80%      { opacity: 0.5; }
}

/* ── Badge ── */
.lp-badge {
  display: inline-block;
  padding: 0.25rem 1rem;
  margin-bottom: 0.75rem;
  font-size: 0.75rem;
  font-weight: 600;
  letter-spacing: 0.05em;
  color: var(--vp-c-brand-1);
  background: color-mix(in srgb, var(--vp-c-brand-1) 12%, transparent);
  border: 1px solid color-mix(in srgb, var(--vp-c-brand-1) 25%, transparent);
  border-radius: 100px;
}

/* ── Title ── */
.lp-title {
  font-size: 2.2rem;
  font-weight: 800;
  margin: 0 0 0.5rem;
  line-height: 1.3;
  background: linear-gradient(135deg, #10b981 0%, #3b82f6 40%, #8b5cf6 70%, #ec4899 100%);
  background-size: 200% 200%;
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  background-clip: text;
  animation: gradient-shift 6s ease infinite;
}

@keyframes gradient-shift {
  0%, 100% { background-position: 0% 50%; }
  50%      { background-position: 100% 50%; }
}

.title-deco {
  display: inline-block;
  opacity: 0.4;
  font-weight: 300;
  -webkit-text-fill-color: var(--vp-c-text-3);
}

/* ── Subtitle ── */
.lp-subtitle {
  color: var(--vp-c-text-2);
  font-size: 1.05rem;
  margin: 0 0 1.5rem;
  font-weight: 400;
}

/* ═══════════════════════════════════════════
   Global Progress
   ═══════════════════════════════════════════ */
.lp-global-progress {
  max-width: 420px;
  margin: 0 auto 1.25rem;
}

.gp-row {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 0.4rem;
}

.gp-label {
  font-size: 0.8rem;
  font-weight: 600;
  color: var(--vp-c-text-2);
}

.gp-pct {
  font-size: 0.85rem;
  font-weight: 700;
  color: var(--vp-c-brand-1);
  font-variant-numeric: tabular-nums;
}

.gp-bar {
  position: relative;
  height: 10px;
  background: var(--vp-c-bg-soft);
  border-radius: 10px;
  overflow: hidden;
  margin-bottom: 0.4rem;
  box-shadow: inset 0 1px 3px rgba(0, 0, 0, 0.06);
}

:root.dark .gp-bar {
  box-shadow: inset 0 1px 3px rgba(0, 0, 0, 0.25);
}

.gp-fill {
  position: relative;
  height: 100%;
  border-radius: 10px;
  transition: width 0.6s cubic-bezier(0.4, 0, 0.2, 1);
  box-shadow: 0 0 12px color-mix(in srgb, var(--vp-c-brand-1) 40%, transparent);
  overflow: hidden;
}

.gp-shimmer {
  position: absolute;
  inset: 0;
  background: linear-gradient(
    90deg,
    transparent 0%,
    rgba(255, 255, 255, 0.3) 50%,
    transparent 100%
  );
  animation: shimmer 2.5s ease-in-out infinite;
}

:root.dark .gp-shimmer {
  background: linear-gradient(
    90deg,
    transparent 0%,
    rgba(255, 255, 255, 0.15) 50%,
    transparent 100%
  );
}

@keyframes shimmer {
  0%   { transform: translateX(100%); }
  100% { transform: translateX(-100%); }
}

.gp-detail {
  font-size: 0.78rem;
  color: var(--vp-c-text-3);
}

/* ═══════════════════════════════════════════
   Resume Button
   ═══════════════════════════════════════════ */
.lp-resume {
  display: inline-flex;
  align-items: center;
  gap: 0.6rem;
  margin-top: 0.5rem;
  padding: 0.6rem 1.4rem;
  border: 1px solid color-mix(in srgb, var(--vp-c-brand-1) 30%, transparent);
  border-radius: 12px;
  background: color-mix(in srgb, var(--vp-c-brand-1) 8%, var(--vp-c-bg));
  color: var(--vp-c-brand-1);
  font-size: 0.9rem;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.25s ease;
  font-family: inherit;
  direction: rtl;
}

.lp-resume:hover {
  background: color-mix(in srgb, var(--vp-c-brand-1) 15%, var(--vp-c-bg));
  border-color: var(--vp-c-brand-1);
  transform: translateY(-1px);
  box-shadow: 0 4px 16px color-mix(in srgb, var(--vp-c-brand-1) 20%, transparent);
}

.lp-resume:active {
  transform: translateY(0);
}

.resume-icon {
  display: flex;
  align-items: center;
  justify-content: center;
  width: 28px;
  height: 28px;
  background: var(--vp-c-brand-1);
  color: #fff;
  border-radius: 50%;
  flex-shrink: 0;
}

.resume-icon svg {
  margin-right: -1px;
}

.resume-text {
  white-space: nowrap;
  overflow: hidden;
  text-overflow: ellipsis;
  max-width: 220px;
}

.resume-arrow {
  opacity: 0.5;
  transition: opacity 0.2s, transform 0.2s;
  font-size: 1.1rem;
}

.lp-resume:hover .resume-arrow {
  opacity: 1;
  transform: translateX(-3px);
}

/* ═══════════════════════════════════════════
   Responsive
   ═══════════════════════════════════════════ */
@media (max-width: 640px) {
  .learning-path {
    padding: 1rem 0.75rem 2rem;
  }

  .lp-header {
    padding: 1.75rem 1rem 1.5rem;
    border-radius: 16px;
    margin-bottom: 1.5rem;
  }

  .lp-title {
    font-size: 1.5rem;
  }

  .lp-subtitle {
    font-size: 0.92rem;
  }

  .lp-global-progress {
    max-width: 100%;
  }

  .lp-resume {
    width: 100%;
    justify-content: center;
    font-size: 0.85rem;
    padding: 0.55rem 1rem;
  }

  .resume-text {
    max-width: 160px;
  }

  .dot {
    display: none;
  }
}

@media (max-width: 380px) {
  .lp-title {
    font-size: 1.25rem;
  }

  .title-deco {
    display: none;
  }
}
</style>
