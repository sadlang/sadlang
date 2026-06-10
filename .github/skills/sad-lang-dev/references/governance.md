# نظام الحوكمة الإلزامي (BMAD) — ما يجب أن يعرفه كل مطوّر

> هذا أعلى أولوية في المشروع. تجاهله = خرق مباشر للسياسة، ويحقّ للمستخدم رفض كل مخرجاتك.
> المصدر الكامل: `.github/copilot-instructions.md` (أعلى الملف). هذا الملف ملخّص تشغيلي.

## 1. متى تنطبق الحوكمة؟ (حدّد بدقّة)

| تعمل في... | الحوكمة؟ |
|-----------|----------|
| `_bmad-output/` (سياسات، أنظمة، ستوريات، حالة، قرارات) | ✅ **نعم — إلزامية كاملة** (اقرأ السياسة + سطر الإقرار) |
| كود اللغة فقط (`interpreter/`, `compiler/`, `shared/`, `language-truth/`) دون مهمة محكومة | قواعد الكود `CW-*`/`BF-*` تكفي |
| كود اللغة **ضمن ستوري/سبرنت محكوم** (مثل أنظمة `builtin-functions` أو `error-messages`) | ✅ تتبّع الستوري وحدّث حالتها |

> ⚠️ **تشابك مهم:** أنظمة `_bmad-output/systems/builtin-functions/` و `error-messages/` و
> `living-documentation/` **محكومة**. فإن كان عملك على الدوال المضمنة أو الأخطاء جزءاً من
> ستوري، فالتغيير الكودي + تحديث حالة الستوري **كلاهما مطلوب** (راجع [./interconnected-systems.md](./interconnected-systems.md)).

## 2. الملفات الإلزامية للقراءة (بالترتيب) قبل أي عمل في `_bmad-output/`

1. السياسة الأم: `_bmad-output/governance/1-policy/planning/PRD.md`
2. إطار إدارة المشروع: `_bmad-output/governance/1-policy/planning/PROJECT_MANAGEMENT_FRAMEWORK.md`
3. آخر تقرير تحقق (مصدر حقيقة الحالة): أحدث `_bmad-output/governance/1-policy/status/VERIFICATION_REPORT_<date>.md`
4. السبرنت الحالي: `_bmad-output/governance/1-policy/sprints/SPRINT_CURRENT.md`
5. عقد الكود: `_bmad-output/governance/3-code-contract/planning/prd.md`

## 3. سطر الإقرار (إلزامي في أول رد يَمَس `_bmad-output/`)

> «قرأت السياسة في `_bmad-output/governance/1-policy/`؛ آخر تقرير تحقق:
> `VERIFICATION_REPORT_<YYYY-MM-DD>.md`؛ السبرنت الحالي: `<اسم>`.»

بدون هذا السطر = لم تلتزم بالحوكمة.

## 4. البنية الموحَّدة الستة (لكل نظام تحت `governance/*` و `systems/*`)

```
<area>/
├── planning/    ← PRD, ARCHITECTURE, ROADMAP, مواصفات
├── epics/       ← BACKLOG.md + EPIC-XXX-*.md
├── stories/     ← STORY-*.md
├── sprints/     ← SPRINT_CURRENT.md + SPRINT_<date>_RETRO.md
├── status/      ← implementation_status.md + VERIFICATION_REPORT_<date>.md
├── decisions/   ← ADR-XXX-*.md
└── README.md
```

> استثناء `living-documentation/`: يتبع بنية خاصة (3 وثائق قانونية بالجذر: `STRATEGY.md`,
> `ARCHITECTURE.md`, `IMPLEMENTATION_PLAN.md`). راجع copilot-instructions.

## 5. القواعد الذهبية (GR) — لا تُخالَف

- **GR-01:** لا ادّعاء نسبة إنجاز بلا دليل من الكود الفعلي (grep/build/list).
- **GR-02:** لا تُحذف ADRs أبداً — المُلغى يُعلَّم `status: Superseded` + `supersededBy`.
- **GR-03:** السبرنت لا ينتهي بلا `RETRO`.
- **GR-04:** كل ملف زائف (يدّعي إنجازاً غير موجود) يُعلَّم `status: OUT-OF-DATE` فوراً.
- **GR-05:** قبل إنشاء نظام جديد، انسخ `_bmad-output/systems/_TEMPLATE/` بنفس بنية الستة.
- **GR-06:** التواريخ من الجهاز فقط — `Get-Date -Format "yyyy-MM-dd"` قبل أي timestamp.

## 6. خلاصة عملية للمطوّر

1. **هل مهمتي تَمَس `_bmad-output/`؟** إن لا → التزم `CW-*`/`BF-*` وتابع.
2. إن نعم → اقرأ الملفات الخمسة، اكتب سطر الإقرار، اتبع البنية الستة وقواعد GR.
3. **هل عملي الكودي جزء من ستوري محكوم؟** إن نعم → حدّث `status/` بدليل فعلي بعد الإنجاز (GR-01).
4. عند الشك، اقرأ `copilot-instructions.md` (قسم الحوكمة) — هو المرجع الكامل.
