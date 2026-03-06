<template>
  <!-- شريط الأمثلة -->
  <div class="pg-examples">
    <span class="pg-examples-label">أمثلة جاهزة:</span>
    <button
      v-for="ex in examples"
      :key="ex.label"
      class="pg-tab"
      :class="{ active: activeExample === ex.label }"
      @click="loadExample(ex)"
    >{{ ex.icon }} {{ ex.label }}</button>
  </div>

  <!-- منطقة التحرير -->
  <div class="pg-workspace">
    <!-- المحرر -->
    <div class="pg-pane editor">
      <div class="pg-header">
        <span>📝 المحرر</span>
        <span class="pg-filename">{{ activeFilename }}</span>
        <div class="pg-mode-badge" :class="execMode">
          {{ execMode === 'wasm' ? '⚡ WASM' : execMode === 'api' ? '🔌 API' : '⏳' }}
        </div>
        <button class="pg-run-btn" @click="runCode" :disabled="running">
          <span v-if="running">⏳ جارٍ التنفيذ...</span>
          <span v-else>▶ تشغيل</span>
        </button>
      </div>
      <div ref="editorEl" class="pg-editor-mount" />
    </div>

    <!-- جانب الخرج والإدخال -->
    <div class="pg-side">
      <!-- stdin -->
      <div class="pg-pane stdin">
        <div class="pg-header">
          <span>⌨️ إدخال stdin </span>
          <span class="pg-hint-small">(اختياري)</span>
        </div>
        <textarea
          v-model="stdinInput"
          class="pg-stdin"
          placeholder="أدخل قيمة اقرأ() هنا..."
          dir="auto"
          spellcheck="false"
        />
      </div>

      <!-- الخرج -->
      <div class="pg-pane output" :class="{ 'has-error': hasError }">
        <div class="pg-header">
          <span>📤 الخرج</span>
          <span v-if="execMs !== null" class="pg-time">{{ execMs }}ms</span>
          <button class="pg-clear" @click="clearOutput">×</button>
        </div>
        <div class="pg-output-body">
          <div v-if="outputLines.length === 0 && !hasError" class="pg-placeholder">
            اضغط "تشغيل" لتنفيذ الكود ✨
          </div>
          <div v-if="hasError" class="pg-error">
            <span class="pg-error-icon">❌</span>
            <pre>{{ errorText }}</pre>
          </div>
          <pre v-if="outputLines.length" class="pg-out-text">{{ outputLines.join('\n') }}</pre>
        </div>
      </div>

      <!-- الرسوميات -->
      <div class="pg-pane graphics">
        <div class="pg-header">
          <span>🎨 الرسوميات Canvas</span>
        </div>
        <div style="background: #fff; display: flex; justify-content: center; align-items: center; padding: 10px;">
          <canvas id="sad-canvas" width="400" height="300" style="border: 1px solid #ccc; max-width: 100%;"></canvas>
        </div>
      </div>
    </div>
  </div>

  <!-- حالة التحميل (WASM) -->
  <div v-if="!ready" class="pg-loading-bar">
    <div class="pg-loading-fill" />
    <span>جارٍ تحميل مفسر لغة ص...</span>
  </div>
</template>

<script setup lang="ts">
import { ref, computed, onMounted, onUnmounted } from 'vue'

// ── مراجع DOM ─────────────────────────────────────────────────────────────
const editorEl = ref<HTMLElement | null>(null)

// ── الحالة ──────────────────────────────────────────────────────────────────
const ready          = ref(false)
const running        = ref(false)
const outputLines    = ref<string[]>([])
const errorText      = ref('')
const execMs         = ref<number | null>(null)
const hasGraphics    = ref(false)
const stdinInput     = ref('')
const activeExample  = ref('مرحباً')
const activeFilename = ref('مرحبا.ص')
const execMode       = ref<'wasm' | 'api' | 'loading'>('loading')
const hasError       = computed(() => !!errorText.value)

// ── طبيعة الـ WASM ──────────────────────────────────────────────────────────
let sadWasm: any = null
let editorView:  any = null

// ── الأمثلة ─────────────────────────────────────────────────────────────────
const examples = [
  {
    label: 'مرحباً', icon: '👋', filename: 'مرحبا.ص',
    code: `# مرحبا بالعالم!
اطبع_سطر("مرحباً يا عالم! 🌍")
اطبع_سطر("أهلاً بلغة ص — لغة البرمجة العربية")
اطبع_سطر("الإصدار: 1.0.0")
`,
  },
  {
    label: 'الفيبوناتشي', icon: '🔢', filename: 'فيبو.ص',
    code: `دالة فيبو(ن)
  إذا (ن <= 1)  ارجع ن  نهاية
  ارجع فيبو(ن - 1) + فيبو(ن - 2)
نهاية

اطبع_سطر("متتالية فيبوناتشي:")
لكل ي في مدى(0، 12)
  اطبع(فيبو(ي) + "  ")
نهاية
`,
  },
  {
    label: 'الأصناف', icon: '🏗️', filename: 'أصناف.ص',
    code: `صنف حيوان
  باني(الاسم، الصوت)
    هذا.الاسم  = الاسم
    هذا.الصوت = الصوت
  نهاية
  دالة تحدث()
    اطبع_سطر(هذا.الاسم + " يقول: " + هذا.الصوت + "!")
  نهاية
نهاية

صنف كلب يرث حيوان
  باني(الاسم)  الأساس(الاسم، "هاو")  نهاية
نهاية

صنف قطة يرث حيوان
  باني(الاسم)  الأساس(الاسم، "مياو")  نهاية
نهاية

متغير حيوانات = [جديد كلب("بوبي")، جديد قطة("مشمش")]
لكل حيوان في حيوانات
  حيوان.تحدث()
نهاية
`,
  },
  {
    label: 'الترتيب', icon: '📊', filename: 'ترتيب.ص',
    code: `# خوارزمية فرز الفقاعات
دالة فرز_فقاعي(مصفوفة)
  متغير حجم = مصفوفة.طول()
  لكل ي في مدى(0، حجم)
    لكل ج في مدى(0، حجم - ي - 1)
      إذا (مصفوفة[ج] > مصفوفة[ج + 1])
        متغير مؤقت = مصفوفة[ج]
        مصفوفة[ج] = مصفوفة[ج + 1]
        مصفوفة[ج + 1] = مؤقت
      نهاية
    نهاية
  نهاية
  ارجع مصفوفة
نهاية

متغير أرقام = [64، 34، 25، 12، 22، 11، 90]
اطبع_سطر("قبل الترتيب: " + أرقام)
متغير مرتبة = فرز_فقاعي(أرقام)
اطبع_سطر("بعد الترتيب: " + مرتبة)
`,
  },
  {
    label: 'معالجة الأخطاء', icon: '🛡️', filename: 'أخطاء.ص',
    code: `دالة قسمة_آمنة(أ، ب)
  حاول
    إذا (ب == 0)
      ارمي("خطأ: لا يمكن القسمة على صفر!")
    نهاية
    ارجع أ / ب
  امسك خطأ
    اطبع_سطر("تم التقاط خطأ: " + خطأ)
    ارجع لاشيء
  نهاية
نهاية

اطبع_سطر(قسمة_آمنة(10، 2))
اطبع_سطر(قسمة_آمنة(5، 0))
اطبع_سطر(قسمة_آمنة(9، 3))
`,
  },
  {    label: 'الرسوميات', icon: '🎨', filename: 'رسومات.ص',
    code: `استورد نظام_رسوميات

متغير عرض = 400
متغير طول = 300

صنف برنامجنا
  باني()
  نهاية

  دالة إعداد()
  نهاية

  دالة تحديث(م)
    نظام_رسوميات.امسح_الشاشة(200، 200، 255، 255)
    نظام_رسوميات.ارسم_مربع(100، 100، 200، 100، 255، 50، 50)
  نهاية
نهاية

متغير ب = جديد برنامجنا()
نظام_رسوميات.شغل(ب)
`,
  },
  {    label: 'async/await', icon: '⚡', filename: 'غيرمتزامن.ص',
    code: `غير_متزامن دالة جلب_بيانات(معرف)
  اطبع_سطر("جارٍ جلب البيانات لـ " + معرف + "...")
  انتظر تأخير(100)
  ارجع "بيانات المستخدم #" + معرف
نهاية

غير_متزامن دالة رئيسية()
  متغير بيانات = انتظر جلب_بيانات(42)
  اطبع_سطر("النتيجة: " + بيانات)
نهاية

رئيسية()
`,
  },
]

// ── تحميل مثال ───────────────────────────────────────────────────────────────
function loadExample(ex: typeof examples[0]) {
  activeExample.value  = ex.label
  activeFilename.value = ex.filename
  if (editorView) {
    editorView.dispatch({
      changes: { from: 0, to: editorView.state.doc.length, insert: ex.code }
    })
  }
  clearOutput()
}

// ── مسح الخرج ────────────────────────────────────────────────────────────────
function clearOutput() {
  outputLines.value = []
  errorText.value   = ''
  execMs.value       = null
}

// ── تشغيل الكود ──────────────────────────────────────────────────────────────
async function runCode() {
  if (!editorView || running.value) return
  const source = editorView.state.doc.toString()
  const stdin  = stdinInput.value
    hasGraphics.value = source.includes('نظام_رسوميات')
  running.value     = true
  outputLines.value = []
  errorText.value   = ''
  execMs.value       = null
  const t0 = performance.now()

  try {
    let result: { success: boolean; output: string; error: string }

    // ── مسار 1: WASM (client-side تلقائي) ───────────────────────────────────
    if (sadWasm && execMode.value === 'wasm') {
      try {
        const jsonPtr = sadWasm.ccall('sad_execute', 'number', ['string', 'string'], [source, stdin])
        const json    = sadWasm.UTF8ToString(jsonPtr)
        sadWasm._free(jsonPtr)
        result = JSON.parse(json)
      } catch {
        result = await callApi(source, stdin)
      }
    } else {
      // ── مسار 2: API server / محاكاة ───────────────────────────────────────
      result = await callApi(source, stdin)
    }

    execMs.value = Math.round(performance.now() - t0)

    if (!result.success && result.error) {
      errorText.value = result.error
    } else {
      outputLines.value = result.output.split('\n')
    }
  } catch (e: any) {
    errorText.value = `خطأ في التنفيذ: ${e?.message ?? e}`
    execMs.value     = Math.round(performance.now() - t0)
  } finally {
    running.value = false
  }
}

// ── استدعاء API المحلي ───────────────────────────────────────────────────────
async function callApi(source: string, stdin: string) {
  const controller = new AbortController()
  const timer = setTimeout(() => controller.abort(), 10_000)
  try {
    const resp = await fetch('http://localhost:3731/execute', {
      method:  'POST',
      headers: { 'Content-Type': 'application/json' },
      body:    JSON.stringify({ source, stdin }),
      signal:  controller.signal,
    })
    clearTimeout(timer)
    return await resp.json()
  } catch {
    clearTimeout(timer)
    // لا يوجد خادم محلي — إرجاع رسالة إرشادية
    return {
      success: false,
      output:  '',
      error:   [
        '⚠️ لتشغيل الكود في المتصفح، ابدأ خادم API المحلي:',
        '',
        '  node website/api/server.js',
        '',
        'أو ابنِ sad.wasm عبر Emscripten:',
        '  انظر website/build-wasm.ps1',
      ].join('\n'),
    }
  }
}

// ── تهيئة CodeMirror 6 ───────────────────────────────────────────────────────
async function initEditor() {
  if (!editorEl.value) return

  try {
    const [
      { EditorState },
      { EditorView, keymap, lineNumbers, highlightActiveLineGutter,
        highlightSpecialChars, drawSelection, dropCursor,
        rectangularSelection, crosshairCursor, highlightActiveLine },
      { defaultKeymap, historyKeymap, history },
      { indentOnInput, syntaxHighlighting, defaultHighlightStyle, bracketMatching },
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
        doc: examples[0].code,
        extensions: [
          lineNumbers(),
          highlightActiveLineGutter(),
          highlightSpecialChars(),
          history(),
          drawSelection(),
          dropCursor(),
          rectangularSelection(),
          crosshairCursor(),
          indentOnInput(),
          syntaxHighlighting(defaultHighlightStyle, { fallback: true }),
          bracketMatching(),
          closeBrackets(),
          highlightActiveLine(),
          keymap.of([
            ...closeBracketsKeymap,
            ...defaultKeymap,
            ...historyKeymap,
            { key: 'Ctrl-Enter', run: () => { runCode(); return true } },
          ]),
          oneDark,
          EditorView.theme({
            '&':            { height: '100%', fontSize: '14px' },
            '.cm-content':  { direction: 'ltr', textAlign: 'left', paddingTop: '10px' },
            '.cm-scroller': { overflow: 'auto', fontFamily: '"JetBrains Mono", monospace' },
          }),
        ],
      }),
      parent: editorEl.value!,
    })
  } catch {
    // fallback: textarea
    const ta = document.createElement('textarea')
    ta.value = examples[0].code
    ta.style.cssText = `width:100%;height:100%;background:#0d1117;color:#e6edf3;
      padding:12px;font-family:monospace;font-size:14px;border:none;resize:none;
      direction:ltr;box-sizing:border-box;`
    editorEl.value!.appendChild(ta)
    editorView = {
      state: { doc: { toString: () => ta.value, length: ta.value.length } },
      dispatch: ({ changes }: any) => {
        if (changes?.insert !== undefined) ta.value = changes.insert
      },
    }
  }
}

// ── تحميل WASM ──────────────────────────────────────────────────────────────
async function loadWasm() {
  try {
    const script = document.createElement('script')
    script.src = '/sad.js'
    await new Promise<void>((res, rej) => {
      script.onload = () => res()
      script.onerror = () => rej()
      document.head.appendChild(script)
    })
    const SadWasm = (window as any).SadWasm
    sadWasm = await SadWasm()
    execMode.value = 'wasm'
  } catch {
    // WASM غير مبني بعد — استخدام API fallback
    execMode.value = 'api'
  }
  ready.value = true
}

onMounted(async () => {
  await initEditor()
  await loadWasm()
})

onUnmounted(() => { editorView?.destroy?.() })
</script>

<style scoped>
/* ── أمثلة ──────────────────────────────────────────────────────────────── */
.pg-examples {
  display:     flex;
  flex-wrap:   wrap;
  align-items: center;
  gap:         6px;
  margin:      0 0 12px;
}
.pg-examples-label {
  font-size:   0.8rem;
  font-weight: 700;
  color:       var(--vp-c-text-2);
  margin-left: 4px;
}
.pg-tab {
  padding:       4px 10px;
  border:        1px solid var(--vp-c-border);
  border-radius: 16px;
  background:    var(--vp-c-bg-soft);
  font-size:     0.8rem;
  cursor:        pointer;
  color:         var(--vp-c-text-1);
  transition:    all 0.15s;
}
.pg-tab:hover, .pg-tab.active {
  background:   var(--vp-c-brand-soft);
  border-color: var(--vp-c-brand-1);
  color:        var(--vp-c-brand-1);
  font-weight:  700;
}

/* ── workspace ──────────────────────────────────────────────────────────── */
.pg-workspace {
  display:            grid;
  grid-template-columns: 1fr 340px;
  gap:                12px;
  height:             460px;
}
@media (max-width: 768px) {
  .pg-workspace { grid-template-columns: 1fr; height: auto; }
}

/* ── pane ────────────────────────────────────────────────────────────────── */
.pg-pane {
  display:       flex;
  flex-direction: column;
  border:        1px solid var(--vp-c-border);
  border-radius: 10px;
  overflow:      hidden;
  background:    var(--vp-c-bg-soft);
}
.pg-pane.editor { min-height: 460px; }
.pg-pane.stdin  { height: 110px; flex-shrink: 0; }
.pg-pane.output { flex: 1; overflow: hidden; }
.pg-pane.has-error { border-color: #f97316; }

/* ── header ─────────────────────────────────────────────────────────────── */
.pg-header {
  display:     flex;
  align-items: center;
  gap:         8px;
  padding:     7px 12px;
  background:  rgba(0,0,0,.15);
  font-size:   0.82rem;
  font-weight: 600;
  border-bottom: 1px solid var(--vp-c-border);
  user-select: none;
}
.pg-filename {
  flex:     1;
  color:    var(--vp-c-text-2);
  font-size: 0.75rem;
}
.pg-hint-small { color: var(--vp-c-text-3); font-size: 0.72rem; font-weight: 400; margin-right: auto; }
.pg-time { color: var(--vp-c-text-2); font-size: 0.75rem; margin-right: auto; }

/* ── mode badge ──────────────────────────────────────────────────────────── */
.pg-mode-badge {
  padding:       2px 8px;
  border-radius: 10px;
  font-size:     0.72rem;
  font-weight:   700;
  background:    var(--vp-c-bg-mute);
  color:         var(--vp-c-text-2);
}
.pg-mode-badge.wasm { background: rgba(34,197,94,.15); color: #22c55e; }
.pg-mode-badge.api  { background: rgba(59,130,246,.15); color: #3b82f6; }

/* ── run btn ─────────────────────────────────────────────────────────────── */
.pg-run-btn {
  padding:       5px 16px;
  background:    var(--vp-c-brand-1);
  color:         #fff;
  border:        none;
  border-radius: 7px;
  font-size:     0.82rem;
  font-weight:   700;
  cursor:        pointer;
  transition:    background 0.15s;
  white-space:   nowrap;
}
.pg-run-btn:disabled { opacity: .5; cursor: default; }
.pg-run-btn:not(:disabled):hover { background: var(--vp-c-brand-2); }

/* ── editor mount ────────────────────────────────────────────────────────── */
.pg-editor-mount { flex: 1; overflow: hidden; }
.pg-editor-mount :deep(.cm-editor) { height: 100%; }

/* ── stdin ───────────────────────────────────────────────────────────────── */
.pg-stdin {
  flex:        1;
  resize:      none;
  background:  transparent;
  border:      none;
  padding:     8px 12px;
  font-size:   0.82rem;
  font-family: monospace;
  color:       var(--vp-c-text-1);
  outline:     none;
}

/* ── side ────────────────────────────────────────────────────────────────── */
.pg-side { display: flex; flex-direction: column; gap: 10px; }

/* ── output body ─────────────────────────────────────────────────────────── */
.pg-output-body {
  flex:       1;
  overflow:   auto;
  padding:    10px 14px;
  font-size:  0.82rem;
}
.pg-placeholder { color: var(--vp-c-text-3); font-style: italic; }
.pg-out-text {
  margin:      0;
  white-space: pre-wrap;
  word-break:  break-word;
  color:       #22c55e;
  font-family: monospace;
  font-size:   0.82rem;
  direction:   ltr;
}
.pg-error { display: flex; align-items: flex-start; gap: 8px; }
.pg-error-icon { font-size: 1.1rem; flex-shrink: 0; }
.pg-error pre {
  margin:      0;
  white-space: pre-wrap;
  color:       #f97316;
  font-size:   0.8rem;
  direction:   ltr;
}

/* ── clear btn ───────────────────────────────────────────────────────────── */
.pg-clear {
  background:    none;
  border:        1px solid var(--vp-c-border);
  border-radius: 4px;
  color:         var(--vp-c-text-2);
  cursor:        pointer;
  padding:       1px 8px;
  font-size:     0.85rem;
  line-height:   1.4;
}
.pg-clear:hover { border-color: #f97316; color: #f97316; }

/* ── loading bar ─────────────────────────────────────────────────────────── */
.pg-loading-bar {
  display:     flex;
  align-items: center;
  gap:         10px;
  padding:     6px 0;
  font-size:   0.8rem;
  color:       var(--vp-c-text-2);
}
.pg-loading-fill {
  width:            120px;
  height:           4px;
  border-radius:    2px;
  background:       var(--vp-c-bg-mute);
  overflow:         hidden;
  position:         relative;
}
.pg-loading-fill::after {
  content:          '';
  position:         absolute;
  inset:            0;
  background:       var(--vp-c-brand-1);
  animation:        pgLoad 1.4s ease-in-out infinite;
  transform-origin: left;
}
@keyframes pgLoad {
  0%   { transform: scaleX(0)   translateX(0); }
  50%  { transform: scaleX(0.6) translateX(50%); }
  100% { transform: scaleX(0)   translateX(200%); }
}
</style>
