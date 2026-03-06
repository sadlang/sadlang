<script setup lang="ts">
import { ref, computed, onMounted } from 'vue'

const props = defineProps<{
  completedCount: number
  totalLessons: number
}>()

const collapsed = ref(false)
const animatedCount = ref(0)
const animatedStreak = ref(0)

const STORAGE_KEY = 'sad-learn-daily'

function getTodayStr(): string {
  return new Date().toISOString().slice(0, 10)
}

function loadDaily(): Record<string, boolean> {
  try {
    return JSON.parse(localStorage.getItem(STORAGE_KEY) || '{}')
  } catch { return {} }
}

function recordToday() {
  const daily = loadDaily()
  daily[getTodayStr()] = true
  localStorage.setItem(STORAGE_KEY, JSON.stringify(daily))
}

function calcStreak(): number {
  const daily = loadDaily()
  const dates = Object.keys(daily).sort().reverse()
  if (!dates.length) return 0
  let streak = 0
  const d = new Date()
  for (let i = 0; i < 365; i++) {
    const key = d.toISOString().slice(0, 10)
    if (daily[key]) { streak++ } else if (i > 0) break
    d.setDate(d.getDate() - 1)
  }
  return streak
}

function calcDaysSinceFirst(): number {
  const daily = loadDaily()
  const dates = Object.keys(daily).sort()
  if (!dates.length) return 1
  const first = new Date(dates[0])
  const now = new Date()
  return Math.max(1, Math.ceil((now.getTime() - first.getTime()) / 86400000) + 1)
}

const streak = ref(0)
const daysSinceFirst = ref(1)

const progressPct = computed(() =>
  props.totalLessons ? Math.round((props.completedCount / props.totalLessons) * 100) : 0
)

const circumference = 2 * Math.PI * 36
const strokeOffset = computed(() =>
  circumference - (circumference * progressPct.value) / 100
)

const achievementLevel = computed(() => {
  const p = progressPct.value
  if (p >= 95) return 'خبير'
  if (p >= 75) return 'محترف'
  if (p >= 50) return 'متقدم'
  if (p >= 30) return 'متوسط'
  if (p >= 10) return 'متعلم'
  return 'مبتدئ'
})

const learningTime = computed(() => {
  const mins = animatedCount.value * 5
  if (mins < 60) return `${mins} دقيقة`
  const h = Math.floor(mins / 60)
  const m = mins % 60
  return m ? `${h} ساعة و ${m} د` : `${h} ساعة`
})

const dailyAvg = computed(() =>
  (animatedCount.value / daysSinceFirst.value).toFixed(1)
)

function animateValue(target: number, setter: (v: number) => void, duration = 800) {
  const start = performance.now()
  const step = (now: number) => {
    const t = Math.min((now - start) / duration, 1)
    const ease = 1 - Math.pow(1 - t, 3)
    setter(Math.round(target * ease))
    if (t < 1) requestAnimationFrame(step)
  }
  requestAnimationFrame(step)
}

onMounted(() => {
  recordToday()
  streak.value = calcStreak()
  daysSinceFirst.value = calcDaysSinceFirst()
  animateValue(props.completedCount, v => (animatedCount.value = v))
  animateValue(streak.value, v => (animatedStreak.value = v))
})
</script>

<template>
  <div class="ls-panel" :class="{ collapsed }">
    <button class="ls-header" @click="collapsed = !collapsed">
      <span class="ls-title">📊 إحصائيات التعلم</span>
      <span class="ls-toggle">{{ collapsed ? '▸' : '▾' }}</span>
    </button>

    <div v-show="!collapsed" class="ls-body">
      <div class="ls-grid">
        <!-- Completed lessons with ring -->
        <div class="ls-card ls-card--ring">
          <svg class="ls-ring" viewBox="0 0 80 80">
            <circle cx="40" cy="40" r="36" class="ls-ring-bg" />
            <circle cx="40" cy="40" r="36" class="ls-ring-fg"
              :stroke-dasharray="circumference"
              :stroke-dashoffset="strokeOffset"
              transform="rotate(-90 40 40)" />
            <text x="40" y="38" class="ls-ring-pct">{{ progressPct }}%</text>
            <text x="40" y="50" class="ls-ring-sub">{{ animatedCount }}/{{ totalLessons }}</text>
          </svg>
          <div class="ls-label">📚 دروس مكتملة</div>
        </div>

        <!-- Achievement level -->
        <div class="ls-card">
          <div class="ls-icon">🏆</div>
          <div class="ls-value">{{ achievementLevel }}</div>
          <div class="ls-label">مستوى الإنجاز</div>
        </div>

        <!-- Streak -->
        <div class="ls-card">
          <div class="ls-icon">{{ animatedStreak > 3 ? '🔥' : '📅' }}</div>
          <div class="ls-value">{{ animatedStreak }} <span class="ls-unit">يوم</span></div>
          <div class="ls-label">سلسلة التعلم</div>
        </div>

        <!-- Learning time -->
        <div class="ls-card">
          <div class="ls-icon">⏱️</div>
          <div class="ls-value ls-value--sm">{{ learningTime }}</div>
          <div class="ls-label">وقت التعلم</div>
        </div>

        <!-- Daily average -->
        <div class="ls-card">
          <div class="ls-icon">📈</div>
          <div class="ls-value">{{ dailyAvg }}</div>
          <div class="ls-label">معدل يومي</div>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.ls-panel {
  direction: rtl;
  border: 1px solid var(--vp-c-divider);
  border-radius: 12px;
  background: var(--vp-c-bg-soft);
  overflow: hidden;
  margin-bottom: 20px;
  transition: box-shadow 0.3s;
}
.ls-panel:hover { box-shadow: 0 2px 12px rgba(0, 0, 0, 0.06); }
:root.dark .ls-panel:hover { box-shadow: 0 2px 12px rgba(0, 0, 0, 0.3); }

.ls-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  width: 100%;
  padding: 14px 20px;
  background: none;
  border: none;
  cursor: pointer;
  font-size: 15px;
  font-weight: 600;
  color: var(--vp-c-text-1);
}
.ls-toggle {
  font-size: 13px;
  color: var(--vp-c-text-3);
  transition: transform 0.2s;
}

.ls-body { padding: 0 16px 16px; }

.ls-grid {
  display: grid;
  grid-template-columns: repeat(2, 1fr);
  gap: 12px;
}
@media (max-width: 640px) {
  .ls-grid { grid-template-columns: 1fr; }
}

.ls-card {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: 4px;
  padding: 16px 12px;
  border-radius: 10px;
  background: var(--vp-c-bg);
  border: 1px solid var(--vp-c-divider);
  transition: transform 0.2s, border-color 0.2s;
  text-align: center;
}
.ls-card:hover {
  transform: translateY(-2px);
  border-color: var(--vp-c-brand-1);
}
.ls-card--ring { grid-row: span 2; }

.ls-icon { font-size: 28px; line-height: 1; }

.ls-value {
  font-size: 22px;
  font-weight: 700;
  color: var(--vp-c-text-1);
}
.ls-value--sm { font-size: 17px; }
.ls-unit {
  font-size: 13px;
  font-weight: 400;
  color: var(--vp-c-text-2);
}

.ls-label {
  font-size: 12px;
  color: var(--vp-c-text-2);
  margin-top: 2px;
}

/* Ring */
.ls-ring { width: 90px; height: 90px; }
.ls-ring-bg {
  fill: none;
  stroke: var(--vp-c-divider);
  stroke-width: 6;
}
.ls-ring-fg {
  fill: none;
  stroke: var(--vp-c-brand-1);
  stroke-width: 6;
  stroke-linecap: round;
  transition: stroke-dashoffset 0.8s cubic-bezier(0.4, 0, 0.2, 1);
}
.ls-ring-pct {
  fill: var(--vp-c-text-1);
  font-size: 14px;
  font-weight: 700;
  text-anchor: middle;
  dominant-baseline: auto;
}
.ls-ring-sub {
  fill: var(--vp-c-text-2);
  font-size: 9px;
  text-anchor: middle;
  dominant-baseline: auto;
}
</style>
