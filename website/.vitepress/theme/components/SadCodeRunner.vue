<template>
  <div class="code-runner">
    <!-- شريط العنوان -->
    <div class="runner-header">
      <span class="header-label">
        <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M13 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V9z"/><polyline points="13 2 13 9 20 9"/></svg>
        {{ filename || 'محرر ص' }}
      </span>
      <div class="header-actions">
        <button class="icon-btn" title="نسخ الكود" @click="copyCode">
          <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><rect x="9" y="9" width="13" height="13" rx="2"/><path d="M5 15H4a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h9a2 2 0 0 1 2 2v1"/></svg>
          <span v-if="codeCopied" class="copied-badge">✓</span>
        </button>
      </div>
    </div>

    <!-- المحرر -->
    <div class="editor-wrapper">
      <div ref="editorEl" class="editor-mount" />
    </div>

    <!-- شريط الحالة -->
    <div class="status-bar">
      <span>{{ lineCount }} سطر · {{ charCount }} حرف</span>
      <span class="status-hint">Ctrl+Enter للتشغيل</span>
    </div>

    <!-- أزرار التحكم -->
    <div class="runner-toolbar">
      <button class="run-btn" :disabled="running" title="تشغيل (Ctrl+Enter)" @click="runCode">
        <svg v-if="running" class="spinner" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><circle cx="12" cy="12" r="10" stroke-dasharray="31.4 31.4" stroke-linecap="round"/></svg>
        <template v-else>▶</template>
        {{ running ? 'جارٍ التشغيل...' : 'تشغيل' }}
      </button>
      <button class="reset-btn" @click="resetCode">↺ إعادة</button>
      <span v-if="wasmLoading" class="wasm-status loading">⏳ تحميل المحرك...</span>
      <span v-else-if="wasmLoaded" class="wasm-status ready">● جاهز</span>
    </div>

    <!-- المخرجات -->
    <Transition name="output-fade">
      <div v-if="output || error || hasCanvas" class="output-panel" :class="{ 'has-error': !!error, 'success-flash': successFlash }">
        <div class="output-header">
          <div class="output-tabs">
            <button v-if="output || !hasCanvas" class="tab-btn" :class="{ active: activeTab === 'output' }" @click="activeTab = 'output'">📤 المخرجات</button>
            <button v-if="error" class="tab-btn tab-error" :class="{ active: activeTab === 'error' }" @click="activeTab = 'error'">
              <svg width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><circle cx="12" cy="12" r="10"/><line x1="15" y1="9" x2="9" y2="15"/><line x1="9" y1="9" x2="15" y2="15"/></svg>
              خطأ
            </button>
            <button v-if="hasCanvas" class="tab-btn" :class="{ active: activeTab === 'canvas' }" @click="activeTab = 'canvas'">🎨 رسوميات</button>
          </div>
          <div class="output-actions">
            <button class="icon-btn" title="نسخ المخرجات" @click="copyOutput">
              <svg width="13" height="13" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><rect x="9" y="9" width="13" height="13" rx="2"/><path d="M5 15H4a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h9a2 2 0 0 1 2 2v1"/></svg>
              <span v-if="outputCopied" class="copied-badge">✓</span>
            </button>
            <button class="icon-btn" title="مسح" @click="clearOutput">✕</button>
          </div>
        </div>
        <pre v-if="activeTab === 'output' && output" class="output-text">{{ output }}</pre>
        <pre v-if="activeTab === 'error' && error" class="output-text error-text">{{ error }}</pre>
        <div v-show="hasCanvas" :style="{ display: activeTab === 'canvas' ? 'block' : 'none' }" class="canvas-container">
          <canvas id="canvas" class="sad-canvas"></canvas>
        </div>
      </div>
    </Transition>
  </div>
</template>

<script setup lang="ts">
import { ref, computed, onMounted, onUnmounted, watch, nextTick } from 'vue'
import { useSadWasm } from '../composables/useSadWasm'

const { loaded: wasmLoaded, loading: wasmLoading, loadWasm } = useSadWasm()

const props = defineProps<{
  initialCode: string
  filename?: string
}>()

// Ensure WASM is loading
onMounted(() => { loadWasm() })
const editorEl = ref<HTMLElement>()
const running = ref(false)
const output = ref('')
const error = ref('')
const activeTab = ref<'output' | 'error' | 'canvas'>('output')
const hasCanvas = ref(false)
const codeCopied = ref(false)
const outputCopied = ref(false)
const successFlash = ref(false)

let editorView: any = null

const lineCount = computed(() => editorView?.state?.doc?.lines ?? props.initialCode.split('\n').length)
const charCount = computed(() => editorView?.state?.doc?.length ?? props.initialCode.length)

// ── تهيئة CodeMirror 6 ──
async function initEditor() {
  if (!editorEl.value) return
  editorView?.destroy?.()
  editorEl.value.innerHTML = ''

  try {
    const [
      { EditorState },
      { EditorView, keymap, lineNumbers, highlightActiveLine },
      { defaultKeymap, history, historyKeymap },
      { indentOnInput, syntaxHighlighting, defaultHighlightStyle },
      { closeBrackets, closeBracketsKeymap },
      { oneDark },
    ] = await Promise.all([
      import('@codemirror/state'),
      import('@codemirror/view'),
      import('@codemirror/commands'),
      import('@codemirror/language'),
      import('@codemirror/autocomplete'),
      import('@codemirror/theme-one-dark'),
    ])

    editorView = new EditorView({
      state: EditorState.create({
        doc: props.initialCode,
        extensions: [
          lineNumbers(),
          highlightActiveLine(),
          history(),
          indentOnInput(),
          syntaxHighlighting(defaultHighlightStyle, { fallback: true }),
          closeBrackets(),
          keymap.of([
            ...closeBracketsKeymap,
            ...defaultKeymap,
            ...historyKeymap,
          ]),
          oneDark,
          EditorView.theme({
            '&': { height: '200px', fontSize: '13px' },
            '.cm-content': { direction: 'ltr', textAlign: 'left' },
            '.cm-scroller': {
              overflow: 'auto',
              fontFamily: 'JetBrains Mono, monospace',
            },
          }),
          // Ctrl+Enter = run
          keymap.of([{ key: 'Ctrl-Enter', run: () => { runCode(); return true } }]),
        ],
      }),
      parent: editorEl.value!,
    })
  } catch {
    // Fallback: textarea
    const ta = document.createElement('textarea')
    ta.value = props.initialCode
    ta.style.cssText =
      'width:100%;height:200px;background:#0d1117;color:#e6edf3;padding:10px;font-family:monospace;font-size:13px;border:none;resize:none;direction:ltr;border-radius:8px;'
    editorEl.value!.appendChild(ta)
    editorView = {
      state: { doc: { toString: () => ta.value, length: ta.value.length, lines: ta.value.split('\n').length } },
      dispatch({ changes }: any) {
        if (changes?.insert !== undefined) ta.value = changes.insert
      },
      destroy() {},
    }
    ta.addEventListener('input', () => {
      editorView.state.doc = {
        toString: () => ta.value,
        length: ta.value.length,
        lines: ta.value.split('\n').length,
      }
    })
  }
}

// ── تشغيل الكود ──
async function runCode() {
  if (!editorView) return
  const code = editorView.state.doc.toString()
  running.value = true
  output.value = ''
  error.value = ''
  successFlash.value = false

  try {
    const w = (window as any).__sadWasm
    if (w?.runCode) {
      const r = w.runCode(code)
      if (r.error) error.value = r.error
      else output.value = r.output
    } else {
      const resp = await fetch('/api/run', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ code }),
      })
      const data = await resp.json()
      if (data.error) error.value = data.error
      else output.value = data.output ?? ''
    }
  } catch (e: any) {
    error.value = `خطأ: ${e?.message}`
  } finally {
    running.value = false
    activeTab.value = error.value ? 'error' : 'output'
    if (!error.value && output.value) {
      await nextTick()
      successFlash.value = true
      setTimeout(() => { successFlash.value = false }, 600)
    }
  }
}

// ── نسخ الكود ──
async function copyCode() {
  const code = editorView?.state?.doc?.toString() ?? props.initialCode
  await navigator.clipboard.writeText(code)
  codeCopied.value = true
  setTimeout(() => { codeCopied.value = false }, 1500)
}

// ── نسخ المخرجات ──
async function copyOutput() {
  const text = activeTab.value === 'error' ? error.value : output.value
  await navigator.clipboard.writeText(text)
  outputCopied.value = true
  setTimeout(() => { outputCopied.value = false }, 1500)
}

// ── مسح المخرجات ──
function clearOutput() {
  output.value = ''
  error.value = ''
}

// ── إعادة الكود الأصلي ──
function resetCode() {
  output.value = ''
  error.value = ''
  editorView?.dispatch({
    changes: {
      from: 0,
      to: editorView.state.doc.length,
      insert: props.initialCode,
    },
  })
}

// ── مراقبة تغيير الكود الأولي ──
watch(() => props.initialCode, () => {
  resetCode()
})

onMounted(initEditor)
onUnmounted(() => editorView?.destroy?.())
</script>

<style scoped>
.code-runner {
  border: 1px solid var(--vp-c-divider);
  border-radius: 10px;
  overflow: hidden;
  background: var(--vp-c-bg);
  box-shadow: 0 1px 3px rgba(0,0,0,.06);
  transition: box-shadow .2s;
}
.code-runner:hover { box-shadow: 0 2px 8px rgba(0,0,0,.1); }

/* شريط العنوان */
.runner-header {
  display: flex; align-items: center; justify-content: space-between;
  padding: .4rem .75rem; background: var(--vp-c-bg-soft);
  border-bottom: 1px solid var(--vp-c-divider); direction: rtl;
}
.header-label {
  display: flex; align-items: center; gap: .35rem;
  font-size: .78rem; font-weight: 600; color: var(--vp-c-text-2);
}
.header-actions { display: flex; gap: .25rem; }

.icon-btn {
  position: relative; display: inline-flex; align-items: center; gap: .15rem;
  background: none; border: 1px solid transparent; border-radius: 5px;
  cursor: pointer; color: var(--vp-c-text-3); padding: .2rem .35rem;
  font-size: .72rem; transition: all .15s;
}
.icon-btn:hover { background: var(--vp-c-bg-alt); color: var(--vp-c-text-1); border-color: var(--vp-c-divider); }
.copied-badge {
  position: absolute; top: -6px; right: -6px;
  background: #22c55e; color: #fff; font-size: .55rem;
  border-radius: 50%; width: 14px; height: 14px;
  display: flex; align-items: center; justify-content: center;
}

/* المحرر */
.editor-wrapper { border-bottom: 1px solid var(--vp-c-divider); }
.editor-mount { min-height: 120px; }

/* شريط الحالة */
.status-bar {
  display: flex; align-items: center; justify-content: space-between;
  padding: .2rem .75rem; background: var(--vp-c-bg-soft);
  border-bottom: 1px solid var(--vp-c-divider);
  font-size: .68rem; color: var(--vp-c-text-3); direction: rtl;
}
.status-hint {
  font-family: 'JetBrains Mono', monospace; font-size: .62rem;
  background: var(--vp-c-bg-alt); padding: .1rem .4rem;
  border-radius: 3px; border: 1px solid var(--vp-c-divider);
}

/* أزرار التحكم */
.runner-toolbar {
  display: flex; gap: .5rem; padding: .5rem .75rem;
  background: var(--vp-c-bg-soft); direction: rtl;
}
.run-btn {
  display: inline-flex; align-items: center; gap: .35rem;
  padding: .45rem 1.3rem; border: none; border-radius: 7px;
  background: #22c55e; color: #fff; font-weight: 700;
  font-size: .85rem; cursor: pointer; transition: all .2s;
}
.run-btn:hover:not(:disabled) { background: #16a34a; transform: translateY(-1px); }
.run-btn:active:not(:disabled) { transform: translateY(0); }
.run-btn:disabled { opacity: .55; cursor: not-allowed; }

.spinner {
  animation: spin .8s linear infinite;
}
@keyframes spin { to { transform: rotate(360deg); } }

.reset-btn {
  padding: .4rem .9rem; border: 1px solid var(--vp-c-divider);
  border-radius: 7px; background: var(--vp-c-bg); color: var(--vp-c-text-2);
  font-size: .8rem; cursor: pointer; transition: all .15s;
}
.reset-btn:hover { background: var(--vp-c-bg-alt); }

/* المخرجات */
.output-panel { border-top: 1px solid var(--vp-c-divider); transition: border-color .3s; }
.output-panel.success-flash { border-top-color: #22c55e; box-shadow: inset 0 2px 8px rgba(34,197,94,.12); }

.output-header {
  display: flex; align-items: center; justify-content: space-between;
  padding: .3rem .75rem; background: var(--vp-c-bg-soft);
  border-bottom: 1px solid var(--vp-c-divider); direction: rtl;
}
.output-tabs { display: flex; gap: .15rem; }
.tab-btn {
  display: inline-flex; align-items: center; gap: .25rem;
  padding: .25rem .6rem; border: none; border-radius: 5px;
  background: transparent; color: var(--vp-c-text-3);
  font-size: .72rem; font-weight: 600; cursor: pointer; transition: all .15s;
}
.tab-btn:hover { background: var(--vp-c-bg-alt); }
.tab-btn.active { background: var(--vp-c-bg); color: var(--vp-c-text-1); box-shadow: 0 1px 2px rgba(0,0,0,.06); }
.tab-btn.tab-error { color: var(--vp-c-text-3); }
.tab-btn.tab-error.active { color: #ef4444; }
.output-actions { display: flex; gap: .25rem; }

.output-text {
  margin: 0; padding: .75rem;
  font-family: 'JetBrains Mono', monospace; font-size: .82rem;
  line-height: 1.6; direction: ltr; text-align: left;
  white-space: pre-wrap; max-height: 220px; overflow-y: auto;
  scroll-behavior: smooth; background: var(--vp-c-bg); color: var(--vp-c-text-1);
}
.error-text {
  color: #ef4444; border-right: 3px solid #ef4444;
  background: rgba(239,68,68,.04);
}
.canvas-container {
  padding: .5rem;
  background: #111;
  display: flex;
  justify-content: center;
  align-items: center;
  overflow: hidden;
}
.sad-canvas {
  max-width: 100%;
  border-radius: 4px;
}
.canvas-hint {
  color: #888;
  font-size: .8rem;
}

/* انتقال المخرجات */
.output-fade-enter-active { transition: all .25s ease-out; }
.output-fade-leave-active { transition: all .15s ease-in; }
.output-fade-enter-from { opacity: 0; transform: translateY(-6px); }
.output-fade-leave-to { opacity: 0; }

/* الجوال */
@media (max-width: 640px) {
  .runner-toolbar { flex-wrap: wrap; }
  .run-btn, .reset-btn { flex: 1; justify-content: center; font-size: .8rem; }
  .status-hint { display: none; }
  .runner-header { padding: .35rem .5rem; }
  .output-text { font-size: .75rem; padding: .5rem; }
  .wasm-status { display: none; }
}

/* WASM status */
.wasm-status {
  margin-right: auto;
  font-size: .7rem;
  padding: .2rem .5rem;
  border-radius: 4px;
}
.wasm-status.loading {
  color: var(--vp-c-text-3);
  animation: pulse 1.5s infinite;
}
.wasm-status.ready {
  color: #22c55e;
}
@keyframes pulse {
  0%, 100% { opacity: 1; }
  50% { opacity: .4; }
}
</style>
