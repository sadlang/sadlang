<template>
  <div class="tutorial-container">
    <!-- شريط التقدم -->
    <div class="tutorial-progress">
      <div
        class="progress-bar"
        :style="{ width: progressPct + '%' }"
      />
      <span class="progress-text">
        الخطوة {{ currentStep + 1 }} من {{ steps.length }}
      </span>
    </div>

    <!-- أزرار الخطوات -->
    <div class="step-tabs">
      <button
        v-for="(step, index) in steps"
        :key="index"
        class="step-tab"
        :class="{
          active:    index === currentStep,
          completed: index < currentStep,
        }"
        @click="goToStep(index)"
      >
        <span class="step-num">{{ index + 1 }}</span>
        <span class="step-label">{{ step.title }}</span>
      </button>
    </div>

    <!-- محتوى الخطوة -->
    <div class="step-content">
      <div class="step-explanation">
        <!-- العنوان -->
        <h3 class="step-title">{{ currentStepData.title }}</h3>

        <!-- الشرح (HTML آمن من المطور) -->
        <!-- eslint-disable-next-line vue/no-v-html -->
        <div class="step-desc" v-html="currentStepData.descHtml" />

        <!-- تلميح اختياري -->
        <div v-if="currentStepData.hint && showHint" class="step-hint">
          💡 {{ currentStepData.hint }}
        </div>
        <button
          v-if="currentStepData.hint && !showHint"
          class="hint-btn"
          @click="showHint = true"
        >
          💡 عرض تلميح
        </button>
      </div>

      <!-- المحرر والخرج -->
      <div class="step-editor">
        <div class="editor-toolbar">
          <span class="file-badge">{{ currentStepData.filename || 'تمرين.ص' }}</span>
          <button
            class="reset-btn"
            @click="resetCode"
            title="إعادة الكود الأصلي"
          >↺</button>
          <button
            class="run-step-btn"
            @click="runStepCode"
            :disabled="running"
          >
            {{ running ? '⏳' : '▶ تشغيل' }}
          </button>
        </div>
        <div ref="stepEditorEl" class="step-editor-mount" />
        <div class="step-output" :class="{ error: hasError }">
          <span v-if="!output && !error" class="output-hint">الخرج سيظهر هنا...</span>
          <span v-if="error" class="output-error">{{ error }}</span>
          <span v-if="output">{{ output }}</span>
        </div>
      </div>
    </div>

    <!-- أزرار التنقل -->
    <div class="step-nav">
      <button
        class="nav-btn prev"
        :disabled="currentStep === 0"
        @click="prevStep"
      >
        ← السابق
      </button>

      <div class="step-dots">
        <span
          v-for="(_, i) in steps"
          :key="i"
          class="dot"
          :class="{ active: i === currentStep, done: i < currentStep }"
          @click="goToStep(i)"
        />
      </div>

      <button
        v-if="currentStep < steps.length - 1"
        class="nav-btn next"
        @click="nextStep"
      >
        التالي →
      </button>
      <button
        v-else
        class="nav-btn finish"
        @click="$emit('finished')"
      >
        ✓ إنهاء الدرس
      </button>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, computed, watch, onMounted, onUnmounted } from 'vue'

interface TutorialStep {
  title:      string
  descHtml:   string
  code:       string
  filename?:  string
  hint?:      string
  expected?:  string   // جزء من الخرج المتوقع للتحقق التلقائي
}

const props = defineProps<{ steps: TutorialStep[] }>()
const emit  = defineEmits<{ (e: 'finished'): void }>()

// ── الحالة
const currentStep   = ref(0)
const showHint      = ref(false)
const running       = ref(false)
const output        = ref('')
const error         = ref('')
const stepEditorEl  = ref<HTMLElement | null>(null)
let editorView: any = null

const currentStepData = computed(() => props.steps[currentStep.value])
const progressPct     = computed(() => ((currentStep.value) / (props.steps.length - 1)) * 100)
const hasError        = computed(() => !!error.value)

// ── التنقل
function goToStep(index: number) {
  currentStep.value = index
  showHint.value    = false
  output.value      = ''
  error.value       = ''
}
function nextStep() { if (currentStep.value < props.steps.length - 1) goToStep(currentStep.value + 1) }
function prevStep() { if (currentStep.value > 0) goToStep(currentStep.value - 1) }
function resetCode() {
  if (editorView) {
    editorView.dispatch({
      changes: { from: 0, to: editorView.state.doc.length, insert: currentStepData.value.code }
    })
  }
}

// ── تشغيل الكود
async function runStepCode() {
  if (!editorView) return
  const code = editorView.state.doc.toString()
  running.value = true
  output.value  = ''
  error.value   = ''
  try {
    const w = (window as any).__sadWasm
    if (w?.runCode) {
      const r = w.runCode(code)
      if (r.error) error.value  = r.error
      else          output.value = r.output
    } else {
      const resp = await fetch('/api/run', { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify({ code }) })
      const data = await resp.json()
      if (data.error) error.value  = data.error
      else             output.value = data.output ?? ''
    }
  } catch (e: any) {
    error.value = `خطأ: ${e?.message}`
  } finally {
    running.value = false
  }
}

// ── تزامن المحرر مع الخطوة
watch(currentStep, async () => {
  await initEditor()
})

// ── تهيئة المحرر
async function initEditor() {
  if (!stepEditorEl.value) return
  editorView?.destroy?.()
  stepEditorEl.value.innerHTML = ''
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
        doc: currentStepData.value.code,
        extensions: [
          lineNumbers(), highlightActiveLine(), history(), indentOnInput(),
          syntaxHighlighting(defaultHighlightStyle, { fallback: true }),
          closeBrackets(),
          keymap.of([...closeBracketsKeymap, ...defaultKeymap, ...historyKeymap]),
          oneDark,
          EditorView.theme({
            '&':           { height: '200px', fontSize: '13px' },
            '.cm-content': { direction: 'ltr', textAlign: 'left' },
            '.cm-scroller': { overflow: 'auto', fontFamily: 'JetBrains Mono, monospace' },
          }),
        ],
      }),
      parent: stepEditorEl.value!,
    })
  } catch {
    const ta         = document.createElement('textarea')
    ta.value         = currentStepData.value.code
    ta.style.cssText = 'width:100%;height:200px;background:#0d1117;color:#e6edf3;padding:10px;font-family:monospace;font-size:13px;border:none;resize:none;direction:ltr;'
    stepEditorEl.value!.appendChild(ta)
    editorView = {
      state: { doc: { toString: () => ta.value, length: ta.value.length } },
      dispatch({ changes }: any) { if (changes.insert !== undefined) ta.value = changes.insert },
      destroy() {},
    }
    ta.addEventListener('input', () => { editorView.state.doc = { toString: () => ta.value, length: ta.value.length } })
  }
}

onMounted(initEditor)
onUnmounted(() => editorView?.destroy?.())
</script>

<style scoped>
.tutorial-container {
  border:        1px solid var(--vp-c-border);
  border-radius: 12px;
  overflow:      hidden;
  margin:        24px 0;
}

/* شريط التقدم */
.tutorial-progress {
  background:     var(--vp-c-bg-soft);
  border-bottom:  1px solid var(--vp-c-border);
  padding:        8px 16px;
  display:        flex;
  align-items:    center;
  gap:            12px;
  position:       relative;
  overflow:       hidden;
}
.progress-bar {
  position:   absolute;
  bottom:     0;
  right:      0;
  height:     3px;
  background: var(--sad-primary);
  transition: width 0.4s ease;
}
.progress-text { font-size: 0.78rem; color: var(--vp-c-text-2); font-weight: 600; }

/* أزرار الخطوات */
.step-tabs {
  display:     flex;
  overflow-x:  auto;
  background:  var(--vp-c-bg-soft);
  border-bottom: 1px solid var(--vp-c-border);
  scrollbar-width: none;
}
.step-tab {
  display:     flex;
  align-items: center;
  gap:         6px;
  padding:     10px 16px;
  white-space: nowrap;
  cursor:      pointer;
  border:      none;
  background:  transparent;
  color:       var(--vp-c-text-2);
  font-size:   0.82rem;
  transition:  all 0.15s;
  border-bottom: 2px solid transparent;
}
.step-tab:hover  { color: var(--vp-c-text-1); }
.step-tab.active { color: var(--vp-c-brand-1); border-bottom-color: var(--vp-c-brand-1); font-weight: 700; }
.step-tab.completed .step-num { background: var(--sad-green); color: #fff; }
.step-num {
  display:         flex;
  align-items:     center;
  justify-content: center;
  width:           20px;
  height:          20px;
  border-radius:   50%;
  background:      var(--vp-c-bg-mute);
  font-size:       0.72rem;
  font-weight:     700;
  flex-shrink:     0;
}

/* محتوى الخطوة */
.step-content {
  display: grid;
  grid-template-columns: 1fr 1fr;
  min-height: 360px;
}
@media (max-width:768px) {
  .step-content { grid-template-columns: 1fr; }
}
.step-explanation {
  padding:        24px;
  border-left:    1px solid var(--vp-c-border);
  overflow-y:     auto;
}
.step-title { font-size: 1.1rem; font-weight: 700; margin: 0 0 12px; }
.step-desc  { font-size: 0.9rem; line-height: 1.7; color: var(--vp-c-text-1); }
.step-desc :deep(code) { direction: ltr; }
.step-hint {
  background:    var(--vp-c-brand-soft);
  border:        1px solid var(--vp-c-brand-1);
  border-radius: 8px;
  padding:       10px 14px;
  font-size:     0.85rem;
  margin-top:    14px;
  color:         var(--vp-c-brand-1);
}
.hint-btn {
  background:    none;
  border:        1px dashed var(--vp-c-border);
  border-radius: 6px;
  padding:       5px 12px;
  font-size:     0.8rem;
  cursor:        pointer;
  margin-top:    12px;
  color:         var(--vp-c-text-2);
}
.hint-btn:hover { border-color: var(--vp-c-brand-1); color: var(--vp-c-brand-1); }

/* المحرر */
.step-editor { display: flex; flex-direction: column; overflow: hidden; }
.editor-toolbar {
  background:    var(--vp-c-bg-soft);
  border-bottom: 1px solid var(--vp-c-border);
  padding:       6px 12px;
  display:       flex;
  align-items:   center;
  gap:           8px;
}
.file-badge {
  font-size:   0.75rem;
  color:       var(--vp-c-text-2);
  flex:        1;
}
.reset-btn {
  background:    none;
  border:        none;
  cursor:        pointer;
  font-size:     1.1rem;
  color:         var(--vp-c-text-2);
  padding:       0 4px;
}
.reset-btn:hover { color: var(--vp-c-brand-1); }
.run-step-btn {
  background:    var(--sad-green);
  color:         #fff;
  border:        none;
  border-radius: 5px;
  padding:       4px 14px;
  font-weight:   700;
  font-size:     0.8rem;
  cursor:        pointer;
}
.run-step-btn:hover    { background: #16a34a; }
.run-step-btn:disabled { opacity: 0.5; cursor: not-allowed; }
.step-editor-mount { flex: 1; }
.step-editor-mount :deep(.cm-editor) { height: 100%; }
.step-output {
  background:  var(--vp-c-bg-mute);
  border-top:  1px solid var(--vp-c-border);
  padding:     10px 14px;
  min-height:  70px;
  font-family: var(--vp-font-family-mono);
  font-size:   0.82rem;
  direction:   ltr;
  text-align:  left;
  white-space: pre-wrap;
}
.step-output.error { border-top-color: var(--sad-red); }
.output-hint  { color: var(--vp-c-text-2); font-style: italic; }
.output-error { color: var(--sad-red); }

/* التنقل */
.step-nav {
  background:      var(--vp-c-bg-soft);
  border-top:      1px solid var(--vp-c-border);
  padding:         12px 16px;
  display:         flex;
  align-items:     center;
  justify-content: space-between;
}
.nav-btn {
  padding:       8px 20px;
  border-radius: 7px;
  border:        1px solid var(--vp-c-border);
  background:    var(--vp-c-bg-mute);
  cursor:        pointer;
  font-weight:   700;
  font-size:     0.875rem;
  transition:    all 0.15s;
  color:         var(--vp-c-text-1);
}
.nav-btn:hover:not(:disabled) { border-color: var(--vp-c-brand-1); color: var(--vp-c-brand-1); }
.nav-btn:disabled { opacity: 0.4; cursor: not-allowed; }
.nav-btn.next   { background: var(--vp-c-brand-1); color: #fff; border-color: var(--vp-c-brand-1); }
.nav-btn.finish { background: var(--sad-green); color: #fff; border-color: var(--sad-green); }
.step-dots { display: flex; gap: 6px; }
.dot {
  width:         8px;
  height:        8px;
  border-radius: 50%;
  background:    var(--vp-c-border);
  cursor:        pointer;
  transition:    background 0.2s;
}
.dot.done   { background: var(--sad-green); }
.dot.active { background: var(--vp-c-brand-1); transform: scale(1.3); }
</style>
