# البناء والاختبار وإعادة التوليد

## 1. البناء

```powershell
# تهيئة البناء لأول مرة (يُشغّل codegen تلقائياً)
cmake -S . -B build

# بناء المفسر فقط (أسرع)
cmake --build build --config Debug --target sad-run

# بناء المترجم sad-build (Release، أو Debug عند توفّر LLVM Debug)
cmake --build build --config Release --target sad-build

# بناء كل شيء
cmake --build build --config Debug
```

- **LLVM 18** اختياري — للمترجم `sad-build` فقط (`ENABLE_LLVM_BACKEND=ON`، `#ifdef HAS_LLVM`).
- الاختبارات معطّلة افتراضياً — فعّلها بـ `-DBUILD_TESTS=ON` عند التهيئة.

## 2. إعادة التوليد من YAML

التوليد يحدث تلقائياً وقت البناء عند تغيّر أي YAML. لتشغيله يدوياً:

```powershell
# بيئة Python (مرة واحدة)
pip install pyyaml jsonschema

# تشغيل كل المولّدات (المُنسِّق الموحَّد + حارس الانجراف --check)
python x.py gen

# أو مولّد محدد — مثال الكلمات المفتاحية
python scripts/codegen/gen_keywords.py `
    --yaml language-truth/keywords.yaml `
    --schema language-truth/_schemas/keywords.schema.json `
    --header shared/lexer/generated/keywords_generated.h `
    --source shared/lexer/generated/keywords_generated.cpp

# تحقق الـ Schemas مدمج في المولّدات نفسها (jsonschema داخل gen_*.py — لا أداة مستقلّة)
```

> إن لم ينعكس تغيير YAML بعد البناء: احذف ملف الإخراج المُولَّد وأعد البناء لإجبار التوليد.

## 3. تشغيل ملف `.ص`

> ⚠️ **انتبه لاسم الملف التنفيذي:** `sad.exe` صار **موزِّع أوامر (hub)** بعد إعادة هيكلة
> الأدوات. المُفسِّر الفعلي هو **`sad-run.exe`** — استدعِه مباشرةً.

```powershell
.\build\bin\Debug\sad-run.exe examples\test_simple.ص       # المفسر (الفعلي)
.\build\bin\Debug\sad-build.exe ملف.ص -o ملف.exe           # المترجم (هدف sad-build؛ Release أيضًا صالح)
.\build\bin\Debug\sad-build.exe ملف.ص --أظهر-llvm -o ملف.ll # فحص LLVM IR (BF-07)
```

## 4. الاختبارات — مُشغّل التنفيذ المزدوج (الأساسي)

الأداة المعتمَدة لاختبار `.ص` هي **`tests/runner.py`**: تُشغّل كل ملف عبر
المفسر **والمترجم** وتقارن المخرجات (لا تمر إلا إذا تطابقا). هذا يحقق قاعدة BF-08 آلياً.

```powershell
pip install pyyaml

# دخان سريع (5 اختبارات — لكل commit)
python tests/runner.py --level P0

# قسم ميزة واحدة أثناء التطوير
python tests/runner.py --section متغيرات
python tests/runner.py --level P0.أنماط      # دخان + قسم

# هرم المستويات: P0 (commit) ⊂ P1 (كل PR) ⊂ P2 (nightly) ⊂ P3 ⊂ P4 ⊂ full (807+)
python tests/runner.py --level P1            # المطلوب قبل أي PR
python tests/runner.py --level P0 --cpu sequential  # تسلسلي للتشخيص
python tests/runner.py --level full --report --html # تقرير شامل
```

دليل كامل: `tests/behavior/README.md`. المسارات في `tests/config.yaml`
(`interpreter: build/bin/Debug/sad-run.exe`, `compiler: build/bin/Debug/sad-build.exe`).
الاسم البائت `sadc.exe` متقاعد — لا يُنتجه أيّ هدف؛ المترجم الآن هدف `sad-build`.

### 4.1 صيغة ملف اختبار `.ص` (توجيهات التعليق)

```sad
# @description: وصف موجز للاختبار
# @expected: الناتج المتوقع حرفياً (سطر بسطر)
# @priority: P0
# @requires: اطبع_سطر
# @skip_compiler: السبب     ← (اختياري) إن كانت الميزة غير مدعومة في المترجم (sad-build) بعد

اطبع_سطر("مرحبا بالعالم")
```
- ضع الملف في القسم المناسب تحت `tests/behavior/sections/<القسم>/` (مثل `core/`, `match/`, `errors/`).
- `@expected` يطابق مخرج **كلا** المسارين؛ مع `@skip_compiler` يطابق مخرج المفسر فقط.

### 4.2 اختبارات C++ والمولّدات والتدقيقات

```powershell
# اختبارات C++ للوحدات (مثل نظام الأخطاء)
cmake --build build --config Debug --target comprehensive_tests
ctest --test-dir build -R Comprehensive

# اختبارات مولّدات الكود (Python)
python -m pytest scripts/codegen/
python scripts/codegen/test_gen_error_messages.py
python scripts/codegen/test_gen_builtins.py

# تدقيقات الدوال المضمنة
python scripts/codegen/audit_duplicate_registrations.py
python scripts/codegen/check_docs_coverage.py
```

## 5. منهجية الاختبار عند إصلاح خطأ (BF-01, BF-08, BF-29)

1. **أعِد إنتاج الخطأ** في أصغر ملف `.ص` ممكن.
2. **قارن المفسر والمترجم:** إن عمل في `sad-run` وفشل في `sad-build` → المشكلة في SIR builder أو
   LLVM codegen. ولّد IR بـ `--أظهر-llvm` وافحص: entry block، اتساق الأنواع، ترتيب
   التعليمات، فهارس `getelementptr`.
3. **أصلِح في الطبقة الصحيحة** (BF-10) — لا ترقيع في مكان الاستعمال.
4. **اختبار التراجع الشامل** قبل الاعتبار منتهياً:
   - الحزمة الشاملة تمر
   - `sad-build` يبني بلا أخطاء
   - `sad-run` يعمل بلا تراجع

## 6. ملاحظات المترجم sad-build

- يُبنى **Debug** (أُصلح بناء المترجم في Debug جذريًّا في cmake/llvm.cmake) أو **Release**.
- أهداف مدعومة: x86_64, ARM64, WebAssembly, Arduino, ESP32.
- مستويات تحسين: `-O0`..`-O3`, `-Os`. يدعم bare-metal/freestanding لأنظمة التشغيل.
