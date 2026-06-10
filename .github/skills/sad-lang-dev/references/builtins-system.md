# نظام الدوال المضمنة في لغة ص

نظام الدوال المضمنة **مدفوع بالبيانات** ويعيش في ثلاث طبقات: تعريف YAML (SoT)، سجل مُولَّد،
وتنفيذ C++ (مفسر + codegen مترجم).

> ⚠️ **ملاحظة مسارات:** بعض الوثائق القديمة (مثل `BUILTIN_CODING_STANDARDS.md`) تشير إلى
> `interpreter_new/src/builtins/`. المسار **الفعلي الحالي** هو `interpreter/src/builtins/`.
> اعتمد المسار الفعلي دائماً.

## 1. الطبقات الثلاث

```
language-truth/builtins/*.yaml          ← (1) المصدر SoT: تعريف كل دالة (اسم، وحدة، استراتيجية)
        │  gen_builtins_registry.py
        ▼
shared/builtins/generated/
    builtin_registry_generated.h        ← (2) السجل المُولَّد (لا يُحرَّر يدوياً)
        │
        ├──► interpreter/src/builtins/builtin_*.cpp     ← (3أ) تنفيذ المفسر (lambda C++)
        └──► compiler/src/backend/llvm/builders/builtins/  ← (3ب) توليد LLVM IR للمترجم sadc
```

## 2. تعريف دالة في YAML

ملفات `language-truth/builtins/*.yaml` مقسّمة حسب المجال (`core.yaml`, `math.yaml`,
`strings.yaml`, `arrays.yaml`, `kernel_*.yaml`, `compiler_*.yaml`, ...). الفهرس في `_index.yaml`.

```yaml
- cpp_id: SQRT                  # المعرّف في C++ (ثابت)
  canonical: جذر               # الاسم العربي القانوني المستخدم في الكود
  namespace: Math
  category: MODULE_FUNCTION     # CORE_FUNCTION | MODULE_FUNCTION | TYPE_METHOD ...
  require_import: true          # هل تحتاج `استورد`؟ (false = أساسية تلقائية)
  module: MATH                  # الوحدة التي تنتمي لها
  compiler_strategy: LLVM_INTRINSIC  # كيف يولّدها sadc: LLVM_INTRINSIC | RUNTIME_CALL | INLINE
  since: 0.1.0
  status: stable                # stable | experimental | deprecated
  description_ar: الجذر التربيعي
  description_en: Square root
  params: []
  lookup_table: MODULE_FUNCTION
```

### الحقول المفصلية

- **`require_import`**: `false` → الدالة في النواة (تُحمَّل تلقائياً، بلا `استورد`). `true` → تحتاج وحدتها.
- **`compiler_strategy`**: يحدد كيف يترجمها `sadc`:
  - `LLVM_INTRINSIC` — تُربط بـ intrinsic مباشر من LLVM (أسرع)
  - `RUNTIME_CALL` — تُستدعى من runtime الخاص باللغة
  - `INLINE` — تُدمج مباشرة في IR
- **`canonical`** هو الاسم القانوني الوحيد، ويُولَّد كثابت `Bn::<Group>::<CPP_ID>`. الألقاب
  النصية اليدوية (aliases) **مُلغاة** (`scripts/codegen/remove_aliases.py`) — لا تسجّل أسماء بديلة يدوياً.

## 3. التحميل الكسول (Lazy Loading)

1. عند بدء البرنامج → تُسجَّل دوال النواة فقط (`require_import: false`).
2. عند `استورد وحدة` → المفسر يبحث في سجل الوحدات ويستدعي دالة تسجيل تلك الوحدة فقط.
3. إن لم تُوجد كوحدة مدمجة → يبحث `ModuleResolver` عن ملف `.ص`.

**الفائدة:** بدء سريع، ذاكرة أقل، كل وحدة تُحمَّل مرة واحدة.

## 4. ملفات التنفيذ في المفسر

| النمط | الاستخدام |
|------|-----------|
| `builtin_core_*.cpp` | دوال النواة (بلا استيراد) — مثل `builtin_core_io.cpp` |
| `builtin_module_*.cpp` | وحدة تحتاج `استورد` — مثل `builtin_module_strings.cpp` |
| `builtin_kernel_*.cpp` | دوال نظام التشغيل/النواة — مثل `builtin_kernel_cpu.cpp` |
| `builtin_registry.cpp` | السجل المركزي + `suggestModuleForFunction` (ملف واحد) |
| `builtin_common.h` | header موحّد للـ includes المشتركة |

**ممنوع** (من `BUILTIN_CODING_STANDARDS.md`): أسماء عامة (`builtin_extras.cpp`)، أرقام
تسلسلية (`builtin_3.cpp`)، اختصارات غامضة (`builtin_misc.cpp`)، أو ملفات بلا بادئة `builtin_`.

## 5. إضافة دالة مضمنة جديدة (الإجراء الكامل)

1. **عرّفها في YAML** — أضِف الكتلة في `language-truth/builtins/<domain>.yaml` المناسب
   (أنشئ ملفاً جديداً وأضِفه إلى `_index.yaml` إن لم يناسبها مجال).
2. **أعد توليد السجل:**
   ```powershell
   python scripts/codegen/gen_builtins_registry.py `
       --yaml-dir language-truth/builtins `
       --index   language-truth/builtins/_index.yaml `
       --out-h   shared/builtins/generated/builtin_registry_generated.h
   ```
   (أو ببساطة أعد بناء CMake — التوليد ضمن الهدف `sad_builtin_registry_codegen`.)
3. **نفّذها في المفسر** — في ملف `builtin_*.cpp` المناسب. **التسجيل عبر الثابت المُولَّد
   `Bn::<Group>::<CPP_ID>` (من YAML)، والأخطاء عبر `ErrorCode` + placeholders — لا سلاسل حرفية
   ولا نص خطأ حر:**
   ```cpp
   namespace Bn = Sad::Builtins::Names;   // أعلى الملف
   // (AR) جذر — الجذر التربيعي لرقم
   // (EN) sqrt — square root of a number
   auto sqrt_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args)
       -> std::shared_ptr<Data::Value> {
       if (args.size() != 1 || !args[0]->isNumeric()) {   // CW-18: فاحصات Value
           Sad::Errors::throwRuntime(
               Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED, pos, {{"func", "جذر"}});
       }
       double x = args[0]->isInteger()
                    ? static_cast<double>(args[0]->toInt64()) : args[0]->toDouble();
       return std::make_shared<Data::Value>(std::sqrt(x));
   };
   // الثابت المُولَّد فقط — الألقاب اليدوية مُلغاة (remove_aliases.py)
   interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bn::Math::SQRT), sqrt_fn);
   ```
   > **قاعدة صارمة** (`runtime_throw.h`): لا نسخة من `throwRuntime` تقبل نصاً حراً — نص الرسالة
   > (عربي/إنجليزي + `fix_hint`) يعيش في YAML الأخطاء وحده. لا تخترع `ErrorCode` غير موجود.
   > الملفات القديمة قد تُظهر `throw std::runtime_error("...")` — هذا **نمط مهجور**، لا تقلّده.
4. **نفّذها في المترجم** (إن كان `sadc` يدعمها) — أضِف codegen في
   `compiler/src/backend/llvm/builders/builtins/` وفق `compiler_strategy`. الملفات هناك
   مقسّمة بالمجال: `math_builtins.cpp`, `io_builtins_ops.cpp`, `builtins_network_ops.cpp`,
   `builtins_math_async_ops.cpp`, `security_builtins_ops.cpp`, `builtin_funcs_ops.cpp`
   (راجع `README.md` في نفس المجلد). `LLVM_INTRINSIC` → اربط intrinsic، `RUNTIME_CALL` →
   نادِ دالة runtime، `INLINE` → ولّد IR مباشرة.
5. **اكتب اختباراً** بلغة ص (`_test_*.ص`) واختبر في المفسر **والمترجم** (BF-08, BF-29).

## 6. إضافة وحدة جديدة

1. أنشئ `interpreter/src/builtins/builtin_module_<name>.cpp` بالهيكل الكامل (header مزدوج
   اللغة، `#include "builtin_common.h"`، دالة تسجيل `registerModule<Name>`).
2. سجّل الوحدة في `builtin_registry.cpp` (عبر ثابت اسم الوحدة المُولَّد → دالة التسجيل).
3. أنشئ `language-truth/builtins/<name>.yaml` وأضِفه إلى `_index.yaml`.
4. أضِفه إلى CMake إن لزم (مصدر جديد).

## 7. قواعد التنفيذ الصارمة (من BUILTIN_CODING_STANDARDS)

- **تحقق المعاملات في أول سطرين** (عدد، نوع، فراغ) — CW-18.
- **رسائل خطأ مزدوجة اللغة** داخل الـ lambda.
- **RAII** لأي مورد؛ لا `malloc/free` مباشر — CW-16.
- **حدود المصفوفات** قبل أي وصول بفهرس — CW-17.
- **التزامن:** الدوال التي تُستدعى من goroutines يجب أن تكون reentrant/آمنة.
- **حجم الملف < 800 سطر** — قسّمه إن تجاوز (CW-05).

## 8. التدقيق والتحقق

```powershell
python scripts/codegen/audit_duplicate_registrations.py   # كشف تسجيل مكرر بين الوحدات
python scripts/codegen/check_docs_coverage.py             # تغطية التوثيق لكل دالة
python scripts/validate_schemas.py            # يقبل --strict / --verbose فقط (لا --truth-dir)
```
