---
title: Playground — جرّب لغة ص في المتصفح
description: اكتب وشغّل كود لغة ص مباشرة في المتصفح دون تثبيت
---

# Playground — جرّب لغة ص مباشرةً

<p class="lead">اكتب كود لغة ص وشغّله مباشرة في متصفحك — لا تثبيت مطلوب.</p>

<CodePlayground />

---

## كيف يعمل Playground؟

يستخدم Playground مفسّر لغة ص المُجمَّع إلى **WebAssembly (WASM)** باستخدام Emscripten، مما يتيح تشغيل الكود مباشرة في المتصفح بدون أي خادم.

```
كودك .ص  →  مفسر ص (WASM)  →  النتيجة
```

### الميزات

- ✅ محرر كود ذكي (CodeMirror 6) مع تلوين لغة ص
- ✅ تنفيذ فوري بدون انتظار
- ✅ أمثلة جاهزة متنوعة
- ✅ قياس وقت التنفيذ
- ✅ رسائل خطأ واضحة بالعربية

### القيود الحالية

::: warning القيود
- الوصول للملفات النظام **غير متاح** (WebAssembly sandbox)
- العمليات الشبكية **محدودة** في البيئة المتصفحية
- المكتبات الخارجية **غير مدعومة** بعد
:::

## تشغيل محلي (بدون قيود)

للوصول الكامل، ثبّت لغة ص محلياً:

::: code-group
```powershell [Windows]
irm https://sad-lang.org/install.ps1 | iex
sad برنامجي.ص
```

```bash [Linux/macOS]
curl -fsSL https://sad-lang.org/install.sh | bash
sad برنامجي.ص
```
:::

## بناء WASM يدوياً

إذا أردت بناء نسخة WASM خاصة بك:

```bash
# متطلبات: Emscripten SDK
emcmake cmake -S . -B build-wasm -DBUILD_WASM=ON
emmake cmake --build build-wasm --target sad_wasm
# الناتج: build-wasm/sad.wasm + sad.js
```

<style scoped>
.lead { font-size: 1.1rem; color: var(--vp-c-text-2); margin-bottom: 24px; }
</style>
