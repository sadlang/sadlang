# خطّة استخراج `sad_runtime` وإغلاق تسرّب ترويسات المفسّر

> **الغرض:** توثيق الخريطة الدقيقة والخطوات متعدّدة الـPR لاستخراج طبقة `sad_runtime`
> (خدمات وقت التشغيل المشتركة) من `interpreter/`، وإغلاق تسرّب تصدير `interpreter/include`
> من `sad_shared` (المرحلة 3 من [RFC sadlang-rfcs#10](https://github.com/sadlang/sadlang-rfcs/blob/main/text/0010-unified-core-with-internal-boundaries.md)).
>
> **عدسة هذه الوثيقة:** خريطة *تنفيذيّة* مستخرَجة من الكود (`ملف:سطر`)، مكمّلة لـ
> [cmake-target-boundaries.md](cmake-target-boundaries.md). هذه الشريحة **متعدّدة الخطوات** لأنّها
> تمسّ حدًّا حسّاسًا سلوكيًّا (المحلّل النحويّ مشترك بين المفسّر والمترجم).

---

## 1. الخريطة الدقيقة (لقطة *ما-قبل* التنفيذ، مستخرَجة من الكود 2026-06-27)

> ⚠️ **هذا القسم لقطةٌ تاريخيّةٌ للحالة قبل خطوات 2–4** (تشرح اللغز والعائق). بعد التنفيذ:
> ترويسة+`.cpp` لـ`ClassManager` صارتا في `shared/types/`، وأُغلق تصدير `interpreter/include`
> من `sad_shared`. القسم 2 يعكس الحالة النهائيّة (كلّ الخطوات ✅).

### أين يُجمَّع كلّ مدير؟ (مفاجأة تحلّ اللغز)

| المدير | الترويسة | الـ`.cpp` يُجمَّع في | الطبقة المنطقيّة |
|---|---|---|---|
| **`ClassManager`** | `interpreter/include/managers/class_manager.h` | **`sad_shared`** ([shared/CMakeLists.txt:49](../../shared/CMakeLists.txt#L49)) | **أساس** — المحلّل يحتاجه |
| `FunctionManager` | `interpreter/include/managers/function_manager.h` | `sad_core` ([sources.cmake:143](../../cmake/sources.cmake#L143)) | **مرشّح `sad_runtime`** |
| `ObjectManager` | `.../object_manager.h` | `sad_core` ([sources.cmake:158](../../cmake/sources.cmake#L158)) | **مرشّح `sad_runtime`** |
| `OwnershipManager` | `.../ownership_manager.h` | `sad_core` ([sources.cmake:145](../../cmake/sources.cmake#L145)) | **مرشّح `sad_runtime`** |
| `ScopeManager` | `.../scope_manager.h` | `sad_core` ([sources.cmake:144](../../cmake/sources.cmake#L144)) | يبقى في `sad_interp` (شجريّ) |
| `VariableManager` | `.../variable_manager.h` | `sad_core` ([sources.cmake:142](../../cmake/sources.cmake#L142)) | يبقى في `sad_interp` (شجريّ) |

**الاكتشاف الجوهريّ:** `ClassManager` **ليس مديرًا خاصًّا بالمفسّر** — `.cpp`ه مُجمَّع داخل `sad_shared`،
لأنّ **المحلّل النحويّ** (في `sad_shared`) يستعمله. موقع ترويسته تحت `interpreter/include/managers/`
هو **سبب التسرّب**، لا حاجةٌ بنيويّة. (يفسّر هذا كيف يحلّ المترجم `sad-build` — الذي يربط `sad_shared`
لا `sad_core` — نداءَ `ClassManager::getInstance()` من المحلّل: التعريف في `sad_shared`.)

### التسرّب

[shared/CMakeLists.txt:116,130-131](../../shared/CMakeLists.txt#L116) يُصدّر `interpreter/include`
و`interpreter/include/managers` كـ`PUBLIC` ⇒ كلّ هدف يربط `sad_shared` (ومنه نظام المترجم) يرى ترويسات المفسّر.
حارس [check_interpreter_boundary.py](../../scripts/codegen/check_interpreter_boundary.py) (PR #100) يضمن **عدم استغلال** المترجم لهذا الباب.

### اقتران المحلّل ↔ `ClassManager` (العائق)

المحلّل يستعمل `Data::ClassManager::getInstance()` في **موضعين حقيقيّين فقط** وقت التحليل:
- [parser_helpers.cpp:2172](../../shared/parser/src/core/parser_helpers.cpp#L2172) — `isClassName()` ⟵ `hasClass(name)`.
- [parser_declarations.cpp:1192](../../shared/parser/src/declarations/parser_declarations.cpp#L1192) — تسجيل صنف مبكر (`hasClass` + `registerClass`) ليُسمح بمتغيّرات من نوعه لاحقًا في نفس الملفّ.

وكان يضمّ `class_manager.h` في **6 ملفّات**، **4 منها ميتة** (بلا استعمال أيّ رمز) — **أُزيلت في هذا الـPR** ⇒ بقي ملفّان حقيقيّان.

> **خلاصة العائق:** بما أنّ `ClassManager.cpp` **أصلًا في `sad_shared`**، فاستعمال المحلّل له **ليس دورة** —
> ما دام التعريف في الطبقة نفسها. العائق الحقيقيّ هو **موقع الترويسة** تحت `interpreter/`، لا الاقتران ذاته.

### `sad_builtins` لا يحتاج المدراء

`sad_builtins` (14 ملفًّا، PR #97) **نقيّ: لا `FunctionManager`** ([libraries.cmake:34](../../cmake/libraries.cmake#L34)).
الملفّ الوحيد الذي يلمس `FunctionManager` هو لِحام التسجيل [stdlib_manager.cpp](../../shared/builtins/src/runtime/stdlib_manager.cpp) — يبقى في `sad_core`.

---

## 2. الخطوات متعدّدة الـPR

| # | الخطوة | الأثر | المخاطرة | الحالة |
|:--:|---|---|:--:|:--:|
| **1** | **تقليم تضمينات المحلّل الميتة** (PR #108) — إزالة `#include "class_manager.h"` من 4 ملفّات لا تستعمله ⇒ سطح الاقتران 6→2 ملفّات. | تنظيف آمن، يضيّق التشابك الظاهر. | منخفضة | ✅ مدموجة |
| **2** | **نقل ترويسة `ClassManager` إلى شجرة `sad_shared`** — نُقلت الترويسة و`.cpp`ها إلى `shared/types/` (بجوار `class_type.h` الذي تضمّه؛ المسار عامّ + PUBLIC أصلًا فلا مسار تضمين جديد). المحلّل يبقى يستعملها مباشرةً (لا دورة، الطبقة نفسها). | يجعل `ClassManager` مواطنًا كامل العضويّة في `sad_shared`، ويزيل **سبب** تصدير `interpreter/include/managers`. | متوسطة | ✅ منجزة |
| **3** | **إغلاق التسرّب** — أُزيل تصدير `interpreter/include` + `interpreter/include/managers` من `sad_shared` العامّ. بعد خطوة 2 لم يَعُد أيُّ مصدرٍ في `sad_shared` يحتاج ترويسات المفسّر (المحلّل ↔ `class_manager.h` فقط، وهي الآن في `types/`). `hot_reload` يُجمَّع داخل `sad_interp` لا كمستهلكٍ عامّ. الحارس يثبّت السلامة. | يجعل رسم التبعيّات أمينًا: النواة لا تُصدّر ترويسات المفسّر لمن يربطها. (الكتلة العامّة `include_directories` تبقى — سطح أوسع يحرسه `check_interpreter_boundary.py`.) | متوسطة | ✅ منجزة |
| **4** | **استخراج `sad_runtime`** — مكتبة ساكنة جديدة = `function_manager.cpp` + `object_manager.cpp` + `ownership_manager.cpp`؛ نُقلت مصادرها من `sad_core` إليها؛ `sad_interp` يربط `sad_runtime` PUBLIC (اتّجاه أحاديّ). `Scope`/`Variable` يبقيان في `sad_interp` (شجريّان). **تحقّق الفصل:** `function_manager` يستعمل `BuiltinContext` كنوعٍ في توقيعات `std::function` فقط (لا دورة رموز)، و`object/ownership` بلا اعتماد على دواخل المفسّر ⇒ `sad-run.exe` رُبط نظيفًا بلا `LNK`. | طبقة خدمات وقت التشغيل المشتركة — موضع شقيق الآلة الافتراضية عند عودتها (§ط-10). صُنّف في نظام المفسّر بحارس الطبقات (يُرقَّى إلى الحزام عند ظهور مستهلكٍ ثانٍ). | عالية | ✅ منجزة |

> **مبدأ:** خطوات 2–4 سلوكيّة الحساسيّة ⇒ تحقّق صارم قبل الدمج. **نُفّذت دفعةً واحدةً** (قرار المستخدم) وخضعت لبناء `BUILD_TESTS=ON` (Debug+Release) والبوّابة المزدوجة (95.4% (1988/2084) ≥ 86%، CONCERNS بلا تراجع حرج).

### خطوة 5 (إضافيّة، بطلب المستخدم «عالج الديون المؤجَّلة») — إغلاق الكتلة العامّة

| # | الخطوة | الأثر | الحالة |
|:--:|---|---|:--:|
| **5** | **نقل `interpreter/include*` من الكتلة العامّة** — أُزيلت مسارات المفسّر الثمانية من `include_directories` الجذر، ونُقلت إلى **`sad_interp PUBLIC`** (يرثها كلُّ من يربط `sad_interp`: المفسّر/الأدوات/الاختبارات/`sad_ui_bridge` عبر `$<TARGET_PROPERTY>`) و**`sad_runtime PRIVATE`** (لا يربط `sad_interp`). ونُقل `user_thrown.h` (نوع استثناء مشترك حبسه `interpreter/include`، يحتاجه `sad_builtins`) إلى `shared/errors/include`. | يُغلق **الباب الثاني** للتسريب: نظام المترجم (يربط `sad_shared` فقط) لم يَعُد يرى ترويسات المفسّر لا بالتصدير ولا بالكتلة العامّة ⇒ الحدّ نظيفٌ **بنيويًّا** لا بالممارسة. | ✅ منجزة |

> **ملاحظة:** كانت هذه الخطوة مؤجَّلةً في النسخة الأولى من هذه الوثيقة («سطحٌ أوسع، PR مكرَّس»)؛ نُفِّذت في نفس الدفعة بطلب صريح. `check_interpreter_boundary.py` يبقى وقائيًّا يمنع الانحدار.

---

## 3. المراجع

- خريطة الحدود: [cmake-target-boundaries.md](cmake-target-boundaries.md)
- حارس التضمين: [check_interpreter_boundary.py](../../scripts/codegen/check_interpreter_boundary.py)
- حارس الطبقات (G4): [check_layering.py](../../scripts/codegen/check_layering.py)
- RFC الحاكم: [sadlang-rfcs#10](https://github.com/sadlang/sadlang-rfcs/blob/main/text/0010-unified-core-with-internal-boundaries.md) (ملحق §ط-10)
