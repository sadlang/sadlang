# بناء لغة ص لـ WebAssembly — BUILD_WASM.md

## نظرة عامة

لغة ص تدعم تشغيل المفسر في المتصفح عبر WebAssembly (WASM).  
المخرج النهائي: `sad.js` + `sad.wasm` — وحدة JavaScript يمكن تضمينها في أي صفحة ويب.

---

## المتطلبات

| المتطلب | الإصدار | ملاحظات |
|---------|---------|---------|
| Emscripten SDK | 3.1+ | [تحميل](https://emscripten.org/docs/getting_started/downloads.html) |
| CMake | 3.15+ | اختياري — للبناء عبر CMake |
| C++17 | — | مدعوم تلقائياً عبر emcc |

### تثبيت Emscripten (Windows)

```powershell
cd C:\
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
.\emsdk.ps1 install latest
.\emsdk.ps1 activate latest
```

---

## طريقة البناء

### الطريقة 1: سكريبت PowerShell (الموصى بها)

```powershell
# من جذر المشروع
.\build_wasm.ps1              # بناء بتحسين O1 (افتراضي)
.\build_wasm.ps1 -O 2         # تحسين أعلى (إنتاج)
.\build_wasm.ps1 -O 0         # بدون تحسين (تصحيح)
```

**المخرجات:**
- `build_wasm/sad.js` — واجهة JavaScript المُحزَّمة
- `build_wasm/sad.wasm` — ملف WebAssembly
- يتم نسخ الملفات تلقائياً إلى `website/docs/public/`

### الطريقة 2: CMake + Emscripten

```powershell
# تفعيل بيئة Emscripten
C:\emsdk\emsdk_env.ps1

# تهيئة البناء
emcmake cmake -S . -B build_wasm -DBUILD_WASM=ON -DCMAKE_BUILD_TYPE=Release

# تنفيذ البناء
cmake --build build_wasm --target sad_wasm
```

---

## البنية الهندسية

```
┌──────────────────────────────────────────────────────────┐
│  كود لغة ص (.ص)                                        │
│  ↓                                                       │
│  sad_execute(source_utf8, stdin_utf8)  ← C API           │
│  ↓                                                       │
│  LexerCore → ParserCore → AST → Interpreter              │
│  ↓                                                       │
│  OutputCapture (cout/cerr → buffer)                      │
│  ↓                                                       │
│  JSON Response: {success, output, error}                 │
└──────────────────────────────────────────────────────────┘
          ↕ Emscripten cwrap/ccall
┌──────────────────────────────────────────────────────────┐
│  JavaScript  (sad_wasm_runtime.js)                       │
│  - createSadRuntime(memory)                              │
│  - DOM manipulation                                      │
│  - Fetch API, LocalStorage                               │
└──────────────────────────────────────────────────────────┘
```

### الملفات الرئيسية

| الملف | الدور |
|-------|------|
| `tools/wasm/sad_wasm.cpp` | نقطة دخول WASM — يُصدّر `sad_execute()` و `sad_version()` |
| `tools/wasm/wasm_builtins.cpp` | إعادة تعريف جميع الـ 38 builtin part آمنة لـ WASM |
| `tools/wasm/sad_wasm_runtime.js` | وقت تشغيل JavaScript — ذاكرة، DOM، Fetch |
| `cmake/wasm.cmake` | إعدادات CMake لـ Emscripten |
| `build_wasm.ps1` | سكريبت بناء PowerShell مباشر |
| `CMakeLists_wasm.txt` | ملف standalone legacy (مرجعي) — غير مستخدم في المسار الرئيسي |

### الدوال المُصدَّرة (C API)

```c
// تنفيذ كود ص وإرجاع JSON
char* sad_execute(const char* source_utf8, const char* stdin_utf8);
// → {"success": true/false, "output": "...", "error": "..."}

// إرجاع إصدار المفسر
const char* sad_version();
// → "1.0.0"
```

### الاستخدام من JavaScript

```javascript
const SadWasm = await import('./sad.js');
const sad = await SadWasm.default();

const execute = sad.cwrap('sad_execute', 'number', ['string', 'string']);
const ptr = execute('اطبع("مرحبا!")', '');
const result = JSON.parse(sad.UTF8ToString(ptr));
sad._free(ptr);

console.log(result.output);  // "مرحبا!"
```

---

## خيارات الترجمة

| الخيار | القيمة | الوصف |
|--------|--------|------|
| `-sWASM=1` | — | تفعيل WebAssembly |
| `-sMODULARIZE=1` | — | تصدير كوحدة ES |
| `-sEXPORT_NAME=SadWasm` | — | اسم الوحدة |
| `-sALLOW_MEMORY_GROWTH=1` | — | ذاكرة ديناميكية |
| `-sMAXIMUM_MEMORY=256MB` | — | حد أقصى |
| `-sFILESYSTEM=0` | — | بدون نظام ملفات (أصغر) |
| `-sDYNAMIC_EXECUTION=0` | — | بدون eval (أمان) |
| `-fexceptions` | — | دعم الاستثناءات C++ |

### تعريفات البناء (Defines)

| التعريف | الغرض |
|---------|-------|
| `SAD_WASM_BUILD=1` | تفعيل مسار WASM |
| `HAD_GRAPHICS=0` | تعطيل SDL2/رسوميات |
| `SAD_NO_FILESYSTEM=1` | تعطيل عمليات الملفات |
| `SAD_NO_NETWORK=1` | تعطيل الشبكة |
| `SAD_NO_SDL2=1` | تعطيل SDL2 |

---

## أحجام المخرجات النموذجية

| مستوى التحسين | sad.js | sad.wasm |
|---------------|--------|---------|
| `-O0` (تصحيح) | ~210 KB | ~7.7 MB |
| `-O1` (افتراضي) | ~153 KB | ~2.2 MB |
| `-O2` (إنتاج) | ~140 KB | ~1.8 MB |

---

## الاختبار في المتصفح

1. انسخ `sad.js` + `sad.wasm` إلى مجلد الخادم
2. أنشئ صفحة HTML:

```html
<!DOCTYPE html>
<html dir="rtl" lang="ar">
<head><meta charset="utf-8"><title>لغة ص — WASM</title></head>
<body>
  <textarea id="code" rows="10" cols="60">اطبع("مرحبا من المتصفح!")</textarea>
  <button onclick="run()">تشغيل</button>
  <pre id="output"></pre>

  <script type="module">
    import SadWasm from './sad.js';
    const sad = await SadWasm();
    window.run = () => {
      const code = document.getElementById('code').value;
      const exec = sad.cwrap('sad_execute', 'number', ['string', 'string']);
      const ptr = exec(code, '');
      const result = JSON.parse(sad.UTF8ToString(ptr));
      sad._free(ptr);
      document.getElementById('output').textContent =
        result.success ? result.output : '❌ ' + result.error;
    };
  </script>
</body>
</html>
```

3. شغّل خادم محلي: `npx serve .` أو `python -m http.server`
4. افتح `http://localhost:3000` في المتصفح

---

## ملاحظات ومحاذير

- **wasm_builtins.cpp** يعيد تعريف جميع الـ 38 builtin part بشكل آمن لـ WASM (بدون SDL2/OS/شبكة)
- الدوال غير المتوفرة في WASM (رسوميات، ملفات، شبكة) تُرجع رسائل خطأ واضحة
- بناء PowerShell (`build_wasm.ps1`) يشمل مصادر أكثر اكتمالاً من CMake (`cmake/wasm.cmake`)
- `CMakeLists_wasm.txt` مُبقى كمرجع تاريخي/standalone فقط، بينما المسار الرسمي الحالي هو:
  - `CMakeLists.txt` + `option(BUILD_WASM)` + `cmake/wasm.cmake`
- حجم WASM النهائي يعتمد على مستوى التحسين — استخدم `-O2` للإنتاج
- الموقع الإلكتروني يستخدم VitePress ←- شغّل `cd website; npx vitepress build .` بعد نسخ الملفات

---

## حالة البناء الحالية

| العنصر | الحالة |
|--------|--------|
| `build_wasm/sad.wasm` | ✅ موجود (2.2 MB, O1) |
| `build_wasm/sad.js` | ✅ موجود (153 KB) |
| `website/docs/public/sad.wasm` | ✅ موجود (7.7 MB, O0) |
| `website/docs/public/sad.js` | ✅ موجود (208 KB) |
| JS Runtime | ✅ مكتمل (`sad_wasm_runtime.js`) |
| WASM builtins | ✅ 38 part مُعاد تعريفها |

> **آخر بناء:** مارس 2026
