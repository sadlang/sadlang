# 🤖 الوكلاء — خطة التوازي v1.3

## النموذج: 5 وكلاء بنطاقات معزولة

| الوكيل | الاسم | النطاق | الحجم | WIP |
|---|---|---|---|---|
| α | [Alpha](agent_alpha.md) | Frontend (`shared/`) | 64K loc | 2 |
| β | [Beta](agent_beta.md) | Runtime (`interpreter/`+`vm/`+`runtime/`) | 66K loc, 171 ملف | 3 |
| γ | [Gamma](agent_gamma.md) | Compiler (`compiler/`) | 127K loc, 379 ملف | 2 |
| δ | [Delta](agent_delta.md) | Ecosystem (`stdlib/`+`tools/`+`tests/`) | 219K loc | 4 |
| ε | [Epsilon](agent_epsilon.md) | Domain Libs (`graphics/`+`sad_ui/`+`network/`+`platform/`+`cmake/`) | ~235K loc | 3 |

## وثائق الحوكمة المرجعية

- **[الملفات المحروسة](../../docs/governance/GUARDED_FILES.md)** — قائمة 10 ملفات/مجموعات تحتاج موافقة Saleh
- **[BACKLOG](../management/BACKLOG.md)** — المصدر الوحيد للستوريات
- **[copilot-instructions](../../.github/copilot-instructions.md)** — العقد الأساسي للوكلاء

## ربط BACKLOG بالوكلاء

| Story | Lead | Follow | حالة |
|---|---|---|---|
| B-001 GPG | **Saleh** | — | PENDING_SALEH (محظور للوكلاء) |
| B-002 Deputy | **Saleh** | — | PENDING_SALEH (محظور للوكلاء) |
| B-003 Union Types | α | γ, β | جاهزة للتكليف |
| B-004 TypePtr Inference | γ | — | جاهزة |
| B-005 Template Constraints | α | γ | جاهزة |
| B-006 Dispatch Unification | β | γ | جاهزة **[GUARDED:G-04]** |
| B-007 stdlib → ABI | δ | β | جاهزة |
| B-008 Architecture Refactor #4 | α | — | جاهزة |
| B-009 LSP تحسين | δ | — | جاهزة |
| B-010 sad pkg | δ | — | جاهزة |
| B-011 sad_ui rendering | ε | — | جاهزة |
| B-012 network bugs | ε | — | جاهزة |
| B-013 توسعة اختبارات | δ | — | جاهزة |

## بروتوكول التشغيل

1. كل وكيل يقرأ ميثاقه أولاً
2. ثم `docs/governance/GUARDED_FILES.md`
3. ثم `BACKLOG.md` لأخذ ستوري مكلَّفة له
4. لـ Lead-Follow: Lead يكتب RFC في `_bmad-output/rfcs/B-XXX-rfc.md`
5. تنفيذ متوازي ضمن حدود WIP
