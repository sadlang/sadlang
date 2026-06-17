# 📦 أرشيف: توحيد نظام الحوكمة — 2026-05-30

> **سياق تاريخي:** قبل 2026-05-30، كانت الحوكمة موزَّعة على **ثلاثة مجلدات منفصلة**
> في جذر `_bmad-output/`:
>
> - `agents/` — مواثيق الوكلاء الخمسة
> - `codeRolePlan/` — خطة Contract-as-Code
> - `management/` — إطار PMF + التنفيذ
>
> في 2026-05-30 تم **توحيد الثلاثة** تحت [`_bmad-output/governance/`](../../governance/)
> كنظام واحد بثلاث طبقات (Policy / Agents / Code Contract).

---

## الملفات في هذا الأرشيف

| الملف | الغرض |
|---|---|
| [COMPARISON_agents_codeRolePlan_management.md](COMPARISON_agents_codeRolePlan_management.md) | تحليل تفصيلي يجيب على سؤال "هل المجلدات الثلاثة مكرَّرة؟" → لا، كل واحد طبقة مختلفة. يحوي 4 نقاط تداخل مشروعة + نسب التنفيذ الفعلي مع المراجع. |
| [REORGANIZATION_PROPOSAL.md](REORGANIZATION_PROPOSAL.md) | مقترح المهاجرة بخطواته الأربع: إنشاء `governance/`، نقل الملفات، استبدال الروابط، حذف المجلدات القديمة لسلة المهملات. |

---

## لماذا الإبقاء عليهما؟

1. **توثيق القرار:** كل إعادة هيكلة جذرية يجب أن تحتفظ بسبب اتخاذها لاسترشاد القرارات المستقبلية.
2. **مراجع لـ ARCHITECTURE.md:** [governance/ARCHITECTURE.md §5](../../governance/ARCHITECTURE.md) يستند إلى الـ Reality Check الوارد في `COMPARISON`.
3. **نمط قابل للتكرار:** صيغة المهاجرة الآمنة (`Recycle Bin` + pre-check + استبدال جماعي) صالحة لأي إعادة هيكلة مستقبلية مماثلة.

---

## كيف نُفِّذت المهاجرة (ملخص)

| الخطوة | الإجراء | النتيجة |
|---|---|---|
| 1 | إنشاء `governance/{1-policy, 2-agents, 3-code-contract}/` + README + ARCHITECTURE | ✅ |
| 2 | `Move-Item` للملفات (لم تكن متتبَّعة بـ git، فلا حاجة لـ `git mv`) + إعادة تنظيم داخلي (`critiques/`, `runtime-state/`) | ✅ 28 ملف منقول |
| 3 | استبدال جماعي للروابط في 30 ملف (داخل + خارج `_bmad-output/`) | ✅ 0 إشارات متبقية |
| 4 | فحص الفراغ ثم `Microsoft.VisualBasic.FileIO.FileSystem.DeleteDirectory(..., SendToRecycleBin)` | ✅ 3 مجلدات في سلة المهملات (قابلة للاسترداد) |

---

## للاسترداد إن لزم

افتح Recycle Bin في Windows واسترد:

- `_bmad-output/agents/`
- `_bmad-output/codeRolePlan/`
- `_bmad-output/management/`

ثم اعكس الاستبدالات (الجدول في [REORGANIZATION_PROPOSAL.md](REORGANIZATION_PROPOSAL.md) §4.3).

---

**المؤلف:** Amelia (bmad-agent-dev)
**التاريخ:** 2026-05-30
