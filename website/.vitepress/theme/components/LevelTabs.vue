<template>
  <div class="level-tabs">
    <button
      v-for="(level, i) in levels"
      :key="level.id"
      class="level-tab"
      :class="{ active: level.id === activeLevel, completed: getProgress(level.id) >= 100 }"
      :style="{ ...tabStyle(level), animationDelay: `${i * 80}ms` }"
      @click="$emit('select', level.id)"
    >
      <span class="tab-icon">{{ level.icon }}</span>
      <div class="tab-info">
        <span class="tab-title">{{ level.title }}</span>
        <span class="tab-subtitle">{{ level.titleEn }}</span>
        <span class="tab-count">{{ getLessonCount(level) }} درس</span>
      </div>
      <div class="tab-progress-ring" :class="{ 'ring-pulse': level.id === activeLevel }">
        <svg viewBox="0 0 36 36" class="progress-ring">
          <path class="ring-bg" d="M18 2.0845a15.9155 15.9155 0 010 31.831 15.9155 15.9155 0 010-31.831" />
          <path class="ring-fill" :style="{ stroke: level.color, strokeDasharray: ringDash(level.id) }"
            d="M18 2.0845a15.9155 15.9155 0 010 31.831 15.9155 15.9155 0 010-31.831" />
        </svg>
        <span v-if="getProgress(level.id) >= 100" class="ring-star">⭐</span>
        <span v-else class="ring-text">{{ getProgress(level.id) }}%</span>
      </div>
      <div class="tab-indicator" />
    </button>
  </div>
</template>

<script setup lang="ts">
import type { Level } from '../lessons'

const props = defineProps<{
  levels: Level[]
  activeLevel: number
  getProgress: (levelId: number) => number
}>()

defineEmits<{
  (e: 'select', levelId: number): void
}>()

function tabStyle(level: Level) {
  const isActive = level.id === props.activeLevel
  return {
    '--tab-color': level.color,
    borderColor: isActive ? level.color : 'transparent',
    background: isActive ? `${level.color}18` : undefined,
  }
}

function getLessonCount(level: Level): number {
  return level.sections.reduce((sum, s) => sum + s.lessons.length, 0)
}

function ringDash(levelId: number): string {
  const pct = props.getProgress(levelId)
  return `${pct}, 100`
}
</script>

<style scoped>
/* --- entrance animation --- */
@keyframes tab-enter {
  from { opacity: 0; transform: translateY(12px); }
  to   { opacity: 1; transform: translateY(0); }
}
@keyframes pulse-ring {
  0%, 100% { filter: drop-shadow(0 0 0 transparent); }
  50%      { filter: drop-shadow(0 0 4px var(--tab-color)); }
}

.level-tabs {
  display: flex;
  gap: 0.5rem;
  overflow-x: auto;
  padding: 0.5rem 0;
  margin-bottom: 1.5rem;
  scrollbar-width: thin;
}

.level-tab {
  position: relative;
  display: flex;
  align-items: center;
  gap: 0.6rem;
  padding: 0.65rem 1rem;
  border: 2px solid transparent;
  border-radius: 14px;
  background: var(--vp-c-bg-soft);
  cursor: pointer;
  transition: border-color 0.3s, background 0.3s, box-shadow 0.3s, transform 0.25s;
  white-space: nowrap;
  min-width: 150px;
  direction: rtl;
  animation: tab-enter 0.4s ease both;
}

/* --- hover glow --- */
.level-tab:hover {
  border-color: var(--tab-color);
  transform: translateY(-2px);
  box-shadow: 0 4px 16px color-mix(in srgb, var(--tab-color) 25%, transparent);
}

/* --- active state --- */
.level-tab.active {
  box-shadow: 0 4px 18px color-mix(in srgb, var(--tab-color) 35%, transparent);
}

/* --- bottom indicator bar --- */
.tab-indicator {
  position: absolute;
  bottom: -2px;
  inset-inline: 20%;
  height: 3px;
  border-radius: 3px;
  background: var(--tab-color);
  transform: scaleX(0);
  transition: transform 0.3s ease;
}
.level-tab.active .tab-indicator { transform: scaleX(1); }

/* --- completed shimmer --- */
.level-tab.completed { border-color: var(--tab-color); }

.tab-icon { font-size: 1.4rem; line-height: 1; }

.tab-info {
  display: flex;
  flex-direction: column;
  text-align: start;
  flex: 1;
  gap: 1px;
}

.tab-title {
  font-size: 0.85rem;
  font-weight: 600;
  color: var(--vp-c-text-1);
}
.tab-subtitle {
  font-size: 0.68rem;
  color: var(--vp-c-text-3);
}
.tab-count {
  font-size: 0.62rem;
  color: var(--vp-c-text-3);
  opacity: 0.8;
}

/* --- progress ring --- */
.tab-progress-ring {
  position: relative;
  width: 36px;
  height: 36px;
  flex-shrink: 0;
}
.ring-pulse {
  animation: pulse-ring 2s ease-in-out infinite;
}

.progress-ring {
  width: 36px;
  height: 36px;
  transform: rotate(-90deg);
}
.ring-bg {
  fill: none;
  stroke: var(--vp-c-divider);
  stroke-width: 3;
}
.ring-fill {
  fill: none;
  stroke-width: 3;
  stroke-linecap: round;
  transition: stroke-dasharray 0.8s cubic-bezier(0.4, 0, 0.2, 1);
}

.ring-text {
  position: absolute;
  inset: 0;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 0.55rem;
  font-weight: 700;
  color: var(--vp-c-text-2);
}
.ring-star {
  position: absolute;
  inset: 0;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 0.85rem;
}

/* --- dark mode --- */
.dark .level-tab {
  background: var(--vp-c-bg-soft);
}
.dark .level-tab:hover {
  background: color-mix(in srgb, var(--tab-color) 12%, var(--vp-c-bg-soft));
}
.dark .level-tab.active {
  background: color-mix(in srgb, var(--tab-color) 15%, var(--vp-c-bg-soft));
}
.dark .ring-bg { stroke: var(--vp-c-bg-alt); }

/* --- mobile: 2-col grid --- */
@media (max-width: 640px) {
  .level-tabs {
    display: grid;
    grid-template-columns: 1fr 1fr;
    overflow-x: visible;
  }
  .level-tab {
    min-width: 0;
    padding: 0.5rem 0.6rem;
  }
  .tab-subtitle { display: none; }
}
</style>
