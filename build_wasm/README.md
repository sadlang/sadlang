# ملعب لغة ص — Web Playground

ملعب تفاعلي لتجربة لغة ص في المتصفح باستخدام WebAssembly.

## التشغيل المحلي

### الطريقة 1: Python HTTP Server
```powershell
cd build_wasm
python -m http.server 8080
# افتح http://localhost:8080
```

### الطريقة 2: Node.js
```powershell
cd build_wasm
npx http-server -p 8080
# افتح http://localhost:8080
```

### الطريقة 3: VS Code Live Server
1. ثبّت إضافة "Live Server"
2. انقر بالزر الأيمن على `index.html`
3. اختر "Open with Live Server"

## الملفات المطلوبة

```
build_wasm/
├── index.html    # واجهة الملعب
├── sad.js        # WASM loader (Emscripten)
└── sad.wasm      # المفسر مُترجم لـ WebAssembly
```

## البناء من المصدر

```powershell
# من المجلد الجذر للمشروع
.\build_wasm.ps1
```

أو يدوياً:
```powershell
mkdir build_wasm
cd build_wasm
emcmake cmake .. -DCMAKE_BUILD_TYPE=Release -DWASM_BUILD=ON
emmake make sad_wasm
```

## الميزات

- ✅ تحرير كود بلغة ص
- ✅ تنفيذ فوري في المتصفح
- ✅ أمثلة جاهزة (Hello World, دوال, أصناف, ...)
- ✅ عرض الأخطاء بوضوح
- ✅ قياس وقت التنفيذ
- ✅ اختصار Ctrl+Enter للتنفيذ

## API

الملعب يستخدم دالة `sad_execute` من WASM:

```javascript
const result = sadModule.ccall(
    'sad_execute',
    'number',
    ['string', 'string'],
    [sourceCode, stdinInput]
);
```

الإرجاع JSON:
```json
{
    "success": true,
    "output": "مرحباً بالعالم!",
    "error": ""
}
```

## النشر

لنشر الملعب على موقع:
1. انسخ `index.html`, `sad.js`, `sad.wasm` إلى خادم الويب
2. تأكد من إعداد MIME type لـ `.wasm`:
   ```
   application/wasm
   ```

## الترخيص

MIT License — مشروع لغة ص
