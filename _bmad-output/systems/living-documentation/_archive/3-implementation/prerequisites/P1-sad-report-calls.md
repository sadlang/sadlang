# Prerequisite P1: `sad --report-calls`

> **Type:** Compiler feature (not part of sadinfo epic)
> **Blocks:** S-013 (Tier 3 Snapshots)
> **Owner:** Compiler/Interpreter team
> **Architecture ref:** §3.3, §14 (Prerequisites)

---

## Goal

إضافة flag `--report-calls` للـ`sad.exe` (وممكن `sadc.exe`) يُخرج JSON بقائمة الاستدعاءات الفعلية في ملف `.ص` من خلال lexer/parser (لا regex، لا تنفيذ).

## Why

اكتشاف non-determinism في أمثلة lessons بدقة:
- regex على source code يعطي **false positives** (تعليق `# لا تستخدم الآن()`)
- regex يعطي **false negatives** (دالة وسيطة تستدعي `الآن()` داخلياً)

الحل الصحيح: تحليل lexer/parser للحصول على قائمة الاستدعاءات الفعلية + traversal للدوال المستدعاة.

## Acceptance Criteria

| AC | معيار |
|----|------|
| AC-1 | `sad --report-calls path.ص` يُرجع JSON على stdout |
| AC-2 | JSON يحوي `calls: [{name: "الآن", line: 5, col: 12}, ...]` |
| AC-3 | JSON يحوي `nondet_calls: [...]` (subset) عبر مطابقة مع `data/_schemas/non_deterministic_builtins.yaml` |
| AC-4 | Transitive: إذا الكود يستدعي `وقتي()` التي تستدعي `الآن()` → `الآن` يظهر في `nondet_calls` مع `via: ["وقتي"]` |
| AC-5 | Exit code 0 للـsuccess، 1 للـparse error |
| AC-6 | Flag `--report-calls-depth N` لتحديد عمق الـtransitive (افتراضي 3) |

## Output Format

```json
{
  "file": "example.ص",
  "calls": [
    {"name": "اطبع", "line": 3, "col": 5, "kind": "builtin"},
    {"name": "الآن", "line": 5, "col": 12, "kind": "builtin"}
  ],
  "nondet_calls": [
    {"name": "الآن", "line": 5, "col": 12, "via": []}
  ],
  "parse_errors": []
}
```

## Tasks

- [ ] Task 1: تعريف flag `--report-calls` في CLI args
- [ ] Task 2: إضافة Visitor جديد `CallReporterVisitor` يطوف على AST
- [ ] Task 3: قراءة `data/_schemas/non_deterministic_builtins.yaml` (إذا موجود)
- [ ] Task 4: Transitive traversal للدوال المستدعاة
- [ ] Task 5: JSON serializer للنتيجة
- [ ] Task 6: Unit tests على 5 سيناريوهات (مباشر، transitive، تعليق، parse error، مزيج)
- [ ] Task 7: Integration test يستدعي `sad.exe` فعلياً

## File List (متوقَّع)

- `interpreter_new/include/visitors/call_reporter_visitor.h` (جديد)
- `interpreter_new/src/visitors/call_reporter_visitor.cpp` (جديد)
- `interpreter_new/src/cli_args.cpp` (تعديل)
- `interpreter_new/src/main.cpp` (تعديل)
- `tests/interpreter/call_reporter_tests.cpp` (جديد)
- `data/_schemas/non_deterministic_builtins.yaml` (جديد، أو ضمن sadinfo POC)

## Definition of Done

- [ ] جميع AC مُحقَّقة
- [ ] 100% pass للـunit tests الجديدة
- [ ] لا تراجع في الـcomprehensive_tests
- [ ] دلتا الأداء على `sad.exe` < 5% (الـflag لا يُؤثِّر إلا عند الاستخدام)
- [ ] توثيق في `docs/SAD_LANGUAGE_COMPLETE_REFERENCE.md` تحت "أدوات الكومبايلر"

## Fallback Strategy

إذا تأخَّر P1، S-013 ينفِّذ Tier 3 مع warning:
```
[warning] sad --report-calls not available, using regex fallback (lower accuracy)
```
ولا يفشل البناء.
