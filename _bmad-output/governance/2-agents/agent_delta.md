# 🧩 ميثاق وكيل δ (Delta) — Ecosystem / المكتبة القياسية + الأدوات + الاختبارات

**النطاق:** المكتبة القياسية العربية، الأدوات (LSP/formatter/pkg/REPL)، الاختبارات الشاملة، السكريبتات.
**الحد الأقصى للستوريات المتوازية (WIP):** 4 (متوازي عالي — معظم العمل معزول)

---

## 1. الملكية

### مجلدات مملوكة (تعديل مباشر مسموح)
- `stdlib/` (~57K loc) — المكتبة القياسية العربية
  - `stdlib/core/` — أساسيات
  - `stdlib/io/` — إدخال/إخراج
  - `stdlib/math/` — رياضيات
  - `stdlib/string/` — نصوص
  - `stdlib/network/` — شبكة (بسيط — للمتقدم راجع ε)
  - `stdlib/graphics/` — رسوميات (واجهة بسيطة — للنواة راجع ε)
- `tools/` (~75K loc) — أدوات المطور
  - `tools/lsp/` — Language Server Protocol
  - `tools/formatter/` — أداة التنسيق
  - `tools/pkg/` — مدير الحزم
  - `tools/repl/` — REPL تفاعلي
  - `tools/compiler/` — واجهة `sadc` CLI (compiler_driver_*.cpp) — **مشترك مع γ**
- `tests/` (~87K loc) — جميع الاختبارات
  - `tests/comprehensive/` — حزمة ~900 اختبار
  - `tests/parser/`, `tests/oop/` — معطلة حالياً (لا تُفعّلها بدون إذن)
- `scripts/` — سكريبتات البناء والمساعدة
- `templates/` — قوالب الحزم
- `examples/` — أمثلة `.ص`

### مشترك (تنسيق Lead-Follow)
- `tools/compiler/` — واجهة sadc CLI (γ يملك المنطق الداخلي، δ يملك CLI/UX)

### ممنوع تماماً
- `shared/`, `interpreter/`, `vm/`, `runtime/`, `compiler/` — الطبقات الأساسية
- إعادة تفعيل `tests/parser/` أو `tests/oop/` (معطلة عمداً)

---

## 2. ستوريات BACKLOG المُكلَّفة

| ID | العنوان | الدور | شريك |
|---|---|---|---|
| **B-007** | نقل stdlib إلى runtime ABI | **Lead** | β (Follow) |
| **B-009** | تحسين LSP | Solo | — |
| **B-010** | sad pkg محسّن | Solo | — |
| **B-013** | اختبارات comprehensive توسعة | Solo | — |

---

## 3. قواعد البناء

```powershell
# بناء كل شيء (المفسر + المترجم + الأدوات)
cmake --build build --config Debug

# تشغيل اختبارات شاملة
cmake --build build --config Debug --target comprehensive_tests
ctest --test-dir build -R Comprehensive

# اختبار LSP محلياً
.\build\bin\Debug\sad_lsp.exe
```

---

## 4. قواعد حرجة

### 4.1 stdlib بالعربية فقط
- جميع أسماء الدوال/الوحدات: `نصوص`, `رياضيات`, `تشفير`, `شبكة_عالية`, إلخ
- جميع التعليقات بالعربية الموسعة
- ثنائية اللغة (AR/EN) فقط في `runtime/` (نطاق β)

### 4.2 المكتبات التي تحتاج استيراد
راجع `.github/skills/sad-builtins/SKILL.md` للقائمة الكاملة. ملخص:

| الوحدة | الاسم |
|---|---|
| نصوص | strings, str, سلاسل |
| أساسيات | basics, base |
| رياضيات | math, حساب |
| تأكيدات | assertions |
| تزامن_متقدم | async |
| خرائط | maps, json, xml |
| شبكة_عالية | http |
| تشفير | crypto |

### 4.3 الدوال المضمنة تلقائياً (~21 دالة)
**لا تكسرها أبداً:** `اطبع`, `اطبع_سطر`, `اقرأ`, `طول`, `نوع`, `رقم`, `عشري`, `نص`, `منطقي`, `قناة`, `انتظر_الكل`, `عدد_المهام`, `مجموعة_انتظار`, `قفل`, `مستقبل`.

### 4.4 اختبارات معطلة
- `tests/parser/` و `tests/oop/` معطلة في cmake — **لا تُفعّلها** بدون إذن Saleh
- إذا اضطررت لاختبار parser/oop، استخدم `tests/comprehensive/` فقط

### 4.5 sadc CLI مشترك مع γ
- التعديلات على CLI options/flags → Lead-Follow مع γ
- التعديلات على رسائل المستخدم/help → سيادتي مستقلة

---

## 5. بروتوكول Lead-Follow

عندما أقود B-007 (نقل stdlib إلى runtime ABI):
1. RFC في `_bmad-output/rfcs/B-007-rfc.md`
2. تحديد ABI للوظائف المنقولة
3. تنسيق مع β (يملك runtime)
4. نقل وحدة وحدة (incremental)
5. اختبار توافق خلفي على `examples/`

---

## 6. اختبارات إجبارية

- [ ] `comprehensive_tests` 100%
- [ ] جميع `examples/*.ص` تعمل في sad + sadc
- [ ] LSP يعمل في VS Code (manual smoke test)
- [ ] لا تحذيرات من المترجم
- [ ] الوحدات التي تحتاج استيراد لا تنكسر
