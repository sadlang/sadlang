# Story EM-CPP-7 — كنس الرمي الخام غير المضمّن

**Epic**: [EPIC-EM-CPP](../epics/EPIC-EM-CPP-MIGRATION.md) | **النقاط**: 3 | **الأولوية**: متوسطة | **الحالة**: جاهزة (مستقلة)
**التبعيات**: لا شيء (هذه الطبقات تملك مواقع — ترحيل مباشر بلا `BuiltinContext`) | **المُسنَد إلى**: TBD

---

## الهدف
ترحيل **23 موقع رمي خام** في طبقات تملك `location`/`Position` بالفعل — ترحيل **مباشر** عبر
`throwRuntime`/`reportFromCatalog` (بلا جسر BuiltinError).

## النطاق (أدلة grep)
| الطبقة | المواقع | الواجهة المستهدفة |
|--------|:------:|-------------------|
| lexer (`shared/lexer`) | 4 | `reportFromCatalog(LEX_*, location, ctx)` |
| parser (`shared/parser`) | 3 | `reportFromCatalog(SYN_*, location, ctx)` |
| interpreter visitors | 6 | `throwRuntime(RUN_*/SEM_*, pos, ctx)` |
| compiler (`compiler/src`) | 10 | `reportFromCatalog(code, location, ctx)` |

## التنفيذ
- [ ] لكل موقع: حدّد `ErrorCode` (موجود/ناقص→YAML) + الواجهة حسب الطبقة (الدليل §5).
- [ ] استبدال مباشر + بناء + اختبار + `grep throw std::runtime_error = 0` لكل طبقة.

## معايير القبول
- **0 رمي خام** في الطبقات الأربع · رسائل من الكتالوج · بناء أخضر (sad + sadc) · لا تراجع.

## تعريف "تم"
الطبقات الأربع مُرحَّلة (23→0). **يمكن تنفيذها بالتوازي مع EM-CPP-0** (مستقلة عن الجسر).
