<template>
  <div class="section-list">
    <!-- حالة فارغة -->
    <div v-if="!sections.length" class="empty-state">
      <span class="empty-icon">📚</span>
      <p class="empty-text">لا توجد أقسام بعد — ترقب المحتوى الجديد!</p>
    </div>

    <div
      v-for="section in sections"
      :key="section.id"
      class="section-card"
      :class="{ 'section-done': isSectionDone(section) }"
    >
      <!-- شريط التمييز الجانبي -->
      <div class="accent-strip" :style="{ background: isSectionDone(section) ? '#22c55e' : levelColor }" />

      <!-- رأس القسم -->
      <button
        class="section-header"
        :class="{ open: openSections[section.id] }"
        @click="toggleSection(section.id)"
      >
        <span class="section-icon">{{ section.icon }}</span>
        <div class="section-info">
          <span class="section-title">{{ section.title }}</span>
          <span class="section-meta">{{ section.lessons.length }} درس</span>
        </div>
        <div class="section-progress-bar">
          <div
            class="sp-fill"
            :style="{
              width: getSectionProgress(section.lessons.map(l => l.id)) + '%',
              background: isSectionDone(section)
                ? '#22c55e'
                : `linear-gradient(90deg, ${levelColor}, ${levelColor}dd)`,
            }"
          />
        </div>
        <span class="section-pct">{{ getSectionProgress(section.lessons.map(l => l.id)) }}%</span>
        <span v-if="isSectionDone(section)" class="done-badge">✓</span>
        <span class="chevron" :class="{ rotated: openSections[section.id] }">▼</span>
      </button>

      <!-- قائمة الدروس -->
      <Transition @enter="onEnter" @leave="onLeave">
        <div v-if="openSections[section.id]" class="lesson-grid">
          <button
            v-for="(lesson, i) in section.lessons"
            :key="lesson.id"
            class="lesson-card"
            :class="{ completed: isCompleted(lesson.id) }"
            :style="{ '--stagger': i }"
            @click="$emit('openLesson', lesson, section.id)"
          >
            <span class="lesson-num" :style="{ background: isCompleted(lesson.id) ? levelColor : undefined }">
              <template v-if="isCompleted(lesson.id)"><span class="check-anim">✓</span></template>
              <template v-else>{{ i + 1 }}</template>
            </span>
            <div class="lesson-info">
              <span class="lesson-title">{{ lesson.title }}</span>
              <span class="lesson-desc">{{ lesson.desc }}</span>
            </div>
            <div class="lesson-tags">
              <span
                v-for="concept in lesson.concepts.slice(0, 3)"
                :key="concept"
                class="tag"
              >{{ concept }}</span>
            </div>
          </button>

          <!-- زر اختبار القسم -->
          <button
            v-if="hasQuiz && hasQuiz(section.id)"
            class="quiz-btn"
            :style="{ '--accent': levelColor }"
            @click.stop="$emit('startQuiz', section.id, section.title)"
          >
            <span class="quiz-icon">📝</span>
            <span class="quiz-label">اختبار: {{ section.title }}</span>
            <span class="quiz-arrow">←</span>
          </button>
        </div>
      </Transition>
    </div>
  </div>
</template>

<script setup lang="ts">
import { reactive } from 'vue'
import type { Section, Lesson } from '../lessons'

const props = defineProps<{
  sections: Section[]
  levelColor: string
  isCompleted: (lessonId: string) => boolean
  getSectionProgress: (lessonIds: string[]) => number
  hasQuiz?: (sectionId: string) => boolean
}>()

defineEmits<{
  (e: 'openLesson', lesson: Lesson, sectionId: string): void
  (e: 'startQuiz', sectionId: string, sectionTitle: string): void
}>()

const openSections = reactive<Record<string, boolean>>({})

function toggleSection(sectionId: string) {
  openSections[sectionId] = !openSections[sectionId]
}

function isSectionDone(section: Section) {
  return section.lessons.length > 0 && section.lessons.every(l => props.isCompleted(l.id))
}

/* ---- JS-hook accordion ---- */
function onEnter(el: Element, done: () => void) {
  const htm = el as HTMLElement
  htm.style.overflow = 'hidden'
  htm.style.height = '0'
  // force reflow
  void htm.offsetHeight
  htm.style.transition = 'height .35s ease, opacity .3s ease'
  htm.style.height = htm.scrollHeight + 'px'
  htm.style.opacity = '1'
  htm.addEventListener('transitionend', function handler() {
    htm.removeEventListener('transitionend', handler)
    htm.style.height = ''
    htm.style.overflow = ''
    done()
  })
}
function onLeave(el: Element, done: () => void) {
  const htm = el as HTMLElement
  htm.style.overflow = 'hidden'
  htm.style.height = htm.scrollHeight + 'px'
  void htm.offsetHeight
  htm.style.transition = 'height .3s ease, opacity .2s ease'
  htm.style.height = '0'
  htm.style.opacity = '0'
  htm.addEventListener('transitionend', function handler() {
    htm.removeEventListener('transitionend', handler)
    done()
  })
}
</script>

<style scoped>
/* ======== القائمة ======== */
.section-list { display: flex; flex-direction: column; gap: 0.85rem; }

/* حالة فارغة */
.empty-state {
  text-align: center; padding: 2.5rem 1rem;
  color: var(--vp-c-text-3);
}
.empty-icon { font-size: 2.5rem; display: block; margin-bottom: .5rem; }
.empty-text { font-size: .95rem; }

/* ======== بطاقة القسم ======== */
.section-card {
  position: relative;
  border: 1px solid var(--vp-c-divider);
  border-radius: 14px;
  overflow: hidden;
  background: var(--vp-c-bg);
  transition: border-color .25s, box-shadow .25s;
}
.section-card:hover { border-color: var(--vp-c-brand-2); }
.section-card.section-done { border-color: #22c55e44; }

/* شريط جانبي ملون */
.accent-strip {
  position: absolute; inset-block: 0; inset-inline-start: 0;
  width: 4px; border-radius: 0 4px 4px 0; z-index: 1;
}

/* ======== رأس القسم ======== */
.section-header {
  display: flex; align-items: center; gap: .85rem;
  width: 100%; padding: 1.1rem 1.35rem 1.1rem 1rem;
  border: none; background: var(--vp-c-bg-soft);
  cursor: pointer; transition: background .2s;
  direction: rtl; text-align: right;
}
.section-header:hover { background: var(--vp-c-bg-alt); }
.section-icon { font-size: 1.6rem; line-height: 1; }
.section-info { flex: 1; display: flex; flex-direction: column; gap: .1rem; }
.section-title { font-weight: 700; font-size: 1.05rem; color: var(--vp-c-text-1); }
.section-meta { font-size: .78rem; color: var(--vp-c-text-3); }

/* شريط التقدم المصغر */
.section-progress-bar {
  width: 68px; height: 6px;
  background: var(--vp-c-bg); border-radius: 3px; overflow: hidden;
}
.sp-fill { height: 100%; border-radius: 3px; transition: width .5s ease; }
.section-pct { font-size: .74rem; color: var(--vp-c-text-2); min-width: 32px; text-align: center; }

.done-badge {
  display: inline-flex; align-items: center; justify-content: center;
  width: 22px; height: 22px; border-radius: 50%;
  background: #22c55e; color: #fff; font-size: .7rem; font-weight: 700;
}

.chevron {
  font-size: .7rem; color: var(--vp-c-text-3);
  transition: transform .3s cubic-bezier(.4,0,.2,1);
}
.chevron.rotated { transform: rotate(180deg); }

/* ======== شبكة الدروس ======== */
.lesson-grid {
  display: flex; flex-direction: column;
  padding: .55rem .65rem; gap: .45rem;
}

/* بطاقة الدرس */
.lesson-card {
  display: flex; align-items: center; gap: .8rem;
  padding: .8rem 1rem;
  border: 1px solid var(--vp-c-divider);
  border-radius: 10px;
  border-right: 3px solid transparent;
  background: var(--vp-c-bg);
  cursor: pointer;
  direction: rtl; text-align: right; width: 100%;
  transition: transform .22s ease, box-shadow .22s ease,
              border-color .22s ease, background .22s ease;
  /* staggered entrance */
  animation: cardIn .3s ease both;
  animation-delay: calc(var(--stagger) * 40ms);
}
@keyframes cardIn {
  from { opacity: 0; transform: translateY(8px); }
  to   { opacity: 1; transform: translateY(0); }
}

.lesson-card:hover {
  transform: translateY(-2px);
  box-shadow: 0 4px 14px rgba(0,0,0,.08);
  border-right-color: var(--vp-c-brand-1);
}

/* حالة مكتمل */
.lesson-card.completed {
  background: color-mix(in srgb, #22c55e 5%, var(--vp-c-bg));
  border-color: #22c55e33;
}
.lesson-card.completed:hover { border-right-color: #22c55e; }

/* رقم الدرس */
.lesson-num {
  width: 30px; height: 30px;
  display: flex; align-items: center; justify-content: center;
  border-radius: 50%; font-size: .78rem; font-weight: 700;
  background: var(--vp-c-bg-soft); color: var(--vp-c-text-2);
  flex-shrink: 0; transition: transform .25s;
}
.lesson-card.completed .lesson-num { color: #fff; }
.check-anim { animation: popIn .35s ease; display: inline-block; }
@keyframes popIn {
  0%   { transform: scale(0); }
  60%  { transform: scale(1.3); }
  100% { transform: scale(1); }
}

/* معلومات الدرس */
.lesson-info { flex: 1; display: flex; flex-direction: column; min-width: 0; gap: .1rem; }
.lesson-title { font-weight: 600; font-size: .92rem; color: var(--vp-c-text-1); }
.lesson-desc {
  font-size: .76rem; color: var(--vp-c-text-3);
  overflow: hidden; text-overflow: ellipsis; white-space: nowrap;
}

/* العلامات */
.lesson-tags { display: flex; gap: .3rem; flex-shrink: 0; flex-wrap: wrap; }
.tag {
  padding: .18rem .45rem; font-size: .62rem; border-radius: 6px;
  background: var(--vp-c-bg-soft); color: var(--vp-c-text-3);
  white-space: nowrap; transition: background .2s;
}
.lesson-card:hover .tag { background: var(--vp-c-bg-alt); }

/* ======== الوضع الداكن ======== */
:root.dark .section-card { border-color: rgba(255,255,255,.08); }
:root.dark .section-card:hover { border-color: var(--vp-c-brand-2); }
:root.dark .lesson-card {
  border-color: rgba(255,255,255,.07);
  background: var(--vp-c-bg-soft);
}
:root.dark .lesson-card:hover {
  box-shadow: 0 4px 18px rgba(0,0,0,.3);
  background: var(--vp-c-bg-alt);
}
:root.dark .lesson-card.completed {
  background: color-mix(in srgb, #22c55e 6%, var(--vp-c-bg-soft));
}
:root.dark .tag { background: rgba(255,255,255,.06); color: var(--vp-c-text-2); }

/* ======== زر الاختبار ======== */
.quiz-btn {
  display: flex; align-items: center; gap: .7rem;
  width: 100%;
  padding: .9rem 1.2rem;
  border: 2px dashed var(--accent, var(--vp-c-brand-1));
  border-radius: 12px;
  background: color-mix(in srgb, var(--accent, var(--vp-c-brand-1)) 6%, var(--vp-c-bg));
  cursor: pointer;
  direction: rtl; text-align: right;
  transition: all .25s ease;
  animation: cardIn .3s ease both;
  animation-delay: calc(var(--stagger, 10) * 40ms);
}
.quiz-btn:hover {
  background: color-mix(in srgb, var(--accent, var(--vp-c-brand-1)) 12%, var(--vp-c-bg));
  border-style: solid;
  transform: translateY(-2px);
  box-shadow: 0 4px 14px rgba(0,0,0,.08);
}
.quiz-icon { font-size: 1.3rem; }
.quiz-label {
  flex: 1;
  font-weight: 700; font-size: .92rem;
  color: var(--accent, var(--vp-c-brand-1));
}
.quiz-arrow {
  font-size: 1.1rem; color: var(--accent, var(--vp-c-brand-1));
  transition: transform .2s;
}
.quiz-btn:hover .quiz-arrow { transform: translateX(-4px); }

:root.dark .quiz-btn {
  background: color-mix(in srgb, var(--accent, var(--vp-c-brand-1)) 8%, var(--vp-c-bg-soft));
  border-color: color-mix(in srgb, var(--accent, var(--vp-c-brand-1)) 40%, transparent);
}
:root.dark .quiz-btn:hover {
  background: color-mix(in srgb, var(--accent, var(--vp-c-brand-1)) 15%, var(--vp-c-bg-soft));
}

/* ======== جوال ======== */
@media (max-width: 640px) {
  .section-header { padding: .9rem 1rem; gap: .6rem; }
  .section-progress-bar { width: 42px; }
  .lesson-card {
    flex-wrap: wrap; padding: .65rem .8rem; gap: .5rem;
    min-height: 52px;
  }
  .lesson-tags {
    width: 100%;
    padding-inline-start: 38px;
    margin-top: -.15rem;
  }
}
</style>
