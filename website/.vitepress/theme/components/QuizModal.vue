<script setup lang="ts">
import { ref, computed, onMounted, onUnmounted, watch } from 'vue'

export interface QuizQuestion {
  question: string
  options: string[]
  correct: number
  explanation: string
}

const props = defineProps<{
  title: string
  questions: QuizQuestion[]
  levelColor: string
}>()

const emit = defineEmits<{
  (e: 'close'): void
  (e: 'complete', score: number, total: number): void
}>()

const current = ref(0)
const selected = ref<number | null>(null)
const answered = ref(false)
const score = ref(0)
const finished = ref(false)
const slideDir = ref<'left' | 'right'>('left')
const showCelebration = ref(false)

const q = computed(() => props.questions[current.value])
const progress = computed(() => ((current.value + (answered.value ? 1 : 0)) / props.questions.length) * 100)
const pct = computed(() => Math.round((score.value / props.questions.length) * 100))
const circumference = 2 * Math.PI * 54
const strokeOffset = computed(() => circumference - (circumference * pct.value) / 100)

const resultMessage = computed(() => {
  if (pct.value === 100) return 'ممتاز! 🎯 أتقنت هذا القسم'
  if (pct.value >= 70) return 'أحسنت! 👏 إنجاز رائع'
  if (pct.value >= 50) return 'جيد! 📚 راجع بعض الدروس'
  return 'حاول مجدداً 💪 المحاولة تصنع النجاح'
})

function selectOption(idx: number) {
  if (answered.value) return
  selected.value = idx
  answered.value = true
  if (idx === q.value.correct) {
    score.value++
    showCelebration.value = true
    setTimeout(() => (showCelebration.value = false), 700)
  }
}

function next() {
  if (current.value < props.questions.length - 1) {
    slideDir.value = 'left'
    current.value++
    selected.value = null
    answered.value = false
  } else {
    finished.value = true
    emit('complete', score.value, props.questions.length)
  }
}

function optionClass(idx: number) {
  if (!answered.value) return ''
  if (idx === q.value.correct) return 'correct'
  if (idx === selected.value) return 'incorrect'
  return 'dimmed'
}

function onKey(e: KeyboardEvent) {
  if (e.key === 'Escape') { emit('close'); return }
  if (finished.value) return
  if (!answered.value && e.key >= '1' && e.key <= '4') {
    const idx = parseInt(e.key) - 1
    if (idx < q.value.options.length) selectOption(idx)
  }
  if (answered.value && (e.key === 'Enter' || e.key === ' ')) next()
}

onMounted(() => {
  document.body.style.overflow = 'hidden'
  window.addEventListener('keydown', onKey)
})
onUnmounted(() => {
  document.body.style.overflow = ''
  window.removeEventListener('keydown', onKey)
})
</script>

<template>
  <Teleport to="body">
    <Transition name="modal">
      <div class="quiz-overlay" @click.self="emit('close')">
        <div class="quiz-card" :style="{ '--accent': levelColor }">
          <button class="close-btn" @click="emit('close')" aria-label="إغلاق">✕</button>

          <!-- Progress bar -->
          <div class="progress-track">
            <div class="progress-fill" :style="{ width: progress + '%' }" />
          </div>

          <!-- Questions -->
          <template v-if="!finished">
            <div class="quiz-header">
              <span class="counter">السؤال {{ current + 1 }} من {{ questions.length }}</span>
              <h2 class="quiz-title">{{ title }}</h2>
            </div>

            <TransitionGroup :name="'slide-' + slideDir" tag="div" class="question-area">
              <div :key="current" class="question-block">
                <p class="question-text">{{ q.question }}</p>

                <div class="options">
                  <button
                    v-for="(opt, i) in q.options"
                    :key="i"
                    class="option-btn"
                    :class="optionClass(i)"
                    :disabled="answered"
                    @click="selectOption(i)"
                  >
                    <span class="option-num">{{ i + 1 }}</span>
                    <span class="option-text">{{ opt }}</span>
                    <span v-if="answered && i === q.correct" class="mark">✓</span>
                    <span v-else-if="answered && i === selected && i !== q.correct" class="mark">✗</span>
                  </button>
                </div>

                <Transition name="fade">
                  <div v-if="answered" class="explanation">
                    <p>{{ q.explanation }}</p>
                  </div>
                </Transition>

                <Transition name="fade">
                  <button v-if="answered" class="next-btn" @click="next">
                    {{ current < questions.length - 1 ? 'التالي ←' : 'عرض النتيجة' }}
                  </button>
                </Transition>
              </div>
            </TransitionGroup>

            <!-- Celebration glow -->
            <div v-if="showCelebration" class="celebration" />
          </template>

          <!-- Results -->
          <template v-else>
            <div class="results">
              <h2 class="results-heading">النتيجة</h2>

              <div class="ring-container">
                <svg viewBox="0 0 120 120" class="ring-svg">
                  <circle cx="60" cy="60" r="54" class="ring-bg" />
                  <circle
                    cx="60" cy="60" r="54"
                    class="ring-fg"
                    :style="{
                      strokeDasharray: circumference,
                      strokeDashoffset: strokeOffset,
                      stroke: levelColor
                    }"
                  />
                </svg>
                <span class="ring-pct">{{ pct }}%</span>
              </div>

              <p class="score-line">{{ score }} / {{ questions.length }}</p>
              <p class="result-msg">{{ resultMessage }}</p>

              <button class="close-result-btn" @click="emit('close')">إغلاق</button>
            </div>
          </template>
        </div>
      </div>
    </Transition>
  </Teleport>
</template>

<style scoped>
.quiz-overlay {
  position: fixed;
  inset: 0;
  z-index: 9999;
  display: flex;
  align-items: center;
  justify-content: center;
  background: rgba(0, 0, 0, 0.55);
  backdrop-filter: blur(4px);
  direction: rtl;
}

.quiz-card {
  position: relative;
  width: min(94vw, 560px);
  max-height: 90vh;
  overflow-y: auto;
  background: var(--vp-c-bg);
  border-radius: 16px;
  padding: 32px 28px 28px;
  box-shadow: 0 20px 60px rgba(0, 0, 0, 0.3);
  border: 1px solid var(--vp-c-divider);
}

.close-btn {
  position: absolute;
  top: 12px;
  left: 12px;
  background: none;
  border: none;
  font-size: 20px;
  color: var(--vp-c-text-2);
  cursor: pointer;
  width: 32px;
  height: 32px;
  border-radius: 8px;
  display: flex;
  align-items: center;
  justify-content: center;
  transition: background 0.2s;
}
.close-btn:hover {
  background: var(--vp-c-bg-soft);
  color: var(--vp-c-text-1);
}

/* Progress */
.progress-track {
  height: 4px;
  background: var(--vp-c-bg-soft);
  border-radius: 4px;
  margin-bottom: 24px;
  overflow: hidden;
}
.progress-fill {
  height: 100%;
  background: var(--accent);
  border-radius: 4px;
  transition: width 0.4s ease;
}

/* Header */
.quiz-header { margin-bottom: 20px; }
.counter {
  font-size: 13px;
  color: var(--vp-c-text-3);
  letter-spacing: -0.01em;
}
.quiz-title {
  font-size: 16px;
  font-weight: 600;
  color: var(--accent);
  margin: 4px 0 0;
}

/* Question */
.question-area {
  position: relative;
  overflow: hidden;
}
.question-block { width: 100%; }
.question-text {
  font-size: 17px;
  font-weight: 600;
  color: var(--vp-c-text-1);
  line-height: 1.65;
  margin-bottom: 20px;
}

/* Options */
.options { display: flex; flex-direction: column; gap: 10px; }
.option-btn {
  display: flex;
  align-items: center;
  gap: 12px;
  width: 100%;
  padding: 12px 16px;
  border: 1.5px solid var(--vp-c-divider);
  border-radius: 10px;
  background: var(--vp-c-bg);
  color: var(--vp-c-text-1);
  font-size: 15px;
  cursor: pointer;
  text-align: right;
  transition: all 0.2s;
}
.option-btn:not(:disabled):hover {
  border-color: var(--accent);
  background: color-mix(in srgb, var(--accent) 8%, var(--vp-c-bg));
}
.option-num {
  flex-shrink: 0;
  width: 28px;
  height: 28px;
  border-radius: 8px;
  background: var(--vp-c-bg-soft);
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 13px;
  font-weight: 700;
  color: var(--vp-c-text-2);
}
.option-text { flex: 1; }
.mark { font-size: 18px; font-weight: 700; }

.option-btn.correct {
  border-color: #22c55e;
  background: rgba(34, 197, 94, 0.1);
}
.option-btn.correct .mark { color: #22c55e; }
.option-btn.incorrect {
  border-color: #ef4444;
  background: rgba(239, 68, 68, 0.1);
}
.option-btn.incorrect .mark { color: #ef4444; }
.option-btn.dimmed { opacity: 0.45; }

/* Explanation */
.explanation {
  margin-top: 16px;
  padding: 12px 16px;
  background: var(--vp-c-bg-soft);
  border-radius: 10px;
  border-inline-start: 3px solid var(--accent);
  font-size: 14px;
  color: var(--vp-c-text-2);
  line-height: 1.7;
}

/* Next button */
.next-btn {
  display: block;
  margin: 20px auto 0;
  padding: 10px 32px;
  font-size: 15px;
  font-weight: 600;
  color: #fff;
  background: var(--accent);
  border: none;
  border-radius: 10px;
  cursor: pointer;
  transition: opacity 0.2s, transform 0.15s;
}
.next-btn:hover { opacity: 0.88; transform: scale(1.03); }

/* Celebration */
.celebration {
  position: absolute;
  inset: 0;
  border-radius: 16px;
  pointer-events: none;
  animation: glow 0.7s ease-out forwards;
  box-shadow: inset 0 0 80px rgba(34, 197, 94, 0.25);
}
@keyframes glow {
  0% { opacity: 1; }
  100% { opacity: 0; }
}

/* Results */
.results { text-align: center; padding: 16px 0 8px; }
.results-heading {
  font-size: 20px;
  font-weight: 700;
  color: var(--vp-c-text-1);
  margin-bottom: 24px;
}
.ring-container {
  position: relative;
  width: 130px;
  height: 130px;
  margin: 0 auto 20px;
}
.ring-svg {
  width: 100%;
  height: 100%;
  transform: rotate(-90deg);
}
.ring-bg {
  fill: none;
  stroke: var(--vp-c-bg-soft);
  stroke-width: 8;
}
.ring-fg {
  fill: none;
  stroke-width: 8;
  stroke-linecap: round;
  transition: stroke-dashoffset 1s ease;
}
.ring-pct {
  position: absolute;
  inset: 0;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 28px;
  font-weight: 800;
  color: var(--vp-c-text-1);
}
.score-line {
  font-size: 18px;
  font-weight: 600;
  color: var(--vp-c-text-2);
  margin-bottom: 8px;
}
.result-msg {
  font-size: 16px;
  color: var(--vp-c-text-1);
  margin-bottom: 28px;
}
.close-result-btn {
  padding: 10px 40px;
  font-size: 15px;
  font-weight: 600;
  background: var(--accent);
  color: #fff;
  border: none;
  border-radius: 10px;
  cursor: pointer;
  transition: opacity 0.2s;
}
.close-result-btn:hover { opacity: 0.85; }

/* Transitions */
.modal-enter-active, .modal-leave-active { transition: opacity 0.25s ease; }
.modal-enter-active .quiz-card { animation: card-in 0.3s ease; }
.modal-leave-active .quiz-card { animation: card-in 0.2s ease reverse; }
.modal-enter-from, .modal-leave-to { opacity: 0; }
@keyframes card-in {
  from { transform: scale(0.92); opacity: 0; }
  to { transform: scale(1); opacity: 1; }
}

.fade-enter-active, .fade-leave-active { transition: opacity 0.25s ease; }
.fade-enter-from, .fade-leave-to { opacity: 0; }

.slide-left-enter-active, .slide-left-leave-active {
  transition: transform 0.3s ease, opacity 0.3s ease;
}
.slide-left-enter-from { transform: translateX(-30px); opacity: 0; }
.slide-left-leave-to { transform: translateX(30px); opacity: 0; }
</style>
