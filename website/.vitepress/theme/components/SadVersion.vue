<template>
  <div class="sad-version">
    <span class="version-label">الإصدار</span>
    <a
      class="version-tag"
      href="https://github.com/sad-lang/sad-language/releases"
      target="_blank"
      rel="noopener"
    >
      v{{ version }}
    </a>
  </div>
</template>

<script setup lang="ts">
const version = '1.0.0'
</script>

<style scoped>
.sad-version {
  display:     inline-flex;
  align-items: center;
  gap:         8px;
  font-size:   0.85rem;
}
.version-label {
  color: var(--vp-c-text-2);
}
.version-tag {
  background:    var(--vp-c-brand-soft);
  color:         var(--vp-c-brand-1);
  border:        1px solid var(--vp-c-brand-1);
  padding:       2px 10px;
  border-radius: 99px;
  font-weight:   700;
  text-decoration: none;
}
.version-tag:hover {
  background: var(--vp-c-brand-1);
  color:      #fff;
}
</style>
