<template>
  <div class="lesson-wizard" ref="wizardEl">
    <!-- شريط علوي محسّن -->
    <div class="wizard-topbar">
      <button class="back-btn" @click="$emit('close')" aria-label="رجوع">
        <span class="back-icon">✕</span>
        <span class="back-text">رجوع</span>
      </button>
      <div class="topbar-center">
        <span class="lesson-title">{{ lesson.title }}</span>
        <div class="progress-dots">
          <span v-for="i in totalLessons" :key="i" class="pdot"
            :class="{ current: i - 1 === lessonIndex, done: i - 1 < lessonIndex }"
            :style="{ '--clr': levelColor }" />
        </div>
      </div>
      <span class="step-counter">{{ lessonIndex + 1 }}/{{ totalLessons }}</span>
    </div>

    <!-- مؤشر الخطوات المحسّن -->
    <div class="step-indicator">
      <div class="step-track">
        <div class="track-line">
          <div class="track-fill" :style="{ width: (step / 2) * 100 + '%', background: levelColor }" />
        </div>
        <button v-for="(s, i) in steps" :key="i" class="step-circle"
          :class="{ active: step === i, done: step > i }"
          :style="{ '--clr': levelColor }" @click="goToStep(i)">
          <span class="circle-inner">
            <template v-if="step > i">✓</template>
            <template v-else>{{ i + 1 }}</template>
          </span>
          <span class="circle-label">{{ s.label }}</span>
        </button>
      </div>
    </div>

    <!-- محتوى الخطوة مع انتقال انزلاقي -->
    <div class="step-content" ref="contentEl">
      <Transition :name="slideDir" mode="out-in">
        <div :key="step" class="step-card">

          <!-- الشرح -->
          <template v-if="step === 0">
            <p class="desc">{{ lesson.desc }}</p>

            <!-- شرح موسع -->
            <div v-if="lesson.explanation" class="explanation-box">
              <div class="explanation-header">
                <span class="explanation-icon">📘</span>
                <span>شرح مفصّل</span>
              </div>
              <div class="explanation-content" v-html="formatExplanation(lesson.explanation)" />
            </div>

            <div v-if="lesson.tips?.length" class="tips-box">
              <div class="tips-header"><span class="tips-icon">💡</span> نصائح</div>
              <ul>
                <li v-for="(tip, i) in lesson.tips" :key="i">{{ tip }}</li>
              </ul>
            </div>
            <div v-if="lesson.concepts?.length" class="concepts-row">
              <span v-for="c in lesson.concepts" :key="c" class="concept-chip"
                :style="{ '--accent': levelColor }">{{ c }}</span>
            </div>
          </template>

          <!-- الكود -->
          <template v-if="step === 1">
            <div class="section-header"><span>📝</span> مثال تفاعلي</div>
            <SadCodeRunner :initial-code="lesson.code" />
          </template>

          <!-- التمرين -->
          <template v-if="step === 2">
            <div class="exercise-card" :style="{ '--accent': levelColor }">
              <div class="section-header"><span>🎯</span> تمرين</div>
              <p class="exercise-text">{{ lesson.exercise }}</p>
            </div>
            <SadCodeRunner :initial-code="exerciseStarter" />
            <button v-if="lesson.hint" class="hint-btn" @click="showHint = !showHint">
              {{ showHint ? 'إخفاء التلميح' : '🔍 عرض تلميح' }}
            </button>
            <Transition name="hint-reveal">
              <div v-if="showHint" class="hint-box">💡 {{ lesson.hint }}</div>
            </Transition>
          </template>

        </div>
      </Transition>

      <!-- رسالة إتمام -->
      <Transition name="celebrate">
        <div v-if="showCelebration" class="celebration">
          <span class="confetti">🎉</span>
          <span class="celebrate-msg">أحسنت! أكملت الدرس</span>
        </div>
      </Transition>
    </div>

    <!-- لوحة الملاحظات -->
    <NotesPanel
      :lesson-id="lesson.id"
      :lesson-title="lesson.title"
      :section-id="sectionId"
      :level-id="levelId"
      :level-color="levelColor"
    />

    <!-- أزرار التنقل -->
    <div class="wizard-nav">
      <button v-if="lessonIndex > 0" class="nav-btn ghost" @click="$emit('prev')">
        <span class="arrow">⟩</span> الدرس السابق
      </button>
      <div class="spacer" />
      <button v-if="step > 0" class="nav-btn secondary" @click="prevStep">
        <span class="arrow">⟩</span> السابق
      </button>
      <button v-if="step < 2" class="nav-btn primary" :style="{ '--bg': levelColor }" @click="nextStep">
        التالي <span class="arrow">⟨</span>
      </button>
      <button v-else-if="!isCompleted" class="nav-btn primary complete-btn"
        :style="{ '--bg': levelColor }" @click="handleComplete">
        ✓ إكمال الدرس
      </button>
      <button v-else class="nav-btn primary" :style="{ '--bg': levelColor }" @click="$emit('next')">
        الدرس التالي <span class="arrow">⟨</span>
      </button>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, watch, computed, nextTick } from 'vue'
import type { Lesson } from '../lessons'
import SadCodeRunner from './SadCodeRunner.vue'
import NotesPanel from './NotesPanel.vue'

const props = defineProps<{
  lesson: Lesson
  lessonIndex: number
  totalLessons: number
  isCompleted: boolean
  levelColor: string
  sectionId?: string
  levelId?: number
}>()

const emit = defineEmits<{
  (e: 'close'): void
  (e: 'complete'): void
  (e: 'next'): void
  (e: 'prev'): void
}>()

const step = ref(0)
const showHint = ref(false)
const slideDir = ref('slide-left')
const showCelebration = ref(false)
const wizardEl = ref<HTMLElement>()
const contentEl = ref<HTMLElement>()

const steps = [
  { icon: '📖', label: 'الشرح' },
  { icon: '💻', label: 'الكود' },
  { icon: '🎯', label: 'التمرين' },
]

const exerciseStarter = computed(() => `# ${props.lesson.exercise}\n\n`)

/** تحويل النص إلى HTML بسيط (فقرات + نقاط + كود) */
function formatExplanation(text: string): string {
  return text
    .split('\n\n')
    .map(p => {
      const trimmed = p.trim()
      if (!trimmed) return ''
      // نقاط
      if (trimmed.startsWith('- ') || trimmed.startsWith('• ')) {
        const items = trimmed.split('\n').map(l =>
          `<li>${l.replace(/^[-•]\s*/, '')}</li>`
        ).join('')
        return `<ul>${items}</ul>`
      }
      // كود مضمن
      const withCode = trimmed.replace(/`([^`]+)`/g, '<code>$1</code>')
      return `<p>${withCode}</p>`
    })
    .join('')
}

function scrollToTop() {
  nextTick(() => contentEl.value?.scrollTo({ top: 0, behavior: 'smooth' }))
}

function goToStep(i: number) {
  slideDir.value = i > step.value ? 'slide-left' : 'slide-right'
  step.value = i
  scrollToTop()
}

function nextStep() {
  slideDir.value = 'slide-left'
  step.value++
  scrollToTop()
}

function prevStep() {
  slideDir.value = 'slide-right'
  step.value--
  scrollToTop()
}

function handleComplete() {
  showCelebration.value = true
  emit('complete')
  setTimeout(() => { showCelebration.value = false }, 2200)
}

watch(() => props.lesson.id, () => {
  step.value = 0
  showHint.value = false
  showCelebration.value = false
})
</script>

<style scoped>
.lesson-wizard {
  display: flex;
  flex-direction: column;
  background: var(--vp-c-bg);
  border: 1px solid var(--vp-c-divider);
  border-radius: 16px;
  overflow: hidden;
  box-shadow: 0 2px 12px rgba(0,0,0,.06);
}

/* ── شريط علوي ── */
.wizard-topbar {
  display: flex; align-items: center; gap: .75rem;
  padding: .7rem 1.25rem;
  background: var(--vp-c-bg-soft);
  border-bottom: 1px solid var(--vp-c-divider);
  direction: rtl;
}
.back-btn {
  display: flex; align-items: center; gap: .35rem;
  padding: .3rem .7rem; border: 1px solid var(--vp-c-divider);
  border-radius: 8px; background: var(--vp-c-bg);
  cursor: pointer; font-size: .8rem; color: var(--vp-c-text-2);
  transition: all .15s;
}
.back-btn:hover { background: var(--vp-c-bg-alt); color: var(--vp-c-text-1); }
.topbar-center { flex: 1; display: flex; flex-direction: column; gap: .3rem; }
.lesson-title { font-weight: 700; font-size: 1rem; color: var(--vp-c-text-1); }
.progress-dots { display: flex; gap: 4px; }
.pdot {
  width: 6px; height: 6px; border-radius: 50%;
  background: var(--vp-c-divider); transition: all .2s;
}
.pdot.current { background: var(--clr); transform: scale(1.3); }
.pdot.done { background: var(--clr); opacity: .5; }
.step-counter { font-size: .75rem; color: var(--vp-c-text-3); font-variant-numeric: tabular-nums; }

/* ── مؤشر الخطوات ── */
.step-indicator { padding: .85rem 1.25rem; direction: rtl; }
.step-track {
  display: flex; align-items: center; justify-content: center;
  position: relative; gap: 0;
}
.track-line {
  position: absolute; top: 18px; right: calc(16.6% + 18px); left: calc(16.6% + 18px);
  height: 3px; background: var(--vp-c-divider); border-radius: 2px;
}
.track-fill {
  height: 100%; border-radius: 2px;
  transition: width .35s cubic-bezier(.4,0,.2,1);
}
.step-circle {
  display: flex; flex-direction: column; align-items: center; gap: .35rem;
  background: none; border: none; cursor: pointer;
  z-index: 1; flex: 1; padding: 0;
}
.circle-inner {
  width: 36px; height: 36px; border-radius: 50%;
  display: flex; align-items: center; justify-content: center;
  font-size: .85rem; font-weight: 700;
  border: 2.5px solid var(--vp-c-divider);
  background: var(--vp-c-bg); color: var(--vp-c-text-3);
  transition: all .25s;
}
.step-circle.active .circle-inner {
  border-color: var(--clr); background: var(--clr); color: #fff;
  box-shadow: 0 0 0 4px color-mix(in srgb, var(--clr) 18%, transparent);
}
.step-circle.done .circle-inner {
  border-color: var(--clr); color: var(--clr); background: var(--vp-c-bg);
}
.circle-label {
  font-size: .72rem; font-weight: 600;
  color: var(--vp-c-text-3); transition: color .2s;
}
.step-circle.active .circle-label { color: var(--clr); }
.step-circle.done .circle-label { color: var(--vp-c-text-2); }

/* ── محتوى الخطوة ── */
.step-content {
  padding: 1.25rem 1.5rem; min-height: 320px;
  direction: rtl; position: relative; overflow-y: auto;
}
.step-card {
  background: var(--vp-c-bg); border-radius: 12px;
  padding: .25rem 0;
}
.desc {
  font-size: 1.05rem; line-height: 1.85;
  color: var(--vp-c-text-1); margin-bottom: 1.25rem;
}

/* شرح مفصّل */
.explanation-box {
  margin-bottom: 1.25rem;
  padding: 1.25rem 1.5rem;
  background: color-mix(in srgb, var(--vp-c-brand-1) 6%, var(--vp-c-bg-soft));
  border: 1px solid color-mix(in srgb, var(--vp-c-brand-1) 15%, transparent);
  border-radius: 12px;
  border-right: 4px solid var(--vp-c-brand-1);
}
.explanation-header {
  display: flex;
  align-items: center;
  gap: .5rem;
  font-size: .95rem;
  font-weight: 700;
  color: var(--vp-c-brand-1);
  margin-bottom: .75rem;
}
.explanation-icon { font-size: 1.2rem; }
.explanation-content {
  font-size: .92rem;
  line-height: 1.85;
  color: var(--vp-c-text-1);
}
.explanation-content p {
  margin: 0 0 .75rem;
}
.explanation-content p:last-child { margin-bottom: 0; }
.explanation-content ul {
  list-style: none;
  padding: 0;
  margin: 0 0 .75rem;
}
.explanation-content li {
  padding: .25rem 0 .25rem 1.25rem;
  position: relative;
}
.explanation-content li::before {
  content: '◆';
  position: absolute;
  right: 0;
  color: var(--vp-c-brand-1);
  font-size: .6rem;
  top: .45rem;
}
.explanation-content code {
  background: var(--vp-c-bg-alt);
  padding: .15rem .4rem;
  border-radius: 4px;
  font-family: 'JetBrains Mono', monospace;
  font-size: .85rem;
  color: var(--vp-c-brand-1);
  direction: ltr;
  display: inline-block;
}

/* نصائح */
.tips-box {
  padding: 1rem 1.25rem; border-radius: 12px; margin-bottom: 1rem;
  background: color-mix(in srgb, var(--vp-c-warning-1) 8%, var(--vp-c-bg-soft));
  border: 1px solid color-mix(in srgb, var(--vp-c-warning-1) 20%, transparent);
}
.tips-header {
  font-size: .9rem; font-weight: 700; margin-bottom: .5rem;
  color: var(--vp-c-text-1); display: flex; align-items: center; gap: .4rem;
}
.tips-icon { font-size: 1.1rem; }
.tips-box ul { list-style: none; padding: 0; margin: 0; }
.tips-box li {
  padding: .3rem 0 .3rem 0; font-size: .85rem;
  color: var(--vp-c-text-2); position: relative; padding-right: 1rem;
}
.tips-box li::before {
  content: '▸'; position: absolute; right: 0;
  color: var(--vp-c-warning-1); font-weight: 700;
}

/* مفاهيم */
.concepts-row { display: flex; flex-wrap: wrap; gap: .5rem; margin-top: 1rem; }
.concept-chip {
  padding: .3rem .75rem; font-size: .75rem; font-weight: 600;
  border-radius: 20px; color: var(--accent);
  background: color-mix(in srgb, var(--accent) 10%, var(--vp-c-bg-soft));
  border: 1px solid color-mix(in srgb, var(--accent) 30%, transparent);
}

/* عنوان قسم */
.section-header {
  font-size: 1rem; font-weight: 700; margin-bottom: .75rem;
  display: flex; align-items: center; gap: .4rem;
  color: var(--vp-c-text-1);
}

/* التمرين */
.exercise-card {
  padding: 1rem 1.25rem; border-radius: 12px; margin-bottom: 1rem;
  background: color-mix(in srgb, var(--accent) 5%, var(--vp-c-bg-soft));
  border: 1px solid color-mix(in srgb, var(--accent) 20%, transparent);
  border-right: 4px solid var(--accent);
}
.exercise-text {
  font-size: .95rem; line-height: 1.75;
  color: var(--vp-c-text-1); margin: .25rem 0 0;
}

/* تلميح */
.hint-btn {
  margin-top: .75rem; padding: .45rem 1.1rem;
  border: 1px solid var(--vp-c-divider); border-radius: 8px;
  background: var(--vp-c-bg); cursor: pointer;
  font-size: .8rem; color: var(--vp-c-text-2); transition: all .15s;
}
.hint-btn:hover { background: var(--vp-c-bg-soft); color: var(--vp-c-text-1); }
.hint-box {
  margin-top: .6rem; padding: .85rem 1.1rem;
  border-radius: 10px; font-size: .85rem; line-height: 1.6;
  background: color-mix(in srgb, var(--vp-c-warning-1) 12%, var(--vp-c-bg-soft));
  color: var(--vp-c-text-1);
  border: 1px solid color-mix(in srgb, var(--vp-c-warning-1) 25%, transparent);
}

/* ── احتفال ── */
.celebration {
  position: absolute; inset: 0; display: flex;
  flex-direction: column; align-items: center; justify-content: center;
  background: color-mix(in srgb, var(--vp-c-bg) 92%, transparent);
  z-index: 10; border-radius: 12px; pointer-events: none;
}
.confetti { font-size: 3rem; animation: bounce .6s ease infinite alternate; }
.celebrate-msg {
  margin-top: .5rem; font-size: 1.15rem; font-weight: 700;
  color: var(--vp-c-brand-1);
}
@keyframes bounce { to { transform: scale(1.2) rotate(8deg); } }
.celebrate-enter-active { transition: all .35s ease; }
.celebrate-leave-active { transition: all .3s ease; }
.celebrate-enter-from { opacity: 0; transform: scale(.8); }
.celebrate-leave-to { opacity: 0; transform: scale(1.05); }

/* ── أزرار التنقل ── */
.wizard-nav {
  display: flex; align-items: center; gap: .5rem;
  padding: .75rem 1.25rem;
  border-top: 1px solid var(--vp-c-divider); direction: rtl;
}
.spacer { flex: 1; }
.nav-btn {
  padding: .5rem 1.25rem; border-radius: 10px;
  font-size: .85rem; font-weight: 600; cursor: pointer;
  border: none; transition: all .2s; display: flex; align-items: center; gap: .3rem;
}
.nav-btn.primary {
  background: var(--bg); color: #fff;
}
.nav-btn.primary:hover { filter: brightness(1.1); transform: translateY(-1px); }
.nav-btn.complete-btn { animation: pulse-glow 2s ease infinite; }
@keyframes pulse-glow {
  0%, 100% { box-shadow: 0 0 0 0 color-mix(in srgb, var(--bg) 40%, transparent); }
  50% { box-shadow: 0 0 0 6px color-mix(in srgb, var(--bg) 0%, transparent); }
}
.nav-btn.secondary {
  background: var(--vp-c-bg-soft); color: var(--vp-c-text-2);
  border: 1px solid var(--vp-c-divider);
}
.nav-btn.secondary:hover { background: var(--vp-c-bg-alt); color: var(--vp-c-text-1); }
.nav-btn.ghost {
  background: none; color: var(--vp-c-text-3);
}
.nav-btn.ghost:hover { color: var(--vp-c-text-1); background: var(--vp-c-bg-soft); }
.arrow { font-size: .75rem; }

/* ── انتقالات انزلاقية ── */
.slide-left-enter-active, .slide-left-leave-active,
.slide-right-enter-active, .slide-right-leave-active {
  transition: all .28s cubic-bezier(.4,0,.2,1);
}
.slide-left-enter-from { opacity: 0; transform: translateX(-24px); }
.slide-left-leave-to { opacity: 0; transform: translateX(24px); }
.slide-right-enter-from { opacity: 0; transform: translateX(24px); }
.slide-right-leave-to { opacity: 0; transform: translateX(-24px); }

/* hint reveal */
.hint-reveal-enter-active { transition: all .25s ease; }
.hint-reveal-leave-active { transition: all .2s ease; }
.hint-reveal-enter-from { opacity: 0; transform: translateY(-8px); }
.hint-reveal-leave-to { opacity: 0; transform: translateY(-4px); }

/* ── متجاوب ── */
@media (max-width: 640px) {
  .step-content { padding: 1rem; }
  .track-line { right: calc(16.6% + 14px); left: calc(16.6% + 14px); }
  .circle-inner { width: 30px; height: 30px; font-size: .75rem; }
  .wizard-nav { flex-wrap: wrap; gap: .4rem; }
  .nav-btn { flex: 1 1 auto; justify-content: center; }
  .spacer { display: none; }
  .back-text { display: none; }
  .progress-dots { display: none; }
  .lesson-title { font-size: .9rem; }
}
</style>
