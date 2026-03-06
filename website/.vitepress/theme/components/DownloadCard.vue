<template>
  <div class="download-card">
    <div class="platform-icon">{{ icon }}</div>
    <h3>{{ platform }}</h3>
    <span class="version-badge">v{{ version }} • {{ arch }}</span>
    <ul class="file-list">
      <li v-for="file in files" :key="file.name">
        <a :href="file.url" class="download-btn" :download="file.name">
          ⬇ {{ file.label }}
          <span class="file-size">({{ file.size }})</span>
        </a>
      </li>
    </ul>
    <p v-if="sha256" class="sha-line">SHA256: {{ sha256 }}</p>
    <div v-if="installCmd" class="install-cmd">
      <code>{{ installCmd }}</code>
      <button class="copy-btn" @click="copy(installCmd)" :title="copied ? 'تم النسخ!' : 'نسخ'">
        {{ copied ? '✓' : '⧉' }}
      </button>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref } from 'vue'

interface FileEntry {
  name:  string
  label: string
  url:   string
  size:  string
}

defineProps<{
  icon:       string
  platform:   string
  version:    string
  arch:       string
  files:      FileEntry[]
  sha256?:    string
  installCmd?: string
}>()

const copied = ref(false)
function copy(text: string) {
  navigator.clipboard?.writeText(text)
  copied.value = true
  setTimeout(() => (copied.value = false), 1800)
}
</script>

<style scoped>
.file-list {
  list-style: none;
  padding:    0;
  margin:     12px 0 0;
  display:    flex;
  flex-direction: column;
  gap:        8px;
}
.download-btn {
  display:         flex;
  align-items:     center;
  justify-content: space-between;
  background:      var(--vp-c-brand-1);
  color:           #fff;
  padding:         9px 14px;
  border-radius:   7px;
  text-decoration: none;
  font-weight:     700;
  font-size:       0.88rem;
  transition:      background 0.2s;
}
.download-btn:hover { background: var(--vp-c-brand-2); }
.file-size { font-weight: 400; font-size: 0.78rem; opacity: 0.85; }

.install-cmd {
  display:       flex;
  align-items:   center;
  background:    var(--vp-c-bg-mute);
  border:        1px solid var(--vp-c-border);
  border-radius: 6px;
  padding:       6px 10px;
  margin-top:    12px;
  font-size:     0.78rem;
  direction:     ltr;
  text-align:    left;
  overflow:      hidden;
  gap:           8px;
}
.install-cmd code {
  flex:         1;
  background:   transparent;
  border:       none;
  padding:      0;
  font-size:    0.78rem;
  overflow:     hidden;
  text-overflow: ellipsis;
  white-space:  nowrap;
}
.copy-btn {
  background:    none;
  border:        none;
  cursor:        pointer;
  color:         var(--vp-c-brand-1);
  font-size:     1rem;
  padding:       0 4px;
  line-height:   1;
}
</style>
