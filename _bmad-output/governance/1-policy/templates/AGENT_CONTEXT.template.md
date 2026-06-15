# سياق الوكلاء المشترك — قالب

> **هذا قالب فارغ.** انسخه إلى جذر المستودع باسم `AGENT_CONTEXT.md` واملأه قبل أوّل استخدام. لا تترك أيّ حقل بصيغته الأصلية.

| الحقل | القيمة |
|---|---|
| **التاريخ** | YYYY-MM-DD (يُؤخَذ من `Get-Date -Format "yyyy-MM-dd"` — لا تخمين) |
| **الإصدار** | v1.0 |
| **المالك** | صالح |
| **المُعرِّف** | AGENT_CONTEXT-001 |
| **الحد الأقصى** | 2000 رمز (محسوبة بـ `tiktoken`) |

> أُنشئ من قالب `AGENT_CONTEXT.template.md` بتاريخ YYYY-MM-DD.

---

## 1. ملخّص الحالة (≤ 300 كلمة)

اشرح في فقرة واحدة فقط: المرحلة النشطة من PMF (1/2/3/4)، آخر إنجاز كبير، أهم ملف يجب على الوكيل قراءته بعد هذا.

## 2. المخطّطات النشطة

- **PRD النشط:** [PRD.md](_bmad-output/governance/1-policy/PRD.md) — حالة: نشطة/مسوَّدة/مؤرشفة
- **ARCHITECTURE النشط:** [ARCHITECTURE.md](_bmad-output/governance/1-policy/ARCHITECTURE.md) — حالة: نشطة/مسوَّدة/مؤرشفة
- **السبرنت الحالي:** SPRINT-N (بدأ YYYY-MM-DD، ينتهي YYYY-MM-DD)
- **الستوريات النشطة:** اقرأ [AGENT_LOCK.json](_bmad-output/governance/1-policy/AGENT_LOCK.json) للقائمة الحيّة.

## 3. القرارات المحجوزة (لا تُعَد فتحها)

| القرار | المرجع | تاريخ الإغلاق |
|---|---|---|
| مثال: اعتماد LLVM 18 backend | ADR-XXX | YYYY-MM-DD |

## 4. قائمة «لا تفعل» (Do Not List)

- لا تعدّل ملفات `_bmad-output/governance/1-policy/*` بدون مراجعة صالح (`.github/CODEOWNERS` يفرض ذلك).
- لا تفترض تاريخاً من ذاكرتك — استخدم `Get-Date` دائماً.
- لا تحجز ستوري لأكثر من 4 ساعات بدون تجديد.
- لا تكتب إنجليزياً في وثائق الحوكمة — استخدم الترجمة العربية للمصطلحات التقنية.
- (أضف القيود الخاصة بمشروعك هنا)

## 5. مفاتيح الاختصارات

| الاختصار | المعنى |
|---|---|
| PMF | الإطار الحاكم (Project Management Framework) — انظر [PROJECT_MANAGEMENT_FRAMEWORK.md](_bmad-output/governance/1-policy/PROJECT_MANAGEMENT_FRAMEWORK.md) |
| SoT | المصدر الموثوق الوحيد (Single Source of Truth) |
| DoD | تعريف النهاية (Definition of Done) |
| BMAD | منظومة المهارات المُعتمدة — انظر `.github/skills/` |
