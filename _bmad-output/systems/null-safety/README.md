# نظام أمان null (Null Safety) — لغة ص

> نظام **مستقلّ متقاطع** يبني على [نظام الأنواع](../type-system/README.md) (تمثيل `Null`/`T؟`/`Result`)
> ويتكامل مع [سياسة الذاكرة](../../../shared/memory_policy/) عبر `dispatch()` (الأعلام الثلاثة).
> أُنشئ: 2026-06-13 (بنية الستة GR-05).

## لماذا نظام مستقلّ (لا جزء من نظام الأنواع)؟

- **نظام الأنواع** يملك **التمثيل**: `SadTypeKind::Null` (P1)، `SadOptionalType`/`T؟` (P4)، `SadResultType` (P3)، `isAssignableTo` (`Null <: T?`). ✅ مُنجَز.
- **نظام أمان null** يملك **التحليل والإنفاذ**: تحليل تدفّق حسّاس (smart narrowing)، رصد الوصول غير الآمن، عامل التأكيد `!!`، والتكامل مع سياسة الذاكرة (`--gc`/`--learn`/`--prod` → تجاهل/تحذير/قاتل).
- **القرينة:** فاحص الملكية (`shared/ownership/`) نظام تحليل مستقلّ يُستدعى من المحرّكين — وأمان null شقيقه (flow-sensitive analysis)، لا تمثيل أنواع.
- **عائلة اللغات:** Kotlin/TypeScript/Dart (التمثيل في الأنواع، التدفّق تحليل منفصل).

## المعمارية باختصار

`NullSafetyAnalyzer` مكوّن مشترك (`shared/null_safety/` أو ضمن `shared/ownership/`) يمرّ على الـAST:
1. يتتبّع قابلية null لكل متغيّر (`T؟`↔`T`) بتحليل تدفّق (تضييق بعد `إذا (س != لاشيء)`، تراجع عند الخروج).
2. يرصد: إسناد `لاشيء` لغير-اختياري، وصول لعضو على `T؟` دون تضييق/`؟.`.
3. يوجّه كل تشخيص عبر `shared/errors/builders/dispatch.h` → الشدّة بالسياسة (`--gc`=تجاهل، `--learn`=تحذير، `--prod`=قاتل).
4. يُستدعى من **المفسّر** (`interpreter_core`) و**المترجم** (`compiler_driver_analysis`) — مصدر حقيقة واحد.

## البنية الموحَّدة

- `planning/` — ARCHITECTURE + ROADMAP + PRD.
- `stories/` — STORY-NS-*.
- `decisions/` — ADRs (يبني على [ADR-TYPESYSTEM-001](../type-system/decisions/ADR-TYPESYSTEM-001-NULL-SAFETY.md)).
- `status/` — implementation_status.md.

## الحالة

مُخطَّط (لم يبدأ التنفيذ). الأساس (تمثيل الأنواع + فرض P9 الابتدائي في المفسّر + عوامل `?.`/`??`/`؟`) جاهز في نظام الأنواع.
