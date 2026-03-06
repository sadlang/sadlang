<script setup lang="ts">
import { ref, computed, onMounted } from 'vue'
import { useAchievements } from '../composables/useAchievements'
import type { Achievement } from '../composables/useAchievements'

const { getAchievements, getUnlockedCount } = useAchievements()

const achievements = ref<Achievement[]>([])
const activeFilter = ref<'all' | 'lesson' | 'level' | 'streak' | 'speed'>('all')
const ready = ref(false)

const filters = [
  { key: 'all' as const, label: 'الكل' },
  { key: 'lesson' as const, label: 'الدروس' },
  { key: 'level' as const, label: 'المستويات' },
  { key: 'streak' as const, label: 'السلسلة' },
  { key: 'speed' as const, label: 'السرعة' },
]

const filtered = computed(() => {
  if (activeFilter.value === 'all') return achievements.value
  return achievements.value.filter(a => a.category === activeFilter.value)
})

const unlockedCount = computed(() => achievements.value.filter(a => a.unlocked).length)
const totalCount = computed(() => achievements.value.length)
const allUnlocked = computed(() => totalCount.value > 0 && unlockedCount.value === totalCount.value)

function formatDate(ts: number | null): string {
  if (!ts) return ''
  return new Date(ts).toLocaleDateString('ar-EG', { year: 'numeric', month: 'short', day: 'numeric' })
}

onMounted(() => {
  achievements.value = getAchievements()
  requestAnimationFrame(() => { ready.value = true })
})
</script>

<template>
  <div class="achievements-panel" dir="rtl">
    <div class="achievements-header">
      <h3>🏆 الإنجازات <span class="count">({{ unlockedCount }} / {{ totalCount }})</span></h3>
      <p v-if="allUnlocked" class="all-done">أحسنت! 🎉</p>
    </div>

    <div class="filter-tabs">
      <button
        v-for="f in filters"
        :key="f.key"
        :class="['filter-btn', { active: activeFilter === f.key }]"
        @click="activeFilter = f.key"
      >{{ f.label }}</button>
    </div>

    <div class="badges-grid">
      <div
        v-for="(a, i) in filtered"
        :key="a.id"
        :class="['badge-card', { unlocked: a.unlocked, locked: !a.unlocked }]"
        :style="a.unlocked && ready ? `animation-delay: ${i * 60}ms` : undefined"
      >
        <div class="badge-icon">
          <span class="icon-emoji">{{ a.icon }}</span>
          <span v-if="!a.unlocked" class="lock-overlay">🔒</span>
        </div>
        <div class="badge-info">
          <span class="badge-title">{{ a.title }}</span>
          <span class="badge-desc" :class="{ hidden: !a.unlocked }">{{ a.description }}</span>
          <span v-if="a.unlocked && a.unlockedAt" class="badge-date">{{ formatDate(a.unlockedAt) }}</span>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.achievements-panel {
  background: var(--vp-c-bg-soft);
  border: 1px solid var(--vp-c-divider);
  border-radius: 12px;
  padding: 24px;
  margin: 16px 0;
}

.achievements-header {
  display: flex;
  align-items: center;
  gap: 12px;
  margin-bottom: 16px;
  flex-wrap: wrap;
}
.achievements-header h3 {
  margin: 0;
  font-size: 1.25rem;
  color: var(--vp-c-text-1);
}
.count {
  font-size: 0.9rem;
  color: var(--vp-c-text-3);
  font-weight: 400;
}
.all-done {
  margin: 0;
  font-size: 1rem;
  color: var(--vp-c-brand-1);
  font-weight: 600;
}

/* Filter tabs */
.filter-tabs {
  display: flex;
  gap: 8px;
  margin-bottom: 20px;
  flex-wrap: wrap;
}
.filter-btn {
  padding: 6px 16px;
  border: 1px solid var(--vp-c-divider);
  border-radius: 20px;
  background: var(--vp-c-bg);
  color: var(--vp-c-text-2);
  cursor: pointer;
  font-size: 0.85rem;
  font-family: inherit;
  transition: all 0.2s ease;
}
.filter-btn:hover {
  border-color: var(--vp-c-brand-1);
  color: var(--vp-c-brand-1);
}
.filter-btn.active {
  background: var(--vp-c-brand-1);
  color: #fff;
  border-color: var(--vp-c-brand-1);
}

/* Grid */
.badges-grid {
  display: grid;
  grid-template-columns: repeat(4, 1fr);
  gap: 14px;
}

/* Badge card */
.badge-card {
  display: flex;
  flex-direction: column;
  align-items: center;
  text-align: center;
  padding: 18px 12px;
  border-radius: 10px;
  border: 1px solid var(--vp-c-divider);
  background: var(--vp-c-bg);
  transition: transform 0.2s ease, box-shadow 0.2s ease;
  position: relative;
}

/* Unlocked */
.badge-card.unlocked {
  animation: pop-in 0.35s ease both;
  box-shadow: 0 0 12px rgba(var(--vp-c-brand-1-rgb, 100, 108, 255), 0.15);
}
.badge-card.unlocked:hover {
  transform: translateY(-3px);
  box-shadow: 0 4px 18px rgba(var(--vp-c-brand-1-rgb, 100, 108, 255), 0.25);
}

/* Locked */
.badge-card.locked {
  opacity: 0.55;
  filter: grayscale(0.7);
}
.badge-card.locked:hover {
  opacity: 0.85;
  filter: grayscale(0.3);
}
.badge-card.locked:hover .badge-desc.hidden {
  max-height: 60px;
  opacity: 1;
}

/* Icon */
.badge-icon {
  position: relative;
  font-size: 2rem;
  line-height: 1;
  margin-bottom: 10px;
}
.lock-overlay {
  position: absolute;
  bottom: -4px;
  left: -6px;
  font-size: 0.9rem;
}

/* Info */
.badge-info {
  display: flex;
  flex-direction: column;
  gap: 4px;
}
.badge-title {
  font-weight: 600;
  font-size: 0.88rem;
  color: var(--vp-c-text-1);
}
.badge-desc {
  font-size: 0.78rem;
  color: var(--vp-c-text-3);
  line-height: 1.4;
  transition: max-height 0.3s ease, opacity 0.3s ease;
}
.badge-desc.hidden {
  max-height: 0;
  opacity: 0;
  overflow: hidden;
}
.badge-date {
  font-size: 0.72rem;
  color: var(--vp-c-text-3);
  margin-top: 4px;
}

/* Pop-in animation */
@keyframes pop-in {
  0% { transform: scale(0.7); opacity: 0; }
  70% { transform: scale(1.05); }
  100% { transform: scale(1); opacity: 1; }
}

/* Dark mode */
:root.dark .badge-card.unlocked {
  box-shadow: 0 0 14px rgba(160, 140, 255, 0.18);
}
:root.dark .badge-card.unlocked:hover {
  box-shadow: 0 4px 20px rgba(160, 140, 255, 0.3);
}

/* Responsive */
@media (max-width: 960px) {
  .badges-grid {
    grid-template-columns: repeat(3, 1fr);
  }
}
@media (max-width: 640px) {
  .badges-grid {
    grid-template-columns: repeat(2, 1fr);
  }
  .achievements-panel {
    padding: 16px;
  }
  .badge-card {
    padding: 14px 8px;
  }
}
</style>
