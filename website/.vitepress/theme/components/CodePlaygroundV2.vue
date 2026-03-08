<template>
  <!-- 
    بسم الله الرحمن الرحيم
    Playground v2 — لغة ص
    الميزات الجديدة: multi-file, saved projects, sharing
    @author عمر — مهندس البنية التحتية
    @date 2026-03-06
  -->

  <!-- شريط الأدوات العلوي -->
  <div class="pg2-toolbar">
    <div class="pg2-left">
      <button class="pg2-btn" @click="newProject" title="مشروع جديد">
        📄 جديد
      </button>
      <button class="pg2-btn" @click="openSavedProjects" title="فتح مشروع محفوظ">
        📂 فتح
      </button>
      <button class="pg2-btn" @click="saveProject" title="حفظ المشروع">
        💾 حفظ
      </button>
      <button class="pg2-btn primary" @click="shareProject" title="مشاركة">
        🔗 مشاركة
      </button>
    </div>
    <div class="pg2-center">
      <input 
        v-model="projectName" 
        class="pg2-project-name" 
        placeholder="اسم المشروع"
        dir="auto"
      />
    </div>
    <div class="pg2-right">
      <button class="pg2-btn run" @click="runCode" :disabled="running">
        <span v-if="running">⏳</span>
        <span v-else>▶</span>
        {{ running ? 'جارٍ التنفيذ...' : 'تشغيل' }}
      </button>
    </div>
  </div>

  <!-- شريط الملفات (Tabs) -->
  <div class="pg2-tabs">
    <div 
      v-for="(file, idx) in files" 
      :key="file.id"
      class="pg2-tab"
      :class="{ active: activeFileId === file.id }"
      @click="switchFile(file.id)"
    >
      <span class="pg2-tab-name">{{ file.name }}</span>
      <button 
        v-if="files.length > 1"
        class="pg2-tab-close" 
        @click.stop="closeFile(file.id)"
        title="إغلاق"
      >×</button>
    </div>
    <button class="pg2-tab-add" @click="addFile" title="ملف جديد">+</button>
  </div>

  <!-- منطقة العمل الرئيسية -->
  <div class="pg2-workspace">
    <!-- المحرر -->
    <div class="pg2-editor-pane">
      <div ref="editorEl" class="pg2-editor-mount" />
    </div>

    <!-- جانب الخرج -->
    <div class="pg2-output-pane">
      <!-- stdin -->
      <div class="pg2-section stdin">
        <div class="pg2-section-header">⌨️ إدخال stdin</div>
        <textarea
          v-model="stdinInput"
          class="pg2-stdin"
          placeholder="أدخل قيمة اقرأ() هنا..."
          dir="auto"
          rows="3"
        />
      </div>

      <!-- الخرج -->
      <div class="pg2-section output" :class="{ error: hasError }">
        <div class="pg2-section-header">
          📤 الخرج
          <span v-if="execMs !== null" class="pg2-time">{{ execMs }}ms</span>
        </div>
        <div class="pg2-output-body">
          <pre v-if="hasError" class="pg2-error">{{ errorText }}</pre>
          <pre v-else-if="outputLines.length" class="pg2-output-text">{{ outputLines.join('\n') }}</pre>
          <div v-else class="pg2-placeholder">اضغط "تشغيل" لتنفيذ الكود ✨</div>
        </div>
      </div>
    </div>
  </div>

  <!-- نافذة المشاريع المحفوظة -->
  <div v-if="showSavedProjects" class="pg2-modal-overlay" @click.self="showSavedProjects = false">
    <div class="pg2-modal">
      <div class="pg2-modal-header">
        📂 المشاريع المحفوظة
        <button class="pg2-modal-close" @click="showSavedProjects = false">×</button>
      </div>
      <div class="pg2-modal-body">
        <div v-if="savedProjects.length === 0" class="pg2-empty">
          لا توجد مشاريع محفوظة
        </div>
        <div v-for="proj in savedProjects" :key="proj.id" class="pg2-project-item">
          <div class="pg2-project-info">
            <span class="pg2-project-item-name">{{ proj.name }}</span>
            <span class="pg2-project-item-date">{{ formatDate(proj.updatedAt) }}</span>
            <span class="pg2-project-item-files">{{ proj.files.length }} ملف</span>
          </div>
          <div class="pg2-project-actions">
            <button @click="loadProject(proj)">فتح</button>
            <button @click="deleteProject(proj.id)" class="danger">حذف</button>
          </div>
        </div>
      </div>
    </div>
  </div>

  <!-- نافذة المشاركة -->
  <div v-if="showShareModal" class="pg2-modal-overlay" @click.self="showShareModal = false">
    <div class="pg2-modal">
      <div class="pg2-modal-header">
        🔗 مشاركة المشروع
        <button class="pg2-modal-close" @click="showShareModal = false">×</button>
      </div>
      <div class="pg2-modal-body">
        <p>انسخ الرابط التالي لمشاركة مشروعك:</p>
        <div class="pg2-share-url-box">
          <input 
            ref="shareUrlInput"
            :value="shareUrl" 
            readonly 
            class="pg2-share-url"
          />
          <button @click="copyShareUrl" class="pg2-copy-btn">
            {{ copied ? '✓ تم النسخ' : '📋 نسخ' }}
          </button>
        </div>
        <p class="pg2-share-note">
          ⚠️ الرابط يحتوي على الكود مشفراً — للمشاريع الكبيرة، استخدم خيار "حفظ على الخادم"
        </p>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, computed, onMounted, onUnmounted, watch } from 'vue'

// ══════════════════════════════════════════════════════════════════════════════
//  الأنواع
// ══════════════════════════════════════════════════════════════════════════════
interface ProjectFile {
  id: string
  name: string
  content: string
}

interface Project {
  id: string
  name: string
  files: ProjectFile[]
  mainFileId: string
  createdAt: number
  updatedAt: number
}

// ══════════════════════════════════════════════════════════════════════════════
//  الثوابت
// ══════════════════════════════════════════════════════════════════════════════
const STORAGE_KEY = 'sad-playground-v2-projects'
const DEFAULT_FILE_CONTENT = `# ملف جديد
اطبع_سطر("مرحباً!")
`

// ══════════════════════════════════════════════════════════════════════════════
//  الحالة
// ══════════════════════════════════════════════════════════════════════════════
const editorEl = ref<HTMLElement | null>(null)
let editorView: any = null

// المشروع الحالي
const projectId = ref(generateId())
const projectName = ref('مشروع جديد')
const files = ref<ProjectFile[]>([
  { id: generateId(), name: 'رئيسي.ص', content: DEFAULT_FILE_CONTENT }
])
const activeFileId = ref(files.value[0].id)

// الخرج
const running = ref(false)
const outputLines = ref<string[]>([])
const errorText = ref('')
const execMs = ref<number | null>(null)
const stdinInput = ref('')
const hasError = computed(() => !!errorText.value)

// النوافذ
const showSavedProjects = ref(false)
const savedProjects = ref<Project[]>([])
const showShareModal = ref(false)
const shareUrl = ref('')
const copied = ref(false)

// ══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة
// ══════════════════════════════════════════════════════════════════════════════
function generateId(): string {
  return Date.now().toString(36) + Math.random().toString(36).substr(2, 9)
}

function formatDate(timestamp: number): string {
  return new Date(timestamp).toLocaleDateString('ar-SA', {
    year: 'numeric',
    month: 'short',
    day: 'numeric',
    hour: '2-digit',
    minute: '2-digit'
  })
}

function getActiveFile(): ProjectFile | undefined {
  return files.value.find(f => f.id === activeFileId.value)
}

// ══════════════════════════════════════════════════════════════════════════════
//  إدارة الملفات
// ══════════════════════════════════════════════════════════════════════════════
function addFile() {
  const num = files.value.length + 1
  const newFile: ProjectFile = {
    id: generateId(),
    name: `ملف${num}.ص`,
    content: DEFAULT_FILE_CONTENT
  }
  files.value.push(newFile)
  switchFile(newFile.id)
}

function closeFile(fileId: string) {
  if (files.value.length <= 1) return
  const idx = files.value.findIndex(f => f.id === fileId)
  if (idx === -1) return
  
  // حفظ المحتوى الحالي قبل الإغلاق
  saveCurrentFileContent()
  
  // إزالة الملف
  files.value.splice(idx, 1)
  
  // التبديل لملف آخر إذا كان الملف المغلق هو النشط
  if (activeFileId.value === fileId) {
    activeFileId.value = files.value[Math.min(idx, files.value.length - 1)].id
    loadFileInEditor()
  }
}

function switchFile(fileId: string) {
  if (fileId === activeFileId.value) return
  
  // حفظ محتوى الملف الحالي
  saveCurrentFileContent()
  
  // التبديل
  activeFileId.value = fileId
  loadFileInEditor()
}

function saveCurrentFileContent() {
  if (!editorView) return
  const file = getActiveFile()
  if (file) {
    file.content = editorView.state.doc.toString()
  }
}

function loadFileInEditor() {
  if (!editorView) return
  const file = getActiveFile()
  if (file) {
    editorView.dispatch({
      changes: { from: 0, to: editorView.state.doc.length, insert: file.content }
    })
  }
}

// ══════════════════════════════════════════════════════════════════════════════
//  إدارة المشاريع
// ══════════════════════════════════════════════════════════════════════════════
function newProject() {
  if (!confirm('هل تريد إنشاء مشروع جديد؟ سيتم فقدان التغييرات غير المحفوظة.')) return
  
  projectId.value = generateId()
  projectName.value = 'مشروع جديد'
  files.value = [{ id: generateId(), name: 'رئيسي.ص', content: DEFAULT_FILE_CONTENT }]
  activeFileId.value = files.value[0].id
  loadFileInEditor()
  clearOutput()
}

function saveProject() {
  saveCurrentFileContent()
  
  const project: Project = {
    id: projectId.value,
    name: projectName.value || 'بدون اسم',
    files: JSON.parse(JSON.stringify(files.value)),
    mainFileId: activeFileId.value,
    createdAt: Date.now(),
    updatedAt: Date.now()
  }
  
  // تحميل المشاريع المحفوظة
  const stored = localStorage.getItem(STORAGE_KEY)
  const projects: Project[] = stored ? JSON.parse(stored) : []
  
  // تحديث أو إضافة
  const existingIdx = projects.findIndex(p => p.id === project.id)
  if (existingIdx >= 0) {
    project.createdAt = projects[existingIdx].createdAt
    projects[existingIdx] = project
  } else {
    projects.unshift(project)
  }
  
  // حفظ
  localStorage.setItem(STORAGE_KEY, JSON.stringify(projects))
  alert('✅ تم حفظ المشروع!')
}

function openSavedProjects() {
  const stored = localStorage.getItem(STORAGE_KEY)
  savedProjects.value = stored ? JSON.parse(stored) : []
  showSavedProjects.value = true
}

function loadProject(project: Project) {
  projectId.value = project.id
  projectName.value = project.name
  files.value = JSON.parse(JSON.stringify(project.files))
  activeFileId.value = project.mainFileId || files.value[0]?.id || ''
  
  loadFileInEditor()
  showSavedProjects.value = false
  clearOutput()
}

function deleteProject(projId: string) {
  if (!confirm('هل أنت متأكد من حذف هذا المشروع؟')) return
  
  const stored = localStorage.getItem(STORAGE_KEY)
  let projects: Project[] = stored ? JSON.parse(stored) : []
  projects = projects.filter(p => p.id !== projId)
  localStorage.setItem(STORAGE_KEY, JSON.stringify(projects))
  
  savedProjects.value = projects
}

// ══════════════════════════════════════════════════════════════════════════════
//  المشاركة
// ══════════════════════════════════════════════════════════════════════════════
function shareProject() {
  saveCurrentFileContent()
  
  // تشفير المشروع كـ base64
  const projectData = {
    name: projectName.value,
    files: files.value.map(f => ({ name: f.name, content: f.content })),
    mainFile: activeFileId.value
  }
  
  const encoded = btoa(encodeURIComponent(JSON.stringify(projectData)))
  const baseUrl = window.location.origin + window.location.pathname
  shareUrl.value = `${baseUrl}?project=${encoded}`
  
  copied.value = false
  showShareModal.value = true
}

function copyShareUrl() {
  const input = document.querySelector('.pg2-share-url') as HTMLInputElement
  if (input) {
    input.select()
    document.execCommand('copy')
    copied.value = true
    setTimeout(() => { copied.value = false }, 2000)
  }
}

function loadFromUrl() {
  const params = new URLSearchParams(window.location.search)
  const encoded = params.get('project')
  if (!encoded) return
  
  try {
    const json = decodeURIComponent(atob(encoded))
    const data = JSON.parse(json)
    
    projectId.value = generateId()
    projectName.value = data.name || 'مشروع مستورد'
    files.value = data.files.map((f: any) => ({
      id: generateId(),
      name: f.name,
      content: f.content
    }))
    activeFileId.value = files.value[0]?.id || ''
    
    // إزالة البارامتر من URL
    window.history.replaceState({}, '', window.location.pathname)
  } catch (e) {
    console.error('فشل تحميل المشروع من URL:', e)
  }
}

// ══════════════════════════════════════════════════════════════════════════════
//  تشغيل الكود
// ══════════════════════════════════════════════════════════════════════════════
function clearOutput() {
  outputLines.value = []
  errorText.value = ''
  execMs.value = null
}

async function runCode() {
  if (running.value) return
  saveCurrentFileContent()
  
  // تجميع كل الملفات (الرئيسي أولاً)
  const mainFile = getActiveFile()
  if (!mainFile) return
  
  // لحالياً نشغل الملف النشط فقط
  // في المستقبل يمكن دعم import بين الملفات
  const source = mainFile.content
  const stdin = stdinInput.value
  
  running.value = true
  clearOutput()
  const t0 = performance.now()
  
  try {
    // محاولة استدعاء API
    const result = await callApi(source, stdin)
    execMs.value = Math.round(performance.now() - t0)
    
    if (!result.success && result.error) {
      errorText.value = result.error
    } else {
      outputLines.value = result.output.split('\n')
    }
  } catch (e: any) {
    errorText.value = `خطأ في التنفيذ: ${e?.message ?? e}`
    execMs.value = Math.round(performance.now() - t0)
  } finally {
    running.value = false
  }
}

async function callApi(source: string, stdin: string) {
  try {
    const resp = await fetch('http://localhost:3731/execute', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ source, stdin }),
      signal: AbortSignal.timeout(10000)
    })
    return await resp.json()
  } catch {
    return {
      success: false,
      output: '',
      error: '⚠️ لتشغيل الكود، ابدأ الخادم المحلي:\n\n  node website/api/server.js'
    }
  }
}

// ══════════════════════════════════════════════════════════════════════════════
//  تهيئة المحرر
// ══════════════════════════════════════════════════════════════════════════════
async function initEditor() {
  if (!editorEl.value) return
  
  try {
    const [
      { EditorState },
      { EditorView, keymap, lineNumbers, highlightActiveLineGutter,
        highlightSpecialChars, drawSelection, highlightActiveLine },
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
    
    const file = getActiveFile()
    
    editorView = new EditorView({
      state: EditorState.create({
        doc: file?.content || DEFAULT_FILE_CONTENT,
        extensions: [
          lineNumbers(),
          highlightActiveLineGutter(),
          highlightSpecialChars(),
          history(),
          drawSelection(),
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
            { key: 'Ctrl-s', run: () => { saveProject(); return true } },
          ]),
          oneDark,
          EditorView.theme({
            '&': { height: '100%', fontSize: '14px' },
            '.cm-content': { direction: 'ltr', textAlign: 'left' },
            '.cm-scroller': { overflow: 'auto', fontFamily: '"JetBrains Mono", monospace' },
          }),
        ],
      }),
      parent: editorEl.value!,
    })
  } catch (e) {
    console.error('فشل تحميل CodeMirror:', e)
  }
}

// ══════════════════════════════════════════════════════════════════════════════
//  دورة الحياة
// ══════════════════════════════════════════════════════════════════════════════
onMounted(async () => {
  await initEditor()
  loadFromUrl()
})

onUnmounted(() => {
  editorView?.destroy()
})
</script>

<style scoped>
/* ═══════════════════════════════════════════════════════════════════════════
   أنماط Playground v2
   ═══════════════════════════════════════════════════════════════════════════ */

.pg2-toolbar {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 8px 16px;
  background: var(--vp-c-bg-soft);
  border-bottom: 1px solid var(--vp-c-divider);
  gap: 16px;
}

.pg2-left, .pg2-right {
  display: flex;
  gap: 8px;
}

.pg2-btn {
  display: inline-flex;
  align-items: center;
  gap: 4px;
  padding: 6px 12px;
  border: 1px solid var(--vp-c-divider);
  border-radius: 6px;
  background: var(--vp-c-bg);
  color: var(--vp-c-text-1);
  font-size: 13px;
  cursor: pointer;
  transition: all 0.2s;
}

.pg2-btn:hover { background: var(--vp-c-bg-soft); }
.pg2-btn.primary { background: var(--vp-c-brand); color: #fff; border-color: var(--vp-c-brand); }
.pg2-btn.run { background: #22c55e; color: #fff; border-color: #22c55e; font-weight: 600; }
.pg2-btn:disabled { opacity: 0.6; cursor: not-allowed; }

.pg2-project-name {
  padding: 6px 12px;
  border: 1px solid var(--vp-c-divider);
  border-radius: 6px;
  background: var(--vp-c-bg);
  color: var(--vp-c-text-1);
  font-size: 14px;
  text-align: center;
  width: 200px;
}

/* شريط الملفات */
.pg2-tabs {
  display: flex;
  align-items: center;
  background: var(--vp-c-bg-soft);
  border-bottom: 1px solid var(--vp-c-divider);
  padding: 0 8px;
  overflow-x: auto;
}

.pg2-tab {
  display: flex;
  align-items: center;
  gap: 8px;
  padding: 8px 16px;
  border-bottom: 2px solid transparent;
  color: var(--vp-c-text-2);
  cursor: pointer;
  transition: all 0.2s;
  white-space: nowrap;
}

.pg2-tab:hover { color: var(--vp-c-text-1); }
.pg2-tab.active {
  color: var(--vp-c-brand);
  border-bottom-color: var(--vp-c-brand);
  background: var(--vp-c-bg);
}

.pg2-tab-close {
  width: 18px;
  height: 18px;
  border: none;
  background: none;
  color: var(--vp-c-text-3);
  cursor: pointer;
  border-radius: 4px;
  font-size: 14px;
  line-height: 1;
}

.pg2-tab-close:hover { background: var(--vp-c-bg-soft); color: #ef4444; }

.pg2-tab-add {
  padding: 4px 12px;
  border: none;
  background: none;
  color: var(--vp-c-text-3);
  font-size: 18px;
  cursor: pointer;
}

.pg2-tab-add:hover { color: var(--vp-c-brand); }

/* منطقة العمل */
.pg2-workspace {
  display: grid;
  grid-template-columns: 1fr 350px;
  height: 500px;
  background: var(--vp-c-bg);
}

.pg2-editor-pane {
  border-right: 1px solid var(--vp-c-divider);
  overflow: hidden;
}

.pg2-editor-mount {
  height: 100%;
}

.pg2-output-pane {
  display: flex;
  flex-direction: column;
  overflow: hidden;
}

.pg2-section {
  display: flex;
  flex-direction: column;
}

.pg2-section.stdin {
  border-bottom: 1px solid var(--vp-c-divider);
}

.pg2-section.output {
  flex: 1;
  overflow: hidden;
}

.pg2-section.output.error .pg2-section-header {
  color: #ef4444;
}

.pg2-section-header {
  padding: 8px 12px;
  font-size: 13px;
  font-weight: 600;
  color: var(--vp-c-text-2);
  background: var(--vp-c-bg-soft);
  display: flex;
  justify-content: space-between;
}

.pg2-time {
  font-weight: normal;
  color: var(--vp-c-text-3);
}

.pg2-stdin {
  padding: 8px 12px;
  border: none;
  background: var(--vp-c-bg);
  color: var(--vp-c-text-1);
  font-family: monospace;
  resize: none;
}

.pg2-output-body {
  flex: 1;
  padding: 12px;
  overflow: auto;
  font-family: monospace;
  font-size: 13px;
}

.pg2-output-text { color: var(--vp-c-text-1); margin: 0; white-space: pre-wrap; }
.pg2-error { color: #ef4444; margin: 0; white-space: pre-wrap; }
.pg2-placeholder { color: var(--vp-c-text-3); text-align: center; padding-top: 40px; }

/* النوافذ */
.pg2-modal-overlay {
  position: fixed;
  inset: 0;
  background: rgba(0, 0, 0, 0.5);
  display: flex;
  align-items: center;
  justify-content: center;
  z-index: 1000;
}

.pg2-modal {
  background: var(--vp-c-bg);
  border-radius: 12px;
  width: 90%;
  max-width: 500px;
  max-height: 80vh;
  overflow: hidden;
  box-shadow: 0 20px 50px rgba(0, 0, 0, 0.3);
}

.pg2-modal-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 16px 20px;
  border-bottom: 1px solid var(--vp-c-divider);
  font-weight: 600;
}

.pg2-modal-close {
  width: 28px;
  height: 28px;
  border: none;
  background: none;
  font-size: 20px;
  cursor: pointer;
  color: var(--vp-c-text-3);
}

.pg2-modal-body {
  padding: 20px;
  max-height: 60vh;
  overflow-y: auto;
}

.pg2-empty {
  text-align: center;
  color: var(--vp-c-text-3);
  padding: 40px;
}

.pg2-project-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 12px;
  border: 1px solid var(--vp-c-divider);
  border-radius: 8px;
  margin-bottom: 8px;
}

.pg2-project-info {
  display: flex;
  flex-direction: column;
  gap: 4px;
}

.pg2-project-item-name { font-weight: 600; }
.pg2-project-item-date { font-size: 12px; color: var(--vp-c-text-3); }
.pg2-project-item-files { font-size: 12px; color: var(--vp-c-text-3); }

.pg2-project-actions {
  display: flex;
  gap: 8px;
}

.pg2-project-actions button {
  padding: 6px 12px;
  border: 1px solid var(--vp-c-divider);
  border-radius: 6px;
  background: var(--vp-c-bg);
  cursor: pointer;
}

.pg2-project-actions button:hover { background: var(--vp-c-bg-soft); }
.pg2-project-actions button.danger { color: #ef4444; }

.pg2-share-url-box {
  display: flex;
  gap: 8px;
  margin: 16px 0;
}

.pg2-share-url {
  flex: 1;
  padding: 10px;
  border: 1px solid var(--vp-c-divider);
  border-radius: 6px;
  font-family: monospace;
  font-size: 12px;
}

.pg2-copy-btn {
  padding: 10px 16px;
  border: none;
  border-radius: 6px;
  background: var(--vp-c-brand);
  color: #fff;
  cursor: pointer;
  white-space: nowrap;
}

.pg2-share-note {
  font-size: 12px;
  color: var(--vp-c-text-3);
}

/* Responsive */
@media (max-width: 768px) {
  .pg2-workspace {
    grid-template-columns: 1fr;
    grid-template-rows: 300px 200px;
  }
  
  .pg2-editor-pane {
    border-right: none;
    border-bottom: 1px solid var(--vp-c-divider);
  }
  
  .pg2-toolbar {
    flex-wrap: wrap;
    gap: 8px;
  }
  
  .pg2-center { order: -1; width: 100%; }
  .pg2-project-name { width: 100%; }
}
</style>
