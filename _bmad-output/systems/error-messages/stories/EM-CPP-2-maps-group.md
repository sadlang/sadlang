# Story EM-CPP-2 — ترحيل مجموعة maps

**Epic**: [EPIC-EM-CPP](../epics/EPIC-EM-CPP-MIGRATION.md) | **النقاط**: 8 | **الأولوية**: عالية | **الحالة**: محجوبة (تبعية EM-CPP-1)
**التبعيات**: EM-CPP-0 (الجسر) + EM-CPP-1 (النمط المُثبَت) | **المُسنَد إلى**: TBD

---

## الهدف
ترحيل وحدات maps إلى التوقيع الجديد `(BuiltinContext&)` واستبدال **97 موقع رمي خام** بـ
`ctx.error(ErrorCode, placeholders)`.

## النطاق (أدلة grep)
| الملف | المواقع |
|------|:------:|
| `builtin_module_maps_arrays.cpp` | 24 |
| `builtin_module_maps_core.cpp` | 23 |
| `builtin_module_maps_text.cpp` | 20 |
| `builtin_module_maps_json_xml.cpp` | 17 |
| `builtin_module_maps_utils.cpp` | 13 |

## التنفيذ (لكل ملف، حسب نمط EM-CPP-1)
- [ ] جرد المواقع → `ErrorCode` مناسب لكل (موجود) أو رمز ناقص يُضاف لـ YAML (الدليل §6).
- [ ] استبدال `throw std::runtime_error` بـ`ctx.error(ErrorCode::X, {{...}})`.
- [ ] البناء + اختبار سلوكي لكل وحدة + `grep throw std::runtime_error = 0`.

## معايير القبول
- **0 رمي خام** في الملفات الخمسة · رسائل maps من الكتالوج · بناء أخضر · لا تراجع.

## تعريف "تم"
maps مُرحَّلة بالكامل (97→0) + اختبارات سلوكية تمرّ.
