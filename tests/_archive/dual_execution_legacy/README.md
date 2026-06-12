# أرشيف dual_execution القديم (NOT-A-SoT — للرجوع فقط)

> **أُرشف في:** 2026-06-12 — ضمن قصة TEST-003 (ترحيل السلوك إلى `tests/behavior/`)
> راجع: `_bmad-output/systems/testing-system/stories/TEST-003-migrate-behavior.md`

## أين ذهب المحتوى الحي؟

| كان هنا | الموقع الجديد |
|---|---|
| `features/01..12_*` (452 ملفاً) | `tests/behavior/sections/01..12_*` (نفس الترقيم — AC-01) |
| `P0_smoke/` (5 ملفات) | `tests/behavior/P0_smoke/` |
| `runner.py` + `config.yaml` | `tests/` (الجذر — منذ TEST-002) |
| `rules_matrix/` | `tests/behavior/rules_matrix/` (منذ TEST-002) |

## ما الذي بقي هنا ولماذا؟

المجلدات `core/ oop/ match/ advanced/ concurrency/ errors/ negative/ stdlib/`
(**380 ملف .ص**) هي **التنظيم القديم** الذي سبق إعادة هيكلة `features/`. معظمها
مكرر وظيفياً في الأقسام الجديدة، وبعضها فريد لم يُدمَج بعد.

- **القرار المعلّق:** الدمج الانتقائي للفريد منها في `behavior/sections/` يقع ضمن
  **TEST-005** (دمج + أرشفة). لا تُحرَّر هذه الملفات — أي اختبار جديد يُكتب في
  `tests/behavior/` حصراً.
- `TESTING_GUIDE.md`: دليل النظام القديم — حلّ محله `tests/behavior/README.md`.
  (أما `report_html.py` فأداة حيّة يستوردها الراننر — نُقلت إلى `tests/report_html.py`.)

> قاعدة GR-04: أرشفة لا حذف. هذا المجلد خارج نطاق مستويات `config.yaml` ولا
> يشغّله CI.
