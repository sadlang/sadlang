# ADR-EM-3: YAML كمصدر حقيقة وحيد لرسائل الأخطاء

> ⚠️ **Superseded by [ADR-DOCS-V4-005](../../living-documentation/decisions/ADR-DOCS-V4-005-UNIFIED-SOT-MIGRATION-COMPLETION.md) (2026-06-10).**
> هذا الـADR اعتمد **ملف V4 واحد** `data/language/error_messages.yaml` كـ SoT. هاجر V5 إلى
> `language-truth/errors/*.yaml` (مقسّم بالفئة)، وحُذف ملف V4 في `EM-V5-3`. يُحفظ للرجوع التاريخي
> (GR-02) — لا يُعتمَد كمصدر للحالة الراهنة.

**الحالة:** Superseded (كان: مُنفَّذ M1→M7) — استُبدلت معماريّة الملف الواحد بـ V5 الموحَّد
**التاريخ:** 2026-05-19
**المؤلف:** Amelia (bmad-agent-dev)
**المرجع السابق:** [ADR-006a (توحيد codegen)](../ADR-006a_توحيد_codegen.md) — يستهلك `_lib/{loader,emit}.py`
**الخطة الأصلية:** [error_messages_yaml_migration.md](../../error_system/error_messages_yaml_migration.md)
**Epic:** [EPIC-EM (Error Messages)](../../error_system/epic-error-messages.md)
**الموقع في خارطة المشروع:** EM-3 (المرحلة الثالثة من خارطة طريق نظام الأخطاء)

> **ملخص تنفيذي:** هذا الـADR يُغلق دورة هجرة رسائل الأخطاء من C++ مكتوبة يدوياً إلى **معماريّة طبقتين فقط:** YAML واحد (`data/language/error_messages.yaml` بـ231 إدخالاً) + مولِّد Python واحد (`gen_error_messages.py`). جميع ملفات البيانات اليدوية الـ17 حُذفت نهائياً. النتيجة: SoT خالص، صفر تكرار، صفر overrides.

---

## 1. السياق (Context)

### 1.1. الوضع قبل EM-3

بعد EM-1 (إنشاء yaml) وEM-2 (إثراء الإدخالات إلى 231)، كان النظام يحوي **طبقات متعدّدة متناقضة:**

| المصدر | عدد الأسطر | الدور | المشكلة |
|---|---|---|---|
| `data/language/error_messages.yaml` | 231 entry | المصدر الأساسي | ✅ صحيح |
| `data/language/error_messages_overrides.yaml` | 75 override | تجاوزات يدوية لجودة 23+52 | ❌ ينتهك SoT |
| `shared/errors/src/error_codes.cpp` | 843 | enum→string + map رسائل | ❌ مكتوب يدوياً |
| `shared/errors/src/error_catalog_init.cpp` | 213 | `registerDefaults()` يدوي | ❌ يدوي + ناقص |
| `shared/errors/src/catalog/runtime_*.cpp` | 9 ملفات | جداول runtime منفصلة | ❌ تكرار |
| `scripts/codegen/apply_error_message_overrides.py` | — | يدمج overrides في yaml المتولِّد | ❌ تعقيد بدون فائدة |

### 1.2. المشاكل المعمارية المُبرِّرة

| المشكلة | القاعدة المنتهكة | التأثير |
|---|---|---|
| 11 ملف C++ يكرّر بيانات الـyaml | CW-19 (DRY) | تعديل رسالة واحدة = 3 أماكن |
| طبقة overrides تتجاوز yaml | **SoT انتهاك صريح** | yaml لا يعكس المنتج النهائي |
| 4 دوال (`getErrorCodeString`, `getErrorDescription`, `getErrorCategory`, `registerDefaults`) موزّعة على 11 ملف | CW-21 (واجهات واضحة) | لا يمكن إضافة دالة جديدة في مكان واحد |
| 156 رسالة internal بلا template غني، 75 فقط مُكتمَلة | CW-04 (التناسق البنيوي) | تجربة مطوّر متفاوتة |

### 1.3. توجيه المستخدم الصريح

> «احذف كل الملفات التي لا تستخدم yaml لتوليد الأخطاء وابدأ من البداية لكل الأخطاء. يمكنك تقسيم العمل إلى عدة مراحل، استخدم مهارة bmad-agent-dev».

تحوّل القرار من «3 طبقات (yaml + overrides + C++)» إلى **«طبقتان فقط (yaml + مولِّد)»**.

---

## 2. القرار (Decision)

### 2.1. البنية الجديدة (طبقتان حصراً)

```
┌──────────────────────────────────────────┐
│ 1. data/language/error_messages.yaml     │ ← SoT الوحيد، تحرير يدوي فقط
│    (231 entry: lex 6 + syn 8 + sem 9     │
│     + run 52 + int 156)                  │
└────────────────┬─────────────────────────┘
                 │ يقرأ
                 ▼
┌──────────────────────────────────────────┐
│ 2. scripts/codegen/gen_error_messages.py │ ← مولِّد وحيد (480 سطر)
│    + scripts/codegen/_lib/{loader,emit}  │   (يستهلك ADR-006a)
└────────────────┬─────────────────────────┘
                 │ يُنتج
                 ▼
┌──────────────────────────────────────────┐
│ shared/errors/generated/                 │ ← لا تحرَّر يدوياً
│   error_messages_generated.h             │   (.h.baseline + .cpp.baseline
│   error_messages_generated.cpp           │    ملتزمان للبناء بدون Python)
└──────────────────────────────────────────┘
```

### 2.2. الملف المولَّد يحوي 4 دوال مُوحَّدة

| الدالة | المُعرَّفة في header | المهمة |
|---|---|---|
| `Sad::Errors::getErrorCodeString(ErrorCode)` | `error_codes.h` | ErrorCode → "SEM002" |
| `Sad::Errors::getErrorDescription(ErrorCode, Language)` | `error_codes.h` | ErrorCode + AR/EN/BOTH → نص الـbrief |
| `Sad::Errors::getErrorCategory(ErrorCode)` | `error_codes.h` | ErrorCode → "lexical"/"syntactic"/... |
| `Sad::Errors::ErrorCatalog::registerDefaults()` | `error_catalog.h` | تسجيل 231 ErrorTemplate في الـSingleton |

### 2.3. مخطّط YAML (Schema)

كل entry تحوي:
- `code` (string، ENUM canonical name مثل `SEM_UNDEFINED_VARIABLE`)
- `id` (string، code قصير مثل `SEM001`)
- `category` (`lexical|syntactic|semantic|runtime|internal`)
- `title.{ar,en}`, `brief.{ar,en}`, `fix_hint.{ar,en}`, `detailed.{ar,en}`
- `placeholders[]` (قائمة `{name}` الموجودة في النصوص)

---

## 3. البدائل المرفوضة (Rejected Alternatives)

| البديل | سبب الرفض |
|---|---|
| **3 طبقات (yaml + overrides + C++)** | ينتهك SoT — yaml لا يعكس النص النهائي |
| **توليد yaml من C++ بسكريبت** | يكرّس الـC++ كمصدر، يحوِّل yaml لمشتقّ |
| **Mustache/Jinja templates لكل entry** | تعقيد بلا قيمة لـ4 دوال فقط |
| **JSON بدلاً من YAML** | عدم اتساق مع `keywords.yaml` و`type_methods.yaml` (ADR-006a) |
| **بقاء overrides لـ«حالات خاصة»** | كل تجاوز قابل للتعبير في yaml نفسه — لا حاجة لطبقة منفصلة |

---

## 4. النتائج (Consequences)

### 4.1. الإيجابية ✅

- **17 ملف محذوف** (انظر §6 خارطة الترحيل) → تخفيض 1056 سطر يدوي.
- **SoT خالص:** تحرير `error_messages.yaml` = الحقيقة الوحيدة.
- **صفر overrides:** `data/language/error_messages_overrides.yaml` لم يعد موجوداً.
- **بناء قابل للتكرار:** baseline files (`.h.baseline`, `.cpp.baseline`) مُلتزَمة → CI بدون Python.
- **التغطية: 100%** من 231 ErrorCode لها template غني (سابقاً 26/203 = 13%).
- **اختبار ذاتي:** `pytest scripts/codegen/test_gen_error_messages.py` → 13/13 ✅.

### 4.2. السلبية / الالتزامات ⚠️

- إضافة دالة API جديدة لـerror catalog → تعديل `SOURCE_EPILOGUE` في `gen_error_messages.py` (ليس في C++ مباشرة).
- مراجعة جودة المحتوى تحتاج دفعات يدوية (M6).
- baseline files تُحدَّث في كل تعديل yaml → التزام في git.

### 4.3. التحقق من عدم التراجع (Regression)

| الاختبار | النتيجة قبل EM-3 | النتيجة بعد EM-3 | الحكم |
|---|---|---|---|
| `pytest test_gen_error_messages.py` | 13/13 | 13/13 | ✅ لا تراجع |
| `test_errors_comprehensive.exe` | passed | passed | ✅ |
| `test_interpreter_comprehensive.exe` | 74/76 | 74/76 | ✅ الفشل قديم |
| `test_regression_comprehensive.exe` | 32/35 | 32/35 | ✅ الفشل قديم (concat/+=) |
| بناء `sad.exe` Debug | clean | clean | ✅ |
| بناء `sad_shared.lib` Debug | clean | clean | ✅ |

---

## 5. مراحل التنفيذ (Execution Stages)

| المرحلة | المخرَج | الحالة |
|---|---|---|
| **M1** | `error_messages.yaml` بـ231 entry مُثرى | ✅ مكتمل |
| **M2** | `gen_error_messages.py` يُنتج 4 دوال مع pytest 13/13 | ✅ |
| **M3** | حذف 17 ملف legacy + ربط 3 ملفات CMake | ✅ |
| **M4** | بناء `sad_shared` + `sad` نظيف Debug | ✅ |
| **M5** | تحقّق من عدم تراجع `comprehensive_tests` | ✅ |
| **M6** | مراجعة جودة (الدفعة 1: 23 رسالة موجَّهة للمستخدم) — تحسين SYN002 | 🟡 الدفعة 1 مكتملة، 2+3 معلَّقة |
| **M7** | توثيق رسمي = هذا الـADR | ✅ |

---

## 6. خارطة الترحيل (Migration Map)

### 6.1. ملفات محذوفة (17)

| الملف | السبب |
|---|---|
| `shared/errors/src/error_codes.cpp` | استُبدل بدوال مولَّدة |
| `shared/errors/src/error_catalog_init.cpp` | `registerDefaults()` مولَّد |
| `shared/errors/src/catalog/runtime_arithmetic_catalog.cpp` | بياناته في yaml |
| `shared/errors/src/catalog/runtime_builtins_catalog.cpp` | بياناته في yaml |
| `shared/errors/src/catalog/runtime_collections_catalog.cpp` | بياناته في yaml |
| `shared/errors/src/catalog/runtime_concurrency_catalog.cpp` | بياناته في yaml |
| `shared/errors/src/catalog/runtime_contracts_catalog.cpp` | بياناته في yaml |
| `shared/errors/src/catalog/runtime_functions_catalog.cpp` | بياناته في yaml |
| `shared/errors/src/catalog/runtime_misc_catalog.cpp` | بياناته في yaml |
| `shared/errors/src/catalog/runtime_oop_catalog.cpp` | بياناته في yaml |
| `shared/errors/src/catalog/runtime_types_catalog.cpp` | بياناته في yaml |
| `shared/errors/src/catalog/runtime_template_helpers.h` | لا حاجة بعد المولِّد |
| `data/language/error_messages_overrides.yaml` | SoT خالص لا overrides |
| `scripts/codegen/apply_error_message_overrides.py` | لا overrides لتطبيقها |
| `_scratch/_gen_runtime_overrides.py` | scratch قديم |
| `_scratch/_dump_batch1.py` | scratch قديم |
| `_scratch/_dump_batch2.py` | scratch قديم |

### 6.2. ملفات مُحدَّثة (3 CMake)

| الملف | التعديل |
|---|---|
| `shared/CMakeLists.txt` | استبدلت 11 ملف بـ`errors/generated/error_messages_generated.cpp` + أضيف `errors/generated/` لـPUBLIC includes |
| `cmake/libraries.cmake` | أُضيف `add_dependencies(sad_shared sad_error_messages_codegen)` |
| `platform/android/CMakeLists.txt` line 127 | `error_codes.cpp` → `errors/generated/error_messages_generated.cpp` |

### 6.3. ملفات محتفظ بها في `shared/errors/src/`

`diagnostic.cpp`, `error_catalog.cpp` (Singleton + render logic فقط — البيانات في المولَّد), `error_hints.cpp`, `error_manager.cpp`, `explanation_level.cpp`, `formatter.cpp`, `smart_*.cpp`, `builders/dispatch.cpp`.

---

## 7. مرجع للمطوّرين

### 7.1. كيفية إضافة رسالة خطأ جديدة

1. أضف enum value في `shared/errors/include/error_codes.h`.
2. أضف entry جديدة في `data/language/error_messages.yaml` بنفس code و id.
3. شغّل: `cmake --build build --config Debug --target sad_error_messages_codegen`.
4. حدِّث baseline: انسخ `.h` و`.cpp` المولَّدين إلى `.h.baseline` و`.cpp.baseline`.
5. أعد بناء `sad_shared`.

### 7.2. كيفية إضافة دالة API جديدة لـerror catalog

عدّل `SOURCE_EPILOGUE` في `scripts/codegen/gen_error_messages.py` فقط. لا تكتب C++ يدوياً في `shared/errors/src/`.

### 7.3. اختبار التوليد

```powershell
$env:PYTHONIOENCODING="utf-8"
cd scripts/codegen
python -m pytest test_gen_error_messages.py -q
```

ينبغي أن يُنتج: `13 passed`.

---

**نهاية ADR-EM-3.**
