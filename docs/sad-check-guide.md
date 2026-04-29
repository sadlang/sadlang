# دليل sad-check — فاحص الملكية والأنواع الثابت

> **الإصدار:** `sad-check 1.1.0` (مع الواجهة الموحَّدة في `sad-hub`)
>
> **الموقع:**
> - مباشر: `build/bin/Debug/sad-check.exe`
> - عبر الهب: `sad check <args>` (يُمرَّر تماماً إلى sad-check)

---

## 1. ما هو sad-check؟

`sad-check` فاحص ملكية واستعارة **ثابت (static)** يحلّل ملفات `.ص` **بدون تنفيذها**.
يعتمد على نفس المكوّنات المشتركة المستخدمة في `sad-run` و `sad-build`:

```
shared/lexer  →  shared/parser  →  shared/ownership::BorrowChecker
```

**ضمان التوحيد:** نفس قواعد الملكية تُطبَّق في:

| المسار | المحرّك |
|---|---|
| `sad-run` (المفسر الشجري) | `Sad::Semantic::OwnershipTracker` (مغلَّف بـ `interpreter/managers/ownership_manager`) |
| `sad-build` (المترجم LLVM) | `Sad::Semantic::OwnershipTracker` (مباشر) |
| `sad-check` (الفحص الثابت) | `Sad::Semantic::BorrowChecker` فوق نفس المحرّك |

النتيجة: **تطابق بايت-ببايت** — ما يقبله `sad-check` يقبله المسارات الأخرى دائماً.

---

## 2. سيناريوهات الاستخدام

### 2.1 فحص ملف واحد (الوضع الافتراضي)

```powershell
sad-check examples\01_مرحبا.ص
```

المخرج عند النجاح:
```
✓ examples\01_مرحبا.ص: نظيف / clean  (vars=3 borrows=0 moves=0)
```

**رمز الخروج:** `0` (نظيف) أو `1` (أخطاء ملكية/نحوية) أو `2` (لا يمكن قراءة الملف).

### 2.2 فحص مجلد بأكمله — `--recursive`

```powershell
sad-check -r examples
```

يكتشف **جميع** ملفات `.ص` (UTF-8: `2E D8 B5`) داخل المجلد بشكل تكراري ويفحصها واحداً تلو الآخر، ثم يطبع ملخصاً نهائياً تلقائياً.

### 2.3 وضع الإحصائيات المختصرة — `--summary`

مناسب لـ **CI** (لا ضوضاء، كتلة واحدة فقط):

```powershell
sad-check -r --summary examples
```

```
── ملخص / Summary ──
  files=8  clean=4  ownership-fail=0  parse-fail=4  read-fail=0
  vars=14  borrows=0  moves=0  errors=0
```

> الكاتم الداخلي `StderrSilencer` يحجب جميع رسائل تشخيص الـparser في هذا الوضع لضمان مخرج آلي نظيف.

### 2.4 وضع JSON للأدوات الخارجية — `--json`

```powershell
sad-check -r --json examples > report.json
```

البنية:
```jsonc
{
  "results": [
    {
      "file": "examples/01_مرحبا.ص",
      "clean": true,
      "readOk": true,
      "parseOk": true,
      "ownershipOk": true,
      "variables": 3,
      "borrows": 0,
      "moves": 0,
      "errors": []
    }
  ]
}
```

كل خطأ يحتوي: `variable`, `message` (EN), `messageAr` (AR), `suggestion`.

### 2.5 وضع المراقبة — `--watch` / `-w`

يعيد الفحص تلقائياً عند أي تعديل:

```powershell
sad-check -w examples\01_مرحبا.ص
```

```
(AR) وضع المراقبة نشط — اضغط Ctrl+C للخروج
(EN) watch mode active — press Ctrl+C to exit

✓ examples\01_مرحبا.ص: نظيف / clean  (vars=3 borrows=0 moves=0)

── إعادة فحص / re-check @ 14:32:18 ──
✓ examples\01_مرحبا.ص: نظيف / clean  (vars=3 borrows=0 moves=0)
```

**التنفيذ:** استطلاع `fs::last_write_time` كل 500 مللي ثانية. يعمل مع `--recursive` كذلك (يراقب جميع الملفات المُكتشَفة).

### 2.6 شرح إضافي — `--explain`

يضيف ملاحظة سياق تحت كل خطأ + إرشاد لتشغيل `sad-run` للحصول على موضع الخطأ بدقة في وقت التنفيذ.

### 2.7 تعطيل الألوان — `--no-color`

ANSI مفعَّل تلقائياً عندما `stdout` طرفية وليس `--json`. على ويندوز يتم تفعيل `ENABLE_VIRTUAL_TERMINAL_PROCESSING` تلقائياً.

```powershell
sad-check --no-color examples\01_مرحبا.ص
```

---

## 3. عبر الواجهة الموحَّدة (sad-hub)

كل أمر `sad-check ARGS` يعمل بنفس الطريقة عبر:

```powershell
sad check ARGS
```

الهب يمرّر argv مع الحفاظ الكامل على Unicode عبر `CreateProcessW` + `utf8ToWide`.

---

## 4. أمثلة CI

### 4.1 GitHub Actions — فشل البناء عند خطأ ملكية

```yaml
- name: Static ownership check
  run: |
    sad check -r --summary src
  shell: pwsh
```

`exit code != 0` يفشل الخطوة تلقائياً.

### 4.2 إنتاج تقرير JSON

```yaml
- name: Generate ownership report
  run: |
    sad check -r --json src > ownership-report.json
- uses: actions/upload-artifact@v4
  with:
    name: ownership-report
    path: ownership-report.json
```

### 4.3 Pre-commit hook (Git)

```sh
#!/bin/sh
# .git/hooks/pre-commit
files=$(git diff --cached --name-only --diff-filter=ACM | grep '\.ص$')
[ -z "$files" ] && exit 0
echo "$files" | xargs sad check
```

---

## 5. رموز الخروج

| الرمز | المعنى |
|---|---|
| `0` | فحص نظيف بالكامل |
| `1` | أخطاء ملكية أو parse على ملف واحد على الأقل |
| `2` | فشل قراءة ملف، أو لا توجد ملفات `.ص` للفحص |
| `3` | معاملات سطر أوامر خاطئة |

في الوضع التكراري `-r`: يُرجع **أعلى** رمز خروج بين جميع الملفات.

---

## 6. مقارنة سريعة مع `sad-run`

| الجانب | `sad-run` | `sad-check` |
|---|---|---|
| ينفّذ الكود؟ | نعم | لا |
| يفحص الملكية؟ | نعم (في وقت التشغيل) | نعم (ثابت، نفس المحرّك) |
| يطبع مخرج البرنامج؟ | نعم | لا |
| سرعة؟ | أبطأ (يشمل التنفيذ) | أسرع بكثير |
| مناسب لـCI؟ | للاختبارات الوظيفية | للفحص الستاتيك |

استخدم **كليهما**: `sad-check` كحارس بوابة سريع، و`sad-run`/الاختبارات الفعلية للسلوك.

---

## 7. الملفات ذات الصلة

- `tools/check/src/main.cpp` — تنفيذ الأداة
- `tools/check/CMakeLists.txt` — بناء `sad-check`
- `tools/hub/include/hub/utf8_args.h` — معالجة argv بـUTF-8 على ويندوز
- `shared/ownership/include/ownership/borrow_checker.h` — المحرّك المشترك
- `interpreter/managers/ownership_manager.cpp` — المغلِّف الذي يضمن التوحيد
- `docs/architecture-ownership-unification.md` — تفاصيل التوحيد المعماري
- `docs/architecture-tools-hub.md` — تصميم مركز الأدوات
