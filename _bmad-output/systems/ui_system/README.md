---
title: "نظام الواجهات والرسومات (SadUI) — مجلد النظام"
date: 2026-06-08
status: ACTIVE
author: Amelia (bmad-agent-dev)
governanceAck: "قرأت 1-policy؛ آخر تحقق VERIFICATION_REPORT_2026-05-30؛ السبرنت SPRINT_CURRENT"
---

# نظام الواجهات والرسومات (SadUI)

يتبع هذا المجلد البنية الموحّدة الستة (GR-05): `planning/ epics/ stories/ sprints/ status/ decisions/`.

## الوثائق المعتمدة
- [`planning/STRATEGY.md`](planning/STRATEGY.md) — **الاستراتيجية** (لماذا، الطبقات، المستهلكون، التكامل) بمخططات.
- [`planning/ROADMAP.md`](planning/ROADMAP.md) — **خارطة الطريق الموحّدة** (المراحل، التسلسل، التبعيات).
- [`planning/SADUI_SYSTEM_REPORT.md`](planning/SADUI_SYSTEM_REPORT.md) — تقرير النظام (ماذا يدعم، كيف يعمل، كيف نكتب).
- [`status/implementation_status.md`](status/implementation_status.md) — حالة التنفيذ مع نتائج التحقق الوظيفي (P0).
- [`status/platform_support_matrix.md`](status/platform_support_matrix.md) — مصفوفة دعم العناصر الـ15 عبر المنصات الأربع.
- [`decisions/`](decisions/) — قرارات ADR-UI-01/02/07.
- [`epics/BACKLOG.md`](epics/BACKLOG.md) — الملاحم والخطط الفرعية ([EPIC-UI-W15](epics/EPIC-UI-W15-interpreter-compiler.md)).
- [`stories/`](stories/) — ستوريات قابلة للتنفيذ (STORY-UI-W15-01..04).
- التوثيق الكامل بالمخططات الرسومية: [`planning/docs/نظام_الواجهات_الرسومي_ص.md`](planning/docs/نظام_الواجهات_الرسومي_ص.md).
- الأنظمة الداخلية للنواة بمخططات: [`planning/docs/الأنظمة_الداخلية_لنظام_الواجهات.md`](planning/docs/الأنظمة_الداخلية_لنظام_الواجهات.md).
- الباطن لكل منصة بمخططات: [`planning/docs/الباطن_لكل_منصة.md`](planning/docs/الباطن_لكل_منصة.md).
- دليل العناصر الأولية الـ15 (مرجع): [`planning/docs/دليل_العناصر_الـ15.md`](planning/docs/دليل_العناصر_الـ15.md).

## ملاحظات
- **المصدر الموثوق للصيغة:** المحلل النحوي `shared/parser/src/ui/parser_ui.cpp` — لا أمثلة `examples/` القديمة.
- التوثيق القديم (`docs/تقرير_نظام_الواجهات_SadUI.md`, `docs/تصميم_نظام_الواجهات_v2.md`) حُذف لاحتوائه صيغة قديمة (`صنف...يرث حالة_عنصر` / أقواس `{}`) لا يقبلها المحلل، واستُبدل بالتوثيق الجديد الدقيق.
- المثال العامل بالصيغة المعتمدة: `tests/integration/test_event_system.ص`.
- التزاماً بـ GR-01: لا نِسَب إنجاز بلا دليل من الكود.
