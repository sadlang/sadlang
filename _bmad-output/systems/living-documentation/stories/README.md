# 📋 ستوريات نظام التوثيق الحي (Living Documentation)

> هذه الستوريات تُنفِّذ [IMPLEMENTATION_PLAN.md](../IMPLEMENTATION_PLAN.md) V5.1 — 26 ستوري عبر 4 مراحل.
> كل ستوري قابل للتطوير المستقل في sprint، ويُغلَق فقط ببناء أخضر + اختبارات خضراء.

## البنية

```
stories/
└── implementation/
    ├── M0-Foundation/      ← 4 ستوريات (بنية + جسر)
    ├── M1-DataPopulation/  ← 14 ستوري (ملء النطاقات)
    ├── M2-Library/         ← 4 ستوريات (Wrapper + توحيد)
    └── M3-Quality/         ← 4 ستوريات (اختبارات Truth ↔ اللغة)
```

## فهرس الستوريات

### M0 — Foundation
| ID | العنوان | الاعتماد |
|---|---|---|
| [S-V5-M0-001](implementation/M0-Foundation/STORY-V5-M0-001.md) | إنشاء بنية `language-truth/` | — |
| [S-V5-M0-002](implementation/M0-Foundation/STORY-V5-M0-002.md) | جسر `keywords.yaml` | M0-001 |
| [S-V5-M0-003](implementation/M0-Foundation/STORY-V5-M0-003.md) | JSON Schemas الأساسية | M0-001 |
| [S-V5-M0-004](implementation/M0-Foundation/STORY-V5-M0-004.md) | اختبار T1 المبدئي | M0-003 |

### M1 — Data Population
| ID | النطاق | Codegen | الاعتماد |
|---|---|---|---|
| [S-V5-M1-001](implementation/M1-DataPopulation/STORY-V5-M1-001.md) | keywords | ✅ موجود | M0-004 |
| [S-V5-M1-002](implementation/M1-DataPopulation/STORY-V5-M1-002.md) | builtins | ⚠️ تحديث | M1-001 |
| [S-V5-M1-003](implementation/M1-DataPopulation/STORY-V5-M1-003.md) | type_methods ⭐ | ✅ موجود | M1-001 |
| [S-V5-M1-004](implementation/M1-DataPopulation/STORY-V5-M1-004.md) | modules ⭐ | ✅ موجود | M1-001 |
| [S-V5-M1-005](implementation/M1-DataPopulation/STORY-V5-M1-005.md) | errors | ⚠️ تحديث | M1-001 |
| [S-V5-M1-006](implementation/M1-DataPopulation/STORY-V5-M1-006.md) | operators | ❌ جديد | M1-001 |
| [S-V5-M1-007](implementation/M1-DataPopulation/STORY-V5-M1-007.md) | directives | ❌ جديد | M1-001 |
| [S-V5-M1-008](implementation/M1-DataPopulation/STORY-V5-M1-008.md) | types | ❌ جديد | M1-001 |
| [S-V5-M1-009](implementation/M1-DataPopulation/STORY-V5-M1-009.md) | patterns ⭐ | ❌ جديد | M1-001 |
| [S-V5-M1-010](implementation/M1-DataPopulation/STORY-V5-M1-010.md) | grammar_constructs ⭐ | ❌ جديد | M1-001 |
| [S-V5-M1-011](implementation/M1-DataPopulation/STORY-V5-M1-011.md) | stdlib | ❌ جديد | M1-001 |
| [S-V5-M1-012](implementation/M1-DataPopulation/STORY-V5-M1-012.md) | learning content ⭐ | ⚠️ تحديث | M1-001 |
| [S-V5-M1-013](implementation/M1-DataPopulation/STORY-V5-M1-013.md) | oop_constructs ⭐ | ❌ جديد | M1-001 |
| [S-V5-M1-014](implementation/M1-DataPopulation/STORY-V5-M1-014.md) | expr_constructs ⭐ | ❌ جديد | M1-001 |

### M2 — Library
| ID | العنوان | الاعتماد |
|---|---|---|
| [S-V5-M2-001](implementation/M2-Library/STORY-V5-M2-001.md) | تصميم `libsadlangtruth` API | M1-014 |
| [S-V5-M2-002](implementation/M2-Library/STORY-V5-M2-002.md) | تنفيذ Wrapper | M2-001 |
| [S-V5-M2-003](implementation/M2-Library/STORY-V5-M2-003.md) | توسعة `codegen.cmake` | M2-002 |
| [S-V5-M2-004](implementation/M2-Library/STORY-V5-M2-004.md) | حذف `data/language/` ⚠️ | M2-003 |

### M3 — Quality
| ID | العنوان | الاعتماد |
|---|---|---|
| [S-V5-M3-001](implementation/M3-Quality/STORY-V5-M3-001.md) | T1 Schema Validation | M2-004 |
| [S-V5-M3-002](implementation/M3-Quality/STORY-V5-M3-002.md) | T2 Language Match | M3-001 |
| [S-V5-M3-003](implementation/M3-Quality/STORY-V5-M3-003.md) | T3 + T4 Completeness/Suggestions | M3-002 |
| [S-V5-M3-004](implementation/M3-Quality/STORY-V5-M3-004.md) | T5 Doc Channels Coverage | M3-003 |

---

**جميع الستوريات تتبع تعريف "تم" في [IMPLEMENTATION_PLAN.md §7](../IMPLEMENTATION_PLAN.md).**
