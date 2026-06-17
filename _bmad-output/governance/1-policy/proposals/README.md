# المقترحات المعمارية (ADRs)

> **Architecture Decision Records** — قرارات معمارية موثَّقة بنمط Michael Nygard.

## متى يجب على الوكيل فتح ADR؟

الوكيل **يُوقف التنفيذ** ويفتح ADR قبل أي مهمة تستوفي **أحد** هذه الشروط:

1. **تأثير ≥ 3 ملفات في طبقات مختلفة** (مثال: AST + Parser + Interpreter).
2. **كسر API موجود** (تغيير توقيع دالة عامة، حذف عضو من Value، تغيير سلوك keyword).
3. **إضافة Dependency جديد** (مكتبة C++، خيار CMake، module جديد في stdlib).
4. **مخالفة قاعدة CW أو BF** بشكل واعٍ (مع تبرير).
5. **اقتراح حذف ميزة موجودة** (حذف opcode من SIR، حذف keyword، حذف زائر).

## الترقيم والاسم

`ADR-NNN-<kebab-case-title>.md`  
مثال: `ADR-001-git-permissions-for-agents.md`

ترقيم تسلسلي عبر كل المشروع. لا يُعاد التدوير.

## الحالات (Status)

| الحالة | المعنى |
|---|---|
| `proposed` | الوكيل كتب المقترح، ينتظر PM |
| `accepted` | PM وافق — الوكيل يُكمل التنفيذ بناءً على هذا القرار |
| `rejected` | PM رفض — الوكيل يبحث عن حل بديل |
| `superseded by ADR-MMM` | قرار جديد ألغى هذا |

## القالب

[../templates/ADR_TEMPLATE.md](../templates/ADR_TEMPLATE.md)

## القواعد الصارمة

1. **الوكيل يوقف التنفيذ** حال فتح ADR ويُحدّث `TASK-XXX.state.md` إلى `blocked: waiting-ADR-NNN`.
2. PM يردّ خلال ≤ 24 ساعة عمل (إن تأخر، الوكيل يبدأ مهمة أخرى من قائمته).
3. لا تعديل على ADR `accepted` — أي تغيير يحتاج ADR جديد يلغيه (`supersedes`).
4. كل ADR يُرفق رابطه في `TASK-XXX.md` تحت قسم "القرارات المعمارية".
