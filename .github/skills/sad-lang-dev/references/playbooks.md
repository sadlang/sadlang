# كتيّب المهام — تدفّقات قابلة للتنفيذ (قوائم مهام)

> المراجع الأخرى تشرح «لماذا» و«ما هي الأنظمة». هذا الملف يعطيك **قائمة مهام مرقّمة تتبعها
> حرفياً** لكل مهمة شائعة، مع **كيفية تعديل الكود** بالضبط. انسخ القائمة، نفّذها بالترتيب،
> علّم كل بند `[x]` فقط بعد إتمامه فعلاً.

---

## كتيّب 1: إضافة دالة مضمنة جديدة

**مثال:** `جذر_تكعيبي(س)` في وحدة `رياضيات`.

```
[ ] 1. عرّف في YAML
[ ] 2. أعد توليد السجل
[ ] 3. نفّذ في المفسر
[ ] 4. نفّذ في المترجم 
[ ] 5. ادمج نظام الأخطاء
[ ] 6. اكتب اختبار .ص (إيجابي + سلبي)
[ ] 7. ابنِ + شغّل runner
[ ] 8. اجتَز قائمة التسليم
```

### المهمة 1 — عرّف في YAML
افتح `language-truth/builtins/math.yaml`، أضِف (انسخ نمط دالة موجودة بجوارها):
```yaml
- cpp_id: CBRT
  canonical: جذر_تكعيبي
  namespace: Math
  category: MODULE_FUNCTION
  require_import: true
  module: MATH
  compiler_strategy: RUNTIME_CALL
  since: 5.0.0
  status: stable
  description_ar: الجذر التكعيبي        # إلزامي — وإلا يفشل حارس التوثيق
  description_en: Cube root
  params: []
  lookup_table: MODULE_FUNCTION
```

### المهمة 2 — أعد توليد السجل
```powershell
python scripts/codegen/gen_builtins_registry.py --yaml-dir language-truth/builtins `
    --index language-truth/builtins/_index.yaml `
    --out-h shared/builtins/generated/builtin_registry_generated.h
```
(أو أعد بناء CMake). **ضمّ الملف المُولَّد للـ commit.**

### المهمة 3 — نفّذ في المفسر (وفق ثلاثة أنظمة متداخلة)
في `interpreter/src/builtins/builtin_module_math.cpp`، داخل دالة تسجيل الوحدة. **التسجيل يتمّ
عبر الثابت المُولَّد من YAML — لا سلسلة عربية حرفية.** الثابت `Bn::Math::CBRT` يُولَّد من حقل
`cpp_id: CBRT` في `shared/builtins/generated/builtin_registry_generated.h`:

```cpp
namespace Bn = Sad::Builtins::Names;   // أعلى الملف (موجود مسبقاً)

// (AR) جذر_تكعيبي — الجذر التكعيبي لرقم
// (EN) cbrt — cube root of a number
auto cbrt_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args)
    -> std::shared_ptr<Data::Value> {
    // ① نظام الأنواع: استخدم فاحصات Value لا فحصاً يدوياً
    if (args.size() != 1 || !args[0]->isNumeric()) {
        // ② نظام الأخطاء: ErrorCode مُولَّد + placeholders — ممنوع نص حر (runtime_throw.h)
        Sad::Errors::throwRuntime(
            Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED,   // رمز فعلي من error_codes.h
            pos,                                              // Position (انظر ملاحظة الموقع)
            {{"func", "جذر_تكعيبي"}});                       // متغيّرات القالب
    }
    double x = args[0]->isInteger()
                 ? static_cast<double>(args[0]->toInt64())   // الدالة الفعلية toInt64()
                 : args[0]->toDouble();
    // ③ نظام الأنواع: ابنِ Value عبر بنّاء النوع الصحيح
    return std::make_shared<Data::Value>(std::cbrt(x));
};
// نظام التسجيل: الثابت المُولَّد فقط (لا "جذر_تكعيبي" حرفية، ولا ألقاب يدوية)
interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bn::Math::CBRT), cbrt_fn);
```

> **قواعد إلزامية (مؤكَّدة من الكود — ليست أسلوبية):**
> - **التسجيل بالثابت المُولَّد** `Bn::<Group>::<CPP_ID>` (انظر `builtin_core_io.cpp`). الألقاب
>   النصية اليدوية **مُلغاة** (`scripts/codegen/remove_aliases.py`) — الاسم القانوني من YAML وحده.
> - **الأخطاء بـ `ErrorCode` + placeholders فقط.** `runtime_throw.h` ينصّ صراحةً: «لا توجد نسخة
>   تأخذ نصاً حراً؛ نص الرسالة يعيش في ErrorCatalog وحده». استخدم رمزاً فعلياً
>   (`RUN_TYPE_CHECK_FAILED`, `SEM_WRONG_ARG_COUNT`...) أو **أضِف رمزاً جديداً** عبر كتيّب 2 +
>   [./error-system.md](./error-system.md) §4 إن لم يناسب الموجود. الرسالة العربية/الإنجليزية + `fix_hint`
>   تعيش كلها في YAML الأخطاء — لا في كود C++.
> - **ملاحظة الموقع (Position):** `throwRuntime(ErrorCode, Position, placeholders)`. مرّر `pos` من
>   سياق الاستدعاء؛ إن كانت بنية الـ lambda لا تملكه، اتبع نمط ملفات الدوال **المُحدَّثة حديثاً**
>   (التي تستخدم `Bn::`/`ErrorCode`) لا الملفات القديمة التي ما زالت ترمي `std::runtime_error` (مهجور).
> - **النوع:** اعتمد فاحصات `Value` (`isNumeric/isInteger/...`) وبنّاءاته — لا تحويلات خام (CW-14).

### المهمة 4 — نفّذ في المترجم
في `compiler/src/backend/llvm/builders/builtins/math_builtins.cpp` أضِف codegen وفق
`compiler_strategy` (`RUNTIME_CALL` → نادِ دالة runtime). إن تعذّر الآن، علّم الاختبار `@skip_compiler`.

### المهمة 5 — الأخطاء (مدمجة في المهمة 3)
لا `throw std::runtime_error("نص")`. استخدم رمزاً من الكتالوج؛ إن لم يوجد مناسب، أضِف رمزاً
عبر [./error-system.md](./error-system.md) §4.

### المهمة 6 — اختبار `.ص`
`tests/dual_execution/math/cbrt.ص`:
```sad
# @description: الجذر التكعيبي
# @expected: 3
# @priority: P1
استورد رياضيات
اطبع_سطر(جذر_تكعيبي(27))
```
وأضِف ملفاً سلبياً يتحقق أن `جذر_تكعيبي("نص")` يُطلق الخطأ الصحيح (BF-22).

### المهمة 7 — ابنِ وشغّل
```powershell
cmake --build build --config Debug --target sad
cmake --build build --config Release --target sadc
python tests/dual_execution/runner.py --section رياضيات
python tests/dual_execution/runner.py --level P1
```

### المهمة 8 — التسليم
اجتَز [./delivery-checklist.md](./delivery-checklist.md) بالكامل.

---

## كتيّب 2: إضافة رمز خطأ جديد

```
[ ] 1. أضِف الرمز في enum ErrorCode (shared/errors/include/error_codes.h)
[ ] 2. أضِف الرسالة في language-truth/errors/<cat>.yaml (المصدر الموحَّد — title/brief/fix_hint/detailed بـ ar+en)
[ ] 4. أعد التوليد (تحقّق gen_error_messages.py --help للوسائط)
[ ] 5. أطلِق الخطأ من الطبقة الصحيحة (BF-10) مع RenderContext للـ placeholders
[ ] 6. اكتب اختباراً يعيد إنتاج الحالة
[ ] 7. ابنِ + شغّل comprehensive + runner P1
[ ] 8. اجتَز قائمة التسليم
```
التفاصيل: [./error-system.md](./error-system.md) §4.

---

## كتيّب 3: إضافة كلمة مفتاحية

```
[ ] 1. language-truth/keywords.yaml (محجوزة أو سياقية — راجع adding-systems)
[ ] 2. أعد توليد keywords_generated.* (ضمّه للـ commit)
[ ] 3. Parser: قاعدة في shared/parser/src/<dir>/ (سياقية → تحقّق مزدوج)
[ ] 4. AST: عقدة جديدة في shared/ast/include/ (ترث ASTNode)
[ ] 5. المفسر: visitor في interpreter/src/visitors/
[ ] 6. المترجم: SIRBuilder + LLVM (opcode جديد إن لزم)
[ ] 7. LSP: تلوين (سياقية → semantic_tokens_provider.cpp)
[ ] 8. اختبار .ص + ابنِ + runner P1
[ ] 9. اجتَز قائمة التسليم
```
التفاصيل: [./adding-systems.md](./adding-systems.md) §أ.

---

## كتيّب 4: إصلاح خطأ يظهر في المترجم دون المفسر

```
[ ] 1. أعِد الإنتاج في أصغر ملف .ص (BF-01)
[ ] 2. أكّد: يعمل في sad-run، يفشل في sadc → المشكلة SIR/LLVM (BF-08)
[ ] 3. ولّد IR: sadc ملف.ص --emit-llvm -o ملف.ll وافحص (entry block، أنواع، getelementptr) (BF-07)
[ ] 4. حدّد الطبقة: ترتيب حقول → SIRBuilder ؛ تحويل نوع → LLVM codegen (BF-10)
[ ] 5. أصلِح السبب الجذري لا العَرَض — لا تحويل ضمني لإخفاء (BF-09, BF-04)
[ ] 6. اكتب اختبار يفشل قبل ويمرّ بعد + ابحث عن نفس النمط في أماكن أخرى (BF-26)
[ ] 7. runner P1 — لا تراجع
[ ] 8. اجتَز قائمة التسليم + وثّق السبب (BF-14)
```

---

## كيفية تعديل كود نظام أثناء التنفيذ (قواعد عامة)

0. **استخدم الثوابت المُولَّدة لا السلاسل الحرفية:** أسماء الدوال (`Bn::<Group>::<CPP_ID>`)، طرق
   الأنواع (`TM::<Group>::<NAME>`)، ورموز الأخطاء (`ErrorCode::<NAME>`) — كلها مُولَّدة من YAML.
   السلاسل العربية/الإنجليزية الحرفية والألقاب اليدوية **مهجورة** (`remove_aliases.py`).
1. **اقرأ 3 ملفات/دوال مجاورة** في نفس المجلد وقلّد نمطها بدقّة (CW-04, BF-25) — الترويسة،
   ترتيب الـ includes، التسمية، تنظيم namespaces. **قلّد الملفات المُحدَّثة** (التي تستخدم الثوابت
   المُولَّدة) لا القديمة (`throw std::runtime_error` / سلاسل حرفية).
2. **لا تخترع نمطاً جديداً** إن وُجد نمط متبع — أضِف عبر عقدة/visitor/opcode جديد لا بتعديل
   switch موجود (CW-20).
3. **API عام = تعليق مزدوج اللغة** `@brief (AR)`/`@brief (EN)` (CW-08).
4. **تحقّق المعاملات أولاً** (CW-18)، **حدود المصفوفات قبل الوصول** (CW-17)، **RAII للموارد** (CW-16).
5. **ملف > 800 سطر؟ قسّمه** بأسماء واضحة (CW-05). دالة > 50 سطر؟ قسّمها (CW-01).
6. **بعد التعديل:** أعد التوليد إن مسستَ YAML، ابنِ، شغّل `runner`، ثم قائمة التسليم.
