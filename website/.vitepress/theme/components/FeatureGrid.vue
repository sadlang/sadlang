<template>
  <div class="feature-grid">
    <div
      v-for="feat in features"
      :key="feat.title"
      class="feature-item"
    >
      <div class="feat-icon">{{ feat.icon }}</div>
      <h3 class="feat-title">{{ feat.title }}</h3>
      <p class="feat-desc">{{ feat.desc }}</p>
    </div>
  </div>
</template>

<script setup lang="ts">
defineProps<{
  features: { icon: string; title: string; desc: string }[]
}>()
</script>

<style scoped>
.feature-grid {
  display:               grid;
  grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
  gap:                   20px;
  margin:                32px 0;
}
.feature-item {
  background:    var(--vp-c-bg-soft);
  border:        1px solid var(--vp-c-border);
  border-radius: 12px;
  padding:       24px 20px;
  text-align:    center;
  transition:    transform 0.2s, border-color 0.2s;
}
.feature-item:hover {
  transform:    translateY(-4px);
  border-color: var(--vp-c-brand-1);
}
.feat-icon {
  font-size:     2.4rem;
  margin-bottom: 12px;
}
.feat-title {
  font-size:     1rem;
  font-weight:   700;
  margin-bottom: 8px;
  color:         var(--vp-c-text-1);
}
.feat-desc {
  font-size:  0.88rem;
  color:      var(--vp-c-text-2);
  line-height: 1.6;
}
</style>
