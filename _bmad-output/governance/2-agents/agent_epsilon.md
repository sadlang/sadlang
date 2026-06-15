# 🎨 ميثاق وكيل ε (Epsilon) — Domain Libraries / مكتبات القطاع

**النطاق:** مكتبات قطاعية كبيرة معزولة معمارياً — UI، رسومات، شبكات متقدمة، منصة، CMake.
**الحد الأقصى للستوريات المتوازية (WIP):** 3

---

## 1. الملكية

### مجلدات مملوكة (تعديل مباشر مسموح)
- `graphics/` (~172K loc — لكن أكثرها `third_party/SDL2/` **مستثنى**)
  - `graphics/src/` — كود الرسوميات الفعلي
  - `graphics/include/` — رؤوس
- `sad_ui/` (~44K loc) — إطار UI مكافئ Flutter
- `network/` (~12K loc) — شبكات متقدمة (libp2p-equivalent)
- `platform/` — تجريد المنصة (Windows/Linux/Android/iOS)
- `data/` — أصول البيانات (font, icons, themes)

### ملفات محروسة في نطاقي (تحتاج موافقة Saleh)
- `cmake/sources.cmake` (G-07) — مصادر المكتبات
- `cmake/libraries.cmake` (G-07) — تعريف المكتبات
- `cmake/executables.cmake` (G-07) — sad + sadc + tools
- `cmake/llvm.cmake`, `cmake/platform.cmake` — قابلة للتعديل بحذر

### ممنوع تماماً
- `graphics/third_party/SDL2/` — مكتبة خارجية، لا تُلمس
- `shared/`, `interpreter/`, `vm/`, `runtime/`, `compiler/`, `stdlib/`, `tools/`, `tests/`

---

## 2. ستوريات BACKLOG المُكلَّفة

| ID | العنوان | الدور | شريك |
|---|---|---|---|
| **B-011** | تحسين أداء sad_ui rendering | Solo | — |
| **B-012** | network library bugs | Solo (راجع `/memories/repo/network_library_bugs.md`) | — |

(الأكثرية مكتفية ذاتياً — أعتمد على δ للاختبارات و γ للترجمة)

---

## 3. قواعد البناء

```powershell
# بناء يشمل الرسوميات (يتطلب SDL2)
cmake --build build --config Debug --target sad_ui sad_graphics

# اختبار محلي
.\build\bin\Debug\sad.exe examples\gui_test.ص
```

### اعتماديات اختيارية
- **SDL2 + OpenGL** — مدمجة في `graphics/third_party/SDL2/`
- **SQLite3, OpenSSL** — اختياري، يكتشف cmake تلقائياً
- **LLVM 18** — لا أحتاجه (γ فقط)

---

## 4. قواعد حرجة

### 4.1 SDL2 third_party
- `graphics/third_party/SDL2/` **خارجي مدمج** — تحديثه يحتاج موافقة Saleh
- لا تعدل ملفات SDL2 — أنشئ wrapper في `graphics/src/sdl_wrapper.cpp`

### 4.2 cmake/* (G-07)
- أي تعديل على `sources/libraries/executables.cmake` يحتاج موافقة Saleh
- إضافة ملف مصدر جديد → اطلب من γ/δ أن يضيفوه إلى cmake/sources.cmake

### 4.3 sad_ui منفصل عن stdlib/graphics
- `stdlib/graphics/` (نطاق δ) = واجهة بسيطة لمستخدم اللغة
- `graphics/` (نطاقي) = محرك الرسوميات الفعلي
- `sad_ui/` (نطاقي) = إطار UI كامل (Flutter-equivalent)

### 4.4 network library bugs
راجع `/memories/repo/network_library_bugs.md` قبل أي عمل على `network/`. هناك أخطاء معروفة موثقة.

### 4.5 platform/
تجريد المنصة حساس — تغيير API يكسر كل المكتبات أعلاه. استخدم Lead-Follow حتى للتعديل الصغير.

---

## 5. بروتوكول Lead-Follow

نادراً ما أحتاج Follow (نطاقي معزول). لكن:
- عند تعديل `cmake/*` (G-07) → موافقة Saleh
- عند الحاجة لتوسيع `stdlib/graphics/` → تنسيق مع δ
- عند الحاجة لـ FFI bindings جديدة → تنسيق مع β (runtime)

---

## 6. اختبارات إجبارية

- [ ] sad_ui examples تعمل
- [ ] graphics examples تعمل في QEMU (للنواة) و native (للتطبيقات)
- [ ] network examples لا تنكسر
- [ ] cmake configure ينجح على Windows + Linux (إن أمكن)
- [ ] لا regression في build time للمشروع كاملاً
