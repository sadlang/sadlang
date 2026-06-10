# ADR-006a: توحيد نظام توليد الثوابت (Codegen Unification)

**الحالة:** مُعتمَد — جاهز للتنفيذ  
**التاريخ:** 2026-05-15  
**فُصِل من:** ADR-006 الأصلي (بقرار جلسة الخبراء 2026-05-15)  
**المرجع السابق:** ADR-003 (توحيد المكتبات القياسية)  
**الموقع في خارطة المشروع:** UTM-6.0 → UTM-6.7  

> **ملاحظة التقسيم:** بعد نقد الخبراء في الجولة الثانية، قُسِّم ADR-006 الأصلي إلى:
> - **ADR-006a (هذا الملف):** البنية التحتية لتوليد الكود — **مُعتمَد**.
> - **ADR-006b:** pipeline توليد التوثيق — **مؤجَّل** حتى اكتمال ADR-006a + 3 أشهر تحت حمل حقيقي.
>
> سبب التقسيم: الجزآن لهما SLA مختلف، مخاطر مختلفة، وفريق مختلف. دمجهما كان ينتج hybrid غير صالح إذا فشل أحدهما.

> ## 🔗 التطبيقات العملية لهذا الـADR
>
> هذا الـADR يحدِّد **القواعد المعمارية**. التطبيقات الفعلية على ميزات محدَّدة تعيش في epics منفصلة:
>
> | الميزة المُرحَّلة | الEpic | الحالة |
> |---|---|---|
> | `keywords.yaml` + `type_methods.yaml` | UTM-6.0 → 6.7 (داخل هذا الـADR) | 🟢 مُنفَّذ |
> | رسائل الأخطاء (203 ErrorCode) | [error_system/](../error_system/README.md) — EPIC-EM | 🟦 جاهز للتنفيذ |
> | (مستقبلاً) رسائل المكتبة القياسية | TBD | 🔴 لم يُخطَّط |
>
> **القاعدة:** كل تطبيق جديد يستهلك `scripts/codegen/_lib/{loader,emit}.py` ولا يخترع آلية موازية.

---

## 1. السياق (Context)

### 1.1. الوضع الحالي

نظام «الأسماء القانونية للدوال المدمجة وطرق الأنواع» في لغة ص يقع حالياً في **ثلاثة جزر معزولة**:

| الملف | عدد العناصر | الحالة | المصدر |
|---|---|---|---|
| `shared/builtins/include/builtin_registry.h` | 552 ثابت في 32 namespace | **يدوي** | لا مصدر — مكتوب يدوياً |
| `shared/builtins/include/module_definitions.h` | 10 modules + aliases | **يدوي** | لا مصدر — مكتوب يدوياً |
| `shared/builtins/include/builtin_categories.h` | 2 enums (Category, CompilerStrategy) | **يدوي** | يبقى يدوياً (schema — لا يُولَّد) |
| `shared/builtins/generated/type_methods_generated.h` | 80 طريقة × 7 أنواع | **مولَّد** | `data/language/type_methods.yaml` |
| `shared/builtins/generated/keywords_generated.h` | كلمات محجوزة | **مولَّد** | `data/language/keywords.yaml` |

### 1.2. المولِّدات الموجودة

- `scripts/codegen/gen_keywords.py` (273 سطر) — منعزل.
- `scripts/codegen/gen_type_methods.py` (490 سطر) — منعزل.

كل منهما يكرر:
- منطق قراءة YAML.
- منطق التحقق من schema.
- منطق `write_if_changed` (deterministic build).
- منطق `cpp_string_literal()` (UTF-8 escape).
- منطق توليد `inline constexpr std::string_view` constants.

### 1.3. المشاكل المعمارية المُبرِّرة لهذا ADR

| المشكلة | القاعدة المنتهكة | التأثير |
|---|---|---|
| 552 اسم في `builtin_registry.h` غير مرتبطة بمصدر بيانات | CW-04 (التناسق البنيوي) | تعديل اسم → بحث يدوي في 9+ ملفات |
| `gen_keywords.py` و `gen_type_methods.py` يكرران ~200 سطر | CW-19 (DRY) | تغيير منطق emit → تكرار في n مولِّد |
| لا orchestrator واحد | CW-02 (تسلسل الطبقات) | ترتيب التوليد غير واضح في CMake |
| لا schema موحَّد | CW-21 (واجهات واضحة) | إضافة مولِّد جديد = نسخ-لصق |

---

## 2. القرار (Decision)

نبني **نظام codegen موحَّد بثلاث طبقات** يحلّ محل المولِّدات المنعزلة الحالية ويُحوِّل كل الأسماء القانونية إلى مصدر حقيقة YAML.

**النطاق الصريح لهذا ADR:**
- ✅ مولِّدات C++ headers.
- ✅ `_lib/` المشتركة (loader, emit, validator, context).
- ✅ `gen_all.py` كـ orchestrator.
- ✅ CMake integration.
- ✅ YAML schemas للبيانات القانونية.
- ❌ **لا** توليد Markdown للتوثيق — هذا في ADR-006b.
- ❌ **لا** LSP output — هذا في ADR-006b.
- ❌ **لا** MkDocs integration — هذا في ADR-006b.

---

## 3. المعمارية المستهدفة

```
┌─────────────────────────────────────────────────────────────────────┐
│                     طبقة البيانات (Data Layer)                      │
│                       data/language/                                │
├─────────────────────────────────────────────────────────────────────┤
│  _meta/                                                             │
│    index.yaml              ← manifest: feeds + outputs + deps      │
│    schema/                                                          │
│      builtin_function.schema.json                                   │
│      type_method.schema.json    (موجود)                            │
│      module.schema.json                                             │
│      keyword.schema.json        (موجود)                            │
│                                                                     │
│  type_methods.yaml         ← (موجود) 80 طريقة × 7 أنواع            │
│  keywords.yaml             ← (موجود) كلمات محجوزة                  │
│  modules.yaml              ← (جديد) 10 modules + aliases           │
│  builtins/                 ← (جديد) — مقسَّم بـ namespace           │
│    core.yaml               ← Core (5) + TypeCtor (4)               │
│    concurrency.yaml        ← Concurrency (6)                       │
│    math.yaml               ← Math (~50)                            │
│    strings.yaml            ← Strings (~32)                         │
│    arrays.yaml             ← Arrays (~15)                          │
│    basics.yaml             ← Basics (~15)                          │
│    assertions.yaml         ← Assertions (~10)                      │
│    maps.yaml               ← Maps (~98)                            │
│    network.yaml, sockets.yaml, platform.yaml, kernel.yaml, ...     │
│    ui_widgets.yaml, ui_core.yaml, ui_services.yaml                  │
└─────────────────────────────────────────────────────────────────────┘
                                  │
                                  ▼
┌─────────────────────────────────────────────────────────────────────┐
│                  طبقة المكتبة المشتركة (Library Layer)              │
│                    scripts/codegen/_lib/                            │
├─────────────────────────────────────────────────────────────────────┤
│  loader.py       ← تحميل index.yaml + feeds + schema validation    │
│  validator.py    ← no-duplicates, NFC, no-shadda, no-collision      │
│  emit.py         ← cpp_string_literal, emit_namespace, write_if_changed │
│  context.py      ← GenerationContext (no global state)             │
└─────────────────────────────────────────────────────────────────────┘
                                  │
                                  ▼
┌─────────────────────────────────────────────────────────────────────┐
│                طبقة المولِّدات (Generators Layer)                    │
│                    scripts/codegen/                                 │
├─────────────────────────────────────────────────────────────────────┤
│  gen_keywords.py        ← (يُعاد كتابته فوق _lib)                   │
│  gen_type_methods.py    ← (يُعاد كتابته فوق _lib)                   │
│  gen_modules.py         ← (جديد)                                   │
│  gen_builtins.py        ← (جديد)                                   │
│  gen_all.py             ← (جديد) orchestrator                      │
└─────────────────────────────────────────────────────────────────────┘
                                  │
                                  ▼
┌─────────────────────────────────────────────────────────────────────┐
│                    طبقة المخرجات (C++ Headers Only)                 │
│                shared/builtins/generated/                           │
├─────────────────────────────────────────────────────────────────────┤
│  type_methods_generated.{h,cpp}     (موجود → يُحدَّث)               │
│  keywords_generated.{h,cpp}         (موجود → يُحدَّث)               │
│  modules_generated.h                ← يحل محل module_definitions.h │
│  builtin_names_generated.h          ← يحل محل قسم Names::* (552)  │
│  builtin_modules_generated.h        ← mapping name → ModuleId      │
│  builtin_categories_generated.h     ← mapping name → Category      │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 4. المبادئ الموجِّهة

1. **مصدر حقيقة وحيد:** لا اسم قانوني في `shared/builtins/include/` بدون مصدر YAML.
2. **توحيد الأدوات:** جميع المولِّدات تستعمل `_lib/`. لا تكرار منطق.
3. **orchestrator واحد:** CMake يستدعي `gen_all.py` فقط.
4. **API ثابت:** `Names::Core::PRINT` تبقى كما هي — الترحيل داخلي 100% (CW-24).
5. **بناء deterministic:** `write_if_changed` في كل output.
6. **schema للجميع:** كل feed له JSON Schema يُفحص قبل أي توليد.

---

## 5. خطة التنفيذ (UTM-6.0 → UTM-6.7)

| Phase | المخرَج | شرط الانتقال |
|---|---|---|
| **6.0** | ADR-006a + index.yaml + 4 schemas ✅ | هذا الملف |
| **6.1** | `_lib/{loader,emit,validator,context}.py` + إعادة كتابة gen_keywords + gen_type_methods + `gen_all.py` ✅ **منجز** (44 اختبار وحدوي / diff=0) | ✅ **مكتمل** |
| **6.2** | gen_modules + modules.yaml + modules_generated.h ✅ **منجز** (3/3 generators / C++ build / 44 pytest) | ✅ **مكتمل** |
| **6.3** | gen_builtins + Core(5) + TypeCtor(4) + Concurrency(6) = **15 ثابت** — diff=0 ✅ **منجز** (63 pytest / C++ build / gen_all 4/4) | ✅ **مكتمل** |
| **6.4** | Math(33) + Strings(21) + Arrays(10) + Basics(10) + Assertions(5) + Maps(83) = **162 ثابت** ✅ **منجز** (75 pytest / C++ build / 7 YAML files / 177 constants total) | ✅ **مكتمل** |
| **6.5** | Sockets(19)+HttpClient(18)+HttpServer(19)+NetworkUtils(12)+WebSocketClient(13)+WebSocketServer(12)+SadNet(15)+Platform(9)+Kernel(30)+KernelTimers(30)+KernelThreads(20)+KernelUSB(19) = **216 ثابت** ✅ **منجز** (42 pytest / C++ build / 9 YAML files / 393 constants total) | ✅ **مكتمل** |
| **6.6** | UIWidgets(42)+UICore(20)+UIAudio(5)+UICrypto(6)+UINetwork(4)+UIIO(7)+UIDialog(4)+UITimer(3)+UIStorage(7)+UIPlatform(43)+UIDevice(18) = **159 ثابت** ✅ **منجز** (96 pytest / C++ build / 10 YAML files / 552 constants total) | ✅ **مكتمل** |
| **6.7** | حذف جميع `inline constexpr std::string_view` اليدوية من `builtin_registry.h` + تفعيل `enforce_guards` ✅ **منجز** (102 pytest / C++ build / guard يطبع [guard] OK) | ✅ **مكتمل** |

**شرط ذهبي:** جميع الاختبارات تبقى خضراء بعد كل phase. صفر regression.

---

### تفصيل UTM-6.3 — المهام والمعايير

**الهدف:** ترحيل Core + TypeCtor + Concurrency = 15 ثابت + بناء `gen_builtins.py` كأداة قادرة على ترحيل الكل.

#### 6.3-T1: YAML + Schema
- إنشاء `data/language/builtins/` مجلد
- إنشاء `data/language/builtin_function.schema.json`:
  ```json
  حقول إلزامية: cpp_id (UPPER_SNAKE_CASE), canonical (Arabic), namespace (e.g. "Core")
  حقول اختيارية: category, module_id, description_ar, description_en
  ```
- إنشاء `data/language/builtins/core.yaml` — 15 دالة (Core + TypeCtor + Concurrency)

#### 6.3-T2: gen_builtins.py
- يقرأ `builtins/*.yaml` → يُجمِّع كل namespaces → يُنتج `shared/builtins/generated/builtin_names_generated.h`
- البنية المُنتَجة: `namespace Sad::Builtins::Names::Core { inline constexpr std::string_view PRINT = "اطبع"; ... }`
- **شرط صلب diff=0:** القيم والأسماء مطابقة 100% لـ builtin_registry.h الحالي
- تسجيل في `gen_all.py` + `cmake/codegen.cmake` (`sad_builtins_codegen` + تحديث `sad_all_codegen`)

#### 6.3-T3: تحديث builtin_registry.h
- حذف `namespace Core {}` + `namespace TypeCtor {}` + `namespace Concurrency {}` اليدوية
- إضافة `#include "../generated/builtin_names_generated.h"`
- بناء C++ يجب أن ينجح بدون أي تغيير في باقي المشروع

#### 6.3-T4: اختبارات وحدوية
- إنشاء `scripts/codegen/test_gen_builtins.py`
- اختبارات: diff=0 لكل namespace مُرحَّل، schema يرفض `cpp_id` بصيغة خاطئة، يرفض `canonical` فارغ
- يجب أن تمر **44 + N اختبار** كلها

#### شرط الانتقال لـ UTM-6.4:
`cmake --build build --target sad` نجح ✓ · `python gen_all.py` يطبع `4/4 passed` ✓ · pytest خضراء ✓

---

### توزيع YAML files المخطط (UTM-6.3 → 6.7)

| الملف | Namespaces | الثوابت | UTM |
|---|---|---|---|
| `builtins/core.yaml` | Core(5) + TypeCtor(4) + Concurrency(6) | **15** | 6.3 |
| `builtins/math.yaml` | Math(33) | **33** | 6.4 |
| `builtins/strings.yaml` | Strings(21) | **21** | 6.4 |
| `builtins/arrays.yaml` | Arrays(10) | **10** | 6.4 |
| `builtins/basics.yaml` | Basics(10) | **10** | 6.4 |
| `builtins/assertions.yaml` | Assertions(5) | **5** | 6.4 |
| `builtins/maps.yaml` | Maps(83) | **83** | 6.4 |
| `builtins/network.yaml` | Sockets+HttpClient+HttpServer+NetworkUtils+WebSocket*+SadNet | **108** | 6.5 |
| `builtins/platform.yaml` | Platform+Kernel+KernelTimers+KernelThreads+KernelUSB | **108** | 6.5 |
| `builtins/ui.yaml` | UIWidgets+UICore+UIAudio+UICrypto+UINetwork+UIIO+UIDialog+UITimer+UIStorage+UIPlatform+UIDevice | **162** | 6.6 |
| **الإجمالي** | | **555** | |

> (AR) ملاحظة: ADR-006 الأصلي ذكر 552 ثابت. الفحص الفعلي بـ `Select-String` في 2026-06-09 أعطى 552 — التفاوت 3 محتمل في ثوابت مُضافة حديثاً.



قبل الانتقال من 6.1 إلى 6.2، يجب اجتياز جميع اختبارات `_lib/` الوحدوية:

| الملف | الحالة الاختبارية الإلزامية |
|---|---|
| `loader.py` | `load_index()` ناجح + فشل عند index.yaml مشوَّه + `load_feed()` ناجح + `load_feed()` يرمي `ValidationError` عند schema mismatch |
| `emit.py` | `cpp_string_literal()` صحيح مع: حرف عادي, شدّة (U+0651), ZWJ (U+200D), `"`, `\`, أحرف > 0x7F |
| `validator.py` | كل `validate_*`: حالة نجاح + حالة فشل + حالة edge (فراغ، تكرار، shadda خفي) |
| `context.py` | `GenerationContext` لا يحمل global state — اختبار إنشاء نسختين مستقلتين بدون تداخل |

### خطة Rollback (ملاحظة بايج ب)

كل phase مستقل وقابل للتراجع بأمان:
- لا shared state بين المراحل — كل phase يُنتج ملفات generated جديدة.
- عند فشل أي phase: `git revert` إلى tag آخر phase ناجح.
- الضمان: `write_if_changed` يعني أن revert = إعادة generated headers القديمة بدون إعادة build كاملة.
- **الحد الأقصى للخسارة:** phase واحد من العمل — لا يوجد حالة hybrid لا يمكن الرجوع منها.

### Schema Versioning Policy (ملاحظة بايج ج)

`index.yaml` يحمل `version: 1` (انظر ADR-006 الأصلي القسم 3.1). السياسة:
- **تغيير additive** (حقل اختياري جديد في schema): `version` لا يتغير.
- **تغيير breaking** (حقل إلزامي جديد أو تغيير enum): `version` يرتفع إلى 2، و`gen_all.py` يتحقق من التوافق ويرمي خطأ واضحاً مع تعليمات الترحيل.
- **لا deprecation للـ schema نفسه** في نطاق ADR-006a — هذا يُعالَج في ADR-006b عند رفع التجميد.

---

## 6. معايير القبول (Acceptance Criteria)

1. ✅ صفر ثابت `inline constexpr std::string_view` يدوي في `shared/builtins/include/` (ما عدا `builtin_categories.h` — enums فقط).
2. ✅ جميع الـ generators تستعمل `_lib/` — لا تكرار منطق.
3. ✅ `gen_all.py` هو نقطة الدخول الوحيدة من CMake.
4. ✅ جميع الاختبارات الموجودة تبقى خضراء بعد كل phase.
5. ✅ CMake guard يفشل البناء عند إضافة ثابت يدوي.
6. ✅ `Names::Core::PRINT`, `Names::Concurrency::CHANNEL`, `ModuleId::MATH`, `resolveModuleId("رياضيات")` تعمل بنفس السلوك.

---

## 7. المخاطر والتخفيف

| المخاطرة | الاحتمال | التأثير | التخفيف |
|---|---|---|---|
| خطأ مطبعي في ترحيل 552 ثابت | متوسط | عالي | كل phase يقارن `nm builtin_registry.o` قبل/بعد |
| schema غير مكتمل | منخفض | متوسط | UTM-6.1 يُرحِّل المولِّدين الموجودين أولاً (proof-of-concept) |
| تكامل CMake يكسر incremental builds | منخفض | عالي | UTM-6.1 يحافظ على نفس DEPENDS ونفس OUTPUT paths |
| مساهمون يضيفون constants يدوية بعد UTM-6.7 | متوسط | عالي | `enforce_guards` في CMake + رسالة خطأ واضحة |

---

## 8. الوثيقة المرتبطة

- **ADR-006b** (مؤجَّل): توليد التوثيق من YAML → Markdown. لن يُنفَّذ قبل:
  - استقرار ADR-006a تحت حمل حقيقي لمدة 3 أشهر.
  - وجود Document-IR حقيقي (ليس Jinja2 مباشرة إلى Markdown).
  - خطة ترحيل مصفوفة (ملف → مصير → مالك → تاريخ) للمحتوى الحالي.
  - نظام versioning (`since`, `deprecated_in`, `stability`) في schema.
