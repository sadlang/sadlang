# Story EM-CPP-4 — ترحيل basics + async + exceptions

**Epic**: [EPIC-EM-CPP](../epics/EPIC-EM-CPP-MIGRATION.md) | **النقاط**: 5 | **الأولوية**: متوسطة | **الحالة**: محجوبة (تبعية EM-CPP-1)
**التبعيات**: EM-CPP-0 + EM-CPP-1 | **المُسنَد إلى**: TBD

---

## الهدف
ترحيل **34 موقع رمي خام**:
| الملف | المواقع |
|------|:------:|
| `builtin_module_basics.cpp` | 19 |
| `builtin_module_async.cpp` | 14 |
| `builtin_module_exceptions.cpp` | 1 |

## ملاحظة خاصة
- **basics:** عمليات ملفات/خروج/مدى → رموز `IO_*`/`RUN_*`.
- **async:** أخطاء تزامن (قنوات/أقفال) → `RUN_CONCURRENCY_*`/`RUN_CHANNEL_OPERATION_FAILED`.

## التنفيذ
- [ ] جرد + تعيين `ErrorCode` (موجود/ناقص→YAML) لكل ملف.
- [ ] استبدال بـ`ctx.error` (التوقيع الجديد BuiltinContext) + بناء + اختبار + `grep = 0`.

## معايير القبول
- **0 رمي خام** في الثلاثة · رسائل من الكتالوج · بناء أخضر · لا تراجع.

## تعريف "تم"
الملفات الثلاثة مُرحَّلة (34→0) + اختبارات تمرّ.
